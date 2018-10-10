
#include "Program.h"

#include <QtCore/QCoreApplication>
#include <QtGui/QApplication>
#include <QtCore/QTimer>
#include <QtCore/QObject>
#include <QtCore/QDebug>


#ifdef _DEBUG
#pragma comment(lib, "LibtorrentWrapperX86d.lib")
#else
#pragma comment(lib, "LibtorrentWrapperX86.lib")
#endif

int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);
    QApplication a(argc, argv);
    Program program;
    qDebug() << "shutdown connect " << QObject::connect(&a, SIGNAL(aboutToQuit()), &program, SLOT(beforeShutdown()));
    //QTimer::singleShot(0, &program, SLOT(start()));

    return a.exec();
}
