#include "synchronizer.h"
#include <QPixmap>
#include <QFile>
#include <QDebug>
#include <QDir>

const QChar token=';';
const QString subToken=",";

const QString KImageStorageFolder = "E:/Images/ContactImage/";

Synchronizer::Synchronizer(QObject *parent) :
    QObject(parent)
{
    QDir dir(KImageStorageFolder);
    dir.mkdir(KImageStorageFolder);

}

void Synchronizer::connectProfile(QContact contact,QString fbFriendId)
{
    QContactName name=contact.detail(QContactName::DefinitionName);
    if(name.firstName().length() > 0)
        profileMap.insert(name.firstName() + " " + name.lastName() ,fbFriendId);
    else
        profileMap.insert(name.lastName() ,fbFriendId);
}

void Synchronizer::setAvatar(QContactManager* cm,QContact aContact,const QPixmap* pic,QString imageName)
{
     QFile file(KImageStorageFolder+imageName);
     file.open(QIODevice::WriteOnly);
     pic->save(&file,"JPG");
     file.close();

    QContact contact = cm->compatibleContact(aContact);

    QContactAvatar av = contact.detail(QContactAvatar::DefinitionName);
    av.setImageUrl(QUrl(KImageStorageFolder+imageName));
    qDebug() << "contact.savedetail" << contact.saveDetail(&av);

    QContactThumbnail t = contact.detail(QContactThumbnail::DefinitionName);
    pic->scaled(QSize(80,80),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    t.setThumbnail(pic->toImage());
    qDebug() << "contact.savedetail" << contact.saveDetail(&t);

    qDebug() << "cm.savecontact" << cm->saveContact(&contact);
     qDebug() << cm->error();
}

QString Synchronizer::serialize()
{
    QString string;
    QMapIterator<QString, QString> i(profileMap);
     while (i.hasNext())
    {
         i.next();
         string += i.key() + subToken + i.value();
         string += token;
    }
     return string;
}
void Synchronizer::saveLinks(QString string)
{
    if(QFile::exists(KImageStorageFolder+"links.txt"))
    {
        QFile::remove(KImageStorageFolder+"links.txt");
    }
    QFile file(KImageStorageFolder+"links.txt");
    file.open(QIODevice::WriteOnly);
    file.write(QByteArray().append(string));
    file.close();
}
void Synchronizer::readLinks()
{
    QFile file(KImageStorageFolder+"links.txt");
    file.open(QIODevice::ReadOnly);
    QByteArray array = file.readAll();
    file.close();

    QString string(array);

    int ptr=0;
    int prevTokenIndex=-1;
    for(ptr=0 ; ptr < string.length() ; ptr++)
    {
        if(string.at(ptr)== token)
        {
            QString link = string.mid(prevTokenIndex + 1,ptr-prevTokenIndex-1);
            int subTokenIndex = link.indexOf(subToken);
            QString contact = link.left(subTokenIndex);
            QString fbFriendId = link.right(link.length() - subTokenIndex-1);

            qDebug() << contact << " " << fbFriendId;
            profileMap.insert(contact,fbFriendId);
            prevTokenIndex=ptr;
        }
    }
}
