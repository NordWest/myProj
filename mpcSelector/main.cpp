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
    int isObs, isObj, k, r, i;
    QString mpNum, obsCode;

    QString cfgFileName = "mpcSelector.ini";
    QSettings *sett = new QSettings(cfgFileName, QSettings::IniFormat);

    QStringList obsCodeList = sett->value("general/obsCodeList", "").toString().split("|");
    QStringList objNumList = sett->value("general/objNumList", "").toString().split("|");
    QString timeS0 = sett->value("general/time0", "-1").toString();
    QString timeS1 = sett->value("general/time1", "-1").toString();

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

        mpR.getMpNumber(mpNum);
        for(i=0; i<objNum && objNumList.at(i).size()>0;i++)isObj = (QString().compare(mpNum, objNumList.at(i))==0);

        mpR.getObsCode(obsCode);
        for(i=0; i<obsNum && obsCodeList.at(i).size()>0;i++)isObs = (QString().compare(obsCode, obsCodeList.at(i))==0);

        if(isObj&&isObs)
        {
            resStm << mpR.toStr() << "\n";
            r++;
        }
        k++;
    }

    qDebug() << QString("\nFrom %1 records %2 selected\n").arg(k).arg(r);

    inFile.close();
    resFile.close();

    //return a.exec();
}
