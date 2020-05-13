#include "websocketframe.hpp"
#include <QDataStream>
#include <QRandomGenerator>


QByteArray WebSocketFrame::toByteArray(bool useMask)
{
    QByteArray array;
    QDataStream dataStream(&array, QIODevice::WriteOnly);
    dataStream.setByteOrder(QDataStream::BigEndian);

    std::array<quint8, 2> header;

    header.at(0) = static_cast<char>(isFinalFrame()) << 7 | (static_cast<char>(opcode()) & 0x0F);

    if (useMask)
        header.at(1) = 1 << 7;

    dataStream << header.at(0);

    if (mData.size() <= 125)
    {
        header.at(1) |= mData.size() & 0x7F;
        dataStream << header.at(1);
    }
    else if (mData.size() <= 65535)
    {
        header.at(1) |= 126;
        dataStream << header.at(1);
        dataStream << static_cast<quint16>(mData.size());
    }
    else
    {
        header.at(1) |= 127;
        dataStream << header.at(1);
        dataStream << static_cast<quint64>(mData.size());
    }

    quint32 mask = 0;

    if (useMask)
    {
        mask = QRandomGenerator().generate();
        dataStream << mask;
    }

    int maskPosition = 0;
    auto byteMask = reinterpret_cast<quint8 *>(&mask);

    for (const auto &byte : mData)
    {
        array.push_back(byte ^ byteMask[maskPosition]);

        maskPosition = (maskPosition + 1) & 0x3;
    }

    return array;
}

const bool &WebSocketFrame::isFinalFrame() const
{
    return mIsFinalFrame;
}

void WebSocketFrame::setIsFinalFrame(bool isFinalFrame)
{
    mIsFinalFrame = isFinalFrame;
}

const QByteArray &WebSocketFrame::data() const
{
    return mData;
}

void WebSocketFrame::setData(const QByteArray &data)
{
    mData = data;
}

const WebSocketFrame::WebSocketOpcode &WebSocketFrame::opcode() const
{
    return mOpcode;
}

void WebSocketFrame::setOpcode(const WebSocketOpcode &opcode)
{
    mOpcode = opcode;
}
