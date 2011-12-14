#include <QtCore/QCoreApplication>

#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QSettings>
#include <QProcess>
#include <QDebug>


int main(int argc, char *argv[])
{
 //   qInstallMsgHandler(customMessageHandler);
    QCoreApplication app(argc, argv);

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

    //command line  ///////////////////////////////////////
        QString optName, optVal, optStr;
        QString tFile;
        QString rmCfg = QString("rulermanager.ini");
        QString cfgFileName = "ruler3.ini";
        QString resFolder = "./results";
        QString scFile = "./syscorr.ini";
        int i;


        int isCFG = 0;
        int isRD = 0;
        int isSC = 0;

        QString listFile = QString(argv[1]);

        for(i=2; i<argc; i++)
        {
            optStr = QString(argv[i]);
            optName = optStr.section("=", 0, 0);
            optVal = optStr.section("=", 1, 1);
            if(QString::compare(optName, "config", Qt::CaseSensitive)==0)
            {
                isCFG = 1;
                cfgFileName = optVal;
            }
            else if(QString::compare(optName, "resFolder", Qt::CaseSensitive)==0)
            {
                isRD=1;
                resFolder = optVal;
            }
            else if(QString::compare(optName, "syscorr", Qt::CaseSensitive)==0)
            {
                isSC = 1;
                scFile = optVal;

            }
            else
            {
                qDebug() << "\nERR: Wrong command line param: config | resFolder | syscorr";
                return 1;
            }
        }


///////// 1. Reading settings ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //stream << "started....\n";

    //BEGIN settings
    QSettings *sett = new QSettings(rmCfg, QSettings::IniFormat);

    QString r3_prog = sett->value("general/r3_prog", "./ruler3").toString();
    QString r3_prog_folder = sett->value("general/r3_prog_folder", "./").toString();
    int r3_wait = sett->value("general/r3_wait", -1).toInt();
    if(r3_wait>0) r3_wait *=1000;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


     QFile iniFile(listFile);
     iniFile.open(QIODevice::ReadOnly | QIODevice::Text);
     QTextStream iniStm(&iniFile);

     QProcess r3Process;
     QStringList outerArguments;
     int resR3;


     while(!iniStm.atEnd())
     {
         tFile = iniStm.readLine();
         outerArguments.clear();
         r3Process.setWorkingDirectory(r3_prog_folder);
         outerArguments << tFile;
         if(isCFG) outerArguments << QString("config=%1").arg(cfgFileName);
         if(isRD) outerArguments << QString("resFolder=%1").arg(resFolder);
         if(isSC) outerArguments << QString("syscorr=%1").arg(scFile);

         qDebug() << QString("%1 %2\n").arg(r3_prog).arg(outerArguments.join(" "));

         r3Process.start(r3_prog, outerArguments);

         resR3 = r3Process.waitForFinished(r3_wait);

         if(!resR3)
         {
             r3Process.close();
         }
     }

    return 0;//a.exec();
}
