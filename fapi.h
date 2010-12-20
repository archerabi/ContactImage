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
        int getImage(QString id,QString accessToken);
        int getFBContacts(QString);

        bool isConnected();
        static FApi* Instance();
    signals:
        void fbContactsFetched(QList<Friend*>*);
        void imageRecieved(QImage*,QString,int Token);
        void imageLoading();
        void displayImageName(QString,int);
        void invalidAuthToken();
    public slots:
        void readReply(QNetworkReply*);
        int downloadImage(QString url);

    private:
        QString stripExtensionFromUrl(QString);
        explicit FApi(QObject *parent = 0);
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

        static FApi* instance;

};

#endif // FAPI_H
