#ifndef QTBITCOINDRPCTEST_HPP
#define QTBITCOINDRPCTEST_HPP

#include <QtTest/QtTest>
#include <QNetworkAccessManager>

#include <lib/Client.hpp>

class QtBitCoindRPCTest : public QObject
{
    Q_OBJECT

public:

    // Constructor
    QtBitCoindRPCTest();

private:

    // Network manager used by client
    QNetworkAccessManager _manager;

    // BitCoind RPC client
    BitCoindRPC::Client _client;

private slots:

    void getBlockCount();
    void getBalance();
    void listAccounts();

};

#endif // QTBITCOINDRPCTEST_HPP
