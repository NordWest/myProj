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

struct obsCodeCouter
{
    QString obsCode;
    int count;
    obsCodeCouter()
    {
        obsCode="";
        count = 0;
    };
};

struct objCouter
{
    QString objNum;
    int count;
    objCouter()
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

void addObsCode(QList <obsCodeCouter*> &obsList, QString obsCode)
{
    int i, sz;
    obsCodeCouter* obsRec;
    sz = obsList.count();
    for(i=0; i<sz; i++)
    {
        if(QString().compare(obsList.at(i)->obsCode, obsCode)==0)
        {
            obsList.at(i)->count++;
            return;
        }

    }
    obsRec = new obsCodeCouter;
    obsRec->obsCode = obsCode;
    obsRec->count=1;
    obsList << obsRec;

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

void sortObsNum(QList <obsCodeCouter*> &obsList)
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
        QString mpNum, obsCode;
        double mjd;
        DATEOBS date_obs;
        QString mpcFile(argv[1]);

        QList <obsCodeCouter*> obsList;
        QList <yearCounter*> yrList;

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

            addObsCode(obsList, obsCode);
            addYear(yrList, date_obs.year);
        }

        qDebug() << QString("obs: %1\nyears: %2\n").arg(obsList.count()).arg(yrList.count());

        sortObsNum(obsList);

    //save obsList

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



        resFile.setFileName("./yearCounter.txt");

        if(resFile.open(QFile::WriteOnly | QFile::Truncate))
        {
            resStm.setDevice(&resFile);
            sz = yrList.count();
            for(i=0; i<sz; i++)
            {

                resStm << QString("%1|%2\n").arg(yrList.at(i)->year).arg(obsList.at(i)->count);
            }

            resFile.close();
        }


	
//	printf("end\n");
//	getch();
	return 0; 
}
	
