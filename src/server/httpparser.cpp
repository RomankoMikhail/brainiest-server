#include "httpparser.hpp"

#include <QByteArray>
#include <QDebug>
#include <QString>

int headersCallback(http_parser *parser)
{
    auto caller = static_cast<HttpParser *>(parser->data);

    caller->headersFlush();

    return 0;
}

int messageCallback(http_parser *parser)
{
    auto caller = static_cast<HttpParser *>(parser->data);

    switch (parser->method)
    {
    case HTTP_GET:
        caller->setMethod("get");
        break;

    case HTTP_HEAD:
        caller->setMethod("head");
        break;

    case HTTP_POST:
        caller->setMethod("post");
        break;

    default:
        caller->setMethod("");
        break;
    }

    caller->setHttpVersion(parser->http_major, parser->http_minor);

    caller->messageReady();
    return 0;
}

int urlCallback(http_parser *parser, const char *at, size_t length)
{
    auto caller = static_cast<HttpParser *>(parser->data);

    caller->setUrl(QString(QByteArray(at, length)));

    return 0;
}

int bodyCallback(http_parser *parser, const char *at, size_t length)
{
    auto caller = static_cast<HttpParser *>(parser->data);

    caller->setData(QByteArray(at, length));

    return 0;
}

int statusCallback(http_parser */*parser*/, const char *at, size_t length)
{
    qDebug() << "STATUS:" << QByteArray(at, length);

    return 0;
}

int headerFieldCallback(http_parser *parser, const char *at, size_t length)
{
    auto caller = static_cast<HttpParser *>(parser->data);

    caller->setField(QString(QByteArray(at, length)));

    return 0;
}

int headerValueCallback(http_parser *parser, const char *at, size_t length)
{
    auto caller = static_cast<HttpParser *>(parser->data);

    caller->setValue(QString(QByteArray(at, length)));

    return 0;
}

HttpParser::HttpParser(QObject *parent) : QObject(parent)
{
    http_parser_settings_init(&mSettings);

    mSettings.on_url              = urlCallback;
    mSettings.on_body             = bodyCallback;
    mSettings.on_status           = statusCallback;
    mSettings.on_header_field     = headerFieldCallback;
    mSettings.on_header_value     = headerValueCallback;
    mSettings.on_message_complete = messageCallback;
    mSettings.on_headers_complete = headersCallback;

    mParser       = new http_parser();
    mParser->data = this;

    http_parser_init(mParser, HTTP_REQUEST);
}

HttpParser::~HttpParser()
{
    delete mParser;
}

void HttpParser::parse(QIODevice *device, const qint64 &peekSize)
{
    QByteArray readed = device->peek(peekSize);

    qint64 skip = http_parser_execute(mParser, &mSettings, readed.data(), readed.size());

    if (device->isOpen())
        device->skip(skip);
}

void HttpParser::headersFlush()
{
    mCurrentRequest.addHeader(mCurrentField, mCurrentValue);
}

void HttpParser::setUrl(const QString &url)
{
    mCurrentRequest.setPath(mCurrentRequest.path() + url);
}

void HttpParser::setMethod(const QString &method)
{
    mCurrentRequest.setMethod(method);
}

void HttpParser::setField(const QString &field)
{
    if (mState == None || mState == Value)
    {
        headersFlush();
        mCurrentField = field;
    }
    else
    {
        mCurrentField += field;
    }
}

void HttpParser::setValue(const QString &value)
{
    if (mState == None || mState == Field)
    {
        mCurrentValue = value;
    }
    else
    {
        mCurrentValue += value;
    }
}

void HttpParser::setData(const QByteArray &data)
{
    mCurrentRequest.setData(mCurrentRequest.data() + data);
}

void HttpParser::setHttpVersion(const int &major, const int &minor)
{
    mCurrentRequest.setProtocolVersion(major * 10 + minor);
}

void HttpParser::messageReady()
{
    mCurrentRequest.parseCookies();

    auto socket = dynamic_cast<QTcpSocket *>(parent());

    if (socket != nullptr)
        emit httpParsed(socket, mCurrentRequest);

    mCurrentRequest = HttpRequest();
}
