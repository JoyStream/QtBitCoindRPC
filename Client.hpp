#ifndef CLIENT_HPP
#define CLIENT_HPP

//#include "Future.hpp" // Not sure we even need to include this, try to forward declare later

#include <QString>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QJsonObject>
#include <QFuture>
#include <QtGlobal>
#include <QMutex>

class QJsonObject;

namespace BitCoindRPC {

template <class T>
class Future;

class Client : public QObject {

    Q_OBJECT

public:

    // Constructor
    Client(QString host, int port, QString userName, QString password, QString account);

    // Asynchronous routines for access bitcoind RPC
    // Caller owns object, but should not delete it until future object
    // emits state change signal.
    Future<uint> * getblockcount();
    Future<double> * getbalance(int minconf = 1);
    Future<QMap<QString, double>> * listaccounts(int minconf = 1);

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

    // Synchronizes atomicallyGetNextFutureId()
    QMutex _mutex;

    // Futures
    quint64 _futuresCounter; // counter used to generate unique futures ids across all maps and rpc requests (id field)

    QMap<quint64, Future<uint> *> _getblockcountFutures;
    QMap<quint64, Future<double> *> _getbalanceFutures;
    QMap<quint64, Future<QMap<QString, double>> *> _listaccountsFutures;

    // Does rpc to server
    QNetworkReply * performRPC(QString method, QJsonArray params, quint64 futureId);

    // Atomically increments _futuresCounter
    quint64 atomicallyGetNextFutureId();

    // Handlers used by finished slot
    void _getblockcount(const QJsonValue & result, quint64 futureId);
    void _getbalance(const QJsonValue & result, quint64 futureId);
    void _listaccounts(const QJsonValue & result, quint64 futureId);

private slots:

    // Used by _manager
    void finished(QNetworkReply * reply);

signals:

};

}
#endif // CLIENT_HPP
