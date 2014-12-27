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

    // Process results for corresponding method
    void _getblockcount(const QJsonValue & result);

public slots:

    // Used by _manager
    void finished(QNetworkReply * reply);

signals:

};

}
#endif // CLIENT_HPP
