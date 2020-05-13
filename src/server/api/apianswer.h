#ifndef APIANSWER_H
#define APIANSWER_H

#include "../httprequest.h"
#include "../httpresponse.h"

void onAnswerList(const HttpRequest &request, HttpResponse &response);
void onAnswerAdd(const HttpRequest &request, HttpResponse &response);
void onAnswerUpdate(const HttpRequest &request, HttpResponse &response);
void onAnswerInfo(const HttpRequest &request, HttpResponse &response);
void onAnswerListQuestions(const HttpRequest &request, HttpResponse &response);

#endif // APIANSWER_H
