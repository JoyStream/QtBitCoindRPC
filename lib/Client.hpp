#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <QNetworkAccessManager>
#include <QNetworkRequest>

class QJsonObject;
template <typename T> class QFuture;
class QEventLoop;

namespace BitCoindRPC {

class Client : public QObject {

    Q_OBJECT

public:

    // Constructor
    Client(QString host, int port, QString userName, QString password, QString account, QNetworkAccessManager * manager = NULL);

    // Destructor
    ~Client();

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

    // If network manager was allocated in constructor
    bool _ownsNetworkManager;

    // Allows access to network
    QNetworkAccessManager * _manager;

    // Request sent used for all RPC calls (is read-only after ctor)
    QNetworkRequest _request;

    // Blocking RPC call of given method with given parameters
    QJsonValue rpc(const QString & method, const QJsonArray & parameters);

    // Blocking POST which must be done on same thread as _manager owner
    Q_INVOKABLE QNetworkReply * blockingPostOnManagerThread(const QByteArray & payload);
};

}
#endif // CLIENT_HPP
