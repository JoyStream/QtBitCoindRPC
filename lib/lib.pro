TARGET = QtBitCoindRPCLib
TEMPLATE = lib

CONFIG  += staticlib
CONFIG  += create_prl # Following http://qt-project.org/doc/qt-5/qmake-advanced-usage.html
CONFIG  += console
CONFIG  += c++11 # Needed for class enum
#CONFIG  -= app_bundle

QT     += core network
QT     -= gui

INCLUDEPATH += $$PWD # be able to include w.r.t root of this project

HEADERS += \
    Client.hpp

SOURCES += \
    Client.cpp

CONFIG(debug, debug|release) {
    LIBS += DbgHelp.lib
}
