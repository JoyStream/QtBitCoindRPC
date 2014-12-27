#include "Future.hpp"

/*
 * Note: even simple assignments are not guaranteed to be
 * atomic, and hence must be synchronized.
 *
 * http://stackoverflow.com/questions/8290768/is-assignment-operator-atomic
 */

Future::Future()
    : _state(Started)
    , _finished(false) {

    _created.start();
}


State Future::state() const {

    State r;

    _mutex.lock();
    r = _state;
    _mutex.unlock();

    return r;
}

T Future::result() const {

}

int Future::age() const {

    int r;

    _mutex.lock();
    r = _created.elapsed();
    _mutex.unlock();

    return r;
}

QString Future::error() const {

    QString r;

    _mutex.lock();
    r = _error;
    _mutex.unlock();

    return r;
}
