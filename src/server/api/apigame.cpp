#include "apigame.h"

#include "../api.h"
#include "../models/game.h"
#include "../webserver.hpp"
#include "../singleton.hpp"
#include "token.h"

#include <QJsonArray>

void onGameAdd(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
}

void onGameList(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();

    QJsonObject object;

    QList<int> ids = Game::getIds();

    QJsonArray gameArray;

    for (const auto &id : ids)
    {
        QJsonObject userObject;
        Game game = Game::getById(id);

        userObject["id"]       = game.id();
        userObject["authorId"] = game.authorId();
        userObject["date"]     = game.date().toMSecsSinceEpoch();
        userObject["complete"] = game.isComplete();
        userObject["open"]     = game.isOpen();

        gameArray.append(userObject);
    }
    object["items"] = gameArray;

    response.setData(formResponse(object), "application/json");
}

void onGameListOpen(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();

    QJsonObject object;

    QList<int> ids = Game::getIds();

    QJsonArray gameArray;

    for (const auto &id : ids)
    {
        QJsonObject userObject;
        Game game = Game::getById(id);

        if(game.isOpen() == false)
            continue;

        userObject["id"]       = game.id();
        userObject["authorId"] = game.authorId();
        userObject["date"]     = game.date().toMSecsSinceEpoch();
        userObject["complete"] = game.isComplete();
        userObject["open"]     = game.isOpen();

        gameArray.append(userObject);
    }
    object["items"] = gameArray;

    response.setData(formResponse(object), "application/json");
}

extern WebServer *webServer;

void onGameDebug(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_STRING(message);

    WebSocketFrame frame;

    frame.setData(message);
    frame.setOpcode(WebSocketFrame::OpcodeText);
    frame.setIsFinalFrame(true);

    webServer->sendAllWebSockets(frame);
}

void onGameUpdate(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
}

void onGameJoin(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
}

void onGameInfo(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
}

void onGameAnswer(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
}
