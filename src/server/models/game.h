#ifndef GAME_H
#define GAME_H

#include <QDateTime>
#include <QString>

class Game
{
public:
    Game() = default;

    static Game create(int authorId, QDateTime date, int isOpen, int isComplete);
    static Game getById(int id);
    static QList<int> getIds();
    static QList<int> getOpenIds();
    static QList<int> getCompletedIds();

    int authorId() const;
    void setAuthorId(int authorId);

    QDateTime date() const;
    void setDate(const QDateTime &date);

    int isOpen() const;
    void setIsOpen(int isOpen);

    int isComplete() const;
    void setIsComplete(int isComplete);

    bool isValid() const;

    int id() const;

    bool update();

private:
    int mId       = -1;
    bool mIsValid = false;

    int mAuthorId;
    QDateTime mDate;
    int mIsOpen;
    int mIsComplete;
};

#endif // GAME_H
