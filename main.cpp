#include <QCoreApplication>
#include "Client.hpp"

#include <QObject>

class ClientRunner : public QObject {

    Q_OBJECT

public:

 Q_INVOKABLE void start() {

        // Create client
        BitCoindRPC::Client client("127.0.0.1",
                                 8332,
                                 "bitcoinrpc",
                                 "DDKVyZDNros2cKvkk5KpGmJWGazzYMezoWTeKaXcqxEj",
                                 "*");

        // Make request
        qDebug() << client.getBalanceBlocking();

    }
};


int main(int argc, char *argv[]) {

    QCoreApplication a(argc, argv);

    ClientRunner * r = new ClientRunner();

    //QMetaObject::invokeMethod(&r, "start");

    return a.exec();
}
