#ifndef SOCKETCONTEXT_HPP
#define SOCKETCONTEXT_HPP

#include "httprequest.h"
#include "httpresponse.h"
#include "websocketframe.hpp"
#include <QTcpSocket>
#include <QTimer>

struct SocketContext
{
    QTcpSocket *socket;
    QTimer *timeoutTimer;

    bool operator==(const SocketContext &other)
    {
        return other.socket == socket;
    }
};
Q_DECLARE_METATYPE(SocketContext);

using onHttpPacketFunction     = void (*)(const HttpRequest &, HttpResponse &);
using onWebsocketFrameFunction = WebSocketFrame (*)(SocketContext &, const WebSocketFrame &);

Q_DECLARE_METATYPE(onHttpPacketFunction);
Q_DECLARE_METATYPE(onWebsocketFrameFunction);

#endif // SOCKETCONTEXT_HPP
