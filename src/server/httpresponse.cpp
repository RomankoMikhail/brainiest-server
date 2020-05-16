#include "httpresponse.h"
#include <QByteArray>
#include <QString>

void HttpResponse::write(const QByteArray &array)
{
    mData.append(array);
}

void HttpResponse::flush(QTcpSocket *socket)
{
    socket->write(
        QString("HTTP/1.1 " + QString::number(statusCode()) + " " + statusCodeString() + "\r\n").toUtf8());

    addHeader("Content-Length", QString::number(mData.size()));

    for (const auto &header : mHeaders.keys())
    {
        for (const auto &value : mHeaders.values(header))
        {
            socket->write(header.toUtf8());
            socket->write(": ");
            socket->write(value.toUtf8());
            socket->write("\r\n");
        }
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

QString HttpResponse::statusCodeString() const
{
    switch (mStatusCode)
    {
    case CodeSwitchingProtocols:
        return "Switching Protocols";
    case CodeNotModified:
        return "Not Modified";
    case CodeBadRequest:
        return "Bad Request";
    case CodeForbidden:
        return "Forbidden";
    case CodeNotFound:
        return "Not Found";
    case CodeRequestTimeout:
        return "Request Timeout";
    case CodeLengthRequired:
        return "Length Required";
    case CodePayloadTooLarge:
        return "Payload Too Large";
    case CodeUriTooLong:
        return "URI Too Long";
    case CodeInternalServerError:
        return "Internal Server Error";
    case CodeNotImplemented:
        return "Not Implemented";
    case CodeServiceUnavailable:
        return "Service Unavailable";
    case CodeVersionNotSupported:
        return "Version Not Supported";

    case CodeOk:
    default:
        return "Ok";
    }
}

QMultiMap<QString, QString> HttpResponse::headers() const
{
    return mHeaders;
}

void HttpResponse::setHeaders(const QMultiMap<QString, QString> &headers)
{
    mHeaders = headers;
}

void HttpResponse::addHeader(const QString &header, const QString &value)
{
    mHeaders.insert(header, value.trimmed());
}

QByteArray HttpResponse::data() const
{
    return mData;
}

void HttpResponse::setData(const QByteArray &data)
{
    mData = data;
}

void HttpResponse::setData(const QByteArray &data, const QString &mimeType)
{
    setData(data);
    mHeaders.remove("Content-Type");
    addHeader("Content-Type", mimeType);
}
