#include "QtBitCoindRPCTest.hpp"

#include <QNetworkReply>
#include <QSignalSpy>

QtBitCoindRPCTest::QtBitCoindRPCTest()
    : _client("127.0.0.1"
              ,8332
              ,"bitcoinrpc"
              ,"DDKVyZDNros2cKvkk5KpGmJWGazzYMezoWTeKaXcqxEj"
              ,"*"
              ,_manager) {
}

void QtBitCoindRPCTest::getBlockCount() {

    // Get reply
    QNetworkReply * reply = _client.getBlockCount();

    // Spy on finished signal
    QSignalSpy spy(reply, SIGNAL(finished()));

    // Wait for it to be issued
    QVERIFY(spy.wait(5000));

    // Parse result
    uint result = _client.getBlockCount(reply);

    // Check that a positive number of blocks is reported
    QVERIFY(result > 0);

    //qDebug() << result;
}

void QtBitCoindRPCTest::getBalance() {

    // Get reply
    QNetworkReply * reply = _client.getBalance();

    // Spy on finished signal
    QSignalSpy spy(reply, SIGNAL(finished()));

    // Wait for it to be issued
    QVERIFY(spy.wait(5000));

    // Parse result
    double result = _client.getBalance(reply);

    // Check that a positive number of blocks is reported
    QVERIFY(result > 0);

    qDebug() << result;
}

void QtBitCoindRPCTest::listAccounts() {

    // Get reply
    QNetworkReply * reply = _client.listAccounts();

    // Spy on finished signal
    QSignalSpy spy(reply, SIGNAL(finished()));

    // Wait for it to be issued
    QVERIFY(spy.wait(5000));

    // Parse result
    QMap<QString, double> result = _client.listAccounts(reply);

    // Check that a positive number of blocks is reported
    QVERIFY(result.size() > 0);

    qDebug() << result.size();
}

// Generates a main routine which runs all private slot
QTEST_MAIN(QtBitCoindRPCTest)
