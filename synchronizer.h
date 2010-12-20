#ifndef SYNCHRONIZER_H
#define SYNCHRONIZER_H

#include <QObject>
#include <QMap>
class QPixmap;

#include <QContactThumbnail>
//#include <QtContacts/QContact>
#include "qtcontacts.h"
#include <QContactManager>
#include <QThread>
QTM_USE_NAMESPACE

class FBFriendsModel;
class ContactModel;

class Synchronizer : public QThread
{
    Q_OBJECT
    public:
        explicit Synchronizer(FBFriendsModel*,ContactModel*,QContactManager*,QObject *parent = 0);
        QString serialize();
        void saveLinks(QString);
        void readLinks();
        void syncContactImages();

        QString getConnectedPhoneContact(QString fbFriendId);
        int getNumberOfConnectedContacts();

        void autoConnect();
        void run();
    signals:
        void syncProgress(int);
    public slots:
        void connectProfile(int contactIndex,QString fbFriendId);
        void setAvatar(int contactIndex,const QPixmap* pic,QString imageName);
    private slots:
        void gotImageName(QString,int);
        void loadImage(QImage*,QString,int);
        QString stripExtensionFromUrl(QString );
        void downloadNextImage();
    private:
        QMap<QString,QString> profileMap;
        int synced;
        int nextptr;
        QMap<int,QString> replyMap;

        FBFriendsModel* fbModel;
        ContactModel* contactModel;
        QContactManager* cm;
        QList<QString> downloadQueue;
};

#endif // SYNCHRONIZER_H
