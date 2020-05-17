#ifndef GAMEHASCIPHER_H
#define GAMEHASCIPHER_H

#include <QList>

class GameHasCipher
{
public:
    static GameHasCipher create(int gameId, int ciphersId);
    static GameHasCipher getById(int gameId, int ciphersId);
    static QList<int> getGameIds(int ciphersId);
    static QList<int> getCiphersIds(int gameId);

    bool isValid() const;

    int gameId() const;
    int ciphersId() const;

    bool remove();
private:
    bool mIsValid = false;
    int mGameId;
    int mCiphersId;
};

#endif // GAMEHASCIPHER_H
