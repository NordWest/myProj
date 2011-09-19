#include "sysCorr.h"

//////////////////////////



//  syscorr

double magEqCorr(double mag, double x, QList <double*> mCoefX, int mDeg)
{
    int i, j;
    int xDeg = mCoefX.size();
    double *Zx = new double[xDeg];
    double res;

    res = 0;
    for(i=0; i<xDeg; i++)
    {
        Zx[i] = 0;
        for(j=0; j<mDeg; j++)
        {
            Zx[i] += mCoefX[i][j]*pow(mag, j);
        }
        res += Zx[i]*pow(x, i);
    }
    //qDebug() << QString("magEqCorr res: %1\n").arg(res, 12, 'g');
    return res;
}


double comaCorr(double x, double mag, double c, double mag0)
{
    return(c*x*(mag-mag0));
}

sysCorrParam::sysCorrParam()
{
    isMagEqCorr = isComaCorr = isVfCorr0 = isVfCorr1 = isMdCorr = 0;
    comaCorr = NULL;
    magEqCorr = NULL;
    vfCorr0 = NULL;
    vfCorr1 = NULL;
    mdCorr = NULL;
}

int sysCorrParam::init(QString confName)
{
    qDebug() << "syscorr init\n";
    QSettings *sett = new QSettings(confName, QSettings::IniFormat);

    int i;
    QStringList allSett;
    allSett << sett->allKeys();
    int sz = allSett.size();
    qDebug() << QString("\n\nconf.ini: %1\n").arg(confName);
    for(i=0; i<sz; i++) qDebug() << QString("%1=%2\n").arg(allSett.at(i)).arg(sett->value(allSett.at(i)).toString());
    qDebug() << "\n\n";

    int nmin, mDeg, ctype, isW;
    QString vfCorrFile;
    double rMax;



    //syscorr
        isMagEqCorr = sett->value("general/isMagEqCorr", 0).toInt();
        isComaCorr = sett->value("general/isComaCorr", 0).toInt();
        isVfCorr0 = sett->value("general/isVfCorr0", 0).toInt();
        isMdCorr = sett->value("general/isMdCorr", 0).toInt();
        isVfCorr1 = sett->value("general/isVfCorr1", 0).toInt();

        //comaCorr
        if(isComaCorr)
        {
            comaCorr = new comaCorrParam;
            comaCorr->cKsi = mas_to_grad((sett->value("comaCorr/cKsi", 0).toDouble()));
            comaCorr->cEta = mas_to_grad((sett->value("comaCorr/cEta", 0).toDouble()));
            comaCorr->cMag0 = (sett->value("comaCorr/cMag0", 0).toDouble());
        }

        //magEqCorr
        if(isMagEqCorr)
        {
            magEqCorr = new magEqCorrParam;
            QString dataSep = sett->value("magEqCorr/dataSep").toString();
            QString xCoefFileName = sett->value("magEqCorr/xCoefFileName").toString();
            QString yCoefFileName = sett->value("magEqCorr/yCoefFileName").toString();
            isMagEqCorr = !magEqCorr->init(xCoefFileName, yCoefFileName, dataSep);
            if(!isMagEqCorr) qDebug() << "\nisMagEqCorr init error\n";
        }

        //vfCorr0
        if(isVfCorr0)
        {
            vfCorr0 = new vfCorrParam;
            vfCorrFile = sett->value("vfCorr0/vfCorrFile", "vf.txt").toString();
            rMax = sett->value("vfCorr0/rMax", 0).toDouble();
            nmin = sett->value("vfCorr0/nmin", 0).toInt();
            isW = sett->value("vfCorr0/isW", 0).toInt();
            //isBC = sett->value("vfCorr0/isBC", 0).toInt();
            //isRadM = sett->value("vfCorr0/isRadM", 0).toInt();
            ctype = sett->value("vfCorr0/corrType", 0).toInt();
            mDeg = sett->value("vfCorr0/mDeg", 2).toInt();

            isVfCorr0 = !vfCorr0->init(vfCorrFile, rMax, nmin, ctype, mDeg, isW);
            if(!isVfCorr0) qDebug() << "\nvfCorr0 init error\n";
        }

        //vfCorr1
        if(isVfCorr1)
        {
            vfCorr1 = new vfCorrParam;
            vfCorrFile = sett->value("vfCorr1/vfCorrFile", "vf.txt").toString();
            rMax = sett->value("vfCorr1/rMax", 0).toDouble();
            nmin = sett->value("vfCorr1/nmin", 0).toInt();
            ctype = sett->value("vfCorr1/corrType", 0).toInt();
            mDeg = sett->value("vfCorr1/mDeg", 2).toInt();
            isW = sett->value("vfCorr1/isW", 0).toInt();

            isVfCorr1 = !vfCorr1->init(vfCorrFile, rMax, nmin, ctype, mDeg, isW);
            if(!isVfCorr1) qDebug() << "\nvfCorr1 init error\n";
        }

        //mdCorr
        if(isMdCorr)
        {
            mdCorr = new mdCorrParam;
            QString mdCorrX = sett->value("mdCorr/mdCorrX", "./mdCorrX.txt").toString();
            QString mdCorrY = sett->value("mdCorr/mdCorrY", "./mdCorrY.txt").toString();
            QString colSep = sett->value("mdCorr/colSep",  "|").toString();
            int corrModel = sett->value("mdCorr/corrModel", 0).toInt();
            isMdCorr = !mdCorr->init(mdCorrX, mdCorrY, colSep, corrModel);
            if(!isMdCorr) qDebug() << "\nisMdCorr init error\n";
        }

        return 0;

}

