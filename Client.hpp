#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <QNetworkAccessManager>
#include <QNetworkRequest>

class QJsonObject;
template <typename T> class QFuture;

namespace BitCoindRPC {

class Client {

public:

    // Constructor
    Client(QString host, int port, QString userName, QString password, QString account);

    // Asynchronous routines for accessing bitcoind RPC
    QFuture<uint> getBlockCount();
    QFuture<double> getBalance(int minconf = 1);
    QFuture<QMap<QString, double>> listAccounts(int minconf = 1);

    // Synchronous routines for accessing bitcoind RPC (used by QtConcurrent::run calls)
    uint getBlockCountBlocking();
    double getBalanceBlocking(int minconf = 1);
    QMap<QString, double> listAccountsBlocking(int minconf = 1);

private:

    // URL
    QString _host;
    int _port;

    // Authentication
    QString _user;
    QString _password;

    // Bitcoind account to use
    QString _account;

    // Request sent used for all RPC calls (is read-only after ctor)
    QNetworkRequest _request;

    // Allows access to network
    QNetworkAccessManager _manager;

    // Does blocking RPC call of given method with given parameters
    QJsonValue blockingRPC(const QString & method, const QJsonArray & parameters);
};

}
#endif // CLIENT_HPP
