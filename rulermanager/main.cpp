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
        QStringList commLine;
        QString tFile;
        QString rmCfg = QString("rulermanager.ini");

        int i;


        QString listFile = QString(argv[1]);

        for(i=2; i<argc; i++)
        {
            commLine << QString(argv[i]);

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
         outerArguments << commLine;

         qDebug() << QString("%1 %2\n").arg(r3_prog).arg(outerArguments.join(" "));

         r3Process.start(r3_prog, outerArguments);

         resR3 = r3Process.waitForFinished(r3_wait);

         if(!resR3)
         {
             r3Process.close();
             qDebug() << QString("\ntime elapsed\n");
         }
         else
         {
             qDebug() << QString(r3Process.readAllStandardOutput()) << "\n";
         }
     }

    return 0;//a.exec();
}