int magEqCorrParam::init(QString xCoefFileName, QString yCoefFileName, QString dataSep)
{
    qDebug() << "\nmagEqCorrParam::init\n";
    int i, j, szi;
    int dNum;
    double *x;

    QString dataStr;
    QStringList dataStrL, opers;

    QFile rFile;
    QTextStream dataStream;

    rFile.setFileName(xCoefFileName);
    if(!rFile.open(QIODevice::ReadOnly| QIODevice::Text))
    {
        qDebug() << "\nCoefX File not open\n";
        return 1;
    }
    dataStream.setDevice(&rFile);

    mCoefX.clear();
    while(!dataStream.atEnd())
    {
        dataStr = dataStream.readLine();
        opers = dataStr.split(dataSep);
        mCoefDeg = opers.size();
        x = new double[mCoefDeg];
        for(i=0; i<mCoefDeg;i++) x[i] = opers.at(i).toDouble();
        mCoefX << x;
    }
    rFile.close();

    rFile.setFileName(yCoefFileName);
    if(!rFile.open(QIODevice::ReadOnly| QIODevice::Text))
    {
        qDebug() << "\nCoefY File not open\n";
        return 1;
    }
    dataStream.setDevice(&rFile);

    mCoefY.clear();
    while(!dataStream.atEnd())
    {
        dataStr = dataStream.readLine();
        opers = dataStr.split(dataSep);
        mCoefDeg = opers.size();
        x = new double[mCoefDeg];
        for(i=0; i<mCoefDeg;i++) x[i] = opers.at(i).toDouble();
        mCoefY << x;
    }
    rFile.close();

    QStringList dsList;
    szi = mCoefX.size();
    for(i=0; i<szi; i++)
    {
        dsList.clear();
        for(j=0; j<mCoefDeg; j++) dsList << QString("%1").arg(mCoefX[i][j], 12, 'e');
        qDebug() << QString("mCoefX[%1]: %2\n").arg(i).arg(dsList.join("|"));
    }

    szi = mCoefY.size();
    for(i=0; i<szi; i++)
    {
        dsList.clear();
        for(j=0; j<mCoefDeg; j++) dsList << QString("%1").arg(mCoefY[i][j], 12, 'e');
        qDebug() << QString("mCoefY[%1]: %2\n").arg(i).arg(dsList.join("|"));
    }

    return 0;

}
/*
void fitsdata::initComaCorr(double cKsi, double cEta, double cMag0)
{
    comaKsi = cKsi;
    comaEta = cEta;
    comaMag0 = cMag0;

    isComaCorr = 1;
}
*/
int vfCorrParam::init(QString vfFileName, double rmax, int nmin, int ctype, int mdeg, int isw)
{
    vectFcorr = new vectGrid3D;
    QByteArray tmp = vfFileName.toAscii();
    if(vectFcorr->init(tmp.data())) return 1;
    vectFcorr->initVF(rmax);
    corrType = ctype;
    rMax = rmax;
    nMin = nmin;
    mDeg = mdeg;
    isW = isw;
    qDebug() << QString("isW: %1\n").arg(isW);

    return 0;
}

