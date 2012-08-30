#ifndef MPCFILE_H
#define MPCFILE_H

#include <QtCore>

#include "astro.h"
#include "comfunc.h"

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
    void getCatFlag(QString &catFlag);
    void getObsCode(QString &obsCode);
    int  getObsNum();

    void setPos(double mjdt, double radeg, double deDeg);
    void setMagn(double magn);
    void setName(QString name);
    void setProvDest(QString pName);

    int fromStr(QString dStr);
    int fromRec(mpcRec& source);
    QString toStr();
    mpcRec();
    mpcRec(QString nStr);
};

class mpcFile
{
    QList <mpcRec*> recList;
public:
    mpcFile();
    QString fileName;
    int init(QString fName);
    void addObs(QString mpNumber, QString provDest, double mjd, double ra, double de, double magn, QString discAster = " ", QString note1 = " ", QString note2 = " ", QString magnBand = " ");
    mpcRec* at(int i);
    int size();
    int removeAt(int pos);
    void addRec(mpcRec& nRec);
    int saveAs(QString fName);
    int save();
};

#endif // MPCFILE_H
