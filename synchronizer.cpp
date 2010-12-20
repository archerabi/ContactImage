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

const QString KImageStorageFolder = "E:/Images/RGBA/";

Synchronizer::Synchronizer(FBFriendsModel* aFbModel,ContactModel* aContactModel,
                           QContactManager* aContactManager,QObject *parent) :
    QThread(parent)
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
    qDebug()<<"Saving avatar for "<<
            ((QContactName)(contact.detail(QContactName::DefinitionName))).firstName()<< " "
            << ((QContactName)(contact.detail(QContactName::DefinitionName))).lastName();
    QContactAvatar av = contact.detail(QContactAvatar::DefinitionName);
    av.setImageUrl(QUrl(KImageStorageFolder+imageName));
    contact.saveDetail(&av);

    QContactThumbnail t = contact.detail(QContactThumbnail::DefinitionName);
    pic->scaled(QSize(80,80),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    t.setThumbnail(pic->toImage());
    contact.saveDetail(&t);

    cm->saveContact(&contact);
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
    synced=0;
    if(profileMap.count()==0)
        return;
    nextptr=0;
    connect(FApi::Instance(),SIGNAL(imageRecieved(QImage*,QString,int)),this,SLOT(loadImage(QImage*,QString,int)));
    connect(FApi::Instance(),SIGNAL(displayImageName(QString,int)),this,SLOT(gotImageName(QString,int)));
    downloadNextImage();
}

void Synchronizer::loadImage(QImage*image,QString string,int token)
{
    QString contactName = replyMap.value(token);
    int i=0;
    foreach(QContact contact,contactModel->getContacts())
    {
        QContactName name=contact.detail(QContactName::DefinitionName);
        QString namestr;
        if(name.firstName().length() > 0)
            namestr = name.firstName() + " " + name.lastName() ;
        else
            namestr = name.lastName() ;
        if(namestr.contains(contactName))
        {
            QPixmap p = QPixmap::fromImage(*image);
            setAvatar(i,&p,string);
            synced++;
            emit syncProgress(100 * synced/profileMap.count());
            downloadNextImage();
            return;
        }
        i++;
    }
    downloadNextImage();
}

void Synchronizer::gotImageName(QString imageName,int aToken)
{
    nextptr++;
    if(QFile::exists(KImageStorageFolder+stripExtensionFromUrl(imageName)))
    {
        qDebug() << replyMap.value(aToken) << " " << "Available";
        synced++;
        emit syncProgress(100 * synced/profileMap.count());
        downloadNextImage();
    }
    else
    {
        qDebug() << replyMap.value(aToken) << " " << "Not Available";
        int token = FApi::Instance()->downloadImage(imageName);
        replyMap.insert(token,replyMap.value(aToken));
    }
}

QString Synchronizer::stripExtensionFromUrl(QString file)
{
    QString string = file;

    int i=string.length()-1;
    while(string.at(i)!= '/')
    {
        i--;
    }
    string = string.right(string.length()-i-1);
    return string.left(string.length()-4);
}

QString Synchronizer::getConnectedPhoneContact(QString fbFriendId)
{
    QMapIterator<QString, QString> i(profileMap);
    while (i.hasNext())
    {
        i.next();
        if(QString::compare(i.value(),fbFriendId) == 0)
            return i.key();
    }
    return "";
}

int Synchronizer::getNumberOfConnectedContacts()
{
    return profileMap.count();
}

void Synchronizer::downloadNextImage()
{
    QMapIterator<QString, QString> i(profileMap);
    int temp=-1;
    while(i.hasNext())
    {
        i.next();
        temp++;
        if(temp == nextptr)
        {
            int token = FApi::Instance()->getImage(i.value(),"");
            replyMap.insert(token,i.key());
            qDebug()<<"Downloading image for "<<i.key();
            return;
        }
    }
}

void Synchronizer::autoConnect()
{
    start();
}

void Synchronizer::run()
{
    int contactIndex=0;
    foreach(QContact contact,contactModel->getContacts())
    {
        QString name = ((QContactName)contact.detail(QContactName::DefinitionName)).firstName() + " " +
                       ((QContactName)contact.detail(QContactName::DefinitionName)).lastName();
        foreach(Friend* f,fbModel->getFriends())
        {
            QString fbName = f->getFirstName() + " " + f->getLastName();
            if( name.contains(fbName,Qt::CaseInsensitive) || fbName.contains(name,Qt::CaseInsensitive))
            {
                connectProfile(contactIndex,f->getId());
                break;
            }
        }
        contactIndex++;
    }
    qDebug()<< "Went Through " <<contactIndex ;
}
