#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QVBoxLayout>
#include "SlidingStackedWidget.h"
#include <QWebView>
#include "contactmanager.h"
#include <QProgressDialog>
#include "fbfriendsmodel.h"
#include "synchronizer.h"
#include <QSortFilterProxyModel>
//#include "SmaatoAdSDK/smaatoadlabel.h"
#include "InnerActiveAd/InnerActiveAdModule.h"
#include "InnerActiveAd/InnerActiveAdWidget.h"
class FApi;
namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

    private:
        void initFBFriendsListView();
    private slots:
        void loadingImage();
        void startFBAuth();
        void finishedLoadingFBAuthPage(bool);
        void  urlChanged(QUrl);
        void phoneContactClicked(QModelIndex);
        void fbFriendClicked(QModelIndex);

        void loadImage(QImage*,QString imageName,int token);
        void gotImageName(QString,int);

        void sync();
        void syncAll();
        void backToPhoneContacts();

        void initMainScreen();
        QWidget* getAuthPage();
        void quit();
        void adAvailable();
        void adderror(QString);

        void autoSyncProgress(int);
        void sort();
        void filter();

        void showFullList();
    private:
        Ui::MainWindow *ui;

        SlidingStackedWidget* stackedWidget;
        QListView* contactListView;
        QWebView* webView;

        ContactModel* contactsModel;
        FBFriendsModel* fbModel;

        QLabel* infoLabel;
        QPushButton* iAuthFBButton;

        QProgressDialog* msgbox;
        FApi* api;

        QPushButton* syncButton;
        QLabel* image ;
        QLabel* connectionStatus;

        QListView* fbListView;

        QSortFilterProxyModel* contactSortModel;
        QSortFilterProxyModel* fbSortModel;

        QString currentImageName;
        Synchronizer* synchronizer;

        QContactManager cm;
        QLabel* webViewStatus;

        QPushButton* syncAllButton;
        QProgressBar* syncAllProgress;
        QStackedWidget* syncAllArea;

        QPushButton* exitButton;
        QPushButton* showFullListButton;
//        SmaatoAdLabel* adLabel;
        InnerActiveAdModule* adModule;
        InnerActiveAdWidget* adWidget;
};

#endif // MAINWINDOW_H
