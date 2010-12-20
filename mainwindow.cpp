#include "mainwindow.h"
#include <QDebug>
#include "ui_mainwindow.h"
#include <QNetworkRequest>
#include <QSettings>
#include <QContactThumbnail>
//#include "qtcontacts.h"
#include <QContactManager>
#include <QMessageBox>
#include <QContactSaveRequest>
#include "fapi.h"
#include "scrollingComponents/QsWidgets/QsKineticScroller.h"

QTM_USE_NAMESPACE

QString K_AUTH_URL = "https://graph.facebook.com/oauth/authorize?client_id=149457911761543&redirect_uri=http://www.facebook.com/connect/login_success.html&type=user_agent&scope=user_about_me,offline_access&display=wap";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qDebug() <<QString("Chris Nielubowicz").contains("Chris Nielubowicz",Qt::CaseInsensitive);
    adModule = new InnerActiveAdModule(this,"Reddy_RGBA",30);
//    adModule->setAdProtocolParameter( EIaProtocolParams_DistributionChannelPortal, QString("551"));
    adModule->setAdProtocolParameter( EIaProtocolParams_Keywords, "ovi,nokia,facebook,contacts,images");

    adWidget = adModule->createAdBanner(this,true);

    connect(adModule,SIGNAL(adDataReady()),this,SLOT(adAvailable()));
    connect(adModule,SIGNAL(networkError(QString)),this,SLOT(adderror(QString)));

    adModule->requestAd();
    contactsModel =new ContactModel(this);
    contactsModel->readContacts(&cm);
    contactSortModel = new QSortFilterProxyModel(this);
    contactSortModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    contactSortModel->setSourceModel(contactsModel);
    contactSortModel->sort(0);

    initMainScreen();

    initFBFriendsListView();

    synchronizer = new Synchronizer(fbModel,contactsModel,&cm,this);
    synchronizer->readLinks();

    ui->setupUi(this);

    connect(webView,SIGNAL(urlChanged(QUrl)),this,SLOT(urlChanged(QUrl)));

    api = FApi::Instance();

    QSettings settings("Trishul", "RGBA");
//    settings.clear();
    if(settings.value("access_token").toString().length() > 0)
    {
        infoLabel->setVisible(false);
        iAuthFBButton->setVisible(false);
        api->getFBContacts(settings.value("access_token").toString());
    }
    else
    {

    }
    setCentralWidget(stackedWidget);

    connect(this->iAuthFBButton,SIGNAL(clicked()),this,SLOT(startFBAuth()));
    connect(api,SIGNAL(fbContactsFetched(QList<Friend*>*)),fbModel,SLOT(add(QList<Friend*>*)));
    connect(api,SIGNAL(invalidAuthToken()),this,SLOT(startFBAuth()));
       connect(contactListView,SIGNAL(clicked(QModelIndex)),this,SLOT(phoneContactClicked(QModelIndex)));
    msgbox = new QProgressDialog(this);

    connect(api,SIGNAL(imageLoading()),this,SLOT(loadingImage()));
    connect(showFullListButton,SIGNAL(clicked()),this,SLOT(showFullList()));
//    adLabel->requestBannerAd(SmaatoAdConstants::EAdTypeImage);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete FApi::Instance();
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

        QSettings settings("Trishul", "RGBA");
        settings.setValue("access_token",aUrl.toString().mid(start,end-start));

        stackedWidget->slideInPrev();
        infoLabel->setVisible(false);
        iAuthFBButton->setVisible(false);

        api->getFBContacts(settings.value("access_token").toString());
    }
}

void MainWindow::startFBAuth()
{
    webViewStatus->setText("Loading Facebook.com ...");
    stackedWidget->slideInNext();
    connect(webView,SIGNAL(loadFinished(bool)),this,SLOT(finishedLoadingFBAuthPage(bool)));
    webView->load(K_AUTH_URL);
}

void MainWindow::finishedLoadingFBAuthPage(bool)
{
    webViewStatus->setText("");
}

