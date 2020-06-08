#include "apiquestion.h"
#include "../models/questionhasanswer.h"

#include "../api.h"
#include "../models/answer.h"
#include "../singleton.hpp"
#include "token.h"

#include <QJsonArray>

void onAnswerList(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();

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

    SEND_RESPONSE(object);
}

void onAnswerAdd(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_STRING(answer);
    REQUIRE_TOKEN();
	
    Answer answerElement = Answer::create(answer);

    if (!answerElement.isValid())
        SEND_ERROR(GeneralError);
	
	QJsonObject item;
	item["id"] = answerElement.id();
	
    SEND_RESPONSE(item);
}

void onAnswerUpdate(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_STRING(answer);
    REQUIRE_INT(id);

    REQUIRE_TOKEN();

    Answer answerElement = Answer::getById(id);

    if (!answerElement.isValid())
        SEND_ERROR(NotFound);

    answerElement.setAnswer(answer);

    if (!answerElement.update())
        SEND_ERROR(GeneralError);

    SEND_RESPONSE();
}

void onAnswerInfo(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_INT(id);
    REQUIRE_TOKEN();

    Answer answerElement = Answer::getById(id);

    if (!answerElement.isValid())
        SEND_ERROR(NotFound);

    QJsonObject object;

    object["answer"] = answerElement.answer();

    SEND_RESPONSE(object);
}

void onAnswerListQuestions(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_INT(id);
    REQUIRE_TOKEN();

    QList<int> ids = QuestionHasAnswer::getQuestionIds(id);

    QJsonArray array;

    for(const auto &id : ids)
    {
        array.append(id);
    }

    QJsonObject object;

    object["items"] = array;

    SEND_RESPONSE(object);
}
