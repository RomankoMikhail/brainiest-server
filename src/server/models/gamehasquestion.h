#ifndef GAMEHASQUESTION_H
#define GAMEHASQUESTION_H

#include <QList>

class GameHasQuestion
{
public:
    static GameHasQuestion create(int gameId, int questionId, int round);
    static GameHasQuestion getById(int gameId, int questionId);
    static QList<int> getGameIds(int questionId);
    static QList<int> getQuestionIds(int gameId);

    bool isValid() const;

    int gameId() const;
    int questionId() const;

    int round() const;
    void setRound(int round);

    bool update();
    bool remove();
private:
    bool mIsValid = false;
    int mGameId;
    int mQuestionId;
    int mRound;
};

#endif // GAMEHASQUESTION_H
