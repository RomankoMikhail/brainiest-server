#include "gamehasquestion.h"

#include "../singleton.hpp"
#include <QList>

const auto CREATE = QString(
    R"(INSERT INTO `game_has_question`(`gameid`, `questionid`, `round`) VALUES(?, ?, ?))");

const auto UPDATE = QString(
    R"(UPDATE `game_has_question` SET `round` = ? WHERE `gameid` = ? and `questionid` = ?)");

const auto SELECT_BY_ID = QString(
    R"(SELECT `round` FROM `game_has_question` WHERE `gameid` = ? and `questionid` = ?)");

const auto SELECT_QUESTION_BY_GAME =
    QString(R"(SELECT `questionid` FROM `game_has_question` WHERE `gameid` = ?)");

const auto SELECT_GAME_BY_QUESTION =
    QString(R"(SELECT `gameid` FROM `game_has_question` WHERE `questionid` = ?)");

const auto DELETE =
    QString(R"(DELETE FROM `game_has_question` WHERE `gameid` = ? and `questionid` = ?)");


GameHasQuestion GameHasQuestion::create(int gameId, int questionId, int round)
{
    GameHasQuestion newGameHasQuestion;

    newGameHasQuestion.mQuestionId = questionId;
    newGameHasQuestion.mGameId   = gameId;
    newGameHasQuestion.mRound    = round;

    QSqlQuery query(Singleton::database().database());

    query.prepare(CREATE);
    query.addBindValue(gameId);
    query.addBindValue(questionId);
    query.addBindValue(round);

    if (!query.exec())
    {
        qDebug() << query.lastError();
        return GameHasQuestion();
    }

    newGameHasQuestion.mIsValid = true;

    return newGameHasQuestion;
}

GameHasQuestion GameHasQuestion::getById(int gameId, int questionId)
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_BY_ID);
    query.addBindValue(gameId);
    query.addBindValue(questionId);
    query.exec();

    if (!query.next())
        return GameHasQuestion();

    GameHasQuestion gameHasQuestion;
    gameHasQuestion.mQuestionId = questionId;
    gameHasQuestion.mGameId   = gameId;
    gameHasQuestion.mRound    = query.value(0).toInt();
    gameHasQuestion.mIsValid    = true;

    return gameHasQuestion;
}

QList<int> GameHasQuestion::getGameIds(int questionId)
{
    QList<int> ids;

    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_GAME_BY_QUESTION);
    query.addBindValue(questionId);

    query.exec();

    while (query.next())
    {
        ids.push_back(query.value(0).toInt());
    }

    return ids;
}

QList<int> GameHasQuestion::getQuestionIds(int gameId)
{
    QList<int> ids;

    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_QUESTION_BY_GAME);
    query.addBindValue(gameId);

    query.exec();

    while (query.next())
    {
        ids.push_back(query.value(0).toInt());
    }

    return ids;
}

bool GameHasQuestion::isValid() const
{
    return mIsValid;
}

int GameHasQuestion::gameId() const
{
    return mGameId;
}

int GameHasQuestion::questionId() const
{
    return mQuestionId;
}


int GameHasQuestion::round() const
{
    return mRound;
}

void GameHasQuestion::setRound(int round)
{
    mRound = round;
}

bool GameHasQuestion::update()
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(UPDATE);
    query.addBindValue(mRound);
    query.addBindValue(mGameId);
    query.addBindValue(mQuestionId);

    bool result = query.exec();

    if (!result)
        qDebug() << query.lastError().text();

    return result;
}

bool GameHasQuestion::remove()
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(DELETE);
    query.addBindValue(mGameId);
    query.addBindValue(mQuestionId);

    if (!query.exec())
    {
        qDebug() << query.lastError().text();
        return false;
    }

    mIsValid = false;

    return true;
}
