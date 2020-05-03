#include "httpresponse.h"
#include <QString>
#include <QByteArray>

HttpResponse::HttpResponse()
{

}

void HttpResponse::write(const QByteArray &array)
{
    mData.append(array);
}

void HttpResponse::flush(QTcpSocket *socket)
{
    socket->write(QString("HTTP/1.1 " + QString::number(statusCode()) + "\r\n").toUtf8());

    addHeader("content-length", QString::number(mData.size()));

    for(const auto &header : mHeaders.keys())
    {
        for(const auto &value : mHeaders.values(header))
        {
            socket->write(QString(header + ": " + value + "\r\n").toUtf8());
        }
    }

    for(const auto &cookie : mCookies)
    {
        socket->write(cookie.toField().toUtf8());
    }

    socket->write("\r\n");

    socket->write(mData);
}

HttpResponse::Code HttpResponse::statusCode() const
{
    return mStatusCode;
}

void HttpResponse::setStatusCode(Code statusCode)
{
    mStatusCode = statusCode;
}

QMultiMap<QString, QString> HttpResponse::headers() const
{
    return mHeaders;
}

void HttpResponse::setHeaders(const QMultiMap<QString, QString> &headers)
{
    mHeaders = headers;
}

void HttpResponse::addHeader(const QString header, const QString value)
{
    mHeaders.insert(header.toLower(), value.trimmed());
}

QByteArray HttpResponse::data() const
{
    return mData;
}

void HttpResponse::setData(const QByteArray &data)
{
    mData = data;
}

QList<Cookie> HttpResponse::cookies() const
{
    return mCookies;
}

void HttpResponse::setCookies(const QList<Cookie> &cookies)
{
    mCookies = cookies;
}

void HttpResponse::addCookie(const Cookie &cookie)
{
    mCookies.append(cookie);
}
