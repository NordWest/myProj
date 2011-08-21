#include <QtCore/QCoreApplication>
#include "./../libs/fitsdata.h"
#include "./../libs/imgprofile.h"


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


void marksUnify1(fitsdata *fitsd, int dist, int r);
void binFilter(fitsdata *fitsd, int levl, int apSize, int aper);


int main(int argc, char *argv[])
{
    qInstallMsgHandler(customMessageHandler);

    QCoreApplication a(argc, argv);

    QFile* logFile = new QFile("messages.log");
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
        clog = new QDataStream(logFile);


    fitsdata *fitsd = new fitsdata(argv[1]);
    marksGrid *mGr = fitsd->marksGIpix;


    int levelBinMin = fitsd->minp;
    int levelBinMax = 3.0*fitsd->maxp;
    int levelBinVal = fitsd->maxp;

    qDebug() << "\nLevels: " << levelBinMin << ":" << levelBinMax << ":" << levelBinVal << "\n";

    img2d *binImg = new img2d(USHORT_TYPE, fitsd->imgArr->naxes[0], fitsd->imgArr->naxes[1]);

    long i, sz;
    QSettings *binSett =  new QSettings("binFilter.ini", QSettings::IniFormat);
    sz = fitsd->imgArr->size;
    long levl = levelBinVal;
    int apSize = binSett->value("general/apSize").toInt();
    int aper = binSett->value("general/aperture").toInt();
    int sTarget = binSett->value("general/sTarget").toInt();
    int oMode = binSett->value("general/outputmode").toInt();
    QString mSep = binSett->value("general/marksFileSep").toString();
    QString mColumn = binSett->value("general/marksLoadFileColumn").toString();

    int levstep = (levelBinVal-levelBinMin)*0.05;
    int snum;

    do
    {
        binFilter(fitsd, levl, apSize, aper);
        levl -= levstep;
        snum = mGr->marks.size();
        qDebug() << "\nStars found: " << snum << "\n";
    }while(snum<sTarget);

    imgProfile *ipos = imgProfile;


    if(oMode==0)
    {
        fitsd->detPpix(aper, 1);
        mGr->saveTanImg(argv[2], mSep, mColumn);
    }
    else if(oMode==1)
    {
        ipos->setMask("2 0 1");
        ipos->setModel(0);

    }

    qDebug() << "\nend\n";

//    return 0;
    delete clog;
    clog = 0;
    delete logFile;
    logFile = 0;

    qInstallMsgHandler(0);

    return a.exec();
}

void binFilter(fitsdata *fitsd, int levl, int apSize, int aper)
{
    marksGrid *mGr = fitsd->marksGIpix;


    img2d *binImg = new img2d(USHORT_TYPE, fitsd->imgArr->naxes[0], fitsd->imgArr->naxes[1]);

    long i, sz;
    QSettings *binSett =  new QSettings("binFilter.ini", QSettings::IniFormat);
    sz = fitsd->imgArr->size;
//    long levl = levelBinVal;

    qDebug() << "\napSize: " << apSize << "\n";


    double *val = new double[sz];
    double ppx;
    long j;
    unsigned short pp1 = 0;
    long k, l;
    long p, q;
    imgAperture *imA = new imgAperture();

    int n=0;
    for (i=0; i<sz; i++)
    {
        ppx = fitsd->imgArr->getPos(i);

        if (ppx>=levl)
        {
            val[i] = 1.0;
            n++;
//			qDebug() << "\nppx:" << ppx << "\n";
        }
        else  val[i] = 0.0;
    }
    qDebug() << "\nnlevl:" << n << "\n";
    qDebug() << "\nFIND Ape:\n";
    int pos;

    for (i=0; i<sz; i++)
    {


        if (val[i]>0)
        {
            l = i/fitsd->imgArr->naxes[0];
            k = i - l*fitsd->imgArr->naxes[0];

 //                       qDebug() << "\n>0:" << k << ":" << l<< ":" << val[i] << "\n";
            ApeData(imA, val, k, l, apSize, fitsd->imgArr->naxes[0], fitsd->imgArr->naxes[1]);
//			qDebug() << "\n" << k << ":" << l<< ":" << val[i] << "\n";
//            qDebug() << "\nape\t" << k << ":" << l<< ":" << imA->minADC << "\n";
            if (imA->minADC>0.0)
            {

                mGr->addImgMark(k, l);
                for (p=k-apSize; p<=k+apSize; p++)
                {
                    for (q=l-apSize; q<=l+apSize; q++)
                        if (sqrt((p-k)*(p-k) + (q-l)*(q-l))<apSize)
                        {
                            pos = q*fitsd->imgArr->naxes[0]+p;
//							qDebug() << "\n\tpos:" << pos << "\n";
                            if ((pos>=0)&&(pos<sz))val[pos] = 0;
                        }
                }
            }
            delete [] imA->buffer;
        }
    }

    marksUnify1(fitsd, aper, apSize);
}

