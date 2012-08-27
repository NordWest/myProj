#include <QApplication>
#include <QtGui>
//#include "OpenFileExDlg.h"
//#include "fitsio.h"
#include <iostream>
#include <QtCore>
#include <QDataStream>

//#include "conio.h"
#include "math.h"
#include "./../libs/astro.h"
#include "./../libs/mpcfile.h"
#include "./../libs/comfunc.h"
//#include "./../libs/redStat.h"
//#include "./../libs/multidim.h"
//#include "./../libs/vectGrid3D.h"
//#include "gammaf.h"
/*
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
*/

struct obsCodeCounter
{
    QString obsCode;
    int count;
    obsCodeCounter()
    {
        obsCode="";
        count = 0;
    };
};

struct objCounter
{
    QString objNum;
    int count;
    objCounter()
    {
        objNum="";
        count = 0;
    };
};

struct yearCounter
{
    int year;
    int count;
    yearCounter()
    {
        year=0;
        count = 0;
    };
};

struct catFlagCounter
{
    QString catFlag;
    int count;
    catFlagCounter()
    {
        catFlag="";
        count = 0;
    };
};

void addObsCode(QList <obsCodeCounter*> &obsList, QString obsCode)
{
    int i, sz;
    obsCodeCounter* obsRec;
    sz = obsList.count();
    for(i=0; i<sz; i++)
    {
        if(QString().compare(obsList.at(i)->obsCode, obsCode)==0)
        {
            obsList.at(i)->count++;
            return;
        }

    }
    obsRec = new obsCodeCounter;
    obsRec->obsCode = obsCode;
    obsRec->count=1;
    obsList << obsRec;

}

void addCatFlag(QList <catFlagCounter*> &cfList, QString catFlag)
{
    int i, sz;
    catFlagCounter* cfRec;
    sz = cfList.count();
    for(i=0; i<sz; i++)
    {
        if(QString().compare(cfList.at(i)->catFlag, catFlag)==0)
        {
            cfList.at(i)->count++;
            return;
        }

    }
    cfRec = new catFlagCounter;
    cfRec->catFlag = catFlag;
    cfRec->count=1;
    cfList << cfRec;

}

void addYear(QList <yearCounter*> &yrList, int year)
{
    int i, sz;
    yearCounter* yrRec;
    sz = yrList.count();
    for(i=0; i<sz; i++)
    {
        if(yrList.at(i)->year==year)
        {
            yrList.at(i)->count++;
            return;
        }

    }
    yrRec = new yearCounter;
    yrRec->year = year;
    yrRec->count=1;
    yrList << yrRec;

}

void addObjNum(QList <objCounter*> &objList, QString objNum)
{
    int i, sz;
    objCounter* objRec;
    sz = objList.count();
    for(i=0; i<sz; i++)
    {
        if(QString().compare(objList.at(i)->objNum, objNum)==0)
        {
            objList.at(i)->count++;
            return;
        }

    }
    objRec = new objCounter;
    objRec->objNum = objNum;
    objRec->count=1;
    objList << objRec;

}

void sortObsNum(QList <obsCodeCounter*> &obsList)
{
    int i, j, sz;
    sz = obsList.size();
    for(i=0;i<sz-1;i++)
    {
        for(j=i+1;j<sz;j++)
        {
            if(obsList.at(i)->count<obsList.at(j)->count) obsList.swap(i, j);
        }
    }
}

void sortYear(QList <yearCounter*> &yrList, int dir = 1)
{
    int i, j, sz;
    sz = yrList.size();
    for(i=0;i<sz-1;i++)
    {
        for(j=i+1;j<sz;j++)
        {
            if(dir*yrList.at(i)->year<yrList.at(j)->year*dir) yrList.swap(i, j);
        }
    }
}

void sortObjNum(QList <objCounter*> &objList)
{
    int i, j, sz;
    sz = objList.size();
    for(i=0;i<sz-1;i++)
    {
        for(j=i+1;j<sz;j++)
        {
            if(objList.at(i)->count<objList.at(j)->count) objList.swap(i, j);
        }
    }
}

