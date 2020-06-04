#include "database.hpp"
#include <QString>
#include <QtSql>

const auto CREATE_ANSWER = QString(
    R"(CREATE TABLE answer (id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, answer varchar(100) NOT NULL))");
const auto CREATE_CIPHERS = QString(
    R"(CREATE TABLE ciphers (id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, word varchar(20) NOT NULL UNIQUE))");
const auto CREATE_GAME = QString(
    R"(CREATE TABLE game (id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, author_id integer(10) NOT NULL, "date" timestamp NOT NULL, is_open integer(1) NOT NULL, is_complete integer(1) NOT NULL, FOREIGN KEY(author_id) REFERENCES "user"(id)))");
const auto CREATE_GAME_HAS_CIPHERS = QString(
    R"(CREATE TABLE game_has_ciphers (gameid integer(10) NOT NULL, ciphersid integer(10) NOT NULL, round integer(10) NOT NULL, PRIMARY KEY (gameid, ciphersid), FOREIGN KEY(gameid) REFERENCES game(id), FOREIGN KEY(ciphersid) REFERENCES ciphers(id)))");
const auto CREATE_GAME_HAS_QUESTION = QString(
    R"(CREATE TABLE game_has_question (gameid integer(10) NOT NULL, questionid integer(10) NOT NULL, round integer(10) NOT NULL, PRIMARY KEY (gameid, questionid), FOREIGN KEY(gameid) REFERENCES game(id), FOREIGN KEY(questionid) REFERENCES question(id)))");
const auto CREATE_PLAYER = QString(
    R"(CREATE TABLE player (userid integer(10) NOT NULL, gameid integer(10) NOT NULL, won integer(1) NOT NULL, bonus integer(10) DEFAULT 0 NOT NULL, PRIMARY KEY (userid, gameid), FOREIGN KEY(userid) REFERENCES "user"(id), FOREIGN KEY(gameid) REFERENCES game(id)))");
const auto CREATE_QUESTION = QString(
    R"(CREATE TABLE question (id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, theme varchar(32) NOT NULL, question varchar(100) NOT NULL UNIQUE, type integer(10) NOT NULL))");
const auto CREATE_QUESTION_HAS_ANSWER(
    R"(CREATE TABLE question_has_answer (questionid integer(10) NOT NULL, answerid integer(10) NOT NULL, correct integer(1) NOT NULL, PRIMARY KEY (questionid, answerid), FOREIGN KEY(questionid) REFERENCES question(id), FOREIGN KEY(answerid) REFERENCES answer(id)))");
const auto CREATE(
    R"(CREATE TABLE "user" (id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, name varchar(80) NOT NULL, surname varchar(80) NOT NULL, patronymic varchar(80), username varchar(32) NOT NULL UNIQUE, password varchar(256) NOT NULL, is_administator integer(10) NOT NULL))");
const auto CREATE_USER_ANSWERED(
    R"(CREATE TABLE user_answered (userid integer(10) NOT NULL, gameid integer(10) NOT NULL, questionid integer(10) NOT NULL, answerid integer(10), PRIMARY KEY (userid, gameid, questionid), FOREIGN KEY(gameid) REFERENCES game(id), FOREIGN KEY(userid) REFERENCES "user"(id), FOREIGN KEY(questionid) REFERENCES question(id), FOREIGN KEY(answerid) REFERENCES answer(id)))");

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

    if (!mDatabase.open())
        return mDatabase.lastError();

    // Включаем поддержку внешних ключей
    QSqlQuery query(mDatabase);
    query.exec("PRAGMA foreign_keys = ON;");

    QStringList databaseTables = mDatabase.tables();

    bool userExist     = databaseTables.contains("user", Qt::CaseInsensitive);
    bool answerExist   = databaseTables.contains("answer", Qt::CaseInsensitive);
    bool questionExist = databaseTables.contains("question", Qt::CaseInsensitive);
    bool ciphersExist  = databaseTables.contains("ciphers", Qt::CaseInsensitive);
    bool gameExist     = databaseTables.contains("game", Qt::CaseInsensitive);

    bool gameHasCiphersExist  = databaseTables.contains("game_has_ciphers", Qt::CaseInsensitive);
    bool gameHasQuestionExist = databaseTables.contains("game_has_question", Qt::CaseInsensitive);
    bool playerExist          = databaseTables.contains("player", Qt::CaseInsensitive);
    bool questionHasAnswerExist =
        databaseTables.contains("question_has_answer", Qt::CaseInsensitive);
    bool userAnsweredExist = databaseTables.contains("user_answered", Qt::CaseInsensitive);

    if (!userExist)
        if (!query.exec(CREATE))
            return query.lastError();

    if (!answerExist)
        if (!query.exec(CREATE_ANSWER))
            return query.lastError();

    if (!questionExist)
        if (!query.exec(CREATE_QUESTION))
            return query.lastError();

    if (!ciphersExist)
        if (!query.exec(CREATE_CIPHERS))
            return query.lastError();

    if (!gameExist)
        if (!query.exec(CREATE_GAME))
            return query.lastError();

    if (!gameHasCiphersExist)
        if (!query.exec(CREATE_GAME_HAS_CIPHERS))
            return query.lastError();

    if (!gameHasQuestionExist)
        if (!query.exec(CREATE_GAME_HAS_QUESTION))
            return query.lastError();

    if (!playerExist)
        if (!query.exec(CREATE_PLAYER))
            return query.lastError();

    if (!questionHasAnswerExist)
        if (!query.exec(CREATE_QUESTION_HAS_ANSWER))
            return query.lastError();

    if (!userAnsweredExist)
        if (!query.exec(CREATE_USER_ANSWERED))
            return query.lastError();

    mDatabase.commit();
    return QSqlError();
}

QSqlDatabase Database::database() const
{
    return mDatabase;
}
