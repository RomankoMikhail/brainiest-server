#include "ciphers.h"

#include "../singleton.hpp"

const auto CREATE = QString(R"(INSERT INTO `ciphers`(`word`) VALUES(?))");

const auto UPDATE = QString(R"(UPDATE `ciphers` SET `word` = ? WHERE `id` = ?)");

const auto SELECT_BY_ID = QString(R"(SELECT `word` FROM `ciphers` WHERE `id` = ?)");

const auto SELECT_ALL_IDS = QString(R"(SELECT `id` FROM `ciphers`)");

Ciphers Ciphers::create(QString word)
{
    Ciphers newCipher;

    newCipher.mWord = word.toUpper();

    QSqlQuery query(Singleton::database().database());

    query.prepare(CREATE);
    query.addBindValue(word.toUpper());

    if (!query.exec())
    {
        qDebug() << query.lastError();
        return Ciphers();
    }

    newCipher.mId      = query.lastInsertId().toInt();
    newCipher.mIsValid = true;

    return newCipher;
}

QList<int> Ciphers::getIds()
{
    QList<int> ids;

    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_ALL_IDS);
    query.exec();

    while (query.next())
    {
        ids.push_back(query.value(0).toInt());
    }

    return ids;
}

Ciphers Ciphers::getById(int id)
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_BY_ID);
    query.addBindValue(id);
    query.exec();

    if (!query.next())
        return Ciphers();

    Ciphers cipher;
    cipher.mId   = id;
    cipher.mWord = query.value(0).toString();
    cipher.mIsValid = true;

    return cipher;
}

int Ciphers::id() const
{
    return mId;
}

bool Ciphers::isValid() const
{
    return mIsValid;
}

QString Ciphers::word() const
{
    return mWord.toUpper();
}

void Ciphers::setWord(const QString &word)
{
    mWord = word.toUpper();
}

bool Ciphers::update()
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(UPDATE);
    query.addBindValue(mWord);
    query.addBindValue(mId);

    bool result = query.exec();

    if (!result)
        qDebug() << query.lastError().text();

    return result;
}
