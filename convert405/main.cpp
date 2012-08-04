#include <QtCore/QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDataStream>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QList>
#include <QDebug>
#include <QSettings>

struct part_head
{
        int npart;
        int ncoeff;
        double T0;
        double T1;
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

//    if(argc!=2) return 1;

    QFile inFile(argv[1]);
 //   QFile inFile("./ascp2000.405");
    inFile.open(QFile::ReadOnly | QIODevice::Text);
    QTextStream iData(&inFile);

//    QFile binFile(argv[2]);
/*    QFile binFile("./binp2000.405");
    binFile.open(QFile::WriteOnly | QFile::Truncate);
    QDataStream oData(&binFile);*/

    FILE *oFile = fopen(argv[2], "wb+");
//    FILE *oFile = fopen("./binp2000.405", "wb+");

    QString sLine, tLine;
    QStringList sElems;
    int iVal, sz, i, q;
    double dVal;
    part_head ph;
    int resw;
    int kSize = 1018;
    double *part = new double[kSize];

    //int shN = 1;
    q=0;
    while(!iData.atEnd())
    {
 //       qDebug() << ++q << "\n";

        sLine = iData.readLine();
        sElems = sLine.split(" ", QString::SkipEmptyParts);
        sz = sElems.size();
        if(sz==2)
        {
            if(q) fwrite(part, sizeof(double)*1016, 1, oFile);

            q=0;
            tLine = sElems.at(0);
            iVal = tLine.toInt();
            ph.npart = iVal;
//            oData << iVal;
 //           fwrite(&iVal, sizeof(int), 1, oFile);
            tLine = sElems.at(1);
            iVal = tLine.toInt();
            ph.ncoeff = iVal;
//            oData << iVal;
 //           fwrite(&iVal, sizeof(int), 1, oFile);

            sLine = iData.readLine();
            sElems = sLine.split(" ", QString::SkipEmptyParts);

            tLine = sElems.at(0);
            dVal = tLine.toDouble();
            ph.T0 = dVal;

            tLine = sElems.at(1);
            dVal = tLine.toDouble();
            ph.T1 = dVal;
//                oData << dVal;
            resw = fwrite(&ph, sizeof(part_head), 1, oFile);
            if(resw!=1) qDebug() << QString("%1").arg(q);
//            oData << ph;
            tLine = sElems.at(2);
            dVal = tLine.toDouble();
            part[q] = dVal;
            q++;
//            resw = fwrite(&dVal, sizeof(double), 1, oFile);
//            if(resw!=1) qDebug() << QString("%1").arg(q);
//            oData << dVal;
        }
        else
        {
            for(i=0; i<sz; i++)
            {
                tLine = sElems.at(i);
                dVal = tLine.toDouble();

                part[q] = dVal;
                q++;
//                resw = fwrite(&dVal, sizeof(double), 1, oFile);
//                if(resw!=1) qDebug() << QString("%1").arg(q);
//                oData << dVal;
            }
        }
//        q++;
    }

    return 0;
//    return a.exec();
}
