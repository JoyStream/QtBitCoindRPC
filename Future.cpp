#include "Future.hpp"

/*
 * Note: even simple assignments are not guaranteed to be
 * atomic, and hence must be synchronized.
 *
 * http://stackoverflow.com/questions/8290768/is-assignment-operator-atomic
 */

using namespace BitCoindRPC;

template <class T>
Future<T>::Future()
    : _state(State::Active) {

    // Start timer
    _created.start();
}

template <class T>
State Future<T>::state() const {

    State r;

    _mutex.lock();
    r = _state;
    _mutex.unlock();

    return r;
}

template <class T>
T Future<T>::result() const {

    T r;

    _mutex.lock();

    // Wait until result has been set,
    if(_state == State::Active)
        _condition.wait(_mutex);

    r = _result;
    _mutex.unlock();

    // If processing failed, then raise exception
    if(_state == State::Failed)
        throw std::exception("Processing failed.");

    return r;
}

template <class T>
bool Future<T>::expired(int milliSecondLimit) const {

    bool r;

    _mutex.lock();
    r = _created.elapsed() > milliSecondLimit;
    _mutex.unlock();

    return r;
}

template <class T>
QString Future<T>::error() const {

    QString r;

    _mutex.lock();
    r = _error;
    _mutex.unlock();

    return r;
}

template <class T>
void Future<T>::finished(const T & result) {

    _mutex.lock();
    _result = result;
    _state = State::Finished;
    _condition.wakeAll();
    _mutex.unlock();
}

template <class T>
void Future<T>::failed(const QString & error) {

    _mutex.lock();
    _state = State::Failed;
    _condition.wakeAll();
    _error = error;
    _mutex.unlock();
}
