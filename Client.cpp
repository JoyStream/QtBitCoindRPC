#include "Client.hpp"
#include "Future.hpp"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonDocument>

using namespace BitCoindRPC;

Client::Client(QString host, int port, QString userName, QString password, QString account)
    : _host(host)
    , _port(port)
    , _userName(userName)
    , _password(password)
    , _account(account)
    , _futuresCounter(0) {

    // Create url
    _baseUrl.setScheme("http");
    _baseUrl.setHost(host);
    _baseUrl.setPort(port);
    _baseUrl.setUserName(userName);
    _baseUrl.setPassword(password);

    // _manager::finished should process finished signals
    QObject::connect(&_manager, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(finished(QNetworkReply*)));
}

QNetworkReply * Client::performRPC(QString method, QJsonArray params, quint64 futureId) {

    // Create request
    QNetworkRequest request(_baseUrl);

    // Set content type
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json-rpc");

    // Create json-rpc based on standard: http://json-rpc.org/wiki/specification
    // Bitcoind client: https://github.com/bitcoin/bitcoin/blob/master/src/rpcprotocol.cpp
    QJsonObject RPCJson;

    RPCJson["jsonrpc"] = "1.0";
    RPCJson["method"] = method;
    RPCJson["params"] = params;
    RPCJson["id"] = method + ":" + QString::number(futureId); // use id to recover correct processor and future object

    // Turn into byte array
    QByteArray data = QJsonDocument(RPCJson).toJson();

    // Submit POST request
    QNetworkReply * reply = _manager.post(request, data);

    return reply;
}

quint64 Client::atomicallyGetNextFutureId() {

    quint64 counter;

    _mutex.lock();
    counter = _futuresCounter++;
    _mutex.unlock();

    return counter;
}

void Client::finished(QNetworkReply* reply) {

    // Get data
    QByteArray data = reply->readAll();

    // Check that there was no error
    QNetworkReply::NetworkError e = reply->error();

    if(e != QNetworkReply::NoError) {

        qDebug() << "Request failed:" << data;
        return;
    }

    //qDebug() << data;

    // Parse into json
    QJsonDocument jsonResponse = QJsonDocument::fromJson(data);
    QJsonObject jsonObject = jsonResponse.object();

    // Get error
    QJsonValue error = jsonObject["error"];

    // Call correct handler if there was no error
    if(error.isNull()) {

        // Get result
        QJsonValue result = jsonObject["result"];

        // Recover request type and futureid
        QString id = jsonObject["id"].toString();
        QStringList idList = id.split(":");
        QString methodName = idList.front();
        quint64 futureId = idList.back().toLong();

        // Call corresponding handler
        if(methodName.compare("getblockcount") == 0)
            _getblockcount(result, futureId);
        else if(methodName.compare("getbalance") == 0)
            _getbalance(result, futureId);
        else if(methodName.compare("listaccounts") == 0)
            _listaccounts(result, futureId);
        else
            qDebug() << "Uknown method: id field not recognized.";
    } else
        qDebug() << "Response with error:" << error.toString();

    // Delete reply
    delete reply;
}

Future<uint> * Client::getblockcount() {

    quint64 futureId = atomicallyGetNextFutureId();

    Future<uint> * future = new Future<uint>();

    _getblockcountFutures[futureId] = future;

    performRPC("getblockcount", QJsonArray(), futureId);

    return future;
}

Future<double> * Client::getbalance(int minconf) {

    quint64 futureId = atomicallyGetNextFutureId();

    Future<double> * future = new Future<double>();

    _getbalanceFutures[futureId] = future;

    QJsonArray params;
    params.append(_account);
    params.append(minconf);

    performRPC("getbalance", params, futureId);

    return future;
}

Future<QMap<QString, double>> * Client::listaccounts(int minconf) {

    quint64 futureId = atomicallyGetNextFutureId();

    Future<QMap<QString, double>> * future = new Future<QMap<QString, double>>();

    _listaccountsFutures[futureId] = future;

    QJsonArray params;
    params.append(minconf);

    performRPC("listaccounts", params, futureId);

    return future;
}

void Client::_getblockcount(const QJsonValue & result, quint64 futureId) {

    int blockCount = result.toInt();

    //qDebug() << "getblockcount =" << blockCount;

    QMap<quint64, Future<uint> *>::iterator i = _getblockcountFutures.find(futureId);

    (*i)->setToFinished(blockCount);

    _getblockcountFutures.erase(i);
}

void Client::_getbalance(const QJsonValue & result, quint64 futureId) {

    double balance = result.toDouble();

    //qDebug() << "getbalance =" << balance;

    QMap<quint64, Future<double> *>::iterator i = _getbalanceFutures.find(futureId);

    (*i)->setToFinished(balance);

    _getbalanceFutures.erase(i);
}

void Client::_listaccounts(const QJsonValue & result, quint64 futureId) {

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

    _listaccountsFutures[futureId]->setToFinished(accountBalances);
}
