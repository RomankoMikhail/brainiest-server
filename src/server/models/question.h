#ifndef QUESTION_H
#define QUESTION_H

#include <QString>

class Question
{
public:
    static Question create(const QString &theme, const QString &question, int type);
    static Question getById(int id);
    static QList<int> getByTheme(QString theme);
    static QStringList getThemes();
    static QList<int> getIds();

    Question() = default;

    QString theme() const;
    void setTheme(const QString &theme);

    QString question() const;
    void setQuestion(const QString &question);

    int type() const;
    void setType(int type);

    bool update();
    bool remove();

    int id() const;

    bool isValid() const;

private:
    bool mIsValid = false;
    int mId       = -1;

    QString mTheme;
    QString mQuestion;
    int mType;
};

#endif // QUESTION_H
