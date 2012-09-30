#include <QtCore/QCoreApplication>
#include "./../libs/healpix/chealpix.h"

#include "./../libs/comfunc.h"

#include <QFile>
#include <QTextStream>
#include <QVector>

static QTextStream* clog = 0;
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

    //cout << msgType[type] << msg << endl;
    cout << msg << endl;
    if(clog && clog->device())
        //*clog << type << msg;
        *clog << msg;

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
    QCoreApplication a(argc, argv);
qInstallMsgHandler(customMessageHandler);
setlocale(LC_NUMERIC, "C");

    QString inFN   = QString(argv[1]);

    QString tStr;
    int iNum, i, j;
    int nNum, rNum, nrNum, nearNum;
    double dist;
    int minNr, maxNr, nrNum0;

    int k;//=0;
    double adx, ady, dx, dy, tNum;
    double nC = 1.0;
    double rMin;
    int nMin;
    QFile resFile;
    QTextStream resStm;
    QString resFN;
    double rMax;
    int iterNum, riterNum;

    QFile inFile(inFN);
    if(!inFile.open(QFile::ReadOnly)) exit(1);
    QTextStream inStm(&inFile);
    QString logFileName("./sphI.log");

    QFile* logFile = new QFile(logFileName);
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
        clog = new QTextStream(logFile);

    long ns;// = 64;


    long ipnest;

    double *data;

    QSettings *sett = new QSettings("sphI.ini", QSettings::IniFormat);

//    int solMode = sett->value("general/solutionMode", 0).toInt();
    int isZonal = sett->value("general/isZonal", 0).toInt();
    double dMin = grad2rad(sett->value("general/dMin", -90).toDouble());
    double dMax = grad2rad(sett->value("general/dMax", 90).toDouble());
//    int coefNum = sett->value("general/coefNum", 9).toInt();
     ns = sett->value("general/ns", 64).toInt();
     double r_mult = sett->value("general/r_mult", 1.0).toDouble();
    nMin = sett->value("general/nMin", 4).toInt();

    QString colSep = sett->value("general/colSep", "|").toString();
    int cx = sett->value("general/cx", 0).toInt();
    int cy = sett->value("general/cy", 1).toInt();
    int cdx = sett->value("general/cdx", 2).toInt();
    int cdy = sett->value("general/cdy", 3).toInt();
    int cn = sett->value("general/cn", 4).toInt();

    QVector <double*> dataVect;
    QVector <double> numVect;
    QVector <int> npixVect;

    long npix = nside2npix(ns);

    for(i=0; i<npix; i++)
    {
        data = new double[4];

        pix2ang_nest(ns, i, &data[1], &data[0]);
        data[1] = PI/2.0 - data[1];

        data[2] = 0;
        data[3] = 0;

        dataVect << data;
        numVect << 0;
//        npixVect<< i;
    }

    double s1 = sin(dMin);
    double s2 = sin(dMax);



    qDebug() << QString("npix: %1\tsz: %2\n").arg(npix).arg(dataVect.size());

