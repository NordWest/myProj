#include <QtCore>

#include "./../astro/astro.h"
#include "comfunc.h"
#include "./../mb/mb.h"

#ifndef HRONOBASEFILE_H
#define HRONOBASEFILE_H

struct hronoDateRec
{
    int jdn;
    double sTime;
};

struct hronoBaseRec
{
    QDate date;
    QTime timeReal;
    QTime timeHrono;
    QString comment;

    int fromString(QString iStr);
    void toString(QString *iStr);

    double getMjdReal();
    double getMjdReal1();
    double getMjdHrono();
    double getJDNreal();
    double getJDNhrono();
    double getU();

    double getJDT(double lam);
};

class hronoBaseFile
{
public:
    QList <hronoBaseRec*> hronoList;
    QString fileName;
    hronoBaseFile();
    hronoBaseFile(QString baseFileName);
    void initJournal();
    int findU(double *uCorr, int jDN, double sTime, int dJDN = 7);
    int findU_mjd(double *uCorr, double mJD, double dDay = 10);

};

#endif // HRONOBASEFILE_H
