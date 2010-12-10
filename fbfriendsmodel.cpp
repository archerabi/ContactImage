#include "fbfriendsmodel.h"
#include <QDebug>
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
    return QVariant();
}

void FBFriendsModel::add(QList<Friend*>* list)
{

   int i=0;
   emit beginInsertRows(QModelIndex(),iFriendList->count(),
                        iFriendList->count()+list->count()-1);
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
