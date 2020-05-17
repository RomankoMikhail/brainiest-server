#ifndef PLAYER_H
#define PLAYER_H

#include "../singleton.hpp"

class Player
{
public:
    static Player create(int gameId, int userId, int won, int bonus);
    static Player getById(int gameId, int userId);
    static QList<int> getGameIds(int userId);
    static QList<int> getUserIds(int gameId);

    int gameId() const;
    int userId() const;

    int won() const;
    void setWon(int won);

    int bonus() const;
    void setBonus(int bonus);

    bool isValid() const;

    bool update();
    bool remove();

private:
    bool mIsValid = false;
    int mGameId;
    int mUserId;
    int mWon;
    int mBonus;
};

#endif // PLAYER_H
