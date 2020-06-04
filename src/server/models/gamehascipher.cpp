#include "gamehascipher.h"

#include "singleton.hpp"

const auto CREATE = QString(
    R"(INSERT INTO `game_has_ciphers`(`gameid`, `ciphersid`, `round`) VALUES(?, ?, ?))");

const auto SELECT_BY_ID = QString(
    R"(SELECT * FROM `game_has_ciphers` WHERE `gameid` = ? and `ciphersid` = ?)");

const auto SELECT_BY_ROUND = QString(R"(SELECT `ciphersid` FROM `game_has_ciphers` WHERE `gameid` = ? and `round` = ?)");

const auto SELECT_CIPHERS_BY_GAME =
    QString(R"(SELECT `ciphersid` FROM `game_has_ciphers` WHERE `gameid` = ?)");

const auto SELECT_GAME_BY_CIPHERS =
    QString(R"(SELECT `gameid` FROM `game_has_ciphers` WHERE `ciphersid` = ?)");

const auto DELETE = QString(
    R"(DELETE FROM `game_has_ciphers` WHERE `gameid` = ? and `ciphersid` = ?)");

int GameHasCipher::ciphersId() const
{
    return mCiphersId;
}

bool GameHasCipher::remove()
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(DELETE);
    query.addBindValue(mGameId);
    query.addBindValue(mCiphersId);

    if (!query.exec())
    {
        qDebug() << query.lastError().text();
        return false;
    }

    mIsValid = false;

    return true;
}

GameHasCipher GameHasCipher::create(int gameId, int ciphersId, int round)
{
    GameHasCipher newGameHasCiphers;

    newGameHasCiphers.mCiphersId = ciphersId;
    newGameHasCiphers.mGameId    = gameId;
    newGameHasCiphers.mRound = round;

    QSqlQuery query(Singleton::database().database());

    query.prepare(CREATE);
    query.addBindValue(gameId);
    query.addBindValue(ciphersId);
    query.addBindValue(round);

    if (!query.exec())
    {
        qDebug() << query.lastError();
        return GameHasCipher();
    }

    newGameHasCiphers.mIsValid = true;

    return newGameHasCiphers;
}

GameHasCipher GameHasCipher::getById(int gameId, int ciphersId)
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_BY_ID);
    query.addBindValue(gameId);
    query.addBindValue(ciphersId);
    query.exec();

    if (!query.next())
        return GameHasCipher();

    GameHasCipher gameHasQuestion;
    gameHasQuestion.mCiphersId = ciphersId;
    gameHasQuestion.mGameId    = gameId;
    gameHasQuestion.mRound     = query.value("round").toInt();
    gameHasQuestion.mIsValid   = true;

    return gameHasQuestion;
}

QList<int> GameHasCipher::getGameIds(int ciphersId)
{
    QList<int> ids;

    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_GAME_BY_CIPHERS);
    query.addBindValue(ciphersId);

    query.exec();

    while (query.next())
    {
        ids.push_back(query.value(0).toInt());
    }

    return ids;
}

QList<int> GameHasCipher::getCiphersIds(int gameId)
{
    QList<int> ids;

    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_CIPHERS_BY_GAME);
    query.addBindValue(gameId);

    query.exec();

    while (query.next())
    {
        ids.push_back(query.value(0).toInt());
    }

    return ids;
}

QList<int> GameHasCipher::getCiphersIdsByRound(int gameId, int round)
{
    QList<int> ids;

    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_BY_ROUND);
    query.addBindValue(gameId);
    query.addBindValue(round);

    query.exec();

    while (query.next())
    {
        ids.push_back(query.value(0).toInt());
    }

    return ids;
}

bool GameHasCipher::isValid() const
{
    return mIsValid;
}

int GameHasCipher::gameId() const
{
    return mGameId;
}
