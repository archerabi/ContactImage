#-------------------------------------------------
#
# Project created by QtCreator 2010-09-18T22:19:45
#
#-------------------------------------------------

QT       += core gui webkit network script

TARGET = ContactImage
TEMPLATE = app



INCLUDEPATH += . \
               /home/abhijith/qtmobility/qt-mobility/src/contacts\
               /home/abhijith/qtmobility/qt-mobility/src/contacts/filters \
               /home/abhijith/qtmobility/qt-mobility/src/contacts/requests \
               /home/abhijith/qtmobility/qt-mobility/src/contacts/details

SOURCES += main.cpp\
        mainwindow.cpp \
    fapi.cpp \
    SlidingStackedWidget.cpp \
    contactmanager.cpp \
    friend.cpp \
    fbfriendsmodel.cpp

HEADERS  += mainwindow.h \
    fapi.h \
    SlidingStackedWidget.h \
    contactmanager.h \
    friend.h \
    fbfriendsmodel.h

FORMS    += mainwindow.ui

CONFIG += mobility
MOBILITY = contacts

symbian {
    TARGET.UID3 = 0xe016bc7d
    TARGET.CAPABILITY += NetworkServices
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x020000 0x800000
}
