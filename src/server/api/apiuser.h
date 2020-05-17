#ifndef APIUSER_H
#define APIUSER_H

#include "../httprequest.h"
#include "../httpresponse.h"

void onUserChangePassword(const HttpRequest &request, HttpResponse &response);
void onUserInfo(const HttpRequest &request, HttpResponse &response);
void onUserList(const HttpRequest &request, HttpResponse &response);
void onUserLogin(const HttpRequest &request, HttpResponse &response);
void onUserRegister(const HttpRequest &request, HttpResponse &response);
void onUserUpdate(const HttpRequest &request, HttpResponse &response);
void onTokenCheck(const HttpRequest &request, HttpResponse &response);

#endif // APIUSER_H
