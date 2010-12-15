#include "synchronizer.h"
#include <QPixmap>
#include <QFile>
#include <QDebug>
#include <QDir>
#include "fapi.h"
#include "fbfriendsmodel.h"
#include "contactmanager.h"
const QChar token=';';
const QString subToken=",";

const QString KImageStorageFolder = "E:/Images/ContactImage/";

Synchronizer::Synchronizer(FBFriendsModel* aFbModel,ContactModel* aContactModel,
                           QContactManager* aContactManager,QObject *parent) :
    QObject(parent)
{
    QDir dir(KImageStorageFolder);
    dir.mkdir(KImageStorageFolder);
    fbModel = aFbModel;
    contactModel = aContactModel;
    cm = aContactManager;
}

void Synchronizer::connectProfile(int contactIndex,QString fbFriendId)
{
    QContact contact = contactModel->getContactAt(contactIndex);
    QContactName name=contact.detail(QContactName::DefinitionName);
    if(name.firstName().length() > 0)
        profileMap.insert(name.firstName() + " " + name.lastName() ,fbFriendId);
    else
        profileMap.insert(name.lastName() ,fbFriendId);
}

void Synchronizer::setAvatar(int contactIndex,const QPixmap* pic,QString imageName)
{
    QFile file(KImageStorageFolder+imageName);
    file.open(QIODevice::WriteOnly);
    pic->save(&file,"JPG");
    file.close();

    QContact contact = contactModel->getContactAt(contactIndex);

//    QContact contact = cm->compatibleContact(aContact);
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

void Synchronizer::syncContactImages()
{
    connect(FApi::Instance(),SIGNAL(imageRecieved(QImage*,QString,int)),this,SLOT(loadImage(QImage*,QString,int)));
    QMapIterator<QString, QString> i(profileMap);
    synced=0;
    while (i.hasNext())
    {
        i.next();
        int token = FApi::Instance()->getImage(i.value(),"");
        replyMap.insert(token,i.key());
    }
}

void Synchronizer::loadImage(QImage*image,QString string,int token)
{
    if(replyMap.contains(token))
    {

    }
}
