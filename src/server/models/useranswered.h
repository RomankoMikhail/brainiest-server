#ifndef USERANSWERED_H
#define USERANSWERED_H

#include <QString>

class UserAnswered
{
public:
    static UserAnswered create(int userId, int gameId, int questionId, int answerId);
    static UserAnswered getById(int userId, int gameId, int questionId);
    static QList<int> getUsersIds(int gameId, int questionId);

    UserAnswered() = default;
    bool isValid() const;

    bool update();
    bool remove();

    int userId() const;

    int gameId() const;

    int questionId() const;

    int answerId() const;
    void setAnswerId(int answerId);



private:
    bool mIsValid = false;
    int mUserId;
    int mGameId;
    int mQuestionId;
    int mAnswerId;
};

#endif // USERANSWERED_H
