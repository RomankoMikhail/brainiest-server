#include "logic.h"

#include "singleton.hpp"

Logic::Logic(QObject *parent) : QObject(parent)
{
    mTimer.setInterval(500);
    mTimer.start();

    connect(&mTimer, &QTimer::timeout, this, &Logic::check);
}

void Logic::check()
{
    for(auto key : Singleton::games().keys())
    {
        Singleton::games()[key].update();
    }
}
