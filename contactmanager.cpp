#include "contactmanager.h"
#include <QPixmap>
ContactModel::ContactModel(QObject *parent) :
    QAbstractListModel(parent)
{
    iContactList = new QList<QContact>();
    connect(this,SIGNAL(ready()),this,SLOT(readContacts()));
    emit ready();
}

int ContactModel::rowCount ( const QModelIndex & parent ) const
{
    return iContactList->count();
}

QVariant ContactModel::data ( const QModelIndex & index, int role ) const
{
    switch(role)
    {
        case Qt::DisplayRole:
            QString name;
            name+=iContactList->at(index.row()).detail<QContactName>().firstName();
            name+=" ";
            name+=iContactList->at(index.row()).detail<QContactName>().lastName();
            return name;

    }
    return QVariant();
}

void ContactModel::readContacts()
{
    QContactManager cm;
    QList<QContact> allContacts = cm.contacts();

    emit beginInsertRows(QModelIndex(),0,allContacts.count());
    foreach(QContact contact,allContacts)
    {
        iContactList->append(contact);
    }
    emit endInsertRows();
}

void ContactModel::setThumbnail(int index,const QPixmap* pic)
{
    QContactManager cm;
    QContact contact = iContactList->at(index);
    QContactThumbnail t = contact.detail<QContactThumbnail>();
    t.setThumbnail(pic->toImage());
    contact.saveDetail(&t);
    cm.saveContact(&contact);


}
