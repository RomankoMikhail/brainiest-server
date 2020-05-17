#include "apiquestion.h"

#include "../api.h"
#include "../models/answer.h"
#include "../models/question.h"
#include "../models/questionhasanswer.h"
#include "../singleton.hpp"
#include "token.h"

#include <QJsonArray>

void onQuestionList(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();

    auto ids = Question::getIds();

    QJsonObject object;
    QJsonArray array;

    for (const auto &id : ids)
    {
        QJsonObject item;

        Question question = Question::getById(id);

        item["id"]       = question.id();
        item["theme"]    = question.theme();
        item["question"] = question.question();
        item["type"]     = question.type();

        array.append(item);
    }

    object["items"] = array;
    SEND_RESPONSE(object);
}

void onQuestionAdd(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_STRING(theme);
    REQUIRE_STRING(question);
    REQUIRE_INT(type);

    REQUIRE_TOKEN();

    Question questionElement = Question::create(theme, question, type);

    if (!questionElement.isValid())
        SEND_ERROR(GeneralError);

    SEND_RESPONSE();
}

void onQuestionUpdate(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_STRING(theme);
    REQUIRE_STRING(question);
    REQUIRE_INT(type);
    REQUIRE_INT(id);

    REQUIRE_TOKEN();

    Question questionElement = Question::getById(id);

    if (!questionElement.isValid())
        SEND_ERROR(NotFound);

    questionElement.setType(type);
    questionElement.setTheme(theme);
    questionElement.setQuestion(question);

    if (!questionElement.update())
        SEND_ERROR(GeneralError);

    SEND_RESPONSE();
}

void onQuestionInfo(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_INT(id);

    REQUIRE_TOKEN();

    Question question = Question::getById(id);

    if (!question.isValid())
        SEND_ERROR(NotFound);

    QJsonObject object;

    object["theme"]    = question.theme();
    object["question"] = question.question();
    object["type"]     = question.type();

    SEND_RESPONSE(object);
}

void onQuestionThemes(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();

    auto themes = Question::getThemes();

    QJsonObject object;
    QJsonArray array;

    for (const auto &theme : themes)
    {
        array.append(theme);
    }
    object["items"] = array;

    SEND_RESPONSE(object);
}

void onQuestionListAnswers(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_INT(id);
    REQUIRE_TOKEN();

    QList<int> ids = QuestionHasAnswer::getAnswerIds(id);

    QJsonArray array;

    for (const auto &id : ids)
    {
        array.append(id);
    }

    QJsonObject object;

    object["items"] = array;

    SEND_RESPONSE(object);
}

void onQuestionAddAnswer(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_INT(id);
    REQUIRE_INT(answerId);
    REQUIRE_INT(correct);

    REQUIRE_TOKEN();

    auto questionHasAnswer = QuestionHasAnswer::create(id, answerId, correct);

    if (!questionHasAnswer.isValid())
        SEND_ERROR(GeneralError);

    SEND_RESPONSE();
}

void onQuestionRemoveAnswer(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_INT(id);
    REQUIRE_INT(answerId);

    REQUIRE_TOKEN();

    auto questionHasAnswer = QuestionHasAnswer::getById(id, answerId);

    if (!questionHasAnswer.isValid())
        SEND_ERROR(NotFound);

    if (!questionHasAnswer.remove())
        SEND_ERROR(GeneralError);

    SEND_RESPONSE();
}

void onQuestionUpdateAnswer(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_INT(id);
    REQUIRE_INT(answerId);
    REQUIRE_INT(correct);

    REQUIRE_TOKEN();

    auto questionHasAnswer = QuestionHasAnswer::getById(id, answerId);

    if (!questionHasAnswer.isValid())
        SEND_ERROR(NotFound);

    questionHasAnswer.setCorrect(correct);

    if (!questionHasAnswer.update())
        SEND_ERROR(GeneralError);

    SEND_RESPONSE();
}

void onQuestionListDetailed(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();

    auto ids = Question::getIds();

    QJsonObject object;
    QJsonArray array;

    for (const auto &id : ids)
    {
        QJsonObject item;

        Question question = Question::getById(id);

        item["id"]       = question.id();
        item["theme"]    = question.theme();
        item["question"] = question.question();
        item["type"]     = question.type();
        QJsonArray answers;

        auto answersIds = QuestionHasAnswer::getAnswerIds(question.id());
        for (const auto &answerId : answersIds)
        {
            QJsonObject answerObject;

            Answer answer = Answer::getById(answerId);

            answerObject["id"]     = answer.id();
            answerObject["answer"] = answer.answer();
            answerObject["correct"] = QuestionHasAnswer::getById(question.id(), answer.id()).correct();

            answers.append(answerObject);
        }

        item["answers"] = answers;

        array.append(item);
    }

    object["items"] = array;
    SEND_RESPONSE(object);
}
