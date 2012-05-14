#include <QtCore>
/*
#include "./../libs/observ.h"
#include "./../libs/orbit.h"
#include "./../libs/astro.h"
#include "./../libs/comfunc.h"
#include "./../libs/redStat.h"
#include "./../libs/mpcfile.h"
#include "./../libs/skyarea.h"
*/

//#include "./../libs/listfile.h"

class tRecord
{
public:
//    tRecord(){};
//    ~tRecord(){};

    int fromString(QString tStr){};
    int toString(QString &tStr){};
};

template <class tRecord>
class listFile
{
    QList <tRecord*> recList;
    QString fileName;
 public:
    int init(QString fname)
    {
        fileName = fname;
        QFile iniFile(fileName);
        iniFile.open(QIODevice::ReadOnly);
        QTextStream iniStm(&iniFile);

        QString tStr;

        recList.clear();

        while(!iniStm.atEnd())
        {
            tStr = iniStm.readLine();
            s2rec(tStr);//) continue;
            //recList << tRec;
            //tRec = new tRecord;
        }

        iniFile.close();

        return 0;
    };

    int s2rec(QString tStr){
        tRecord *tRec;
        tRec = new tRecord;
        if(tRec->toString(tStr)) return 1;
        recList << tRec;
        return 0;
    };

};

class tlRecord : public tRecord	//Task List record
{
public:
//	int noftask;	//number of task

        double exp;			//experience of task
        int Ntot;			//Total number of observation for one object
        double texc;		//exclusion of a object after successful observations
        double dRA;			//maximum distance from celestial meridian
        int NinN;			//number obsrvations during one night
        int flag_active;	//flag for activ task
        QString name, desc, dirPath, catName;

//	char *tail;

        tlRecord();
        int fromString(QString tStr);
        void toString(QString &tStr);
        tlRecord& operator=(const tlRecord &rhs);
        //tlRecord& operator=(const TLRecord &rhs);

        void getIniName(QString &iniName);
};

bool operator==( const tlRecord& lhs, const tlRecord& rhs );

class taskList : listFile <tlRecord>
{
public:
    taskList(){};
    //int init(QString fname);
};

int taskList::init(QString fname)
{
    return 0;
}


//using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    setlocale(LC_NUMERIC, "C");



    QString eassDir = QString("/home/nuts/Work/lab/EA_NA");
    QString taskListFile = QString("%1/task.lst").arg(eassDir);
    QString catListFile = QString("%1/cat.lst").arg(eassDir);

    QString resDir = QString("./");
    QString tListFile = QString("%1/task.lst").arg(resDir);


//    TaskList taskL;
//    taskL.init(taskListFile.toAscii().data());


    taskList tList;
    tList.init(tListFile);
 /*   tlRecord *ntRec;

    int i, sz;

    sz = taskL.nstr();

    for(i=0;i<sz;i++)
    {
        taskL.GetRec(i);
        ntRec = new tlRecord;
        &ntRec = &taskL.record;
        tList.addRec(ntRec);
    }

    tList.save();
*/





    return 0;
}