int mdCorrParam::init(QString mdcorrx, QString mdcorry, QString colsep, int corrmodel)
{
//    mdcParam = new mdCorr;
    colSep = QString(colsep);
    corrModel = corrmodel;
    mdCorrX = QString(mdcorrx);
    mdCorrY = QString(mdcorry);

    if(initMdCoef(mdCorrX, &mdCoefX, &mdCoefXsz, colSep)) return 1;
    if(initMdCoef(mdCorrY, &mdCoefY, &mdCoefYsz, colSep)) return 1;

    dCoefXsz = mdCoefX.size();
    dCoefYsz = mdCoefY.size();

    return 0;
}

double comaCorrParam::dKsi(double ksi, double mag)
{
    return(cKsi*ksi*(mag-cMag0));
}

double comaCorrParam::dEta(double eta, double mag)
{
    return(cEta*eta*(mag-cMag0));
}

int vfCorrParam::detCorr(double *dx, double *dy, long *ni, double x, double y, double magn)
{
    double dX, dY;
    long nI;
    int res;

    qDebug() << QString("corrType: %1\n").arg(corrType);
    qDebug() << QString("isW: %1\n").arg(isW);


    switch(corrType)
    {
    case 0:
        res = vectFcorr->int2D(x, y, magn, &dX, &dY, &nI);
        *ni = nI;
        break;
    case 1:
        res = vectFcorr->int2Drad(x, y,magn, &dX, &dY, rMax, nMin);
        *ni = 20;
        break;
    case 2:
        res = vectFcorr->intIDW(x, y, magn, &dX, &dY);
        *ni = 20;
        break;
    case 3:
        res = vectFcorr->int2DradM(x, y,magn, &dX, &dY, rMax, nMin, mDeg, isW);
        *ni = 20;
        break;
    case 4:
        res = vectFcorr->intIDWM(x, y,magn, &dX, &dY, mDeg, isW);
        *ni = 20;
        break;

    }
    qDebug() << QString("res: %1\n").arg(res);
    if(res)
    {
        *dx = 0.0;
        *dy = 0.0;
        qDebug() << QString("vfCorrParam::detCorr: error\n");
        return 1;
    }

    *dx = dX;
    *dy = dY;


    /*


    if(isBC)
    {
        //if(!vectFcorr->int2Drad(mT->sdata[4], mT->sdata[5], mT->mEkv[2], &dX, &dY, vfRmax, nMin))
        if(!vectFcorr->int2D(x, y, magn, &dX, &dY, &nI))
        {
            *dx = dX;
            *dy = dY;
            *ni = nI;

//                mT->objdata[4]-= dX;
//                mT->objdata[5]-= dY;
        }
        else
        {
            *dx = 0.0;
            *dy = 0.0;
            *ni = nI;
            return 1;
        }
    }
    else
    {
        if(isRadM)
        {
            if(!vectFcorr->int2DradM(x, y,magn, &dX, &dY, rMax, nMin))
            {
                *dx = dX;
                *dy = dY;
                qDebug() << QString("int2DradM: %1\t%2\n").arg(dX).arg(dY);
    //                mT->objdata[4]-= dX;
    //                mT->objdata[5]-= dY;
            }
            else
            {
                *dx = 0.0;
                *dy = 0.0;
                qDebug() << QString("int2DradM: error\n");
                return 1;
            }
        }
        else
        {
            if(!vectFcorr->int2Drad(x, y,magn, &dX, &dY, rMax, nMin))
            {
                *dx = dX;
                *dy = dY;
                //qDebug() << QString("int2Drad: %1\t%2\n").arg(dX).arg(dY);
    //                mT->objdata[4]-= dX;
    //                mT->objdata[5]-= dY;
            }
            else
            {
                *dx = 0.0;
                *dy = 0.0;
                //qDebug() << QString("int2Drad: error\n");
                return 1;
            }
        }
    }
    */


    return 0;
}

double magEqCorrParam::corrX(double x, double magn)
{
    return(magEqCorr(magn, x, mCoefX, mCoefDeg));
}

double magEqCorrParam::corrY(double y, double magn)
{
    return(magEqCorr(magn, y, mCoefY, mCoefDeg));
}

////////////////////////////////////////////////////////////////
