#ifndef WEBSOCKETPARSER_H
#define WEBSOCKETPARSER_H

#include "websocketframe.hpp"
#include <QIODevice>
#include <QObject>
#include <QTcpSocket>

class WebSocketParser : public QObject
{
    Q_OBJECT
public:
    enum State
    {
        StateReadHeader,
        StateReadPayload16Bit,
        StateReadPayload64Bit,
        StateReadMask,
        StateReadPayload
    };

    explicit WebSocketParser(QObject *parent = nullptr);

    void parse(QIODevice *device);

signals:
    void frameReady(QTcpSocket *socket, WebSocketFrame frame);

private:
    WebSocketFrame mCurrentFrame;

    quint64 mPayloadSize;
    quint8 mMask[4];

    State readHeader(QIODevice *device);
    State readPayloadSize16Bit(QIODevice *device);
    State readPayloadSize64Bit(QIODevice *device);
    State readMask(QIODevice *device);
    State readPayload(QIODevice *device);

    State mState = StateReadHeader;
    QByteArray mInternalBuffer;
};
Q_DECLARE_METATYPE(WebSocketParser *)

#endif // WEBSOCKETPARSER_H
