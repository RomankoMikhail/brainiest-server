#ifndef LOGIC_H
#define LOGIC_H

#include <QObject>
#include <QTimer>

class Logic : public QObject
{
private:
    Q_OBJECT
public:
    Logic(QObject *parent = nullptr);
public slots:
    void check();
private:
    QTimer mTimer;
};

#endif // LOGIC_H
