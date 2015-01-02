#include "Client.hpp"

#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonDocument>
#include <QEventLoop>
#include <QtConcurrent/QtConcurrent>

using namespace BitCoindRPC;

Client::Client(QString host, int port, QString user, QString password, QString account, QNetworkAccessManager * manager)
    : _host(host)
    , _port(port)
    , _user(user)
    , _password(password)
    , _account(account)
    , _ownsNetworkManager(manager == NULL) {

    // Create request
    QUrl url;
    url.setScheme("http");
    url.setHost(host);
    url.setPort(port);
    url.setUserName(user);
    url.setPassword(password);

    if(_ownsNetworkManager)
        _manager = new QNetworkAccessManager();

    _request = QNetworkRequest(url);

    // Set request content type
    _request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json-rpc");
}

Client::~Client(){

    // Delete network manager if we allocated one
    if(_ownsNetworkManager)
        delete _manager;
}

QJsonValue Client::rpc(const QString & method, const QJsonArray & parameters) {

    // Create RPC payload: http://json-rpc.org/wiki/specification
    // Bitcoind client: https://github.com/bitcoin/bitcoin/blob/master/src/rpcprotocol.cpp
    QJsonObject RPCJson;

    RPCJson["jsonrpc"] = "1.0";
    RPCJson["method"] = method;
    RPCJson["params"] = parameters;
    //RPCJson["id"] = method + ":" + QString::number(futureId); // use id to recover correct processor and future object
    RPCJson["id"] = method; // can be anything, we dont use it in reply

    // Turn into byte array
    QByteArray payload = QJsonDocument(RPCJson).toJson();

    // Use a direct connection if we are on manager thread, otherwise use a blocking connection
    Qt::ConnectionType type;

    if(QThread::currentThread() == _manager.thread())
        type = Qt::DirectConnection;
    else
        type = Qt::BlockingQueuedConnection;

    // Get return value
    QNetworkReply * reply;

    QMetaObject::invokeMethod(this,
                              "blockingPostOnManagerThread",
                              type,
                              Q_RETURN_ARG(QNetworkReply*, reply),
                              Q_ARG(QByteArray, payload));

    // Get response data
    QByteArray response = reply->readAll();

    // If there was an error, throw exception
    QNetworkReply::NetworkError e = reply->error();
    if(e != QNetworkReply::NoError)
        throw std::exception("Network request error: " + response);

    // Dispose of reply on its own thread,
    // do not use after this point!
    reply->deleteLater();

    // Parse into json
    QJsonDocument jsonResponse = QJsonDocument::fromJson(response);
    QJsonObject jsonObject = jsonResponse.object();

    // Get error
    QJsonValue error = jsonObject["error"];

    // Return result key if there was no error, otherwise throw exception
    if(error.isNull())
        return jsonObject["result"];
    else {

        // Create error string
        QString errorString = "RPC request error: " + error.toString();

        // Throw exception with given error
        throw std::exception(errorString.toLatin1());
    }
}

QNetworkReply * Client::blockingPostOnManagerThread(const QByteArray & payload) {

    qDebug() << "Do we ever get here!!!!";

    // Temporary event loop on stack
    QEventLoop eventLoop;

    // Submit POST request
    QNetworkReply * reply = _manager.post(_request, payload);

    // Stop event loop when reply is finished
    QObject::connect(reply, SIGNAL(finished()),
                     &eventLoop, SLOT(quit()));

    // Enter event loop until reply is finished
    eventLoop.exec();

    // Return reply
    return reply;
}

QFuture<uint> Client::getBlockCount() {
    return QtConcurrent::run(this, &BitCoindRPC::Client::getBlockCountBlocking);
}

uint Client::getBlockCountBlocking() {

    // Make blocking RPC call
    QJsonValue result = rpc("getblockcount", QJsonArray());

    // Return value
    return result.toInt();
}

QFuture<double> Client::getBalance(int minconf) {
    return QtConcurrent::run(this, &BitCoindRPC::Client::getBalanceBlocking, minconf);
}

double Client::getBalanceBlocking(int minconf) {

    // Create RPC parameters
    QJsonArray parameters;
    parameters.append(_account);
    parameters.append(minconf);

    // Make blocking RPC call
    QJsonValue result = rpc("getbalance", parameters);

    // Return value
    return result.toDouble();
}

QFuture<QMap<QString, double>> Client::listAccounts(int minconf) {
    return QtConcurrent::run(this, &BitCoindRPC::Client::listAccountsBlocking, minconf);
}

QMap<QString, double> Client::listAccountsBlocking(int minconf) {

    // Create RPC parameters
    QJsonArray parameters;
    parameters.append(minconf);

    // Make blocking RPC call
    QJsonValue result = rpc("listaccounts", parameters);

    // Balances
    QMap<QString, double> accountBalances;

    // Get map
    const QJsonObject resultObject = result.toObject();

    qDebug() << "listaccounts =";

    for(QJsonObject::const_iterator i = resultObject.constBegin(),
            end(resultObject.constEnd()); i != end;i++) {

        QString name = i.key();
        double balance = i.value().toDouble();

        // Save in map
        accountBalances[name] = balance;

        qDebug() << name << "->" << balance;
    }

    return accountBalances;
}
