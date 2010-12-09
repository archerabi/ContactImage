#include "fbfriendsmodel.h"

FBFriendsModel::FBFriendsModel(QObject *parent) :
    QAbstractListModel(parent)
{
    iFriendList = new QList<Friend*>();
}


int FBFriendsModel::rowCount ( const QModelIndex & parent  ) const
{
    return iFriendList->count();
}

QVariant FBFriendsModel::data ( const QModelIndex & index, int role ) const
{
    Friend* fr = iFriendList->at(index.row());
    switch(role)
    {
        case Qt::DisplayRole:
            return fr->getFirstName()+ " " + fr->getLastName();
            break;
    }
}

void FBFriendsModel::add(QList<Friend*>* list)
{
    emit beginInsertRows(QModelIndex(),iFriendList->count(),list->count());
    foreach(Friend* f,*list)
    {
        iFriendList->append(f);
    }

    emit endInsertRows();
}

Friend* FBFriendsModel::getFriendAt(int index)
{
    if(index > iFriendList->count())
        return NULL;
    return iFriendList->at(index);
}
