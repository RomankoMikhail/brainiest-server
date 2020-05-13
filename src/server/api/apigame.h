#ifndef APIGAME_H
#define APIGAME_H

#include "../httprequest.h"
#include "../httpresponse.h"

void onGameAdd(const HttpRequest &request, HttpResponse &response);
void onGameList(const HttpRequest &request, HttpResponse &response);
void onGameListOpen(const HttpRequest &request, HttpResponse &response);
void onGameListComplete(const HttpRequest &request, HttpResponse &response);

#endif // APIGAME_H
