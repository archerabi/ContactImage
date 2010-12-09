#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    qDebug()<< "double:" << sizeof(double);
    qDebug()<< "double:" << sizeof(double);
    qDebug()<< "uint:" << sizeof(uint);
//    qDebug()<< "uint32:" << sizeof(uint32_t);

    MainWindow w;
#if defined(Q_WS_S60)
    w.showMaximized();
#else
    w.show();
#endif

    return a.exec();
}
