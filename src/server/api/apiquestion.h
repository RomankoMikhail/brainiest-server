#ifndef APIQUESTION_H
#define APIQUESTION_H

#include "../httprequest.h"
#include "../httpresponse.h"

void onQuestionAdd(const HttpRequest &request, HttpResponse &response);


void onQuestionAddAnswer(const HttpRequest &request, HttpResponse &response);
void onQuestionInfo(const HttpRequest &request, HttpResponse &response);
void onQuestionList(const HttpRequest &request, HttpResponse &response);
void onQuestionListDetailed(const HttpRequest &request, HttpResponse &response);
void onQuestionListAnswers(const HttpRequest &request, HttpResponse &response);
void onQuestionRemoveAnswer(const HttpRequest &request, HttpResponse &response);
void onQuestionThemes(const HttpRequest &request, HttpResponse &response);
void onQuestionUpdate(const HttpRequest &request, HttpResponse &response);
void onQuestionUpdateAnswer(const HttpRequest &request, HttpResponse &response);
void onQuestionUpdateCorrectAnswer(const HttpRequest &request, HttpResponse &response);
void onQuestionListTheme(const HttpRequest &request, HttpResponse &response);

void onQuestionAddBatch(const HttpRequest &request, HttpResponse &response);
void onQuestionRemove(const HttpRequest &request, HttpResponse &response);

#endif // APIQUESTION_H
