#include "questionhasanswer.h"

#include "../singleton.hpp"
#include <QList>

const auto CREATE = QString(
    R"(INSERT INTO `question_has_answer`(`questionid`, `answerid`, `correct`) VALUES(?, ?, ?))");

const auto UPDATE = QString(
    R"(UPDATE `question_has_answer` SET `correct` = ? WHERE `questionid` = ? and `answerid` = ?)");

const auto SELECT_BY_ID = QString(
    R"(SELECT `correct` FROM `question_has_answer` WHERE `questionid` = ? and `answerid` = ?)");

const auto SELECT_QUESTION_BY_ANSWER =
    QString(R"(SELECT `questionid` FROM `question_has_answer` WHERE `answerid` = ?)");

const auto SELECT_ANSWER_BY_QUESTION =
    QString(R"(SELECT `answerid` FROM `question_has_answer` WHERE `questionid` = ?)");

const auto DELETE =
    QString(R"(DELETE FROM `question_has_answer` WHERE `questionid` = ? and `answerid` = ?)");

QuestionHasAnswer QuestionHasAnswer::create(int questionId, int answerId, int correct)
{
    QuestionHasAnswer newQuestionHasAnswer;

    newQuestionHasAnswer.mQuestionId = questionId;
    newQuestionHasAnswer.mAnswerId   = answerId;
    newQuestionHasAnswer.mCorrect    = correct;

    QSqlQuery query(Singleton::database().database());

    query.prepare(CREATE);
    query.addBindValue(questionId);
    query.addBindValue(answerId);
    query.addBindValue(correct);

    if (!query.exec())
    {
        qDebug() << query.lastError();
        return QuestionHasAnswer();
    }

    newQuestionHasAnswer.mIsValid = true;

    return newQuestionHasAnswer;
}

QuestionHasAnswer QuestionHasAnswer::getById(int questionId, int answerId)
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_BY_ID);
    query.addBindValue(questionId);
    query.addBindValue(answerId);
    query.exec();

    if (!query.next())
        return QuestionHasAnswer();

    QuestionHasAnswer questionHasAnswer;
    questionHasAnswer.mQuestionId = questionId;
    questionHasAnswer.mAnswerId   = answerId;
    questionHasAnswer.mCorrect    = query.value(0).toInt();
    questionHasAnswer.mIsValid    = true;

    return questionHasAnswer;
}

QList<int> QuestionHasAnswer::getAnswerIds(int questionId)
{
    QList<int> ids;

    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_ANSWER_BY_QUESTION);
    query.addBindValue(questionId);

    query.exec();

    while (query.next())
    {
        ids.push_back(query.value(0).toInt());
    }

    return ids;
}

QList<int> QuestionHasAnswer::getQuestionIds(int answerId)
{
    QList<int> ids;

    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_QUESTION_BY_ANSWER);
    query.addBindValue(answerId);

    query.exec();

    while (query.next())
    {
        ids.push_back(query.value(0).toInt());
    }

    return ids;
}

bool QuestionHasAnswer::isValid() const
{
    return mIsValid;
}

int QuestionHasAnswer::questionId() const
{
    return mQuestionId;
}

int QuestionHasAnswer::answerId() const
{
    return mAnswerId;
}

int QuestionHasAnswer::correct() const
{
    return mCorrect;
}

void QuestionHasAnswer::setCorrect(int correct)
{
    mCorrect = correct;
}

bool QuestionHasAnswer::update()
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(UPDATE);
    query.addBindValue(mCorrect);
    query.addBindValue(mQuestionId);
    query.addBindValue(mAnswerId);

    bool result = query.exec();

    if (!result)
        qDebug() << query.lastError().text();

    return result;
}

bool QuestionHasAnswer::remove()
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(DELETE);
    query.addBindValue(mQuestionId);
    query.addBindValue(mAnswerId);

    if (!query.exec())
    {
        qDebug() << query.lastError().text();
        return false;
    }

    mIsValid = false;

    return true;
}