/*
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    setlocale(LC_NUMERIC, "C");

    int nbd;
    int szi, i, j;
    mpccat mpCat;
    orbit orb_elem;
    double Sdist, Edist, x, y, z, magn, ra, de, utc0, utct, step;
    double xM, yM, zM;
    QString tRa, tDe;
    double dRa, dDe;
    QString sJD, tStr, objName;
    QStringList objList;
    QList <double*> raL;
    QList <double*> deL;
    QList <double*> raML;
    QList <double*> deML;
    QList <double*> timeL;
    double *times, *ravect, *devect;


    QSettings sett("./saTest.ini", QSettings::IniFormat);

    QString mpcCatName = sett.value("general/mpcCatName", "mpcorb.dat").toString();
    QString deleCatName = sett.value("general/deleCatName", "dele.cat").toString();
    nbd = sett.value("general/nbd", 1).toInt();
    step = sett.value("general/step", 1).toInt();

    observ obsPos;
    mpcRec mpcr;

    procData miriadeProcData;
    miriadeProcData.name = "./miriadeEph";
    miriadeProcData.folder = "./";
    miriadeProcData.waitTime = -1;

    QProcess outerProcess;
    QStringList outerArguments;
    //QTextStream objStream;
    QString objDataStr, strT;
    mpephRec mpcObj;

    obsPos.init("./Obs.txt", deleCatName.toAscii().data());

    obsPos.set_obs_parpam(GEOCENTR_NUM, CENTER_SUN, SK_EKVATOR, "500");

    QFile logFile("log.txt");
    logFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream logStm(&logFile);
    QFile resFile("res.txt");
    resFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream resStm(&resFile);

    QFile iniFile("ini.lst");
    iniFile.open(QIODevice::ReadOnly);
    QTextStream iniStm(&iniFile);

    QFile recFile("rec.txt");
    recFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream recStm(&recFile);





    //iList.init("ini.lst");
    //iCat.init("ini.cat");
    mpCat.init(mpcCatName.toAscii().data());


    //szi = iList.nstr;
    //step = 30;
    //nbd = 1;


/*
    utc = 2454500.5;

    obsPos.det_observ(utc);


    ////

        outerArguments.clear();
        outerArguments << QString("-name=p:Earth");
        sJD = QString("%1").arg(utc, 11, 'f',7);
        outerArguments << QString("-ep=%1").arg(sJD);
        outerArguments << QString("-tcoor=2");
        outerArguments << QString("-observer=@sun");
        outerArguments << QString("-rplane=1");

        outerProcess.setWorkingDirectory(miriadeProcData.folder);
        outerProcess.setProcessChannelMode(QProcess::MergedChannels);
        outerProcess.setReadChannel(QProcess::StandardOutput);

        logStm << "outerArguments: " << miriadeProcData.name << " " << outerArguments.join("|") << "\n";

        outerProcess.start(miriadeProcData.name, outerArguments);

        if(!outerProcess.waitForFinished(miriadeProcData.waitTime))
        {
            logStm << "\nmpephProc finish error\n";
        }
        QTextStream objStream(outerProcess.readAllStandardOutput());

        while (!objStream.atEnd())
        //for(i=0; i<orList.size(); i++)
        {
            objDataStr = objStream.readLine();
            //objDataStr = orList.at(i);
            if(objDataStr.at(0)=='#') continue;
            logStm << QString("objDataStr: %1").arg(objDataStr);

            xM = objDataStr.section(" ", 1, 1, QString::SectionSkipEmpty).toDouble();
            yM = objDataStr.section(" ", 2, 2, QString::SectionSkipEmpty).toDouble();
            zM = objDataStr.section(" ", 3, 3, QString::SectionSkipEmpty).toDouble();

            if(mpcObj.fromMiriStr(objDataStr))
            {
                logStm << "\nfromString error\n";
                continue;
            }
            else break;
        }

        logStm << QString("observ: %1 %2 %3\n").arg(obsPos.ox).arg(obsPos.oy).arg(obsPos.oz);
        logStm << QString("Miriade: %1 %2 %3\n").arg(xM).arg(yM).arg(zM);

    ////

/

    while(!iniStm.atEnd())
    {
        //if(iList.GetRec(i))logStm << QString("ilstt err\n");

            tStr = iniStm.readLine();
            objName = tStr.mid(0, 16);
            qDebug() << QString("%1\n").arg(tStr);

        if(mpCat.GetRecName(objName.simplified().toAscii().data())) logStm << QString("icat err\n");



        logStm << QString("num %1: %2\n").arg(i).arg(objName);
        qDebug() << QString("num %1: %2\n").arg(i).arg(objName);

        utc0 = mpCat.record->getEpoch();


        objList << objName;
        orb_elem.get(&mpCat);
        times = new double[nbd];
        ravect = new double[nbd];
        devect = new double[nbd];

        for(i=0; i<nbd; i++)
        {
            utct = utc0 + step*i;
            logStm << QString("utct: %1\n").arg(utct,13,'f',5);
            obsPos.det_observ(utct);
            orb_elem.detRecEkv(&x, &y, &z, utct);
            getStrFTNfromMJD(&strT, jd2mjd(utct), 0.0);
            recStm << QString("%1|%2|%3|%4|%5|%6\n").arg(utct,13,'f',5).arg(strT).arg(x,13,'f',5).arg(y,13,'f',5).arg(z,13,'f',5).arg(objName);
            detRDnumGC(&ra, &de, x, y, z, obsPos.ox, obsPos.oy, obsPos.oz, obsPos.obs->dcx, obsPos.obs->dcy, obsPos.obs->dcz);



            ravect[i] = ra;
            devect[i] = de;
            times[i] = utct;
        }

        timeL << times;
        raL << ravect;
        deL << devect;
    }

        //res_list->record->set_name(iCat->record->name);
        //sprintf(this->res_list->record->num, "%8d", this->orb_catalog->record->number);

        szi = objList.size();
        for(i=0; i<szi; i++)
        {
            ravect = new double[nbd];
            devect = new double[nbd];


            outerArguments.clear();
            outerArguments << QString("-name=\"%1\"").arg(objList[i].simplified());
            sJD = QString("%1").arg(timeL[i][0], 11, 'f',7);
            outerArguments << QString("-ep=%1").arg(sJD);
            //outerArguments << QString("-teph=2");
            outerArguments << QString("-nbd=%1").arg(nbd);
            outerArguments << QString("-step=%1d").arg(step);
            outerArguments << QString("-observer=500");

            outerProcess.setWorkingDirectory(miriadeProcData.folder);
            outerProcess.setProcessChannelMode(QProcess::MergedChannels);
            outerProcess.setReadChannel(QProcess::StandardOutput);

            logStm << "outerArguments: " << miriadeProcData.name << " " << outerArguments.join("|") << "\n";

            outerProcess.start(miriadeProcData.name, outerArguments);

            if(!outerProcess.waitForFinished(miriadeProcData.waitTime))
            {
                logStm << "\nmpephProc finish error\n";
            }
            QTextStream objStream1(outerProcess.readAllStandardOutput());

            j=0;
            while (!objStream1.atEnd()&&j<nbd)
            //for(i=0; i<orList.size(); i++)
            {
                objDataStr = objStream1.readLine();
                //objDataStr = orList.at(i);
                if(objDataStr.at(0)=='#') continue;
                //logStm << QString("objDataStr: %1").arg(objDataStr);


                if(mpcObj.fromMiriStr(objDataStr))
                {
                    logStm << "\nfromString error\n";
                    continue;
                }

                ravect[j] = mpcObj.ra;
                devect[j] = mpcObj.de;

                j++;
            }

            raML << ravect;
            deML << devect;

        }


    ////
/*
        outerArguments.clear();
        outerArguments << QString("-name=\"%1\"").arg(QString(iList.record->name).simplified());
        sJD = QString("%1").arg(utc, 11, 'f',7);
        outerArguments << QString("-ep=%1").arg(sJD);
        outerArguments << QString("-tcoor=2");
        outerArguments << QString("-observer=500");
        outerArguments << QString("-rplane=2");

        outerProcess.setWorkingDirectory(miriadeProcData.folder);
        outerProcess.setProcessChannelMode(QProcess::MergedChannels);
        outerProcess.setReadChannel(QProcess::StandardOutput);

        logStm << "outerArguments: " << miriadeProcData.name << " " << outerArguments.join("|") << "\n";

        outerProcess.start(miriadeProcData.name, outerArguments);

        if(!outerProcess.waitForFinished(miriadeProcData.waitTime))
        {
            logStm << "\nmpephProc finish error\n";
        }
        QTextStream objStream0(outerProcess.readAllStandardOutput());

        while (!objStream0.atEnd())
        //for(i=0; i<orList.size(); i++)
        {
            objDataStr = objStream0.readLine();
            //objDataStr = orList.at(i);
            if(objDataStr.at(0)=='#') continue;
            logStm << QString("objDataStr: %1").arg(objDataStr);

            xM = objDataStr.section(" ", 1, 1, QString::SectionSkipEmpty).toDouble();
            yM = objDataStr.section(" ", 2, 2, QString::SectionSkipEmpty).toDouble();
            zM = objDataStr.section(" ", 3, 3, QString::SectionSkipEmpty).toDouble();

            if(mpcObj.fromMiriStr(objDataStr))
            {
                logStm << "\nfromString error\n";
                continue;
            }
            else break;
        }

        logStm << QString("observ: %1 %2 %3\n").arg(x).arg(y).arg(z);
        logStm << QString("Miriade: %1 %2 %3\n").arg(xM).arg(yM).arg(zM);

    ////


        Sdist = sqrt(x*x + y*y + z*z);
        Edist = sqrt((obsPos.ox - x)*(obsPos.ox - x) + (obsPos.oy - y)*(obsPos.oy - y) + (obsPos.oz - z)*(obsPos.oz - z));

        magn = det_m(iCat.record->H, Sdist, Edist, 5.8, detPhase(obsPos.ox, obsPos.oy, obsPos.oz, x, y, z));

        deg_to_hms(&tRa, rad2grad(ra), " ", 4);
        deg_to_damas(&tDe, rad2grad(de), " ", 3);
        logStm << QString("name: %1\tra: %2\tde: %3\n").arg(iList.record->name).arg(tRa).arg(tDe);

        outerArguments.clear();
        outerArguments << QString("-name=\"%1\"").arg(QString(iList.record->name).simplified());
        sJD = QString("%1").arg(utc, 11, 'f',7);
        outerArguments << QString("-ep=%1").arg(sJD);
        //outerArguments << QString("-teph=2");

        outerProcess.setWorkingDirectory(miriadeProcData.folder);
        outerProcess.setProcessChannelMode(QProcess::MergedChannels);
        outerProcess.setReadChannel(QProcess::StandardOutput);

        //logStm << "outerArguments: " << miriadeProcData.name << " " << outerArguments.join("|") << "\n";

        outerProcess.start(miriadeProcData.name, outerArguments);

        if(!outerProcess.waitForFinished(miriadeProcData.waitTime))
        {
            logStm << "\nmpephProc finish error\n";
        }
        QTextStream objStream1(outerProcess.readAllStandardOutput());

        while (!objStream1.atEnd())
        //for(i=0; i<orList.size(); i++)
        {
            objDataStr = objStream1.readLine();
            //objDataStr = orList.at(i);
            if(objDataStr.at(0)=='#') continue;
            //logStm << QString("objDataStr: %1").arg(objDataStr);


            if(mpcObj.fromMiriStr(objDataStr))
            {
                logStm << "\nfromString error\n";
                continue;
            }
            else break;
        }
/


        szi = objList.size();

        qDebug() << QString("objL: %1\ntimes: %2\nraML: %3\ndeML: %4\nraL: %5\ndeL: %6\n\n").arg(szi).arg(timeL.size()).arg(raML.size()).arg(deML.size()).arg(raL.size()).arg(deL.size());

    for(i=0; i<szi; i++)
    {
        for(j=0; j<nbd; j++)
        {
            deg_to_hms(&tRa, raML.at(i)[j], " ", 4);
            deg_to_damas(&tDe, deML.at(i)[j], " ", 3);
            logStm << QString("miriade: %1\t%2\n").arg(tRa).arg(tDe);
            deg_to_hms(&tRa, rad2grad(raL.at(i)[j]), " ", 4);
            deg_to_damas(&tDe, rad2grad(deL.at(i)[j]), " ", 3);
            logStm << QString("orb: %1\t%2\n").arg(tRa).arg(tDe);

            dRa = fabs(raML.at(i)[j] - rad2grad(raL.at(i)[j]));
            dDe = fabs(deML.at(i)[j] - rad2grad(deL.at(i)[j]));
            logStm << QString("dRa: %1\tdDe: %2\n").arg(grad_to_mas(dRa)/1000.0/60.0).arg(grad_to_mas(dDe)/1000/60.0);
            resStm << QString("%1\t%2\t%3\t%4\n").arg(objList.at(i)).arg(step*j).arg(grad_to_mas(dRa)/1000/60.0).arg(grad_to_mas(dDe)/1000/60.0);
        }
    }


        logFile.close();
        iniFile.close();
        resFile.close();
        recFile.close();

    return 0;//a.exec();
}
*/
