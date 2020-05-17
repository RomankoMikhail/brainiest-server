#include "apiquestion.h"

#include "../api.h"
#include "../models/question.h"
#include "../models/questionhasanswer.h"
#include "../models/answer.h"
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
    response.setData(formResponse(object), "application/json");
}

void onQuestionAdd(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_STRING(theme);
    REQUIRE_STRING(question);
    REQUIRE_INT(type);

    REQUIRE_TOKEN();

    Question questionElement = Question::create(theme, question, type);

    if (!questionElement.isValid())
    {
        response.setData(formError(GeneralError), "application/json");
        return;
    }

    response.setData(formResponse(), "application/json");
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
    {
        response.setData(formError(NotFound), "application/json");
        return;
    }

    questionElement.setType(type);
    questionElement.setTheme(theme);
    questionElement.setQuestion(question);

    if (!questionElement.update())
    {
        response.setData(formError(GeneralError), "application/json");
        return;
    }

    response.setData(formResponse(), "application/json");
}

void onQuestionInfo(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_INT(id);

    REQUIRE_TOKEN();

    Question question = Question::getById(id);

    if (!question.isValid())
    {
        response.setData(formError(NotFound), "application/json");
        return;
    }

    QJsonObject object;

    object["theme"]    = question.theme();
    object["question"] = question.question();
    object["type"]     = question.type();

    response.setData(formResponse(object), "application/json");
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

    response.setData(formResponse(object), "application/json");
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

    response.setData(formResponse(object), "application/json");
}

void onQuestionAddAnswer(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_INT(id);
    REQUIRE_INT(answerId);
    REQUIRE_INT(correct);

    REQUIRE_TOKEN();

    auto questionHasAnswer = QuestionHasAnswer::create(id, answerId, correct);

    if (!questionHasAnswer.isValid())
    {
        response.setData(formError(GeneralError), "application/json");
        return;
    }

    response.setData(formResponse(), "application/json");
}

void onQuestionRemoveAnswer(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_INT(id);
    REQUIRE_INT(answerId);

    REQUIRE_TOKEN();

    auto questionHasAnswer = QuestionHasAnswer::getById(id, answerId);

    if (!questionHasAnswer.isValid())
    {
        response.setData(formError(NotFound), "application/json");
        return;
    }

    if (!questionHasAnswer.remove())
    {
        response.setData(formError(GeneralError), "application/json");
        return;
    }

    response.setData(formResponse(), "application/json");
}

void onQuestionUpdateAnswer(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_INT(id);
    REQUIRE_INT(answerId);
    REQUIRE_INT(correct);

    REQUIRE_TOKEN();

    auto questionHasAnswer = QuestionHasAnswer::getById(id, answerId);

    if (!questionHasAnswer.isValid())
    {
        response.setData(formError(NotFound), "application/json");
        return;
    }

    questionHasAnswer.setCorrect(correct);

    if (!questionHasAnswer.update())
    {
        response.setData(formError(GeneralError), "application/json");
        return;
    }

    response.setData(formResponse(), "application/json");
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
        for(const auto &answerId : answersIds)
        {
            QJsonObject answerObject;

            Answer answer = Answer::getById(answerId);

            answerObject["id"] = answer.id();
            answerObject["answer"] = answer.answer();

            answers.append(answerObject);
        }

        item["answers"] = answers;

        array.append(item);
    }

    object["items"] = array;
    response.setData(formResponse(object), "application/json");
}
