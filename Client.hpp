#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <QString>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QJsonObject>

class QJsonObject;

namespace BitCoindRPC {

class Client : public QObject {

    Q_OBJECT

public:

    // Constructor
    Client(QString host, int port, QString userName, QString password, QString account);

    // Asynchronous routines for access bitcoind rpc
    void getblockcount();

private:

    // URL
    QString _host;
    int _port;

    // Authentication
    QString _userName;
    QString _password;

    // Url
    QUrl _baseUrl;

    // Bitcoind account to use
    QString _account;

    // Allows access to network
    QNetworkAccessManager _manager;

    // Does rpc to server
    QNetworkReply * performRPC(QString method, QJsonArray params);

    // Create request
    QNetworkRequest createRequest(QString method);

    // QJson rpc base object (http://json-rpc.org/wiki/specification)
    QJsonObject createRPCJson(QString method, QJsonArray params);

    // Used by QNetworkReply reply objects
    void _getblockcount(const QByteArray & data);

public slots:

    // Used by _manager
    void authenticationRequired(QNetworkReply * reply, QAuthenticator * authenticator);
    //void encrypted(QNetworkReply * reply);
    void finished(QNetworkReply * reply);
    //void networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility accessible);
    //void proxyAuthenticationRequired(const QNetworkProxy & proxy, QAuthenticator * authenticator);
    //void sslErrors(QNetworkReply * reply, const QList<QSslError> & errors);

signals:

};

}
#endif // CLIENT_HPP
