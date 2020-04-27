#ifndef HTTPPARSER_H
#define HTTPPARSER_H

#include "HttpPacket.hpp"
#include "http_parser.h"
#include <QIODevice>
#include <QObject>
#include <QTcpSocket>

class HttpParser : public QObject
{
    Q_OBJECT
public:
    explicit HttpParser(QObject *parent = nullptr);
    ~HttpParser();

    void parse(QIODevice *device, const qint64 &peekSize);

    void setUrl(const QString &url);
    void setMethod(const HttpPacket::Method &method);
    void setField(const QString &field);
    void setValue(const QString &value);
    void setData(const QByteArray &data);
    void setHttpVersion(const int &major, const int &minor);
    void messageReady();

    HttpPacket currentPacket() const;
    void setCurrentPacket(const HttpPacket &currentPacket);

signals:
    void httpParsed(QTcpSocket *socket, HttpPacket packet);

private:
    QString mCurrentField;
    HttpPacket mCurrentPacket;

    http_parser_settings mSettings;
    http_parser *mParser = nullptr;
};
Q_DECLARE_METATYPE(HttpParser *)

#endif // HTTPPARSER_H
