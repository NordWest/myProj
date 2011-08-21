#include <QtCore>

#include "./../libs/httpheader.h"

int main(int argc, char *argv[])    //instr plateName
{
   // QTextCodec *codec1 = new QTextCodec("KOI8-R");
    //QTextCodec::setCodecForCStrings(QTextCodec::codecForName("KOI8-R"));
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("KOI8-R"));

    QString codecName;
    #if defined(Q_OS_LINUX)
        codecName = "UTF-8";
    #elif defined(Q_OS_WIN)
        codecName = "CP1251";
    #endif

    QTextCodec *codec1 = QTextCodec::codecForName(codecName.toAscii().constData());
    Q_ASSERT( codec1 );
    QTextCodec::setCodecForCStrings(codec1);

    QCoreApplication app(argc, argv);


    if(argc!=2)
    {
            QTextStream stream(stdout);
            stream << "httpheader: command line needed." << "\n";
            return 1;
    }
    else
    {
            httpHeaderRequest *headReq = new httpHeaderRequest(&app);
            return app.exec();
    }



 //   return a.exec();
}
