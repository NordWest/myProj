#include <QtGui/QApplication>
#include "setupmarkscolor.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    setupMarksColor w;
    w.show();

    return a.exec();
}
