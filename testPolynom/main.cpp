#include <QtCore/QCoreApplication>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QStringList>
#include <QDebug>
#include "math.h"
#include "mb.h"

int detCurf3(QString *resFunc, double *Zi, double *x, double *y, int num, int deg);


int main(int argc, char *argv[])    //testPolynom.exe data.txt degree
{
//    QCoreApplication a(argc, argv);

    QString fName = QString(argv[1]);
    int deg = QString(argv[2]).toInt();

    int i;

    QVector <double> X, Y;
//    QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate

    QFile dFile(fName);
    dFile.open(QIODevice::Text | QIODevice::ReadOnly);
    QTextStream dataStream(&dFile);

    QString dStr;

    while(!dataStream.atEnd())
    {
        dStr = dataStream.readLine();
        X << dStr.section("|", 0, 0).toDouble();
        Y << dStr.section("|", 1, 1).toDouble();
    }

    int sz = X.size();

    double *x = new double[sz];
    double *y = new double[sz];

    x = X.data();
    y = Y.data();

    double *Zi = new double[deg+1];

    QString resFunc;
    detCurf3(&resFunc, Zi, x, y, sz, deg);

    printf("%s\n", resFunc.toAscii().data());

    for(i=0; i<deg+1; i++)
    {
        printf("%e ", Zi[i]);
    }

    printf("\n");


    return 0;
//    return a.exec();
}


int detCurf3(QString *resFunc, double *Zi, double *x, double *y, int num, int deg)
{
    int i, j, k;
    deg++;
    double *z = new double[deg];
    double *c = new double[deg*num];
    double *we = new double[num];

    int *EXCLIND;
    double *D = new double[deg*deg];
    int RN;
    //QStringList tStrL;

    for(i=0; i<num; i++)
    {
        //tStrL.clear();

        for(j=0; j<deg; j++)
        {
            c[i*deg+j] = pow(x[i], deg-j-1);

            //tStrL << QString("Z0*%1").arg(c[i+j]);
        }

        we[i] = 1;
    }

    EXCLIND = new int [num];
    D=new double[deg*deg];
    RN=0;
    double UWE;
    //iLSM(deg, num, 500, EXCLIND, z, c, y, UWE, D, 3, RN, we);
    //lsm(deg, num, z, c, y, UWE, D, we);
    slsm(deg, num, z, c, y);

    QStringList opers;
    QStringList opers1;

    qDebug() << QString("RN: %1\tUWE: %2\n").arg(RN).arg(UWE);

    int sLog;
    for(i=0;i<deg; i++)
    {
        Zi[i] = z[i];
        if(z[i]>0) opers << "+";
        else opers << "-";
        sLog = (int)(log10(fabs(z[i]))/fabs(log10(fabs(z[i]))));
        opers << QString("(%1*10^(%2))*x^%3").arg(fabs(z[i])*pow(10, -((int)log10(fabs(z[i])))+1)).arg((int)log10(fabs(z[i]))+sLog).arg(deg-i-1);
        qDebug() << QString("sLog: %1\n").arg(sLog);

        opers1 << QString("((%1)*x^%3").arg(z[i]).arg(deg-i-1);
    }

    resFunc->clear();
    resFunc->insert(0, opers.join(""));
    qDebug() << QString("resFunc: %1\n").arg(opers.join(""));
    qDebug() << opers1.join("+") << "\n";

    delete [] we;
    delete [] D;
    delete [] z;
    delete [] c;
}
