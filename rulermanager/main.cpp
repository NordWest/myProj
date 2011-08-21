#include <QApplication>
#include "basewidget.h"


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




int main(int argc, char *argv[])
    {
    	qInstallMsgHandler(customMessageHandler); 
    	
		QApplication app(argc, argv);
		
		 QFile* logFile = new QFile("messages.log");
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
        clog = new QDataStream(logFile);
		
		baseWidget *bw = new baseWidget;
        bw->show();
        
        int exit_code = app.exec();

	    delete clog;
	    clog = 0;
	    delete logFile;
	    logFile = 0;
	
	    qInstallMsgHandler(0);
	
	    return exit_code;
		/*//BEGIN read settings
		QSettings *settings = new QSettings("rulermanager.ini",QSettings::IniFormat);
		QString ruler2name = settings->value("general/ruler2name").toString();
		QString ruler2folder = settings->value("general/ruler2folder").toString();
		QString workingfolder = settings->value("general/workingfolder").toString();
		QString ruler2outputfolder = settings->value("general/ruler2outputfolder").toString();
		//ruler2outputfolder=D:/processing/out
		//END read settings
		//BEGIN list of files
		QDir dir(workingfolder);
		QStringList filters;
		filters << "*.fit" << "*.fits" << "*.fts";
		dir.setNameFilters(filters); 
	    QStringList flist = dir.entryList();
		QProcess outerProcess;
		QStringList outerArguments;
		outerProcess.setWorkingDirectory(ruler2folder);
		outerProcess.setProcessChannelMode(QProcess::MergedChannels);
		outerProcess.setReadChannel(QProcess::StandardOutput);
		for (int i=0;i<flist.count();i++)
		{
			outerArguments.clear();
			outerArguments << workingfolder+"/"+flist.at(i);
			outerProcess.start(ruler2name, outerArguments);
			outerProcess.waitForFinished(-1);
		}
		//stream << "\n rulermanager finished...\n";
		return 0;*/  
	}
