#include "mainwindow.h"
#include <QDebug>
#include "ui_mainwindow.h"
#include <QNetworkRequest>
#include <QSettings>
#include <QContactThumbnail>
//#include "qtcontacts.h"
#include <QContactManager>

#include <QContactSaveRequest>
#include "fapi.h"
#include "scrollingComponents/QsWidgets/QsKineticScroller.h"

QTM_USE_NAMESPACE


QString K_AUTH_URL = "https://graph.facebook.com/oauth/authorize?client_id=149457911761543&redirect_uri=http://www.facebook.com/connect/login_success.html&type=user_agent&scope=user_about_me,offline_access&display=wap";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    iLayout = new QVBoxLayout;
    QVBoxLayout* iVLayout = new QVBoxLayout;
        QLabel *label= new QLabel("Phone Contacts",this);
        label->setAlignment(Qt::AlignCenter);
        iAuthFBButton = new QPushButton("Connect Facebook Account",this);
        iAuthFBButton->setFont(QFont("serif",6));
        iInfoLabel = new QLabel("Facebook account not connected.",this);
        iInfoLabel->setFont(QFont("serif",6));
    iVLayout->addWidget(label);
    iVLayout->addWidget(iInfoLabel);
    iVLayout->addWidget(iAuthFBButton);

    synchronizer = new Synchronizer(this);
    synchronizer->readLinks();

    contactsModel =new ContactModel(this);
    contactsModel->readContacts(&cm);
    contactSortModel = new QSortFilterProxyModel(this);
    contactSortModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    contactSortModel->setSourceModel(contactsModel);
    contactSortModel->sort(0);

    iListView = new QListView;
    iListView->setVerticalScrollMode(QListView::ScrollPerPixel);;
    iListView->setModel(contactSortModel);

    QsKineticScroller* scroller = new QsKineticScroller(this);
    scroller->enableKineticScrollFor(iListView);

    iLayout->addLayout(iVLayout);
    iLayout->addWidget(iListView,7);
    QWidget* widget=new QWidget(this);
    widget->setLayout(iLayout);

    stackedWidget = new SlidingStackedWidget(this);


    iWebView = new QWebView(this);

    stackedWidget->addWidget(widget);
    stackedWidget->addWidget(iWebView);

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

    adLabel = new SmaatoAdLabel(this);
    adLabel->setFixedSize(300,50);

    adLabel->setAdSpaceIdAndPublisherId(65735725 , 923832245);
    adLabel->setWidth(300);
    adLabel->setHeight(50);
    adLabel->setAutoGPS(true);
    adLabel->setKeywords("facebook,friends,nokia,ovi");

    adLabel->setSearchString("beyonce,ringtones,tech");

    QWidget* mainwidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(stackedWidget);
    layout->addWidget(adLabel);
    mainwidget->setLayout(layout);
    setCentralWidget(mainwidget);

    connect(this->iAuthFBButton,SIGNAL(clicked()),this,SLOT(startFBAuth()));
    connect(api,SIGNAL(fbContactsFetched(QList<Friend*>*)),fbModel,SLOT(add(QList<Friend*>*)));
       connect(iListView,SIGNAL(clicked(QModelIndex)),this,SLOT(phoneContactClicked(QModelIndex)));
    msgbox = new QProgressDialog(this);

    connect(api,SIGNAL(imageRecieved(QImage*,QString)),this,SLOT(loadImage(QImage*,QString)));
    connect(api,SIGNAL(imageLoading()),this,SLOT(loadingImage()));

    connect(adLabel, SIGNAL(smaatoAdError(int)),
                this, SLOT(smaatoAdErrorSlot(int)));

    int requestBannerErrorCode = adLabel->requestBannerAd(SmaatoAdConstants::EAdTypeImage);
    qDebug() << "ad error " << requestBannerErrorCode;
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

        stackedWidget->slideInPrev();
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
        stackedWidget->slideInNext();
}

void MainWindow::initPhoneContactListView()
{

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
    fbListView->setFont(QFont("serif",5));;

    QsKineticScroller* scroller = new QsKineticScroller(this);
    scroller->enableKineticScrollFor(fbListView);

    fbModel = new FBFriendsModel(this);
    fbSortModel = new QSortFilterProxyModel(this);
    fbSortModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    fbSortModel->setSourceModel(fbModel);
    fbSortModel->sort(0);
    fbSortModel->setDynamicSortFilter(true);

        syncButton = new QPushButton("Sync Image",this);
        syncButton->setEnabled(false);
        syncButton->setFont(QFont("serif",6));
        syncButton->setMinimumSize(QSize(64,64));

       hlayout->addWidget(image);
       hlayout->addWidget(syncButton);

    layout->addWidget(backButton);
    layout->addWidget(infoLabel);
    layout->addWidget(fbListView);
    layout->addLayout(hlayout);
    image->setMinimumSize(200,200);
    QWidget* widget= new QWidget(this);
    widget->setLayout(layout);

    stackedWidget->addWidget(widget);
    connect(fbListView,SIGNAL(clicked(QModelIndex)),this,SLOT(fbFriendClicked(QModelIndex)));
    connect(syncButton,SIGNAL(clicked()),this,SLOT(sync()));
    connect(backButton,SIGNAL(clicked()),this,SLOT(backToPhoneContacts()));
}

void MainWindow::backToPhoneContacts()
{
    stackedWidget->slideInIdx(0);
}

void MainWindow::phoneContactClicked(QModelIndex aIndex)
{
    QSettings settings("Trishul", "contactimage");

    if(settings.value("access_token").toString().length() > 0)
    {
        fbListView->setModel(fbSortModel);
        stackedWidget->slideInIdx(2);
    }
}


void MainWindow::fbFriendClicked(QModelIndex aindex)
{
    QSettings settings("Trishul", "contactimage");
    int fbIndex = fbSortModel->mapToSource(fbListView->currentIndex()).row();
    api->getImage(fbModel->getFriendAt(fbIndex)->getId(),
                  settings.value("access_token").toString());
}
void MainWindow::loadImage(QImage* aImage,QString imageName)
{
    currentImageName = imageName;
    syncButton->setEnabled(true);
    QPixmap pixmap =QPixmap::fromImage(*aImage);

    pixmap.scaled(QSize(360,200),Qt::KeepAspectRatio);
    image->setPixmap(pixmap);
}

void MainWindow::loadingImage()
{
    syncButton->setEnabled(false);
}

void MainWindow::sync()
{
    int contactindex = contactSortModel->mapToSource(iListView->currentIndex()).row();
    int fbIndex = fbSortModel->mapToSource(fbListView->currentIndex()).row();
    QContact c = contactsModel->getContactAt(contactindex);
    if(contactindex > 0 && fbIndex > 0)
    {
        synchronizer->setAvatar(&cm,c,image->pixmap(),currentImageName);
        synchronizer->connectProfile(c,fbModel->getFriendAt(fbIndex)->getId());
        synchronizer->saveLinks(synchronizer->serialize());
        syncButton->setText("Synced");
        syncButton->setEnabled(false);
    }

}

void MainWindow::smaatoAdErrorSlot(int errorCode)
{
    qDebug() << "Errorcode " << errorCode;
    // 42 is an acceptible response from the server to indicate no Ad was found, so don't show an alert in our example application
    if (errorCode != 42)
    {
//        displayErrorNote(errorCode);
    }
}
