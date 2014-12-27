#include "Client.hpp"
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

QNetworkReply * Client::performRPC(QString method, QJsonArray params) {

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

    // use id to recover correct processor and future object
    RPCJson["id"] = method + ":" + QString::number(_futuresCounter++);

    // Turn into byte array
    QByteArray data = QJsonDocument(RPCJson).toJson();

    // Submit post request
    QNetworkReply * reply = _manager.post(request, data);

    return reply;
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

        // Recover request type
        QString id = jsonObject["id"].toString();


        QString methodName
        quint64 futureId;


        if(method.compare("getblockcount") == 0)
            _getblockcount(result);
        else if(method.compare("getbalance") == 0)
            _getbalance(result);
        else if(method.compare("listaccounts") == 0)
            _listaccounts(result);
        else
            qDebug() << "Uknown method: id field not recognized.";
    } else
        qDebug() << "Response with error:" << error.toString();

    // Delete reply
    delete reply;
}

void Client::getblockcount() {
    performRPC("getblockcount", QJsonArray());
}

void Client::getbalance(int minconf) {

    QJsonArray params;
    params.append(_account);
    params.append(minconf);

    performRPC("getbalance", params);
}

void Client::listaccounts(int minconf) {

    QJsonArray params;
    params.append(minconf);

    performRPC("listaccounts", params);
}

void Client::_getblockcount(const QJsonValue & result) const {

    int blockCount = result.toInt();
    qDebug() << "getblockcount =" << blockCount;
}

void Client::_getbalance(const QJsonValue & result) const {

    double balance = result.toDouble();
    qDebug() << "getbalance =" << balance;
}

void Client::_listaccounts(const QJsonValue & result) const {

    /*
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
    */
}
