#include "useranswered.h"
#include "singleton.hpp"

const auto CREATE = QString(
    R"(INSERT INTO `user_answered`(`userid`, `gameid`, `questionid`, `answerid`) VALUES(?, ?, ?, ?))");

const auto CREATE_2 =
    QString(R"(INSERT INTO `user_answered`(`userid`, `gameid`, `questionid`, `answerid`) VALUES(?, ?, ?, NULL))");

const auto UPDATE = QString(
    R"(UPDATE `user_answered` SET `answerid` = ? WHERE `userid` = ? and `gameid` = ? and `questionid` = ?)");

const auto SELECT_BY_ID = QString(
    R"(SELECT `answerid` FROM `user_answered` WHERE `userid` = ? and `gameid` = ? and `questionid` = ?)");

const auto SELECT_BY_QUESTION = QString(
    R"(SELECT `gameid` FROM `user_answered` WHERE `questionid` = ?)");

const auto SELECT_USERS =
    QString(R"(SELECT userid FROM user_answered WHERE gameid = ? and questionid = ?)");

const auto DELETE = QString(
    R"(DELETE FROM `user_answered` WHERE `userid` = ? and `gameid` = ? and `questionid` = ?)");


UserAnswered UserAnswered::create(int userId, int gameId, int questionId)
{
    UserAnswered newUserAnswered;

    newUserAnswered.mQuestionId = questionId;
    newUserAnswered.mAnswerId   = 0;
    newUserAnswered.mUserId     = userId;
    newUserAnswered.mGameId     = gameId;

    QSqlQuery query(Singleton::database().database());

    query.prepare(CREATE_2);
    query.addBindValue(userId);
    query.addBindValue(gameId);
    query.addBindValue(questionId);


    if (!query.exec())
    {
        qDebug() << query.lastError();
        qDebug() << query.lastQuery();
        qDebug() << query.lastError().databaseText();
        return UserAnswered();
    }

    newUserAnswered.mIsValid = true;

    return newUserAnswered;
}

UserAnswered UserAnswered::create(int userId, int gameId, int questionId, int answerId)
{
    UserAnswered newUserAnswered;

    newUserAnswered.mQuestionId = questionId;
    newUserAnswered.mAnswerId   = answerId;
    newUserAnswered.mUserId     = userId;
    newUserAnswered.mGameId     = gameId;

    QSqlQuery query(Singleton::database().database());

    query.prepare(CREATE);
    query.addBindValue(userId);
    query.addBindValue(gameId);
    query.addBindValue(questionId);
    query.addBindValue(answerId);

    if (!query.exec())
    {
        qDebug() << query.lastError();
        return UserAnswered();
    }

    newUserAnswered.mIsValid = true;

    return newUserAnswered;
}

UserAnswered UserAnswered::getById(int userId, int gameId, int questionId)
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_BY_ID);
    query.addBindValue(userId);
    query.addBindValue(gameId);
    query.addBindValue(questionId);
    query.exec();

    if (!query.next())
        return UserAnswered();

    UserAnswered userAnswered;
    userAnswered.mQuestionId = questionId;
    userAnswered.mGameId     = gameId;
    userAnswered.mUserId     = userId;
    userAnswered.mAnswerId   = query.value(0).toInt();
    userAnswered.mIsValid    = true;

    return userAnswered;
}

QList<int> UserAnswered::getGamesIds(int questionId)
{
    QList<int> ids;

    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_BY_QUESTION);

    query.addBindValue(questionId);

    query.exec();

    while (query.next())
    {
        ids.push_back(query.value(0).toInt());
    }

    return ids;
}

QList<int> UserAnswered::getUsersIds(int gameId, int questionId)
{
    QList<int> ids;

    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_USERS);

    query.addBindValue(gameId);
    query.addBindValue(questionId);

    query.exec();

    while (query.next())
    {
        ids.push_back(query.value(0).toInt());
    }

    return ids;
}

bool UserAnswered::update()
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(UPDATE);
    query.addBindValue(mAnswerId);
    query.addBindValue(mUserId);
    query.addBindValue(mGameId);
    query.addBindValue(mQuestionId);

    mIsValid = query.exec();

    if (!mIsValid)
    {
        qDebug() << query.lastError().text();
    }
    query.finish();
    return mIsValid;
}

bool UserAnswered::remove()
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(DELETE);
    query.addBindValue(mAnswerId);
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

bool UserAnswered::isValid() const
{
    return mIsValid;
}

int UserAnswered::userId() const
{
    return mUserId;
}

int UserAnswered::gameId() const
{
    return mGameId;
}

int UserAnswered::questionId() const
{
    return mQuestionId;
}

int UserAnswered::answerId() const
{
    return mAnswerId;
}

void UserAnswered::setAnswerId(int answerId)
{
    mAnswerId = answerId;
}
