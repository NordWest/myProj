#include <QtCore/QCoreApplication>
#include "./../../libs/mpcfile.h"
#include "./../../libs/comfunc.h"



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    setlocale(LC_NUMERIC, "C");

    if(argc<3)
    {
        qDebug() << QString("error ini file\nmpcSelector ini.txt res.txt\n");
        return 1;
    }

    mpcRec mpR;
    int obsNum, obsNumN, objNum, cfNum, otNum;
    int isObs, isObsN, isObj, isTime, isCF, isMag, isOT;
    int k, r, i, j, sz;
    QString mpNum, obsCode, catFlag, obsType;
    double mjd0, mjd1, mjd, tMag;
    QStringList objNumList;
    QStringList obsCodeList, obsCodeListN;

    QTime workTime;
    workTime.start();

    QString cfgFileName = "mpcSelector.ini";
    QSettings *sett = new QSettings(cfgFileName, QSettings::IniFormat);

    QString obsFileName = sett->value("general/obsFileName", "").toString();
    obsCodeList = sett->value("general/obsCodeList", "").toString().split("|");
    obsCodeListN = sett->value("general/obsCodeListN", "").toString().split("|");
    QString objFileName = sett->value("general/objFileName", "").toString();
    obsCodeList << sett->value("general/obsCodeList", "").toString().split("|", QString::SkipEmptyParts);
    QStringList catFlagList = sett->value("general/catFlagList", "").toString().split("|");
    QStringList obsTypeList = sett->value("general/obsTypeList", "").toString().split("|");
    QString timeS0 = sett->value("general/time0", "").toString();
    QString timeS1 = sett->value("general/time1", "").toString();

    double mag0 = sett->value("general/mag0", -30).toDouble();
    double mag1 = sett->value("general/mag1", 30).toDouble();

    if(timeS0.size()!=0) getMJDfromStrFTN(&mjd0, timeS0, 0);
    if(timeS1.size()!=0) getMJDfromStrFTN(&mjd1, timeS1, 0);


    QVector <int> mpcNums;

    bool isOk;
    QString upStr;

    QDir wDir;

    wDir.setPath("./");
    wDir.mkdir("./mpcs");
    wDir.setPath("./mpcs");


    //unpackString(&upStr, dataStr.mid(0, 5));

    int objn, pMin, nMin;


    QFile parFile;//(objFileName);
    QTextStream parStm;
    if(QFile().exists(objFileName))
    {
        parFile.setFileName(objFileName);
        if(parFile.open(QFile::ReadOnly))
        {
            parStm.setDevice(&parFile);
            while(!parStm.atEnd())
            {
                objNumList << QString("%1").arg(parStm.readLine().section("|", 0, 0), 5, QLatin1Char( '0' ) );
            }
            parFile.close();
        }
/*
        sz = objNumList.size();
        mpcNums.size();
        for(i=0; i<sz; i++)
        {
            unpackString(&upStr, objNumList.at(i));
            objn = upStr.toInt(&isOk);
            if(isOk)
            {
                mpcNums.append(objn);
            }
        }
        sz = mpcNums.size();



        for(i=0; i<sz-1; i++)
        {
            pMin = -1;
            nMin = mpcNums.at(i);
            for(j=i+1; j<sz; j++)
            {
                if(mpcNums.at(j)<nMin)
                {
                    nMin = mpcNums.at(j);
                    pMin = j;
                }
            }
            if(pMin!=-1)
            {
                mpcNums[pMin] = mpcNums[i];
                mpcNums[i] = nMin;
                //mpcNums.swap(i, j);
            }
        }
        */
    }

    if(QFile().exists(obsFileName))
    {
        parFile.setFileName(obsFileName);
        if(parFile.open(QFile::ReadOnly))
        {
            parStm.setDevice(&parFile);
            while(!parStm.atEnd())
            {
                obsCodeList << QString("%1").arg(parStm.readLine().section("|", 0, 0));
            }

            parFile.close();
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

    QFile mFile;
    QTextStream mStm;


    obsNum = obsCodeList.size();
    obsNumN = obsCodeList.size();
    objNum = objNumList.size();
    cfNum = catFlagList.size();
    otNum = obsTypeList.size();
    qDebug() << QString("obsNum= %1\tobjNum= %2\tcfNum= %3\totNum=%4\n").arg(obsNum).arg(objNum).arg(cfNum).arg(otNum);

    k=0; r=0;
    while(!inFile.atEnd())
    {
        mpR.fromStr(inStm.readLine());

        isObj = !objNum;
        isObs = !obsNum;
        isObsN = obsNumN;
        isTime = 1;
        isCF = 1;
        isMag = 1;
        isOT = 1;

//time
        mjd = mpR.mjd();
        isTime = (timeS0.size()==0||mjd>=mjd0)&&(timeS1.size()==0||mjd<=mjd1);
        if(!isTime) continue;

        mpR.getMpNumber(mpNum);
        if(!isObj) isObj = objNumList.indexOf(mpNum)!=-1;
        /*objn = mpR.mpNumber();
        if(!isObj)
        {
            isObj = (mpcNums.indexOf(objn)!=-1);
        }
/*
        for(i=0; i<objNum;i++)
        {
            if(objn==mpcNums.at(i))
            {
                isObj=1;
                break;
            }
            if(objn>mpcNums.at(i)) break;

            //isObj = (QString().compare(mpNum, objNumList.at(i))==0);
            //if(isObj) break;
        }
*/


//obsCode
        mpR.getObsCode(obsCode);
/*
        if(!isObs)
        {
            //isObs =
            if(obsCodeList.indexOf(obsCode)!=-1)
            {
                isObs=1;
            }
        }
        */
        for(i=0; i<obsNum && obsCodeList.at(i).size()>0;i++)
        {
            isObs = (QString().compare(obsCode, obsCodeList.at(i))==0);
            if(isObs) break;
        }
/*
        for(i=0; i<obsNumN && obsCodeListN.at(i).size()>0;i++)
        {
            isObsN = (QString().compare(obsCode, obsCodeListN.at(i))==0);
            if(isObsN) break;
        }
        if(isObsN) continue;
*/
//catFlag
        mpR.getCatFlag(catFlag);
        for(i=0; i<cfNum && catFlagList.at(i).size()>0;i++)
        {
            isCF = (QString().compare(catFlag, catFlagList.at(i))==0);
            if(isCF) break;
        }
//obsType
        mpR.getNote2(obsType);
        for(i=0; i<otNum && obsTypeList.at(i).size()>0;i++)
        {
            isCF = (QString().compare(obsType, obsTypeList.at(i))==0);
            if(isOT) break;
        }

        tMag = mpR.magn();
        if(tMag<mag0||tMag>mag1) isMag = 0;

        if(isObj&&isObs&&isTime&&isCF&&isMag)
        {
            resStm << mpR.toStr() << "\n";
            r++;
            wDir.mkdir(obsCode);
            mFile.setFileName(QString("./%1/%2.txt").arg(obsCode).arg(mpNum));
            mFile.open(QFile::WriteOnly | QFile::Append);
            mStm.setDevice(&mFile);
            mStm << mpR.toStr() << "\n";
        }
        k++;
    }

    qDebug() << QString("\nFrom %1 records %2 selected\n").arg(k).arg(r);

    inFile.close();
    resFile.close();


    qDebug() << QString("Time: %1 sec").arg(workTime.elapsed()/1000.0);

    return 0;//a.exec();
}
