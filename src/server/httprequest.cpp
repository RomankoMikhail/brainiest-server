#include "httprequest.h"
#include <QBuffer>
#include <QDebug>
#include <QRegularExpression>
#include <QUrl>

QHostAddress HttpRequest::hostAddress() const
{
    return mHostAddress;
}

void HttpRequest::setHostAddress(const QHostAddress &hostAddress)
{
    mHostAddress = hostAddress;
}

QString HttpRequest::path() const
{
    return mPath;
}

void HttpRequest::setPath(const QString &path)
{
    mPath = path;
    parseGetArguments();
}

void HttpRequest::parseGetArguments()
{
    QStringList splitedUrl = mPath.split(QRegExp("[?&]"));

    if (splitedUrl.size() == 0)
        return;

    mPath = QUrl::fromPercentEncoding(splitedUrl[0].toUtf8());

    for (auto arg = splitedUrl.begin() + 1; arg != splitedUrl.end(); ++arg)
    {
        QStringList pair = arg->split("=");

        if (pair.size() >= 2)
            mArguments[QUrl::fromPercentEncoding(pair[0].toUtf8())] = QUrl::fromPercentEncoding(pair[1].toUtf8());
        else if (pair.size() >= 1)
            mArguments[QUrl::fromPercentEncoding(pair[0].toUtf8())] = true;
    }
}

QString HttpRequest::method() const
{
    return mMethod;
}

void HttpRequest::setMethod(const QString &method)
{
    mMethod = method;
}

int HttpRequest::protocolVersion() const
{
    return mProtocolVersion;
}

void HttpRequest::setProtocolVersion(int protocolVersion)
{
    mProtocolVersion = protocolVersion;
}

QMultiMap<QString, QString> HttpRequest::headers() const
{
    return mHeaders;
}

void HttpRequest::addHeader(const QString &header, const QString &value)
{
    mHeaders.insert(header.toLower(), value.trimmed());
}

QByteArray HttpRequest::data() const
{
    return mData;
}

void HttpRequest::setData(const QByteArray &data)
{
    mData = data;
}

QMap<QString, QString> HttpRequest::arguments() const
{
    return mArguments;
}
