#ifndef ONGOINGGAME_H
#define ONGOINGGAME_H

#include "order.h"
#include <QDateTime>
#include <QJsonObject>
#include <QMap>
#include <QString>
#include <QVector>
#include <initializer_list>

class CurrentGame
{
public:
    enum States
    {
        Prepare,
        Decrypt,
        TieBreak,

        Round1,

        Round2Select,
        Round2,

        Round3Select,
        Round3,
        End
    };
    QString encrypted(const QString &mEncryptedWord);

    States currentState() const;

    bool load(int gameId);

    bool decrypt(const int &userId, const QString &userAnswer);

    bool answerRound1(const int &userId, const int &userAnswerId);
    bool answerRound2(const int &userId, const QString &userAnswer);
    bool answerRound3(const int &userId, const QString &userAnswer);

    bool selectRound2(const int &userId, const QString &theme);
    bool selectRound3(const int &userId, const int &index);

    void next(const States &newState, std::function<void(void)> function = nullptr);
    void prev();

    void setTime(int seconds);

    QJsonObject status();

    void update();

    void         checkAnswer(const int &userId, const int &questionId, const int &answerId);
    QVector<int> getTopUsers(int top = 6);

    int  gameId() const;
    void setGameId(int gameId);

private:
    QStringList unasweredThemes(const QVector<int> &questionIds);
    QStringList roundThemes(const QVector<int> &questionIds);
    int         firstUnanswered(const QVector<int> &questionIds, const QString &theme = QString());

    bool processAnswerRound1(const int &userId, const int &questionId, const int &userAnswerId);
    bool processAnswerRound2(const int &userId, const QString &userAnswer);
    bool processAnswerRound3(const int &userId, const QString &userAnswer);

    bool processSelectRound2(const int &userId, const QString &theme);
    bool processSelectRound3(const int &userId, const int &index);

    int  nearestAnswer(const int &questionId, const QString &userAnswer);
    bool createAnswer(const int &userId, const int &questionId, const int &answerId);

    bool everyoneAnswered(const int &questionId);
    bool everyoneDecrypted();

    QVector<int> generateIndecies();

    void       notify();
    Order<int> mOrder;

    QDateTime mUntil;

    QMap<int, int>       mUsersScore;
    QMap<int, QDateTime> mUsersDecrypted;
    QString              mWordToDecrypt;

    QVector<int> mRound1;
    QVector<int> mRound2;
    QVector<int> mRound3;

    States mCurrentState = Prepare;
    States mPreviousState = Prepare;

    QString mRound2Theme;

    int mGameId;
    int mRound1Index;
    int mRound3Index;
};

#endif // GAME_H
