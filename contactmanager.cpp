#include "contactmanager.h"

ContactModel::ContactModel(QObject *parent) :
    QAbstractListModel(parent)
{
    contactList = new QList<QContact>();
}

int ContactModel::rowCount ( const QModelIndex & parent ) const
{
    return contactList->count();
}

QVariant ContactModel::data ( const QModelIndex & index, int role ) const
{
    switch(role)
    {
        case Qt::DisplayRole:
            QString name;
            name+=contactList->at(index.row()).detail<QContactName>().firstName();
            if(name.length()>0)
                name+=" ";
            name+=contactList->at(index.row()).detail<QContactName>().lastName();
            return name;

    }
    return QVariant();
}

void ContactModel::readContacts(QContactManager* cm)
{

    QList<QContact> allContacts = cm->contacts();

    emit beginInsertRows(QModelIndex(),0,allContacts.count());
    foreach(QContact contact,allContacts)
    {
        contactList->append(contact);
    }
    emit endInsertRows();
}

QContact ContactModel::getContactAt(int index)
{
    if(index >= contactList->count() || index < 0)
        return QContact();
    return contactList->at(index);
}
QList<QContact> ContactModel::getContacts()
{
    return *contactList;
}
