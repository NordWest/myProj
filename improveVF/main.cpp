#include <QtCore>


#include "./../libs/mb.h"
#include "./../libs/vectGrid3D.h"
#include "./../libs/comfunc.h"

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

struct vectVF
{
    double px, py, pm, x, y;
    long nums;
};

int main(int argc, char *argv[])
{
    qInstallMsgHandler(customMessageHandler);
    QCoreApplication a(argc, argv);

    QFile* logFile = new QFile("messages.log");
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
        clog = new QDataStream(logFile);

     vectGrid3D *vectFdata0 = new vectGrid3D();

     vectFdata0->init(argv[1]);
     vectFdata0->initVF();

     int i, j, k;
     int szi, szj, szk;

     szi = vectFdata0->ax[0];
     szj = vectFdata0->ax[1];
     szk = vectFdata0->ax[2];

     long pos[3];
     long nums;
     double px, py, pm, x, y, x0, y0, dx, dy;

     double *Cx, *Cy, *Lx, *Ly, *Dx, *Dy, *WeX, *WeY;
     double uweX, uweY, fl, maxdiv;
     int deg;
     double *zX, *zY;

     deg = 5;
     int ki = szi*szj*szk;

     Cx = new double[deg*ki];
     Cy = new double[deg*ki];
     Lx = new double[ki];
     Ly = new double[ki];
     WeX = new double[ki];
     WeY = new double[ki];

     Dx = new double[deg*deg];
     Dy = new double[deg*deg];

     int pp;

     QFile resFile, inFile, corrFile;
     QTextStream resStm, inStm, corrStm;
     QString rFileName, inFileName, corrFileName;

     double *xLevels, *yLevels, *mLevels;
     int xLevNum, yLevNum, mLevNum;

     mLevNum = vectFdata0->ax[2]+1;
     xLevNum = vectFdata0->ax[0]+1;
     yLevNum = vectFdata0->ax[1]+1;

     mLevels = new double[mLevNum];
     xLevels = new double[xLevNum];
     yLevels = new double[yLevNum];

     qDebug() << QString("LevelsNum: %1\t%2\t%3\n").arg(xLevNum).arg(yLevNum).arg(mLevNum);


     for(k=0; k<mLevNum; k++)
     {
         mLevels[k] = vectFdata0->dims[2][k];
         qDebug() << QString("mLevels[%1]= %2\n").arg(k).arg(mLevels[k]);
     }
     for(k=0; k<xLevNum; k++)
     {
         xLevels[k] = vectFdata0->dims[0][k];
         qDebug() << QString("xLevels[%1]= %2\n").arg(k).arg(xLevels[k]);
     }
     for(k=0; k<yLevNum; k++)
     {
         yLevels[k] = vectFdata0->dims[1][k];
         qDebug() << QString("yLevels[%1]= %2\n").arg(k).arg(yLevels[k]);
     }

     vectGrid3D *vectFdata1 = new vectGrid3D();
     vectFdata1->setLevels(xLevels, xLevNum, yLevels, yLevNum, mLevels, mLevNum);
     vectGrid3D *vectFdata2 = new vectGrid3D();
     vectFdata2->setLevels(xLevels, xLevNum, yLevels, yLevNum, mLevels, mLevNum);

     long numMax = vectFdata0->getNumMax();
     double *vectF = new double[3];

     QList <double*> coefX, coefY;
     QList <double*> mCoefX, mCoefY;
     QVector <double> mVect;
     QVector <int> numVect;
     int numk;
     QString mCoefFileNameX, mCoefFileNameY;
     QFile rFileX, rFileY;
     QTextStream dataStreamX, dataStreamY;
     double *Cmx, *Cmy, *Lmx, *Lmy, *Dmx, *Dmy, *Wem;
     double uweMx, uweMy;
     double *zmX, *zmY;
     int degm;
     QStringList dataStrL;
     QString dataSep, resFunc;
     double *xT, *dxT, *dyT;
     double *mcX, *mcY;
     int numkMax;

     QStringList resListX, resListY;


     QList <vectVF*> vfList;
     vectVF* vfRec;
     int vfNums;


     for(k=0; k<szk; k++)
     {
         pos[2] = k;

         inFileName = QString("inp_%1.txt").arg(k, 2, 10, QLatin1Char( '0' ) );



         inFile.setFileName(inFileName);
         inFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
         inStm.setDevice(&inFile);

         numk = 0;

         vfList.clear();


         for(j=0; j<szj; j++)
         {
             pos[1] = j;
             for(i=0; i<szi; i++)
             {
                 pos[0] = i;

                 vectFdata0->getVect(i, j, k, &px, &py, &pm, &x, &y, &nums);
                 //qDebug() << QString("%1 : %2 : %3 : %4 : %5 : %6\n").arg(px).arg(py).arg(pm).arg(x).arg(y).arg(nums);
                 inStm << QString("%1|%2|%3|%4|%5\n").arg(px).arg(py).arg(x).arg(y).arg(nums);

                 if(nums>=20)
                 {
                     vfRec = new vectVF;

                     vfRec->px = px;
                     vfRec->py = py;
                     vfRec->pm = pm;
                     vfRec->x = x;
                     vfRec->y = y;
                     vfRec->nums = nums;

                     vfList << vfRec;
                 }
             }
         }

         vfNums = vfList.size();

         Cx = new double[deg*vfNums];
         Cy = new double[deg*vfNums];
         Lx = new double[vfNums];
         Ly = new double[vfNums];
         //WeX = new double[vfNums];
         //WeY = new double[vfNums];

         Dx = new double[deg*deg];
         Dy = new double[deg*deg];

         zX = new double[deg];
         zY = new double[deg];

         for(i=0; i<vfNums; i++)
         {

             //pp = k*szi*szj + j*szi+i;
             Lx[i] = vfList[i]->x;
             //Cx[i*deg] = vfList[i]->px*(vfList[i]->px*vfList[i]->px+vfList[i]->py*vfList[i]->py);
             Cx[i*deg] = vfList[i]->px;
             Cx[i*deg + 1] = vfList[i]->py;
             Cx[i*deg + 2] = vfList[i]->px*vfList[i]->px;
             Cx[i*deg + 3] = vfList[i]->px*vfList[i]->py;
             Cx[i*deg + 4] = vfList[i]->px*(vfList[i]->px*vfList[i]->px+vfList[i]->py*vfList[i]->py);
             //Cx[pp*deg + 5] = pm;
             //Cx[pp*deg + 6] = pm*pm;
             //WeX[pp] = nums/(numMax*1.0);
             //WeX[pp] = 1.0*(nums>=20);

             Ly[i] = vfList[i]->y;
             //Cy[i*deg] = vfList[i]->py*(vfList[i]->px*vfList[i]->px+vfList[i]->py*vfList[i]->py);
             Cy[i*deg] = vfList[i]->px;
             Cy[i*deg + 1] = vfList[i]->py;
             Cy[i*deg + 2] = vfList[i]->py*vfList[i]->py;
             Cy[i*deg + 3] = vfList[i]->px*vfList[i]->py;
             Cy[i*deg + 4] = vfList[i]->py*(vfList[i]->px*vfList[i]->px+vfList[i]->py*vfList[i]->py);
             //Cy[pp*deg + 5] = pm;
             //Cy[pp*deg + 6] = pm*pm;
             //WeY[pp] = nums/(numMax*1.0);
             //WeY[pp] = 1.0*(nums>=20);

             numk += vfList[i]->nums;

         }

         inFile.close();


         lsm(deg, vfNums, zX, Cx, Lx, uweX, Dx);

         qDebug() << QString("zX: %1\t%2\t%3\t%4\t%5\n").arg(zX[0]).arg(zX[1]).arg(zX[2]).arg(zX[3]).arg(zX[4]);

         lsm(deg, vfNums, zY, Cy, Ly, uweY, Dy);

         qDebug() << QString("zY: %1\t%2\t%3\t%4\t%5\n").arg(zY[0]).arg(zY[1]).arg(zY[2]).arg(zY[3]).arg(zY[4]);

         qDebug() << QString("uweX: %1\tuweY: %2\n").arg(uweX).arg(uweY);


         rFileName = QString("res_%1.txt").arg(k, 2, 10, QLatin1Char( '0' ) );

         resFile.setFileName(rFileName);
         resFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
         resStm.setDevice(&resFile);

         corrFileName = QString("corr_%1.txt").arg(k, 2, 10, QLatin1Char( '0' ) );

         corrFile.setFileName(corrFileName);
         corrFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
         corrStm.setDevice(&corrFile);
         for(j=0; j<szj; j++)
         {
             for(i=0; i<szi; i++)
             {
                 vectFdata0->getVect(i, j, k, &px, &py, &pm, &x0, &y0, &nums);

                 //qDebug() << QString("zX: %1\t%2\t%3\t%4\t%5\n").arg(zX[0]).arg(zX[1]).arg(zX[2]).arg(zX[3]).arg(zX[4]);
                 //qDebug() << QString("zY: %1\t%2\t%3\t%4\t%5\n").arg(zY[0]).arg(zY[1]).arg(zY[2]).arg(zY[3]).arg(zY[4]);

                 x = zX[0]*px + zX[1]*py + zX[2]*px*px + zX[3]*px*py + zX[4]*px*(px*px+py*py);// + zX[5]*pm + zX[6]*pm*pm;
                 y = zY[0]*px + zY[1]*py + zY[2]*py*py + zY[3]*px*py + zY[4]*py*(px*px+py*py);// + zY[5]*pm + zY[6]*pm*pm;

                 //x = zX[0]*px*(px*px+py*py);// + zX[5]*pm + zX[6]*pm*pm;
                 //y = zY[0]*py*(px*px+py*py);// + zY[5]*pm + zY[6]*pm*pm;

                 //qDebug() << QString("x0: %1\ty0: %2\n").arg(x0).arg(y0);
                 //qDebug() << QString("x: %1\ty: %2\n").arg(x).arg(y);

                 if(nums>0)
                 {
                     dx = x0-x;
                     dy = y0-y;
                 }
                 else
                 {
                     dx = 0.0;
                     dy = 0.0;
                 }
                 //qDebug() << QString("dx: %1\tdy: %2\n").arg(dx).arg(dy);

                 vectF[0] = px;
                 vectF[1] = py;
                 vectF[2] = pm;
                 vectFdata1->setPoint(vectF, x, y, 1);
                 if(nums>=20)
                 {
                     vectFdata2->setPoint(vectF, x0, y0, nums);
                 }
                 else
                 {
                     vectFdata2->setPoint(vectF, x, y, 20);
                 }

                 resStm << QString("%1|%2|%3|%4|%5\n").arg(px).arg(py).arg(dx).arg(dy).arg(nums);
                 corrStm << QString("%1|%2|%3|%4|%5\n").arg(px).arg(py).arg(x).arg(y).arg(nums);
             }
         }
         resFile.close();
         corrFile.close();


         mCoefFileNameX = QString("./coefX_%1.txt").arg(k, 2, 10, QLatin1Char('0'));
         mCoefFileNameY = QString("./coefY_%1.txt").arg(k, 2, 10, QLatin1Char('0'));

         rFileX.setFileName(mCoefFileNameX);
         rFileX.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
         dataStreamX.setDevice(&rFileX);

         rFileY.setFileName(mCoefFileNameY);
         rFileY.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
         dataStreamY.setDevice(&rFileY);

         resListX.clear();
         resListY.clear();

         for(i=0; i<deg; i++)
         {
             resListX << QString("%1").arg(zX[i]);
             resListY << QString("%1").arg(zY[i]);
         }

         dataStreamX << resListX.join("|");
         dataStreamY << resListY.join("|");


         coefX << zX;
         coefY << zY;

         mVect.append(pm);
         numVect.append(numk);

         rFileX.close();
         rFileY.close();

         delete [] Cx;
         delete [] Cy;
         delete [] Lx;
         delete [] Ly;
         //WeX = new double[vfNums];
         //WeY = new double[vfNums];

         delete [] Dx;
         delete [] Dy;

         //delete [] zX;
        // delete [] zY;
    }



         mCoefFileNameX = QString("./mdCoefX.txt");
         mCoefFileNameY = QString("./mdCoefY.txt");

         rFileX.setFileName(mCoefFileNameX);
         rFileX.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
         dataStreamX.setDevice(&rFileX);

         rFileY.setFileName(mCoefFileNameY);
         rFileY.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
         dataStreamY.setDevice(&rFileY);


     //vectFdata1->detMean();
     vectFdata1->initVF();
     vectFdata1->saveVF("./resModel.vf");
     vectFdata1->saveDotList("./", "|", "resModel_");

     vectFdata2->initVF();
     vectFdata2->saveVF("./resCombine.vf");
     vectFdata2->saveDotList("./", "|", "resCombine_");
/*
     QStringList resListX, resListY;

     for(i=0; i<deg; i++)
     {
         resListX << QString("%1").arg(zX[i]);
         resListY << QString("%1").arg(zY[i]);
     }

     dataStreamX << resListX.join("|");
     dataStreamY << resListY.join("|");
*/

     qDebug() << "\ndegm\n";

     szi = deg;
     degm = 2;

     Cmx = new double[degm*szk];
     Cmy = new double[degm*szk];
     Lmx = new double[szk];
     Lmy = new double[szk];
     Wem = new double[szk];
//     WemY = new double[szk];

     Dmx = new double[deg*deg];
     Dmy = new double[deg*deg];

     qDebug() << QString("szk= %1\tnumVext.size= %2\n").arg(szk).arg(numVect.size());

     numkMax = 0;
     for(k=0; k<szk; k++) if(numVect[k]>numkMax) numkMax = numVect[k];
     for(k=0; k<szk; k++) Wem[k] = numVect[k]/numkMax;


     mCoefFileNameX = QString("./mdCoefX.txt");
     mCoefFileNameY = QString("./mdCoefY.txt");

     rFileX.setFileName(mCoefFileNameX);
     rFileX.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
     dataStreamX.setDevice(&rFileX);

     rFileY.setFileName(mCoefFileNameY);
     rFileY.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
     dataStreamY.setDevice(&rFileY);

     dataSep = "|";

     qDebug() << QString("szi= %1\n").arg(szi);

     for(i=0; i<deg; i++)
     {
         qDebug() << QString("\nZ[%1]\n").arg(i);
         //zmX = new double[degm];
         //zmY = new double[degm];

         xT = new double[szk];
         dxT = new double[szk];
         dyT = new double[szk];

         for(k=0;k<szk;k++)
         {
             xT[k] = mVect[k];
             dxT[k] = coefX[k][i];
             dyT[k] = coefY[k][i];
             qDebug() << QString("%1: xT= %2\tdxT= %3\tdyT= %4\n").arg(k).arg(xT[k]).arg(dxT[k]).arg(dyT[k]);
         }



         mcX = new double[degm+1];
         mcY = new double[degm+1];

         detCurf3(&resFunc, mcX, xT, dxT, szk, degm);

         qDebug() << QString("resFunc M: %1\n").arg(resFunc);

         dataStrL.clear();
         //dataStrL << QString("%1").arg((magEq.diaps[k]+magEq.diaps[k+1])/2.0);
         for(j=0; j<degm+1; j++)
         {
              dataStrL << QString("%1").arg(mcX[j], 10, 'e');
         }

         dataStreamX << dataStrL.join(dataSep) << "\n";


         detCurf3(&resFunc, mcY, xT, dyT, szk, degm);

         qDebug() << QString("resFunc M: %1\n").arg(resFunc);

         dataStrL.clear();
         //dataStrL << QString("%1").arg((magEq.diaps[k]+magEq.diaps[k+1])/2.0);
         for(j=0; j<degm+1; j++)
         {
              dataStrL << QString("%1").arg(mcY[j], 10, 'e');
         }

         dataStreamY << dataStrL.join(dataSep) << "\n";



         delete [] mcX;
         delete [] mcY;
         delete [] xT;
         delete [] dxT;
         delete [] dyT;
     }

     delete clog;
     clog = 0;
     delete logFile;
     logFile = 0;

     qInstallMsgHandler(0);


    return 0;
    //return a.exec();
}
