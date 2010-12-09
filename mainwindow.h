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
        void initPhoneContactListView();
        void initFBFriendsListView();
    private slots:
        void loadingImage();
        void startFBAuth();
        void showAuthPage(bool);
        void  urlChanged(QUrl);
        void phoneContactClicked(QModelIndex);
        void fbFriendClicked(QModelIndex);

        void loadImage(QImage*);
        void sync();
    private:
        Ui::MainWindow *ui;

        QVBoxLayout* iLayout;
        SlidingStackedWidget* iStackedWidget;
        QListView* iListView;
        QWebView* iWebView;

        ContactModel* iModel;
        FBFriendsModel* iFbModel;

        QLabel* iInfoLabel;
        QPushButton* iAuthFBButton;

        QProgressDialog* msgbox;
        FApi* api;

        QPushButton* iSyncButton;
        QLabel* image ;

        QListView* fbListView;

};

#endif // MAINWINDOW_H
