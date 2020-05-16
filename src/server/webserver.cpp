#include "webserver.hpp"
#include "httpparser.hpp"
#include "httprequest.h"
#include <QCryptographicHash>
#include <QDir>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QMimeType>

WebServer::WebServer(int maximumPendingConnections, int keepAliveTime,
                     QObject *parent)
    : QObject(parent)
{
    connect(&mTcpServer, &QTcpServer::newConnection, this,
            &WebServer::onNewConnection);

    const int milisecondsInSecond = 1000;

    mMaximumPendingConnections = maximumPendingConnections;
    mKeepAliveTime             = keepAliveTime * milisecondsInSecond;
}

WebServer::~WebServer()
{
    closeConnections();
}

void WebServer::closeConnections()
{
    mTcpServer.pauseAccepting();

    for (auto &context : mContexts)
    {
        context.socket->close();
    }

    mContexts.clear();
}

bool WebServer::listen(const QHostAddress &address, const quint16 &port)
{
    mTcpServer.setMaxPendingConnections(mMaximumPendingConnections);
    return mTcpServer.listen(address, port);
}

#include <QVariant>

void WebServer::onNewConnection()
{
    SocketContext context = {};

    context.socket       = mTcpServer.nextPendingConnection();
    context.timeoutTimer = new QTimer();

    context.timeoutTimer->setSingleShot(true);
    context.timeoutTimer->setInterval(mKeepAliveTime);
    context.timeoutTimer->start();

    auto httpParser      = new HttpParser(context.socket);
    auto webSocketParser = new WebSocketParser(context.socket);

    context.socket->setProperty("httpParser", QVariant::fromValue(httpParser));
    context.socket->setProperty("webSocketParser",
                                QVariant::fromValue(webSocketParser));
    context.socket->setProperty("context", QVariant::fromValue(context));

    connect(context.timeoutTimer, &QTimer::timeout, context.socket,
            &QTcpSocket::close);
    connect(context.socket, &QTcpSocket::disconnected, this,
            &WebServer::onDisconnect);
    connect(context.socket, &QTcpSocket::readyRead, this,
            &WebServer::onReadyRead);
    connect(httpParser, &HttpParser::httpParsed, this,
            &WebServer::onHttpPacketParsed);
    connect(webSocketParser, &WebSocketParser::frameReady, this,
            &WebServer::onWebSocketFrameParsed);

    context.socket->setProperty("protocol", ProtocolHttp);

    mContexts.push_back(context);
}

void WebServer::onDisconnect()
{
    auto socket = dynamic_cast<QTcpSocket *>(sender());

    if (socket == nullptr)
        return;

    HttpParser *httpParser;
    WebSocketParser *webSocketParser;
    SocketContext context = {};

    httpParser = socket->property("httpParser").value<HttpParser *>();
    webSocketParser =
        socket->property("webSocketParser").value<WebSocketParser *>();
    context = socket->property("context").value<SocketContext>();

    mContexts.removeOne(context);

    disconnect(httpParser, &HttpParser::httpParsed, this,
               &WebServer::onHttpPacketParsed);
    disconnect(webSocketParser, &WebSocketParser::frameReady, this,
               &WebServer::onWebSocketFrameParsed);
    disconnect(socket, &QTcpSocket::disconnected, this,
               &WebServer::onDisconnect);
    disconnect(socket, &QTcpSocket::readyRead, this, &WebServer::onReadyRead);
}

void WebServer::onReadyRead()
{
    auto socket = dynamic_cast<QTcpSocket *>(sender());

    if (socket == nullptr)
        return;

    HttpParser *httpParser;
    WebSocketParser *webSocketParser;
    SocketContext context = {};

    httpParser = socket->property("httpParser").value<HttpParser *>();
    webSocketParser =
        socket->property("webSocketParser").value<WebSocketParser *>();
    context = socket->property("context").value<SocketContext>();
    context.timeoutTimer->start();

    Protocols socketProtocol =
        static_cast<Protocols>(socket->property("protocol").toInt());

    switch (socketProtocol)
    {
    case ProtocolHttp:
        httpParser->parse(socket, mMaxRequestSize);
        break;

    case ProtocolWebSocket:
        webSocketParser->parse(socket);
        break;
    }
}

void WebServer::onWebSocketFrameParsed(QTcpSocket *socket,
                                       const WebSocketFrame &frame)
{
    WebSocketFrame returnFrame;

    onWebsocketFrameFunction webSocketCallback;
    webSocketCallback =
        socket->property("webSocketCallback").value<onWebsocketFrameFunction>();

    SocketContext context = {};
    context               = socket->property("context").value<SocketContext>();

    switch (frame.opcode())
    {
    case WebSocketFrame::OpcodePing:
        returnFrame.setOpcode(WebSocketFrame::OpcodePong);
        returnFrame.setIsFinalFrame(true);
        returnFrame.setData(frame.data());
        break;

    case WebSocketFrame::OpcodeClose:
        returnFrame.setOpcode(WebSocketFrame::OpcodeClose);
        returnFrame.setIsFinalFrame(true);
        returnFrame.setData(frame.data());

        socket->write(returnFrame.toByteArray());
        socket->close();
        return;

    default:
        returnFrame = webSocketCallback(context, frame);
        break;
    }

    socket->write(returnFrame.toByteArray());
    socket->flush();
}

