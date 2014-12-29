#ifndef QTBITCOINDRPCTEST_HPP
#define QTBITCOINDRPCTEST_HPP

#include <QtTest/QtTest>

#include <lib/Client.hpp>

class QtBitCoindRPCTest : public QObject
{
    Q_OBJECT

public:

    // Constructor
    QtBitCoindRPCTest();

private:

    // BitCoind RPC client
    BitCoindRPC::Client _client;

private slots:

    void getBlockCount();
    //void getBlockCountBlocking();

};

#endif // QTBITCOINDRPCTEST_HPP
