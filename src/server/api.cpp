#include "api.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

static QString ErrorMessages[]{"Ok",
                               "General error occured",
                               "User authorization failed",
                               "User registration failed",
                               "Token not found or invalid token given",
                               "This method requires token",
                               "One or several of the parameters specified was missing or invalid",
                               "Data not found",
                                "Game is currently running",
                                "Game is closed",
                                "This is not your turn",
                                "You are not in this game"};

QByteArray formError(ErrorCodes code)
{
    QJsonObject root;
    QJsonObject error;

    error["code"]    = code;
    error["message"] = ErrorMessages[code];

    root["error"] = error;

    QJsonDocument document(root);
    return document.toJson(QJsonDocument::Compact);
}

QByteArray formResponse(const QJsonObject &object)
{
    QJsonObject root;

    if (!object.isEmpty())
        root["response"] = object;
    else
        root["response"] = 1;

    QJsonDocument document(root);
    return document.toJson(QJsonDocument::Compact);
}
