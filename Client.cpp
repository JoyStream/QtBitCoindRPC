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
    //_baseUrl.setUserName(userName);
    //_baseUrl.setPassword(password);

    // Connect manager's signals to client slots
    QObject::connect(&_manager, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(finished(QNetworkReply*)));
    QObject::connect(&_manager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
                     this, SLOT(authenticationRequired(QNetworkReply*,QAuthenticator*)));
}

QNetworkRequest Client::createRequest(QString method) {

    // Create full url
    QUrl url(_baseUrl);
    url.setPath("/" + method);

    // Create request
    QNetworkRequest request(url);

    // Set content type
    //request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json-rpc");

    request.setRawHeader(QByteArray("Content-Type"), QByteArray("application/json-rpc"));
    request.setRawHeader(QByteArray("Host"), QByteArray("127.0.0.1"));
    request.setRawHeader(QByteArray("Authorization"), QByteArray("Basic xxxs"));

    //request.setRawHeader(QByteArray("Host"), QByteArray("127.0.0.1"));

    /*
    request.setRawHeader("Host", _host);

request_info << "Content-Type: application/json-rpc\n";
                    request_info << ":  " << b64_password << "\n";
                    request_info << "Content-Length: "<<json.size() << "\n\n";
    */

    // Set attribute to find source of request
    request.setAttribute(QNetworkRequest::User, QVariant(method));

    return request;
}

QNetworkReply * Client::performRPC(QString method, QJsonArray params) {

    // Get request
    QNetworkRequest request = createRequest(method);

    // Get rpc json
    QJsonObject RPCJson = createRPCJson(method, params);

    // Turn into byte array
    QByteArray data = QJsonDocument(RPCJson).toJson();

    //request.setHeader(QNetworkRequest::ContentLengthHeader, QString::number(data.length()));
    request.setRawHeader(QByteArray("Content-Length"), QByteArray("1"));

    // Submit post request
    QNetworkReply * reply = _manager.post(request, data);

    /*
    // Diagnotics
    qDebug() << "Posted RPC:";

     reply->request();

    qDebug() << "url: ";
*/


    return reply;
}

QJsonObject Client::createRPCJson(QString method, QJsonArray params) {

    QJsonObject json;

    json["jsonrpc"] = "1.0";
    json["method"] = method;
    json["params"] = params;
    json["id"] = "1";

    return json;
}

void Client::finished(QNetworkReply* reply) {

    //qDebug() << "finished:" << reply->url().toString();

    // Check that there was no error
    QNetworkReply::NetworkError e = reply->error();
    if(e != QNetworkReply::NoError) {

        qDebug() << "REQUEST FAILED";
        qDebug() << "data:" << reply->readAll();
        //return;
    }

    // What was source of request
    QVariant variant = reply->attribute(QNetworkRequest::User);
    QString source = variant.toString();

    // Get data
    QByteArray data = reply->readAll();

    if(source.compare("getblockcount"))
        _getblockcount(data);

    // Delete reply
    delete reply;
}

void Client::authenticationRequired(QNetworkReply * reply, QAuthenticator * authenticator) {
    qDebug() << "authenticationRequired:";
}

void Client::getblockcount() {
    performRPC("getblockcount", QJsonArray());
}

void Client::_getblockcount(const QByteArray & data) {
    qDebug() << "_getblockcount:" << data.toInt();
}
