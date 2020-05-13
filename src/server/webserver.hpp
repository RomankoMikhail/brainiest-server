#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "httpparser.hpp"
#include "httprequest.h"
#include "socketcontext.hpp"
#include "websocketframe.hpp"
#include "websocketparser.hpp"
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class WebServer : public QObject
{
    Q_OBJECT
public:

    WebServer() = delete;
    WebServer(const WebServer &other) = delete;
    WebServer& operator=(const WebServer &other) = delete;
    WebServer& operator=(WebServer &other) = delete;


    WebServer(int maximumPendingConnections = 50,
              int keepAliveTime = 5,
              QObject *parent = nullptr);


    ~WebServer() override;

    bool listen(const QHostAddress &address, const quint16 &port = 8080);
    void closeConnections();

    qint64 maxRequestSize() const;
    void setMaxRequestSize(const qint64 &maxRequestSize);

    void registerHttpRoute(const QString &pattern, onHttpPacketFunction callback);
    void registerWebSocketRoute(const QString &pattern, onWebsocketFrameFunction callback);



private slots:

    void sendAllWebSockets(WebSocketFrame &frame);

    void onNewConnection();
    void onDisconnect();
    void onReadyRead();

    void onWebSocketFrameParsed(QTcpSocket *socket, const WebSocketFrame &frame);
    void onHttpPacketParsed(QTcpSocket *socket, const HttpRequest &request);

    onHttpPacketFunction getHttpCallback(const QString &path);
    onWebsocketFrameFunction getWebSocketCallback(const QString &path);

private:
    enum Protocols
    {
        ProtocolHttp,
        ProtocolWebSocket
    };

    void upgradeToWebsocket(QTcpSocket *socket, const QString &webSocketKey, bool ieFix = false);

    QMap<QString, onHttpPacketFunction> mHttpRouter;
    QMap<QString, onWebsocketFrameFunction> mWebsocketRouter;

    QVector<SocketContext> mContexts;

    QTcpServer mTcpServer;
    qint64 mMaximumPendingConnections       = 50;
    qint64 mKeepAliveTime = 5000;
    qint64 mMaxRequestSize   = 10485760; // 10 Mb
};

#endif // HTTPSERVER_H
