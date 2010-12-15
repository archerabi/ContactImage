#include "fapi.h"
#include <QNetworkRequest>

//#include <QContactThumbnail>
#include "qtcontacts.h"
//#include <QContactManager>

//#include <QContactSaveRequest>
#include <QDebug>

#include <QScriptEngine>
#include <QScriptValueIterator>

const QString KPictureRequestUrl  = "https://graph.facebook.com/%1/picture?type=large";
FApi* FApi::instance = NULL;


FApi::FApi(QObject *parent) :
    QObject(parent)
{
    iNetManager = new QNetworkAccessManager(this);
    connect(iNetManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(readReply(QNetworkReply*)));

    image = new QImage();
    engine = new QScriptEngine(this);
}

int FApi::getImage(QString id,QString accessToken)
{
    QString url=KPictureRequestUrl;
    url = url.arg(id);

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    qDebug() << "URL is " << url;

    QNetworkReply* reply = iNetManager->get(request);
    callingNow =E_IMAGE;
    emit imageLoading();
    return (int)reply;
}

int FApi::downloadImage(QString url)
{
    QNetworkRequest request;
    request.setUrl(url);
    QNetworkReply* reply = iNetManager->get(request);
    return (int)reply;
}

void FApi::readReply(QNetworkReply* aReply)
{
    if(aReply->header(QNetworkRequest::LocationHeader).toString().length() >0)
    {
//        QString string = stripExtensionFromUrl(aReply->header(QNetworkRequest::LocationHeader).toString());
        emit displayImageName(aReply->header(QNetworkRequest::LocationHeader).toString());
    }
    else
    {
        if(callingNow == E_FRIENDS)
        {
            QString reply = aReply->readAll();
            QScriptValue val = engine->evaluate("(" + reply + ")" );
            QList<Friend*>* list = extractFriends(val);

            emit fbContactsFetched(list);
        }
        else
        {
            QByteArray array = aReply->readAll();
            qDebug() << array;
            image->loadFromData(array);


            QString string = aReply->url().toString();

            int i=string.length()-1;
            while(string.at(i)!= '/')
            {
                i--;
            }
            string = string.right(string.length()-i-1);
//            emit filename without the extension
            emit imageRecieved(image,string.left(string.length()-4),(int)aReply);
        }
    }


}

int FApi::getFBContacts(QString accessToken)
{
    QNetworkRequest request;
    request.setUrl(QUrl(KGetFriendsListURL.arg(accessToken)));
    QNetworkReply* reply=iNetManager->get(request);
    callingNow =E_FRIENDS;
    return (int)reply;
}

QList<Friend*>* FApi::extractFriends(QScriptValue& value)
{
   QList<Friend*>* list = new QList<Friend*>();
    if (value.property("data").isArray())
        {
             QScriptValueIterator it(value.property("data"));
             while (it.hasNext())
             {
                 Friend* fr= new Friend;
                 it.next();
                 fr->setFirstName(it.value().property("name").toString());
                 fr->setId(it.value().property("id").toString());
                 *list << fr;
             }
             list->removeLast();
        }
    qDebug() << "Fetched " << list->count() << " friends";
    return list;
}

bool FApi::isConnected()
{

}

FApi* FApi::Instance()
{
    if(!instance)
    {
        instance = new FApi();
    }
    return instance;
}

QString FApi::stripExtensionFromUrl(QString file)
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
