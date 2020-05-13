#ifndef USER_H
#define USER_H

#include <QString>

class User
{
public:
    static User create(const QString &name, const QString &surname, const QString &patronymic,
                       const QString &username, const QString &password, const int &isAdmin = 0);

    static User getById(const int &id);
    static User getByUsername(const QString &username);

    static QList<int> getIds();

    User() = default;
    QString name() const;
    void setName(const QString &name);

    QString surname() const;
    void setSurname(const QString &surname);

    QString patronymic() const;
    void setPatronymic(const QString &patronymic);

    QString username() const;

    QString password() const;
    void setPassword(const QString &password);

    int isAdministrator() const;
    void setIsAdministrator(int isAdministrator);

    bool update();

    int id() const;

    bool isValid() const;

private:
    bool mIsValid = false;
    int mId       = -1;
    QString mName;
    QString mSurname;
    QString mPatronymic;
    QString mUsername;
    QString mPassword;
    int mIsAdministrator = 0;
};

#endif // USER_H