void WebServer::onHttpPacketParsed(QTcpSocket *socket,
                                   const HttpRequest &request)
{
    QString accessPath = request.path();

    qDebug() << accessPath;

    onHttpPacketFunction httpCallback = getHttpCallback(accessPath);

    SocketContext context = {};
    context               = socket->property("context").value<SocketContext>();

    for (const auto &upgradeStrings : request.headers().values("upgrade"))
    {
        if (upgradeStrings.contains("websocket", Qt::CaseInsensitive))
        {
            context.timeoutTimer->stop();
            onWebsocketFrameFunction webSocketCallback =
                getWebSocketCallback(accessPath);

            if (webSocketCallback == nullptr)
                break;

            socket->setProperty("webSocketCallback",
                                QVariant::fromValue(webSocketCallback));
            upgradeToWebsocket(
                socket, request.headers().values("sec-websocket-key").first(),
                true);
            return;
        }
    }

    // Вызов Callback'a

    HttpResponse response;

    if (request.method() == "options")
    {
        response.setStatusCode(HttpResponse::CodeNoContent);
    }
    else
    {
        if (httpCallback != nullptr)
        {
            httpCallback(request, response);
        }
        else
        {
            response.setStatusCode(HttpResponse::CodeNotFound);
            response.write("<html><head></head><body>");
            response.write("<h1>Resource not found</h1>");
            response.write(QString("<p>The requested resource \"" + accessPath +
                                   "\" not found</p>")
                               .toUtf8());
        }
    }

    response.addHeader("Access-Control-Allow-Origin", "*");
    response.addHeader("Access-Control-Allow-Methods",
                       "POST, GET, OPTIONS, DELETE, PUT");
    response.addHeader(
        "Access-Control-Allow-Headers",
        "append,delete,entries,foreach,get,has,keys,set,values,Authorization");

    bool connectionKeepAlive = true;

    for (const auto &connectionStrings : request.headers().values("connection"))
    {
        if (connectionStrings.contains("close", Qt::CaseInsensitive))
        {
            connectionKeepAlive = false;
        }
    }

    if (connectionKeepAlive)
        response.addHeader("Connection", "keep-alive");
    else
        response.addHeader("Connection", "close");

    response.flush(socket);

    if (!connectionKeepAlive)
        socket->close();
}

void WebServer::registerHttpRoute(const QString &pattern,
                                  onHttpPacketFunction callback)
{
    mHttpRouter[pattern] = callback;
}

void WebServer::registerWebSocketRoute(const QString &pattern,
                                       onWebsocketFrameFunction callback)
{
    mWebsocketRouter[pattern] = callback;
}

void WebServer::sendAllWebSockets(WebSocketFrame &frame)
{
    for (auto context : mContexts)
    {
        Protocols socketProtocol = static_cast<Protocols>(
            context.socket->property("protocol").toInt());
        if (socketProtocol == ProtocolWebSocket)
            context.socket->write(frame.toByteArray());
    }
}

onHttpPacketFunction WebServer::getHttpCallback(const QString &path)
{
    for (auto it = mHttpRouter.begin(); it != mHttpRouter.end(); ++it)
    {
        QRegExp regular(it.key());

        if (regular.exactMatch(path))
            return it.value();
    }

    return nullptr;
}

onWebsocketFrameFunction WebServer::getWebSocketCallback(const QString &path)
{
    for (auto it = mWebsocketRouter.begin(); it != mWebsocketRouter.end(); ++it)
    {
        QRegExp regular(it.key());

        if (regular.exactMatch(path))
            return it.value();
    }

    return nullptr;
}

qint64 WebServer::maxRequestSize() const
{
    return mMaxRequestSize;
}

void WebServer::setMaxRequestSize(const qint64 &maxRequestSize)
{
    mMaxRequestSize = maxRequestSize;
}

void WebServer::upgradeToWebsocket(QTcpSocket *socket,
                                   const QString &webSocketKey, bool ieFix)
{
    QString magicString = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    QString stringKey   = webSocketKey + magicString;

    QByteArray resultOfHash =
        QCryptographicHash::hash(stringKey.toUtf8(), QCryptographicHash::Sha1);
    QString acceptString = resultOfHash.toBase64();

    HttpResponse response;

    response.addHeader("Sec-WebSocket-Accept", acceptString);
    response.addHeader("Connection", "Upgrade");

    if (ieFix)
        response.addHeader("Upgrade", "Websocket");
    else
        response.addHeader("Upgrade", "websocket");

    response.setStatusCode(HttpResponse::CodeSwitchingProtocols);

    socket->setProperty("protocol", ProtocolWebSocket);

    response.flush(socket);
}
