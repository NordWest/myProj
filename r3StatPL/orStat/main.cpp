#include <QtCore/QCoreApplication>
#include "./../../libs/redStat.h"

int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);
    objResidualFile iniResFile;
    objResidualFile tempRF;

    iniResFile.init(QString(argv[1]));


    sortORList(&iniResFile.ocList);
    double t0, t1, dt, tt0, tt1, tt;
    int i, sz, k;

    sz = iniResFile.ocList.size();
    t0 = iniResFile.ocList.first()->mJD;
    t1 = iniResFile.ocList.last()->mJD;

    dt = atof(argv[2])*365.2425;

    QFile resFile("./orRes.txt");
    resFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream resStm(&resFile);

    QList <colRec*> cListT;

    k=0;
    tt0 = t0+dt*k;
    tt1 = t0+dt*(k+1);
    for(i=0; i<sz; i++)
    {
        if(iniResFile.ocList.at(i)->mJD<tt1&&iniResFile.ocList.at(i)->mJD>=tt0)
        {
            tempRF.ocList << iniResFile.ocList.at(i);
        }
        else
        {
            tempRF.do3sigma(0.0, 3.0);
            if(!tempRF.countCols(&cListT, "0,6,7")&&cListT.at(0)->num>20)
            {
                resStm << QString("%1|%2|%3|%4|%5|%6|%7|%8\n").arg(cListT.at(0)->mean).arg(cListT.at(0)->num).arg(cListT.at(1)->mean).arg(cListT.at(2)->mean).arg(cListT.at(1)->rmsMean).arg(cListT.at(2)->rmsMean).arg(cListT.at(1)->rmsOne).arg(cListT.at(2)->rmsOne);
            }
            tempRF.clear();
            cListT.clear();
            k++;
            tt0 = t0+dt*k;
            tt1 = t0+dt*(k+1);
        }

    }
    resFile.close();

    return 0;//a.exec();
}
