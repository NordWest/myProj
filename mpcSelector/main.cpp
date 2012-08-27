#include <QtCore/QCoreApplication>
#include "./../libs/mpcfile.h"
#include "./../libs/comfunc.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    setlocale(LC_NUMERIC, "C");

    mpcRec mpR;
    int obsNum;
    int objNum;
    int isObs, isObj, isTime, k, r, i;
    QString mpNum, obsCode;
    double mjd0, mjd1, mjd;

    QTime workTime;
    workTime.start();

    QString cfgFileName = "mpcSelector.ini";
    QSettings *sett = new QSettings(cfgFileName, QSettings::IniFormat);

    QStringList obsCodeList = sett->value("general/obsCodeList", "").toString().split("|");
    QStringList objNumList = sett->value("general/objNumList", "").toString().split("|");
    QString timeS0 = sett->value("general/time0", "").toString();
    QString timeS1 = sett->value("general/time1", "").toString();

    if(timeS0.size()!=0) getMJDfromStrFTN(&mjd0, timeS0, 0);
    if(timeS1.size()!=0) getMJDfromStrFTN(&mjd1, timeS1, 0);

    QString fileName = QString(argv[1]);
    QString fileNameRes = QString(argv[2]);

    QFile inFile(fileName);
    if(!inFile.open(QIODevice::ReadOnly))
    {
        qDebug() << QString("File %1 not open. stop\n").arg(fileName);
        return 1;
    }
    QTextStream inStm(&inFile);

    QFile resFile(fileNameRes);
    if(!resFile.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        qDebug() << QString("File %1 not open. stop\n").arg(fileNameRes);
        return 1;
    }
    QTextStream resStm(&resFile);



    obsNum = obsCodeList.size();
    objNum = objNumList.size();
    qDebug() << QString("obsNum= %1\tobjNum= %2\n").arg(obsNum).arg(objNum);

    k=0; r=0;
    while(!inFile.atEnd())
    {
        mpR.fromStr(inStm.readLine());

        isObj = 1;
        isObs = 1;
        isTime = 1;

        mpR.getMpNumber(mpNum);
        for(i=0; i<objNum && objNumList.at(i).size()>0;i++)
        {
            isObj = (QString().compare(mpNum, objNumList.at(i))==0);
            if(isObj) break;
        }

        mpR.getObsCode(obsCode);
        for(i=0; i<obsNum && obsCodeList.at(i).size()>0;i++)
        {
            isObs = (QString().compare(obsCode, obsCodeList.at(i))==0);
            if(isObs) break;
        }

        mjd = mpR.mjd();

        isTime = (timeS0.size()==0||mjd>=mjd0)&&(timeS1.size()==0||mjd<=mjd1);

        if(isObj&&isObs&&isTime)
        {
            resStm << mpR.toStr() << "\n";
            r++;
        }
        k++;
    }

    qDebug() << QString("\nFrom %1 records %2 selected\n").arg(k).arg(r);

    inFile.close();
    resFile.close();


    qDebug() << QString("Time: %1 sec").arg(workTime.elapsed()/1000.0);

    //return a.exec();
}