data = new double[4];
k=0;
    while(!inStm.atEnd())
    {
        tStr = inStm.readLine();

        /*data[0] = grad2rad(tStr.section(" ", 2, 2).toDouble());
        data[1] = grad2rad(tStr.section(" ", 3, 3).toDouble());
        data[2] = grad2rad(tStr.section(" ", 4, 4).toDouble());
        data[3] = grad2rad(tStr.section(" ", 5, 5).toDouble());*/
        data[0] = tStr.section(colSep, cx, cx).toDouble();
        data[1] = tStr.section(colSep, cy, cy).toDouble();
        data[2] = tStr.section(colSep, cdx, cdx).toDouble();
        data[3] = tStr.section(colSep, cdy, cdy).toDouble();
        iNum = tStr.section(colSep, cn, cn).toDouble();
        //if(isZonal&&(data[1]<dMin||data[1]>dMax)) continue;
        if(isZonal)
        {
            data[1] = asin((2.0*sin(data[1])/(s2-s1))-(s2+s1)/(s2-s1));
            //rat = asin((2.0*sin(rat)/(rs2-rs1))-(rs2+rs1)/(rs2-rs1));
        }

        ang2pix_nest(ns, data[1]+PI/2.0, data[0], &ipnest);
        //ang2pix_nest(ns, data[1]+PI/2.0, data[0], &ipnest);
        //pix2ang_ring(ns, npix, &dect+PI/2.0, &rat);
        //dect = PI/2.0 - dect;
        //qDebug() << QString("i: %1\n").arg(ipnest);


        numVect[ipnest] = iNum;
        dataVect[ipnest][0] = data[0];
        dataVect[ipnest][1] = data[1];
        dataVect[ipnest][2] = data[2];
        dataVect[ipnest][3] = data[3];
        k++;
//        npixVect.replace(ipnest, ipnest);
    }

    qDebug() << QString("readed num: %1\n").arg(k);



    rMin = sqrt(2.0*PI*PI/npix);
    //nMin = 3;

    qDebug() << QString("rMin: %1\n").arg(rMin);



    riterNum = 0;

    for(riterNum=1; riterNum<3; riterNum+=1.0)
    {

        rMax = riterNum*rMin;
        nMin = riterNum*3;
        qDebug() << QString("riterNum: %1\nrMin: %2\tnMin: %3\n").arg(riterNum).arg(rMax).arg(nMin);

        iterNum=0;
        do
        {
            resFN = QString("./sphIter/sphi%1_%2.txt").arg(riterNum, 3, 10, QLatin1Char( '0' )).arg(iterNum, 3, 10, QLatin1Char( '0' ));
            resFile.setFileName(resFN);
            resFile.open(QFile::WriteOnly | QFile::Truncate);
            resStm.setDevice(&resFile);
     //       nrNum0 = nrNum;
            k=0;
            nNum = rNum = 0;
            nrNum = 0;
            minNr = 1000;
            maxNr = 0;
            for(i=0; i<npix; i++)
            {
                if(numVect.at(i)>0)
                {
                    rNum++;

                    resStm << QString("%1|%2|%3|%4|%5|%6|%7|0\n").arg(dataVect[i][0]).arg(dataVect[i][1]).arg(dataVect[i][0]-dataVect[i][2]).arg(dataVect[i][1]-dataVect[i][3]).arg(dataVect[i][2]).arg(dataVect[i][3]).arg(numVect[i]);

                    continue;
                }
                nNum++;
                tNum = nearNum = 0;
                dx = dy = 0.0;


                for(j=0; j<npix; j++)
                {
                    if(i==j||numVect.at(j)<10) continue;
                    dist = sqrt(pow(dataVect[j][0]-dataVect[i][0], 2.0) + pow(dataVect[j][1]-dataVect[i][1], 2.0));
                    if(dist<=rMin)
                    {
                        dx += dataVect[j][2]*(1.0-(dist/rMin))*nC;
                        dy += dataVect[j][3]*(1.0-(dist/rMin))*nC;
                        tNum += numVect[j];
                        //qDebug() << QString("ad: %1\t%2\n").arg(adx).arg(ady);
                        //dx += adx;
                        //dy += ady;
                        nearNum++;
                    }

                }
                if(nearNum>nMin)
                {
                    dataVect[i][2] = dx/nearNum;
                    dataVect[i][3] = dx/nearNum;
                    numVect[i] = tNum/nearNum;
                    resStm << QString("%1|%2|%3|%4|%5|%6|%7|1\n").arg(dataVect[i][0]).arg(dataVect[i][1]).arg(dataVect[i][0]-dataVect[i][2]).arg(dataVect[i][1]-dataVect[i][3]).arg(dataVect[i][2]).arg(dataVect[i][3]).arg(numVect[i]);
                    nrNum++;
                }
                if(minNr>nearNum) minNr = nearNum;
                if(maxNr<nearNum) maxNr = nearNum;
                //qDebug() << QString("%1: %2\n").arg(i).arg(nearNum);
                /*if((i/1000)>k)
                {
                    qDebug() << QString("%1\n").arg(i);
                    k = i/1000;
                }*/
            }

            qDebug() << QString("\n%1\n").arg(iterNum);

            qDebug() << QString("real: %1\tnull: %2\tnrNum: %3\n").arg(rNum).arg(nNum).arg(nrNum);

            qDebug() << QString("min: %1\tmax: %2\n").arg(minNr).arg(maxNr);

            iterNum++;
            resFile.close();

        }while((nNum<npix)&&(nrNum>0));


        qDebug() << QString("iterNum: %1").arg(iterNum);

    }

    ///////////
        logFile->close();
        delete clog;
        clog = 0;
        delete logFile;
        qInstallMsgHandler(0);
    
    return 0;//a.exec();
}
