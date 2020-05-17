#include "apigame.h"

#include "../api.h"
#include "../models/game.h"
#include "../models/gamehascipher.h"
#include "../models/gamehasquestion.h"
#include "../models/player.h"
#include "../singleton.hpp"
#include "../webserver.hpp"
#include "token.h"

#include <QJsonArray>

void onGameAdd(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();

    QJsonObject object;

    int userId = Singleton::tokens().value(token);

    Game game = Game::create(userId, QDateTime::currentDateTime(), 0, 0);

    if (!game.isValid())
        SEND_ERROR(GeneralError);

    SEND_RESPONSE();
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

    SEND_RESPONSE(object);
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

        if (game.isOpen() == false)
            continue;

        userObject["id"]       = game.id();
        userObject["authorId"] = game.authorId();
        userObject["date"]     = game.date().toMSecsSinceEpoch();
        userObject["complete"] = game.isComplete();
        userObject["open"]     = game.isOpen();

        gameArray.append(userObject);
    }
    object["items"] = gameArray;

    SEND_RESPONSE(object);
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
    REQUIRE_INT(id);
    REQUIRE_INT(open);
    REQUIRE_INT(complete);

    REQUIRE_TOKEN();

    Game game = Game::getById(id);

    if (!game.isValid())
        SEND_ERROR(NotFound);

    game.setIsOpen(open);
    game.setIsOpen(complete);

    if (!game.update())
        SEND_ERROR(GeneralError);

    SEND_RESPONSE();
}

void onGameJoin(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_INT(id);

    REQUIRE_TOKEN();

    Game game  = Game::getById(id);
    int userId = Singleton::tokens().value(token);

    if (!game.isValid())
        SEND_ERROR(NotFound);

    if (!game.isOpen())
        SEND_ERROR(GameIsClosed);

    if (game.isComplete())
        SEND_ERROR(GameIsClosed);

    if (game.authorId() == userId)
        SEND_ERROR(GeneralError);

    Player player = Player::getById(id, userId);

    if (player.isValid())
        SEND_ERROR(GeneralError);

    player = Player::create(id, userId, 0, 0);

    if (!player.isValid())
        SEND_ERROR(GeneralError);

    SEND_RESPONSE();
}

void onGameInfo(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_INT(id);

    REQUIRE_TOKEN();

    Game game = Game::getById(id);

    if (!game.isValid())
        SEND_ERROR(NotFound);

    QJsonObject object;

    object["id"]       = game.id();
    object["date"]     = game.date().toSecsSinceEpoch();
    object["authorId"] = game.authorId();
    object["open"]     = game.isOpen();
    object["complete"] = game.isComplete();

    SEND_RESPONSE(object);
}

void onGameAnswer(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
}

void onGameSkip(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
}

void onGameStart(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
}

void onGameStatus(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
}
