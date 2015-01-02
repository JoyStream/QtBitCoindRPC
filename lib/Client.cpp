#include "Client.hpp"

#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

using namespace BitCoindRPC;

Client::Client(QString host, int port, QString user, QString password, QString account, QNetworkAccessManager & manager)
    :
    //_host(host)
    //, _port(port)
    //, _user(user)
    //, _password(password)
      _account(account)
    , _manager(manager){

    // Create request
    //QUrl url;
    _url.setScheme("http");
    _url.setHost(host);
    _url.setPort(port);
    _url.setUserName(user);
    _url.setPassword(password);

    // Handle server replies
    QObject::connect(&_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));
}

QNetworkReply * Client::rpc(const QString & method, const QJsonArray & parameters) {

    // Create RPC payload: http://json-rpc.org/wiki/specification
    // Bitcoind client: https://github.com/bitcoin/bitcoin/blob/master/src/rpcprotocol.cpp
    QJsonObject RPCJson;

    RPCJson["jsonrpc"] = "1.0";
    RPCJson["method"] = method;
    RPCJson["params"] = parameters;
    RPCJson["id"] = "1";

    // Turn into byte array
    QByteArray payload = QJsonDocument(RPCJson).toJson();

    // Set request content type
    QNetworkRequest request(_url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json-rpc");
    request.setAttribute(QNetworkRequest::User, QVariant(method)); // used in Client::finished routine

    // POST RPC and return network reply pointer
    return _manager.post(request, payload);
}

QJsonValue Client::parse(QNetworkReply * reply) {

    // Get response data, without emptying QIODevice
    //(readAll consumes devices, which breaks multiple parsing calls,
    // e.g. due to finished signal and explicit parse call.)
    QByteArray response = reply->peek(reply->bytesAvailable());

    // If there was an error, throw exception
    QNetworkReply::NetworkError e = reply->error();

    if(e != QNetworkReply::NoError)
        throw std::exception("Network request error.");

    // Parse into json
    QJsonDocument jsonResponse = QJsonDocument::fromJson(response);
    QJsonObject jsonObject = jsonResponse.object();

    // Get error
    QJsonValue error = jsonObject["error"];

    // Throw exception if there was an error
    if(!error.isNull()) {

        // Create error string
        QString errorString = "RPC request error: " + error.toString();

        // Throw exception with given error
        throw std::exception(errorString.toLatin1());
    }

    // Return result object
    return jsonObject["result"];
}

void Client::finished(QNetworkReply * reply) {

    // Get method corresponding to reply
    QNetworkRequest request = reply->request();
    QString method = request.attribute(QNetworkRequest::User).toString();

    // Parse response and emit correspondig signal
    if(method == "getblockcount")
        emit blockCount(getBlockCount(reply));
    else if(method == "getbalance")
        emit balance(getBalance(reply));
    else if(method == "listaccounts")
        emit accounts(listAccounts(reply));
}

QNetworkReply * Client::getBlockCount() {
    // Make RPC call
    return rpc("getblockcount", QJsonArray());
}

uint Client::getBlockCount(QNetworkReply * reply) {

    // Get result json or throw error exception
    QJsonValue result = parse(reply);

    // Return int result
    return result.toInt();
}

QNetworkReply * Client::getBalance(int minconf) {

    // Create RPC parameters
    QJsonArray parameters;
    parameters.append(_account);
    parameters.append(minconf);

    // Make RPC call
    return rpc("getbalance", parameters);
}

double Client::getBalance(QNetworkReply * reply) {

    // Get result json or throw error exception
    QJsonValue result = parse(reply);

    // Return int result
    return result.toDouble();
}

QNetworkReply * Client::listAccounts(int minconf) {

    // Create RPC parameters
    QJsonArray parameters;
    parameters.append(minconf);

    // Make RPC call
    return rpc("listaccounts", parameters);
}

QMap<QString, double> Client::listAccounts(QNetworkReply * reply) {

    // Get result json or throw error exception
    QJsonValue result = parse(reply);
    QJsonObject resultObject = result.toObject();

    // Balances
    QMap<QString, double> accountBalances;

    //qDebug() << "listaccounts =";

    for(QJsonObject::const_iterator i = resultObject.constBegin(),
            end(resultObject.constEnd()); i != end;i++) {

        QString name = i.key();
        double balance = i.value().toDouble();

        // Save in map
        accountBalances[name] = balance;

        //qDebug() << name << "->" << balance;
    }

    return accountBalances;
}
