#include "apiquestion.h"

#include "../api.h"
#include "../models/question.h"
#include "../models/questionhasanswer.h"
#include "../singleton.hpp"
#include "token.h"

#include <QJsonArray>

void onQuestionList(const HttpRequest &request, HttpResponse &response)
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
    if (!request.arguments().contains("theme") || !request.arguments().contains("question") ||
        !request.arguments().contains("type"))
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

    QString theme        = request.arguments().value("theme");
    QString questionText = request.arguments().value("question");
    int type             = request.arguments().value("type").toInt();

    Question question = Question::create(theme, questionText, type);

    if (!question.isValid())
    {
        response.setData(formError(GeneralError), "application/json");
        return;
    }

    response.setData(formResponse(), "application/json");
}

void onQuestionUpdate(const HttpRequest &request, HttpResponse &response)
{

    if (!request.arguments().contains("theme") || !request.arguments().contains("question") ||
        !request.arguments().contains("type") || !request.arguments().contains("id"))
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

    QString theme        = request.arguments().value("theme");
    QString questionText = request.arguments().value("question");
    int type             = request.arguments().value("type").toInt();
    int id               = request.arguments().value("id").toInt();

    Question question = Question::getById(id);

    if (!question.isValid())
    {
        response.setData(formError(NotFound), "application/json");
        return;
    }

    question.setType(type);
    question.setTheme(theme);
    question.setQuestion(questionText);

    if (!question.update())
    {
        response.setData(formError(GeneralError), "application/json");
        return;
    }

    response.setData(formResponse(), "application/json");
}

void onQuestionInfo(const HttpRequest &request, HttpResponse &response)
{
    if (!request.arguments().contains("id"))
    {
        response.setData(formError(MissingParameter), "application/json");
        return;
    }

    int id = request.arguments().value("id").toInt();

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
    if (!request.arguments().contains("id") || !request.arguments().contains("answerId") ||
        !request.arguments().contains("correct"))
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

    int id       = request.arguments().value("id").toInt();
    int answerId = request.arguments().value("answerId").toInt();
    int correct  = request.arguments().value("correct").toInt();

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
    if (!request.arguments().contains("id") || !request.arguments().contains("answerId"))
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

    int id       = request.arguments().value("id").toInt();
    int answerId = request.arguments().value("answerId").toInt();

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
    if (!request.arguments().contains("id") || !request.arguments().contains("answerId") ||
        !request.arguments().contains("correct"))
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

    int id       = request.arguments().value("id").toInt();
    int answerId = request.arguments().value("answerId").toInt();
    int correct = request.arguments().value("correct").toInt();

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
