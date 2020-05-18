#include "question.h"

#include "../singleton.hpp"

const auto CREATE =
    QString(R"(INSERT INTO `question`(`theme`, `question`, `type`) VALUES(?, ?, ?))");

const auto UPDATE =
    QString(R"(UPDATE `question` SET `theme` = ?, `question` = ?, `type` = ? WHERE `id` = ?)");

const auto SELECT_BY_ID =
    QString(R"(SELECT `theme`, `question`, `type` FROM `question` WHERE `id` = ?)");

const auto SELECT_BY_THEME = QString(R"(SELECT `id` FROM `question` WHERE `theme` LIKE ?)");

const auto SELECT_ALL_IDS = QString(R"(SELECT `id` FROM `question`)");

const auto SELECT_ALL_THEMES = QString(R"(SELECT DISTINCT `theme` FROM `question`)");

const auto DELETE = QString(R"(DELETE FROM `question` WHERE `id` = ?)");

Question Question::create(const QString &theme, const QString &question, int type)
{
    Question newQuestion;

    newQuestion.mTheme    = theme;
    newQuestion.mQuestion = question;
    newQuestion.mType     = type;

    QSqlQuery query(Singleton::database().database());

    query.prepare(CREATE);
    query.addBindValue(theme);
    query.addBindValue(question);
    query.addBindValue(type);

    if (!query.exec())
    {
        qDebug() << query.lastError();
        return Question();
    }

    newQuestion.mId      = query.lastInsertId().toInt();
    newQuestion.mIsValid = true;

    return newQuestion;
}

Question Question::getById(int id)
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_BY_ID);
    query.addBindValue(id);
    query.exec();

    if (!query.next())
        return Question();

    Question question;
    question.mId       = id;
    question.mTheme    = query.value(0).toString();
    question.mQuestion = query.value(1).toString();
    question.mType     = query.value(2).toInt();
    question.mIsValid  = true;

    return question;
}

QList<int> Question::getByTheme(QString theme)
{
    QList<int> ids;

    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_BY_THEME);

    query.addBindValue(theme);

    query.exec();

    while (query.next())
    {
        ids.push_back(query.value(0).toInt());
    }

    return ids;
}

QStringList Question::getThemes()
{
    QStringList ids;

    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_ALL_THEMES);
    query.exec();

    while (query.next())
    {
        ids.push_back(query.value(0).toString());
    }

    return ids;
}

QList<int> Question::getIds()
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

QString Question::theme() const
{
    return mTheme;
}

void Question::setTheme(const QString &theme)
{
    mTheme = theme;
}

QString Question::question() const
{
    return mQuestion;
}

void Question::setQuestion(const QString &question)
{
    mQuestion = question;
}

int Question::type() const
{
    return mType;
}

void Question::setType(int type)
{
    mType = type;
}

bool Question::update()
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(UPDATE);
    query.addBindValue(mTheme);
    query.addBindValue(mQuestion);
    query.addBindValue(mType);
    query.addBindValue(mId);

    bool result = query.exec();

    if (!result)
        qDebug() << query.lastError().text();

    return result;
}

bool Question::remove()
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(DELETE);
    query.addBindValue(mId);

    if (!query.exec())
    {
        qDebug() << query.lastError().text();
        return false;
    }

    mIsValid = false;

    return true;
}

int Question::id() const
{
    return mId;
}

bool Question::isValid() const
{
    return mIsValid;
}
