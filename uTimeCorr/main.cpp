#include <QtCore/QCoreApplication>
#include "./../libs/hronobasefile.h"
//#include "./../libs/fitsdata.h"


static QDataStream* clog = 0;
void customMessageHandler(QtMsgType type, const char* msg)
{
    static const char* msgType[] =
    {
        "Debug    : ",
        "Warning  : ",
        "Critical : ",
        "Fatal    : "
    };

    static QTextStream cout(stdout);
    static QTextStream cerr(stderr);

    cerr << msgType[type] << msg << endl;
    if(clog && clog->device())
        *clog << type << msg;
    if(type == QtFatalMsg)
    {
        cerr << "aborting..." << endl;

#if defined(Q_CC_MSVC) && defined(QT_DEBUG) && defined(_CRT_ERROR) && defined(_DEBUG)
        int reportMode = _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_WNDW);
        _CrtSetReportMode(_CRT_ERROR, reportMode);
        int ret = _CrtDbgReport(_CRT_ERROR, __FILE__, __LINE__, QT_VERSION_STR, msg);
        if(ret == 0 && reportMode & _CRTDBG_MODE_WNDW)
            return;
        else if(ret == 1)
            _CrtDbgBreak();
#endif

#if defined(Q_OS_UNIX) && defined(QT_DEBUG)
        abort();
#else
        exit(1);
#endif
    }
}

