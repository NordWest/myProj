#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    setlocale(LC_NUMERIC, "C");

    MainWindow w;
    w.workPath = a.applicationDirPath();
    w.show();
    
    return a.exec();
}
