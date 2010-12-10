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

#include <QSortFilterProxyModel>

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

        void loadImage(QImage*,QString imageName);
        void sync();
        void backToPhoneContacts();
    private:
        Ui::MainWindow *ui;

        QVBoxLayout* iLayout;
        SlidingStackedWidget* stackedWidget;
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

        QSortFilterProxyModel* contactSortModel;
        QSortFilterProxyModel* fbSortModel;

        QString currentImageName;
};

#endif // MAINWINDOW_H
