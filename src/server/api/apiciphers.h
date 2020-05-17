#ifndef APICIPHERS_H
#define APICIPHERS_H

#include "../httprequest.h"
#include "../httpresponse.h"

void onCiphersAdd(const HttpRequest &request, HttpResponse &response);
void onCiphersInfo(const HttpRequest &request, HttpResponse &response);
void onCiphersList(const HttpRequest &request, HttpResponse &response);
void onCiphersUpdate(const HttpRequest &request, HttpResponse &response);

#endif // APICIPHERS_H
