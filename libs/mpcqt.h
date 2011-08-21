#ifndef MPCQT_H
#define MPCQT_H

#define MPC_SIZE 81

#include <QString>
#include <QFile>
#include <QTextStream>

struct mpcObsData
{
    double ra, dec;
    double magn;

};

class mpcMPrec
{
public:
    int num;            //Minor planet number
    QString provName;   //Provisional or temporary designation
    int discAster;      //Discovery asterisk

    void fromString(QString headStr);
    void toString(QString *headStr);
};

class mpcqtrec
{
    QString string;

public:
    void fromString(QString inStr);
    void toString(QString *outStr);

    void getHead(QString *headStr);
    void setHead(QString headStr);

    void getMPhead(mpcMPrec *mpRec);
/*
    void getObsData(mpcObsData *obsData);

    int getObsNum();
    void getObsName(QString obsName);

    void getNote1(QChar *note1);
*/

};

class mpcqt
{
    QString fileName;
public:
    mpcqtrec *record;

    mpcqt();

    int initMPCCAT(QString catFileName);

    int readRecord(int pos);
};

#endif // MPCQT_H
