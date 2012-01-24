#ifndef MPCFILE_H
#define MPCFILE_H

#include <QtCore>



class mpcRec
{
    QString dataStr;
public:
    void getMpNumber(QString &mpNumber);
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
    void getObsCode(QString &note2);

    void setStr(QString dStr);
};

class mpcFile
{
    QList <mpcRec*> recList;
public:
    mpcFile();
    void addObs(QString mpNumber, QString provDest, double mjd, double ra, double de, double magn, QString discAster = " ", QString note1 = " ", QString note2 = " ", QString magnBand = " ");
};

#endif // MPCFILE_H
