#ifndef APIGAME_H
#define APIGAME_H

#include "../httprequest.h"
#include "../httpresponse.h"

void onGameAdd(const HttpRequest &request, HttpResponse &response);
void onGameUpdate(const HttpRequest &request, HttpResponse &response);
void onGameJoin(const HttpRequest &request, HttpResponse &response);
void onGameInfo(const HttpRequest &request, HttpResponse &response);
void onGameAnswer(const HttpRequest &request, HttpResponse &response);
void onGameList(const HttpRequest &request, HttpResponse &response);
void onGameListOpen(const HttpRequest &request, HttpResponse &response);
void onGameDebug(const HttpRequest &request, HttpResponse &response);
void onGameSkip(const HttpRequest &request, HttpResponse &response);
void onGameStart(const HttpRequest &request, HttpResponse &response);
void onGameStatus(const HttpRequest &request, HttpResponse &response);
#endif // APIGAME_H