void MainWindow::initFBFriendsListView()
{
    QVBoxLayout* layout = new QVBoxLayout;
    QHBoxLayout* hlayout = new QHBoxLayout;

    QPushButton* backButton = new QPushButton("Back",this);
    QLabel* infoLabel = new QLabel("Facebooks Friends",this);
    image = new QLabel(this);

    QPixmap* p = new QPixmap(":/resources/default.jpg");
    image->setPixmap(*p);
    fbListView = new QListView(this);
    fbListView->setMinimumSize(QSize(360,240));
    fbListView->setVerticalScrollMode(QListView::ScrollPerPixel);;

    QsKineticScroller* scroller = new QsKineticScroller(this);
    scroller->enableKineticScrollFor(fbListView);

    fbModel = new FBFriendsModel(this);
    fbSortModel = new QSortFilterProxyModel(this);

    sort();

        QVBoxLayout* syncLayout = new QVBoxLayout;
        syncButton = new QPushButton("Sync Image",this);
        syncButton->setEnabled(false);
        syncButton->setFont(QFont("serif",6));
        syncButton->setMinimumSize(QSize(64,64));

        showFullListButton = new QPushButton("Show full list",this);
        showFullListButton->setMaximumHeight(30);

        connectionStatus = new QLabel(this);
        connectionStatus->setWordWrap(true);
        connectionStatus->setFont(QFont("serif",5));
        syncLayout->addWidget(connectionStatus);
        syncLayout->addWidget(syncButton);
        syncLayout->setContentsMargins(5,5,5,5);
       hlayout->addWidget(image);
       hlayout->addLayout(syncLayout);
       hlayout->setContentsMargins(5,5,5,5);

    layout->addWidget(backButton);
    layout->addWidget(infoLabel);
    layout->addWidget(fbListView);
    layout->addWidget(showFullListButton);
    layout->addLayout(hlayout);
    image->setMinimumSize(200,200);
    QWidget* widget= new QWidget(this);
    widget->setLayout(layout);

    stackedWidget->addWidget(widget);
    connect(fbListView,SIGNAL(clicked(QModelIndex)),this,SLOT(fbFriendClicked(QModelIndex)));
    connect(syncButton,SIGNAL(clicked()),this,SLOT(sync()));
    connect(backButton,SIGNAL(clicked()),this,SLOT(backToPhoneContacts()));
    connect(syncAllButton,SIGNAL(clicked()),this,SLOT(syncAll()));
    connect(exitButton,SIGNAL(clicked()),this,SLOT(quit()));
}

void MainWindow::backToPhoneContacts()
{
    stackedWidget->slideInIdx(0);
    syncButton->setText("Sync");
    syncButton->setEnabled(true);
}

void MainWindow::phoneContactClicked(QModelIndex aIndex)
{
    QSettings settings("Trishul", "RGBA");
    if(settings.value("access_token").toString().length() > 0)
    {
        image->setPixmap(QPixmap(":/resources/default.jpg"));
        int contactindex = contactSortModel->mapToSource(contactListView->currentIndex()).row();
        QContact c = contactsModel->getContactAt(contactindex);
        QContactName contact = (QContactName)c.detail(QContactName::DefinitionName);
        QString contactName = contact.firstName() + " " +
                              contact.lastName();
        filter();
        fbSortModel->setFilterRegExp(QRegExp(contactName, Qt::CaseInsensitive,
                                                  QRegExp::FixedString));

        showFullListButton->setVisible(true);
        if(fbSortModel->rowCount(QModelIndex()) == 0)
        {
            fbSortModel->setFilterRegExp(QRegExp(contact.firstName(), Qt::CaseInsensitive,
                                                      QRegExp::FixedString));
            QString x= contact.firstName();
            if(contact.firstName().length() <= 1 || fbSortModel->rowCount(QModelIndex()) == 0)
            {
                fbSortModel->setFilterRegExp(QRegExp(contact.lastName(), Qt::CaseInsensitive,
                                                          QRegExp::FixedString));
                if(contact.lastName().length() <= 1 || fbSortModel->rowCount(QModelIndex()) == 0)
                {
                    sort();
                    showFullListButton->setVisible(false);
                }
            }
        }

        connect(api,SIGNAL(imageRecieved(QImage*,QString,int)),this,SLOT(loadImage(QImage*,QString,int)));
        fbListView->setModel(fbSortModel);
        stackedWidget->slideInIdx(2);
    }
}

void MainWindow::fbFriendClicked(QModelIndex aindex)
{
    connect(api,SIGNAL(displayImageName(QString,int)),this,SLOT(gotImageName(QString,int)));
    QSettings settings("Trishul", "RGBA");
    int fbIndex = fbSortModel->mapToSource(fbListView->currentIndex()).row();
    Friend* friendPtr =fbModel->getFriendAt(fbIndex);
    api->getImage(friendPtr->getId(),
                  settings.value("access_token").toString());

    QString connectedContactName = synchronizer->getConnectedPhoneContact(friendPtr->getId());
    if(connectedContactName.length() > 0)
    {
        connectionStatus->setText(friendPtr->getFirstName()+" is connected to "+connectedContactName);
    }
    else
    {
        connectionStatus->setText("");
    }
}
void MainWindow::gotImageName(QString url,int)
{
    disconnect(api,SIGNAL(displayImageName(QString,int)),this,SLOT(gotImageName(QString,int)));
    connect(api,SIGNAL(imageRecieved(QImage*,QString,int)),this,SLOT(loadImage(QImage*,QString,int)));
    api->downloadImage(url);
}

void MainWindow::loadImage(QImage* aImage,QString imageName,int)
{
    currentImageName = imageName;
    syncButton->setEnabled(true);
    QPixmap pixmap =QPixmap::fromImage(*aImage);

    pixmap.scaled(QSize(360,200),Qt::KeepAspectRatio);
    image->setPixmap(pixmap);
    disconnect(api,SIGNAL(imageRecieved(QImage*,QString,int)),this,SLOT(loadImage(QImage*,QString,int)));
}

void MainWindow::loadingImage()
{
    syncButton->setEnabled(false);
}

