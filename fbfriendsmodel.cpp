#include "fbfriendsmodel.h"
#include <QDebug>
FBFriendsModel::FBFriendsModel(QObject *parent) :
    QAbstractListModel(parent)
{
    friendList = new QList<Friend*>();
}


int FBFriendsModel::rowCount ( const QModelIndex & parent  ) const
{
    return friendList->count();
}

QVariant FBFriendsModel::data ( const QModelIndex & index, int role ) const
{
    Friend* fr = friendList->at(index.row());

    switch(role)
    {
        case Qt::DisplayRole:
            return fr->getFirstName()+ " " + fr->getLastName();
            break;
    }
    return QVariant();
}

void FBFriendsModel::add(QList<Friend*>* list)
{

   int i=0;
   emit beginInsertRows(QModelIndex(),friendList->count(),
                        friendList->count()+list->count()-1);
   foreach(Friend* f,*list)
    {
        friendList->append(f);
    }
   emit endInsertRows();
}

Friend* FBFriendsModel::getFriendAt(int index)
{
    if(index > friendList->count())
        return NULL;
    return friendList->at(index);
}

QList<Friend*> FBFriendsModel::getFriends()
{
    return *friendList;
}
