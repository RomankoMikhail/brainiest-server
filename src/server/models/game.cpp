#include "game.h"

#include "singleton.hpp"

const auto CREATE = QString(
    R"(INSERT INTO `game`(`author_id`, `date`, `is_open`, `is_complete`) VALUES(?, ?, ?, ?))");

const auto UPDATE = QString(
    R"(UPDATE `game` SET `author_id` = ?, `date` = ?, `is_open` = ?, `is_complete` = ? WHERE `id` = ?)");

const auto SELECT_BY_ID =
    QString(R"(SELECT `author_id`, `date`, `is_open`, `is_complete` FROM `game` WHERE `id` = ?)");

const auto SELECT_ALL_IDS = QString(R"(SELECT `id` FROM `game`)");

const auto SELECT_ALL_OPEN_IDS =
    QString(R"(SELECT `id` FROM `game` WHERE `is_open` > 0 AND `is_complete` = 0)");

const auto SELECT_ALL_COMPLETED_IDS = QString(R"(SELECT `id` FROM `game` WHERE `is_complete` > 0)");

Game Game::create(int authorId, QDateTime date, int isOpen, int isComplete)
{
    Game newGame;

    newGame.mAuthorId   = authorId;
    newGame.mDate       = date;
    newGame.mIsOpen     = isOpen;
    newGame.mIsComplete = isComplete;

    QSqlQuery query(Singleton::database().database());

    query.prepare(CREATE);
    query.addBindValue(authorId);
    query.addBindValue(date);
    query.addBindValue(isOpen);
    query.addBindValue(isComplete);

    if (!query.exec())
    {
        qDebug() << query.lastError();
        return Game();
    }

    newGame.mId      = query.lastInsertId().toInt();
    newGame.mIsValid = true;

    return newGame;
}

Game Game::getById(int id)
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_BY_ID);
    query.addBindValue(id);
    query.exec();

    if (!query.next())
        return Game();

    Game game;
    game.mId         = id;
    game.mAuthorId   = query.value(0).toInt();
    game.mDate       = query.value(1).toDateTime();
    game.mIsOpen     = query.value(2).toInt();
    game.mIsComplete = query.value(2).toInt();
    game.mIsValid    = true;

    return game;
}

QList<int> Game::getIds()
{
    QList<int> ids;

    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_ALL_IDS);
    query.exec();

    while (query.next())
    {
        ids.push_back(query.value(0).toInt());
    }

    return ids;
}

QList<int> Game::getOpenIds()
{
    QList<int> ids;

    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_ALL_OPEN_IDS);
    query.exec();

    while (query.next())
    {
        ids.push_back(query.value(0).toInt());
    }

    return ids;
}

QList<int> Game::getCompletedIds()
{
    QList<int> ids;

    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_ALL_COMPLETED_IDS);
    query.exec();

    while (query.next())
    {
        ids.push_back(query.value(0).toInt());
    }

    return ids;
}

int Game::authorId() const
{
    return mAuthorId;
}

void Game::setAuthorId(int authorId)
{
    mAuthorId = authorId;
}

QDateTime Game::date() const
{
    return mDate;
}

void Game::setDate(const QDateTime &date)
{
    mDate = date;
}

int Game::isOpen() const
{
    return mIsOpen;
}

void Game::setIsOpen(int isOpen)
{
    mIsOpen = isOpen;
}

int Game::isComplete() const
{
    return mIsComplete;
}

void Game::setIsComplete(int isComplete)
{
    mIsComplete = isComplete;
}

bool Game::isValid() const
{
    return mIsValid;
}

int Game::id() const
{
    return mId;
}

bool Game::update()
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(UPDATE);
    query.addBindValue(mAuthorId);
    query.addBindValue(mDate);
    query.addBindValue(mIsOpen);
    query.addBindValue(mIsComplete);
    query.addBindValue(mId);

    bool result = query.exec();

    if (!result)
        qDebug() << query.lastError().text();

    return result;
}
