#ifndef SINGLETONDATABASE_HPP
#define SINGLETONDATABASE_HPP

#include "database.hpp"
#include "action.hpp"

class Singleton
{
public:
    Singleton(Singleton const&) = delete;
    Singleton& operator= (Singleton const&) = delete;

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
};

#endif // SINGLETONDATABASE_HPP
