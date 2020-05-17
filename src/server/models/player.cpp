#include "player.h"

#include "singleton.hpp"

const auto CREATE = QString(
    R"(INSERT INTO player(userid, gameid, won, bonus) VALUES (?, ?, ?, ?))");

const auto UPDATE = QString(
    R"(UPDATE player SET won = ?, bonus = ? WHERE userid = ? AND gameid = ?)");

const auto SELECT_BY_ID =
    QString(R"(SELECT won, bonus FROM player WHERE userid = ? AND gameid = ?)");

const auto SELECT_USER_BY_GAME =
    QString(R"(SELECT userid FROM player WHERE gameid = ?)");

const auto SELECT_GAME_BY_USER =
    QString(R"(SELECT gameid FROM player WHERE userid = ?)");

const auto DELETE =
    QString(R"(DELETE FROM player WHERE userid = ? AND gameid = ?)");

Player Player::create(int gameId, int userId, int won, int bonus)
{
    Player newPlayer;

    newPlayer.mUserId = userId;
    newPlayer.mGameId = gameId;
    newPlayer.mWon    = won;
    newPlayer.mBonus  = bonus;

    QSqlQuery query(Singleton::database().database());

    query.prepare(CREATE);
    query.addBindValue(userId);
    query.addBindValue(gameId);
    query.addBindValue(won);
    query.addBindValue(bonus);

    if (!query.exec())
    {
        qDebug() << query.lastError();
        return Player();
    }

    newPlayer.mIsValid = true;

    return newPlayer;
}

Player Player::getById(int gameId, int userId)
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_BY_ID);
    query.addBindValue(userId);
    query.addBindValue(gameId);
    query.exec();

    if (!query.next())
        return Player();

    Player player;
    player.mUserId  = userId;
    player.mGameId  = gameId;
    player.mWon     = query.value(0).toInt();
    player.mBonus   = query.value(1).toInt();
    player.mIsValid = true;

    return player;
}

QList<int> Player::getGameIds(int userId)
{
    QList<int> ids;

    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_GAME_BY_USER);
    query.addBindValue(userId);

    query.exec();

    while (query.next())
    {
        ids.push_back(query.value(0).toInt());
    }

    return ids;
}

QList<int> Player::getUserIds(int gameId)
{
    QList<int> ids;

    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_USER_BY_GAME);
    query.addBindValue(gameId);

    query.exec();

    while (query.next())
    {
        ids.push_back(query.value(0).toInt());
    }

    return ids;
}

int Player::gameId() const
{
    return mGameId;
}

int Player::userId() const
{
    return mUserId;
}

int Player::won() const
{
    return mWon;
}

void Player::setWon(int won)
{
    mWon = won;
}

int Player::bonus() const
{
    return mBonus;
}

void Player::setBonus(int bonus)
{
    mBonus = bonus;
}

bool Player::isValid() const
{
    return mIsValid;
}

bool Player::update()
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(UPDATE);
    query.addBindValue(mWon);
    query.addBindValue(mBonus);

    bool result = query.exec();

    if (!result)
        qDebug() << query.lastError().text();

    return result;
}

bool Player::remove()
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(DELETE);
    query.addBindValue(mUserId);
    query.addBindValue(mGameId);

    if (!query.exec())
    {
        qDebug() << query.lastError().text();
        return false;
    }

    mIsValid = false;

    return true;
}
