#include "apistatistics.h"
#include "../singleton.hpp"
#include "../api.h"

void onStatisticsMostFailed(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
    REQUIRE_INT(id);

}

void onStatisticsMostUsed(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
}

void onStatisticsTotalGames(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
}

void onStatisticsWon(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();
}
