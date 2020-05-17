#ifndef API_H
#define API_H

#include <QJsonObject>

#define STR_VALUE(arg) #arg

#define REQUIRE_INT(x)                                                         \
    if (!request.arguments().contains(STR_VALUE(x)))                           \
    {                                                                          \
        response.setData(formError(MissingParameter), "application/json");     \
        return;                                                                \
    }                                                                          \
    int x = request.arguments().value(STR_VALUE(x)).toInt()



#define REQUIRE_STRING(x)                                                      \
    if (!request.arguments().contains(STR_VALUE(x)))                           \
    {                                                                          \
        response.setData(formError(MissingParameter), "application/json");     \
        return;                                                                \
    }                                                                          \
    QString x = request.arguments().value(STR_VALUE(x))

#define REQUIRE_TOKEN()                                                          \
    if (!request.arguments().contains("token"))                                \
    {                                                                          \
        response.setData(formError(TokenRequired), "application/json");        \
        return;                                                                \
    }                                                                          \
                                                                               \
    QString token = request.arguments().value("token");                        \
                                                                               \
    if (!Singleton::tokens().contains(token))                                  \
    {                                                                          \
        response.setData(formError(InvalidToken), "application/json");         \
        return;                                                                \
    }                                                                          \
    (void)0

enum ErrorCodes
{
    Ok,
    GeneralError,
    AuthFailed,
    RegistrationFailed,
    InvalidToken,
    TokenRequired,
    MissingParameter,
    NotFound,
    GameIsRunning,
    GameIsClosed,
    NotYourTurn,
    NotInGame,
};

QByteArray formError(ErrorCodes code = GeneralError);

QByteArray formResponse(const QJsonObject &object = QJsonObject());

#endif // API_H
