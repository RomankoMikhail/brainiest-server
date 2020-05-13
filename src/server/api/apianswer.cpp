#include "apiquestion.h"
#include "../models/questionhasanswer.h"

#include "../api.h"
#include "../models/answer.h"
#include "../singleton.hpp"
#include "token.h"

#include <QJsonArray>

void onAnswerList(const HttpRequest &request, HttpResponse &response)
{
    if (!request.arguments().contains("token"))
    {
        response.setData(formError(TokenRequired), "application/json");
        return;
    }

    QString token = request.arguments().value("token");

    if (!Singleton::tokens().contains(token))
    {
        response.setData(formError(InvalidToken), "application/json");
        return;
    }

    auto ids = Answer::getIds();

    QJsonObject object;
    QJsonArray array;

    for (const auto &id : ids)
    {
        QJsonObject item;

        Answer answer = Answer::getById(id);

        item["id"]     = answer.id();
        item["answer"] = answer.answer();

        array.append(item);
    }

    object["items"] = array;
    response.setData(formResponse(object), "application/json");
}

void onAnswerAdd(const HttpRequest &request, HttpResponse &response)
{
    if (!request.arguments().contains("answer"))
    {
        response.setData(formError(MissingParameter), "application/json");
        return;
    }

    if (!request.arguments().contains("token"))
    {
        response.setData(formError(TokenRequired), "application/json");
        return;
    }

    QString token = request.arguments().value("token");

    if (!Singleton::tokens().contains(token))
    {
        response.setData(formError(InvalidToken), "application/json");
        return;
    }

    QString answerText = request.arguments().value("answer");

    Answer answer = Answer::create(answerText);

    if (!answer.isValid())
    {
        response.setData(formError(GeneralError), "application/json");
        return;
    }

    response.setData(formResponse(), "application/json");
}

void onAnswerUpdate(const HttpRequest &request, HttpResponse &response)
{
    if (!request.arguments().contains("answer") || !request.arguments().contains("id"))
    {
        response.setData(formError(MissingParameter), "application/json");
        return;
    }

    if (!request.arguments().contains("token"))
    {
        response.setData(formError(TokenRequired), "application/json");
        return;
    }

    QString token = request.arguments().value("token");

    if (!Singleton::tokens().contains(token))
    {
        response.setData(formError(InvalidToken), "application/json");
        return;
    }

    QString answerText = request.arguments().value("answer");
    int id             = request.arguments().value("id").toInt();

    Answer answer = Answer::getById(id);

    if (!answer.isValid())
    {
        response.setData(formError(NotFound), "application/json");
        return;
    }

    answer.setAnswer(answerText);

    if (!answer.update())
    {
        response.setData(formError(GeneralError), "application/json");
        return;
    }

    response.setData(formResponse(), "application/json");
}

void onAnswerInfo(const HttpRequest &request, HttpResponse &response)
{
    if (!request.arguments().contains("id"))
    {
        response.setData(formError(MissingParameter), "application/json");
        return;
    }

    int id = request.arguments().value("id").toInt();

    Answer answer = Answer::getById(id);

    if (!answer.isValid())
    {
        response.setData(formError(NotFound), "application/json");
        return;
    }

    QJsonObject object;

    object["answer"] = answer.answer();

    response.setData(formResponse(object), "application/json");
}

void onAnswerListQuestions(const HttpRequest &request, HttpResponse &response)
{
    if (!request.arguments().contains("id"))
    {
        response.setData(formError(MissingParameter), "application/json");
        return;
    }

    if (!request.arguments().contains("token"))
    {
        response.setData(formError(TokenRequired), "application/json");
        return;
    }

    QString token = request.arguments().value("token");

    if (!Singleton::tokens().contains(token))
    {
        response.setData(formError(InvalidToken), "application/json");
        return;
    }

    int id = request.arguments().value("id").toInt();

    QList<int> ids = QuestionHasAnswer::getQuestionIds(id);

    QJsonArray array;

    for(const auto &id : ids)
    {
        array.append(id);
    }

    QJsonObject object;

    object["items"] = array;

    response.setData(formResponse(object), "application/json");
}
