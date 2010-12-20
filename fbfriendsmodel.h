#ifndef FBFRIENDSMODEL_H
#define FBFRIENDSMODEL_H

#include <QAbstractListModel>
#include "friend.h"

class FBFriendsModel : public QAbstractListModel
{
        Q_OBJECT
    public:
        explicit FBFriendsModel(QObject *parent = 0);
        int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
        QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

        Friend* getFriendAt(int);
        QList<Friend*> getFriends();
    signals:

    public slots:
          void add(QList<Friend*>* list);
    private:
        QList<Friend*>* friendList;
};

#endif // FBFRIENDSMODEL_H
