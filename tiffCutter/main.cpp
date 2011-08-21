//test cvs
#include <QApplication>
//#include <QLibrary>
#include <QtGui>
//#include <QTranslator>
#include <QString>
#include <QByteArray>

//#include "OpenFileExDlg.h"
#include "./../libs/fitsio.h"
#include "./../libs/longnam.h"
//#include <iostream.h>
//#include <conio.h>
#include <QtCore>
#include "math.h"
#include "inttypes.h"
//#include "astro.h"
//#include "dialog.h"
//#include "ccdframe.h"

//#include <boost/numeric/ublas/vector.hpp>
//#include <boost/numeric/ublas/matrix.hpp>

//#include "..\libs\comfunc.h"
//#include "..\libs\DynArr.h"

struct iDot
{
    double x, y;
    int ape;
    int fl, ll;
};


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
 

int main(int argc, char *argv[])        //.exe .tif .mks cutKey
{  
     qInstallMsgHandler(customMessageHandler);
	QApplication app(argc, argv);
        setlocale(LC_NUMERIC, "C");
        //QTextStream out_stream;
        //QFile fout("tiff2fits.log");
        //fout.open(QIODevice::WriteOnly | QIODevice::Text);
        //out_stream.setDevice(&fout);

        QTextCodec *codec1 = QTextCodec::codecForName("Windows-1251");
        Q_ASSERT( codec1 );

        QString fileName = codec1->toUnicode(argv[1]);

        QString logFileName = QString("%1.log").arg(fileName);
        QString curDir = QString(fileName);
        if(curDir.lastIndexOf(QDir::separator ())!=-1)curDir = curDir.left(curDir.lastIndexOf("\\"));
        else curDir = QString("./");

        QTime timer;
        timer.start();

        QFile* logFile = new QFile(logFileName);
        if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
            clog = new QDataStream(logFile);

        int i, j;
        qDebug() << "curDir " << curDir << "\n";
        FILE *dt = fopen(argv[1], "rb");
        if(dt==NULL)
        {
            qDebug() << "Mentioned .tiff file are not exist\n";
//		errorMessageDialog->showMessage(tr("Can't open FITS file"));
               // QMessageBox msgBox(QMessageBox::Warning, tr("Can't open .tiff file"), "Mentioned .tiff file are not exist", 0, this);
             //   msgBox.addButton(QMessageBox::Ok);
             //   msgBox.exec();

                return 1;
        }

        QString mksName = codec1->toUnicode(argv[2]);

        QString resName = mksName + ".tif";
        char *resCname = new char[255];

//        resName = resName.section(".tif", 0, 0) + QString("_cut.tif");

        //FILE *dtc = fopen(resName.toAscii().data(), "wb");

        int Width; // Рїв–’Рїв•—РїС‘РїВІРїв– РїВ°РїВІРїв‰¤ РїВ·Рїв•ќРїГ·Рїв•ќРївЊЎРїв€™Рїв•ђРїГ· Р‘в”¬  Рїв•›Рїв•”РїГ·Рїв•”РїВ°Рїв•ќ Рїв•”РївЊ РїВІРїв–‘РїГ·Рїв•ќРїв•“Рїв€™РїВ°Рїв•”Рїв•™
    int Height; // Рїв–’Рїв•—РїС‘РїВІРїв– РїВ°РїВІРїв‰¤ РїВ·Рїв•ќРїГ·Рїв•ќРївЊЎРїв€™Рїв•ђРїГ· Р‘в”¬  Рїв–’Рїв•—Рїв•ћРїВІРїв•ђРїв•ќ Рїв•”РївЊ РїВІРїв–‘РїГ·Рїв•ќРїв•“Рїв€™РїВ°Рїв•”Рїв•™
    char ch1,ch2;
    fread(&ch1, 1, 1, dt);//dt.Read(&ch1,1);
    fread(&ch2, 1, 1, dt);//dt.Read(&ch2,1);
    if (ch1!='I'|| ch2!='I')
    {
        qDebug() << "Mentioned .tiff file is unsuitable\n";
             //   QMessageBox msgBox(QMessageBox::Warning, tr("Wrong .tiff file"), "Mentioned .tiff file is unsuitable", 0, this);
             //   msgBox.addButton(QMessageBox::Ok);
             //   msgBox.exec();
        return 1;//Рїв•