void MainWindow::sync()
{
    int contactindex = contactSortModel->mapToSource(contactListView->currentIndex()).row();
    int fbIndex = fbSortModel->mapToSource(fbListView->currentIndex()).row();
    if(contactindex > 0 && fbIndex > 0)
    {
        synchronizer->setAvatar(contactindex,image->pixmap(),currentImageName);
        synchronizer->connectProfile(contactindex,fbModel->getFriendAt(fbIndex)->getId());
        synchronizer->saveLinks(synchronizer->serialize());

        syncButton->setText("Synced");
        syncButton->setEnabled(false);
    }
}

void MainWindow::initMainScreen()
{
    QVBoxLayout* layout = new QVBoxLayout;
    QVBoxLayout* vLayout = new QVBoxLayout;
        QLabel *label= new QLabel("Select a Contact to Download Image",this);
        label->setAlignment(Qt::AlignCenter);
        label->setWordWrap(true);

        iAuthFBButton = new QPushButton("Connect Facebook Account",this);
        iAuthFBButton->setFont(QFont("serif",6));
        infoLabel = new QLabel("Facebook account not connected.",this);
        infoLabel->setFont(QFont("serif",6));

    vLayout->addWidget(label);
    vLayout->addWidget(infoLabel);
    vLayout->addWidget(iAuthFBButton);

    exitButton = new QPushButton("Quit",this);
    contactListView = new QListView;
    contactListView->setVerticalScrollMode(QListView::ScrollPerPixel);;
    contactListView->setModel(contactSortModel);

    QsKineticScroller* scroller = new QsKineticScroller(this);
    scroller->enableKineticScrollFor(contactListView);

    syncAllArea = new QStackedWidget(this);
    syncAllButton = new QPushButton("Sync all connected contacts.");
    syncAllProgress = new QProgressBar(this);
    syncAllProgress->setRange(0,100);
    syncAllArea->addWidget(syncAllButton);
    syncAllArea->addWidget(syncAllProgress);
    syncAllArea->setCurrentWidget(syncAllButton);
    syncAllArea->setMaximumHeight(40);

//    adLabel = new SmaatoAdLabel(this);
//    adLabel->setAdSpaceIdAndPublisherId(65735725,923832245);
//    adLabel->setFixedSize(360,50);
//    adLabel->setSearchString("nokia,ovi,symbian.facebook,social,pictures");
//    adLabel->setKeywords("nokia,ovi,symbian.facebook,social,pictures");
//    adLabel->setAutoGPS(true);

    layout->addWidget(exitButton);
    layout->addLayout(vLayout);
    layout->addWidget(contactListView,7);
    layout->addWidget(syncAllArea);
    layout->addWidget(adWidget);

    QWidget* widget=new QWidget(this);
    widget->setLayout(layout);

    stackedWidget = new SlidingStackedWidget(this);
    stackedWidget->addWidget(widget);
    stackedWidget->addWidget(getAuthPage());
}

QWidget* MainWindow::getAuthPage()
{
    QVBoxLayout* layout = new QVBoxLayout;
    webView = new QWebView(this);
    webViewStatus = new QLabel();
    webViewStatus->setFixedSize(360,30);
    layout->addWidget(webView);
    layout->addWidget(webViewStatus);

    QWidget* widget = new QWidget;
    widget->setLayout(layout);
    return widget;
}
void MainWindow::syncAll()
{
//    synchronizer->autoConnect();
    QMessageBox msgBox;
     msgBox.setText("This will replace existing contact images.Continue?");
     msgBox.setStandardButtons(QMessageBox::Ok |QMessageBox::Cancel);
     msgBox.setDefaultButton(QMessageBox::Save);
     int ret = msgBox.exec();
     if(ret == QDialog::Accepted)
     {
        connect(synchronizer,SIGNAL(syncProgress(int)),this,SLOT(autoSyncProgress(int)));
        syncAllProgress->setValue(0);
        syncAllArea->setCurrentWidget(syncAllProgress);
        synchronizer->syncContactImages();
    }
}

void MainWindow::quit()
{
    QApplication::quit();
}

void MainWindow::adAvailable()
{
    adModule->updateBanner(adWidget);
}
void MainWindow::adderror(QString string)
{
    qDebug() << string;
}

void MainWindow::autoSyncProgress(int progress)
{
    syncAllProgress->setValue(progress);
    if(progress == 100)
    {
        syncAllButton->setText("Synced " + QString().setNum(synchronizer->getNumberOfConnectedContacts())+" Images !");
        syncAllArea->setCurrentWidget(syncAllButton);
    }
}

void MainWindow::sort()
{
    fbSortModel->setFilterKeyColumn(-1);
    fbSortModel->setFilterRegExp("");
    fbSortModel->setSourceModel(fbModel);
    fbSortModel->setSortCaseSensitivity(Qt::CaseInsensitive);

    fbSortModel->sort(0);
    fbSortModel->setDynamicSortFilter(true);
}
void MainWindow::filter()
{
    fbSortModel->sort(-1);
    fbSortModel->setSourceModel(fbModel);
    fbSortModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    fbSortModel->setFilterKeyColumn(0);
    fbSortModel->setDynamicSortFilter(true);
}
void MainWindow::showFullList()
{
    sort();
    showFullListButton->setVisible(false);
}