void marksUnify1(fitsdata *fitsd, int dist, int r)
{
    marksGrid *mGr = fitsd->marksGIpix;

    marksP* mTemp;
    QList <marksP*> mktList;

    int i, j, k;
    int sz = mGr->marks.size();
    int sz1;
    int minX, maxX, minY, maxY, a;

    int x0, y0, x1, y1, xc, yc;
    qDebug() << "\nmarks num\t" << sz << "\n";
    int summ1, summ2, num1;

    for (i=0; i<mGr->marks.size(); i++)
    {
        mktList.clear();
        qDebug() << "\nuni\t" << i << ":";
        x0 = mGr->marks[i]->mTanImg[0];
        y0 = mGr->marks[i]->mTanImg[1];

        mTemp = new marksP;
        mTemp->mTanImg[0] = x0;
        mTemp->mTanImg[1] = y0;
        mktList << mTemp;
//        summ1 = x0;
//        summ2 = y0;
//        num1 = 1;

        k=-1;
        do
        {
            k++;
//            qDebug() << "k= " << k << "\n";
            x0 = mktList[k]->mTanImg[0];
            y0 = mktList[k]->mTanImg[1];
            sz = mGr->marks.size();
            for (j=sz-1; j>i; j--)
            {
                x1 = mGr->marks[j]->mTanImg[0];
                y1 = mGr->marks[j]->mTanImg[1];

                if(sqrt((x0 - x1)*(x0 - x1) + (y0 - y1)*(y0 - y1))<=dist)
                {
                    mTemp = new marksP;
                    mTemp->mTanImg[0] = x1;
                    mTemp->mTanImg[1] = y1;
                    mktList << mTemp;
                    mGr->remMark(j);
                    break;
                }
            }

            sz1 = mktList.size();
        }while(k<sz1-1);

        summ1 = summ2 = 0.0;
        num1 = mktList.size();
        minX = maxX = mktList[0]->mTanImg[0];
        minY = maxY = mktList[0]->mTanImg[1];
        for(k=0; k<num1; k++)
        {
            summ1 += mktList[k]->mTanImg[0];
            summ2 += mktList[k]->mTanImg[1];
            if(minX>mktList[k]->mTanImg[0]) minX = mktList[k]->mTanImg[0];
            if(maxX<mktList[k]->mTanImg[0]) maxX = mktList[k]->mTanImg[0];
            if(minY>mktList[k]->mTanImg[1]) minY = mktList[k]->mTanImg[1];
            if(maxY<mktList[k]->mTanImg[1]) maxY = mktList[k]->mTanImg[1];
        }

        if(num1>1)
        {
            if(maxY-minY>maxX-minX) a = maxY-minY;
            else a = maxX-minX;

            qDebug() << "a = " << a << "\n";
            qDebug() << "a/sX = " << a/(maxX-minX) << "\n";
            qDebug() << "a/sY = " << a/(maxY-minY) << "\n";
            qDebug() << "num= " << num1 << "\n";
            qDebug() << "koef = " << num1*PI*r*r/(a*a) << "\n";
        }

        xc = summ1/num1;
        yc = summ2/num1;
        mGr->marks[i]->mTanImg[0] = xc;
        mGr->marks[i]->mTanImg[1] = yc;

    }
}
