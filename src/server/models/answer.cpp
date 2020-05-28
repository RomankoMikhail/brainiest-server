#include "answer.h"

#include "singleton.hpp"

const auto CREATE = QString(R"(INSERT INTO `answer`(`answer`) VALUES(?))");

const auto UPDATE = QString(R"(UPDATE `answer` SET `answer` = ? WHERE `id` = ?)");

const auto SELECT_BY_ID = QString(R"(SELECT `answer` FROM `answer` WHERE `id` = ?)");

const auto SELECT_BY_ANSWER = QString(R"(SELECT `id` FROM `answer` WHERE `answer` = ?)");

const auto SELECT_ALL_IDS = QString(R"(SELECT `id` FROM `answer`)");

Answer Answer::create(QString answer)
{
    Answer newAnswer;

    newAnswer.mAnswer = answer;

    QSqlQuery query(Singleton::database().database());

    query.prepare(CREATE);
    query.addBindValue(answer);

    if (!query.exec())
    {
        qDebug() << query.lastError();
        return Answer();
    }

    newAnswer.mId      = query.lastInsertId().toInt();
    newAnswer.mIsValid = true;

    return newAnswer;
}

QList<int> Answer::getIds()
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

Answer Answer::getById(int id)
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_BY_ID);
    query.addBindValue(id);
    query.exec();

    if (!query.next())
        return Answer();

    Answer answer;
    answer.mId     = id;
    answer.mAnswer = query.value(0).toString();
    answer.mIsValid = true;

    return answer;
}

Answer Answer::getByAnswer(QString answer)
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_BY_ANSWER);
    query.addBindValue(answer);
    query.exec();

    if (!query.next())
        return Answer();

    return getById(query.value(0).toInt());
}

int Answer::id() const
{
    return mId;
}

bool Answer::isValid() const
{
    return mIsValid;
}

QString Answer::answer() const
{
    return mAnswer;
}

void Answer::setAnswer(const QString &answer)
{
    mAnswer = answer;
}

bool Answer::update()
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(UPDATE);
    query.addBindValue(mAnswer);
    query.addBindValue(mId);

    bool result = query.exec();

    if (!result)
        qDebug() << query.lastError().text();

    return result;
}
