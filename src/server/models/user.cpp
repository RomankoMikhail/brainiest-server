#include "user.h"
#include "../singleton.hpp"
#include <QSql>
#include <QtSql>

const auto CREATE = QString(
    R"(INSERT INTO `user`(`name`, `surname`, `patronymic`, `username`, `password`, `is_administator`) VALUES(?, ?, ?, ?, ?, ?))");

const auto UPDATE = QString(R"(UPDATE `user` SET `name` = ?, `surname` = ?, `patronymic` = ?,
                                  `password` = ?, `is_administator` = ? WHERE `id` = ?)");

const auto SELECT_BY_ID = QString(R"(SELECT `name`, `surname`, `patronymic`, `username`, `password`,
                                  `is_administator` FROM `user` WHERE `id` = ?)");

const auto SELECT_BY_USERNAME = QString(R"(SELECT `id` FROM `user` WHERE `username` = ?)");

const auto SELECT_ALL_IDS = QString(R"(SELECT `id` FROM `user`)");

User User::create(const QString &name, const QString &surname, const QString &patronymic,
                  const QString &username, const QString &password, const int &isAdminstrator)
{
    User newUser;

    newUser.mName            = name;
    newUser.mSurname         = surname;
    newUser.mPatronymic      = patronymic;
    newUser.mUsername        = username;
    newUser.mPassword        = password;
    newUser.mIsAdministrator = isAdminstrator;

    QSqlQuery query(Singleton::database().database());

    query.prepare(CREATE);
    query.addBindValue(name);
    query.addBindValue(surname);
    query.addBindValue(patronymic);
    query.addBindValue(username);
    query.addBindValue(password);
    query.addBindValue(isAdminstrator);

    if (!query.exec())
    {
        qDebug() << query.lastError();
        return User();
    }

    newUser.mId      = query.lastInsertId().toInt();
    newUser.mIsValid = true;

    return newUser;
}

User User::getById(const int &id)
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_BY_ID);
    query.addBindValue(id);
    query.exec();

    if (!query.next())
        return User();

    User user;
    user.mId              = id;
    user.mName            = query.value(0).toString();
    user.mSurname         = query.value(1).toString();
    user.mPatronymic      = query.value(2).toString();
    user.mUsername        = query.value(3).toString();
    user.mPassword        = query.value(4).toString();
    user.mIsAdministrator = query.value(5).toInt();
    user.mIsValid         = true;

    return user;
}

User User::getByUsername(const QString &username)
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(SELECT_BY_USERNAME);
    query.addBindValue(username);
    query.exec();

    if (!query.next())
        return User();

    return User::getById(query.value(0).toInt());
}

QList<int> User::getIds()
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

QString User::name() const
{
    return mName;
}

void User::setName(const QString &name)
{
    mName = name;
}

QString User::surname() const
{
    return mSurname;
}

void User::setSurname(const QString &surname)
{
    mSurname = surname;
}

QString User::patronymic() const
{
    return mPatronymic;
}

void User::setPatronymic(const QString &patronymic)
{
    mPatronymic = patronymic;
}

QString User::username() const
{
    return mUsername;
}

QString User::password() const
{
    return mPassword;
}

void User::setPassword(const QString &password)
{
    mPassword = password;
}

int User::isAdministrator() const
{
    return mIsAdministrator;
}

void User::setIsAdministrator(int isAdministrator)
{
    mIsAdministrator = isAdministrator;
}

bool User::update()
{
    QSqlQuery query(Singleton::database().database());

    query.prepare(UPDATE);
    query.addBindValue(mName);
    query.addBindValue(mSurname);
    query.addBindValue(mPatronymic);
    query.addBindValue(mPassword);
    query.addBindValue(mIsAdministrator);
    query.addBindValue(mId);

    bool result = query.exec();

    if (!result)
        qDebug() << query.lastError().text();

    return result;
}

int User::id() const
{
    return mId;
}

bool User::isValid() const
{
    return mIsValid;
}
