#ifndef ANSWER_H
#define ANSWER_H


#include <QString>

class Answer
{
public:
    Answer() = default;

    static Answer create(QString answer);
    static QList<int> getIds();
    static Answer getById(int id);

    int id() const;

    bool isValid() const;

    QString answer() const;
    void setAnswer(const QString &answer);

    bool update();
private:
    int mId = -1;
    bool mIsValid = false;
    QString mAnswer;
};

#endif // ANSWER_H
