#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <QNetworkRequest>

class QJsonArray;
class QNetworkAccessManager;
class QNetworkReply;

namespace BitCoindRPC {

class Client : public QObject {

    Q_OBJECT

public:

    // Constructor
    Client(QString host, int port, QString user, QString password, QString account, QNetworkAccessManager & manager);

    // Asynchronous routines mkaing bitcoind RPC
    // Corresponds to https://en.bitcoin.it/wiki/Original_Bitcoin_client/API_Calls_list
    QNetworkReply * getBlockCount();
    QNetworkReply * getBalance(int minconf = 1);
    QNetworkReply * listAccounts(int minconf = 1);

    // Parsing routines corresponding to asynchronous calls,
    // can be called externally with finished reply, and
    // is also called by finished slot.
    static uint getBlockCount(QNetworkReply * reply);
    static double getBalance(QNetworkReply * reply);
    QMap<QString, double> listAccounts(QNetworkReply * reply);

private:

    // URL
    QUrl _url;

    /*
    // URL
    QString _host;
    int _port;

    // Authentication
    QString _user;
    QString _password;
    */

    // Bitcoind account to use
    QString _account;

    // Allows access to network
    QNetworkAccessManager & _manager;

    // Asynchronous RPC call of with given method and paramters
    QNetworkReply * rpc(const QString & method, const QJsonArray & parameters);

    // Parses finished reply and returns result json object, otherwise throw exeption
    // Can't be const since QIODevice::readAll() is not const.
    static QJsonValue parse(QNetworkReply * reply);

public slots:

    // Parse reply and emit corresponding signal
    // Can't be const since emitting signal requires non-const this pointer
    void finished(QNetworkReply * reply);

signals:

    void blockCount(uint blockcount);
    void balance(double balance);
    void accounts(QMap<QString, double> accounts);
};

}
#endif // CLIENT_HPP
