#include <QtCore/QCoreApplication>

#include "./../libs/redStat.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    sstarFile ssRF;
    ssRF.init(QString(argv[1]));
    sstarRes *ssR;

    int i, sz;
    sz = ssRF.ocList.size();

    QFile resFile("sstarRes.txt");
    resFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream resStm(&resFile);

    for(i=0; i<sz; i++)
    {
        ssR = ssRF.ocList.at(i);
        resStm << QString("%1 %2 %3 %4 %5 %6 %7 %8 %9 %10 %11 %12 %13 %14\n").arg(ssR->lspmName.simplified(), -11).arg(ssR->mJD, 13, 'f', 7, QLatin1Char(' ')).arg(ssR->ra, 12, 'f', 8, QLatin1Char(' ')).arg(ssR->de, 12, 'f', 8).arg(ssR->mag, 6, 'f', 2, QLatin1Char(' ')).arg(ssR->ksiOC*cos(grad2rad(ssR->de)), 7, 'f', 1, QLatin1Char(' ')).arg(ssR->etaOC, 7, 'f', 1, QLatin1Char(' ')).arg(ssR->magOC, 6, 'f', 2, QLatin1Char(' ')).arg(ssR->x, 10, 'f', 4, QLatin1Char(' ')).arg(ssR->y, 10, 'f', 4, QLatin1Char(' ')).arg(ssR->pixmag, 11, 'f', 6, QLatin1Char(' ')).arg(ssR->Dx, 7, 'f', 1, QLatin1Char(' ')).arg(ssR->Dy, 7, 'f', 1, QLatin1Char(' ')).arg(ssR->Dpixmag, 10, 'f', 6, QLatin1Char(' '));
    }

    resFile.close();

    return 0;//a.exec();
}
