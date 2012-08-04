#include <QtCore>
#include <QApplication>

#include <QPixmap>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QPixmap bmpFile;

    bmpFile.load("./../../data/images/fantasy/Actual/Кадры 8 разр/10741/map001.bmp");


    return 0;
    //return a.exec();
}
