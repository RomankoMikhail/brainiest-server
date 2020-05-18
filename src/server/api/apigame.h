#ifndef APIGAME_H
#define APIGAME_H

#include "../httprequest.h"
#include "../httpresponse.h"

void onGameAnswerRound1(const HttpRequest &request, HttpResponse &response);
void onGameAnswerRound2(const HttpRequest &request, HttpResponse &response);
void onGameAnswerRound3(const HttpRequest &request, HttpResponse &response);

void onGameSelectRound2(const HttpRequest &request, HttpResponse &response);
void onGameSelectRound3(const HttpRequest &request, HttpResponse &response);
void onGameDecrypt(const HttpRequest &request, HttpResponse &response);

void onGameAdd(const HttpRequest &request, HttpResponse &response);
void onGameAddQuestion(const HttpRequest &request, HttpResponse &response);
void onGameAddCipher(const HttpRequest &request, HttpResponse &response);
void onGameRemoveQuestion(const HttpRequest &request, HttpResponse &response);
void onGameRemoveCipher(const HttpRequest &request, HttpResponse &response);

void onGameDebug(const HttpRequest &request, HttpResponse &response);

void onGameInfo(const HttpRequest &request, HttpResponse &response);
void onGameJoin(const HttpRequest &request, HttpResponse &response);
void onGameList(const HttpRequest &request, HttpResponse &response);
void onGameListOpen(const HttpRequest &request, HttpResponse &response);
void onGameUpdate(const HttpRequest &request, HttpResponse &response);
void onGameQuit(const HttpRequest &request, HttpResponse &response);

void onGameStart(const HttpRequest &request, HttpResponse &response);
void onGameStatus(const HttpRequest &request, HttpResponse &response);


#endif // APIGAME_H
