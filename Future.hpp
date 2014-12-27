#ifndef FUTURE_HPP
#define FUTURE_HPP

/**
 * Had to create a new future type which allowed me to avoid
 * creating a new thread, as QtConcurrent::run requires. Instead
 * I only need to set the result of the future when some signal occurs
 * (QNetworkManager::finished), however
 *
 * Another short coming of the built in QtFuture is that it has no
 * built in concept of failed computations and time.
 *
 * T must have
 * 1) default constructor for when computation is not completed.
 * 2) assignment operator for saving future copy in client code.
 * 3)
 */

#include "Client.hpp"

#include <QMutex>
#include <QTime>

namespace BitCoindRPC {

template <class T>
class Future
{

    // So that finished service routines can set the result
    friend class Client;

public:

    enum class State {
        Started,
        Finished,
        Failed
    };

    Future();

    State state() const;
    T result() const;
    int age() const;
    QString error() const;

private:

    // Synchronizes access to object
    QMutex _mutex;

    // State of computation
    State _state;

    // Computationl result
    T _result;

    // Time of future creation
    QTime _created;

    //
    QString _error;

    void setResult();
};

}

#endif // FUTURE_HPP