int main(int argc, char *argv[])
{  
	///////////////////////////
	//QTranslator translator;
       //translator.load("astrored2_ru");
       //app.installTranslator(&translator);
       /////////////////////////////////
/*         COpenFileExDlg *mainWin = new COpenFileExDlg;
       mainWin->show(); */
       
	
	QApplication app(argc, argv);
    setlocale(LC_NUMERIC, "C");
	
	
    int i, sz;
    QTextStream out_stream;

	QFile fout("./fout.dat");
	fout.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
	
	out_stream.setDevice(&fout);
/*
	QSettings *settings = new QSettings("./mpcstat.ini",QSettings::IniFormat);
	QString workingFolder = settings->value("general/workingFolder").toString();
	QString outputFolder = settings->value("general/outputFolder").toString();
	int taskNum = settings->value("general/taskNum").toInt();
	
//	double velMin = settings->value("task14/velMin").toDouble();
	double velMax = settings->value("general/velMax").toDouble();
	
	int isSysCorr = settings->value("general/isSysCorr").toInt();
	QString sysCorrFile = settings->value("general/sysCorrFile").toString();
        QStringList excNames = settings->value("general/excNames").toString().split("|");
        int expMin = settings->value("general/expMin").toInt();
        int sigmaTest = settings->value("general/sigmaTest", 0).toInt();
*/

        mpcRec mpR;
        QString mpNum, obsCode, catFlag;
        double mjd;
        DATEOBS date_obs;
        QString mpcFile(argv[1]);

        QList <obsCodeCounter*> obsList;
        QList <yearCounter*> yrList;
        QList <catFlagCounter*> cfList;
        QList <objCounter*> objList;


        QFile inFile(mpcFile);
        if(!inFile.open(QIODevice::ReadOnly))
        {
            qDebug() << QString("File %1 not open. stop\n").arg(mpcFile);
            return 1;
        }
        QTextStream inStm(&inFile);

        while(!inFile.atEnd())
        {
            mpR.fromStr(inStm.readLine());

            mpR.getMpNumber(mpNum);
            mpR.getObsCode(obsCode);
            mjd = mpR.mjd();
            getDATEOBSfromMJD(&date_obs, mjd);
            mpR.getCatFlag(catFlag);


            addObsCode(obsList, obsCode);
            addYear(yrList, date_obs.year);
            addCatFlag(cfList, catFlag);
            addObjNum(objList, mpNum);
        }

        qDebug() << QString("obs: %1\nyears: %2\ncatFlags: %3\n").arg(obsList.count()).arg(yrList.count()).arg(cfList.count());



        //save obsList

        sortObsNum(obsList);

            QFile resFile;
            resFile.setFileName("./obsCounter.txt");
            QTextStream resStm;
            if(resFile.open(QFile::WriteOnly | QFile::Truncate))
            {
                resStm.setDevice(&resFile);
                sz = obsList.count();
                for(i=0; i<sz; i++)
                {

                    resStm << QString("%1|%2\n").arg(obsList.at(i)->obsCode).arg(obsList.at(i)->count);
                }

                resFile.close();
            }

    //save yrList
        sortYear(yrList, -1);

        resFile.setFileName("./yearCounter.txt");

        if(resFile.open(QFile::WriteOnly | QFile::Truncate))
        {
            resStm.setDevice(&resFile);
            sz = yrList.count();
            for(i=0; i<sz; i++)
            {

                resStm << QString("%1|%2\n").arg(yrList.at(i)->year).arg(yrList.at(i)->count);
            }

            resFile.close();
        }


    //save cfList
        //sortYear(yrList);

        resFile.setFileName("./cfCounter.txt");

        if(resFile.open(QFile::WriteOnly | QFile::Truncate))
        {
            resStm.setDevice(&resFile);
            sz = cfList.count();
            for(i=0; i<sz; i++)
            {

                resStm << QString("%1|%2\n").arg(cfList.at(i)->catFlag).arg(cfList.at(i)->count);
            }

            resFile.close();
        }

    //save objList
        sortObjNum(objList);

        resFile.setFileName("./objCounter.txt");

        if(resFile.open(QFile::WriteOnly | QFile::Truncate))
        {
            resStm.setDevice(&resFile);
            sz = objList.count();
            for(i=0; i<sz; i++)
            {

                resStm << QString("%1|%2\n").arg(objList.at(i)->objNum).arg(objList.at(i)->count);
            }

            resFile.close();
        }


	
//	printf("end\n");
//	getch();
	return 0; 
}
	
