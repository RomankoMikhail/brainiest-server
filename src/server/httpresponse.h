#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <QTcpSocket>
#include <QBuffer>
#include "cookie.hpp"

class HttpResponse
{
public:
    enum Code
    {
        CodeSwitchingProtocols  = 101, //!<
        CodeOk                  = 200, //!<
        CodeNotModified         = 304, //!<
        CodeBadRequest          = 400, //!<
        CodeForbidden           = 403, //!<
        CodeNotFound            = 404, //!<
        CodeRequestTimeout      = 408, //!<
        CodeLengthRequired      = 411, //!<
        CodePayloadTooLarge     = 413, //!<
        CodeUriTooLong          = 414, //!<
        CodeInternalServerError = 500, //!<
        CodeNotImplemented      = 501, //!<
        CodeServiceUnavailable  = 503, //!<
        CodeVersionNotSupported = 505  //!<
    };


    void write(const QByteArray &array);
    void flush(QTcpSocket *socket);

    HttpResponse();
    Code statusCode() const;
    void setStatusCode(Code statusCode);

    QMultiMap<QString, QString> headers() const;
    void setHeaders(const QMultiMap<QString, QString> &headers);
    void addHeader(const QString header, const QString value);

    QByteArray data() const;
    void setData(const QByteArray &data);

    QList<Cookie> cookies() const;
    void setCookies(const QList<Cookie> &cookies);
    void addCookie(const Cookie &cookie);

private:
    QMultiMap<QString, QString> mHeaders;
    QByteArray mData;
    Code mStatusCode;
    QList<Cookie> mCookies;
};

#endif // HTTPRESPONSE_H
