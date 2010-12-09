#include "mainwindow.h"
#include <QDebug>
#include "ui_mainwindow.h"
#include <QNetworkRequest>
#include <QSettings>
#include <QContactThumbnail>
#include "qtcontacts.h"
#include <QContactManager>

#include <QContactSaveRequest>
#include "fapi.h"
QTM_USE_NAMESPACE


QString K_AUTH_URL = "https://graph.facebook.com/oauth/authorize?client_id=149457911761543&redirect_uri=http://www.facebook.com/connect/login_success.html&type=user_agent&scope=user_about_me,offline_access&display=wap";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    iLayout = new QVBoxLayout;
    QHBoxLayout* iHLayout = new QHBoxLayout;
        QLabel *label= new QLabel("Phone Contacts",this);
        label->setAlignment(Qt::AlignCenter);
        iAuthFBButton = new QPushButton("Connect Facebook Account",this);
        iAuthFBButton->setFont(QFont("serif",4));
        iInfoLabel = new QLabel("Facebook account not connected.",this);
        iInfoLabel->setFont(QFont("serif",4));
    iHLayout->addWidget(label);
    iHLayout->addWidget(iInfoLabel);
    iHLayout->addWidget(iAuthFBButton);

    iModel =new ContactModel(this);

    iListView = new QListView;
    iListView->setModel(iModel);

    iLayout->addLayout(iHLayout);
    iLayout->addWidget(iListView,7);
    QWidget* widget=new QWidget(this);
    widget->setLayout(iLayout);

    iStackedWidget = new SlidingStackedWidget(this);


    iWebView = new QWebView(this);

    iStackedWidget->addWidget(widget);
    iStackedWidget->addWidget(iWebView);

    initFBFriendsListView();
    ui->setupUi(this);

    connect(iWebView,SIGNAL(urlChanged(QUrl)),this,SLOT(urlChanged(QUrl)));

    api = new FApi(this);
    QSettings settings("Trishul", "contactimage");
//    settings.clear();
    if(settings.value("access_token").toString().length() > 0)
    {
        iInfoLabel->setVisible(false);
        iAuthFBButton->setVisible(false);
        api->getFBContacts(settings.value("access_token").toString());
    }
    else
    {

    }
    setCentralWidget(iStackedWidget);


    connect(this->iAuthFBButton,SIGNAL(clicked()),this,SLOT(startFBAuth()));
    connect(api,SIGNAL(fbContactsFetched(QList<Friend*>*)),iFbModel,SLOT(add(QList<Friend*>*)));
       connect(iListView,SIGNAL(clicked(QModelIndex)),this,SLOT(phoneContactClicked(QModelIndex)));
    msgbox = new QProgressDialog(this);

    connect(api,SIGNAL(imageRecieved(QImage*)),this,SLOT(loadImage(QImage*)));
    connect(api,SIGNAL(imageLoading()),this,SLOT(loadingImage()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::urlChanged(QUrl aUrl)
{

    if(aUrl.toString().contains("login_success.html#access_token="))
    {
        qDebug() << aUrl.toString();
        int start=aUrl.toString().indexOf("login_success.html#access_token=");
        start+=QString("login_success.html#access_token=").length();
        int end = aUrl.toString().indexOf("&expires_in");

        qDebug() << "auth key is:"<< aUrl.toString().mid(start,end-start);

        QSettings settings("Trishul", "contactimage");
        settings.setValue("access_token",aUrl.toString().mid(start,end-start));

        iStackedWidget->slideInPrev();
        iInfoLabel->setVisible(false);
        iAuthFBButton->setVisible(false);

        api->getFBContacts(settings.value("access_token").toString());


    }
}

void MainWindow::startFBAuth()
{
        iWebView->load(K_AUTH_URL);
    connect(iWebView,SIGNAL(loadFinished(bool)),this,SLOT(showAuthPage(bool)));
}

void MainWindow::showAuthPage(bool)
{
        iStackedWidget->slideInNext();
}

void MainWindow::initPhoneContactListView()
{

}

void MainWindow::initFBFriendsListView()
{
    QVBoxLayout* layout = new QVBoxLayout;
    QHBoxLayout* hlayout = new QHBoxLayout;

    QLabel* infoLabel = new QLabel("Facebooks Friends",this);
    image = new QLabel(this);
    fbListView = new QListView(this);
    iFbModel = new FBFriendsModel(this);

    fbListView->setModel(iFbModel);
        iSyncButton = new QPushButton("Sync Image",this);
        iSyncButton->setEnabled(false);
       hlayout->addWidget(image);
       hlayout->addWidget(iSyncButton);

    layout->addWidget(infoLabel);
    layout->addWidget(fbListView,6);
    layout->addLayout(hlayout,4);
    image->setFixedSize(200,200);
    QWidget* widget= new QWidget(this);
    widget->setLayout(layout);

    iStackedWidget->addWidget(widget);
    connect(fbListView,SIGNAL(clicked(QModelIndex)),this,SLOT(fbFriendClicked(QModelIndex)));
    connect(iSyncButton,SIGNAL(clicked()),this,SLOT(sync()));
}
void MainWindow::phoneContactClicked(QModelIndex)
{
    iStackedWidget->slideInIdx(2);
}


void MainWindow::fbFriendClicked(QModelIndex aindex)
{
    QSettings settings("Trishul", "contactimage");

    api->getImage(iFbModel->getFriendAt(aindex.row())->getId(),
                  settings.value("access_token").toString());
}
void MainWindow::loadImage(QImage* aImage)
{
    iSyncButton->setEnabled(true);
    QPixmap pixmap ;
    qDebug() << "pixmap loading:" << pixmap.convertFromImage(*aImage);
    pixmap.scaled(QSize(360,200),Qt::KeepAspectRatio);
    image->setPixmap(pixmap);
}
void MainWindow::loadingImage()
{
    iSyncButton->setEnabled(false);
}

void MainWindow::sync()
{
    if(iListView->currentIndex().row() > 0 && fbListView->currentIndex().row() > 0)
    {
        iModel->setThumbnail(iListView->currentIndex().row(),image->pixmap());
    }
}
