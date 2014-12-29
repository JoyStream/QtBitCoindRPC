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
 * T must have a default constructor for when computation is not completed.
 */

#include <QMutex>
#include <QWaitCondition>
#include <QTime>

namespace BitCoindRPC {

// Should be inside Future class, but caused all sorts of issues, fix later.
enum class State {
    Active,
    Finished,
    Failed
};

template <class T>
class Future {

    // Client should have access to private methods
    // * void finished(const T & result)
    // * void failed(const QString & error)
    friend class Client;

public:

    // Default constructor
    Future();

    // Getters
    State state() const;
    T result() const;
    bool expired(int milliSecondLimit) const;
    QString error() const;

signals:

    // Fires when state is altered from State::Active to State::Finished
    void finished() const;

    // Fires when state is altered from State::Active to State::Failed
    void failed() const;

private:

    // Synchronizes access to object
    QMutex _mutex;

    // Condition variable for blocking result() calls
    QWaitCondition _condition;

    // State of computation
    State _state;

    // Computational result
    T _result;

    // Time of future creation, used to service expired()
    QTime _created;

    // Error string which is set in setError if processing failed
    QString _error;

    // Used by client to set the result
    void setToFinished(const T & result);

    // Alters state to Failed and saves error message
    void setToFailed(const QString & error);
};

}

#endif // FUTURE_HPP

