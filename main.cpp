#include <QCoreApplication>
#include "Client.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Create
    BitCoindRPC::Client client("127.0.0.1",
                             8332,
                             "bitcoinrpc",
                             "DDKVyZDNros2cKvkk5KpGmJWGazzYMezoWTeKaXcqxEj",
                             "");

    // Make get block request
    client.getblockcount();

    return a.exec();
}
