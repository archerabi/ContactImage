#include "friend.h"

Friend::Friend(QObject *parent) :
    QObject(parent)
{
}

QString Friend::getFirstName()
{
    return iFirstName;
}

QString Friend::getLastName()
{
    return iLastName;
}

QString Friend::getId()
{
    return iFriendId;
}

void Friend::setId(QString Id)
{
    iFriendId =Id;
}

void Friend::setFirstName(QString name)
{
    iFirstName = name;
}

void Friend::setLastName(QString name)
{
    iLastName = name;
}
