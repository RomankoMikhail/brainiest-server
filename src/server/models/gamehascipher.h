#ifndef GAMEHASCIPHER_H
#define GAMEHASCIPHER_H

#include <QList>

class GameHasCipher
{
public:
    static GameHasCipher create(int gameId, int ciphersId, int round);
    static GameHasCipher getById(int gameId, int ciphersId);
    static QList<int> getGameIds(int ciphersId);
    static QList<int> getCiphersIds(int gameId);
    static QList<int> getCiphersIdsByRound(int gameId, int round);

    bool isValid() const;

    int gameId() const;
    int ciphersId() const;

    bool remove();
private:
    bool mIsValid = false;
    int mGameId;
    int mCiphersId;
    int mRound;
};

#endif // GAMEHASCIPHER_H
