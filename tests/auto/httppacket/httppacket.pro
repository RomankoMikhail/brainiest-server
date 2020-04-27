QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

QMAKE_CFLAGS += -Wno-deprecated-copy
QMAKE_CXXFLAGS += -Wno-deprecated-copy

SOURCES +=  tst_httppacket.cpp

INCLUDEPATH += $$PWD/../../../src/server/
HEADERS += $$PWD/../../../src/server/httppacket.hpp
SOURCES += $$PWD/../../../src/server/httppacket.cpp
