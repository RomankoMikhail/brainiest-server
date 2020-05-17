#include "apigame.h"

#include "../api.h"
#include "../models/game.h"
#include "../singleton.hpp"
#include "token.h"

#include <QJsonArray>

void onGameAdd(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
}

void onGameList(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
}

void onGameListOpen(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
}

void onGameUpdate(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
}

void onGameJoin(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
}

void onGameInfo(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
}

void onGameAnswer(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
}
