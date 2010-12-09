#ifndef FRIEND_H
#define FRIEND_H

#include <QObject>

class Friend : public QObject
{
    Q_OBJECT
    public:
        explicit Friend(QObject *parent = 0);
        QString getFirstName();
        QString getLastName();
        QString getId();

        void setId(QString);
        void setFirstName(QString);
        void setLastName(QString);

    private:
        QString iFirstName;
        QString iLastName;
        QString iFriendId;

};

#endif // FRIEND_H
