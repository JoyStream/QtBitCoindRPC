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
    , _account(account) {

    // Create url
    _baseUrl.setScheme("http");
    _baseUrl.setHost(host);
    _baseUrl.setPort(port);
    _baseUrl.setUserName(userName);
    _baseUrl.setPassword(password);

    // Connect manager's signals to client slot
    QObject::connect(&_manager, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(finished(QNetworkReply*)));
}

QNetworkReply * Client::performRPC(QString method, QJsonArray params) {

    // Create request
    QNetworkRequest request(_baseUrl);

    // Set content type
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json-rpc");

    // Create json-rpc based on standard: http://json-rpc.org/wiki/specification
    QJsonObject RPCJson;

    RPCJson["jsonrpc"] = "1.0";
    RPCJson["method"] = method;
    RPCJson["params"] = params;
    RPCJson["id"] = method; // use id to recover result parser

    // Turn into byte array
    QByteArray data = QJsonDocument(RPCJson).toJson();

    // Submit post request
    QNetworkReply * reply = _manager.post(request, data);

    return reply;
}


void Client::finished(QNetworkReply* reply) {

    // Check that there was no error
    QNetworkReply::NetworkError e = reply->error();
    if(e != QNetworkReply::NoError) {

        qDebug() << "Request failed, data:" << reply->readAll();
        return;
    }

    // Get data
    QByteArray data = reply->readAll();

    // Parse into json
    QJsonDocument jsonResponse = QJsonDocument::fromJson(data);
    QJsonObject jsonObject = jsonResponse.object();

    // Recover request type
    QString method = jsonObject["id"].toString();

    // Get result and error
    QJsonValue result = jsonObject["result"];
    QJsonValue error = jsonObject["error"];

    // Call correct handler if there was no error
    if(error.isNull()) {

        if(method.compare("getblockcount") == 0)
            _getblockcount(result);
    } else
        qDebug() << "Response with error:" << error.toString();

    // Delete reply
    delete reply;
}

void Client::getblockcount() {
    performRPC("getblockcount", QJsonArray());
}

void Client::_getblockcount(const QJsonValue & result) {

    // Parse result
    int blockCount = result.toInt();

    qDebug() << "_getblockcount:" << blockCount;

}
