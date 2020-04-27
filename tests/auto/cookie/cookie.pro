QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

QMAKE_CFLAGS += -Wno-deprecated-copy
QMAKE_CXXFLAGS += -Wno-deprecated-copy

TEMPLATE = app

SOURCES +=  tst_cookie.cpp
