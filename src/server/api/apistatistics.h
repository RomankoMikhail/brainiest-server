#ifndef APISTATISTICS_H
#define APISTATISTICS_H

#include "../httprequest.h"
#include "../httpresponse.h"

void onStatisticsMostFailed(const HttpRequest &request, HttpResponse &response);

void onStatisticsWon(const HttpRequest &request, HttpResponse &response);
void onStatisticsMostUsed(const HttpRequest &request, HttpResponse &response);
void onStatisticsTotalGames(const HttpRequest &request, HttpResponse &response);

#endif // APISTATISTICS_H
