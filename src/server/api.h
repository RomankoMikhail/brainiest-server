#ifndef API_H
#define API_H

#include <QJsonObject>

#define STR_VALUE(arg) #arg

#define REQUIRE_INT(x)                                                         \
    if (!request.arguments().contains(STR_VALUE(x)))                           \
        SEND_ERROR(MissingParameter);                                          \
    int x = request.arguments().value(STR_VALUE(x)).toInt()

#define SEND_RESPONSE(x)                                                       \
    {                                                                          \
        response.setData(formResponse(x), "application/json");                 \
        return;                                                                \
    }                                                                          \
    (void)0

#define SEND_ERROR(x)                                                          \
    {                                                                          \
        response.setData(formError(x), "application/json");                    \
        return;                                                                \
    }                                                                          \
    (void)0

#define REQUIRE_STRING(x)                                                      \
    if (!request.arguments().contains(STR_VALUE(x)))                           \
        SEND_ERROR(MissingParameter);                                          \
    QString x = request.arguments().value(STR_VALUE(x))

#define REQUIRE_TOKEN()                                                        \
    if (!request.arguments().contains("token"))                                \
        SEND_ERROR(TokenRequired);                                             \
                                                                               \
    QString token = request.arguments().value("token");                        \
                                                                               \
    if (!Singleton::tokens().contains(token))                                  \
        SEND_ERROR(InvalidToken);                                              \
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
