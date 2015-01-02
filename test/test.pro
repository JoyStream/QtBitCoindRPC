TARGET = QtBitCoindRPCTest
TEMPLATE = app

CONFIG += link_prl # Following http://qt-project.org/doc/qt-5/qmake-advanced-usage.html
CONFIG += console
QT     += core network testlib

INCLUDEPATH += $$PWD\.. # be able to include w.r.t root of this project

SOURCES += \
    QtBitCoindRPCTest.cpp

HEADERS += \
    QtBitCoindRPCTest.hpp

# Link with lib
# conditions required because on windows the builds are put in extra release/debug subfolders
win32:CONFIG(release, debug|release):       LIBS += -L$$OUT_PWD/../lib/release/ -lQtBitCoindRPCLib
else:win32:CONFIG(debug, debug|release):    LIBS += -L$$OUT_PWD/../lib/debug/ -lQtBitCoindRPCLib
else:unix:                                  LIBS += -L$$OUT_PWD/../lib/ -lQtBitCoindRPCLib
