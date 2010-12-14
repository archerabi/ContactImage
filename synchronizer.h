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

class Synchronizer : public QObject
{
    Q_OBJECT
    public:
        explicit Synchronizer(QObject *parent = 0);
        QString serialize();
        void saveLinks(QString);
        void readLinks();
    signals:

    public slots:
        void connectProfile(QContact contact,QString fbFriendId);
        void setAvatar(QContactManager* cm,QContact contact,const QPixmap* pic,QString imageName);
    private:
        QMap<QString,QString> profileMap;

};

#endif // SYNCHRONIZER_H
