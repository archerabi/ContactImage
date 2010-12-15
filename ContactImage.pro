#-------------------------------------------------
#
# Project created by QtCreator 2010-09-18T22:19:45
#
#-------------------------------------------------

QT       += core gui webkit network script xml

TARGET = ContactImage
TEMPLATE = app



INCLUDEPATH += C:\NokiaQtSDK\Simulator\QtMobility\mingw\include\QtContacts
               #/home/abhijith/qtmobility/qt-mobility/src/contacts\
               #/home/abhijith/qtmobility/qt-mobility/src/contacts/filters \
               #/home/abhijith/qtmobility/qt-mobility/src/contacts/requests \
               #/home/abhijith/qtmobility/qt-mobility/src/contacts/details

SOURCES += main.cpp\
        mainwindow.cpp \
    fapi.cpp \
    SlidingStackedWidget.cpp \
    contactmanager.cpp \
    friend.cpp \
    fbfriendsmodel.cpp \
    scrollingComponents/SignalGuard/SignalGuard.cpp \
    scrollingComponents/QsWidgets/QsKineticScroller.cpp \
    synchronizer.cpp


HEADERS  += mainwindow.h \
    fapi.h \
    SlidingStackedWidget.h \
    contactmanager.h \
    friend.h \
    fbfriendsmodel.h \
    scrollingComponents/QsWidgets/QsKineticScroller.h \
    scrollingComponents/SignalGuard/SignalGuard.h \
    synchronizer.h


FORMS    += mainwindow.ui

CONFIG += mobility
MOBILITY = contacts

include(./smaatoadsdk/smaatoadsdk.pri)

symbian {
    TARGET.UID3 = 0x200387D3
    TARGET.CAPABILITY += NetworkServices ReadDeviceData ReadUserData WriteUserData Location SwEvent
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x020000 0x1600000
    DEPLOYMENT.installer_header = 0x2002CCCF
    #MMP_RULES -= PAGED
    #MMP_RULES += UNPAGED
}

OTHER_FILES +=

RESOURCES += \
    resource.qrc
