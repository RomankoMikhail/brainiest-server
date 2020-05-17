#include "apiciphers.h"

#include "../api.h"
#include "../models/ciphers.h"
#include "../singleton.hpp"
#include "token.h"

#include <QJsonArray>

void onCiphersList(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();

    auto ids = Ciphers::getIds();

    QJsonObject object;
    QJsonArray array;

    for (const auto &id : ids)
    {
        QJsonObject item;

        Ciphers cipher = Ciphers::getById(id);

        item["id"]   = cipher.id();
        item["word"] = cipher.word();

        array.append(item);
    }

    object["items"] = array;
    response.setData(formResponse(object), "application/json");
}

void onCiphersAdd(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_STRING(word);

    REQUIRE_TOKEN();

    Ciphers cipher = Ciphers::create(word);

    if (!cipher.isValid())
    {
        response.setData(formError(GeneralError), "application/json");
        return;
    }

    response.setData(formResponse(), "application/json");
}

void onCiphersUpdate(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_STRING(word);
    REQUIRE_INT(id);

    REQUIRE_TOKEN();

    Ciphers cipher = Ciphers::getById(id);

    if (!cipher.isValid())
    {
        response.setData(formError(NotFound), "application/json");
        return;
    }

    cipher.setWord(word);

    if (!cipher.update())
    {
        response.setData(formError(GeneralError), "application/json");
        return;
    }

    response.setData(formResponse(), "application/json");
}

void onCiphersInfo(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_INT(id);

    REQUIRE_TOKEN();

    Ciphers answer = Ciphers::getById(id);

    if (!answer.isValid())
    {
        response.setData(formError(NotFound), "application/json");
        return;
    }

    QJsonObject object;

    object["word"] = answer.word();

    response.setData(formResponse(object), "application/json");
}
