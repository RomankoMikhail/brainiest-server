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

    SEND_RESPONSE(object);
}

void onCiphersAdd(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_STRING(word);

    REQUIRE_TOKEN();

    Ciphers cipher = Ciphers::create(word);

    if (!cipher.isValid())
        SEND_ERROR(GeneralError);

    QJsonObject item;
	item["id"] = cipher.id();
	
    SEND_RESPONSE(item);
}

void onCiphersUpdate(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_STRING(word);
    REQUIRE_INT(id);

    REQUIRE_TOKEN();

    Ciphers cipher = Ciphers::getById(id);

    if (!cipher.isValid())
        SEND_ERROR(NotFound);

    cipher.setWord(word);

    if (!cipher.update())
        SEND_ERROR(GeneralError);

    SEND_RESPONSE();
}

void onCiphersInfo(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_INT(id);

    REQUIRE_TOKEN();

    Ciphers answer = Ciphers::getById(id);

    if (!answer.isValid())
        SEND_ERROR(NotFound);

    QJsonObject object;

    object["word"] = answer.word();

    SEND_RESPONSE(object);
}
