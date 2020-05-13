#include "token.h"

#include <QRandomGenerator>
#include <QDateTime>

QString Token::generate()
{
    const int base = 16;

    QString token = QString::number(QRandomGenerator::system()->generate64(), base);
    token += QString::number(QRandomGenerator::system()->generate64(), base);
    token += QString::number(QRandomGenerator::system()->generate64(), base);
    token += QString::number(QRandomGenerator::system()->generate64(), base);
    token = QString::number(QDateTime::currentSecsSinceEpoch(), base) + token;

    return token;
}
