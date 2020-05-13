#ifndef APIQUESTION_H
#define APIQUESTION_H

#include "../httprequest.h"
#include "../httpresponse.h"

void onQuestionList(const HttpRequest &request, HttpResponse &response);
void onQuestionAdd(const HttpRequest &request, HttpResponse &response);
void onQuestionUpdate(const HttpRequest &request, HttpResponse &response);
void onQuestionInfo(const HttpRequest &request, HttpResponse &response);
void onQuestionThemes(const HttpRequest &request, HttpResponse &response);
void onQuestionListAnswers(const HttpRequest &request, HttpResponse &response);
void onQuestionUpdateAnswer(const HttpRequest &request, HttpResponse &response);
void onQuestionAddAnswer(const HttpRequest &request, HttpResponse &response);
void onQuestionRemoveAnswer(const HttpRequest &request, HttpResponse &response);

#endif // APIQUESTION_H
