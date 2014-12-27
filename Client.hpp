#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <QString>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QJsonObject>
#include <QFuture>
#include <QtGlobal>

class QJsonObject;

namespace BitCoindRPC {

class Client : public QObject {

    Q_OBJECT

public:

    // Constructor
    Client(QString host, int port, QString userName, QString password, QString account);

    // Asynchronous routines for access bitcoind rpc
    void getblockcount();
    void getbalance(int minconf = 1);
    void listaccounts(int minconf = 1);

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

    // Futures
    quint64 _futuresCounter; // counter used to generate unique futures ids across all maps and rpc requests (id field)

    QMap<quint64, QFuture<blockCount> *> _getblockcountFutures;
    QMap<quint64, QFuture<balance> *> _getbalanceFutures;
    QMap<quint64, QFuture<accountBalances> *> _getbalanceFutures;

    // Does rpc to server
    QNetworkReply * performRPC(QString method, QJsonArray params);

    // Process results for corresponding method
    void _getblockcount(const QJsonValue & result, quint64 futureId) const;
    void _getbalance(const QJsonValue & result, quint64 futureId) const;
    void _listaccounts(const QJsonValue & result, quint64 futureId) const;

public slots:

    // Used by _manager
    void finished(QNetworkReply * reply);

signals:

};

}
#endif // CLIENT_HPP
