#ifndef CIPHERS_H
#define CIPHERS_H

#include <QString>
#include <QList>

class Ciphers
{
public:
    Ciphers() = default;

    static Ciphers create(QString word);
    static QList<int> getIds();
    static Ciphers getById(int id);

    int id() const;

    bool isValid() const;

    QString word() const;
    void setWord(const QString &word);

    bool update();

private:
    int mId       = -1;
    bool mIsValid = false;
    QString mWord;
};

#endif // CIPHERS_H
