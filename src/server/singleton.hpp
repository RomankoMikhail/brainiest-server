#ifndef SINGLETONDATABASE_HPP
#define SINGLETONDATABASE_HPP

#include "database.hpp"

class Singleton
{
public:
    Singleton(Singleton const &) = delete;
    Singleton &operator=(Singleton const &) = delete;

    static Database &database()
    {
        static Database database;
        return database;
    }

    static QMap<QString, int> &tokens()
    {
        static QMap<QString, int> tokens;
        return tokens;
    }

    static QMimeDatabase &mimeDatabase()
    {
        static QMimeDatabase mimeDatabase;
        return mimeDatabase;
    }
};

#endif // SINGLETONDATABASE_HPP
