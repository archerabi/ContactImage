#ifndef FAPI_H
#define FAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "friend.h"
#include <QScriptValue>

const QString KGetFriendsListURL =
        "https://graph.facebook.com/me/friends?access_token=%1";
class FApi : public QObject
{
    Q_OBJECT
    public:
        explicit FApi(QObject *parent = 0);
        void getImage(QString id,QString accessToken);
        void getFBContacts(QString);

        bool isConnected();
    signals:
        void fbContactsFetched(QList<Friend*>*);
        void imageRecieved(QImage*,QString);
        void imageLoading();
    public slots:
        void readReply(QNetworkReply*);
    private:
        QList<Friend*>* extractFriends(QScriptValue&);

        QNetworkAccessManager* iNetManager;
        QImage* image;
        QScriptEngine* engine;
        int callingNow;
        enum ECall
        {
            E_FRIENDS,
            E_IMAGE
        };

};

#endif // FAPI_H
