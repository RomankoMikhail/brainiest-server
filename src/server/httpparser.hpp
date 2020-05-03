#ifndef HTTPPARSER_H
#define HTTPPARSER_H

#include "httprequest.h"
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
    void headersFlush();
    void setUrl(const QString &url);
    void setMethod(const QString &method);
    void setField(const QString &field);
    void setValue(const QString &value);
    void setData(const QByteArray &data);
    void setHttpVersion(const int &major, const int &minor);
    void messageReady();

signals:
    void httpParsed(QTcpSocket *socket, HttpRequest request);

private:
    enum State
    {
        None,
        Value,
        Field
    } mState;


    QString mCurrentField;
    QString mCurrentValue;
    HttpRequest mCurrentRequest;

    http_parser_settings mSettings;
    http_parser *mParser = nullptr;
};
Q_DECLARE_METATYPE(HttpParser *)

#endif // HTTPPARSER_H
