#include <QtCore/QCoreApplication>
#include "./../libs/hronobasefile.h"
#include "./../libs/fitsdata.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    setlocale(LC_NUMERIC, "C");

    QString codecName;
    #if defined(Q_OS_LINUX)
    codecName = "UTF-8";
    #elif defined(Q_OS_WIN)
    codecName = "CP1251";
    #endif

    QTextCodec *codec1 = QTextCodec::codecForName(codecName.toAscii().constData());
    Q_ASSERT( codec1 );
    QTextCodec::setCodecForCStrings(codec1);

    int i, szi;

    QSettings *sett = new QSettings("./utcTest.ini", QSettings::IniFormat);
    QString uJournalFile = sett->value("general/uJournalFile", "./uJournal.txt").toString();
    QString hdrDir = sett->value("general/hdrDir", "./hdr").toString();
    //int resType = sett->value("general/resType", 0).toInt();

    hronoBaseFile hbFile(uJournalFile);

    hbFile.initJournal();
    szi = hbFile.hronoList.size();

    QFile uFile("./utcCorr.txt");
    uFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream uStm(&uFile);

    for(i=0; i<szi; i++)
    {
        uStm << QString("%1|%2|%3|%4\n").arg(hbFile.hronoList.at(i)->date.toString("yyyy MM dd")).arg(hbFile.hronoList.at(i)->timeReal.toString("HH mm ss.zzz")).arg(hbFile.hronoList.at(i)->getMjdReal(), 12, 'f', 6).arg(hbFile.hronoList.at(i)->getU());
    }
    uFile.close();



    QDirIterator it(hdrDir, QDirIterator::Subdirectories);
    QRegExp rx("*.hdr");
    rx.setPatternSyntax(QRegExp::Wildcard);
    QString tFile;
    QStringList dataFiles;

    while (it.hasNext()) {

        tFile = it.next();
        if(rx.exactMatch(tFile)) dataFiles << tFile;

    }

    szi = dataFiles.size();

    QFile huFile("huCorr.txt");
    huFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream huStm(&huFile);


    QFile hFile;
    QTextStream hStream;//(&hFile);
    QString string, dateStr, timeStr, plName, uTimeHdr;

    HeadList hList;
    int year, month, day, hour, min, fuRes;
    double sec, jdNum, jdDate, sTime, uCorr, uCorrHdr, gm1, jd0, pday, jdUtc, jDay, s0, s1, dS1, dS;

    double Long = grad_to_rad(30.3274)/(2.0*PI);//day

    for(i=0; i<szi; i++)
    {
        tFile = dataFiles.at(i);

        qDebug() << QString("%1: %2\n").arg(i).arg(tFile);

        hFile.setFileName(tFile);

        if(!hFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << QString("Can't open Header file\nMentioned FITS Header file has't exist\n");
            continue;
        }

        hStream.setDevice(&hFile);




        //headList.clear();
        string.clear();
        string = hStream.readAll();

        hFile.close();

        hList.readHeader(string);

        hList.getKeyName("PLATE-N", &plName);
        hList.getKeyName("DATE-OBS", &dateStr);
        hList.getKeyName("TIME-OBS", &timeStr);
        hList.getKeyName("U", &uTimeHdr);
        uCorrHdr = uTimeHdr.trimmed().toDouble();

        year = dateStr.section(" ", 0, 0, QString::SectionSkipEmpty).toInt();//atoi(argv[1]);
        month = dateStr.section(" ", 1, 1, QString::SectionSkipEmpty).toInt();//atoi(argv[2]);
        day = dateStr.section(" ", 2, 2, QString::SectionSkipEmpty).toInt();//atoi(argv[3]);
        hour = timeStr.section(" ", 0, 0, QString::SectionSkipEmpty).toInt();//atoi(argv[4]);
        min = timeStr.section(" ", 1, 1, QString::SectionSkipEmpty).toInt();//atoi(argv[5]);
        sec = timeStr.section(" ", 2, 2, QString::SectionSkipEmpty).toDouble();//atof(argv[6]);
        //fitsd->expList->exps.at(0)->expTime;

        jdNum = dat2JDN(year, month, day);

        sTime = hour/24.0 + min/1440.0 + sec/86400.0;

        gm1 = sTime - Long;
        qDebug() << QString("sTime= %1\tgm1= %2\tLong= %3\n").arg(sTime).arg(gm1).arg(Long);
        //dat2JD(&jDay, year, month, day);

        s0 = gmst0jd(jdNum);
        s1 = gmst0jd(jdNum+1);
        dS = gm1 - s0;
        dS1 = gm1 - s1;
        qDebug() << QString("jDay= %1\ts0= %2\ts1= %3\tdS= %4\tdS1= %5\n").arg(jdNum, 10, 'f', 4).arg(s0).arg(s1).arg(dS).arg(dS1);
        //if(dS<0.0) dS +=1.0;
        //if(dS<0.5) jdNum+=1.0;

    //    if(FD_LOG_LEVEL) qDebug() << QString("dS= %1\tjDay= %2\n").arg(dS).arg(jDay, 10, 'f', 4);

        GMST1_to_jdUT1(&jdUtc, gm1, jdNum);
        qDebug() << QString("jdUTC= %1\tdS= %2\tdS1= %3\n").arg(jdUtc, 10, 'f', 4).arg(dS).arg(dS1);
        if(dS1<0.0)dS1+=1.0;
        qDebug() << QString("dS= %1\tdS1= %2\n").arg(dS).arg(dS1);
        //if((dS1>0.5)&&(dS<0.5)) jdUtc-=(1.0-VIU);


/*
        dat2JD(&jdDate, year, month, day);
        sTime = hour/24.0 + min/1440.0 + sec/86400.0;

        gm1 = sTime - Long;
        jd0 = pdayUT1(jdDate, &pday);
        GMST1_to_jdUT1(&jdUtc, gm1, jd0);
*/
     //   qDebug() << QString("DATE-OBS: %3 %4 %5\tjdNum= %1\tsTime= %2\t%6\t%7\n").arg(jdNum).arg(sTime).arg(year).arg(month).arg(day).arg(jdDate, 10, 'f', 2).arg((int)mjd2jd(fitsd->expList->exps.at(0)->expTime));







        fuRes = hbFile.findU(&uCorr, jdNum, sTime, 7);

        huStm << QString("%1|%2|%3|%4|%5|%6|%7\n").arg(plName).arg(dateStr).arg(timeStr).arg(jd2mjd(jdUtc), 12, 'f', 6).arg(uCorr).arg(uCorrHdr).arg(uCorr-uCorrHdr);

    }
    huFile.close();



    return 0;//a.exec();
}
