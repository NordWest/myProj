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

        int Width; // БШУНДМНИ ОЮПЮЛЕРП ��  ЬХПХМЮ ХГНАПЮФЕМХЪ
    int Height; // БШУНДМНИ ОЮПЮЛЕРП ��  БШЯНРЮ ХГНАПЮФЕМХЪ
    char ch1,ch2;
    fread(&ch1, 1, 1, dt);//dt.Read(&ch1,1);
    fread(&ch2, 1, 1, dt);//dt.Read(&ch2,1);
    if (ch1!='I'|| ch2!='I')
    {
        qDebug() << "Mentioned .tiff file is unsuitable\n";
             //   QMessageBox msgBox(QMessageBox::Warning, tr("Wrong .tiff file"), "Mentioned .tiff file is unsuitable", 0, this);
             //   msgBox.addButton(QMessageBox::Ok);
             //   msgBox.exec();
        return 1;//ЩРН МЕ tif
    };
    uint16_t vg;
    fread(&vg, 2, 1, dt);//dt.Read(&vg,2);
    if (vg!=42)
    {
        qDebug() << "Mentioned .tiff file has wrong type\n";
              //  QMessageBox msgBox(QMessageBox::Warning, tr("Wrong .tiff file"), "Mentioned .tiff file has wrong type", 0, this);
              //  msgBox.addButton(QMessageBox::Ok);
              //  msgBox.exec();
        return 1;// МЕ ХГБЕЯРМЮЪ ПЮГМНБХДМНЯРЭ tif-Ю
    };

        int der;
    fread(&der, 4, 1, dt);//dt.Read(&der,4);
    fseek(dt, der, SEEK_SET);//dt.Seek(der,CFile::begin);// ЯДБХЦЮЕЛЯЪ Б ТЮИ� Е Й МЮВЮ� С ЬЮОЙХ
    uint16_t vg2;
    fread(&vg2, 2, 1, dt);//dt.Read(&vg2,2);
        qDebug() << "vg2 " << vg2 << endl;
        int fres;
    for(int iop=0;iop<vg2; iop++)
    {
        uint16_t teg;
        fres = fread(&teg, 2, 1, dt);//dt.Read(&teg,2);
        fread(&vg, 2, 1, dt);//dt.Read(&vg,2);

                qDebug() << "teg " << teg;
                int kol,dat;
        if(vg==4)
        {
            fread(&kol, 4, 1, dt);//dt.Read(&kol,4);
            fread(&dat, 4, 1, dt);//dt.Read(&dat,4);
        }
        else
        {
            fread(&kol, 4, 1, dt);//dt.Read(&kol,4);
            fread(&dat, 2, 1, dt);//dt.Read(&dat,2);
            int yyu;
            fread(&yyu, 2, 1, dt);//dt.Read(&yyu,2);
        };
                qDebug() << "\tdat " << dat << endl;
        if (teg==257) Height=dat;
        if (teg==256) Width=dat;
        if (teg==258&&dat!=16)
        {
            qDebug() << "Mentioned .tiff file is not 16bit\n";
                   //     out_stream << "don't 16bit " << endl;
                   //     QMessageBox msgBox(QMessageBox::Warning, tr("Wrong .tiff file"), "Mentioned .tiff file is not 16bit", 0, this);
                    //    msgBox.addButton(QMessageBox::Ok);
                   //     msgBox.exec();
                        return 1;// tif МЕ 16-РХ АХРМШИ
        }
        }
        qDebug() << "Width " << Width << "\tHeight" << Height << endl;
        fseek(dt, 0, SEEK_END);
    unsigned int df2 = (unsigned int) ftell(dt);//dt.GetLength();// ОН� СВЮЕЛ Д� ХМС ТЮИ� Ю
    unsigned int df1=((unsigned int)(Width))*((unsigned int)(Height));
        qDebug() << "df1 " << df1 << endl;
        qDebug() << "df2 " << df2 << endl;
    df2=df2-df1*2;
        qDebug() << "df2 " << df2 << endl;

        uint16_t *img, *uimg, *headImg;
        uint16_t *img0;
        headImg = new uint16_t [df2];
    img = new uint16_t [Width];//БШДЕ� ЪЕЛ ОЮЛЪРЭ ОНД ХГНАПЮФЕМХЕ
        uimg = new uint16_t [Width];//БШДЕ� ЪЕЛ ОЮЛЪРЭ ОНД НАПЮЫЮЕЛНЕ ХГНАПЮФЕМХЕ

        uint16_t minI, maxI, tim;		//ЛХМХЛЮ� ЭМШИ Х ЛЮЙЯХЛЮ� ЭМШИ НРЯВЕР Д� Ъ ХМБЕПРХПНБЮМХЪ

        fseek(dt, 0, SEEK_SET);//dt.Seek(df2,CFile::begin);//ЯДБХЦЮЕЛ Й МЮВЮ� С ТЮИ� Ю
        fread(headImg, sizeof(uint16_t), df2, dt);
        //fwrite(headImg, sizeof(uint16_t), df2, dtc);

        fseek(dt, df2, SEEK_SET);//dt.Seek(df2,CFile::begin);//ЯДБХЦЮЕЛ Й МЮВЮ� С ХГНАПЮФЕМХЪ
        //fseek(dtc, df2, SEEK_SET);

        long nelements;
        long *naxes;
        naxes = new long[2];

        naxes[1] = Height;
        naxes[0] = Width;
        nelements = naxes[0]*naxes[1];

        int aper;
        QString dataS;
        QStringList dL;
        double *x, *y;
        int *ap;
        QList <iDot*> dotList;
        iDot *dot;
        int k=0;

        long aperNums = 0;

        if(argc>2)
        {
            //aper = atoi(argv[3]);
            QTextStream aper_stream;
            QFile fape(mksName);
            fape.open(QIODevice::ReadOnly | QIODevice::Text);
            aper_stream.setDevice(&fape);

            while(!aper_stream.atEnd())
            {
                dataS = aper_stream.readLine();
                dL = dataS.split(" ", QString::SkipEmptyParts);
                dot = new iDot;
                dot->x = dL.at(1).toDouble();
                dot->y = dL.at(2).toDouble();
                aper = dL.at(3).toDouble();
                aperNums += aper;
                dot->ape = sqrt(aper) + 1;
                if(dot->ape<35) dot->ape = 35;
                dot->fl = dot->y - dot->ape - 1;
                dot->ll = dot->y + dot->ape + 1;
                if(dot->fl<0) dot->fl = 0;

                dotList << dot;
            }
        }

        qDebug() << QString("dot size: %1\n").arg(dotList.size());

        maxI = pow(2, 16);

        double dist;
        int dotSize = dotList.size();
        int zero;
        double ymin, posMin;
        int cutKey = 0;
        if(argc>3) cutKey = atoi(argv[3]);

        qDebug() << QString("cutKey: %1\n").arg(cutKey);

        switch(cutKey)
        {
        case 0:
            {
                //QByteArray tb = resName.toAscii();
                sprintf(resCname, "%s.tif", argv[2]);
                FILE *dtc = fopen(resCname, "wb");

                if(dtc==NULL)
                {
                    qDebug() << QString("resFile %1 not open\n").arg(resName);
                    return 1;
                }

                fwrite(headImg, sizeof(uint16_t), df2, dtc);
                fseek(dtc, df2, SEEK_SET);

            for(i=0; i<dotSize-1; i++)
            {
                ymin = dotList.at(i)->fl;
                posMin = -1;
                for(j=i+1; j<dotSize; j++)
                {
                    if(ymin>dotList.at(j)->fl)
                    {
                        ymin = dotList.at(j)->fl;
                        posMin = j;
                    }
                }
                if(posMin!=-1) dotList.swap(i, posMin);

            }

            int *fmark = new int[Height];
            int *lmark = new int[Height];

    //        int fl, ll;

            //fmark[0] = 0;

            for(i=0; i<naxes[1]; i++)
            {
                fmark[i] = dotSize-1;
                lmark[i] = 0;

                for(k=0; k<dotSize; k++)
                {
                    //fl = dotList.at(k)->fl;

                    if(dotList.at(k)->fl<=i&&dotList.at(k)->ll>=i)
                    {
                        if(k<fmark[i]) fmark[i] = k;
                        if(k>lmark[i]) lmark[i] = k;
                    }

                }
                qDebug() << QString("%1: %2\t%3\n").arg(i).arg(fmark[i]).arg(lmark[i]);
            }

            x = new double[dotSize];
            y = new double[dotSize];
            ap = new int[dotSize];


            for(k=0; k<dotSize; k++)
            {
                x[k] = dotList.at(k)->x;
                y[k] = dotList.at(k)->y;
                ap[k] = dotList.at(k)->ape;
                qDebug() << QString("%1: %2\t%3\t%4\t%5\n").arg(k).arg(x[k]).arg(y[k]).arg(ap[k]).arg(dotList.at(k)->fl);
            }
            dotList.clear();


            //for(i=0; i<naxes[1];i++)
            i=0;
            //QTime timer;
            int fkt;
            //timer.start();

            long uiNums = 0;


            while(!feof(dt))
            {
                //for(j=0; j<naxes[0]; j++) uimg[j] = maxI;
                for(j=0; j<naxes[0]; j++) uimg[j] = 0;
                fread(img, sizeof(uint16_t), Width, dt);	//ВХРЮЕЛ ОНЯРПНВМН
                if(argc>2&&(lmark[i]-fmark[i]>=0))
                {
                    for(j=0; j<naxes[0]; j++)
                    {
      //                  zero = 1;
                        for(k=fmark[i]; k<=lmark[i]; k++)
                        {
                            dist = sqrt(pow((j-x[k]),  2.0) + pow((i-y[k]),  2.0));
                            if(dist<=ap[k])
                            {
                                uimg[j] = img[j];//zero = 0;
                                uiNums++;
                                break;
                            }
                        }
    //                    if(zero) img[j] = 0;
                    }
                }
                fwrite(uimg, sizeof(uint16_t), Width, dtc);    //ОХЬЕЛ ОНЯРПНВМН
                qDebug() << i << "/" << naxes[1] << "\t" << fmark[i] << "\t" << lmark[i] << "\t" << (lmark[i] - fmark[i]) << "\n";
                i++;
            }

            qDebug() << QString("uiNums = %1\taperNums= %2").arg(uiNums).arg(aperNums);


            fclose(dtc);
            }
            break;
        case 1:
            {
                QDir cDir;
                cDir.setPath(curDir);
                QString fDir = QString("%1.fits").arg(mksName);
                sprintf(resCname, "%s.fits", argv[2]);
                int dirExist = cDir.exists(fDir);
                qDebug() << QString("dir Exist: %1\n").arg(dirExist);
                if(!dirExist)
                {
                    cDir.mkdir(fDir);
                }

                fitsfile *fptr_out;
                int status = 0;
                char *suff = new char[256];
                long fnelem;
                long *fNaxes;
                fNaxes = new long[2];

                int x0, y0, iSh;



                int fWidth, fHeight;
                for(i=0; i<dotSize; i++)
                {
                    qDebug() << QString("%1/%2\n").arg(i).arg(dotSize);
                    fWidth = dotList.at(i)->ape;
                    fHeight = dotList.at(i)->ape;
                    img0 = new uint16_t [fWidth];

                    sprintf(suff, "%s%c%d.fits", resCname, QDir::separator().toAscii(), i);
                    qDebug() << QString("suff: %1\n").arg(suff);

                    fits_open_file(&fptr_out, suff, READWRITE, &status);

                    qDebug() << QString("open_old %1").arg(status) << endl;
                    status = 0;
                    fits_delete_file(fptr_out, &status);
                    qDebug() << QString("delete %1").arg(status) << endl;
                    status = 0;
                    fits_create_file(&fptr_out, suff, &status);
                    qDebug() << QString("create %1").arg(status) << endl;
                    status = 0;

                    fits_create_hdu(fptr_out, &status);
                    qDebug() << QString("create_hdu %1").arg(status) << endl;
                    status = 0;

                    fNaxes[1] = fHeight;
                    fNaxes[0] = fWidth;
                    fnelem = fNaxes[0]*fNaxes[1];

                    qDebug() << QString("fNaxes[0]= %1\tfNaxes[1]: %2\tfnelem: %3\n").arg(fNaxes[0]).arg(fNaxes[1]).arg(fnelem);


                    fits_create_img(fptr_out, USHORT_IMG, 2, fNaxes, &status);
                    qDebug() << QString("fits_create_img: %1\n").arg(status);
                    status = 0;



                    x0 = dotList.at(i)->x - fHeight/2;
                    y0 = dotList.at(i)->y - fWidth/2;
                    iSh = Width - fWidth;

                    char *sVal = new char[255];
                    sprintf(sVal, "%d", x0);
                    fits_write_key_str(fptr_out, "XPOSITION", sVal, "", &status);
                    status = 0;
                    sprintf(sVal, "%d", y0);
                    fits_write_key_str(fptr_out, "YPOSITION", sVal, "", &status);
                    status = 0;

                    qDebug() << QString("x0: %1\ty0: %2\tiSh: %3\n").arg(x0).arg(y0).arg(iSh);

                    fseek(dt, (df2 + (Width*y0)*sizeof(uint16_t)), SEEK_SET);
                  //  img = new uint16_t [fWidth];
                    for(j=0; j<fNaxes[1];j++)
                    {
                        //fseek(dt, (df2 + Width*(y0+j) + x0), SEEK_SET);
                        fread(img, sizeof(uint16_t), Width, dt);
                        //fread(img0, sizeof(uint16_t), fNaxes[0], dt);
                        for(k=0; k<fWidth; k++)
                        {
                            if((k+x0)<Width)img0[k] = img[k+x0];
                            else img0[k] = maxI;
                        }
                        fits_write_img(fptr_out, TUSHORT, fNaxes[0]*j+1, fNaxes[0], (void*) img0, &status);
                        //fseek(dt, iSh, SEEK_CUR);

                        status = 0;
                    }

                    fits_close_file(fptr_out, &status);

                }

            }
            break;
        }

        fpos_t pos;
        int ipos = fgetpos (dt,&pos);

        fseek(dt, 0, SEEK_END);
        unsigned int df3 = (unsigned int) ftell(dt);
        qDebug() << QString("pos= %1\tend= %2\n").arg(ipos).arg(df3);

        fclose(dt);
        //fclose(dtc);
        //fout.close();

        qDebug() << QString("time elapsed: %1 min\n").arg(timer.elapsed()/1000.0/60.0);
	

        delete clog;
        clog = 0;
        delete logFile;
        logFile = 0;

        qInstallMsgHandler(0);

    return 0;
		
}
