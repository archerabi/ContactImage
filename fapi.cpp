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



FApi::FApi(QObject *parent) :
    QObject(parent)
{
    iNetManager = new QNetworkAccessManager(this);
    connect(iNetManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(readReply(QNetworkReply*)));

    image = new QImage();
    engine = new QScriptEngine(this);
}

void FApi::getImage(QString id,QString accessToken)
{
    QString url=KPictureRequestUrl;
    url = url.arg(id);
//          .arg(accessToken);

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    qDebug() << "URL is " << url;

    iNetManager->get(request);
    callingNow =E_IMAGE;
    emit imageLoading();
}

void FApi::readReply(QNetworkReply* aReply)
{
    if(aReply->header(QNetworkRequest::LocationHeader).toString().length() >0)
    {
        QNetworkRequest request;
        request.setUrl(aReply->header(QNetworkRequest::LocationHeader).toString());

        qDebug() << "Redirect:" << aReply->header(QNetworkRequest::LocationHeader).toString();
        iNetManager->get(request);
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
//            emit imageRecieved(image,string);
            emit imageRecieved(image,string.left(string.length()-4));
        }
    }


}

void FApi::getFBContacts(QString accessToken)
{
    QNetworkRequest request;
    request.setUrl(QUrl(KGetFriendsListURL.arg(accessToken)));
    iNetManager->get(request);
    callingNow =E_FRIENDS;
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

        }
    qDebug() << "Fetched " << list->count() << " friends";
    return list;
}

bool FApi::isConnected()
{

}
