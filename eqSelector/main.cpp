#include <QtCore/QCoreApplication>
#include "./../libs/comfunc.h"
#include "./../libs/redStat.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    setlocale(LC_NUMERIC, "C");

    ocRecO ocR;
    int obsNum, objNum, cfNum;
    int isObs, isObj, isTime, isCF;
    int k, r, i;
    QString objName, obsCode, catName, tStr;
    double mjd0, mjd1, mjd;
    QStringList objNumList, obsCodeList;

    QTime workTime;
    workTime.start();

    QString cfgFileName = "eqSelector.ini";
    QSettings *sett = new QSettings(cfgFileName, QSettings::IniFormat);

    QString obsFileName = sett->value("general/obsFileName", "").toString();
    QString objFileName = sett->value("general/objFileName", "").toString();
    QStringList catNameList = sett->value("general/catNameList", "").toString().split("|");
    QString timeS0 = sett->value("general/time0", "").toString();
    QString timeS1 = sett->value("general/time1", "").toString();

    if(timeS0.size()!=0) getMJDfromStrFTN(&mjd0, timeS0, 0);
    if(timeS1.size()!=0) getMJDfromStrFTN(&mjd1, timeS1, 0);


    QFile parFile(objFileName);
    QTextStream parStm;
    if(parFile.open(QFile::ReadOnly))
    {
        parStm.setDevice(&parFile);
        while(!parStm.atEnd())
        {
            objNumList << QString("%1").arg(parStm.readLine().section("|", 0, 0).simplified(), 5, QLatin1Char( '0' ) );
        }
    }
    parFile.close();

    parFile.setFileName(obsFileName);
    if(parFile.open(QFile::ReadOnly))
    {
        parStm.setDevice(&parFile);
        while(!parStm.atEnd())
        {
            obsCodeList << QString("%1").arg(parStm.readLine().section("|", 0, 0).simplified(), 5, QLatin1Char( '0' ) );
        }
    }

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
    cfNum = catNameList.size();
    qDebug() << QString("obsNum= %1\tobjNum= %2\tcfNum= %3\n").arg(obsNum).arg(objNum).arg(cfNum);

    k=0; r=0;
    while(!inFile.atEnd())
    {
        ocR.s2rec(inStm.readLine());

        isObj = 1;
        isObs = 1;
        isTime = 1;
        isCF = 1;

        //mpR.getMpNumber(mpNum);
        objName = ocR.name;
        for(i=0; i<objNum && objNumList.at(i).size()>0;i++)
        {
            isObj = (QString().compare(objName, objNumList.at(i))==0);
            if(isObj) break;
        }

        //mpR.getObsCode(obsCode);
        obsCode = ocR.obsCode;
        for(i=0; i<obsNum && obsCodeList.at(i).size()>0;i++)
        {
            isObs = (QString().compare(obsCode, obsCodeList.at(i))==0);
            if(isObs) break;
        }

        mjd = ocR.MJday;
        isTime = (timeS0.size()==0||mjd>=mjd0)&&(timeS1.size()==0||mjd<=mjd1);

        //mpR.getCatFlag(catName);
        catName = ocR.catName;
        for(i=0; i<cfNum && catNameList.at(i).size()>0;i++)
        {
            isCF = (QString().compare(catName, catNameList.at(i))==0);
            if(isCF) break;
        }

        if(isObj&&isObs&&isTime&&isCF)
        {
            ocR.rec2s(tStr);
            resStm << tStr << "\n";
            r++;
        }
        k++;
    }

    qDebug() << QString("\nFrom %1 records %2 selected\n").arg(k).arg(r);

    inFile.close();
    resFile.close();


    qDebug() << QString("Time: %1 sec").arg(workTime.elapsed()/1000.0);
    
    return 0;//a.exec();
}
