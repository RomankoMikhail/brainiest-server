#ifndef ONGOINGGAME_H
#define ONGOINGGAME_H

#include <QJsonObject>
#include <QMap>
#include <QString>
#include <QVector>
#include <initializer_list>

struct OngoingQuestion
{
    int mQuestionId;
    QList<int> mAnswerIds;
};

template <typename T> class Order
{
public:
    Order()
    {
        mCurrentIndex = 0;
    }

    Order(std::initializer_list<T> l) : mValues(l)
    {
        mCurrentIndex = 0;
    }

    void set(std::vector<T> values)
    {
        mValues = values;
        reset();
    }

    void append(const T &value)
    {
        mValues.push_back(value);
    }

    void clear()
    {
        mValues.clear();
        reset();
    }

    void advance()
    {
        mCurrentIndex++;

        if (mCurrentIndex >= mValues.size())
            mCurrentIndex = 0;
    }

    void reset()
    {
        mCurrentIndex = 0;
    }

    T current() const
    {
        return mValues.at(mCurrentIndex);
    }

private:
    std::vector<T> mValues;
    size_t mCurrentIndex;
};

class OngoingGame
{
public:
    bool answerRound1(const int &userId, const int &userAnswerId);

    bool selectRound2(const int &userId, const QString &theme);
    bool answerRound2(const int &userId, const QString &userAnswer);

    bool selectRound3(const int &userId, const QString &theme);
    bool answerRound3(const int &userId, const QString &userAnswer);

    // bool create(QList<int> userIds, int authorId);
    bool load(int gameId);
    bool answer(const int &userId, const QString &userAnswer);

    QJsonObject currentStatus();

    QString encrypted(const QString &mEncryptedWord);

private:
    int mGameId;

    Order<int> mOrder;

    int nearestAnswer(const int &questionId, const QString &userAnswer);
    bool createAnswer(const int &userId, const int &questionId,
                      const int &answerId);


    void generateOrder();
    // Other




    // Data
    using QuestionData = std::pair<int, bool>;

    QString mRound2Theme;
    int mRound3Number;

    bool mRound2ThemeSelected;
    bool mRound3NumberSelected;

    QVector<QuestionData> mRound1;
    QVector<QuestionData> mRound2;
    QVector<QuestionData> mRound3;

    /*
    QVector<int> mUserAnswered;


    int mCurrentRound1;

    QMap<QString, QVector<int>> mRound2;
    QString mCurrentRound2Theme;
    int mCurrentRound2;

    QVector<int> mRound3;
    QVector<bool> mRound3Solved;

    QVector<int> mCiphers;
    int mCurrentCipher;

    QVector<int> mUserIds;
    int mAuthorId;
    */

    // State mState = Wait;
};

#endif // GAME_H
