#include "webserver.hpp"
#include "httpparser.hpp"
#include <QCryptographicHash>
#include <QDir>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QMimeType>
#include "httprequest.h"

WebServer::WebServer(qint64 maxClients, qint64 keepAliveTimeout, QObject *parent) : QObject(parent)
{
    connect(&mTcpServer, &QTcpServer::newConnection, this, &WebServer::onNewConnection);

    mMaxClients = maxClients;
    mKeepAliveTimeout = keepAliveTimeout * 1000;
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
    mTcpServer.setMaxPendingConnections(mMaxClients);
    return mTcpServer.listen(address, port);
}

#include <QVariant>

void WebServer::onNewConnection()
{
    SocketContext context;

    context.socket = mTcpServer.nextPendingConnection();
    context.timeoutTimer = new QTimer();

    context.timeoutTimer->setSingleShot(true);
    context.timeoutTimer->setInterval(mKeepAliveTimeout);
    context.timeoutTimer->start();

    HttpParser *httpParser           = new HttpParser(context.socket);
    WebSocketParser *webSocketParser = new WebSocketParser(context.socket);

    context.socket->setProperty("httpParser", QVariant::fromValue(httpParser));
    context.socket->setProperty("webSocketParser", QVariant::fromValue(webSocketParser));
    context.socket->setProperty("context", QVariant::fromValue(context));

    connect(context.timeoutTimer, &QTimer::timeout, context.socket, &QTcpSocket::close);
    connect(context.socket, &QTcpSocket::disconnected, this, &WebServer::onDisconnect);
    connect(context.socket, &QTcpSocket::readyRead, this, &WebServer::onReadyRead);
    connect(httpParser, &HttpParser::httpParsed, this, &WebServer::onHttpPacketParsed);
    connect(webSocketParser, &WebSocketParser::frameReady, this,
            &WebServer::onWebSocketFrameParsed);

    context.socket->setProperty("protocol", ProtocolHttp);

    mContexts.push_back(context);
}

void WebServer::onDisconnect()
{
    QTcpSocket *socket = dynamic_cast<QTcpSocket *>(sender());

    if (socket == nullptr)
        return;

    HttpParser *httpParser;
    WebSocketParser *webSocketParser;
    SocketContext context;

    httpParser      = socket->property("httpParser").value<HttpParser *>();
    webSocketParser = socket->property("webSocketParser").value<WebSocketParser *>();
    context         = socket->property("context").value<SocketContext>();

    mContexts.removeOne(context);

    disconnect(httpParser, &HttpParser::httpParsed, this, &WebServer::onHttpPacketParsed);
    disconnect(webSocketParser, &WebSocketParser::frameReady, this,
               &WebServer::onWebSocketFrameParsed);
    disconnect(socket, &QTcpSocket::disconnected, this, &WebServer::onDisconnect);
    disconnect(socket, &QTcpSocket::readyRead, this, &WebServer::onReadyRead);
}

void WebServer::onReadyRead()
{
    QTcpSocket *socket = dynamic_cast<QTcpSocket *>(sender());

    if (socket == nullptr)
        return;

    HttpParser *httpParser;
    WebSocketParser *webSocketParser;
    SocketContext context;

    httpParser      = socket->property("httpParser").value<HttpParser *>();
    webSocketParser = socket->property("webSocketParser").value<WebSocketParser *>();
    context         = socket->property("context").value<SocketContext>();
    context.timeoutTimer->start();

    Protocols socketProtocol = static_cast<Protocols>(socket->property("protocol").toInt());

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

void WebServer::onWebSocketFrameParsed(QTcpSocket *socket, const WebSocketFrame &frame)
{
    WebSocketFrame returnFrame;

    onWebsocketFrameFunction webSocketCallback;
    webSocketCallback = socket->property("webSocketCallback").value<onWebsocketFrameFunction>();

    SocketContext context;
    context = socket->property("context").value<SocketContext>();

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

void WebServer::onHttpPacketParsed(QTcpSocket *socket, const HttpRequest &request)
{
    QString accessPath = request.path();

    qDebug() << accessPath;

    onHttpPacketFunction httpCallback = getHttpCallback(accessPath);

    SocketContext context;
    context = socket->property("context").value<SocketContext>();

    for (const auto &upgradeStrings : request.headers().values("upgrade"))
    {
        if (upgradeStrings.contains("websocket", Qt::CaseInsensitive))
        {
            context.timeoutTimer->stop();
            onWebsocketFrameFunction webSocketCallback = getWebSocketCallback(accessPath);

            if (webSocketCallback == nullptr)
                break;

            socket->setProperty("webSocketCallback", QVariant::fromValue(webSocketCallback));
            upgradeToWebsocket(socket, request.headers().values("sec-websocket-key").first(), true);
            return;
        }
    }

    // Вызов Callback'a

    HttpResponse response;

    if (httpCallback != nullptr)
    {
        httpCallback(request, response);
    }
    else
    {
        static QMimeDatabase mimeDatabase;

        response.setStatusCode(HttpResponse::CodeNotFound);
        response.write("<html><head></head><body>");
        response.write("<h1>Resource not found</h1>");
        response.write(QString("<p>The requested resource \"" + accessPath + "\" not found</p>").toUtf8());
    }

    bool connectionKeepAlive = true;

    for (const auto &connectionStrings : request.headers().values("connection"))
    {
        if (connectionStrings.contains("close", Qt::CaseInsensitive))
        {
                connectionKeepAlive = false;
        }
    }

    if(connectionKeepAlive)
        response.addHeader("connection", "keep-alive");
    else
        response.addHeader("connection", "close");

    response.flush(socket);

    if (!connectionKeepAlive)
        socket->close();
}

void WebServer::registerHttpRoute(const QString &regularExpression, onHttpPacketFunction callback)
{
    mHttpRouter[regularExpression] = callback;
}

void WebServer::registerWebSocketRoute(const QString &regularExpression,
                                       onWebsocketFrameFunction callback)
{
    mWebsocketRouter[regularExpression] = callback;
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

void WebServer::upgradeToWebsocket(QTcpSocket *socket, const QString &webSocketKey, bool ieFix)
{
    QString magicString = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    QString stringKey   = webSocketKey + magicString;

    QByteArray resultOfHash =
        QCryptographicHash::hash(stringKey.toUtf8(), QCryptographicHash::Sha1);
    QString acceptString = resultOfHash.toBase64();

    HttpPacket response;

    response.addValue("Sec-WebSocket-Accept", acceptString);
    response.addValue("Connection", "Upgrade");

    if (ieFix)
        response.addValue("Upgrade", "Websocket");
    else
        response.addValue("Upgrade", "websocket");

    response.setStatusCode(HttpPacket::CodeSwitchingProtocols);

    socket->setProperty("protocol", ProtocolWebSocket);

    socket->write(response.toByteArray());
}
