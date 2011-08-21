#include <QtCore>
#include <QCoreApplication>
#include "./../libs/sscatFB.h"

int main(int argc, char *argv[])    //getLSPM.exe LSPMname LSPMcat
{
    QCoreApplication a(argc, argv);

    if(argc!=3) return 1;

    QTextStream stream(stdout);

    sscatFB *starCat = new sscatFB;
    if(starCat->init(argv[2]))
    {
        stream << "err: catalog not found\n";
        return 2;
    }

    if(starCat->GetName(argv[1])==-1)
    {
        stream << "err: star not found\n";
        return 3;
    }

    char *str_out = new char[1024];
    starCat->rec2s(str_out);

    stream << str_out << "\n";



    delete [] str_out;
    return 0;
}