/*
int main(int argc, char *argv[])
{
    qInstallMsgHandler(customMessageHandler);
    QCoreApplication a(argc, argv);

    QTextStream stream(stdout);

    QString logFileName = QString("messages.log");

    QFile* logFile = new QFile(logFileName);
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
        clog = new QDataStream(logFile);

    QTextCodec *codec1 = QTextCodec::codecForName("KOI8-R");
    Q_ASSERT( codec1 );

    QSettings *sett = new QSettings("./utCorr.ini", QSettings::IniFormat);
    QString uJournalFile = sett->value("general/uJournalFile", "./uTimeCorr.txt");

    hronoBaseFile hbFile(uJournalFile);

    hbFile.initJournal();

    int rSize = hbFile.hronoList.size();
    qDebug() << QString("rec num: %1\n").arg(rSize);

    QFile oFile("resChart.txt");
    oFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    QTextStream outs(&oFile);

    double lam = 30.3274/360.0;
/*
    double jdt1, jdt0, djd;
    jdt1 = 0;
    for(int i=0; i<rSize; i++)
    {
        jdt0 = jdt1;
        jdt1 = hbFile.hronoList.at(i)->getJDT(lam);

        djd = jdt1 - jdt0;
        if(djd<-10.0) outs << QString("%1|%2|%3\n").arg(hbFile.hronoList.at(i)->date.toString("dd.MM.yyyy")).arg(hbFile.hronoList.at(i)->timeReal.toString("HH.mm.ss.zzz")).arg(djd);
    }
/




    QFile plFile(codec1->toUnicode(argv[1]));
    if(!plFile.open(QIODevice::ReadOnly)) qDebug() << "\nFile not open\n";
    QTextStream plStr(&plFile);
    QString tStr;
    QStringList plList;
    int i;
    QString ostr;

    double t0, dt;
    QString errS;



    QString pnStr;
    fitsdata *fitsd = new fitsdata();
    QString get_http_prog_folder = "./";
    QString get_http_prog = "./getHttpHeader.exe";
    QProcess outerProcess;
    QStringList outerArguments;
    QString dateStr, timeStr, uStr;
    int jdNum;
    double uCorr;
    double sTime;

    //int *histG = new int[6];
    //for(i=0; i<6;i++) histG[i] = 0;

    while(!plStr.atEnd())
    {
        tStr = plStr.readLine();
        pnStr = tStr.left(tStr.indexOf(" "));
        qDebug() << QString("plate Name: %1\n").arg(pnStr);
        plList << pnStr;
    }
    int plSz = plList.size();
    qDebug() << QString("plates num: %1\n").arg(plSz);


    int fuRes;


    for(i=0; i<plSz; i++)
    {

    pnStr = plList.at(i);
    qDebug() << QString("plate Name: %1\n").arg(pnStr);





    outerArguments.clear();
    outerProcess.setWorkingDirectory(get_http_prog_folder);
    outerProcess.setProcessChannelMode(QProcess::MergedChannels);
    outerProcess.setReadChannel(QProcess::StandardOutput);
    //outerArguments << instruments->curName.toLower() << pnStr;
    outerArguments << QString("na") << pnStr;
    qDebug() << get_http_prog << outerArguments.join(" ");
    outerProcess.start(get_http_prog, outerArguments);

    outerProcess.waitForFinished(-1);
    QTextStream catStream(outerProcess.readAllStandardOutput());

    fitsd->readHttpHeader(catStream.readAll());




    fitsd->headList.getKeyName("DATE-OBS", &dateStr);
    fitsd->headList.getKeyName("TIME-OBS", &timeStr);

    int year, month, day;

    year = dateStr.section(" ", 0, 0).toInt();
    month = dateStr.section(" ", 1, 1).toInt();
    day = dateStr.section(" ", 2, 2).toInt();

    //fitsd->expList->exps.at(0)->expTime;

    jdNum = dat2JDN(year, month, day);
    double jdDate;
    dat2JD(&jdDate, year, month, day+0.5);
    sTime = timeStr.section(" ", 0, 0).toInt()/24.0 + timeStr.section(" ", 1, 1).toInt()/1440.0 + timeStr.section(" ", 2, 2).toDouble()/86400.0;

    qDebug() << QString("DATE-OBS: %3 %4 %5\tjdNum= %1\tsTime= %2\t%6\t%7\n").arg(jdNum).arg(sTime).arg(year).arg(month).arg(day).arg(jdDate, 10, 'f', 2).arg((int)mjd2jd(fitsd->expList->exps.at(0)->expTime));


    fuRes = hbFile.findU(&uCorr, jdNum, sTime, 7);
    fitsd->headList.getKeyName("U", &uStr);
    qDebug() << "U " << uStr << "\n";

    if(fuRes) uCorr = uStr.toDouble();

    double du1 = uStr.toDouble()-uCorr;

    //if((uStr.lastIndexOf(" .00")==-1)&&(fabs(du1)>30)) outs << QString("%1|%2|%3|%4|%5\n").arg(pnStr).arg(uStr).arg(uCorr).arg(fuRes).arg(du1);
    //if(!fuRes&&(uStr.lastIndexOf(" .00")==-1)&&(fabs(du1)>10)) outs << QString("%1|%2|%3|%4|%5\n").arg(pnStr).arg(uStr).arg(uCorr).arg(fuRes).arg(du1);
    /*if(!fuRes&&(uStr.lastIndexOf(" .00")==-1))
    {
        outs << QString("%1|%2|%3|%4|%5\n").arg(pnStr).arg(uStr).arg(uCorr).arg(fuRes).arg(du1);

        if(fabs(uCorr)<1) histG[0]++;
        if(fabs(uCorr)>=1&&fabs(uCorr)<5) histG[1]++;
        if(fabs(uCorr)>=5&&fabs(uCorr)<10) histG[2]++;
        if(fabs(uCorr)>=10&&fabs(uCorr)<30) histG[3]++;
        if(fabs(uCorr)>=30&&fabs(uCorr)<120) histG[4]++;
        if(fabs(uCorr)>=120) histG[5]++;
    }/
        stream << QString("%1")
    }

    //for(i=0; i<6;i++) qDebug() << QString("diap[%1]= %2\n").arg(i).arg(histG[i]);
/*
    int i;
    t0 = 0;
    double u;
    for(i=0; i<rSize; i++)
    {
        //qDebug() << QString("%1 %2\n").arg(hbFile.hronoList.at(i)->date.toJulianDay()).arg(hbFile.hronoList.at(i)->timeHrono.toString("hh mm ss.zzz"));
        errS.clear();
        dt = hbFile.hronoList.at(i)->getMjdReal() - t0;
        u = hbFile.hronoList.at(i)->getU();
        if((fabs(u)<600))
        {
        if(dt<0)
        {

//            outs << QString("%1|%2|%3\n").arg(hbFile.hronoList.at(i)->date.toString("yyyy MM dd")).arg(hbFile.hronoList.at(i)->getMjdHrono()).arg(hbFile.hronoList.at(i)->getU()*86400.0);
            errS = QString("%1|").arg(dt, 9, 'f', 3);
        }
            ostr.clear();
            hbFile.hronoList.at(i)->toString(&ostr);
            //outs << errS << ostr << "\n";


            outs << QString("%3|%1|%2\n").arg(hbFile.hronoList.at(i)->getMjdReal(), 13, 'f', 6).arg(u).arg(hbFile.hronoList.at(i)->date.toString("yyyy MM dd"));
        }
     //   ostr.clear();
      //  hbFile.hronoList.at(i)->toString(&ostr);
       // outs << errS << ostr << "\n";

        if(dt>0) t0 = hbFile.hronoList.at(i)->getMjdReal();
    }
/

    oFile.close();
    //////////////////////////////////////////////////////////

    delete clog;
    clog = 0;
    delete logFile;
    logFile = 0;

    qInstallMsgHandler(0);

    return 0;
}
*/
/*
int main(int argc, char *argv[])
{
    //qInstallMsgHandler(customMessageHandler);
    QCoreApplication a(argc, argv);

    QTextStream stream(stdout);

    QString dateStr = QString(argv[1]);
    QString timeStr = QString(argv[2]);

    QString logFileName = QString("utcorr.log");
/*
    QFile* logFile = new QFile(logFileName);
    if(logFile->open(QFile::WriteOnly | QIODevice::Append | QIODevice::Unbuffered))
 /
    QTextCodec *codec1 = QTextCodec::codecForName("KOI8-R");
    Q_ASSERT( codec1 );

    QSettings *sett = new QSettings("./utCorr.ini", QSettings::IniFormat);
    QString uJournalFile = sett->value("general/uJournalFile", "./uJournal.txt").toString();
    int resType = sett->value("general/resType", 0).toInt();

    hronoBaseFile hbFile(uJournalFile);

    hbFile.initJournal();


    double lam = 30.3274/360.0;

    QString filePath = "./";


    QString tStr;
    QStringList plList;
    int i;
    QString ostr;

    double t0, dt;
    QString errS;



    int jdNum;
    double uCorr;
    double sTime;

    int fuRes;
    int year, month, day, hour, min;
    double sec;

    year = dateStr.section(" ", 0, 0, QString::SectionSkipEmpty).toInt();//atoi(argv[1]);
    month = dateStr.section(" ", 1, 1, QString::SectionSkipEmpty).toInt();//atoi(argv[2]);
    day = dateStr.section(" ", 2, 2, QString::SectionSkipEmpty).toInt();//atoi(argv[3]);
    hour = timeStr.section(" ", 0, 0, QString::SectionSkipEmpty).toInt();//atoi(argv[4]);
    min = timeStr.section(" ", 1, 1, QString::SectionSkipEmpty).toInt();//atoi(argv[5]);
    sec = timeStr.section(" ", 2, 2, QString::SectionSkipEmpty).toDouble();//atof(argv[6]);
    //fitsd->expList->exps.at(0)->expTime;

    jdNum = dat2JDN(year, month, day);
    double jdDate;
    dat2JD(&jdDate, year, month, day+0.5);
    sTime = hour/24.0 + min/1440.0 + sec/86400.0;



    fuRes = hbFile.findU(&uCorr, jdNum, sTime, 7);


    QString resStr;

    resStr = QString("%1").arg(uCorr, 7, 'f', 2);


    stream << resStr;



    return fuRes;
}
*/
int main(int argc, char *argv[])
{
    //qInstallMsgHandler(customMessageHandler);
    QCoreApplication a(argc, argv);

    QTextStream stream(stdout);

    double mJD = QString(argv[1]).toDouble();

    QString logFileName = QString("utcorr.log");

    QFile* logFile = new QFile(logFileName);
    if(logFile->open(QFile::WriteOnly | QIODevice::Append | QIODevice::Unbuffered))
        clog = new QDataStream(logFile);

    QTextCodec *codec1 = QTextCodec::codecForName("KOI8-R");
    Q_ASSERT( codec1 );

    QSettings *sett = new QSettings("./utCorr.ini", QSettings::IniFormat);
    QString uJournalFile = sett->value("general/uJournalFile", "./uJournal.txt").toString();
    int resType = sett->value("general/resType", 0).toInt();

    hronoBaseFile hbFile(uJournalFile);

    hbFile.initJournal();


    double uCorr;


    int fuRes;

    fuRes = hbFile.findU_mjd(&uCorr, mJD);


    QString resStr;

    if(fuRes) resStr = QString("err\n");
    else resStr = QString("%1\n").arg(uCorr, 7, 'f', 2);


    stream << resStr;




    return fuRes;
}

