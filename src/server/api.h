#ifndef API_H
#define API_H

#include <QJsonObject>

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
