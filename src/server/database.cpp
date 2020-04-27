#include "Database.hpp"
#include <QString>
#include <QtSql>

const auto CREATE_USER = QString(R"(CREATE TABLE IF NOT EXISTS `user` (
                                 `id` INTEGER PRIMARY KEY AUTOINCREMENT,
                                 `name` VARCHAR(80) NOT NULL,
                                 `surname` VARCHAR(80) NOT NULL,
                                 `patronymic` VARCHAR(80) NOT NULL,
                                 `username` VARCHAR(32) NOT NULL UNIQUE,
                                 `password` VARCHAR(256) NOT NULL
                                 ))");

const auto CREATE_QUESTION = QString(R"(CREATE TABLE IF NOT EXISTS `question` (
                                     `id` INTEGER PRIMARY KEY AUTOINCREMENT,
                                     `theme` VARCHAR(300) NOT NULL,
                                     `question` VARCHAR(300) NOT NULL UNIQUE,
                                     `correct_answer` VARCHAR(300) NOT NULL,
                                     `wrong_answer` VARCHAR(300)
                                     ))");

const auto CREATE_CIPHER = QString(R"(CREATE TABLE IF NOT EXISTS `cipher` (
                                   `id` INTEGER PRIMARY KEY AUTOINCREMENT,
                                   `word` VARCHAR(20) NOT NULL
                                   ))");

const auto CREATE_ASSOCIATION = QString(R"(CREATE TABLE IF NOT EXISTS `association` (
                                        `id` INTEGER PRIMARY KEY AUTOINCREMENT,
                                        `left_words` VARCHAR(300) NOT NULL,
                                        `right_words` VARCHAR(300) NOT NULL
                                        ))");

Database::Database()
{
    mDatabase = QSqlDatabase::addDatabase("QSQLITE");
}

Database::~Database()
{
    mDatabase.close();
}

QSqlError Database::init(const QString &filename)
{
    mDatabase.setDatabaseName(filename);

    if(!mDatabase.open())
        return mDatabase.lastError();

    // Включаем поддержку внешних ключей
    QSqlQuery query(mDatabase);
    query.exec("PRAGMA foreign_keys = ON;");

    QStringList databaseTables = mDatabase.tables();

    bool userExist = databaseTables.contains("user", Qt::CaseInsensitive);
    bool questionExist = databaseTables.contains("question", Qt::CaseInsensitive);
    bool cipherExist = databaseTables.contains("cipher", Qt::CaseInsensitive);
    bool associationExist = databaseTables.contains("association", Qt::CaseInsensitive);

    if(!userExist)
        if(!query.exec(CREATE_USER))
            return query.lastError();

    if(!questionExist)
        if(!query.exec(CREATE_QUESTION))
            return query.lastError();

    if(!cipherExist)
        if(!query.exec(CREATE_CIPHER))
            return query.lastError();

    if(!associationExist)
        if(!query.exec(CREATE_ASSOCIATION))
            return query.lastError();

    mDatabase.commit();
    return QSqlError();
}

QSqlDatabase Database::database() const
{
    return mDatabase;
}
