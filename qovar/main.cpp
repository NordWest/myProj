#include <QtCore/QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QString>
#include <QDebug>
#include <math.h>
#include "./../libs/mb.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QFile iFile("ocRefKsi.txt");
    iFile.open(QIODevice::ReadOnly);
    QTextStream iStm(&iFile);

    double *x, *y;
    double summ;

    QVector <double> X;
    QVector <double> Y;
    QString tStr;
    int i, szi, j, szj, p;
    double xt, yt, xmin, xmax;
    double x0, x1, mean, dx;
    int *np;
    x0 = 7000;
    x1 = 14000;

    summ = 0.0;
    xmin = 1e+10;
    xmax = -1e+10;
    while(!iFile.atEnd())
    {
        tStr = iStm.readLine();
        xt = tStr.section("\t", 0, 0).toDouble();
        yt = tStr.section("\t", 1, 1).toDouble();

        if((xt<x1)&&(xt>=x0))
        {

            if(xt<xmin) xmin = xt;
            if(xt>xmax) xmax = xt;

            //summ += yt;




            X << xt;
            Y << yt;
        }
    }
    iFile.close();
    szi = X.size();
    //mean = summ / szi;

    szj = 800;

    dx = (x1-x0)/szj;

    qDebug() << QString("Xsz: %1\tYsz: %2\n").arg(X.size()).arg(Y.size());
    qDebug() << QString("dx: %1\n").arg(dx);

    x = new double[szj];
    y = new double[szj];
    np = new int[szj];

    for(j=0; j<szj; j++)
    {
        x[j] = 0.0;
        y[j] = 0.0;
        np[j] = 0;
    }

    for(i=0; i<X.size(); i++)
    {
        p = floor((X.at(i)-x0)/dx);
        //qDebug() << QString("i= %1\tp= %2\n").arg(i).arg(p);
        if(p>=szj) continue;
        x[p] += X[i]-x0;
        y[p] += Y[i];
        np[p] ++;
    }

    QFile i0f("qini.txt");
    i0f.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream i0s(&i0f);

    int nmin = np[0];

    for(j=0; j<szj; j++)
    {
        if(np[j]<nmin) nmin = np[j];
        if(np[j]>0)
        {
            x[j] /= np[j];
            y[j] /= np[j];

            i0s << QString("%1\t%2\n").arg(x[j]).arg(y[j]);
        }
    }

    i0f.close();

    qDebug() << QString("nmin= %1\n").arg(nmin);

    ///////////polinom
    int pDeg = 4;
    double *Cx = new double[pDeg*szj];
    double *Zx = new double[pDeg];

    for(i=0; i<szj; i++)
    {
        Cx[i*pDeg] = pow(x[i], 3.0);
        Cx[i*pDeg+1] = x[i]*x[i];
        Cx[i*pDeg+2] = x[i];
        Cx[i*pDeg+3] = 1.0;
    }

    slsm(pDeg, szj, Zx, Cx, y);

    qDebug() << QString("Zx: %1\t%2\t%3\t%4\n").arg(Zx[0]).arg(Zx[1]).arg(Zx[2]).arg(Zx[3]);

    i0f.setFileName("qtrend.txt");
    i0f.open(QIODevice::WriteOnly | QIODevice::Truncate);
    i0s.setDevice(&i0f);

    for(i=0; i<szj; i++)
    {
        y[i] -= Zx[0]*pow(x[i], 3.0) + Zx[1]*x[i]*x[i] + Zx[2]*x[i] + Zx[3];

        i0s << QString("%1\t%2\n").arg(x[i]).arg(y[i]);
    }

    i0f.close();


    ////////////////

    double *qvX = new double[szj];
    double *qvY = new double[szj];

    for(i=0; i<szj; i++)
    {
        //p=0;
        summ = 0;
        for(j=0; j<szj-i-1; j++)
        {
            summ += y[j]*y[j+i];
          //  p++;
        }
        //qDebug() << QString("p= %1\n").arg(p);
        qvY[i] = summ/(1.0*szj);
        qvX[i] = i;
    }

    QFile oFile("qvar.txt");
    oFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream oStm(&oFile);

    for(i=0; i<szj; i++) oStm << QString("%1\t%2\n").arg(qvX[i]).arg(qvY[i]);

    oFile.close();


    return 0;//a.exec();
}
