#ifndef SINGLETONDATABASE_HPP
#define SINGLETONDATABASE_HPP

#include "action.hpp"
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

    static Action &action()
    {
        static Action action;
        return action;
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
