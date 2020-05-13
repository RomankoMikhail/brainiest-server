#ifndef QUESTIONHASANSWER_H
#define QUESTIONHASANSWER_H

#include <QString>

class QuestionHasAnswer
{
public:

    static QuestionHasAnswer create(int questionId, int answerId, int correct);
    static QuestionHasAnswer getById(int questionId, int answerId);
    static QList<int> getAnswerIds(int questionId);
    static QList<int> getQuestionIds(int answerId);

    QuestionHasAnswer() = default;

    bool isValid() const;

    int questionId() const;
    int answerId() const;

    int correct() const;
    void setCorrect(int correct);

    bool update();
    bool remove();

private:
    bool mIsValid = false;
    int mQuestionId;
    int mAnswerId;
    int mCorrect;
};

#endif // QUESTIONHASANSWER_H
