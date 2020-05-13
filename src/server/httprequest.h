#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include "cookie.hpp"
#include <QBuffer>
#include <QHostAddress>
#include <QTcpSocket>

class HttpRequest
{
public:
    HttpRequest() = default;

    QHostAddress hostAddress() const;
    void setHostAddress(const QHostAddress &hostAddress);

    QString path() const;
    void setPath(const QString &path);

    void parseGetArguments();
    void parseCookies();

    QString method() const;
    void setMethod(const QString &method);

    int protocolVersion() const;
    void setProtocolVersion(int protocolVersion);

    QMultiMap<QString, QString> headers() const;
    void addHeader(const QString &header, const QString &value);

    QByteArray data() const;
    void setData(const QByteArray &data);

    QMap<QString, QString> arguments() const;

    QList<Cookie> cookies() const;

private:
    QHostAddress mHostAddress;
    QString mPath;
    QString mMethod;
    int mProtocolVersion = 11;

    QList<Cookie> mCookies;
    QMap<QString, QString> mArguments;
    QMultiMap<QString, QString> mHeaders;
    QByteArray mData;
};

#endif // HTTPREQUEST_H