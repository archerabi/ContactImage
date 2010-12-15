#ifndef SYNCHRONIZER_H
#define SYNCHRONIZER_H

#include <QObject>
#include <QMap>
class QPixmap;

#include <QContactThumbnail>
//#include <QtContacts/QContact>
#include "qtcontacts.h"
#include <QContactManager>

QTM_USE_NAMESPACE

class FBFriendsModel;
class ContactModel;

class Synchronizer : public QObject
{
    Q_OBJECT
    public:
        explicit Synchronizer(FBFriendsModel*,ContactModel*,QContactManager*,QObject *parent = 0);
        QString serialize();
        void saveLinks(QString);
        void readLinks();
        void syncContactImages();

    signals:
        void syncProgress(int);
    public slots:
        void connectProfile(int contactIndex,QString fbFriendId);
        void setAvatar(int contactIndex,const QPixmap* pic,QString imageName);
    private slots:
        void loadImage(QImage*,QString,int);
    private:
        QMap<QString,QString> profileMap;
        int synced;
        QMap<int,QString> replyMap;

        FBFriendsModel* fbModel;
        ContactModel* contactModel;
        QContactManager* cm;
};

#endif // SYNCHRONIZER_H
