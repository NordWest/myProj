#ifndef MPCFILE_H
#define MPCFILE_H

#include <QtCore>

#include "astro.h"

class mpcRec
{
    QString dataStr;
public:
    void getMpNumber(QString &mpNumber);
    //QString& getMpNumber();
    int mpNumber();
    void getProvDest(QString &provDest);
    void getDiscAst(QString &discAst);
    int discAstKey();
    void getNote1(QString &note1);
    void getNote2(QString &note2);
    double mjd();
    double ra();
    double dec();
    double magn();
    void getBand(QString &bandStr);
    void getObsCode(QString &note2);

    int fromStr(QString dStr);
    mpcRec();
    mpcRec(QString nStr);
};

class mpcFile
{
    QList <mpcRec*> recList;
public:
    mpcFile();
    int init(QString fName);
    void addObs(QString mpNumber, QString provDest, double mjd, double ra, double de, double magn, QString discAster = " ", QString note1 = " ", QString note2 = " ", QString magnBand = " ");
    mpcRec* at(int i);
    int size();
};

#endif // MPCFILE_H
