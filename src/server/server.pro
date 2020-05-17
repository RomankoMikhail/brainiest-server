QT -= gui
QT += core network sql

CONFIG += c++11 console
CONFIG -= app_bundle

QMAKE_CFLAGS += -Wno-deprecated-copy
QMAKE_CXXFLAGS += -Wno-deprecated-copy

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/../3rdparty/http-parser

SOURCES += \
    $$PWD/../3rdparty/http-parser/http_parser.c \
    api.cpp \
    api/apianswer.cpp \
    api/apiciphers.cpp \
    api/apigame.cpp \
    api/apiquestion.cpp \
    api/apiuser.cpp \
    httprequest.cpp \
    httpresponse.cpp \
    models/answer.cpp \
    models/ciphers.cpp \
    models/game.cpp \
    models/gamehascipher.cpp \
    models/gamehasquestion.cpp \
    models/player.cpp \
    models/question.cpp \
    models/questionhasanswer.cpp \
    models/user.cpp \
    models/useranswered.cpp \
    ongoinggame.cpp \
    token.cpp \
    websocketframe.cpp \
    websocketparser.cpp \
    main.cpp \
    webserver.cpp \
    httpparser.cpp \
    database.cpp

HEADERS += \
    $$PWD/../3rdparty/http-parser/http_parser.h \
    api.h \
    api/apianswer.h \
    api/apiciphers.h \
    api/apigame.h \
    api/apiquestion.h \
    api/apiuser.h \
    httpparser.hpp \
    httprequest.h \
    httpresponse.h \
    models/answer.h \
    models/ciphers.h \
    models/game.h \
    models/gamehascipher.h \
    models/gamehasquestion.h \
    models/player.h \
    models/question.h \
    models/questionhasanswer.h \
    models/user.h \
    models/useranswered.h \
    ongoinggame.h \
    token.h \
    webserver.hpp \
    websocketframe.hpp \
    websocketparser.hpp \
    socketcontext.hpp \
    database.hpp \
    singleton.hpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


