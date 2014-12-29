#include "QtBitCoindRPCTest.hpp"

QtBitCoindRPCTest::QtBitCoindRPCTest()
    : _client("127.0.0.1",
              8332,
              "bitcoinrpc",
              "DDKVyZDNros2cKvkk5KpGmJWGazzYMezoWTeKaXcqxEj",
              "*") {
}

void QtBitCoindRPCTest::getBlockCount() {

    // Get future
    QFuture<uint> f = _client.getBlockCount();

    // Block on result
    uint result = f.result();

    qDebug() << result;
}

/*
void QtBitCoindRPCTest::getBlockCountBlocking() {
    qDebug() << _client.getBlockCountBlocking();
}
*/

// Generates a main routine which runs all private slot
QTEST_MAIN(QtBitCoindRPCTest)
