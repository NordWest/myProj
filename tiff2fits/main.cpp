//test cvs
#include <QApplication>
//#include <QLibrary>
#include <QtGui>
//#include <QTranslator>
#include <QString>
#include <QByteArray>

#include "OpenFileExDlg.h"
#include "fitsio.h"
//#include <iostream.h>
//#include <conio.h>
#include <QtCore>
#include "math.h"
//#include "astro.h"
//#include "dialog.h"
#include "maindlg.h"
//#include "ccdframe.h"

//#include <boost/numeric/ublas/vector.hpp>
//#include <boost/numeric/ublas/matrix.hpp>

//#include "..\libs\comfunc.h"
//#include "..\libs\DynArr.h"


/* int make_fits_header(fitsfile *fptr, char *fheadfn);
void make_date_obs(char *str_date_obs, char *str_time_obs, char *str_date_end, char *str_time_end);
void make_good_RADEC(char *str_ra, int type);
int make_good_fits_header(char *fheader_name);

int streqv(char *str1, char *str2);
int slovo_stopsim(char *str, char *slo, char* stopsim, int posb, int *pose, int nmax);

int dat2JD(double *Jday, int year, int mth, double day);
int dat2YMD(double Jday, int *year, int *mth, double *day);
void day2HMS(double day, int *iday, int *hour, int *min, double *sec);
void HMS2day(double *day, int hour, int min, double sec);
int isVes(int year);
double dinm(int mounth, int isves);
 */
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
	

        QFile* logFile = new QFile("tiff2fits.log");
        if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
            clog = new QDataStream(logFile);
        /*
	QTextStream out_stream;
	QFile fout("tiff2fits.log");
	fout.open(QIODevice::WriteOnly | QIODevice::Text);
	out_stream.setDevice(&fout);

        fout.close();*/
	
        mainDlg dialog;
    return dialog.exec();
		
}
