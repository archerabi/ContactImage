#include "contactmanager.h"
#include <QPixmap>
#include <QFile>
#include <QDebug>
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

void ContactModel::setAvatar(int index,const QPixmap* pic,QString imageName)
{
     QFile file("E:/"+imageName);
     file.open(QIODevice::WriteOnly);
     pic->save(&file);
     file.close();

    QContactManager cm;
    QContact contact = iContactList->at(index);

    contact = cm.compatibleContact(contact);

    QContactAvatar av = contact.detail(QContactAvatar::DefinitionName);
    av.setImageUrl(QUrl("E:/"+imageName));
    qDebug() << "contact.savedetail" << contact.saveDetail(&av);

    QContactThumbnail t = contact.detail(QContactThumbnail::DefinitionName);
    pic->scaled(QSize(80,80),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    t.setThumbnail(pic->toImage());
    qDebug() << "contact.savedetail" << contact.saveDetail(&t);



    qDebug() << "cm.savecontact" << cm.saveContact(&contact);
     qDebug() << cm.error();
}
