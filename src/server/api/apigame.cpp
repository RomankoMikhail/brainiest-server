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
        Game        game = Game::getById(id);

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
        Game        game = Game::getById(id);

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
    game.setIsComplete(complete);

    if (!game.update())
        SEND_ERROR(GeneralError);

    SEND_RESPONSE();
}

void onGameQuit(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_INT(id);

    REQUIRE_TOKEN();

    int  userId = Singleton::tokens().value(token);

    if (Singleton::games().contains(id))
        SEND_ERROR(GeneralError);

    Player player = Player::getById(id, userId);

    if (!player.isValid())
        SEND_ERROR(GeneralError);

    if (!player.remove())
        SEND_ERROR(GeneralError);

    SEND_RESPONSE();
}

void onGameJoin(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_INT(id);

    REQUIRE_TOKEN();

    Game game   = Game::getById(id);
    int  userId = Singleton::tokens().value(token);

    if (!game.isValid())
        SEND_ERROR(NotFound);

    if (!game.isOpen())
        SEND_ERROR(GameIsClosed);

    if (game.isComplete())
        SEND_ERROR(GameIsClosed);

    /*if (game.authorId() == userId)
        SEND_ERROR(GeneralError);*/

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

    QJsonArray questionsArray;

    for (auto questionId : GameHasQuestion::getQuestionIds(id))
    {
        QJsonObject question;

        question["id"]    = questionId;
        question["round"] = GameHasQuestion::getById(id, questionId).round();

        questionsArray.append(question);
    }

    QJsonArray ciphersArray;

    for (auto cipherId : GameHasCipher::getCiphersIds(id))
    {
        QJsonObject cipher;

        cipher["id"] = cipherId;

        ciphersArray.append(cipher);
    }

    object["questions"] = questionsArray;
    object["ciphers"]   = ciphersArray;

    SEND_RESPONSE(object);
}

void onGameStart(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
    REQUIRE_INT(id);

    if (Singleton::games().contains(id))
        SEND_ERROR(GeneralError);

    if (!Singleton::games()[id].load(id))
    {
        Singleton::games().remove(id);
        SEND_ERROR(GeneralError);
    }

    auto game = Game::getById(id);
    game.setIsOpen(false);
    game.update();

    SEND_RESPONSE();
}

void onGameStatus(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
    REQUIRE_INT(id);

    if (!Singleton::games().contains(id))
        SEND_ERROR(NotFound);

    SEND_RESPONSE(Singleton::games()[id].status());
}

void onGameAnswerRound1(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
    REQUIRE_INT(id);
    REQUIRE_INT(answerId);

    if (!Singleton::games().contains(id))
        SEND_ERROR(NotFound);

    auto userId = Singleton::tokens().value(token);

    if (!Singleton::games()[id].answerRound1(userId, answerId))
    {
        SEND_ERROR(GeneralError);
    }

    SEND_RESPONSE();
}

void onGameAnswerRound2(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
    REQUIRE_INT(id);
    REQUIRE_STRING(answer);

    if (!Singleton::games().contains(id))
        SEND_ERROR(NotFound);

    auto userId = Singleton::tokens().value(token);

    if (!Singleton::games()[id].answerRound2(userId, answer))
    {
        SEND_ERROR(GeneralError);
    }

    SEND_RESPONSE();
}

void onGameAnswerRound3(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
    REQUIRE_INT(id);
    REQUIRE_STRING(answer);

    if (!Singleton::games().contains(id))
        SEND_ERROR(NotFound);

    auto userId = Singleton::tokens().value(token);

    if (!Singleton::games()[id].answerRound3(userId, answer))
    {
        SEND_ERROR(GeneralError);
    }

    SEND_RESPONSE();
}

void onGameSelectRound2(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
    REQUIRE_INT(id);
    REQUIRE_STRING(theme);

    if (!Singleton::games().contains(id))
        SEND_ERROR(NotFound);

    auto userId = Singleton::tokens().value(token);

    if (!Singleton::games()[id].selectRound2(userId, theme))
    {
        SEND_ERROR(GeneralError);
    }

    SEND_RESPONSE();
}

void onGameSelectRound3(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
    REQUIRE_INT(id);
    REQUIRE_INT(index);

    if (!Singleton::games().contains(id))
        SEND_ERROR(NotFound);

    auto userId = Singleton::tokens().value(token);

    if (!Singleton::games()[id].selectRound3(userId, index))
    {
        SEND_ERROR(GeneralError);
    }

    SEND_RESPONSE();
}

void onGameDecrypt(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
    REQUIRE_INT(id);
    REQUIRE_STRING(word);

    if (!Singleton::games().contains(id))
        SEND_ERROR(NotFound);

    auto userId = Singleton::tokens().value(token);

    if (!Singleton::games()[id].decrypt(userId, word))
    {
        SEND_ERROR(GeneralError);
    }

    SEND_RESPONSE();
}

void onGameAddQuestion(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
    REQUIRE_INT(id);
    REQUIRE_INT(questionId);
    REQUIRE_INT(round);

    if (!GameHasQuestion::create(id, questionId, round).isValid())
        SEND_ERROR(GeneralError);

    SEND_RESPONSE();
}

void onGameAddCipher(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
    REQUIRE_INT(id);
    REQUIRE_INT(cipherId);

    if (!GameHasCipher::create(id, cipherId).isValid())
        SEND_ERROR(GeneralError);

    SEND_RESPONSE();
}

void onGameRemoveQuestion(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
    REQUIRE_INT(id);
    REQUIRE_INT(questionId);

    auto gameHasQuestion = GameHasQuestion::getById(id, questionId);

    if (!gameHasQuestion.isValid())
        SEND_ERROR(NotFound);

    if (!gameHasQuestion.remove())
        SEND_ERROR(GeneralError);

    SEND_RESPONSE();
}

void onGameRemoveCipher(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
    REQUIRE_INT(id);
    REQUIRE_INT(cipherId);

    auto gameHasCipher = GameHasCipher::getById(id, cipherId);

    if (!gameHasCipher.isValid())
        SEND_ERROR(NotFound);

    if (!gameHasCipher.remove())
        SEND_ERROR(GeneralError);

    SEND_RESPONSE();
}
