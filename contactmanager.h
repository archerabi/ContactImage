#ifndef CONTACTMANAGER_H
#define CONTACTMANAGER_H

#include <QObject>
#include <QAbstractListModel>

#include <QContactThumbnail>
//#include <QtContacts/QContact>
#include "qtcontacts.h"
#include <QContactManager>

QTM_USE_NAMESPACE

class ContactModel : public QAbstractListModel
{
    Q_OBJECT
    public:
        explicit ContactModel(QObject *parent = 0);

        int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
        QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
        void setThumbnail(int index,const QPixmap* pic);
    signals:
        void ready();
    private slots:
        void readContacts();
    private:

        QList<QContact>* iContactList;

};

#endif // CONTACTMANAGER_H
