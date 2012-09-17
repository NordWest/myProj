#include "redStat.h"

#define REDSTAT_LOG_LEVEL 0

void detOrSeriesList(QList <objResRec*> orList, QList <objResidualFile*> *orSeriesList, double expMax, double expMin)
{
    int i, szi;
    double dT0, dT1, expTemp, mjd0, mjd1;
    if(orList.size()==0) return;
    expMax = expMax/1440.0;

    orSeriesList->clear();

    sortORList(&orList);

    szi = orList.size();
    objResRec* tResRec;
    objResidualFile* tResFile;
    tResFile = new objResidualFile;
    //orSeriesList->append(tResFile);
    tResRec = orList.at(0);
    mjd1 = tResRec->mJD;
    tResFile->ocList << tResRec;
    expTemp = 0.0;
    dT1 = 0.0;
    for(i=1; i<szi; i++)
    {
      mjd0 = mjd1;
      dT0 = dT1;


        tResRec = orList.at(i);
        mjd1 = tResRec->mJD;
        dT1 = mjd1 - mjd0;
        expTemp+=dT1;
        //if((tResFile->ocList.size()<=1)||((fabs(dT1-dT0)<1.5*dT0)&&((expTemp<expMax)||(expMax<=0))))
        //if((tResFile->ocList.size()<=1)||((fabs(dT1-dT0)<dExpCoef*dT0)&&((expTemp<expMax)||(expMax<=0))))
        if((expTemp>expMax)&&(expMax>0))
        {
            if(expTemp<expMin)
            {
                tResFile->ocList.clear();
            }
            else
            {
                orSeriesList->append(tResFile);
                tResFile = new objResidualFile;
            }
            //mjd1 = tResRec->mJD;
            tResFile->ocList << tResRec;
            expTemp = 0.0;
            dT1 = 0.0;
        }
        else tResFile->ocList << tResRec;


    }

}

void detErrBSeriesList(QList <errBudgetRec*> ebList, QList <errBudgetFile*> *ebSeriesList, double expMax)
{
    int i, szi;
    double dT0, dT1, expTemp, mjd0, mjd1;
    if(ebList.size()==0) return;
    expMax = expMax/1440.0;

    sortErrBList(&ebList);

    szi = ebList.size();
    errBudgetRec* tResRec;
    errBudgetFile* tResFile;
    tResFile = new errBudgetFile;
    ebSeriesList->append(tResFile);
    tResRec = ebList.at(0);
    mjd1 = tResRec->MJD;
    tResFile->errList << tResRec;
    expTemp = 0.0;
    dT1 = 0.0;
    for(i=1; i<szi; i++)
    {
      mjd0 = mjd1;
      dT0 = dT1;


        tResRec = ebList.at(i);
        mjd1 = tResRec->MJD;
        dT1 = mjd1 - mjd0;
        expTemp+=dT1;
        //if((tResFile->ocList.size()<=1)||((fabs(dT1-dT0)<1.5*dT0)&&((expTemp<expMax)||(expMax<=0))))
        //if((tResFile->ocList.size()<=1)||((fabs(dT1-dT0)<dExpCoef*dT0)&&((expTemp<expMax)||(expMax<=0))))
        if((expTemp>expMax)&&(expMax>0))
        {
            tResFile = new errBudgetFile;
            ebSeriesList->append(tResFile);
            mjd1 = tResRec->MJD;
            tResFile->errList << tResRec;
            expTemp = 0.0;
            dT1 = 0.0;
        }
        else tResFile->errList << tResRec;


    }

}

void sortORList(QList <objResRec*> *orList, int dir)
{
    int i, j, sz;
    sz = orList->size();
    double val;
    int vPos;

    int sn = pow(-1, dir);

    for(i=0; i<sz-1; i++)
    {
        val = orList->at(i)->mJD;
        vPos = i;
        for(j=i+1; j<sz; j++)
        {
            if(sn*val>sn*orList->at(j)->mJD)
            {
                val = orList->at(j)->mJD;
                vPos = j;
            }

        }
        orList->swap(i, vPos);
    }
}

void sortErrBList(QList <errBudgetRec*> *ebList, int dir)
{
    int i, j, sz;
    sz = ebList->size();
    double val;
    int vPos;

    int sn = pow(-1, dir);

    for(i=0; i<sz-1; i++)
    {
        val = ebList->at(i)->MJD;
        vPos = i;
        for(j=i+1; j<sz; j++)
        {
            if(sn*val>sn*ebList->at(j)->MJD)
            {
                val = ebList->at(j)->MJD;
                vPos = j;
            }

        }
        ebList->swap(i, vPos);
    }
}

void detPlateName(QString *plateName, QString originName, int plNameType)
{
    plateName->clear();

    QString pathSep = QString("\\");
    if(originName.indexOf(pathSep)==-1)pathSep = QString("/");


    switch(plNameType)
    {
    case 0:
       plateName->append(originName.section(pathSep, -2, -2));
       break;
    case 1:
       plateName->append(originName.section(pathSep, -1, -1).section(".", 0, 0));
       //plateName = plateName.right(plateName.size()-2);
       break;
    case 2:
       plateName->append(originName);
       break;
    }

    //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("originName: %1\tplateName: %2\n").arg(originName).arg(plateName->toAscii().data());
}

void detVersName(QString *versName, QString originName)
{
    *versName = originName.section(".", -1, -1);
    if(*versName=="aver") *versName = originName.section(".", -2, -2);
}

void detTimeCode(QString &timeCode)
{
    QDateTime cDT;
    cDT = QDateTime::currentDateTime();
    timeCode = cDT.toString("yyyyMMddhhmmsszzz");
}

int doSigmaMul(QList <double> valueList, double sigmaMul, double proofP, double *mean, double *rmsOne, double *rmsMean, int *numEnd)
{
    double maxVal;
    int num0, num1, i, sz;

    num1 = valueList.size();

    do
    {
            num0 = num1;

            if(num0<3) return 1;

            *rmsMean = *rmsOne = *mean = 0;

            for(i=0; i<num0; i++) *mean += valueList[i];
            *mean /= num0;

            for(i=0; i<num0; i++) *rmsOne += pow(valueList[i]-*mean, 2.0);
            *rmsOne = sqrt(*rmsOne/(num0-1));
            *rmsMean = *rmsOne/sqrt(num0);

            maxVal = sigmaMul*(*rmsOne);
            //qDebug() << QString("%1|%2|%3|%4\n").arg(*mean).arg(*rmsOne).arg(*rmsMean).arg(maxVal);

            for(i=num0-1; i>=0;i--)
            {
                if(fabs(*mean-valueList[i])>maxVal) valueList.removeAt(i);
            }

            num1 = valueList.size();

    }while(abs(num0-num1)>(proofP*num1));

    *numEnd = num1;

    //qDebug() << QString("%1|%2|%3|%4\n").arg(*mean).arg(*rmsOne).arg(*rmsMean).arg(maxVal);

    return 0;

}

int doWeghts(QList <double> valList, QList <double> weList, double *mean, double *rmsOne, double *rmsMean)
{
    double maxVal;
    int num0, num1, i, sz;

    num0 = valList.size();
    if(num0!=weList.size()) return 1;

    double p=0;
    for(i=0;i<num0;i++) p+=weList[i];
   /*
    double xav0=0;

    for(i=0;i<num0;i++)
    {
        xav0 += weList[i]*valList[i];
    }
    xav0 /=num0;
*/
    double xav=0;

    for(i=0;i<num0;i++)
    {
        xav += weList[i]*valList[i];
    }
    xav = xav/p;

    double sig=0;
    for(i=0;i<num0;i++)
    {
        sig += weList[i]*pow(valList[i]-xav, 2.0);
    }
    sig = sqrt(sig/(num0-1));

    *mean = xav;
    *rmsOne = sig;
    *rmsMean = sig/sqrt(num0);

    return 0;

}




ocRec::ocRec()
{
        MJday = ra = de = ocRaCosDe = ocDe = ocMag = topDist = muRaCosDe = muDe = Vr = 0.0;
	mag0 = phase = elong = expTime = 0.0;
	catNum = -1;
	vers = 0;
	name.clear();
       /* x = 0;
        y = 0;
        pixmag = 0;
        Dx = 0;
        Dy = 0;
        Dpixmag = 0;*/
}

ocRec::~ocRec()
{
        MJday = ra = de = ocRaCosDe = ocDe = ocMag = topDist = muRaCosDe = muDe = Vr = 0.0;
	mag0 = phase = elong = expTime = 0.0;
	catNum = -1;
	vers = -1;
	name.clear();
       /* x = 0;
        y = 0;
        pixmag = 0;
        Dx = 0;
        Dy = 0;
        Dpixmag = 0;*/
}

ocRec::ocRec(QString str)
{
	ocRec();
	s2rec(str);
};

void ocRec::rec2s(QString *str)
{
    rec2sSeries(str);

    //str->clear();

//	DATEOBS dobs = getDATEOBSfromMJD(MJday);
        /*
        str->insert(0, getStrFromDATEOBS(getDATEOBSfromMJD(MJday), " ", 0, 5) + "|" + deg_to_hms(ra, " ", 4) + "|"+ deg_to_damas(de, " ", 3) + "|" + QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11|%12|%13|%14|%15|%16|%17").arg(mag0, 6, 'f', 3, QLatin1Char(' ')).arg(ocRaCosDe, 8, 'f', 1, QLatin1Char(' ')).arg(ocDe, 8, 'f', 1, QLatin1Char(' ')).arg(ocMag, 8, 'f', 1, QLatin1Char(' ')).arg(x, 10, 'f', 4, QLatin1Char(' ')).arg(y, 10, 'f', 4, QLatin1Char(' ')).arg(pixmag, 15, 'e', 6).arg(Dx, 11, 'g', 7).arg(Dy, 11, 'g', 7).arg(Dpixmag, 15, 'g', 7).arg(topDist, 12, 'f', 9, QLatin1Char(' ')).arg(muRaCosDe, 9, 'f', 2, QLatin1Char(' ')).arg(muDe, 9, 'f', 2, QLatin1Char(' ')).arg(Vr, 6, 'f', 2, QLatin1Char(' ')).arg(phase, 8, 'f', 3, QLatin1Char(' ')).arg(elong, 8, 'f', 3, QLatin1Char(' ')).arg(expTime, 8, 'f', 3, QLatin1Char(' ') )+QString("|%1|%2|%3|%4").arg(name, 16).arg(catName, 8).arg(catMagName, 8).arg(mesureTimeCode));
*/
};

void ocRec::rec2sSeries(QString *str)
{
        str->clear();
//	DATEOBS dobs = getDATEOBSfromMJD(MJday);

        str->insert(0, getStrFromDATEOBS(getDATEOBSfromMJD(MJday), " ", 0, 5) + "|" + deg_to_hms(ra, " ", 4) + "|"+ deg_to_damas(de, " ", 3) + "|" + QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11").arg(mag0, 6, 'f', 3, QLatin1Char(' ')).arg(ocRaCosDe, 8, 'f', 1, QLatin1Char(' ')).arg(ocDe, 8, 'f', 1, QLatin1Char(' ')).arg(ocMag, 8, 'f', 1, QLatin1Char(' ')).arg(topDist, 12, 'f', 9, QLatin1Char(' ')).arg(muRaCosDe, 9, 'f', 2, QLatin1Char(' ')).arg(muDe, 9, 'f', 2, QLatin1Char(' ')).arg(Vr, 6, 'f', 2, QLatin1Char(' ')).arg(phase, 8, 'f', 3, QLatin1Char(' ')).arg(elong, 8, 'f', 3, QLatin1Char(' ')).arg(expTime, 8, 'f', 3, QLatin1Char(' ') )+QString("|%1|%2|%3|%4").arg(name, 16).arg(catName, 8).arg(catMagName, 8).arg(mesureTimeCode));

};

void ocRec::rec2sBase(QString *recStr)
{
    recStr->clear();
//	DATEOBS dobs = getDATEOBSfromMJD(MJday);

    recStr->insert(0, QString("%1 %2 ").arg(name, 16).arg(mjd2jd(MJday), 13, 'f', 5) + getStrFromDATEOBS(getDATEOBSfromMJD(MJday), " ", 0, 6) + " " + deg_to_hms(ra, " ", 3) + " "+ deg_to_damas(de, " ", 2) + " " + QString("%1 %2").arg(ocRaCosDe, 8, 'f', 1, QLatin1Char(' ')).arg(ocDe, 8, 'f', 1, QLatin1Char(' ')));//+QString(" %1").arg(catName, 8));
}

void ocRec::rec2MPC(QString *str, QString obsName, QString objNum, int provNum, QString objType)
{
    //int obj_num;
    str->clear();
    QString discAster(" ");
    QString note1(" ");
    QString outstr;
    DATEOBS dateObs = getDATEOBSfromMJD(MJday);
    //obj_num = name.toInt();
    if(!provNum) outstr = outstr +QString( "%1       " ).arg(objNum,5,QLatin1Char( '0' ));
    else outstr = outstr +QString( "     %1" ).arg(objNum,7,QLatin1Char( '0' ));
    outstr += discAster;
    outstr += note1;
    outstr += objType;
    outstr = outstr +getStrFromDATEOBS(dateObs," ", 0, 6);
    outstr = outstr +deg_to_hms(ra, " ",3);
    outstr = outstr + deg_to_damas(de, " ",2)+"         ";
    outstr = outstr  +QString( "%1" ).arg(mag0,5,'f',2);
    outstr = outstr  +"       "+obsName;
    str->append(outstr);
    /*msgstr = QString( "%1" ).arg(mObj->mpcObj->name);
    //msgstr = msgstr.trimmed();
    //QMessageBox::information(0,"debug",outfolder+"/"+msgstr+"_mpc.txt",QMessageBox::Ok,0,0);
    //QFile mpcFile(outfolder+"/"+msgstr+"_mpc.txt");
    //mpcFile.open(QIODevice::Append| QIODevice::Text);
    //QTextStream mpcStream;
    mpcStream.setDevice(&mpcFile);
    mpcStream << outstr << "\n";
    mpcFile.close();*/
}

void ocRec::s2rec(QString str)
{
//	//if(REDSTAT_LOG_LEVEL>0) qDebug() << "\n" << str << "\n";
	QStringList sL = str.split("|");
//	//if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nsize=" << sL.size() << "\n";
        /*if(sL.size()!=16&&sL.size()!=18) return;
	if(sL.size()==16)
	{
		vers = 0;
	}
        else if(sL.size()==18){vers = 1;}*/
        int szl = sL.size();
        int k;
	
	QString tstr = sL[0];
	
        MJday = getMJDfromYMD(tstr);
        ra = hms_to_deg(sL[1], " ");
        de = damas_to_deg(sL[2], " ");
	mag0 = sL[3].toFloat(); 
        ocRaCosDe = sL[4].toDouble();
	ocDe = sL[5].toDouble();
	ocMag = sL[6].toDouble();

        k=7;
        /*
        if(szl==24)
        {
            x = sL[k++].toDouble();
            y = sL[k++].toDouble();
            pixmag = sL[k++].toDouble();
            Dx = sL[k++].toDouble();
            Dy = sL[k++].toDouble();
            Dpixmag = sL[k++].toDouble();
        }
*/
/*	if(vers==0)
        {*/
                topDist = sL[k++].toDouble();
                muRaCosDe = sL[k++].toDouble();
                muDe = sL[k++].toDouble();
                Vr = sL[k++].toDouble();
                phase = sL[k++].toFloat();
                elong = sL[k++].toFloat();
                expTime = sL[k++].toDouble();
                name = sL[k++];
                catName = sL[k++];
                catMagName = sL[k++];
                mesureTimeCode = sL.at(k++);
//		expTime = sL[15].toFloat();
/*	}
	if(vers==1)
	{
		ra_oc = hms_to_mas(sL[7], " ");
		dec_oc = damas_to_mas(sL[8], " ");
		topDist = sL[9].toDouble();
                muRaCosDe = sL[10].toDouble();
		muDe = sL[11].toDouble();
		Vr = sL[12].toDouble();
		phase = sL[13].toFloat();
		elong = sL[14].toFloat();
		name = sL[15];
		catNum = sL[16].toInt();
		expTime = sL[17].toFloat();
        }*/
};

void ocRec::copy(const ocRec &source)
{
    MJday = source.MJday;
    ra = source.ra;
    de = source.de;
    mag0 = source.mag0;
    ocRaCosDe = source.ocRaCosDe;
    ocDe = source.ocDe;
    ocMag = source.ocMag;
 /*   x = source.x;
    y = source.y;
    pixmag = source.pixmag;
    Dx = source.Dx;
    Dy = source.Dy;
    Dpixmag = source.Dpixmag;*/
   // ra_oc = source.ra_oc;
   // dec_oc = source.dec_oc;
    topDist = source.topDist;
    muRaCosDe = source.muRaCosDe;
    muDe = source.muDe;
    Vr = source.Vr;
    phase = source.phase;
    elong = source.elong;
    expTime = source.expTime;
    name = source.name;
    catName = source.catName;
    catMagName = source.catMagName;
    mesureTimeCode = source.mesureTimeCode;
}

ocRec& ocRec::operator=(const ocRec &source)
{
    copy(source);
 /*   MJday = source.MJday;
    ra = source.ra;
    de = source.de;
    mag0 = source.mag0;
    ocRaCosDe = source.ocRaCosDe;
    ocDe = source.ocDe;
    ocMag = source.ocMag;
    ra_oc = source.ra_oc;
    dec_oc = source.dec_oc;
    topDist = source.topDist;
    muRaCosDe = source.muRaCosDe;
    muDe = source.muDe;
    Vr = source.Vr;
    phase = source.phase;
    elong = source.elong;
    name = source.name;
    catNum = source.catNum;
    expTime = source.expTime;*/
    return *this;
}

void ocRec::fromMpeph(mpephRec *meRec)
{
    //MJday = meRec->
}

/////////////////////////////////////////////////////////////////////

colRec::colRec()
{
	colNum = num = 0;
        mean = rmsMean = rmsOne = 0.0;
};

colRec::colRec(QString str)
{
	colRec();
	s2rec(str);
}
	
void colRec::rec2s(QString *str)
{
	str->clear();
//	str->insert(0, "%" + QString("%1|%2|%3|%4|%5").arg(colNum, 3, 10, QLatin1Char( ' ' )).arg(mean, 10, 'f', 3, QLatin1Char(' ')).arg(rmsMean, 10, 'f', 3, QLatin1Char(' ')).arg(rms, 10, 'f', 3, QLatin1Char(' ')).arg(num, 5, 10, QLatin1Char( ' ' )));
        str->insert(0, "%" + QString("%1|%2|%3|%4|%5").arg(colNum, 3, 10, QLatin1Char( ' ' )).arg(mean,20,'g',13).arg(rmsMean,20,'g',13).arg(rmsOne,20,'g',13).arg(num, 5, 10, QLatin1Char( ' ' )));
//	str->insert(0, "%" + QString("%1|%2|%3|%4|%5").arg(colNum, 3, 10, QLatin1Char( ' ' )).arg(mean, 20).arg(rmsMean, 20).arg(rms, 20).arg(num, 5, 10, QLatin1Char( ' ' )));
};

void colRec::s2rec(QString str)
{
//	//if(REDSTAT_LOG_LEVEL>0) qDebug() << "\n" << str << "\n";
	QStringList sL = str.split("|");
//	//if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nsize=" << sL.size() << "\n";
	if(sL.size()!=5) return;
//	//if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nsL0\t" << sL[0] << "\n";
	QString tstr = sL[0].mid(1);
	colNum = tstr.toInt();
	mean = sL[1].toDouble();
        rmsMean = sL[2].toDouble();
        rmsOne = sL[3].toDouble();
	num = sL[4].toInt();	
};

colRec& colRec::operator=(const colRec &source)
{
//        colRec *resR = new colRec;
        this->colNum = source.colNum;
        this->num = source.num;
        this->mean = source.mean;
        this->rmsMean = source.rmsMean;
        return *this;
}

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////

moveModelRec::moveModelRec()
{
        nPosX = nPosY = -1;
	Tm = xTm = yTm = uweX = uweY = rmsX = rmsY = Xdot = rmsXdot = XdotOC = Ydot = rmsYdot = YdotOC = 0.0;
        fp = 0;
        ft = 0;
        vflag = 1;
        ndig = 3;

}

moveModelRec::moveModelRec(QString str)
{
	moveModelRec();
	s2rec(str);
}


	
void moveModelRec::rec2s(QString *str)
{
	str->clear();
        QString timeLine, xStr, yStr;
        switch(ft)
        {
            case 0:{timeLine=getStrFromDATEOBS(getDATEOBSfromMJD(Tm), " ", 0, ndig);break;}
        }
        switch(fp)
        {
            case 0://positions in RA and DEC
            {
                xStr = deg_to_hms(xTm, " ", 5);
                yStr = deg_to_damas(yTm, " ", 5);
                break;
            }
            case 1://relative positions in arcsec //РњР•Р”РќР”Р•РљР®РњРќ
            {
                xStr = mas_to_hms(xTm, " ", 5);
                yStr = mas_to_damas(yTm, " ", 5);
                break;
            }
        }

	//Tm|xTm|yTm|nPos|uweX|uweY|rmsX|rmsY|Xdot|rmsXdot|XdotOC|Ydot|rmsYdot|YdotOC
//	str->insert(0, "#" + timeLine + "|" + mas_to_hms(xTm, " ", ndig) + "|"+ mas_to_damas(yTm, " ", 3) + "|" + QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11").arg(nPos, 3, 10, QLatin1Char(' ')).arg(uweX, 6, 'f', 1, QLatin1Char(' ')).arg(uweY, 6, 'f', 1, QLatin1Char(' ')).arg(rmsX, 6, 'f', 1, QLatin1Char(' ')).arg(rmsY, 6, 'f', 1, QLatin1Char(' ')).arg(Xdot, 10, 'f', 2, QLatin1Char(' ')).arg(rmsXdot, 7, 'f', 2, QLatin1Char(' ')).arg(XdotOC, 7, 'f', 2, QLatin1Char(' ')).arg(Ydot, 10, 'f', 2, QLatin1Char(' ')).arg(rmsYdot, 7, 'f', 2, QLatin1Char(' ')).arg(YdotOC, 7, 'f', 2, QLatin1Char(' ')));
        //Tm|xTm|yTm|nPosX|uweX|rmsX|Xdot|rmsXdot|XdotOC|nPosY|uweY|rmsY|Ydot|rmsYdot|YdotOC
        str->insert(0, "#" + timeLine + "|" + xStr + "|"+ yStr + "|" + QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11|%12").arg(nPosX, 3, 10, QLatin1Char(' ')).arg(uweX, 6, 'f', 1, QLatin1Char(' ')).arg(rmsX, 6, 'f', 1, QLatin1Char(' ')).arg(Xdot, 10, 'f', 2, QLatin1Char(' ')).arg(rmsXdot, 7, 'f', 2, QLatin1Char(' ')).arg(XdotOC, 7, 'f', 2, QLatin1Char(' ')).arg(nPosY, 3, 10, QLatin1Char(' ')).arg(uweY, 6, 'f', 1, QLatin1Char(' ')).arg(rmsY, 6, 'f', 1, QLatin1Char(' ')).arg(Ydot, 10, 'f', 2, QLatin1Char(' ')).arg(rmsYdot, 7, 'f', 2, QLatin1Char(' ')).arg(YdotOC, 7, 'f', 2, QLatin1Char(' ')));
}

void moveModelRec::s2rec(QString str)
{
//	//if(REDSTAT_LOG_LEVEL>0) qDebug() << "\n" << str << "\n";
	QStringList sL = str.split("|");
//	//if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nsize=" << sL.size() << "\n";
        if(sL.size()!=15) return;
	
	
	QString tstr = sL[0];

        switch(ft)
        {
            case 0:
            {
                Tm = getMJDfromYMD(tstr.mid(1));
                break;
            }
        }
        switch(fp)
        {
            case 0:
            {
                xTm = hms_to_mas(sL[1], " ");
                yTm = damas_to_mas(sL[2], " ");
                break;
            }
        }
	
        nPosX = sL[3].toInt();
        uweX = sL[4].toDouble();
        rmsX = sL[5].toDouble();
        Xdot = sL[6].toDouble();
        rmsXdot = sL[7].toDouble();
        XdotOC = sL[8].toDouble();
        nPosY = sL[9].toInt();
        uweY = sL[10].toDouble();
        rmsY = sL[12].toDouble();
        Ydot = sL[12].toDouble();
        rmsYdot = sL[13].toDouble();
        YdotOC = sL[14].toDouble();

}



/////////////////////////////////////////////////////////////////////

residualsRec::residualsRec()
{
        mJD = ra = de = ksi = eta = mag = ksiOC = etaOC = magOC = x = y = pixmag = Dx = Dy = Dpixmag = 0.0;
        isRefKsi = isRefEta = isRefMag = 0;
        catName = QString("noname");
        catMagName = QString("noname");
        mesureTimeCode = QString("YYYYMMDDHHMMSSSS");
        mesParams = QString("");
};

residualsRec::residualsRec(QString str)
{
	residualsRec();
	s2rec(str);
};
	
void residualsRec::rec2s(QString *str)
{
	str->clear();
        str->insert(0, QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11|%12|%13|%14|%15|%16|%17|%18").arg(mJD, 15, 'f', 7, QLatin1Char(' ')).arg(ra, 13, 'f', 8, QLatin1Char(' ')).arg(de, 13, 'f', 8).arg(ksi, 15, 'f', 12, QLatin1Char(' ')).arg(eta, 15, 'f', 12, QLatin1Char(' ')).arg(mag, 5, 'f', 2, QLatin1Char(' ')).arg(ksiOC, 7, 'f', 1, QLatin1Char(' ')).arg(etaOC, 7, 'f', 1, QLatin1Char(' ')).arg(magOC, 5, 'f', 2, QLatin1Char(' ')).arg(x, 10, 'f', 4, QLatin1Char(' ')).arg(y, 10, 'f', 4, QLatin1Char(' ')).arg(pixmag, 16, 'e', 8, QLatin1Char(' ')).arg(Dx, 16, 'e', 8, QLatin1Char(' ')).arg(Dy, 16, 'e', 8, QLatin1Char(' ')).arg(Dpixmag, 16, 'e', 8, QLatin1Char(' ')).arg(isRefKsi, 1).arg(isRefEta, 1).arg(isRefMag, 1));

        str->append(QString("|%1|%2|%3").arg(catName, 8).arg(catMagName, 8).arg(mesureTimeCode));
        //str->append(QString("|%1").arg(catName).arg(catMagName));
       // str->append(QString("|%1").arg(exptime, 5));
        str->append(QString("#%1").arg(mesParams));
};

void residualsRec::s2rec(QString str)
{
    QStringList parts = str.split("#");
    QStringList sL = str.section("#", 0, 0).split("|");
    if(sL.size()<21) return;

    mJD = sL[0].toDouble();
    ra = sL[1].toDouble();
    de = sL[2].toDouble();
    ksi = sL[3].toDouble();
    eta = sL[4].toDouble();
    mag = sL[5].toDouble();
    ksiOC = sL[6].toDouble();
    etaOC = sL[7].toDouble();
    magOC = sL[8].toDouble();
    x = sL[9].toDouble();
    y = sL[10].toDouble();
    pixmag = sL[11].toDouble();
    Dx = sL[12].toDouble();
    Dy = sL[13].toDouble();

    Dpixmag = sL[14].toDouble();
    isRefKsi = sL[15].toInt();
    isRefEta = sL[16].toInt();
    isRefMag = sL[17].toInt();

    catName = sL[18];
    catMagName = sL[19];

    mesureTimeCode = sL.at(20);
    mesParams = str.section("#", 1, 1);
};

void residualsRec::copy(const residualsRec &source)
{
    mJD = source.mJD;
    ra = source.ra;
    de = source.de;
    ksi = source.ksi;
    eta = source.eta;
    mag = source.mag;
    ksiOC = source.ksiOC;
    etaOC = source.etaOC;
    magOC = source.magOC;
    x = source.x;
    y = source.y;
    pixmag = source.pixmag;
    Dx = source.Dx;
    Dy = source.Dy;
    Dpixmag = source.Dpixmag;
    isRefKsi = source.isRefKsi;
    isRefEta = source.isRefEta;
    isRefMag = source.isRefMag;
    catName = source.catName;
    catMagName = source.catMagName;
    mesureTimeCode = source.mesureTimeCode;
    mesParams = source.mesParams;
}

void residualsRec::setMesParams(double *P, int pSize)
{
    mesParams.clear();
    QStringList mpList;
    for(int i=0; i<pSize; i++)
    {
        mpList << QString("%1").arg(P[i], 18, 'e');
    }
    mesParams = mpList.join("|");
}

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////

sstarRes::sstarRes()
{
        mJD = ra = de = ksi = eta = mag = ksiOC = etaOC = magOC = x = y = pixmag = Dx = Dy = Dpixmag = exptime = 0.0;
        lspmName = QString("noname");
        catName = QString("noname");
        catMagName = QString("noname");
        mesureTimeCode = QString("YYYYMMDDHHMMSSSS");
};

sstarRes::sstarRes(QString str)
{
        sstarRes();
        s2rec(str);
};

sstarRes::~sstarRes()
{

}

void sstarRes::rec2s(QString *str)
{
        str->clear();
        str->insert(0, QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11|%12|%13|%14|").arg(mJD, 15, 'f', 7, QLatin1Char(' ')).arg(ra, 13, 'f', 8, QLatin1Char(' ')).arg(de, 13, 'f', 8).arg(ksi, 13, 'f', 9, QLatin1Char(' ')).arg(eta, 13, 'f', 9, QLatin1Char(' ')).arg(mag, 6, 'f', 2, QLatin1Char(' ')).arg(ksiOC, 7, 'f', 1, QLatin1Char(' ')).arg(etaOC, 7, 'f', 1, QLatin1Char(' ')).arg(magOC, 6, 'f', 2, QLatin1Char(' ')).arg(x, 10, 'f', 4, QLatin1Char(' ')).arg(y, 10, 'f', 4, QLatin1Char(' ')).arg(pixmag, 11, 'f', 6, QLatin1Char(' ')).arg(Dx, 7, 'f', 1, QLatin1Char(' ')).arg(Dy, 7, 'f', 1, QLatin1Char(' ')));
        str->append(QString("%1|%2|%3|%4").arg(Dpixmag, 10, 'f', 6, QLatin1Char(' ')).arg(lspmName, 8).arg(catName, 8).arg(catMagName, 8));
        str->append(QString("|%1").arg(mesureTimeCode));
};

int sstarRes::s2rec(QString str)
{
//	//if(REDSTAT_LOG_LEVEL>0) qDebug() << "\n" << str << "\n";
        QStringList sL = str.split("|");
        //if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nsize=" << sL.size() << "\n";
        if(sL.size()!=19) return 1;

        mJD = sL[0].toDouble();

        ra = sL[1].toDouble();
        de = sL[2].toDouble();

        ksi = sL[3].toDouble();
        eta = sL[4].toDouble();
        mag = sL[5].toDouble();
        ksiOC = sL[6].toDouble();
        etaOC = sL[7].toDouble();
        magOC = sL[8].toDouble();
        x = sL[9].toDouble();
        y = sL[10].toDouble();
        pixmag = sL[11].toDouble();
        Dx = sL[12].toDouble();
        Dy = sL[13].toDouble();

        Dpixmag = sL[14].toDouble();
        //OCmag = sL[15].toDouble();
        lspmName = sL[15];
        catName = sL[16];
        catMagName = sL[17];

        //exptime = sL[18].toDouble();
        mesureTimeCode = sL.at(18);
        return 0;
};

void sstarRes::copy(const sstarRes &source)
{
    mJD = source.mJD;

    ra = source.ra;
    de = source.de;

    ksi = source.ksi;
    eta = source.eta;
    mag = source.mag;
    ksiOC = source.ksiOC;
    etaOC = source.etaOC;
    magOC = source.magOC;
    x = source.x;
    y = source.y;
    pixmag = source.pixmag;
    Dx = source.Dx;
    Dy = source.Dy;

    Dpixmag = source.Dpixmag;
    //OCmag = source.OCmag;
    lspmName = source.lspmName;
    catName = source.catName;
    catMagName = source.catMagName;
    exptime = source.exptime;
    mesureTimeCode = source.mesureTimeCode;
}

///////////////////////////////////////////////////////////

objResRec::objResRec()
{
        mJD = ra = de = ksi = eta = mag = ksiOC = etaOC = magOC = x = y = pixmag = Dx = Dy = Dpixmag = 0.0;
        name = QString("");
        catName = QString("");
        catMagName = QString("");
        mesureTimeCode = QString("");
        mesParams = QString("");
};

objResRec::objResRec(QString str)
{
        objResRec();
        s2rec(str);
};

objResRec::~objResRec()
{

}

void objResRec::clear()
{
        mJD = ra = de = ksi = eta = mag = ksiOC = etaOC = magOC = x = y = pixmag = Dx = Dy = Dpixmag = 0.0;
        name = QString("");
        catName = QString("");
        catMagName = QString("");
        mesureTimeCode = QString("");
        mesParams = QString("");
}

void objResRec::copy(const objResRec &source)
{
    mJD = source.mJD;

    ra = source.ra;
    de = source.de;

    ksi = source.ksi;
    eta = source.eta;
    mag = source.mag;
    ksiOC = source.ksiOC;
    etaOC = source.etaOC;
    magOC = source.magOC;
    x = source.x;
    y = source.y;
    pixmag = source.pixmag;
    Dx = source.Dx;
    Dy = source.Dy;

    Dpixmag = source.Dpixmag;
    //OCmag = source.OCmag;
    name = source.name;
    catName = source.catName;
    catMagName = source.catMagName;
    //exptime = source.exptime;
    mesureTimeCode = source.mesureTimeCode;
    mesParams = source.mesParams;
}

void objResRec::rec2s(QString *str)
{
        str->clear();
        str->insert(0, QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11|%12|%13|%14|").arg(mJD, 15, 'f', 7, QLatin1Char(' ')).arg(ra, 13, 'f', 8, QLatin1Char(' ')).arg(de, 13, 'f', 8).arg(ksi, 12, 'f', 9, QLatin1Char(' ')).arg(eta, 12, 'f', 9, QLatin1Char(' ')).arg(mag, 5, 'f', 2, QLatin1Char(' ')).arg(ksiOC, 7, 'f', 1, QLatin1Char(' ')).arg(etaOC, 7, 'f', 1, QLatin1Char(' ')).arg(magOC, 5, 'f', 2, QLatin1Char(' ')).arg(x, 10, 'f', 4, QLatin1Char(' ')).arg(y, 10, 'f', 4, QLatin1Char(' ')).arg(pixmag, 10, 'f', 6, QLatin1Char(' ')).arg(Dx, 7, 'f', 1, QLatin1Char(' ')).arg(Dy, 7, 'f', 1, QLatin1Char(' ')));
        str->append(QString("%1|%2|%3|%4").arg(Dpixmag, 9, 'f', 6, QLatin1Char(' ')).arg(name, 16).arg(catName, 8).arg(catMagName, 8));
        str->append(QString("|%1").arg(mesureTimeCode));
        str->append(QString("#%1").arg(mesParams));
};

int objResRec::s2rec(QString str)
{
//	//if(REDSTAT_LOG_LEVEL>0) qDebug() << "\n" << str << "\n";
    //QStringList parts = str.split("#");
    QStringList sL = str.section("#", 0, 0).split("|");
        //QStringList sL = str.split("|");
        //if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nsize=" << sL.size() << "\n";
        if(sL.size()!=19) return 1;

        mJD = sL[0].toDouble();

        ra = sL[1].toDouble();
        de = sL[2].toDouble();

        ksi = sL[3].toDouble();
        eta = sL[4].toDouble();
        mag = sL[5].toDouble();
        ksiOC = sL[6].toDouble();
        etaOC = sL[7].toDouble();
        magOC = sL[8].toDouble();
        x = sL[9].toDouble();
        y = sL[10].toDouble();
        pixmag = sL[11].toDouble();
        Dx = sL[12].toDouble();
        Dy = sL[13].toDouble();

        Dpixmag = sL[14].toDouble();
        //OCmag = sL[15].toDouble();
        name = sL[15];
        catName = sL[16];
        catMagName = sL[17];

        //exptime = sL[18].toDouble();
        mesureTimeCode = sL.at(18);

        mesParams = str.section("#", 1, 1);

        return 0;
};

void objResRec::setMesParams(double *P, int pSize)
{
    mesParams.clear();
    QStringList mpList;
    for(int i=0; i<pSize; i++)
    {
        mpList << QString("%1").arg(P[i], 18, 'e');
    }
    mesParams = mpList.join("|");
}

void objResRec::toOcRec(ocRec *rec)
{
    rec->MJday = mJD;
    rec->catMagName = catMagName;
    rec->catName = catName;
    rec->de = de;
    rec->ra = ra;
    rec->ocRaCosDe = ksiOC*cos(grad2rad(rec->de));
    rec->ocDe = etaOC;
    rec->ocMag = magOC;
    /*rec->Dpixmag = Dpixmag;
    rec->Dx = Dx;
    rec->Dy = Dy;*/
    rec->mag0 = mag;
    rec->mesureTimeCode = mesureTimeCode;
    //rec->muDe = etaOC;
    //rec->muRaCosDe = ksiOC*cos(de);
    //rec->ocMag = magOC;
    rec->name = name;
    /*rec->pixmag = pixmag;
    rec->x = x;
    rec->y = y;*/
}

void objResRec::toSstarRes(sstarRes *rec)
{
        rec->mJD = mJD;
        rec->catMagName = catMagName;
        rec->catName = catName;
        rec->ra = ra;
        rec->de = de;
        rec->ksi = ksi;
        rec->eta = eta;
        rec->Dpixmag = Dpixmag;
        rec->Dx = Dx;
        rec->Dy = Dy;
        rec->mag = mag;
        rec->mesureTimeCode = mesureTimeCode;
        rec->etaOC = etaOC;
        rec->ksiOC = ksiOC;
        rec->magOC = magOC;
        rec->lspmName = name;
        rec->pixmag = pixmag;
        rec->x = x;
        rec->y = y;
}

void objResRec::toResultsRec(objResultsRec *resRec)
{
    resRec->name = name;
    resRec->mJD = mJD;
    resRec->ra = ra;
    resRec->de = de;
    resRec->mag = mag;
}

void objResRec::fromColList(QList <colRec*> colList)
{
    int j, sz1;
    double val;
    sz1 = colList.size();
    for(j=0; j<sz1; j++)
    {
        val = colList.at(j)->mean;
        switch(colList.at(j)->colNum)
        {
            case 0:
            mJD = val;
            //cols[k]->num++;
            break;
            case 1:
            ra = val;
            //cols[k]->num++;
            break;
            case 2:
            de = val;
            //cols[k]->num++;
            break;
            case 3:
            ksi = val;
            //cols[k]->num++;
            break;
            case 4:
            eta = val;
            //cols[k]->num++;
            break;
            case 5:
            mag = val;
            //cols[k]->num++;
            break;
            case 6:
            ksiOC = val;
            //cols[k]->num++;
            break;
            case 7:
            etaOC = val;
            //cols[k]->num++;
            break;
            case 8:
            magOC = val;
            //cols[k]->num++;
            break;
            case 9:
            x = val;
            //cols[k]->num++;
            break;
            case 10:
            y = val;
            //cols[k]->num++;
            break;
            case 11:
            pixmag = val;
            //cols[k]->num++;
            break;
            case 12:
            Dx = val;
            //cols[k]->num++;
            break;
            case 13:
            Dy = val;
            //cols[k]->num++;
            break;
            case 14:
            Dpixmag = val;
            //cols[k]->num++;
            break;

        }

    }
}


/////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////

objResultsRec::objResultsRec()
{
        mJD = ra = de  = mag = 0.0;
        name = QString("");
};

objResultsRec::objResultsRec(QString str)
{
        objResRec();
        s2rec(str);
};

objResultsRec::~objResultsRec()
{

}

void objResultsRec::clear()
{
    mJD = ra = de  = mag = 0.0;
    name = QString("");
}

void objResultsRec::copy(const objResultsRec &source)
{
    mJD = source.mJD;

    ra = source.ra;
    de = source.de;
    mag = source.mag;
/*
    ksi = source.ksi;
    eta = source.eta;

    ksiOC = source.ksiOC;
    etaOC = source.etaOC;
    magOC = source.magOC;
    x = source.x;
    y = source.y;
    pixmag = source.pixmag;
    Dx = source.Dx;
    Dy = source.Dy;

    Dpixmag = source.Dpixmag;
    //OCmag = source.OCmag;
    name = source.name;
    catName = source.catName;
    catMagName = source.catMagName;
    //exptime = source.exptime;
    mesureTimeCode = source.mesureTimeCode;
    mesParams = source.mesParams;*/
}

void objResultsRec::rec2s(QString *str)
{
    QString raStr, deStr;
        str->clear();
        DATEOBS dateObs = getDATEOBSfromMJD(mJD);
        QString dateStr = getStrFromDATEOBS(dateObs," ", 0, 6);
        deg_to_hms(&raStr, ra, " ", 2);
        deg_to_damas(&deStr, de, " ", 3);
        str->insert(0, QString("%1|%2|%3|%4|%5").arg(name.simplified(), -16).arg(dateStr).arg(raStr, 11, QLatin1Char(' ')).arg(deStr, 12, QLatin1Char(' ')).arg(mag, 5, 'f', 2, QLatin1Char(' ')));//.arg(ksi, 12, 'f', 9, QLatin1Char(' ')).arg(eta, 12, 'f', 9, QLatin1Char(' ')).arg(ksiOC, 7, 'f', 1, QLatin1Char(' ')).arg(etaOC, 7, 'f', 1, QLatin1Char(' ')).arg(magOC, 5, 'f', 2, QLatin1Char(' ')).arg(x, 10, 'f', 4, QLatin1Char(' ')).arg(y, 10, 'f', 4, QLatin1Char(' ')).arg(pixmag, 10, 'f', 6, QLatin1Char(' ')).arg(Dx, 7, 'f', 1, QLatin1Char(' ')).arg(Dy, 7, 'f', 1, QLatin1Char(' ')));
        //str->append(QString("%1|%2|%3|%4").arg(Dpixmag, 9, 'f', 6, QLatin1Char(' ')).arg(catName, 8).arg(catMagName, 8));
        //str->append(QString("|%1").arg(mesureTimeCode));
        //str->append(QString("#%1").arg(mesParams));
};

int objResultsRec::s2rec(QString str)
{
//	//if(REDSTAT_LOG_LEVEL>0) qDebug() << "\n" << str << "\n";
    //QStringList parts = str.split("#");
    QStringList sL = str.section("#", 0, 0).split("|");
        //QStringList sL = str.split("|");
        //if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nsize=" << sL.size() << "\n";
        if(sL.size()!=19) return 1;

        name = sL[0];
        mJD = getMJDfromYMD(sL[1]);//sL[1].toDouble();

        ra = sL[2].toDouble();
        de = sL[3].toDouble();
        mag = sL[6].toDouble();
 //       ksi = sL[4].toDouble();
 //       eta = sL[5].toDouble();

 //       x = sL[7].toDouble();
 //       y = sL[8].toDouble();
  //      pixmag = sL[9].toDouble();


        return 0;
};



/////////////////////////////////////////////////////////////////////


QString plateConstRec::toString()
{
    int size = params.size();
    QString res;
    res.insert(0, QString("%1|%2").arg(N, 4, 10, QLatin1Char(' ')).arg(UWE, 10, 'f', 2, QLatin1Char( ' ' )));
    for(int i=0; i<size; i++)
    {
        res.append(QString("|%1|%2").arg(params.at(i), 18, 'e', 11, QLatin1Char(' ')).arg(rms.at(i), 18, 'e', 11, QLatin1Char(' ')));
    }
    return res;
}

void plateConstRec::fromString(QString source)
{
    QStringList ops = source.split("|");
    N = ops.at(0).toInt();
    UWE = ops.at(1).toDouble();
    int size = (ops.size() - 2)/2;
    if((size*2)!=(ops.size() - 2)) return;
    double val, err;
    params.clear();
    rms.clear();
    for(int i=0; i<size; i++)
    {
        val = ops.at(i*2+2).toDouble();
        err = ops.at(i*2+3).toDouble();
        params << val;
        rms << err;
    }

}

void plateConstRec::clear()
{
    N = 0;
    UWE = 0;
    params.clear();
    rms.clear();
}

void plateConstRec::addParam(double val, double err)
{
    params << val;
    rms << err;
}

void plateConstRec::setNparams(int Num, double uwe, double *pVect, int pNum)
{
    clear();
    N = Num;
    UWE = uwe;

    for(int i=0; i<pNum; i++)
    {
        addParam(pVect[i*2], pVect[i*2+1]);
    }


}

void plateConstRec::set3params(int Num, double uwe, double A, double rmsA, double B, double rmsB, double C, double rmsC)
{
    clear();
    N = Num;
    UWE = uwe;
    addParam(A, rmsA);
    addParam(B, rmsB);
    addParam(C, rmsC);
}

void plateConstRec::set2params(int Num, double uwe, double A, double rmsA, double B, double rmsB)
{
    clear();
    N = Num;
    UWE = uwe;
    addParam(A, rmsA);
    addParam(B, rmsB);
}

void plateConstRec::copy(const plateConstRec &source)
{
    N = source.N;
    UWE = source.UWE;
    params << source.params;
    rms << source.rms;
}

errBudgetRec::errBudgetRec()
{
    RAoc = DEoc = Xoc = Yoc = 0.0;
        /*Nx = Ny = Nm = -1;
	uweX = Ax = rmsAx = Bx = rmsBx = Cx = rmsCx = 0.0;
	uweY = Ay = rmsAy = By = rmsBy = Cy = rmsCy = 0.0;
        uweM = Am = rmsAm = Bm = rmsBm = 0.0;*/
    //mesureTimeCode = QString("YYYYMMDDHHMMSSSS");
}

void errBudgetRec::clear()
{
    MJD = 0;
    RAoc = DEoc = Xoc = Yoc = 0.0;
    xParams.clear();
    yParams.clear();
    mParams.clear();
    mesureTimeCode.clear();
}

errBudgetRec::errBudgetRec(QString str)
{
	errBudgetRec();
	s2rec(str);
};
	
void errBudgetRec::rec2s(QString *str)
{
	str->clear();
        str->insert(0, QString("%1|%2|%3|%4|%5|%6|%7#").arg(MJD, 15, 'f', 7, QLatin1Char(' ')).arg(RAoc, 13, 'f', 8, QLatin1Char(' ')).arg(DEoc, 13, 'f', 8, QLatin1Char(' ')).arg(Xoc, 10, 'f', 4, QLatin1Char(' ')).arg(Yoc, 10, 'f', 4, QLatin1Char(' ')).arg(redType).arg(mesureTimeCode));
        str->append(QString("%1#%2#%3#%4").arg(xParams.toString()).arg(yParams.toString()).arg(mParams.toString()).arg(originName));
};

int errBudgetRec::s2rec(QString str)
{
    QStringList parts = str.split("#");
    if(parts.size()!=5) return 1;
//	//if(REDSTAT_LOG_LEVEL>0) qDebug() << "\n" << str << "\n";
        QStringList sL = parts.at(0).split("|");
//	//if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nsize=" << sL.size() << "\n";
        if(sL.size()!=7) return 1;

        MJD = sL[0].toDouble();
        RAoc = sL[1].toDouble();
        DEoc = sL[2].toDouble();
        Xoc = sL[3].toDouble();
        Yoc = sL[4].toDouble();
        redType = sL[5].toInt();
        mesureTimeCode = sL.at(6);


        xParams.fromString(parts.at(1));
        yParams.fromString(parts.at(2));
        mParams.fromString(parts.at(3));

        originName = parts.at(4);

        return 0;

};

void errBudgetRec::copy(const errBudgetRec &source)
{
    MJD = source.MJD;
    RAoc = source.RAoc;
    DEoc = source.DEoc;
    Xoc = source.Xoc;
    Yoc = source.Yoc;
    redType = source.redType;
    xParams.copy(source.xParams);
    yParams.copy(source.yParams);
    mParams.copy(source.mParams);
    mesureTimeCode = source.mesureTimeCode;
    originName = source.originName;
}

/////////////////////////////////////////////////////////////////////
/*
ucac2Rec::ucac2Rec()
{
}

ucac2Rec::ucac2Rec(QString str)
{
}

void ucac2Rec::rec2s(QString *str)
{
}

void ucac2Rec::s2rec(QString str)
{
}
*/


//usnob1.0

usnoRec::usnoRec()
{
    usnoName = "";
    tychoName = "";
    r0 = 0.0;
    //ra = 0.0;
    //de = 0.0;

}

usnoRec::usnoRec(QString str)
{
    usnoRec();
    s2rec(str);
}

//_r|USNO-B1.0|Tycho-2|RAJ2000|DEJ2000|e_RAJ2000|e_DEJ2000|Epoch|pmRA|pmDE|muPr|e_pmRA|e_pmDE|fit_RA|fit_DE|Ndet|Flags|B1mag|B1C|B1S|B1f|B1s/g|B1xi|B1eta|R1mag|R1C|R1S|R1f|R1s/g|R1xi|R1eta|B2mag|B2C|B2S|B2f|B2s/g|B2xi|B2eta|R2mag|R2C|R2S|R2f|R2s/g|R2xi|R2eta|Imag|IC|IS|If|Is/g|Ixi|Ieta
//arcmin|||deg|deg|mas|mas|yr|mas/yr|mas/yr|0.1|mas/yr|mas/yr|100mas|100mas|||mag|||||arcsec|arcsec|mag|||||arcsec|arcsec|mag|||||arcsec|arcsec|mag|||||arcsec|arcsec|mag|||||arcsec|arcsec
//--------|------------|------------|----------|----------|---|---|------|------|------|-|---|---|-|-|-|---|-----|-|-|---|--|------|------|-----|-|-|---|--|------|------|-----|-|-|---|--|------|------|-----|-|-|---|--|------|------|-----|-|-|---|--|------|------


void usnoRec::rec2s(QString *str)
{
}

void usnoRec::s2rec(QString str)
{
    QStringList cols;
    cols = str.split("|");
    //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("cols: %1\n").arg(cols.size());
    if(cols.size()<53) return;
    r0 = cols.at(0).toDouble();
    x0 = cols.at(1).toDouble();
    y0 = cols.at(2).toDouble();
    usnoName = cols.at(3);
    tychoName = cols.at(4);
    ra = cols.at(5).toDouble();
    de = cols.at(6).toDouble();
    e_Ra = cols.at(7).toDouble();
    e_De = cols.at(8).toDouble();
    Epoch = cols.at(9).toDouble();
    pmRA = cols.at(10).toDouble();
    pmDE = cols.at(11).toDouble();
    muPr = cols.at(12).toDouble();
    e_pmRA = cols.at(13).toDouble();
    e_pmDE = cols.at(14).toDouble();
    fit_RA = cols.at(15).toDouble();
    fit_DE = cols.at(16).toDouble();
    Ndet = cols.at(17).toInt();
    Flags = cols.at(18);

    B1mag = cols.at(19).toDouble();
    B1C = cols.at(20).toInt();
    B1S = cols.at(21).toInt();
    B1f = cols.at(22).toInt();
    B1s_g = cols.at(23).toInt();
    B1xi = cols.at(24).toDouble();
    B1eta = cols.at(25).toDouble();

    R1mag = cols.at(26).toDouble();
    R1C = cols.at(27).toInt();
    R1S = cols.at(28).toInt();
    R1f = cols.at(29).toInt();
    R1s_g = cols.at(30).toInt();
    R1xi = cols.at(31).toDouble();
    R1eta = cols.at(32).toDouble();

    B2mag = cols.at(33).toDouble();
    B2C = cols.at(34).toInt();
    B2S = cols.at(35).toInt();
    B2f = cols.at(36).toInt();
    B2s_g = cols.at(37).toInt();
    B2xi = cols.at(38).toDouble();
    B2eta = cols.at(39).toDouble();

    R2mag = cols.at(40).toDouble();
    R2C = cols.at(41).toInt();
    R2S = cols.at(42).toInt();
    R2f = cols.at(43).toInt();
    R2s_g = cols.at(44).toInt();
    R2xi = cols.at(45).toDouble();
    R2eta = cols.at(46).toDouble();

    Imag = cols.at(47).toDouble();
    IC = cols.at(48).toInt();
    IS = cols.at(49).toInt();
    If = cols.at(50).toInt();
    Is_g = cols.at(51).toInt();
    Ixi = cols.at(52).toDouble();
    Ieta = cols.at(53).toDouble();
}

void usnoRec::copy(const usnoRec &source)
{
    r0 = source.r0;
    x0 = source.x0;
    y0 = source.y0;
    usnoName = source.usnoName;
    tychoName = source.tychoName;
    ra = source.ra;
    de = source.de;
    e_Ra = source.e_Ra;
    e_De = source.e_De;
    Epoch = source.Epoch;
    pmRA = source.pmRA;
    pmDE = source.pmDE;
    muPr = source.muPr;
    e_pmRA = source.e_pmRA;
    e_pmDE = source.e_pmDE;
    fit_RA = source.fit_RA;
    fit_DE = source.fit_DE;
    Ndet = source.Ndet;
    Flags = source.Flags;

    B1mag = source.B1mag;
    B1C = source.B1C;
    B1S = source.B1S;
    B1f = source.B1f;
    B1s_g = source.B1s_g;
    B1xi = source.B1xi;
    B1eta = source.B1eta;

    R1mag = source.R1mag;
    R1C = source.R1C;
    R1S = source.R1S;
    R1f = source.R1f;
    R1s_g = source.R1s_g;
    R1xi = source.R1xi;
    R1eta = source.R1eta;

    B2mag = source.B2mag;
    B2C = source.B2C;
    B2S = source.B2S;
    B2f = source.B2f;
    B2s_g = source.B2s_g;
    B2xi = source.B2xi;
    B2eta = source.B2eta;

    R2mag = source.R2mag;
    R2C = source.R2C;
    R2S = source.R2S;
    R2f = source.R2f;
    R2s_g = source.R2s_g;
    R2xi = source.R2xi;
    R2eta = source.R2eta;

    Imag = source.Imag;
    IC = source.IC;
    IS = source.IS;
    If = source.If;
    Is_g = source.Is_g;
    Ixi = source.Ixi;
    Ieta = source.Ieta;

}



//
ucac3Rec::ucac3Rec()
{
    ra = 0;
    dec = 0;
    im1 = 0;
    im2 = 0;
    sigmag = 0;
    objt = 0;
    dsf = 0;
    sigra = 0;
    sigdc = 0;
    na1 = 0;
    nu1 = 0;
    us1 = 0;
    cn1 = 0;
    cepra = 0;
    cepdc = 0;
    pmrac = 0;
    pmdc = 0;
    sigpmr = 0;
    sigpmd = 0;
    id2m = 0;
    jmag = 0;
    hmag = 0;
    kmag = 0;
    icqflg = 0;
    e2mpho = 0;
    smB = 0;
    smR2 = 0;
    smI = 0;
    clbl = 0;
    qfB = 0;
    qfR2 = 0;
    qfI = 0;
    catflg = 0;
    g1 = 0;
    c1 = 0;
    leda = 0;
    x2m = 0;
    rn = 0;
}

ucac3Rec::ucac3Rec(QString str)
{
    ucac3Rec();
    s2rec(str);
}

void ucac3Rec::rec2s(QString *str)
{
}

void ucac3Rec::s2rec(QString str)
{
    QStringList opers = str.split("|");
    ////if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("opers num = %1\n").arg(opers.size());
    ra = hms_to_deg(opers.at(0), ":");
    dec = damas_to_deg(opers.at(1), ":");
    im1 = opers.at(2).toDouble();
    im2 = opers.at(3).toDouble();
    sigmag = opers.at(4).toDouble();
    objt = opers.at(5).toInt();
    dsf = opers.at(6).toInt();
    sigra = opers.at(7).toDouble();
    sigdc = opers.at(8).toDouble();
    na1 = opers.at(9).toInt();
    nu1 = opers.at(10).toInt();
    us1 = opers.at(11).toInt();
    cn1 = opers.at(12).toInt();
    cepra = opers.at(13).toDouble();
    cepdc = opers.at(14).toDouble();
    pmrac = opers.at(15).toDouble();
    pmdc = opers.at(16).toDouble();
    sigpmr = opers.at(17).toDouble();
    sigpmd = opers.at(18).toDouble();
    id2m = opers.at(19).toInt();
    jmag = opers.at(20).toDouble();
    hmag = opers.at(21).toDouble();
    kmag = opers.at(22).toDouble();
    icqflg = opers.at(23).toInt();
    e2mpho = opers.at(24).toInt();
    smB = opers.at(25).toDouble();
    smR2 = opers.at(26).toDouble();
    smI = opers.at(27).toDouble();
    clbl = opers.at(28).toInt();
    qfB = opers.at(29).toInt();
    qfR2 = opers.at(30).toInt();
    qfI = opers.at(31).toInt();
    catflg = opers.at(32).toInt();
    g1 = opers.at(33).toInt();
    c1 = opers.at(34).toInt();
    leda = opers.at(35).toInt();
    x2m = opers.at(36).toInt();
    rn = opers.at(37).toInt();
}

void ucac3Rec::s2recCDS(QString strNames, QString strVal)
{
    //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("catStr: %1\n").arg(str);
    QStringList oNames = strNames.split("|");
    QStringList oVal = strVal.split("|");
    //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("opers num = %1\n").arg(opers.size());
    int i, oSize;
    QString cStr;
    oSize = oVal.size();
    for(i=0; i<oSize; i++)
    {
        cStr = oNames.at(i);

        if(QString().compare(cStr, "RAJ2000")==0)
        {
            ra = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "DEJ2000")==0)
        {
            dec = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "f.mag")==0)
        {
            im1 = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "a.mag")==0)
        {
            im2 = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "e_a.mag")==0)
        {
            sigmag = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "ot")==0)
        {
            objt = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "db")==0)
        {
            dsf = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "e_RAJ2000")==0)
        {
            sigra = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "e_DEJ2000")==0)
        {
            sigdc = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "Na")==0)
        {
            na1 = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "Nu")==0)
        {
            nu1 = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "Cu")==0)
        {
            us1 = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "Ca")==0)
        {
            cn1 = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "EpRA")==0)
        {
            cepra = oVal.at(i).toDouble() - 1900;
            continue;
        }

        if(QString().compare(cStr, "EpDE")==0)
        {
            cepdc = oVal.at(i).toDouble() - 1900;
            continue;
        }

        if(QString().compare(cStr, "pmRA")==0)
        {
            pmrac = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "pmDE")==0)
        {
            pmdc = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "e_pmRA")==0)
        {
            sigpmr = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "e_pmDE")==0)
        {
            sigpmd = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "2Mkey")==0)
        {
            id2m = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "Jmag")==0)
        {
            jmag = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "Hmag")==0)
        {
            hmag = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "Kmag")==0)
        {
            kmag = oVal.at(i).toDouble();
            continue;
        }

        //icqflg = opers.at(23).toInt();
        //e2mpho = opers.at(24).toInt();

        if(QString().compare(cStr, "Bmag")==0)
        {
            smB = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "R2mag")==0)
        {
            smR2 = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "smI")==0)
        {
            smI = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "sc")==0)
        {
            clbl = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "q_Bmag")==0)
        {
            qfB = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "q_R2mag")==0)
        {
            qfR2 = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "q_Imag")==0)
        {
            qfI = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "catflg")==0)
        {
            catflg = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "g")==0)
        {
            g1 = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "c")==0)
        {
            c1 = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "LEDA")==0)
        {
            leda = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "2MX")==0)
        {
            x2m = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "MPOS")==0)
        {
            rn = oVal.at(i).toDouble();
            continue;
        }

    }
}

void ucac3Rec::copy(const ucac3Rec &source)
{
    ra = source.ra;
    dec = source.dec;
    im1 = source.im1;
    im2 = source.im2;
    sigmag = source.sigmag;
    objt = source.objt;
    dsf = source.dsf;
    sigra = source.sigra;
    sigdc = source.sigdc;
    na1 = source.na1;
    nu1 = source.nu1;
    us1 = source.us1;
    cn1 = source.cn1;
    cepra = source.cepra;
    cepdc = source.cepdc;
    pmrac = source.pmrac;
    pmdc = source.pmdc;
    sigpmr = source.sigpmr;
    sigpmd = source.sigpmd;
    id2m = source.id2m;
    jmag = source.jmag;
    hmag = source.jmag;
    kmag = source.kmag;
    icqflg = source.icqflg;
    e2mpho = source.e2mpho;
    smB = source.smB;
    smR2 = source.smR2;
    smI = source.smI;
    clbl = source.clbl;
    qfB = source.qfB;
    qfR2 = source.qfR2;
    qfI = source.qfI;
    catflg = source.catflg;
    g1 = source.g1;
    c1 = source.c1;
    leda = source.leda;
    x2m = source.x2m;
    rn = source.rn;
}

/////////////////////////////////////////////////////////////////////

////////////////////        ppmxl_rec   /////////////////////////////

ppmxl_rec::ppmxl_rec()
{
    ppmxl_id = "";
    ra = 0;
    de = 0;
    pmRA = 0.0;
    pmDE = 0.0;
    epRA = 0.0;
    epDE = 0.0;
    e_RA = 0.0;
    e_DE = 0.0;
    e_pmRA = 0.0;
    e_pmDE = 0.0;
    Jmag = 0.0;
    e_Jmag = 0.0;
    Hmag = 0.0;
    e_Hmag = 0.0;
    Kmag = 0.0;
    e_Kmag = 0.0;
    b1mag = 0.0;
    b2mag = 0.0;
    r1mag = 0.0;
    r2mag = 0.0;
    imag = 0.0;
    Smags = "";
    No = 0;
    fl = 0;
    d2m = "";
    Nomad = "";
}

ppmxl_rec::ppmxl_rec(QString str)
{
    ucac3Rec();
    s2rec(str);
}

void ppmxl_rec::rec2s(QString *str)
{
    QStringList sL;
    if(str==NULL) return;
    str->clear();
    sL << QString("%1").arg(ppmxl_id);
    sL << QString("%1").arg(ra, 10, 'f', 6);
    sL << QString("%1").arg(de, 10, 'f', 6);
    sL << QString("%1").arg(pmRA, 8, 'f', 1);
    sL << QString("%1").arg(pmDE, 8, 'f', 1);
    sL << QString("%1").arg(e_pmRA, 4, 'f', 1);
    sL << QString("%1").arg(e_pmDE, 4, 'f', 1);
    sL << QString("%1").arg(Jmag, 6, 'f', 3);
    sL << QString("%1").arg(e_Jmag, 6, 'f', 3);
    sL << QString("%1").arg(Hmag, 6, 'f', 3);
    sL << QString("%1").arg(e_Hmag, 6, 'f', 3);
    sL << QString("%1").arg(Kmag, 6, 'f', 3);
    sL << QString("%1").arg(e_Kmag, 6, 'f', 3);
    sL << QString("%1").arg(b1mag, 5, 'f', 2);
    sL << QString("%1").arg(b2mag, 5, 'f', 2);
    sL << QString("%1").arg(r1mag, 5, 'f', 2);
    sL << QString("%1").arg(r2mag, 5, 'f', 2);
    sL << QString("%1").arg(imag, 5, 'f', 2);
    sL << QString("%1").arg(Smags, 5);
    sL << QString("%1").arg(No, 2);
    sL << QString("%1").arg(fl, 2);
    sL << QString("%1").arg(d2m, 2);
    sL << QString("%1").arg(Nomad, 5);
    str->append(sL.join("|"));
}

void ppmxl_rec::s2rec(QString str)
{
    QStringList opers = str.split("|");
    if(opers.size()<26) return;
    ////if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("opers num = %1\n").arg(opers.size());
    ppmxl_id = opers.at(0);
    ra = opers.at(1).toDouble();
    de = opers.at(2).toDouble();
    pmRA = opers.at(3).toDouble();
    pmDE = opers.at(4).toDouble();
    epRA = opers.at(5).toDouble();
    epDE = opers.at(6).toDouble();
    e_RA = opers.at(7).toDouble();
    e_DE = opers.at(8).toDouble();
    e_pmRA = opers.at(9).toDouble();
    e_pmDE = opers.at(10).toDouble();
    Jmag = opers.at(11).toDouble();
    e_Jmag = opers.at(12).toDouble();
    Hmag = opers.at(13).toDouble();
    e_Hmag = opers.at(14).toDouble();
    Kmag = opers.at(15).toDouble();
    e_Kmag = opers.at(16).toDouble();
    b1mag = opers.at(17).toDouble();
    b2mag = opers.at(18).toDouble();
    r1mag = opers.at(19).toDouble();
    r2mag = opers.at(20).toDouble();
    imag = opers.at(21).toDouble();
    Smags = opers.at(22);
    No = opers.at(23).toInt();
    fl = opers.at(24).toInt();
    d2m = opers.at(25);
    Nomad = opers.at(26);
}

void ppmxl_rec::s2recCDS(QString strNames, QString strVal)
{
    //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("catStr: %1\n").arg(str);
    QStringList oNames = strNames.split("|");
    QStringList oVal = strVal.split("|");
    //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("opers num = %1\n").arg(opers.size());

    int i, oSize;
    QString cStr;
    oSize = oVal.size();
    for(i=0; i<oSize; i++)
    {
        cStr = oNames.at(i);

        if(QString().compare(cStr, "PPMXL")==0)
        {
            ppmxl_id = oVal.at(i);
            continue;
        }

        if(QString().compare(cStr, "RAJ2000")==0)
        {
            ra = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "DEJ2000")==0)
        {
            de = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "pmRA")==0)
        {
            pmRA = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "pmDE")==0)
        {
            pmDE = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "epRA")==0)
        {
            epRA = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "epDE")==0)
        {
            epDE = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "e_RAJ2000")==0)
        {
            e_RA = oVal.at(i).toDouble();
            continue;
        }


        if(QString().compare(cStr, "e_DEJ2000")==0)
        {
            e_DE = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "e_pmRA")==0)
        {
            e_pmRA = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "e_pmDE")==0)
        {
            e_pmDE = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "Jmag")==0)
        {
            Jmag = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "e_Jmag")==0)
        {
            e_Jmag = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "Hmag")==0)
        {
            Hmag = oVal.at(i).toDouble() - 1900;
            continue;
        }

        if(QString().compare(cStr, "e_Hmag")==0)
        {
            e_Hmag = oVal.at(i).toDouble() - 1900;
            continue;
        }

        if(QString().compare(cStr, "Kmag")==0)
        {
            Kmag = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "e_Kmag")==0)
        {
            e_Kmag = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "b1mag")==0)
        {
            b1mag = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "b2mag")==0)
        {
            b2mag = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "r1mag")==0)
        {
            r1mag = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "r2mag")==0)
        {
            r2mag = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "imag")==0)
        {
            imag = oVal.at(i).toDouble();
            continue;
        }

        if(QString().compare(cStr, "Smags")==0)
        {
            Smags = oVal.at(i);
            continue;
        }

        if(QString().compare(cStr, "No")==0)
        {
            No = oVal.at(i).toInt();
            continue;
        }

        if(QString().compare(cStr, "fl")==0)
        {
            fl = oVal.at(i).toInt();
            continue;
        }

        if(QString().compare(cStr, "2M")==0)
        {
            d2m = oVal.at(i);
            continue;
        }

        if(QString().compare(cStr, "NOMAD")==0)
        {
            Nomad = oVal.at(i);
            continue;
        }

    }
}

void ppmxl_rec::copy(const ppmxl_rec &source)
{
    ppmxl_id = source.ppmxl_id;
    ra = source.ra;
    de = source.de;
    pmRA = source.pmRA;
    pmDE = source.pmDE;
    epRA = source.epRA;
    epDE = source.epDE;
    e_RA = source.e_RA;
    e_DE = source.e_DE;
    e_pmRA = source.e_pmRA ;
    e_pmDE = source.e_pmDE;
    Jmag = source.Jmag;
    e_Jmag = source.e_Jmag;
    Hmag = source.Hmag;
    e_Hmag = source.e_Hmag;
    Kmag = source.Kmag;
    e_Kmag = source.e_Kmag;
    b1mag = source.b1mag;
    b2mag = source.b2mag;
    r1mag = source.r1mag;
    r2mag = source.r2mag;
    imag = source.imag;
    Smags = source.Smags;
    No = source.No;
    fl = source.fl;
    d2m = source.d2m;
    Nomad = source.Nomad;
}

/////////////////////////////////////////////////////////////////////


eqFile::eqFile()
{
	mmRec = NULL;
        ocList.clear();
        colList.clear();
};

eqFile::~eqFile()
{
        delete(mmRec);// = NULL;
        ocList.clear();
        colList.clear();
};


void eqFile::clear()
{
	ocList.clear();
	colList.clear();
	if(mmRec!=NULL)
	{
		delete mmRec;
		mmRec = NULL;
	}
}

void eqFile::init(const char *fname)
{
//	ocList.clear();
    if(REDSTAT_LOG_LEVEL>0) qDebug() << "\neqFile::init: " << fname << "\n";
	clear();
	fName.clear();
	fName.insert(0, fname);
	QString dataLine;
        QFile dataFile(fname);
        if(!dataFile.open(QIODevice::ReadOnly| QIODevice::Text))
        {
            if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("can't open: %1\n").arg(fName);
            return;
        }
	QTextStream dataStream;
	dataStream.setDevice(&dataFile);
	
	ocRec* ocr;
	colRec* colr;
	moveModelRec* mmr;
	
	while (!dataStream.atEnd())
	{
		dataLine = dataStream.readLine();
                if(dataLine==NULL) continue;
		if(dataLine[0]=='#')		//mm-string
		{
                      //  if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nline #\n";
			mmRec = new moveModelRec(dataLine);
//			ocr->s2rec(dataLine);
//			mmList << mmr;
		}
		else if(dataLine[0]=='%')	//col-string
		{
                       // if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nline \%\n";
			colr = new colRec(dataLine);
//			ocr->s2rec(dataLine);
			colList << colr;
		}
		else						//oc-string
		{
                       // if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nline\n";
			ocr = new ocRec(dataLine);
//			ocr->s2rec(dataLine);
			ocList << ocr;
		}
	}
        dataFile.close();
	
};

void eqFile::initOld(const char *fname)
{
//	ocList.clear();
	clear();
	fName.clear();
	fName.insert(0, fname);
	QString dataLine;
	QFile dataFile(fname);
	dataFile.open(QIODevice::ReadWrite| QIODevice::Text);
	QTextStream dataStream;
	dataStream.setDevice(&dataFile);
	
	ocRec* ocr;
	colRec* colr;
//	moveModelRec* mmr;
	int cNum=0;
	QStringList sL;
	QString tstr;
	
	while (!dataStream.atEnd())
	{
		dataLine = dataStream.readLine();
		if(dataLine[0]=='#')		//mm-string
		{
			switch(cNum)
			{
				case 0:
					colr = new colRec(dataLine);
					colList << colr;
					cNum++;
				break;
				case 1:
					colr = new colRec(dataLine);
					colList << colr;
					cNum++;
				break;
				case 2:
					mmRec = new moveModelRec();
					sL = dataLine.split("|");
					if(sL.size()!=8) break;
					tstr = sL[0];
					mmRec->Tm = getMJDfromYMD(tstr.mid(1));
					mmRec->xTm = hms_to_mas(sL[1], " ");
		
                                        mmRec->nPosX = sL[2].toInt();
					mmRec->uweX = sL[3].toDouble();
					mmRec->rmsX = sL[4].toDouble();
					mmRec->Xdot = sL[5].toDouble();
					mmRec->rmsXdot = sL[6].toDouble();
					mmRec->XdotOC = sL[7].toDouble();
					cNum++;
				break;
				case 3:
					sL = dataLine.split("|");
					if(sL.size()!=8) break;
					mmRec->yTm = damas_to_mas(sL[1], " ");
                                        mmRec->nPosY = sL[2].toInt();
					mmRec->uweY = sL[3].toDouble();
					mmRec->rmsY = sL[4].toDouble();
					mmRec->Ydot = sL[5].toDouble();
					mmRec->rmsYdot = sL[6].toDouble();
					mmRec->YdotOC = sL[7].toDouble();
					cNum++;
				break;
				default:
				break;
			}
			
			
		}
		else						//oc-string
		{
//			//if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nline\n";
			ocr = new ocRec(dataLine);
//			ocr->s2rec(dataLine);
			ocList << ocr;
		}
	}
	
	dataFile.close();
	
};

void eqFile::save(int saveType)
{
	QString tstr;
	sortColList();
	QFile dataFile(fName);
	dataFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
	QTextStream dataStream;
	dataStream.setDevice(&dataFile);
	
	int i, sz;
	
	sz = ocList.size();
	for(i=0;i<sz;i++)
	{
		tstr.clear();
                switch(saveType)
                {
                case 0:
                    ocList[i]->rec2s(&tstr);
                    break;
                case 1:
                    ocList[i]->rec2sBase(&tstr);
                    break;
                case 2:
                    ocList[i]->rec2sSeries(&tstr);
                    break;
                }
                dataStream << tstr << "\n";
	}
	
	sz = colList.size();
	for(i=0;i<sz;i++)
	{
		tstr.clear();
		colList[i]->rec2s(&tstr);
		dataStream << tstr << "\n";
	}
	if(mmRec!=NULL)
	{
		tstr.clear();
		mmRec->rec2s(&tstr);
		dataStream << tstr << "\n";
	}
	
	dataFile.close();
}

void eqFile::saveAs(QString fname, int saveType)
{
	sortColList();
	fName.clear();
	fName.insert(0, fname);
        save(saveType);
}

int eqFile::getColRecNum(int colNum)
{
//	if(colRec==NULL) return -1;
	int i;
	int csz = colList.size();
	
	for(i=0; i<csz; i++)
	{
		if(colList[i]->colNum==colNum)
		{
//			cRec = colList[i];
			return i;
		}
	}
	
	return -1;
}

int eqFile::setColRec(colRec* cRec)
{
	colRec *pRec;
	int rpos = getColRecNum(cRec->colNum);
//	//if(REDSTAT_LOG_LEVEL>0) qDebug() << "rpos= " << rpos << "\n";
	if(rpos==-1)
	{
		pRec = new colRec;
		colList << pRec;
		rpos = colList.size()-1;
	}

	colList[rpos]->colNum = cRec->colNum;
	colList[rpos]->num = cRec->num;
	colList[rpos]->mean = cRec->mean;
	colList[rpos]->rmsMean = cRec->rmsMean;
        colList[rpos]->rmsOne = cRec->rmsOne;
	
	return 0;
}

void eqFile::sortColList()
{
	int sz = colList.size();
	int nMin, pMin;
	
	for(int i=0; i<sz-1; i++)
	{
		nMin = colList[i]->colNum;
		pMin = i;
		for(int j=i+1; j<sz; j++)
		{
			if(colList[j]->colNum<nMin){pMin = j; nMin = colList[j]->colNum;}
		}
		
		colList.swap(i, pMin);
		
	}
}

void eqFile::delMMrec()
{
	if(mmRec!=NULL) delete(mmRec);
	mmRec = NULL;
}

int eqFile::countCols(QString colNums)
{
    colList.clear();
    QStringList columns = colNums.split(",");
    int i, j, k, sz, sz1;
    QList <colRec *> cols;
    colRec *cRec;
    QString str;

    sz = ocList.size();
   // if(sz<=2) return 1;

            sz1 = columns.count();
            for(j=0;j<sz1;j++)
            {
                    str = columns[j];
                    cRec = new colRec;
                    cRec->colNum = str.toInt();
    //							means<<0;
    //							serrors<<0;
                    cols << cRec;
            }


            for(j=0;j<sz;j++)
            {
                    for(k=0;k<sz1;k++)
                    {
                            switch(cols[k]->colNum)
                            {
                                    case 0:
                                    cols[k]->mean += ocList[j]->MJday;
                                    cols[k]->num++;
                                    break;
                                    case 1:
                                    cols[k]->mean += ocList[j]->ra;
                                    cols[k]->num++;
                                    break;
                                    case 2:
                                    cols[k]->mean += ocList[j]->de;
                                    cols[k]->num++;
                                    break;
                                    case 3:
                                    cols[k]->mean += ocList[j]->mag0;
                                    cols[k]->num++;
                                    break;
                                    case 4:
                                    cols[k]->mean += ocList[j]->ocRaCosDe;
                                    cols[k]->num++;
                                    break;
                                    case 5:
                                    cols[k]->mean += ocList[j]->ocDe;
                                    cols[k]->num++;
                                    break;
                                    case 6:
                                    cols[k]->mean += ocList[j]->ocMag;
                                    cols[k]->num++;
                                    break;

                            /*case 7:
                            cols[k]->mean += ocList[j]->x;
                            cols[k]->num++;
                            break;
                            case 8:
                            cols[k]->mean += ocList[j]->y;
                            cols[k]->num++;
                            break;
                            case 9:
                            cols[k]->mean += ocList[j]->pixmag;
                            cols[k]->num++;
                            break;
                            case 10:
                            cols[k]->mean += ocList[j]->Dx;
                            cols[k]->num++;
                            break;
                            case 11:
                            cols[k]->mean += ocList[j]->Dy;
                            cols[k]->num++;
                            break;
                            case 12:
                            cols[k]->mean += ocList[j]->Dpixmag;
                            cols[k]->num++;
                            break;*/


                                    case 7:
                                    cols[k]->mean += ocList[j]->topDist;
                                    cols[k]->num++;
                                    break;
                                    case 8:
                                    cols[k]->mean += ocList[j]->muRaCosDe;


                                    cols[k]->num++;
                                    break;
                                    case 9:
                                    cols[k]->mean += ocList[j]->muDe;
                                    cols[k]->num++;
                                    break;
                                    case 10:
                                    cols[k]->mean += ocList[j]->Vr;
                                    cols[k]->num++;
                                    break;
                                    case 11:
                                    cols[k]->mean += ocList[j]->phase;
                                    cols[k]->num++;
                                    break;
                                    case 12:
                                    cols[k]->mean += ocList[j]->elong;
                                    cols[k]->num++;
                                    break;
                                    case 13:
                                    cols[k]->mean += ocList[j]->expTime;
                                    cols[k]->num++;
                                    break;
                            }
                    }
            }
            for(k=0;k<sz1;k++) cols[k]->mean /= cols[k]->num;

            for(j=0;j<sz;j++)
            {
                    for(k=0;k<sz1;k++)
                    {
                            switch(cols[k]->colNum)
                            {
                                    case 0:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->MJday, 2.0);
                                    break;
                                    case 1:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->ra, 2.0);
                                    break;
                                    case 2:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->de, 2.0);
                                    break;
                                    case 3:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->mag0, 2.0);
                                    break;
                                    case 4:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->ocRaCosDe, 2.0);
                                    break;
                                    case 5:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->ocDe, 2.0);
                                    break;
                                    case 6:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->ocMag, 2.0);
                                    break;
/*
                            case 7:
                            cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->x, 2.0);
                            break;
                            case 8:
                            cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->y, 2.0);
                            break;
                            case 9:
                            cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->pixmag, 2.0);
                            break;
                            case 10:
                            cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->Dx, 2.0);
                            break;
                            case 11:
                            cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->Dx, 2.0);
                            break;
                            case 12:
                            cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->Dpixmag, 2.0);
                            break;
*/
                                    case 7:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->topDist, 2.0);
                                    break;
                                    case 8:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->muRaCosDe, 2.0);
                                    break;
                                    case 9:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->muDe, 2.0);
                                    break;
                                    case 10:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->Vr, 2.0);
                                    break;
                                    case 11:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->phase, 2.0);
                                    break;
                                    case 12:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->elong, 2.0);
                                    break;
                                    case 13:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->expTime, 2.0);
                                    break;
                            }
                    }
            }

            for(k=0;k<sz1;k++)
            {
                    cols[k]->rmsOne = sqrt(cols[k]->rmsOne/(cols[k]->num-1.0));
                    cols[k]->rmsMean = cols[k]->rmsOne/sqrt(cols[k]->num);
                    str.clear();
                    cols[k]->rec2s(&str);
//                    //if(REDSTAT_LOG_LEVEL>0) qDebug() << k << ": " << str << "\n";
                    setColRec(cols[k]);
            }

    sortColList();
    return 0;
}

int eqFile::reCountCols()
{
    QStringList colNums;
    int i, sz;
    sz = colList.size();

    for(i=0; i<sz; i++)
    {
        colNums << QString("%1").arg(colList[i]->colNum);
    }
    if(countCols(colNums.join(","))) return 1;
    return 0;
}

colRec* eqFile::getColNum(int cNum)
{
    int i, sz;
    sz = colList.size();
    for(i=0; i<sz; i++)
    {
        if(cNum==colList[i]->colNum) return colList[i];
    }
    return NULL;
}

void eqFile::do3sigma(double proofP, double sigmaMul, QList <ocRec*> *rejList)
{

    int num0, num1, i, sz;
    int num0E, num1E;
    double meanRa, meanDec;
    double sigmaKsi, sigmaEta;
    double maxOCKsi, maxOCEta;

    num1 = ocList.size();

    QFile s3File("s3file.txt");
    s3File.open(QIODevice::Append);
    QTextStream s3Stm(&s3File);
    if(REDSTAT_LOG_LEVEL>0) qDebug() << "\ndo3Sigma\n";
    s3Stm << "\ndo3Sigma\n";
    if(num1<3) return;

    if((getColRecNum(4)!=-1)&&(getColRecNum(5)!=-1)) reCountCols();
    else countCols("4,5");

    do
    {
            num0 = num1;



            meanRa = getColNum(4)->mean;
            meanDec = getColNum(5)->mean;

            sigmaKsi = getColNum(4)->rmsOne;
            sigmaEta = getColNum(5)->rmsOne;

            sz = ocList.size();
            if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("\n\nocList:\n");
            for(i=0; i<sz; i++)
            {
                if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("%1:\t%2\t%3\n").arg(i).arg(ocList[i]->ocRaCosDe, 7, 'f', 1).arg(ocList[i]->ocDe, 7, 'f', 1);
                s3Stm << QString("%1:\t%2\t%3\n").arg(i).arg(ocList[i]->ocRaCosDe, 7, 'f', 1).arg(ocList[i]->ocDe, 7, 'f', 1);
            }

            if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("\nsigmaKsi= %1\t\tsigmaEta= %2\n").arg(sigmaKsi, 7, 'f', 1).arg(sigmaEta, 7, 'f', 1);
            if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("\nmeanKsi= %1\t\tmeanEta= %2\n").arg(meanRa, 7, 'f', 1).arg(meanDec, 7, 'f', 1);

            s3Stm << QString("\nsigmaKsi= %1\t\tsigmaEta= %2\n").arg(sigmaKsi, 7, 'f', 1).arg(sigmaEta, 7, 'f', 1);
            s3Stm << QString("\nmeanKsi= %1\t\tmeanEta= %2\n").arg(meanRa, 7, 'f', 1).arg(meanDec, 7, 'f', 1);

            maxOCKsi = sigmaMul*sigmaKsi;
            maxOCEta = sigmaMul*sigmaEta;

            if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("\nmaxOCKsi= %1\t\tmaxOCEta= %2\n").arg(maxOCKsi, 7, 'f', 1).arg(maxOCEta, 7, 'f', 1);
            s3Stm << QString("\nmaxOCKsi= %1\t\tmaxOCEta= %2\n").arg(maxOCKsi, 7, 'f', 1).arg(maxOCEta, 7, 'f', 1);


            sz = ocList.size();
            for(i=sz-1; i>=0; i--)
            {
                if((fabs(meanRa-ocList[i]->ocRaCosDe)>maxOCKsi)||(fabs(meanDec-ocList[i]->ocDe)>maxOCEta))
                {
                    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("%1remove:\t%2\t%3\n").arg(i).arg(ocList[i]->ocRaCosDe).arg(ocList[i]->ocDe);
                    s3Stm << QString("%1remove:\t%2\t%3\n").arg(i).arg(ocList[i]->ocRaCosDe).arg(ocList[i]->ocDe);
                    if(rejList!=NULL) rejList->append(ocList.at(i));
                    ocList.removeAt(i);
                }
            }
            num1 = ocList.size();

            if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("\nnum0= %1\t\tnum1= %2\n").arg(num0).arg(num1);
            s3Stm << QString("\nnum0= %1\t\tnum1= %2\n").arg(num0).arg(num1);

            if((getColRecNum(4)!=-1)&&(getColRecNum(5)!=-1)) reCountCols();
            else countCols("4,5");

    }while(abs(num0-num1)>(proofP*num1));

    s3File.close();
}

int eqFile::countMM(int fp, int ft, int vflag)
{
    int sz, i, j;
    double t;

    delMMrec();
    sz = ocList.size();

    if(sz<=2) return 1;


        double *X = new double[sz];
        double *Y = new double[sz];
        double *Weights = new double[sz];
        double *C = new double[sz*2];
        double *Dx = new double[2*2];
        double *Dy = new double[2*2];
        double *Zx = new double[2];
        double *Zy = new double[2];
        double *uZx = new double[2];
        double *uZy = new double[2];
        int *excX = new int [sz];
        int *excY = new int [sz];
        double uweX, uweY;
        int rnX, rnY;

        double mt = 0,mxdot=0,mydot=0;
        QVector<double> times;
        for (j=0;j<sz;j++)
        {
                switch(ft)
                {
                        case 0:{t = ocList.at(j)->MJday;break;}
                }
                mt = mt + t;
                times << t;
                if(vflag==1)
                {
                        //elS = dataLine.section('|',xdc,xdc);
                        mxdot = mxdot + ocList.at(j)->muRaCosDe;// elS.toDouble();
                        //elS = dataLine.section('|',ydc,ydc);
                        mydot = mydot + ocList.at(j)->muDe;// elS.toDouble();
                }
        }
        mt = mt/(sz*1.0);//dataLines.count();
        if(vflag==1)
        {
                mxdot = mxdot/(1.0*sz);//dataLines.count();
                mydot = mydot/(1.0*sz);//dataLines.count();
        }
        for (j=0;j<sz;j++)
        {
                Weights[j]=1.0;
                //dataLine = dataLines[j];
                switch(fp)
                {
                        case 0:{X[j] = ocList.at(j)->ra;Y[j]=ocList.at(j)->de;break;}
                            default:{X[j] = ocList.at(j)->ra;Y[j]=ocList.at(j)->de;break;}

                }
                C[j*2] = 1;C[j*2+1] = times[j]-mt;
        }
//						matrix<double> Zx = LSM_corr(X, C, Weights, 500);
//						matrix<double> Zy = LSM_corr(Y, C, Weights, 500);
        uweX = uweY = 0.0;
        iLSM(2, sz, 500, excX, Zx, C, X, uweX, Dx, 3, rnX, Weights);
        iLSM(2, sz, 500, excY, Zy, C, Y, uweY, Dy, 3, rnY, Weights);

        if(rnX<=2||rnY<=2) return 2;

/*        lsm(2, sz, Zx, C, X, uweX, Dx, Weights);
        lsm(2, sz, Zy, C, Y, uweY, Dy, Weights);
        rnX = rnY = sz;
*/
        //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("Zx: %1 %2\tuweX: %3\n").arg(Zx[0]).arg(Zx[1]/1440.0).arg(uweX);
        //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("Zy: %1 %2\tuweY: %3\n").arg(Zy[0]).arg(Zy[1]/1440.0).arg(uweY);
        uZx[0] = sqrt(uweX*Dx[0]);
        uZx[1] = sqrt(uweX*Dx[3]);
        uZy[0] = sqrt(uweY*Dy[0]);
        uZy[1] = sqrt(uweY*Dy[3]);
//              slsm(2, int h, double *const z, const double* c, const double* L, const double* W = 0);
        //END apparent motion parameters
        //BEGIN output

        mmRec = new moveModelRec;
        mmRec->Tm = mt;
        mmRec->xTm = Zx[0];
        mmRec->yTm = Zy[0];
        mmRec->nPosX = rnX;
        mmRec->uweX = uweX;
        mmRec->rmsX = uZx[0];
        mmRec->Xdot = Zx[1]/1440.0;
        mmRec->rmsXdot = uZx[1]/1440.0;
        mmRec->XdotOC = Zx[1]/1440.0 - mxdot;
        mmRec->nPosY = rnY;
        mmRec->uweY = uweY;
        mmRec->rmsY = uZy[0];
        mmRec->Ydot = Zy[1]/1440.0;
        mmRec->rmsYdot = uZy[1]/1440.0;
        mmRec->YdotOC = Zy[1]/1440.0 - mydot;

        mmRec->fp = fp;
        mmRec->ft = ft;
        mmRec->vflag = vflag;

        return 0;

}
void eqFile::sortOClist()
{
    int sz = ocList.size();
    int nMin, pMin;

    for(int i=0; i<sz-1; i++)
    {
            nMin = ocList[i]->MJday;
            pMin = i;
            for(int j=i+1; j<sz; j++)
            {
                    if(ocList[j]->MJday<nMin){pMin = j; nMin = ocList[j]->MJday;}
            }

            ocList.swap(i, pMin);

    }
}

void eqFile::findSeries(QList <eqFile*> *eqList)
{
    eqFile *eqTemp;
    int i, szi, j;
    szi = ocList.size();

    sortOClist();

    double timeT = 0.0;
    double timeC, dt0, dt1;
    dt1 = 0;
    double dtMax = 0.0104166666666667;
    j=-1;
    for(i=0; i<szi; i++)
    {
        dt0 = dt1;
        timeC = ocList.at(i)->MJday;
        dt1 = timeC-timeT;
        if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("timeC= %1\tdt0= %2\tdt1= %3\n").arg(timeC, 12, 'f', 5).arg(dt0).arg(dt1);
        //if(dt1>1.5*dt0)
        if(dt1>dtMax)
        {
            if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("\nnew serie\n");
            eqTemp = new eqFile;
            *eqList << eqTemp;
            j++;
        }
        //dt = fabs(timeC-timeT);
        eqList->at(j)->ocList << ocList.at(i);
        timeT = timeC;
    }

    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("eqList size: %1\n").arg(eqList->size());
}

void eqFile::removeMes(QString mesureTimeCode)
{
    int i, szi;
    szi = ocList.size();
    for(i=szi-1; i>=0; i--)
    {
        if(QString::compare(mesureTimeCode, ocList.at(i)->mesureTimeCode)==0) ocList.removeAt(i);
    }
}
/*
void eqFile::getSeriesRec(eqSeriesRec *eqsRec)
{
    countCols("0,1,2,3,4,5,6,13,14,15,16,17,18,19");
    sortColList();
    countMM();
    if(REDSTAT_LOG_LEVEL>0) qDebug() << "mmRec: " << mmRec << "\n";

    colRec* cRec;


    cRec = getColNum(0);
    if(cRec!=NULL) eqsRec->MJday = cRec->mean;

    cRec = getColNum(1);
    if(cRec!=NULL) eqsRec->ra = cRec->mean;

    cRec = getColNum(2);
    if(cRec!=NULL) eqsRec->de = cRec->mean;

    cRec = getColNum(3);
    if(cRec!=NULL) eqsRec->mag0 = cRec->mean;

    cRec = getColNum(4);
    if(cRec!=NULL) eqsRec->ocDe = cRec->mean;

    cRec = getColNum(5);
    if(cRec!=NULL) eqsRec->ocRaCosDe = cRec->mean;

    cRec = getColNum(6);
    if(cRec!=NULL) eqsRec->ocMag = cRec->mean;

    cRec = getColNum(13);
    if(cRec!=NULL) eqsRec->topDist = cRec->mean;

    cRec = getColNum(14);
    if(cRec!=NULL) eqsRec->muRaCosDe = cRec->mean;

    cRec = getColNum(15);
    if(cRec!=NULL) eqsRec->muDe = cRec->mean;

    cRec = getColNum(16);
    if(cRec!=NULL) eqsRec->Vr = cRec->mean;

    cRec = getColNum(17);
    if(cRec!=NULL) eqsRec->phase = cRec->mean;

    cRec = getColNum(18);
    if(cRec!=NULL) eqsRec->elong = cRec->mean;

    cRec = getColNum(19);
    if(cRec!=NULL) eqsRec->expTime = cRec->mean;

    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("\n19 mean\n");

    if(mmRec!=NULL)
    {
        if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("ra: %1\tmmRa: %2\n").arg(eqsRec->ra).arg(mmRec->xTm);
        if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("de: %1\tmmDe: %2\n").arg(eqsRec->de).arg(mmRec->yTm);
        eqsRec->ocRaMM = grad_to_mas(eqsRec->ra - mmRec->xTm);
        eqsRec->ocDeMM = grad_to_mas(eqsRec->de - mmRec->yTm);
    }
    else
    {
        eqsRec->ocRaMM = 0.0;
        eqsRec->ocDeMM = 0.0;
    }

    int i, szi;
    szi = ocList.size();

    eqsRec->name=QString("");
    eqsRec->catName=QString("");
    eqsRec->catMagName=QString("");

    for(i=0; i<szi; i++)
    {
        if((eqsRec->name=="")||(eqsRec->name==ocList.at(i)->name))eqsRec->name=ocList.at(i)->name;
        else eqsRec->name=QString("var");

        if((eqsRec->catName=="")||(eqsRec->catName==ocList.at(i)->catName))eqsRec->catName=ocList.at(i)->catName;
        else eqsRec->catName=QString("var");

        if((eqsRec->catMagName=="")||(eqsRec->catMagName==ocList.at(i)->catMagName))eqsRec->catMagName=ocList.at(i)->catMagName;
        else eqsRec->catMagName=QString("var");
    }
}
*/
void eqFile::getSeriesRec(ocRec *eqsRec)
{
    countCols("0,1,2,3,4,5,6,13,14,15,16,17,18,19");
    sortColList();
    countMM();
    if(REDSTAT_LOG_LEVEL>0) qDebug() << "mmRec: " << mmRec << "\n";

    colRec* cRec;


    cRec = getColNum(0);
    if(cRec!=NULL) eqsRec->MJday = cRec->mean;
    else eqsRec->MJday = 0.0;

    cRec = getColNum(1);
    if(cRec!=NULL) eqsRec->ra = cRec->mean;
    else  eqsRec->ra = 0.0;

    cRec = getColNum(2);
    if(cRec!=NULL) eqsRec->de = cRec->mean;
    else eqsRec->de = 0.0;

    cRec = getColNum(3);
    if(cRec!=NULL) eqsRec->mag0 = cRec->mean;
    else eqsRec->mag0 = 0.0;

    cRec = getColNum(4);
    if(cRec!=NULL) eqsRec->ocDe = cRec->mean;
    else eqsRec->ocDe = 0.0;

    cRec = getColNum(5);
    if(cRec!=NULL) eqsRec->ocRaCosDe = cRec->mean;
    else eqsRec->ocRaCosDe = 0.0;

    cRec = getColNum(6);
    if(cRec!=NULL) eqsRec->ocMag = cRec->mean;
    else eqsRec->ocMag = 0.0;

    cRec = getColNum(13);
    if(cRec!=NULL) eqsRec->topDist = cRec->mean;
    else eqsRec->topDist = 0.0;

    cRec = getColNum(14);
    if(cRec!=NULL) eqsRec->muRaCosDe = cRec->mean;
    else eqsRec->muRaCosDe = 0.0;

    cRec = getColNum(15);
    if(cRec!=NULL) eqsRec->muDe = cRec->mean;
    else eqsRec->muDe = 0.0;

    cRec = getColNum(16);
    if(cRec!=NULL) eqsRec->Vr = cRec->mean;
    else eqsRec->Vr = 0.0;

    cRec = getColNum(17);
    if(cRec!=NULL) eqsRec->phase = cRec->mean;
    else eqsRec->phase = 0.0;

    cRec = getColNum(18);
    if(cRec!=NULL) eqsRec->elong = cRec->mean;
    else eqsRec->elong = 0.0;

    cRec = getColNum(19);
    if(cRec!=NULL) eqsRec->expTime = cRec->mean;
    else eqsRec->expTime = 0.0;

    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("\n19 mean\n");
/*
    if(mmRec!=NULL)
    {
        if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("ra: %1\tmmRa: %2\n").arg(eqsRec->ra).arg(mmRec->xTm);
        if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("de: %1\tmmDe: %2\n").arg(eqsRec->de).arg(mmRec->yTm);
        eqsRec->ocRaMM = grad_to_mas(eqsRec->ra - mmRec->xTm);
        eqsRec->ocDeMM = grad_to_mas(eqsRec->de - mmRec->yTm);
    }
    else
    {
        eqsRec->ocRaMM = 0.0;
        eqsRec->ocDeMM = 0.0;
    }
*/
    int i, szi;
    szi = ocList.size();

    eqsRec->name=QString("");
    eqsRec->catName=QString("");
    eqsRec->catMagName=QString("");
    eqsRec->mesureTimeCode=QString("");

    for(i=0; i<szi; i++)
    {
        if((eqsRec->name=="")||(eqsRec->name==ocList.at(i)->name))eqsRec->name=ocList.at(i)->name;
        else eqsRec->name=QString("var");

        if((eqsRec->catName=="")||(eqsRec->catName==ocList.at(i)->catName))eqsRec->catName=ocList.at(i)->catName;
        else eqsRec->catName=QString("var");

        if((eqsRec->catMagName=="")||(eqsRec->catMagName==ocList.at(i)->catMagName))eqsRec->catMagName=ocList.at(i)->catMagName;
        else eqsRec->catMagName=QString("var");
    }
}

//////////////////////////////////////////////////////////////////////

residualFile::residualFile()
{
    clear();
};

residualFile::~residualFile()
{
        clear();
};

void residualFile::clear()
{
	resList.clear();
}

void residualFile::init(const char *fname)

{
//	resList.clear();
    if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nresidualFile::init: " << fname << "\n";
	clear();
	fName.clear();
	fName.insert(0, fname);
	QString dataLine;
	QFile dataFile(fname);
	dataFile.open(QIODevice::ReadWrite| QIODevice::Text);
	QTextStream dataStream;
	dataStream.setDevice(&dataFile);
	
	residualsRec* rr;
	
	while (!dataStream.atEnd())
	{
		dataLine = dataStream.readLine();
		
//		//if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nline \#\n";
		rr = new residualsRec(dataLine);
//		ocr->s2rec(dataLine);
		resList << rr;
		
	}
	dataFile.close();
};


void residualFile::save()
{
	QString tstr;
	
	QFile dataFile(fName);
        dataFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
	QTextStream dataStream;
	dataStream.setDevice(&dataFile);
	
	int i, sz;
	
	sz = resList.size();
	for(i=0;i<sz;i++)
	{
		resList[i]->rec2s(&tstr);
		dataStream << tstr << "\n";
	}
};

void residualFile::detStat()
{
    int i, recNum;
    recNum = resList.size();
    meanKsi = meanEta = rmsOneKsi = rmsOneEta = rmsMeanKsi = rmsMeanEta = 0.0;

    for(i=0; i<recNum; i++)
    {
        meanKsi += resList.at(i)->ksiOC*cos(grad2rad(resList.at(i)->de));
        meanEta += resList.at(i)->etaOC;
    }
    meanKsi /= recNum;
    meanEta /= recNum;
    for(i=0; i<recNum; i++)
    {
        //rmsOneKsi += pow(resList.at(i)->ksiOC*cos(grad2rad(resList.at(i)->de)) - meanKsi, 2.0);
        rmsOneKsi += pow(resList.at(i)->ksiOC - meanKsi, 2.0);
        rmsOneEta += pow(resList.at(i)->etaOC - meanEta, 2.0);
    }

    rmsOneKsi  = sqrt(rmsOneKsi/(recNum - 1.0));
    rmsOneEta  = sqrt(rmsOneEta/(recNum - 1.0));
    rmsMeanKsi = rmsOneKsi/sqrt(recNum);
    rmsMeanEta = rmsOneEta/sqrt(recNum);

}

void residualFile::remSigma(double sg, double proofP)
{
    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("remSigma: %1\n").arg(sg);
    int i, recNum, recNum0;
    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("stars before %1\n").arg(resList.size());
    if(resList.size()==0) return;

    double maxResKsi, maxResEta, resKsi, resEta;
    do
    {
        recNum = resList.size();
        detStat();

        if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("Ksi: %1\t%2\t%3\n").arg(meanKsi).arg(rmsOneKsi).arg(rmsMeanKsi);
        if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("Eta: %1\t%2\t%3\n").arg(meanEta).arg(rmsOneEta).arg(rmsMeanEta);
        maxResKsi = sg*rmsOneKsi;
        maxResEta = sg*rmsOneEta;
        if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("maxKsi: %1\n").arg(maxResKsi);
        if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("maxEta: %1\n").arg(maxResEta);
        for(i=recNum-1; i>0; i--)
        {
            //resKsi = fabs(meanKsi - resList.at(i)->ksiOC*cos(grad2rad(resList.at(i)->de)));
            resKsi = fabs(meanKsi - resList.at(i)->ksiOC);
            resEta = fabs(meanEta - resList.at(i)->etaOC);

            if(resKsi>maxResKsi||resEta>maxResEta)
            {
                if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("remove oc: %1\t%2\n").arg(resKsi).arg(resEta);
                resList.removeAt(i);
            }
        }

        recNum0 = resList.size();
        if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("num: %1\tnum0: %2\n").arg(recNum).arg(recNum0);
    }while(recNum0>10&&(abs(recNum-recNum0)>(proofP*recNum)));
    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("stars remain %1\n").arg(resList.size());
}

void residualFile::saveAs(QString fname)
{
	fName.clear();
	fName.insert(0, fname);
	save();
};

void residualFile::removeMes(QString mesureTimeCode)
{
    int i, szi;
    szi = resList.size();
    for(i=szi-1; i>=0; i--)
    {
        if(QString::compare(mesureTimeCode, resList.at(i)->mesureTimeCode)==0) resList.removeAt(i);
    }
}

//////////////////////////////////////////////////////////////////////

objResidualFile::objResidualFile()
{
    ocList.clear();
};

objResidualFile::~objResidualFile()
{
        clear();
};

void objResidualFile::clear()
{
        ocList.clear();
}

void objResidualFile::init(QString fname)
{
//	resList.clear();
    if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nresidualFile::init: " << fname << "\n";
        clear();
        fName.clear();
        fName.insert(0, fname);
        QString dataLine;
        QFile dataFile(fname);
        dataFile.open(QIODevice::ReadWrite| QIODevice::Text);
        QTextStream dataStream;
        dataStream.setDevice(&dataFile);

        objResRec* rr;

        while (!dataStream.atEnd())
        {
                dataLine = dataStream.readLine();

                if(dataLine.size()< 10) continue;
//		//if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nline \#\n";
                rr = new objResRec;
                if(rr->s2rec(dataLine)) continue;
//		ocr->s2rec(dataLine);
                ocList << rr;

        }
        dataFile.close();
};


void objResidualFile::save()
{
        QString tstr;

        QFile dataFile(fName);
        dataFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
        QTextStream dataStream;
        dataStream.setDevice(&dataFile);

        int i, sz;

        sz = ocList.size();
        for(i=0;i<sz;i++)
        {
                ocList[i]->rec2s(&tstr);
                dataStream << tstr << "\n";
        }
        dataFile.close();
};

void objResidualFile::saveAs(QString fname)
{
        fName.clear();
        fName.insert(0, fname);
        save();
};

void objResidualFile::removeMes(QString mesureTimeCode)
{
    int i, szi;
    szi = ocList.size();
    for(i=szi-1; i>=0; i--)
    {
        if(QString::compare(mesureTimeCode, ocList.at(i)->mesureTimeCode)==0) ocList.removeAt(i);
    }
}

void objResidualFile::do3sigma(double proofP, double sigmaMul, QList <objResRec*> *rejList)
{

    int num0, num1, i, sz;
    double meanRa, meanDec;
    double sigmaKsi, sigmaEta;
    double maxOCKsi, maxOCEta;

    colRec raCol, deCol;

    num1 = ocList.size();

    //QFile s3File("s3file.txt");
    //s3File.open(QIODevice::Append);
    //QTextStream s3Stm(&s3File);
    if(REDSTAT_LOG_LEVEL>0) qDebug() << "\ndo3Sigma\n";
    //s3Stm << "\ndo3Sigma\n";
    if(num1<3) return;

    do
    {
            num0 = num1;

            if(getColNum(&raCol, 6)||getColNum(&deCol, 7)) return;
            //if((getColRecNum(6)!=-1)&&(getColRecNum(7)!=-1)) reCountCols();
            //else countCols("6,7");

            meanRa = raCol.mean;
            meanDec = deCol.mean;

            sigmaKsi = raCol.rmsOne;
            sigmaEta = deCol.rmsOne;

            sz = ocList.size();
            if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("\n\nocList:\n");
            for(i=0; i<sz; i++)
            {
                if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("%1:\t%2\t%3\n").arg(i).arg(ocList[i]->ksiOC, 7, 'f', 1).arg(ocList[i]->etaOC, 7, 'f', 1);
                //s3Stm << QString("%1:\t%2\t%3\n").arg(i).arg(ocList[i]->ksiOC, 7, 'f', 1).arg(ocList[i]->etaOC, 7, 'f', 1);
            }

            if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("\nsigmaKsi= %1\t\tsigmaEta= %2\n").arg(sigmaKsi, 7, 'f', 1).arg(sigmaEta, 7, 'f', 1);
            if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("\nmeanKsi= %1\t\tmeanEta= %2\n").arg(meanRa, 7, 'f', 1).arg(meanDec, 7, 'f', 1);

            //s3Stm << QString("\nsigmaKsi= %1\t\tsigmaEta= %2\n").arg(sigmaKsi, 7, 'f', 1).arg(sigmaEta, 7, 'f', 1);
            //s3Stm << QString("\nmeanKsi= %1\t\tmeanEta= %2\n").arg(meanRa, 7, 'f', 1).arg(meanDec, 7, 'f', 1);

            maxOCKsi = sigmaMul*sigmaKsi;
            maxOCEta = sigmaMul*sigmaEta;

            if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("\nmaxOCKsi= %1\t\tmaxOCEta= %2\n").arg(maxOCKsi, 7, 'f', 1).arg(maxOCEta, 7, 'f', 1);
            //s3Stm << QString("\nmaxOCKsi= %1\t\tmaxOCEta= %2\n").arg(maxOCKsi, 7, 'f', 1).arg(maxOCEta, 7, 'f', 1);


            sz = ocList.size();
            for(i=sz-1; i>=0; i--)
            {
                if((fabs(meanRa-ocList[i]->ksiOC)>maxOCKsi)||(fabs(meanDec-ocList[i]->etaOC)>maxOCEta))
                {
                    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("%1remove:\t%2\t%3\n").arg(i).arg(ocList[i]->ksiOC).arg(ocList[i]->etaOC);
                    //s3Stm << QString("%1remove:\t%2\t%3\n").arg(i).arg(ocList[i]->ksiOC).arg(ocList[i]->etaOC);
                    if(rejList!=NULL) rejList->append(ocList.at(i));
                    ocList.removeAt(i);
                }
            }
            num1 = ocList.size();

            if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("\nnum0= %1\t\tnum1= %2\n").arg(num0).arg(num1);
            //s3Stm << QString("\nnum0= %1\t\tnum1= %2\n").arg(num0).arg(num1);


    }while(abs(num0-num1)>(proofP*num1));

    //s3File.close();
}

int objResidualFile::countCols(QList <colRec*> *colList, QString colNums)
{
    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("\nobjResidualFile::countCols: %1\n").arg(colNums);
    QStringList columns = colNums.split(",");
    int i, sz;
if(ocList.size()<2) return 1;
    colList->clear();
    colRec *cRec;
    cRec = new colRec;

    sz = columns.count();

    for(i=0;i<sz;i++)
    {
            if(!getColNum(cRec, columns[i].toInt()))
            {
                colList->append(cRec);
                cRec = new colRec;
            }
    }

    //sz1 = cols.size();
    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("cols.size= %1\n").arg(sz);

    return 0;
}

int objResidualFile::getColNum(colRec* cRec, int cNum)
{
    int i, sz;
    double val;

    sz = ocList.size();
    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("ocList.size= %1\n").arg(sz);
    double *vect = new double[sz];
    cRec->colNum = cNum;
    int num = 0;

    for(i=0; i<sz; i++)
    {

        switch(cNum)
        {
                case 0:
                val= ocList[i]->mJD;
                //cols[k]->num++;
                break;
                case 1:
                val= ocList[i]->ra;
                //cols[k]->num++;
                break;
                case 2:
                val= ocList[i]->de;
                //cols[k]->num++;
                break;
                case 3:
                val= ocList[i]->ksi;
                //cols[k]->num++;
                break;
                case 4:
                val= ocList[i]->eta;
                //cols[k]->num++;
                break;
                case 5:
                val= ocList[i]->mag;
                //cols[k]->num++;
                break;
                case 6:
                val= ocList[i]->ksiOC;
                //cols[k]->num++;
                break;
                case 7:
                val= ocList[i]->etaOC;
                //cols[k]->num++;
                break;
                case 8:
                val= ocList[i]->magOC;
                //cols[k]->num++;
                break;
                case 9:
                val= ocList[i]->x;
                //cols[k]->num++;
                break;
                case 10:
                val= ocList[i]->y;
                //cols[k]->num++;
                break;
                case 11:
                val= ocList[i]->pixmag;
                //cols[k]->num++;
                break;
                case 12:
                val= ocList[i]->Dx;
                //cols[k]->num++;
                break;
                case 13:
                val= ocList[i]->Dy;
                //cols[k]->num++;
                break;
                case 14:
                val= ocList[i]->Dpixmag;
                //cols[k]->num++;
                break;
        default:
                if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nobjResidualFile::countCols: Wrong col num\n";
                return 1;

            }
            if(isfinite(val))
            {
                vect[num] = val;
                num++;
            }
        }

    int res = countColStat(cRec, vect, num);

    delete [] vect;

    return res;
}

int objResidualFile::countMoveModel(moveModelRec* mmRec, int fp, int ft, int vflag, int modelDeg)
{
    int sz, i;
    double t;

    //delMMrec();
    sz = ocList.size();

    if(sz<=modelDeg) return 1;


    double *X = new double[sz];
    double *Y = new double[sz];
    double *Weights = new double[sz];
    double *C = new double[sz*modelDeg];
    double *Dx = new double[modelDeg*modelDeg];
    double *Dy = new double[modelDeg*modelDeg];
    double *Zx = new double[modelDeg];
    double *Zy = new double[modelDeg];
    double *uZx = new double[modelDeg];
    double *uZy = new double[modelDeg];
    int *excX = new int [sz];
    int *excY = new int [sz];
    double uweX, uweY;
    int rnX, rnY;

    double mt = 0,mxdot=0,mydot=0;
    QVector<double> times;
    for (i=0;i<sz;i++)
    {
            switch(ft)
            {
                    case 0:{t = ocList.at(i)->mJD;break;}
            }
            mt = mt + t;
            times << t;
            if(vflag==1)
            {
                    //elS = dataLine.section('|',xdc,xdc);
//                        mxdot = mxdot + ocList.at(j)->muRaCosDe;// elS.toDouble();
                    //elS = dataLine.section('|',ydc,ydc);
//                        mydot = mydot + ocList.at(j)->muDe;// elS.toDouble();
            }
    }
    mt = mt/(sz*1.0);//dataLines.count();
    if(vflag==1)
    {
            mxdot = mxdot/(1.0*sz);//dataLines.count();
            mydot = mydot/(1.0*sz);//dataLines.count();
    }
    for (i=0;i<sz;i++)
    {
            Weights[i]=1.0;
            //dataLine = dataLines[j];
            switch(fp)
            {
                    case 0:{X[i] = ocList.at(i)->ra;Y[i]=ocList.at(i)->de;break;}
                        default:{X[i] = ocList.at(i)->ra;Y[i]=ocList.at(i)->de;break;}

            }
            C[i*2] = 1;C[i*2+1] = times[i]-mt;
    }
//						matrix<double> Zx = LSM_corr(X, C, Weights, 500);
//						matrix<double> Zy = LSM_corr(Y, C, Weights, 500);
    uweX = uweY = 0.0;
    iLSM(modelDeg, sz, 500, excX, Zx, C, X, uweX, Dx, 3, rnX, Weights);
    iLSM(modelDeg, sz, 500, excY, Zy, C, Y, uweY, Dy, 3, rnY, Weights);

    if(rnX<=modelDeg||rnY<=modelDeg) return 2;

/*        lsm(2, sz, Zx, C, X, uweX, Dx, Weights);
    lsm(2, sz, Zy, C, Y, uweY, Dy, Weights);
    rnX = rnY = sz;
*/
    //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("Zx: %1 %2\tuweX: %3\n").arg(Zx[0]).arg(Zx[1]/1440.0).arg(uweX);
    //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("Zy: %1 %2\tuweY: %3\n").arg(Zy[0]).arg(Zy[1]/1440.0).arg(uweY);
    uZx[0] = sqrt(uweX*Dx[0]);
    uZx[1] = sqrt(uweX*Dx[3]);
    uZy[0] = sqrt(uweY*Dy[0]);
    uZy[1] = sqrt(uweY*Dy[3]);
//              slsm(2, int h, double *const z, const double* c, const double* L, const double* W = 0);
    //END apparent motion parameters
    //BEGIN output

    //mmRec = new moveModelRec;
    mmRec->Tm = mt;
    mmRec->xTm = Zx[0];
    mmRec->yTm = Zy[0];
    mmRec->nPosX = rnX;
    mmRec->uweX = uweX;
    mmRec->rmsX = uZx[0];
    mmRec->Xdot = Zx[1]*150000;//mas/min
    mmRec->rmsXdot = uZx[1]*150000;
    mmRec->XdotOC = Zx[1]*150000 - mxdot;
    mmRec->nPosY = rnY;
    mmRec->uweY = uweY;
    mmRec->rmsY = uZy[0];
    mmRec->Ydot = Zy[1]*150000;
    mmRec->rmsYdot = uZy[1]*150000;
    mmRec->YdotOC = Zy[1]*150000 - mydot;

    mmRec->fp = fp;
    mmRec->ft = ft;
    mmRec->vflag = vflag;

    return 0;
}

/*
void objResidualFile::getSeriesRec(objResRec *orsRec)
{
    countCols("0,1,2,3,4,5,6,7,8,9,10,11,12,13,14");
    sortColList();
    //countMM();
    //if(REDSTAT_LOG_LEVEL>0) qDebug() << "mmRec: " << mmRec << "\n";

    colRec* cRec;


    cRec = getColNum(0);
    if(cRec!=NULL) orsRec->mJD = cRec->mean;
    else orsRec->mJD = 0.0;

    cRec = getColNum(1);
    if(cRec!=NULL) orsRec->ra = cRec->mean;
    else  orsRec->ra = 0.0;

    cRec = getColNum(2);
    if(cRec!=NULL) orsRec->de = cRec->mean;
    else orsRec->de = 0.0;

    cRec = getColNum(3);
    if(cRec!=NULL) orsRec->ksi = cRec->mean;
    else orsRec->ksi = 0.0;

    cRec = getColNum(4);
    if(cRec!=NULL) orsRec->eta = cRec->mean;
    else orsRec->eta = 0.0;

    cRec = getColNum(5);
    if(cRec!=NULL) orsRec->mag = cRec->mean;
    else orsRec->mag = 0.0;

    cRec = getColNum(6);
    if(cRec!=NULL) orsRec->ksiOC = cRec->mean;
    else orsRec->ksiOC = 0.0;

    cRec = getColNum(7);
    if(cRec!=NULL) orsRec->etaOC = cRec->mean;
    else orsRec->etaOC = 0.0;

    cRec = getColNum(8);
    if(cRec!=NULL) orsRec->magOC = cRec->mean;
    else orsRec->magOC = 0.0;

    cRec = getColNum(9);
    if(cRec!=NULL) orsRec->x = cRec->mean;
    else orsRec->x = 0.0;

    cRec = getColNum(10);
    if(cRec!=NULL) orsRec->y = cRec->mean;
    else orsRec->y = 0.0;

    cRec = getColNum(11);
    if(cRec!=NULL) orsRec->pixmag = cRec->mean;
    else orsRec->pixmag = 0.0;

    cRec = getColNum(12);
    if(cRec!=NULL) orsRec->Dx = cRec->mean;
    else orsRec->Dx = 0.0;

    cRec = getColNum(13);
    if(cRec!=NULL) orsRec->Dy = cRec->mean;
    else orsRec->Dy = 0.0;

    cRec = getColNum(14);
    if(cRec!=NULL) orsRec->Dpixmag = cRec->mean;
    else orsRec->Dpixmag = 0.0;

    //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("\n19 mean\n");
/*
    if(mmRec!=NULL)
    {
        if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("ra: %1\tmmRa: %2\n").arg(eqsRec->ra).arg(mmRec->xTm);
        if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("de: %1\tmmDe: %2\n").arg(eqsRec->de).arg(mmRec->yTm);
        eqsRec->ocRaMM = grad_to_mas(eqsRec->ra - mmRec->xTm);
        eqsRec->ocDeMM = grad_to_mas(eqsRec->de - mmRec->yTm);
    }
    else
    {
        eqsRec->ocRaMM = 0.0;
        eqsRec->ocDeMM = 0.0;
    }
/
    int i, szi;
    szi = ocList.size();

    orsRec->name=QString("");
    orsRec->catName=QString("");
    orsRec->catMagName=QString("");
    orsRec->mesureTimeCode=QString("");

    for(i=0; i<szi; i++)
    {
        if((orsRec->name=="")||(orsRec->name==ocList.at(i)->name))orsRec->name=ocList.at(i)->name;
        else orsRec->name=QString("var");

        if((orsRec->catName=="")||(orsRec->catName==ocList.at(i)->catName))orsRec->catName=ocList.at(i)->catName;
        else orsRec->catName=QString("var");

        if((orsRec->catMagName=="")||(orsRec->catMagName==ocList.at(i)->catMagName))orsRec->catMagName=ocList.at(i)->catMagName;
        else orsRec->catMagName=QString("var");
    }
}
*/
moveModelFile::moveModelFile()
{
    mmList.clear();
}

moveModelFile::~moveModelFile()
{
    mmList.clear();
}

int moveModelFile::saveAs(QString fileName)
{
    QString tstr;

    QFile dataFile(fileName);
    dataFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
    QTextStream dataStream;
    dataStream.setDevice(&dataFile);

    int i, sz;

    sz = mmList.size();
    for(i=0;i<sz;i++)
    {
            mmList[i]->rec2s(&tstr);
            dataStream << tstr << "\n";
    }
    dataFile.close();
}
///////////////////////////////////////////////////////

objSeries::objSeries()
{
    serieList.clear();
}

objSeries::~objSeries()
{
    serieList.clear();
}

int objSeries::saveAs_Full(QString fileName)
{
    int i, sz, j, sz1;
    objResidualFile* orTemp;
    sz = serieList.size();
    QString tStr;
    QStringList tStrL;
    QList <colRec*> colList;

    QFile dataFile(fileName);
    dataFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
    QTextStream dataStream;
    dataStream.setDevice(&dataFile);

    for(i=0; i<sz; i++)
    {
        dataStream << "\n##################################\n";
        tStr.clear();
        tStrL.clear();
        orTemp = serieList.at(i);

        sz1 = orTemp->ocList.size();
        for(j=0;j<sz1;j++)
        {
            orTemp->ocList.at(j)->rec2s(&tStr);
            dataStream << tStr << "\n";
        }

        colList.clear();
        tStr.clear();
        tStrL.clear();
        orTemp->countCols(&colList, "0,1,2,3,4,5,6,7,8,9,10,11,12,13,14");
        sz1 = colList.size();
        for(j=0; j<sz1; j++)
        {
            colList.at(j)->rec2s(&tStr);
            dataStream << tStr << "\n";
        }

        //orTemp->colList.clear();
       // orTemp->countCols("0,1,2,3,4,5,6,7,8,9,10,11,12,13,14");
    }
    dataFile.close();
}

int objSeries::saveAs_Mean(QString fileName)
{
    int i, sz, j, sz1;
    objResidualFile* orTemp;
    sz = serieList.size();
    QString tStr;
    QStringList tStrL;
    QList <colRec*> colList;
    objResRec orRec;

    QFile dataFile(fileName);
    dataFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
    QTextStream dataStream;
    dataStream.setDevice(&dataFile);

    for(i=0; i<sz; i++)
    {
        tStr.clear();
        tStrL.clear();
        orTemp = serieList.at(i);
        colList.clear();

        if(!orTemp->countCols(&colList, "0,1,2,3,4,5,6,7,8,9,10,11,12,13,14"))
        {
            sz1 = colList.size();

            if(sz1)
            {
                orRec.clear();
                orRec.fromColList(colList);

                tStrL << QString("%1").arg(orTemp->ocList.at(0)->name) << QString("%1").arg(orTemp->ocList.at(0)->catMagName) << QString("%1").arg(orTemp->ocList.at(0)->catName);
                dataStream << tStrL.join("|") << "\n";
            }
        }
        //orTemp->colList.clear();
       // orTemp->countCols("0,1,2,3,4,5,6,7,8,9,10,11,12,13,14");
    }
    dataFile.close();
}

int objSeries::saveAs_MoveModel(QString fileName)
{
    int i, sz;
    objResidualFile* orTemp;
    sz = serieList.size();
    QString tStr;
    ocRec ocR;

    moveModelRec mmRec;
    //mmRec = new moveModelRec;

    QFile dataFile(fileName);
    dataFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
    QTextStream dataStream;
    dataStream.setDevice(&dataFile);

    for(i=0; i<sz; i++)
    {
        tStr.clear();

        orTemp = serieList.at(i);
        //dataStream << tStrL.join("|") << "\n";

        if(!orTemp->countMoveModel(&mmRec))
        {
            //orRec.clear();
            ocR.MJday = mmRec.Tm;
            ocR.ra = mmRec.xTm;
            ocR.de = mmRec.yTm;
            ocR.muRaCosDe = mmRec.Xdot;
            ocR.muDe = mmRec.Ydot;
            ocR.rec2MPC(&tStr, "084", 0);

        //mmRec.rec2s(&tStr);
            dataStream << tStr << "\n";// << QString("|%1|%2|%3").arg(orTemp->ocList.at(0)->name).arg(orTemp->ocList.at(0)->catMagName).arg(orTemp->ocList.at(0)->catName) << "\n";
        }
        //orTemp->colList.clear();
       // orTemp->countCols("0,1,2,3,4,5,6,7,8,9,10,11,12,13,14");
    }
    dataFile.close();
}

void objSeries::getMean(QList <objResRec*> *ocList)
{
    int i, sz, sz1;
    objResidualFile* orTemp;
    sz = serieList.size();
    QList <colRec*> colList;
    objResRec *orRec;

    for(i=0; i<sz; i++)
    {
        orTemp = serieList.at(i);

        colList.clear();

        if(!orTemp->countCols(&colList, "0,1,2,3,4,5,6,7,8,9,10,11,12,13,14"))
        {
            sz1 = colList.size();

            if(sz1)
            {
                orRec = new objResRec;
                orRec->fromColList(colList);
                orRec->name = objName;
                orRec->catName = catName;
                orRec->catMagName = catMagName;
                ocList->append(orRec);
            }
        }
        else ocList->append(orTemp->ocList.at(0));
    }
}

void objSeries::getMoveModel(QList <ocRec*> *ocList)
{
    int i, sz, sz1;
    objResidualFile* orTemp;
    sz = serieList.size();
    ocRec *ocR;

    moveModelRec mmRec;
    colRec cRec;
    objResRec orRec;
    QList <colRec*> colList;

    for(i=0; i<sz; i++)
    {
        orTemp = serieList.at(i);
        ocR = new ocRec;
        if(!orTemp->countMoveModel(&mmRec))
        {

            ocR->MJday = mmRec.Tm;
            ocR->ra = mmRec.xTm;
            ocR->de = mmRec.yTm;
            ocR->muRaCosDe = mmRec.Xdot;
            ocR->muDe = mmRec.Ydot;

            if(!orTemp->getColNum(&cRec, 5));
            {
                ocR->mag0 = cRec.mean;
            }


        }
        else if(!orTemp->countCols(&colList, "0,1,2,3,4,5,6,7,8,9,10,11,12,13,14"))
        {
            sz1 = colList.size();

            if(sz1)
            {

                orRec.fromColList(colList);
                orRec.toOcRec(ocR);

            }
        }
        else
        {
            orTemp->ocList.at(0)->toOcRec(ocR);
        }

        ocList->append(ocR);
    }
}


///////////////////////////////////////////////////////

/*
int objResidualFile::getColRecNum(int colNum)
{
//	if(colRec==NULL) return -1;
        int i;
        int csz = colList.size();

        for(i=0; i<csz; i++)
        {
                if(colList[i]->colNum==colNum)
                {
//			cRec = colList[i];
                        return i;
                }
        }

        return -1;
}

int objResidualFile::setColRec(colRec* cRec)
{
        colRec *pRec;
        int rpos = getColRecNum(cRec->colNum);
//	//if(REDSTAT_LOG_LEVEL>0) qDebug() << "rpos= " << rpos << "\n";
        if(rpos==-1)
        {
                pRec = new colRec;
                colList << pRec;
                rpos = colList.size()-1;
        }

        colList[rpos]->colNum = cRec->colNum;
        colList[rpos]->num = cRec->num;
        colList[rpos]->mean = cRec->mean;
        colList[rpos]->rmsMean = cRec->rmsMean;
        colList[rpos]->rmsOne = cRec->rmsOne;

        return 0;
}

void objResidualFile::sortColList()
{
        int sz = colList.size();
        int nMin, pMin;

        for(int i=0; i<sz-1; i++)
        {
                nMin = colList[i]->colNum;
                pMin = i;
                for(int j=i+1; j<sz; j++)
                {
                        if(colList[j]->colNum<nMin){pMin = j; nMin = colList[j]->colNum;}
                }

                colList.swap(i, pMin);

        }
}

int objResidualFile::countCols(QString colNums)
{
    colList.clear();
    QStringList columns = colNums.split(",");
    int i, j, k, sz, sz1;
    QList <colRec *> cols;
    colRec *cRec;
    QString str;

    sz = ocList.size();
    if(sz<=2) return 1;

            sz1 = columns.count();
            for(j=0;j<sz1;j++)
            {
                    str = columns[j];
                    cRec = new colRec;
                    cRec->colNum = str.toInt();
    //							means<<0;
    //							serrors<<0;
                    cols << cRec;
            }


            for(j=0;j<sz;j++)
            {
                    for(k=0;k<sz1;k++)
                    {
                            switch(cols[k]->colNum)
                            {
                                    case 0:
                                    cols[k]->mean += ocList[j]->mJD;
                                    cols[k]->num++;
                                    break;
                                    case 1:
                                    cols[k]->mean += ocList[j]->ra;
                                    cols[k]->num++;
                                    break;
                                    case 2:
                                    cols[k]->mean += ocList[j]->de;
                                    cols[k]->num++;
                                    break;
                                    case 3:
                                    cols[k]->mean += ocList[j]->ksi;
                                    cols[k]->num++;
                                    break;
                                    case 4:
                                    cols[k]->mean += ocList[j]->eta;
                                    cols[k]->num++;
                                    break;
                                    case 5:
                                    cols[k]->mean += ocList[j]->mag;
                                    cols[k]->num++;
                                    break;
                                    case 6:
                                    cols[k]->mean += ocList[j]->ksiOC;
                                    cols[k]->num++;
                                    break;
                                    case 7:
                                    cols[k]->mean += ocList[j]->etaOC;
                                    cols[k]->num++;
                                    break;
                                    case 8:
                                    cols[k]->mean += ocList[j]->magOC;
                                    cols[k]->num++;
                                    break;
                                    case 9:
                                    cols[k]->mean += ocList[j]->x;
                                    cols[k]->num++;
                                    break;
                                    case 10:
                                    cols[k]->mean += ocList[j]->y;
                                    cols[k]->num++;
                                    break;
                                    case 11:
                                    cols[k]->mean += ocList[j]->pixmag;
                                    cols[k]->num++;
                                    break;
                                    case 12:
                                    cols[k]->mean += ocList[j]->Dx;
                                    cols[k]->num++;
                                    break;
                                    case 13:
                                    cols[k]->mean += ocList[j]->Dy;
                                    cols[k]->num++;
                                    break;

                                    case 14:
                                    cols[k]->mean += ocList[j]->Dpixmag;
                                    cols[k]->num++;
                                    break;

                            }
                    }
            }
            for(k=0;k<sz1;k++) cols[k]->mean /= cols[k]->num;

            for(j=0;j<sz;j++)
            {
                    for(k=0;k<sz1;k++)
                    {
                            switch(cols[k]->colNum)
                            {
                                    case 0:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->mJD, 2.0);
                                    break;
                                    case 1:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->ra, 2.0);
                                    break;
                                    case 2:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->de, 2.0);
                                    break;
                                    case 3:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->ksi, 2.0);
                                    break;
                                    case 4:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->eta, 2.0);
                                    break;
                                    case 5:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->mag, 2.0);
                                    break;
                                    case 6:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->ksiOC, 2.0);
                                    break;
                                    case 7:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->etaOC, 2.0);
                                    break;
                                    case 8:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->magOC, 2.0);
                                    break;
                                    case 9:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->x, 2.0);
                                    break;
                                    case 10:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->y, 2.0);
                                    break;
                                    case 11:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->pixmag, 2.0);
                                    break;
                                    case 12:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->Dx, 2.0);
                                    break;
                                    case 13:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->Dy, 2.0);
                                    break;
                                    case 14:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->Dpixmag, 2.0);
                                    break;
                            }
                    }
            }

            for(k=0;k<sz1;k++)
            {
                    cols[k]->rmsOne = sqrt(cols[k]->rmsOne/(cols[k]->num-1.0));
                    cols[k]->rmsMean = cols[k]->rmsOne/sqrt(cols[k]->num);
                    str.clear();
                    cols[k]->rec2s(&str);
//                    //if(REDSTAT_LOG_LEVEL>0) qDebug() << k << ": " << str << "\n";
                    setColRec(cols[k]);
            }

    sortColList();
    return 0;
}

int objResidualFile::reCountCols()
{
    QStringList colNums;
    int i, sz;
    sz = colList.size();

    for(i=0; i<sz; i++)
    {
        colNums << QString("%1").arg(colList[i]->colNum);
    }
    if(countCols(colNums.join(","))) return 1;
    return 0;
}

colRec* objResidualFile::getColNum(int cNum)
{
    int i, sz;
    sz = colList.size();
    for(i=0; i<sz; i++)
    {
        if(cNum==colList[i]->colNum) return colList[i];
    }
    return NULL;
}
*/
//////////////////////////////////////////////////////////////////////

errBudgetFile::errBudgetFile()
{
    errList.clear();
};

errBudgetFile::~errBudgetFile()
{
        errList.clear();
};

void errBudgetFile::init(const char *fname)
{
    if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nerrBudgetFile::init: " << fname << "\n";
	errList.clear();

        fName.clear();
        fName.insert(0, fname);
	
	QString dataLine;
	QFile dataFile(fname);
	dataFile.open(QIODevice::ReadWrite| QIODevice::Text);
	QTextStream dataStream;
	dataStream.setDevice(&dataFile);
	
	errBudgetRec* errB;
	
	while (!dataStream.atEnd())
	{
		dataLine = dataStream.readLine();
                errB = new errBudgetRec;
                if(!errB->s2rec(dataLine))errList << errB;
	}
	
};

void errBudgetFile::saveAs(QString fname)
{
        fName.clear();
        fName.insert(0, fname);
        save();
};

void errBudgetFile::save()
{
        QString tstr;

        QFile dataFile(fName);
        dataFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
        QTextStream dataStream;
        dataStream.setDevice(&dataFile);

        int i, sz;

        sz = errList.size();
        for(i=0;i<sz;i++)
        {
                errList[i]->rec2s(&tstr);
                dataStream << tstr << "\n";
        }
};

void errBudgetFile::removeMes(QString mesureTimeCode)
{
    int i, szi;
    szi = errList.size();
    for(i=szi-1; i>=0; i--)
    {
        if(QString::compare(mesureTimeCode, errList.at(i)->mesureTimeCode)==0) errList.removeAt(i);
    }
}

//////////////////////////////////////////////////////////////////////

int mpephRec::fromString(QString inStr)
{
    QString elS;
    QStringList opers;
    opers = inStr.split("|");
    //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("opers.size()= %1\n").arg(opers.size());
    if(opers.size()<11) return 1;
    elS = opers.at(0);ra = hms_to_deg(elS, ":");
    elS = opers.at(1);de = damas_to_deg(elS, ":");
    elS = opers.at(3);Vmag = elS.toDouble();
    elS = opers.at(2);topDist=elS.toDouble();//topocentric distance from theory of motion or catalogue
    elS = opers.at(6);muRaCosDe=elS.toDouble();//mu_ra*cos(dec) [mas/min]
    elS = opers.at(7);muDe=elS.toDouble();//mu_de [mas/min]
    elS = opers.at(8);Vr=elS.toDouble();//v_r [km/s]
    elS = opers.at(4);phase=elS.toDouble();//phase [deg]
    elS = opers.at(5);elong=elS.toDouble();//elongation [deg]
    elS = opers.at(9);num=QString(elS);//object number
    elS = opers.at(10);name=QString(elS);//object name
    ////if(REDSTAT_LOG_LEVEL>0) qDebug() << "end\n";
    return 0;
}

int mpephRec::fromMiriStr(QString inStr)
{
    QString elS;
    QStringList opers;
    opers = inStr.split(" ", QString::SkipEmptyParts);
    //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("opers.size()= %1\n").arg(opers.size());
    if(opers.size()<14) return 1;
    hms_to_deg(&ra, QString("%1:%2:%3").arg(opers.at(1)).arg(opers.at(2)).arg(opers.at(3)), ":");
    damas_to_deg(&de, QString("%1:%2:%3").arg(opers.at(4)).arg(opers.at(5)).arg(opers.at(6)), ":");

    elS = opers.at(7);topDist=elS.toDouble();//topocentric distance from theory of motion or catalogue
    elS = opers.at(8);Vmag = elS.toDouble();
    elS = opers.at(9);phase=elS.toDouble();//phase [deg]
    elS = opers.at(10);elong=elS.toDouble();//elongation [deg]
    elS = opers.at(11);muRaCosDe=elS.toDouble()*1000.0*60.0;//mu_ra*cos(dec) [mas/min]
    elS = opers.at(12);muDe=elS.toDouble()*1000.0*60.0;//mu_de [mas/min]
    elS = opers.at(13);Vr=elS.toDouble();//v_r [km/s]

    elS = opers.at(9);num=QString(elS);//object number
    elS = opers.at(10);name=QString(elS);//object name
    ////if(REDSTAT_LOG_LEVEL>0) qDebug() << "end\n";
    return 0;
}

void mpephRec::copyTo(mpephRec *targ)
{
    if(targ==NULL) return;
    targ->de = de;
    targ->elong = elong;
    targ->muDe = muDe;
    targ->muRaCosDe = muRaCosDe;
    targ->name = name;
    targ->num = num;
    targ->phase = phase;
    targ->ra = ra;
    targ->topDist = topDist;
    targ->Vmag = Vmag;
    targ->Vr = Vr;
}

//////////////////////////////////////////////////////////////////////

int skybotRec::fromString(QString iStr)
{
    QString elS;
    QStringList opers;
    opers = iStr.split("|");
    //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("opers.size()= %1\n").arg(opers.size());
    if(opers.size()<21) return 1;
    Num = opers.at(0).simplified();
    Name = opers.at(1).simplified();
    elS = opers.at(2).simplified();RA = elS.toDouble()*15.0;
    elS = opers.at(3).simplified();DE = elS.toDouble();
    Class = opers.at(4).simplified();
    elS = opers.at(5).simplified();Mv = elS.toDouble();
    elS = opers.at(6).simplified();Err=elS.toDouble();
    elS = opers.at(7).simplified();d=elS.toDouble();
    elS = opers.at(8).simplified();dRA=elS.toDouble();
    elS = opers.at(9).simplified();dDEC=elS.toDouble();
    elS = opers.at(10).simplified();Dg=elS.toDouble();
    elS = opers.at(11).simplified();Dh=elS.toDouble();
    elS = opers.at(12).simplified();Phase=elS.toDouble();
    elS = opers.at(13).simplified();SunElong=elS.toDouble();
    elS = opers.at(14).simplified();x=elS.toDouble();
    elS = opers.at(15).simplified();y=elS.toDouble();
    elS = opers.at(16).simplified();z=elS.toDouble();
    elS = opers.at(17).simplified();vx=elS.toDouble();
    elS = opers.at(18).simplified();vy=elS.toDouble();
    elS = opers.at(19).simplified();vz=elS.toDouble();;
    elS = opers.at(12).simplified();Epoch=elS.toDouble();


    return 0;
}



void skybotRec::toString(QString *oStr)
{
    QStringList resList;

    resList << Num;
    resList << Name;
    resList << QString("%1").arg(RA/15.0);
    resList << QString("%1").arg(DE);
    resList << Class;
    resList << QString("%1").arg(Mv, 4, 'f', 1);
    resList << QString("%1").arg(Err, 8, 'f', 3);
    resList << QString("%1").arg(d, 8, 'f', 3);
    resList << QString("%1").arg(dRA, 8, 'f', 3);
    resList << QString("%1").arg(dDEC, 8, 'f', 3);
    resList << QString("%1").arg(Dg, 14, 'f', 12);
    resList << QString("%1").arg(Dh, 14, 'f', 12);
    resList << QString("%1").arg(Phase, 6, 'f', 2);
    resList << QString("%1").arg(SunElong, 6, 'f', 2);
    resList << QString("%1").arg(x, 12, 'f', 9);
    resList << QString("%1").arg(y, 12, 'f', 9);
    resList << QString("%1").arg(y, 12, 'f', 9);
    resList << QString("%1").arg(vx, 12, 'f', 9);
    resList << QString("%1").arg(vy, 12, 'f', 9);
    resList << QString("%1").arg(vz, 12, 'f', 9);
    resList << QString("%1").arg(Epoch, 7, 'f', 0);
}

void skybotRec::copy(const skybotRec &source)
{
    Num = source.Num;
    Name = source.Name;
    RA = source.RA;
    DE = source.DE;
    Class = source.Class;
    Mv = source.Mv;
    Err = source.Err;
    d = source.d;
    dRA = source.dRA;
    dDEC = source.dDEC;
    Dg = source.Dg;
    Dh = source.Dh;
    Phase = source.Phase;
    SunElong = source.SunElong;
    x = source.x;
    y = source.y;
    z = source.z;
    vx = source.vx;
    vy = source.vy;
    vz = source.vz;
    Epoch = source.Epoch;
}

///////////////////////////////////////////////////////////////////////////////////////////


sstarFile::sstarFile()
{
}

sstarFile::~sstarFile()
{

    ocList.clear();
    colList.clear();
}

void sstarFile::clear()
{
        ocList.clear();
        colList.clear();

}

int sstarFile::init(QString fileName)
{
    if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nsstarFile::init:" << fileName << "\n";
    QFile inFile(fileName);
    inFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream indata(&inFile);
    sstarRes* tRec;
    QString tStr;
    tRec = new sstarRes;
    while(!indata.atEnd())
    {
        tStr = indata.readLine();
        if(tRec->s2rec(tStr)) continue;
        ocList << tRec;
        tRec = new sstarRes;
    }
    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("\nocList.size= %1\n").arg(ocList.size());
}

void sstarFile::save()
{
        QString tstr;
        sortColList();
        QFile dataFile(fName);
        dataFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
        QTextStream dataStream;
        dataStream.setDevice(&dataFile);

        int i, sz;

        sz = ocList.size();
        for(i=0;i<sz;i++)
        {
                tstr.clear();
                ocList[i]->rec2s(&tstr);
                dataStream << tstr << "\n";
        }

        sz = colList.size();
        for(i=0;i<sz;i++)
        {
                tstr.clear();
                colList[i]->rec2s(&tstr);
                dataStream << tstr << "\n";
        }
        /*if(mmRec!=NULL)
        {
                tstr.clear();
                mmRec->rec2s(&tstr);
                dataStream << tstr << "\n";
        }*/

        dataFile.close();
}

void sstarFile::saveAs(QString fname)
{
        sortColList();
        fName.clear();
        fName.insert(0, fname);
        save();
}

int sstarFile::getColRecNum(int colNum)
{
//	if(colRec==NULL) return -1;
        int i;
        int csz = colList.size();

        for(i=0; i<csz; i++)
        {
                if(colList[i]->colNum==colNum)
                {
//			cRec = colList[i];
                        return i;
                }
        }

        return -1;
}

int sstarFile::setColRec(colRec* cRec)
{
        colRec *pRec;
        int rpos = getColRecNum(cRec->colNum);
//	//if(REDSTAT_LOG_LEVEL>0) qDebug() << "rpos= " << rpos << "\n";
        if(rpos==-1)
        {
                pRec = new colRec;
                colList << pRec;
                rpos = colList.size()-1;
        }

        colList[rpos]->colNum = cRec->colNum;
        colList[rpos]->num = cRec->num;
        colList[rpos]->mean = cRec->mean;
        colList[rpos]->rmsMean = cRec->rmsMean;
        colList[rpos]->rmsOne = cRec->rmsOne;

        return 0;
}

void sstarFile::sortColList()
{
        int sz = colList.size();
        int nMin, pMin;

        for(int i=0; i<sz-1; i++)
        {
                nMin = colList[i]->colNum;
                pMin = i;
                for(int j=i+1; j<sz; j++)
                {
                        if(colList[j]->colNum<nMin){pMin = j; nMin = colList[j]->colNum;}
                }

                colList.swap(i, pMin);

        }
}

int sstarFile::countCols(QString colNums)
{
    colList.clear();
    QStringList columns = colNums.split(",");
    int i, j, k, sz, sz1;
    QList <colRec *> cols;
    colRec *cRec;
    QString str;

    sz = ocList.size();
    if(sz<=2) return 1;

            sz1 = columns.count();
            for(j=0;j<sz1;j++)
            {
                    str = columns[j];
                    cRec = new colRec;
                    cRec->colNum = str.toInt();
    //							means<<0;
    //							serrors<<0;
                    cols << cRec;
            }


            for(j=0;j<sz;j++)
            {
                    for(k=0;k<sz1;k++)
                    {
                            switch(cols[k]->colNum)
                            {
                                    case 0:
                                    cols[k]->mean += ocList[j]->mJD;
                                    cols[k]->num++;
                                    break;
                                    case 1:
                                    cols[k]->mean += ocList[j]->ra;
                                    cols[k]->num++;
                                    break;
                                    case 2:
                                    cols[k]->mean += ocList[j]->de;
                                    cols[k]->num++;
                                    break;
                                    case 3:
                                    cols[k]->mean += ocList[j]->ksi;
                                    cols[k]->num++;
                                    break;
                                    case 4:
                                    cols[k]->mean += ocList[j]->eta;
                                    cols[k]->num++;
                                    break;
                                    case 5:
                                    cols[k]->mean += ocList[j]->mag;
                                    cols[k]->num++;
                                    break;
                                    case 6:
                                    cols[k]->mean += ocList[j]->ksiOC;
                                    cols[k]->num++;
                                    break;
                                    case 7:
                                    cols[k]->mean += ocList[j]->etaOC;
                                    cols[k]->num++;
                                    break;
                                    case 8:
                                    cols[k]->mean += ocList[j]->magOC;
                                    cols[k]->num++;
                                    break;
                                    case 9:
                                    cols[k]->mean += ocList[j]->x;
                                    cols[k]->num++;
                                    break;
                                    case 10:
                                    cols[k]->mean += ocList[j]->y;
                                    cols[k]->num++;
                                    break;
                                    case 11:
                                    cols[k]->mean += ocList[j]->pixmag;
                                    cols[k]->num++;
                                    break;
                                    case 12:
                                    cols[k]->mean += ocList[j]->Dx;
                                    cols[k]->num++;
                                    break;
                                    case 13:
                                    cols[k]->mean += ocList[j]->Dy;
                                    cols[k]->num++;
                                    break;

                                    case 14:
                                    cols[k]->mean += ocList[j]->Dpixmag;
                                    cols[k]->num++;
                                    break;

                            }
                    }
            }
            for(k=0;k<sz1;k++) cols[k]->mean /= cols[k]->num;

            for(j=0;j<sz;j++)
            {
                    for(k=0;k<sz1;k++)
                    {
                            switch(cols[k]->colNum)
                            {
                                    case 0:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->mJD, 2.0);
                                    break;
                                    case 1:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->ra, 2.0);
                                    break;
                                    case 2:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->de, 2.0);
                                    break;
                                    case 3:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->ksi, 2.0);
                                    break;
                                    case 4:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->eta, 2.0);
                                    break;
                                    case 5:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->mag, 2.0);
                                    break;
                                    case 6:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->ksiOC, 2.0);
                                    break;
                                    case 7:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->etaOC, 2.0);
                                    break;
                                    case 8:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->magOC, 2.0);
                                    break;
                                    case 9:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->x, 2.0);
                                    break;
                                    case 10:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->y, 2.0);
                                    break;
                                    case 11:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->pixmag, 2.0);
                                    break;
                                    case 12:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->Dx, 2.0);
                                    break;
                                    case 13:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->Dy, 2.0);
                                    break;
                                    case 14:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->Dpixmag, 2.0);
                                    break;
                            }
                    }
            }

            for(k=0;k<sz1;k++)
            {
                    cols[k]->rmsOne = sqrt(cols[k]->rmsOne/(cols[k]->num-1.0));
                    cols[k]->rmsMean = cols[k]->rmsOne/sqrt(cols[k]->num);
                    str.clear();
                    cols[k]->rec2s(&str);
//                    //if(REDSTAT_LOG_LEVEL>0) qDebug() << k << ": " << str << "\n";
                    setColRec(cols[k]);
            }

    sortColList();
    return 0;
}

int sstarFile::reCountCols()
{
    QStringList colNums;
    int i, sz;
    sz = colList.size();

    for(i=0; i<sz; i++)
    {
        colNums << QString("%1").arg(colList[i]->colNum);
    }
    if(countCols(colNums.join(","))) return 1;
    return 0;
}

colRec* sstarFile::getColNum(int cNum)
{
    int i, sz;
    sz = colList.size();
    for(i=0; i<sz; i++)
    {
        if(cNum==colList[i]->colNum) return colList[i];
    }
    return NULL;
}

void sstarFile::do3sigma(double proofP, double sigmaMul)
{
    int num0, num1, i, sz;
    int num0E, num1E;
    double meanRa, meanDec;
    double sigmaKsi, sigmaEta;
    double maxOCKsi, maxOCEta;

    num1 = ocList.size();

    if(num1<3) return;

    do
    {
            num0 = num1;

            if((getColRecNum(6)!=-1)&&(getColRecNum(7)!=-1)) reCountCols();
            else countCols("6,7");

            meanRa = getColNum(6)->mean;
            meanDec = getColNum(7)->mean;

            sigmaKsi = getColNum(6)->rmsOne;
            sigmaEta = getColNum(7)->rmsOne;

            //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("\nsigmaKsi= %1\t\tsigmaEta= %2\n").arg(sigmaKsi).arg(sigmaEta);

            maxOCKsi = sigmaMul*sigmaKsi;
            maxOCEta = sigmaMul*sigmaEta;


            sz = ocList.size();
            for(i=sz-1; i>=0; i--)
            {
                if((fabs(meanRa-ocList[i]->ksiOC)>maxOCKsi)||(fabs(meanDec-ocList[i]->etaOC)>maxOCEta)) ocList.removeAt(i);
            }
            num1 = ocList.size();

            if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("\nnum0= %1\t\tnum1= %2\n").arg(num0).arg(num1);


    }while(abs(num0-num1)>(proofP*num1));

}

void sstarFile::removeMes(QString mesureTimeCode)
{
    int i, szi;
    szi = ocList.size();
    for(i=szi-1; i>=0; i--)
    {
        if(QString::compare(mesureTimeCode, ocList.at(i)->mesureTimeCode)==0) ocList.removeAt(i);
    }
}
/*
int sstarFile::countMM(int fp, int ft, int vflag)
{
    int sz, i, j;
    double t;

    delMMrec();
    sz = ocList.size();

    if(sz<=2) return 1;


        double *X = new double[sz];
        double *Y = new double[sz];
        double *Weights = new double[sz];
        double *C = new double[sz*2];
        double *Dx = new double[2*2];
        double *Dy = new double[2*2];
        double *Zx = new double[2];
        double *Zy = new double[2];
        double *uZx = new double[2];
        double *uZy = new double[2];
        int *excX = new int [sz];
        int *excY = new int [sz];
        double uweX, uweY;
        int rnX, rnY;

        double mt = 0,mxdot=0,mydot=0;
        QVector<double> times;
        for (j=0;j<sz;j++)
        {
                switch(ft)
                {
                        case 0:{t = ocList.at(j)->MJday;break;}
                }
                mt = mt + t;
                times << t;
                if(vflag==1)
                {
                        //elS = dataLine.section('|',xdc,xdc);
                        mxdot = mxdot + ocList.at(j)->muRaCosDe;// elS.toDouble();
                        //elS = dataLine.section('|',ydc,ydc);
                        mydot = mydot + ocList.at(j)->muDe;// elS.toDouble();
                }
        }
        mt = mt/(sz*1.0);//dataLines.count();
        if(vflag==1)
        {
                mxdot = mxdot/(1.0*sz);//dataLines.count();
                mydot = mydot/(1.0*sz);//dataLines.count();
        }
        for (j=0;j<sz;j++)
        {
                Weights[j]=1.0;
                //dataLine = dataLines[j];
                switch(fp)
                {
                        case 0:{X[j] = ocList.at(j)->ra;Y[j]=ocList.at(j)->de;break;}
                            default:{X[j] = ocList.at(j)->ra;Y[j]=ocList.at(j)->de;break;}

                }
                C[j*2] = 1;C[j*2+1] = times[j]-mt;
        }
//						matrix<double> Zx = LSM_corr(X, C, Weights, 500);
//						matrix<double> Zy = LSM_corr(Y, C, Weights, 500);
        uweX = uweY = 0.0;
        iLSM(2, sz, 500, excX, Zx, C, X, uweX, Dx, 3, rnX, Weights);
        iLSM(2, sz, 500, excY, Zy, C, Y, uweY, Dy, 3, rnY, Weights);

        if(rnX<=2||rnY<=2) return 2;

/*        lsm(2, sz, Zx, C, X, uweX, Dx, Weights);
        lsm(2, sz, Zy, C, Y, uweY, Dy, Weights);
        rnX = rnY = sz;
/
        //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("Zx: %1 %2\tuweX: %3\n").arg(Zx[0]).arg(Zx[1]/1440.0).arg(uweX);
        //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("Zy: %1 %2\tuweY: %3\n").arg(Zy[0]).arg(Zy[1]/1440.0).arg(uweY);
        uZx[0] = sqrt(uweX*Dx[0]);
        uZx[1] = sqrt(uweX*Dx[3]);
        uZy[0] = sqrt(uweY*Dy[0]);
        uZy[1] = sqrt(uweY*Dy[3]);
//              slsm(2, int h, double *const z, const double* c, const double* L, const double* W = 0);
        //END apparent motion parameters
        //BEGIN output

        mmRec = new moveModelRec;
        mmRec->Tm = mt;
        mmRec->xTm = Zx[0];
        mmRec->yTm = Zy[0];
        mmRec->nPosX = rnX;
        mmRec->uweX = uweX;
        mmRec->rmsX = uZx[0];
        mmRec->Xdot = Zx[1]/1440.0;
        mmRec->rmsXdot = uZx[1]/1440.0;
        mmRec->XdotOC = Zx[1]/1440.0 - mxdot;
        mmRec->nPosY = rnY;
        mmRec->uweY = uweY;
        mmRec->rmsY = uZy[0];
        mmRec->Ydot = Zy[1]/1440.0;
        mmRec->rmsYdot = uZy[1]/1440.0;
        mmRec->YdotOC = Zy[1]/1440.0 - mydot;

        mmRec->fp = fp;
        mmRec->ft = ft;
        mmRec->vflag = vflag;

        return 0;

}
*/
///////////////////////////////////////////////////////////////////////////////////////////

reductionStat::reductionStat()
{
    ebFile = new errBudgetFile;
    resFile = new residualFile;
    objFile = new objResidualFile;
    //mpeFile = new eqFile;
    //ssFile = new sstarFile;
}

reductionStat::~reductionStat()
{
    delete ebFile;
    delete resFile;
    //delete mpeFile;
    //delete ssFile;
    delete objFile;
}

int reductionStat::init(QString *ebFileName, QString *resFileName, QString *objFileName)
{
    if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nreductionStat::init\n";
    if(ebFileName!=NULL)
    {
        //ebFile = new errBudgetFile;
        ebFile->init(ebFileName->toAscii().data());
    }

    if(resFileName!=NULL)
    {
        //resFile = new residualFile;
        resFile->init(resFileName->toAscii().data());
    }

    if(objFileName!=NULL)
    {
        //mpeFile = new eqFile;
        objFile->init(*objFileName);
    }

/*
    if(mpeFileName!=NULL)
    {
        //mpeFile = new eqFile;
        mpeFile->init(mpeFileName->toAscii().data());
    }

    if(ssFileName!=NULL)
    {
        //ssFile = new sstarFile;
        ssFile->init(ssFileName->toAscii().data());
    }
*/
    findMeasurements();

    return 0;
}

void reductionStat::getMeasurementsList(QStringList mesCodeList, reductionStat *selRed)
{
    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("reductionStat::getMeasurementsList\n");
    int i, szi;
    szi = mesCodeList.size();
    measurementRec* mesRec;

    if(selRed==NULL) return;

    for(i=0; i<szi; i++)
    {
        mesRec = getMeasurement(mesCodeList.at(i));

        if(selRed->ebFile!=NULL) selRed->ebFile->errList << mesRec->errBud;
        if(selRed->resFile!=NULL) selRed->resFile->resList << mesRec->resList;
        if(selRed->objFile!=NULL) selRed->objFile->ocList << mesRec->objList;
        //if(selRed->mpeFile!=NULL) selRed->mpeFile->ocList << mesRec->mpeList;
        //if(selRed->ssFile!=NULL) selRed->ssFile->ocList << mesRec->sstarList;
    }
}

void reductionStat::getMeasurementsList(QList <measurementRec*> mesList, reductionStat *selRed)
{
    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("reductionStat::getMeasurementsList\n");
    int i, szi;
    szi = mesList.size();
    measurementRec* mesRec;

    if(selRed==NULL) return;

    for(i=0; i<szi; i++)
    {
        mesRec = mesList.at(i);

        if(selRed->ebFile!=NULL) selRed->ebFile->errList << mesRec->errBud;
        if(selRed->resFile!=NULL) selRed->resFile->resList << mesRec->resList;
        if(selRed->objFile!=NULL) selRed->objFile->ocList << mesRec->objList;
        //if(selRed->mpeFile!=NULL) selRed->mpeFile->ocList << mesRec->mpeList;
        //if(selRed->ssFile!=NULL) selRed->ssFile->ocList << mesRec->sstarList;
    }
}

void reductionStat::saveMesList(QString ebFileName, QString resFileName, QString objFileName)
{
    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("reductionStat::saveMesList\n");
    ebFile->saveAs(ebFileName);
    resFile->saveAs(resFileName);
    if(objFileName!="") objFile->saveAs(objFileName);
    //if(mpeFileName!="") mpeFile->saveAs(mpeFileName);
    //if(ssFileName!="") ssFile->saveAs(ssFileName);
}

int reductionStat::removeMes(QString mesTimeCode)
{
    ebFile->removeMes(mesTimeCode);
    resFile->removeMes(mesTimeCode);
    objFile->removeMes(mesTimeCode);
    //mpeFile->removeMes(mesTimeCode);
    //ssFile->removeMes(mesTimeCode);
}

void reductionStat::removeMesList(QStringList mesList)
{
    for(int i=0; i<mesList.size();i++) removeMes(mesList.at(i));
}

void reductionStat::findMeasurements()
{
    if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nreductionStat::findMeasurements\n";
    int i, szi, j, szj;
    if(ebFile==NULL) return;
    szi = ebFile->errList.size();

    errBudgetRec* eRec;
    measurementRec* sRec;
    residualsRec* rRec;
    objResRec* objRec;
    //ocRec* oRec;
    //sstarRes* ssRec;
    measurementStatRec* msRec;

    for(i=0; i<szi; i++)
    {
        //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("i= %1\n").arg(i);
        sRec = new measurementRec;
        eRec = ebFile->errList.at(i);
        sRec->errBud = new errBudgetRec;
        sRec->errBud->copy(*eRec);
        sRec->mesureTimeCode = sRec->errBud->mesureTimeCode;
        //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("mesureTimeCode: %1\n").arg(sRec->mesureTimeCode);
        if(resFile!=NULL)
        {
            szj = resFile->resList.size();
            //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("resFileSize = %1\n").arg(szj);
            for(j=0; j<szj; j++)
            {
                //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("j= %1\n").arg(j);
                if(resFile->resList.at(j)->mesureTimeCode==sRec->mesureTimeCode)
                {
                    rRec = new residualsRec;
                    rRec->copy(*resFile->resList.at(j));
                    sRec->resList << rRec;
                }
            }
        }
        if(objFile!=NULL)
        {
            szj = objFile->ocList.size();
            //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("mpeFile = %1\n").arg(szj);
            for(j=0; j<szj; j++)
            {
                if(objFile->ocList.at(j)->mesureTimeCode==sRec->mesureTimeCode)
                {
                    objRec = new objResRec;
                    //if(REDSTAT_LOG_LEVEL>0) qDebug() << "copying...\n";
                    objRec->copy(*objFile->ocList.at(j));
                    sRec->objList << objRec;
                    //if(REDSTAT_LOG_LEVEL>0) qDebug() << "copyed...\n";
                }
            }
        }
        /*
        if(mpeFile!=NULL)
        {
            szj = mpeFile->ocList.size();
            //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("mpeFile = %1\n").arg(szj);
            for(j=0; j<szj; j++)
            {
                if(mpeFile->ocList.at(j)->mesureTimeCode==sRec->mesureTimeCode)
                {
                    oRec = new ocRec;
                    //if(REDSTAT_LOG_LEVEL>0) qDebug() << "copying...\n";
                    oRec->copy(*mpeFile->ocList.at(j));
                    sRec->mpeList << oRec;
                    //if(REDSTAT_LOG_LEVEL>0) qDebug() << "copyed...\n";
                }
            }
        }
        if(ssFile!=NULL)
        {
            szj = ssFile->ocList.size();
            //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("ssFile = %1\n").arg(szj);
            for(j=0; j<szj; j++)
            {
                //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("j= %1\n").arg(j);
                //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("ssFile.mesureTimeCode: %1\n").arg(ssFile->ocList.at(j)->mesureTimeCode);
                if(ssFile->ocList.at(j)->mesureTimeCode.lastIndexOf(sRec->mesureTimeCode)==0)
                {
                    ssRec = new sstarRes;
                    ssRec->copy(*ssFile->ocList.at(j));
                    sRec->sstarList << ssRec;
                }
            }
        }
        */
        //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("sRec->sstarList = %1\n").arg(sRec->sstarList.size());
        msRec = new measurementStatRec;
        sRec->detStat(msRec);
        mesList << sRec;
        statList << msRec;
    }

}

void measurementRec::detStat(measurementStatRec *stRec)
{
    stRec->mesureTimeCode = mesureTimeCode;
    stRec->originName = errBud->originName;
    stRec->UWEx = errBud->xParams.UWE;
    stRec->UWEy = errBud->yParams.UWE;
    stRec->UWEm = errBud->mParams.UWE;
    stRec->Nx = errBud->xParams.N;
    stRec->Ny = errBud->yParams.N;
    stRec->Nm = errBud->mParams.N;

    int sz0, i;
    sz0 = resList.size();
    stRec->ocNum = sz0;
    stRec->meanOCksi = 0.0;
    stRec->meanOCeta = 0.0;
    for(i=0; i<sz0; i++)
    {
        stRec->meanOCksi += resList.at(i)->ksiOC;
        stRec->meanOCeta += resList.at(i)->etaOC;
    }
    stRec->meanOCksi /= sz0;
    stRec->meanOCeta /= sz0;
    stRec->rmsOneOCksi = 0.0;
    stRec->rmsOneOCeta = 0.0;
    for(i=0; i<sz0; i++)
    {
        stRec->rmsOneOCksi += pow(resList.at(i)->ksiOC-stRec->meanOCksi, 2.0);
        stRec->rmsOneOCeta += pow(resList.at(i)->etaOC-stRec->meanOCeta, 2.0);
    }
    stRec->rmsOneOCksi = sqrt(stRec->rmsOneOCksi/(1.0*sz0-1));
    stRec->rmsOneOCeta = sqrt(stRec->rmsOneOCeta/(1.0*sz0-1));
    stRec->rmsMeanOCksi = stRec->rmsOneOCksi/sqrt(sz0-1);
    stRec->rmsMeanOCeta = stRec->rmsOneOCeta/sqrt(sz0-1);
}

/*
void measurementRec::detMarksList(QList <marksP*> u3List)
{
    if(REDSTAT_LOG_LEVEL>0) qDebug() << "findCloserMarks\n";
    int i, j, szU3, szRes;
    marksP *mkNew;

    u3MarksList.clear();


//    mgRes->clearMarks();
//    mgRes->ctype = mgEkv->ctype;
 //   if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("mgRes size = %1\n").arg(mgRes->marks.size());
    szU3 = u3List.size();
    szRes = resList.size();
    double dist, distMin;//, distMax;
    int posMin;

 //   if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("mgEkv size = %1\n").arg(mgEkv->marks.size());
  //  if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("mgIP size = %1\n").arg(mgIP->marks.size());

    double dMax = mas_to_grad(1);

    for(i=0; i<szRes; i++)
    {
   //     if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("i: %1\n").arg(i);
        distMin = dMax;
        posMin = -1;
        //szE = mgEkv->marks.size();
        szU3 = u3List.size();

        for(j=0; j<szU3; j++)
        {
   //         if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("j: %1\n").arg(j);
//            if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("resList[i]->ra: %1\tu3List[j]->u3Rec->ra: %2\n").arg(resList[i]->ra).arg(u3List[j]->u3Rec->ra);
//            if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("resList[i]->de: %1\tu3List[j]->u3Rec->dec: %2\n").arg(resList[i]->de).arg(u3List[j]->u3Rec->dec);
            dist = sqrt(pow(resList[i]->ra - u3List[j]->mEkv[0], 2.0) + pow(resList[i]->de - u3List[j]->mEkv[1], 2.0));
            //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("dist= %1\n").arg(dist);
            if(dist<distMin)
            {
                distMin = dist;
                posMin = j;
            }
        }
        if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("posMin: %1\tdistMin: %2\tdMax: %3\n").arg(posMin).arg(grad_to_mas(distMin)).arg(grad_to_mas(dMax));
        if((posMin!=-1)&&(distMin<dMax))
        {
            mkNew = new marksP(OBJ_TYPE_UCAC3);

            mkNew->resRec = new residualsRec;

   //         if(REDSTAT_LOG_LEVEL>0) qDebug() << "mkNew: " << mkNew << ":";
            mkNew->copy(u3List.at(posMin));
            mkNew->resRec->copy(*resList.at(i));
   /*         if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("mkNewEkv: %1\t%2\t%3\n").arg(mkNew->mEkv[0]).arg(mkNew->mEkv[1]).arg(mkNew->mEkv[2]);
            if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("ocObj: %1\t%2\t%3\n").arg(mkNew->ocObj->ra).arg(mkNew->ocObj->de).arg(mkNew->ocObj->mag0);
            if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("resUcac2: %1\t%2\t%3\n").arg(mkNew->resUcac2->ra).arg(mkNew->resUcac2->de).arg(mkNew->resUcac2->mag);*/
            /*mkNew->mEkv[0] = mgEkv->marks.at(posMin)->mEkv[0];
            mkNew->mEkv[1] = mgEkv->marks.at(posMin)->mEkv[1];
            mkNew->mEkv[2] = mgEkv->marks.at(posMin)->mEkv[2];
            mkNew->mTan[0] = mgEkv->marks.at(posMin)->mTanImg[0];
            mkNew->mTan[1] = mgEkv->marks.at(posMin)->mTanImg[1];*/
     //       if(REDSTAT_LOG_LEVEL>0) qDebug() << mkNew << "\n";
            //if(REDSTAT_LOG_LEVEL>0) qDebug() << "mgIP->marks.at(i)->P: " << mgIP->marks.at(i)->P << "\n";
//            mkNew->copyImg(*mgIP->marks.at(i));
      /*      if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("mkNewEkv: %1\t%2\t%3\n").arg(mkNew->mEkv[0]).arg(mkNew->mEkv[1]).arg(mkNew->mEkv[2]);
            if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("ocObj: %1\t%2\t%3\n").arg(mkNew->ocObj->ra).arg(mkNew->ocObj->de).arg(mkNew->ocObj->mag0);
            if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("resUcac2: %1\t%2\t%3\n").arg(mkNew->resUcac2->ra).arg(mkNew->resUcac2->de).arg(mkNew->resUcac2->mag);*/
         /*   mkNew->mTanImg[0] = mgIP->marks.at(i)->mTanImg[0];
            mkNew->mTanImg[1] = mgIP->marks.at(i)->mTanImg[1];
            mkNew->mTanImg[2] = mgIP->marks.at(i)->mTanImg[2];
           /
           // if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("mkNew->mTanImg[2] = %1\n").arg(mkNew->mTanImg[2]);
            //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("mkNew->P[18] = %1\n").arg(mkNew->P[18]);
            //if(REDSTAT_LOG_LEVEL>0) qDebug() << "mkNew->P: " << mkNew->P << "\n";
            u3MarksList << mkNew;
            u3List.removeAt(posMin);

        }
        //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("mgIP->marks.removeAt(i)\n");
       // if(!is_empty) mgIP->marks.removeAt(i);
    }

}
*/
measurementRec* reductionStat::getMeasurement(QString mesTimeCode)
{
    //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("\ngetMeasurement\n");
    //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("get mesureTimeCode: %1\n").arg(mesTimeCode);
    int i, sz0;
    sz0 = mesList.size();
    //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("mes size: %1\n").arg(sz0);
    for(i=0; i<sz0; i++)
    {
        //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("mesureTimeCode: %1\n").arg(mesList.at(i)->mesureTimeCode);
        if(mesTimeCode==mesList.at(i)->mesureTimeCode) return mesList.at(i);
    }
    return NULL;
}

measurementStatRec* reductionStat::getMeasurementStat(QString mesTimeCode)
{
    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("\ngetMeasurementStat\n");
    int i, sz0;
    sz0 = statList.size();
    for(i=0; i<sz0; i++)
    {
        if(mesTimeCode==statList.at(i)->mesureTimeCode) return statList.at(i);
    }

    return NULL;
}

void plateStatRec::getMinUWE(measurementStatRec* minRec)
{
    if(REDSTAT_LOG_LEVEL>0) qDebug() << "\ngetMinUWE\n";
    int i, sz0;
    int posMin;
    double UWEmin, uwe;

    if(REDSTAT_LOG_LEVEL>0) qDebug()<< "minRec: " << minRec << "\n";

    UWEmin = 10000;
    posMin = -1;
    sz0 = mStatList.size();
    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("mStatList.size= %1\n").arg(sz0);
    for(i=0; i<sz0; i++)
    {
        uwe = sqrt(pow(mStatList.at(i)->UWEx, 2.0) + pow(mStatList.at(i)->UWEy, 2.0));
        if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("uwe= %1\n").arg(uwe);
        if(uwe<UWEmin)
        {
            UWEmin = uwe;
            posMin = i;
        }
    }
    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("posMin= %1\n").arg(posMin);
    if(posMin!=-1)
    {
        minRec->copy(mStatList.at(posMin));
        if(REDSTAT_LOG_LEVEL>0) qDebug() << "\ngetMinUWE res: " << minRec << " " << minRec->mesureTimeCode << "\n";
    }
    else
    {
        minRec = NULL;
        if(REDSTAT_LOG_LEVEL>0) qDebug() << "\ngetMinUWE res: " << minRec << "\n";
    }

}

int plateStatRec::getMinUWEpos()
{
    if(REDSTAT_LOG_LEVEL>0) qDebug() << "\ngetMinUWE\n";
    int i, sz0;
    int posMin;
    double UWEmin, uwe;

    UWEmin = 10000;
    posMin = -1;
    sz0 = mStatList.size();
    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("mStatList.size= %1\n").arg(sz0);
    for(i=0; i<sz0; i++)
    {
        uwe = sqrt(pow(mStatList.at(i)->UWEx, 2.0) + pow(mStatList.at(i)->UWEy, 2.0));
        if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("uwe= %1\n").arg(uwe);
        if(uwe<UWEmin)
        {
            UWEmin = uwe;
            posMin = i;
        }
    }
    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("posMin= %1\n").arg(posMin);
    return posMin;
    /*
    if(posMin!=-1)
    {
        minRec = mStatList.at(posMin);
        if(REDSTAT_LOG_LEVEL>0) qDebug() << "\ngetMinUWE res: " << minRec << " " << minRec->mesureTimeCode << "\n";
    }
    else
    {
        minRec = NULL;
        if(REDSTAT_LOG_LEVEL>0) qDebug() << "\ngetMinUWE res: " << minRec << "\n";
    }
*/
}

int plateStatRec::getVersNamePos(QString versName)
{
    if(REDSTAT_LOG_LEVEL>0) qDebug() << "\ngetVersNamePos\n";
    int i, sz0;
    //int posMin;
    //double UWEmin, uwe;

    //UWEmin = 10000;
   // posMin = -1;
    sz0 = mStatList.size();
    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("mStatList.size= %1\n").arg(sz0);
    QString tVers;
    for(i=0; i<sz0; i++)
    {
        tVers.clear();
        detVersName(&tVers, mStatList.at(i)->originName);
        if(QString::compare(tVers, versName, Qt::CaseSensitive)==0)
        {
            return i;
        }
        /*
        uwe = sqrt(pow(mStatList.at(i)->UWEx, 2.0) + pow(mStatList.at(i)->UWEy, 2.0));
        if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("uwe= %1\n").arg(uwe);
        if(uwe<UWEmin)
        {
            UWEmin = uwe;
            posMin = i;
        }*/
    }
    //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("posMin= %1\n").arg(posMin);
    return -1;
}

void appendRep0Rec(QString *dataStr, measurementStatRec* msRec, measurementRec* mesRec, int plNameType)
{
    QString versName, plateName;
    int year, mth;
    double day, pyear;
    int j, szj;
    ocRec ocr;

    dataStr->clear();

    detVersName(&versName, msRec->originName);
    detPlateName(&plateName, msRec->originName, plNameType);

    //versName = msRec->originName.section(".", -1, -1);
    //if(versName=="aver") versName = msRec->originName.section(".", -2, -2);
    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("plateName: %1\tversName: %2\n").arg(plateName).arg(versName);

    dat2YMD(mjd2jd(mesRec->errBud->MJD), &year, &mth, &day);
    pyear = partOfYear(year, mth, day);

    dataStr->append(QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11|%12|%13").arg(plateName).arg(pyear, 7, 'f', 2).arg((int)mesRec->errBud->MJD).arg(versName).arg(msRec->Nx).arg(msRec->UWEx).arg(msRec->Ny).arg(msRec->UWEy).arg(msRec->ocNum).arg(msRec->rmsOneOCksi).arg(msRec->rmsOneOCeta).arg(msRec->rmsMeanOCksi).arg(msRec->rmsMeanOCeta));

    szj = mesRec->objList.size();
    for(j=0; j<szj; j++)
    {
        mesRec->objList.at(j)->toOcRec(&ocr);
        dataStr->append(QString("|%1|%2|%3").arg(ocr.ocRaCosDe).arg(ocr.ocDe).arg(ocr.name.simplified()));
    }
/*
    szj = mesRec->mpeList.size();
    for(j=0; j<szj; j++)dataStr->append(QString("|%1|%2|%3").arg(mesRec->mpeList.at(j)->ocRaCosDe).arg(mesRec->mpeList.at(j)->ocDe).arg(mesRec->mpeList.at(j)->name.simplified()));

    szj = mesRec->sstarList.size();
    for(j=0; j<szj; j++)dataStr->append(QString("|%1|%2|%3").arg(mesRec->sstarList.at(j)->ksiOC).arg(mesRec->sstarList.at(j)->etaOC).arg(mesRec->sstarList.at(j)->lspmName.simplified()));*/
    dataStr->append(QString("|%1").arg(msRec->mesureTimeCode));
    dataStr->append(QString("|%1\n").arg(msRec->originName));
}

void platesStat::saveReport0(QString r0name, int isMinUWE, int plNameType, reductionStat *rStat, QList <measurementRec*> *mesList)
{
    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("saveReport0\n");
    int pMin, i, j, k, szi, szj, szRes;
    measurementStatRec* msRec;
    measurementRec* mesRec;
    QFile rFile;
    QTextStream dataStream;
    QString versName, dataStr;
    int year, mth;
    double day, pyear;
    int plSz;

    rFile.setFileName(r0name);
    rFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
    dataStream.setDevice(&rFile);

    szi = platesList.size();
    for(i=0; i<szi; i++)
    {
        //msRec = NULL;
        if(REDSTAT_LOG_LEVEL>0) qDebug() << "msRec: " << msRec << "\n";
        plSz=platesList.at(i)->mStatList.size();
        if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("plSz: %1").arg(plSz);
        if(plSz==0)
        {
            dataStr = QString("%1\n").arg(platesList.at(i)->plateName);
            dataStream << dataStr;
        }
        else
        {

        //plStat.platesList.at(i)->getMinUWE(msRec);
            if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("isMinUWE: %1").arg(isMinUWE);
            if(isMinUWE)
            {
            pMin = platesList.at(i)->getMinUWEpos();
                if(pMin!=-1)
                //if(REDSTAT_LOG_LEVEL>0) qDebug() << "msRec: " << msRec << "\n";
                //if(msRec!=NULL)
                {
                    msRec = platesList.at(i)->mStatList.at(pMin);

                    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("msRec mesureTimeCode: %1\n").arg(msRec->mesureTimeCode);
                    mesRec = rStat->getMeasurement(msRec->mesureTimeCode);
                    if(REDSTAT_LOG_LEVEL>0) qDebug() << "mesRec: " << mesRec << "\n";
                    if(mesList!=NULL) mesList->append(mesRec);

                    appendRep0Rec(&dataStr, msRec, mesRec, plNameType);
        /*
                    msRec->getVersName(&versName);
                    //versName = msRec->originName.section(".", -1, -1);
                    //if(versName=="aver") versName = msRec->originName.section(".", -2, -2);
                    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("plateName: %1\tversName: %2\n").arg(platesList.at(i)->plateName).arg(versName);

                    dat2YMD(mjd2jd(mesRec->errBud->MJD), &year, &mth, &day);
                    pyear = partOfYear(year, mth, day);

                    dataStr = QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11|%12").arg(platesList.at(i)->plateName).arg(pyear, 7, 'f', 2).arg((int)mesRec->errBud->MJD).arg(versName).arg(msRec->Nx).arg(msRec->UWEx).arg(msRec->Ny).arg(msRec->UWEy).arg(msRec->rmsOneOCksi).arg(msRec->rmsOneOCeta).arg(msRec->rmsMeanOCksi).arg(msRec->rmsMeanOCeta);

                    szj = mesRec->mpeList.size();
                    for(j=0; j<szj; j++)dataStr.append(QString("|%1|%2|%3").arg(mesRec->mpeList.at(j)->ocRaCosDe).arg(mesRec->mpeList.at(j)->ocDe).arg(mesRec->mpeList.at(j)->name.simplified()));

                    szj = mesRec->sstarList.size();
                    for(j=0; j<szj; j++)dataStr.append(QString("|%1|%2|%3").arg(mesRec->sstarList.at(j)->ksiOC).arg(mesRec->sstarList.at(j)->etaOC).arg(mesRec->sstarList.at(j)->lspmName.simplified()));
                    dataStr.append(QString("|%1").arg(msRec->originName));*/


                    ///////////////////////////////
                    /*szRes = mesRec->resList.size();
                    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("szRes: %1\n").arg(szRes);
                    for(j=0; j<szRes; j++)
                    {
                        for(k=0; k<magEq.diapsNum-1; k++)
                        {
                            if(mesRec->resList.at(j)->mag>magEq.diaps[k]&&mesRec->resList.at(j)->mag<magEq.diaps[k+1])
                            {
                                magEq.resListDiap.at(k)->resList << mesRec->resList.at(j);
                            }
                        }
                    }

                    if(mesRec->errBud->MJD<t0) t0 = mesRec->errBud->MJD;
                    if(mesRec->errBud->MJD>t1) t1 = mesRec->errBud->MJD;*/
                }
                else
                {
                    dataStr = QString("%1\n").arg(platesList.at(i)->plateName);

                }
                dataStream << dataStr;
            }
            else
            {
                for(j=0; j<plSz; j++)
                {
                    msRec = platesList.at(i)->mStatList.at(j);

                    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("msRec mesureTimeCode: %1\n").arg(msRec->mesureTimeCode);
                    mesRec = rStat->getMeasurement(msRec->mesureTimeCode);
                    if(REDSTAT_LOG_LEVEL>0) qDebug() << "mesRec: " << mesRec << "\n";
                    if(mesList!=NULL) mesList->append(mesRec);

                    appendRep0Rec(&dataStr, msRec, mesRec, plNameType);
                    dataStream << dataStr;
                }
            }
        }


    }
    rFile.close();
}
/*
void platesStat::excludeMes(QStringList excMes)
{
    szi = platesList.size();
    for(i=0; i<szi; i++)
    {
        //msRec = NULL;
        if(REDSTAT_LOG_LEVEL>0) qDebug() << "msRec: " << msRec << "\n";
        plSz=platesList.at(i)->mStatList.size();
        if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("plSz: %1").arg(plSz);
        if(plSz==0)
        {
            dataStr = QString("%1\n").arg(platesList.at(i)->plateName);
            dataStream << dataStr;
        }
        else
        {

        //plStat.platesList.at(i)->getMinUWE(msRec);
            //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("isMinUWE: %1").arg(isMinUWE);


            for(j=0; j<versNum; j++)
            {
                pMin = platesList.at(i)->getVersNamePos(versSeqList.at(j));
                //if(pMin!=-1) break;

                if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("versName: %1\tpmin: %2\n").arg(versSeqList.at(j)).arg(pMin);


                if(pMin!=-1)
                //if(REDSTAT_LOG_LEVEL>0) qDebug() << "msRec: " << msRec << "\n";
                //if(msRec!=NULL)
                {
                    msRec = platesList.at(i)->mStatList.at(pMin);

                    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("msRec mesureTimeCode: %1\n").arg(msRec->mesureTimeCode);
                    if(excMes.contains(msRec->mesureTimeCode, Qt::CaseSensitive))
                    {
                        if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("excluded\n");
                        pMin = -1;
                        continue;
                    }
                }
            }
        }
    }
}
*/

void platesStat::dropObj(reductionStat rStat)
{
    if(REDSTAT_LOG_LEVEL>0) qDebug() << "dropObj\n";
    int i, j, szi, plSz;
    szi = platesList.size();
    measurementStatRec* msRec;
    measurementRec* mRec;
    for(i=0; i<szi; i++)
    {
        plSz=platesList.at(i)->mStatList.size();
        if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("plSz: %1").arg(plSz);
        for(j=plSz-1; j>=0; j--)
        {
            msRec = platesList.at(i)->mStatList.at(j);
            mRec = rStat.getMeasurement(msRec->mesureTimeCode);
            if(mRec->objList.size()<1)platesList.at(i)->mStatList.removeAt(j);
        }
    }
}

void platesStat::selMinUWE(reductionStat *rStat, QList <measurementRec*> *mesList)
{
    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("selMinUWE\n");
    int pMin, i, szi;
    measurementStatRec* msRec;
    measurementRec* mesRec;


    int plSz;


    szi = platesList.size();
    for(i=0; i<szi; i++)
    {
        //msRec = NULL;
        if(REDSTAT_LOG_LEVEL>0) qDebug() << "msRec: " << msRec << "\n";
        plSz=platesList.at(i)->mStatList.size();
        if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("plSz: %1").arg(plSz);


        //plStat.platesList.at(i)->getMinUWE(msRec);

        pMin = platesList.at(i)->getMinUWEpos();
        if(pMin!=-1)
        {
            msRec = platesList.at(i)->mStatList.at(pMin);

            if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("msRec mesureTimeCode: %1\n").arg(msRec->mesureTimeCode);
            mesRec = rStat->getMeasurement(msRec->mesureTimeCode);
            if(REDSTAT_LOG_LEVEL>0) qDebug() << "mesRec: " << mesRec << "\n";
            if(mesList!=NULL) mesList->append(mesRec);


        }

    }
}

void platesStat::selMesList(reductionStat *rStat, QList <measurementRec*> *mesList)
{
    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("selMesList\n");
    int i, szi, j, szj;
    measurementStatRec* msRec;
    measurementRec* mesRec;



    szi = platesList.size();
    for(i=0; i<szi; i++)
    {
        szj = platesList.at(i)->mStatList.size();
        for(j=0; j<szj; j++)
        {
            msRec = platesList.at(i)->mStatList.at(j);

            if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("msRec mesureTimeCode: %1\n").arg(msRec->mesureTimeCode);
            mesRec = rStat->getMeasurement(msRec->mesureTimeCode);
            if(REDSTAT_LOG_LEVEL>0) qDebug() << "mesRec: " << mesRec << "\n";
            if(mesList!=NULL) mesList->append(mesRec);


        }

    }
}



void platesStat::selVersSeq(QStringList versSeqList, reductionStat *rStat, QList <measurementRec*> *mesList, int minObj)
{
    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("selVersSeq\n");
    int pMin, i, j, szi;
    measurementStatRec* msRec;
    measurementRec* mesRec;

    int plSz, isapp, mo, osize;

    //QStringList versSeqList = versSeq.split("|");
    int versNum = versSeqList.size();

    szi = platesList.size();
    for(i=0; i<szi; i++)
    {
        //msRec = NULL;
        //if(REDSTAT_LOG_LEVEL>0) qDebug() << "msRec: " << msRec << "\n";
        plSz=platesList.at(i)->mStatList.size();
        //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("plSz: %1").arg(plSz);
        if(plSz==0)
        {
        //    dataStr = QString("%1\n").arg(platesList.at(i)->plateName);
        //    dataStream << dataStr;
        }
        else
        {

        //plStat.platesList.at(i)->getMinUWE(msRec);
            //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("isMinUWE: %1").arg(isMinUWE);

            isapp = 0;
            mo = minObj;

            do
            {

                for(j=0; j<versNum; j++)
                {
                    pMin = platesList.at(i)->getVersNamePos(versSeqList.at(j));
                    //if(pMin!=-1) break;

                    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("versName: %1\tpmin: %2\n").arg(versSeqList.at(j)).arg(pMin);


                    if(pMin!=-1)
                    //if(REDSTAT_LOG_LEVEL>0) qDebug() << "msRec: " << msRec << "\n";
                    //if(msRec!=NULL)
                    {
                        msRec = platesList.at(i)->mStatList.at(pMin);
                        //msRec->ocNum
                        if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("msRec mesureTimeCode: %1\n").arg(msRec->mesureTimeCode);

                        mesRec = rStat->getMeasurement(msRec->mesureTimeCode);
                        osize = mesRec->objList.size();
                        if(osize<mo) continue;
                        //if(REDSTAT_LOG_LEVEL>0) qDebug() << "mesRec: " << mesRec << "\n";
                        //if(detObj&&(mesRec->objList.size()==0)) break;
                        if(mesList!=NULL)
                        {
                            mesList->append(mesRec);
                            isapp=1;
                            break;
                        }

                    }

                }
                if(mo==0) break;
                mo = 0;

            }while(!isapp);

//            if(pMin==-1) dataStr = QString("%1\n").arg(platesList.at(i)->plateName);

        }


    }
}

void saveReport0(QString r0name, QList <measurementRec*> mesList, int plNameType)
{
    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("saveReport0\n");
    int pMin, i, j, k, szi, szj, szRes;
    measurementStatRec* msRec;
    measurementRec* mesRec;
    QFile rFile;
    QTextStream dataStream;
    QString dataStr;

    rFile.setFileName(r0name);
    rFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
    dataStream.setDevice(&rFile);

    msRec = new measurementStatRec;

    szi = mesList.size();
    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("mesList size: %1\n").arg(szi);
    for(i=0; i<szi; i++)
    {
        mesRec = mesList[i];
        mesRec->detStat(msRec);
        appendRep0Rec(&dataStr, msRec, mesRec, plNameType);
        dataStream << dataStr;
    }


    rFile.close();

}

void platesStat::saveReport0Seq(QString r0name, QString versSeq, QStringList excMes, int plNameType, reductionStat *rStat, QList <measurementRec*> *mesList)
{
    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("saveReport0Seq\n");
    int pMin, i, j, k, szi, szj, szRes;
    measurementStatRec* msRec;
    measurementRec* mesRec;
    QFile rFile;
    QTextStream dataStream;
    QString versName, dataStr;
    int year, mth;
    double day, pyear;
    int plSz;

    QStringList versSeqList = versSeq.split("|");
    int versNum = versSeqList.size();


    rFile.setFileName(r0name);
    rFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
    dataStream.setDevice(&rFile);

    szi = platesList.size();
    for(i=0; i<szi; i++)
    {
        //msRec = NULL;
        if(REDSTAT_LOG_LEVEL>0) qDebug() << "msRec: " << msRec << "\n";
        plSz=platesList.at(i)->mStatList.size();
        if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("plSz: %1").arg(plSz);
        if(plSz==0)
        {
            dataStr = QString("%1\n").arg(platesList.at(i)->plateName);
            dataStream << dataStr;
        }
        else
        {

        //plStat.platesList.at(i)->getMinUWE(msRec);
            //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("isMinUWE: %1").arg(isMinUWE);


            for(j=0; j<versNum; j++)
            {
                pMin = platesList.at(i)->getVersNamePos(versSeqList.at(j));
                //if(pMin!=-1) break;

                if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("versName: %1\tpmin: %2\n").arg(versSeqList.at(j)).arg(pMin);


                if(pMin!=-1)
                //if(REDSTAT_LOG_LEVEL>0) qDebug() << "msRec: " << msRec << "\n";
                //if(msRec!=NULL)
                {
                    msRec = platesList.at(i)->mStatList.at(pMin);

                    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("msRec mesureTimeCode: %1\n").arg(msRec->mesureTimeCode);
                    if(excMes.contains(msRec->mesureTimeCode, Qt::CaseSensitive))
                    {
                        if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("excluded\n");
                        pMin = -1;
                        continue;
                    }
                    mesRec = rStat->getMeasurement(msRec->mesureTimeCode);
                    if(REDSTAT_LOG_LEVEL>0) qDebug() << "mesRec: " << mesRec << "\n";
                    if(mesList!=NULL) mesList->append(mesRec);

                    appendRep0Rec(&dataStr, msRec, mesRec, plNameType);
                    break;
        /*
                    msRec->getVersName(&versName);
                    //versName = msRec->originName.section(".", -1, -1);
                    //if(versName=="aver") versName = msRec->originName.section(".", -2, -2);
                    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("plateName: %1\tversName: %2\n").arg(platesList.at(i)->plateName).arg(versName);

                    dat2YMD(mjd2jd(mesRec->errBud->MJD), &year, &mth, &day);
                    pyear = partOfYear(year, mth, day);

                    dataStr = QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11|%12").arg(platesList.at(i)->plateName).arg(pyear, 7, 'f', 2).arg((int)mesRec->errBud->MJD).arg(versName).arg(msRec->Nx).arg(msRec->UWEx).arg(msRec->Ny).arg(msRec->UWEy).arg(msRec->rmsOneOCksi).arg(msRec->rmsOneOCeta).arg(msRec->rmsMeanOCksi).arg(msRec->rmsMeanOCeta);

                    szj = mesRec->mpeList.size();
                    for(j=0; j<szj; j++)dataStr.append(QString("|%1|%2|%3").arg(mesRec->mpeList.at(j)->ocRaCosDe).arg(mesRec->mpeList.at(j)->ocDe).arg(mesRec->mpeList.at(j)->name.simplified()));

                    szj = mesRec->sstarList.size();
                    for(j=0; j<szj; j++)dataStr.append(QString("|%1|%2|%3").arg(mesRec->sstarList.at(j)->ksiOC).arg(mesRec->sstarList.at(j)->etaOC).arg(mesRec->sstarList.at(j)->lspmName.simplified()));
                    dataStr.append(QString("|%1").arg(msRec->originName));*/


                    ///////////////////////////////
                    /*szRes = mesRec->resList.size();
                    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("szRes: %1\n").arg(szRes);
                    for(j=0; j<szRes; j++)
                    {
                        for(k=0; k<magEq.diapsNum-1; k++)
                        {
                            if(mesRec->resList.at(j)->mag>magEq.diaps[k]&&mesRec->resList.at(j)->mag<magEq.diaps[k+1])
                            {
                                magEq.resListDiap.at(k)->resList << mesRec->resList.at(j);
                            }
                        }
                    }

                    if(mesRec->errBud->MJD<t0) t0 = mesRec->errBud->MJD;
                    if(mesRec->errBud->MJD>t1) t1 = mesRec->errBud->MJD;*/
                }

            }
            if(pMin==-1) dataStr = QString("%1\n").arg(platesList.at(i)->plateName);

            dataStream << dataStr;

        }


    }
    rFile.close();
}




void platesStat::init(QList <measurementStatRec*> mesStatList, int plNameType)
{
    if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nplatesStat::init\n";
    QString plateName;
    int i, sz0, j, sz1;
    int isNew;
    plateStatRec* plRec;
    platesList.clear();

    sz0 = mesStatList.size();
    for(i=0; i<sz0; i++)
    {
        isNew = 1;
        detPlateName(&plateName, mesStatList.at(i)->originName, plNameType);
        /*
        switch(plNameType)
        {
        case 0:
           plateName = mesStatList.at(i)->originName.section("\\", -2, -2);
           break;
        case 1:
           plateName = mesStatList.at(i)->originName.section("\\", -1, -1).section(".", 0, 0);
           //plateName = plateName.right(plateName.size()-2);
           break;
        case 2:
           plateName = mesStatList.at(i)->originName;
           break;
        }
        */
        //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("plateName: %1\n").arg(plateName);
        //plateName = mesStatList.at(i)->originName.section("\\", -2, -2);
        sz1 = platesList.size();
        for(j=0; j<sz1; j++)
        {
            if(plateName.toUpper()==platesList.at(j)->plateName.toUpper())
            {
                isNew = 0;
                platesList.at(j)->mStatList << mesStatList.at(i);
                break;
            }
        }
        if(isNew)
        {
            plRec = new plateStatRec;
            plRec->plateName = plateName.toUpper();
            //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("new plate: %1\n").arg(plRec->plateName);
            plRec->mStatList << mesStatList.at(i);
            platesList << plRec;
        }
    }

    if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nplatesStat::init ended\n";
}

void platesStat::initPlatesNamesFile(QString plnFileName)
{
    if(REDSTAT_LOG_LEVEL>0) qDebug() << "\ninitPlatesNamesFile\n";
    //QTextCodec *codec1 = QTextCodec::codecForName("KOI8-R");
    //Q_ASSERT( codec1 );
    QFile pnFile(plnFileName);
    if(!pnFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        if(REDSTAT_LOG_LEVEL>0) qDebug() << "\npnFile not open\n";
        return;
    }
    QTextStream pnStream(&pnFile);

    QString dStr, plName;
    while(!pnStream.atEnd())
    {
        dStr = pnStream.readLine();
        plName = dStr.section(" ", 0, 0);
        if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("plName: %1\n").arg(plName);
        pushPlateName(plName);
    }
}

int platesStat::pushPlateName(QString plName)
{
    int j, sz1;
    plateStatRec* pRec;

    sz1 = platesList.size();
    for(j=0; j<sz1; j++)
    {
        if(plName.toUpper().simplified()==platesList.at(j)->plateName.toUpper())
        {
            return j;
        }
    }
    pRec = new plateStatRec;
    pRec->plateName = plName.toUpper();
    platesList << pRec;
    return(platesList.size()-1);
}



void measurementStatRec::copy(measurementStatRec *source)
{
    mesureTimeCode = source->mesureTimeCode;
    originName = source->originName;
    UWEx = source->UWEx;
    UWEy = source->UWEy;
    UWEm = source->UWEm;
    Nx = source->Nx;
    Ny = source->Ny;
    Nm = source->Nm;

    meanOCksi = source->meanOCksi;
    meanOCeta = source->meanOCeta;
    rmsMeanOCksi = source->rmsMeanOCksi;
    rmsMeanOCeta = source->rmsMeanOCeta;
    rmsOneOCksi = source->rmsOneOCksi;
    rmsOneOCeta = source->rmsOneOCeta;
}

//objStat
///////////////////////////////////
void objectsStat::init(reductionStat *redStat, int plNameType)
{
    if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nplatesStat::init\n";
    //QString plateName;
    int i, sz0, k, sz2;
    int isNew;
    objStatRec *objStat;
    //plateStatRec* plRec;
    //measurementStatRec *msRec;

    sz2 = redStat->objFile->ocList.size();
    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("objFile.size= %1\n").arg(sz2);
    for(k=0; k<sz2; k++)
    {
        isNew = 1;
        sz0 = objList.size();
        //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("redStat.ssFile->ocList.size= %1\n").arg(redStat.ssFile->ocList.size());
        for(i=0; i<sz0; i++)
        {
            if(QString::compare(redStat->objFile->ocList.at(k)->name, objList.at(i)->objName, Qt::CaseSensitive)==0)
            {
                isNew = 0;

                objList.at(i)->mStatList.append(redStat->getMeasurementStat(redStat->objFile->ocList.at(k)->mesureTimeCode));
                objList.at(i)->objList << redStat->objFile->ocList.at(k);
                break;
            }
        }
        if(isNew)
        {
            objStat = new objStatRec;
            objStat->objName = redStat->objFile->ocList.at(k)->name;
            objStat->catName = redStat->objFile->ocList.at(k)->catName;
            objStat->catMagName = redStat->objFile->ocList.at(k)->catMagName;
            objStat->objList << redStat->objFile->ocList.at(k);
            objStat->mStatList.append(redStat->getMeasurementStat(redStat->objFile->ocList.at(k)->mesureTimeCode));
            objList << objStat;
        }
    }
/*
    sz2 = redStat->ssFile->ocList.size();
    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("ssFile.size= %1\n").arg(sz2);
    for(k=0; k<sz2; k++)
    {

        isNew = 1;
        sz0 = objList.size();
        if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("objList sz0= %1\n").arg(sz0);
        for(i=0; i<sz0; i++)
        {
            //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("sz2= %1\n").arg(redStat.ssFile->ocList.size());
            //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("k= %1\ti= %2\n").arg(k).arg(i);
            if(QString::compare(redStat->ssFile->ocList.at(k)->lspmName, objList.at(i)->objName)==0)
            {
                isNew = 0;

                //msRec = new measurementStatRec;

                //msRec->copy(redStat->getMeasurementStat(redStat->ssFile->ocList.at(k)->mesureTimeCode));
                objList.at(i)->mStatList.append(redStat->getMeasurementStat(redStat->ssFile->ocList.at(k)->mesureTimeCode));
                //objList.at(i)->mStatList << msRec;
                break;
            }
        }
        //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("isNew = %1\n").arg(isNew);
        if(isNew)
        {
            objStat = new objStatRec;
            objStat->objName = redStat->ssFile->ocList.at(k)->lspmName;
            //msRec = new measurementStatRec;

            //msRec->copy(redStat->getMeasurementStat(redStat->ssFile->ocList.at(k)->mesureTimeCode));
            //objStat->mStatList << msRec;
            objStat->mStatList.append(redStat->getMeasurementStat(redStat->ssFile->ocList.at(k)->mesureTimeCode));
            objList.append(objStat);
        }
    }
*/
    sz0 = objList.size();
    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("objList.size= %1\n").arg(sz0);
    for(i=0; i<sz0; i++)
    {
        objList.at(i)->init(plNameType);
    }

    if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nobjStat::init ended\n";
}

void objStatRec::init(int plNameType)
{
    plStat = new platesStat;
    plStat->init(mStatList, plNameType);
}

void objStatRec::do3Sigma(reductionStat *rStat, double sigma, QList <objResRec*> *rejList, double pfc)
{
    if(REDSTAT_LOG_LEVEL>0) qDebug() << "objStatRec::do3Sigma\n";
    double *vectx, *vecty;
    int i, j;
    int sz0, sz1;
    double maxOcX, maxOcY;
    QString tstr;
    measurementRec *mesR;
    //QList <objResRec*> objList;
    //QList <ocRec*> mpeList;
    //QList <sstarRes*> sstarList;
    colRec *cRecRa = new colRec;
    colRec *cRecDe = new colRec;

    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("objName: %1\n").arg(objName);

    sz0 = mStatList.size();
 /*   for(i=0; i<sz0; i++)
    {
        mesR = rStat->getMeasurement(mStatList.at(i)->mesureTimeCode);

        objList << mesR->objList;
        //sstarList << mesR->sstarList;
    }*/
    sz1 = objList.size();
    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("mpeList.size: %1\n").arg(sz1);
    if(sz1>3)
    {
        vectx = new double[sz1];
        vecty = new double[sz1];
        do
        {
           sz0 = sz1;

            for(i=0; i<sz0; i++)
            {
                vectx[i] = objList.at(i)->ksiOC;
                vecty[i] = objList.at(i)->etaOC;
                objList.at(i)->rec2s(&tstr);
                //if(REDSTAT_LOG_LEVEL>0) qDebug() << tstr << "\n";

            }
            countColStat(cRecRa, vectx, sz0);
            countColStat(cRecDe, vecty, sz0);

            cRecRa->rec2s(&tstr);
            if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("statRa: %1\n").arg(tstr);
            cRecDe->rec2s(&tstr);
            if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("statDe: %1\n").arg(tstr);

            maxOcX = cRecRa->rmsOne*sigma;
            maxOcY = cRecDe->rmsOne*sigma;

            if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("maxOcX: %1\tmaxOcY= %2\n").arg(maxOcX).arg(maxOcY);

            for(i=sz0-1; i>=0; i--)
            {
                if((fabs(cRecRa->mean-objList.at(i)->ksiOC)>maxOcX)||(fabs(cRecDe->mean-objList.at(i)->etaOC)>maxOcY))
                {
                    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("remove mes: %1\tocX: %2\tocY: %3\n").arg(objList.at(i)->mesureTimeCode).arg(objList.at(i)->ksiOC).arg(objList.at(i)->etaOC);
                    if(rStat!=NULL) rStat->removeMes(objList.at(i)->mesureTimeCode);
                    removeMes(objList.at(i)->mesureTimeCode);
                    if(rejList!=NULL) *rejList << objList.at(i);
                    objList.removeAt(i);
                }
            }


            sz1 = objList.size();

        }while((abs(sz0-sz1)>(pfc*sz1))&&(sz1>3));
    }
/*
    sz1 = sstarList.size();
    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("sstarList.size: %1\n").arg(sz1);
    if(sz1>3)
    {
        vectx = new double[sz1];
        vecty = new double[sz1];
        do
        {
           sz0 = sz1;

            for(i=0; i<sz0; i++)
            {
                vectx[i] = sstarList.at(i)->ksiOC;
                vecty[i] = sstarList.at(i)->etaOC;
                sstarList.at(i)->rec2s(&tstr);
                if(REDSTAT_LOG_LEVEL>0) qDebug() << tstr << "\n";
            }
            countColStat(cRecRa, vectx, sz0);
            countColStat(cRecDe, vecty, sz0);

            maxOcX = cRecRa->rmsOne*sigma;
            maxOcY = cRecDe->rmsOne*sigma;

            for(i=sz0-1; i>=0; i--)
            {
                if((fabs(cRecRa->mean-sstarList.at(i)->ksiOC)>maxOcX)||(fabs(cRecDe->mean-sstarList.at(i)->etaOC)>maxOcY))
                {
                    rStat->removeMes(sstarList.at(i)->mesureTimeCode);
                    removeMes(sstarList.at(i)->mesureTimeCode);
                    sstarList.removeAt(i);
                }
            }


            sz1 = sstarList.size();

        }while((abs(sz0-sz1)>(pfc*sz1))&&(sz1>3));
    }
*/

    if(REDSTAT_LOG_LEVEL>0) qDebug() << "END objStatRec::do3Sigma\n";
}

void objStatRec::removeMes(QString mesTimeCode)
{
    int i, sz0;
    sz0 = mStatList.size();
    for(i=0; i<sz0; i++) if(QString::compare(mStatList.at(i)->mesureTimeCode, mesTimeCode, Qt::CaseInsensitive)==0)
    {
        mStatList.removeAt(i);
        return;
    }

}

void objStatRec::getOCList(reductionStat *rStat, QList <objResRec*> *ocList)
{
    int i, sz0;
    measurementRec *mesR;
    sz0 = mStatList.size();
    for(i=0; i<sz0; i++)
    {
        mesR = rStat->getMeasurement(mStatList.at(i)->mesureTimeCode);

        ocList->append(mesR->objList);
        //sstarList << mesR->sstarList;
    }
}
/*
void objStatRec::getSstarList(reductionStat *rStat, QList <sstarRes*> *sstarList)
{
    int i, sz0;
    measurementRec *mesR;
    sz0 = mStatList.size();
    for(i=0; i<sz0; i++)
    {
        mesR = rStat->getMeasurement(mStatList.at(i)->mesureTimeCode);

        //mpeList << mesR->mpeList;
        sstarList->append(mesR->sstarList);
    }
}
*/
///////////////////////////////////
/*
void eqPlatesStat::init(reductionStat *redStat, int plNameType)
{
    if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nplatesStat::init\n";
    //QString plateName;
    int i, sz0, j, sz1, k, sz2;
    int isNew;
    eqStatRec *eqStat;
    //plateStatRec* plRec;

    sz2 = redStat->mpeFile->ocList.size();
    for(k=0; k<sz2; k++)
    {
        isNew = 1;
        sz0 = asterList.size();
        //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("redStat.ssFile->ocList.size= %1\n").arg(redStat.ssFile->ocList.size());
        for(i=0; i<sz0; i++)
        {
            if(redStat->mpeFile->ocList.at(k)->name==asterList.at(i)->asterName)
            {
                isNew = 0;

                asterList.at(i)->mStatList.append(redStat->getMeasurementStat(redStat->mpeFile->ocList.at(k)->mesureTimeCode));
                break;
            }
        }
        if(isNew)
        {
            eqStat = new eqStatRec;
            eqStat->asterName = redStat->mpeFile->ocList.at(k)->name;
            eqStat->mStatList.append(redStat->getMeasurementStat(redStat->mpeFile->ocList.at(k)->mesureTimeCode));
            asterList << eqStat;
        }
    }

    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("redStat.ssFile->ocList.size= %1\n").arg(redStat->ssFile->ocList.size());


    sz0 = asterList.size();
    for(i=0; i<sz0; i++)
    {

        asterList.at(i)->init(plNameType);
        //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("redStat.ssFile->ocList.size= %1\n").arg(redStat.ssFile->ocList.size());

    }
    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("redStat.ssFile->ocList.size= %1\n").arg(redStat->ssFile->ocList.size());


    if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nplatesStat::init ended\n";
}

//void init(reductionStat redStat);

void eqStatRec::init(int plNameType)
{
    plStat = new platesStat;
    plStat->init(mStatList, plNameType);
}

//
void ssPlatesStat::init(reductionStat *redStat)
{
    if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nssPlatesStat::init\n";
    int i, sz0, j, sz1, k, sz2;
    int isNew;
    ssStatRec *ssStat;
    measurementStatRec *msRec;

    if(redStat->ssFile==NULL) return;
    sz2 = redStat->ssFile->ocList.size();

    for(k=0; k<sz2; k++)
    {

        isNew = 1;
        sz0 = ssList.size();
        if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("sz0= %1\n").arg(sz0);
        for(i=0; i<sz0; i++)
        {
            //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("sz2= %1\n").arg(redStat.ssFile->ocList.size());
            //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("k= %1\ti= %2\n").arg(k).arg(i);
            if(redStat->ssFile->ocList.at(k)->lspmName == ssList.at(i)->ssName)
            {
                isNew = 0;

                    //if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nocList.at(k): " << redStat.mpeFile->ocList.size() << "\n";



                msRec = new measurementStatRec;

                msRec->copy(redStat->getMeasurementStat(redStat->ssFile->ocList.at(k)->mesureTimeCode));
                ssList.at(i)->mStatList << msRec;
                break;
            }
        }
        //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("isNew = %1\n").arg(isNew);
        if(isNew)
        {
            ssStat = new ssStatRec;
            ssStat->ssName = redStat->ssFile->ocList.at(k)->lspmName;
            msRec = new measurementStatRec;

            msRec->copy(redStat->getMeasurementStat(redStat->ssFile->ocList.at(k)->mesureTimeCode));
            ssStat->mStatList << msRec;
            ssList.append(ssStat);
        }
    }

    sz0 = ssList.size();
    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("ssList.size= %1\n").arg(sz0);
    for(i=0; i<sz0; i++)
    {
        ssList.at(i)->init();
    }

    if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nplatesStat::init ended\n";
}

void ssStatRec::init()
{
    plStat = new platesStat;
    plStat->init(mStatList);
}
*/

/*

void eqSeriesRec::rec2sS(QString *recStr)
{
    recStr->clear();
//	DATEOBS dobs = getDATEOBSfromMJD(MJday);
    this->rec2s(resStr);

    recStr->append(QString("|%1|%2|%3|%4\n"))

   // recStr->insert(0, getStrFromDATEOBS(getDATEOBSfromMJD(MJday), " ", 0, 5) + "|" + deg_to_hms(ra, " ", 4) + "|"+ deg_to_damas(de, " ", 3) + "|" + QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11|%12|%13").arg(mag0, 6, 'f', 3, QLatin1Char(' ')).arg(ocRaCosDe, 8, 'f', 1, QLatin1Char(' ')).arg(ocDe, 8, 'f', 1, QLatin1Char(' ')).arg(ocMag, 8, 'f', 1, QLatin1Char(' ')).arg(topDist, 12, 'f', 9, QLatin1Char(' ')).arg(muRaCosDe, 9, 'f', 2, QLatin1Char(' ')).arg(muDe, 9, 'f', 2, QLatin1Char(' ')).arg(Vr, 6, 'f', 2, QLatin1Char(' ')).arg(phase, 8, 'f', 3, QLatin1Char(' ')).arg(elong, 8, 'f', 3, QLatin1Char(' ')).arg(expTime, 8, 'f', 3, QLatin1Char(' ') )+QString("|%1|%2|%3|%4\n").arg(name, 16).arg(catName, 8).arg(catEphName, 8).arg(catMagName, 8));
}
/
void eqSeriesRec::rec2sBase(QString *recStr)
{
    recStr->clear();
//	DATEOBS dobs = getDATEOBSfromMJD(MJday);

    recStr->insert(0, QString("%1 %2 ").arg(name, 16).arg(mjd2jd(MJday), 13, 'f', 5) + getStrFromDATEOBS(getDATEOBSfromMJD(MJday), " ", 0, 6) + " " + deg_to_hms(ra, " ", 3) + " "+ deg_to_damas(de, " ", 2) + "|" + QString("%1 %2").arg(ocRaCosDe, 8, 'f', 1, QLatin1Char(' ')).arg(ocDe, 8, 'f', 1, QLatin1Char(' '))+QString(" %1 %2\n").arg(catName, 8).arg(catEphName, 8));
}
/*
void eqSeriesRec::s2rec(QString recStr)
{
    QStringList sL = recStr.split("|");
//	//if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nsize=" << sL.size() << "\n";
    /*if(sL.size()!=16&&sL.size()!=18) return;
    if(sL.size()==16)
    {
            vers = 0;
    }
    else if(sL.size()==18){vers = 1;}/
    int szl = sL.size();
    int k;

    QString tstr = sL[0];



    MJday = getMJDfromYMD(tstr);
    ra = hms_to_deg(sL[1], " ");
    de = damas_to_deg(sL[2], " ");
    mag0 = sL[3].toFloat();
    ocRaCosDe = sL[4].toDouble();
    ocDe = sL[5].toDouble();
    ocMag = sL[6].toDouble();


    k=7;

    ocRaMM = sL[k++].toDouble();
    ocDeMM = sL[k++].toDouble();

            topDist = sL[k++].toDouble();
            muRaCosDe = sL[k++].toDouble();
            muDe = sL[k++].toDouble();
            Vr = sL[k++].toDouble();
            phase = sL[k++].toFloat();
            elong = sL[k++].toFloat();
            expTime = sL[k++].toDouble();
            name = sL[k++];
            catName = sL[k++];
            catMagName = sL[k++];
          //  mesureTimeCode = sL.at(k++);

}
/
void eqSeriesRec::s2recBase(QString recStr)
{
    QStringList sL = recStr.split(" ");
//	//if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nsize=" << sL.size() << "\n";
    /*if(sL.size()!=16&&sL.size()!=18) return;
    if(sL.size()==16)
    {
            vers = 0;
    }
    else if(sL.size()==18){vers = 1;}/
    int szl = sL.size();
    int k;

 /*

    name = = sL[0];
//    QString tstr = sL[1];
    MJday = jd2mjd(sL[1].toDouble());//getMJDfromYMD(tstr);
    hms2rad(&ra, sL[5].toInr(), sL[6].toInr(), sL[7].toDouble());
    de = damas_to_deg(sL[2], " ");
    mag0 = sL[3].toFloat();
    ocRaCosDe = sL[4].toDouble();
    ocDe = sL[5].toDouble();
    ocMag = sL[6].toDouble();


    k=7;

  //  ocRaMM = sL[k++].toDouble();
 //   ocDeMM = sL[k++].toDouble();

            topDist = sL[k++].toDouble();
            muRaCosDe = sL[k++].toDouble();
            muDe = sL[k++].toDouble();
            Vr = sL[k++].toDouble();
            phase = sL[k++].toFloat();
            elong = sL[k++].toFloat();
            expTime = sL[k++].toDouble();
            name = sL[k++];
            catName = sL[k++];
            catMagName = sL[k++];
          //  mesureTimeCode = sL.at(k++);
/
}

eqSeriesRec::eqSeriesRec()
{
    ocRec();
    epoch = 2000.0;
    catEphName = QString("none");
}

eqSeriesRec::~eqSeriesRec()
{
    ~ocRec();
    catEphName.clear();
}

eqSeriesRec::eqSeriesRec(QString str)
{

}


/////////////////////////////////////////////////////////////////////

/*
eqSeriesFile::eqSeriesFile()
{
        mmRec = NULL;
};

eqSeriesFile::~eqSeriesFile()
{
        delete(mmRec);// = NULL;
        ocList.clear();
        colList.clear();
};


void eqSeriesFile::clear()
{
        ocList.clear();
        colList.clear();
        if(mmRec!=NULL)
        {
                delete mmRec;
                mmRec = NULL;
        }
}

void eqSeriesFile::init(const char *fname)
{
//	ocList.clear();
    if(REDSTAT_LOG_LEVEL>0) qDebug() << "\neqFile::init: " << fname << "\n";
        clear();
        fName.clear();
        fName.insert(0, fname);
        QString dataLine;
        QFile dataFile(fname);
        if(!dataFile.open(QIODevice::ReadOnly| QIODevice::Text))
        {
            if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("can't open: %1\n").arg(fName);
            return;
        }
        QTextStream dataStream;
        dataStream.setDevice(&dataFile);

        eqSeriesRec* ocr;
        colRec* colr;
        moveModelRec* mmr;

        while (!dataStream.atEnd())
        {
                dataLine = dataStream.readLine();
                if(dataLine==NULL) continue;
                if(dataLine[0]=='#')		//mm-string
                {
                      //  if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nline #\n";
                        mmRec = new moveModelRec(dataLine);
//			ocr->s2rec(dataLine);
//			mmList << mmr;
                }
                else if(dataLine[0]=='%')	//col-string
                {
                       // if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nline \%\n";
                        colr = new colRec(dataLine);
//			ocr->s2rec(dataLine);
                        colList << colr;
                }
                else						//oc-string
                {
                       // if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nline\n";
                        ocr = new ocRec(dataLine);
//			ocr->s2rec(dataLine);
                        ocList << ocr;
                }
        }
        dataFile.close();

};

void eqSeriesFile::initOld(const char *fname)
{
//	ocList.clear();
        clear();
        fName.clear();
        fName.insert(0, fname);
        QString dataLine;
        QFile dataFile(fname);
        dataFile.open(QIODevice::ReadWrite| QIODevice::Text);
        QTextStream dataStream;
        dataStream.setDevice(&dataFile);

        ocRec* ocr;
        colRec* colr;
//	moveModelRec* mmr;
        int cNum=0;
        QStringList sL;
        QString tstr;

        while (!dataStream.atEnd())
        {
                dataLine = dataStream.readLine();
                if(dataLine[0]=='#')		//mm-string
                {
                        switch(cNum)
                        {
                                case 0:
                                        colr = new colRec(dataLine);
                                        colList << colr;
                                        cNum++;
                                break;
                                case 1:
                                        colr = new colRec(dataLine);
                                        colList << colr;
                                        cNum++;
                                break;
                                case 2:
                                        mmRec = new moveModelRec();
                                        sL = dataLine.split("|");
                                        if(sL.size()!=8) break;
                                        tstr = sL[0];
                                        mmRec->Tm = getMJDfromYMD(tstr.mid(1));
                                        mmRec->xTm = hms_to_mas(sL[1], " ");

                                        mmRec->nPosX = sL[2].toInt();
                                        mmRec->uweX = sL[3].toDouble();
                                        mmRec->rmsX = sL[4].toDouble();
                                        mmRec->Xdot = sL[5].toDouble();
                                        mmRec->rmsXdot = sL[6].toDouble();
                                        mmRec->XdotOC = sL[7].toDouble();
                                        cNum++;
                                break;
                                case 3:
                                        sL = dataLine.split("|");
                                        if(sL.size()!=8) break;
                                        mmRec->yTm = damas_to_mas(sL[1], " ");
                                        mmRec->nPosY = sL[2].toInt();
                                        mmRec->uweY = sL[3].toDouble();
                                        mmRec->rmsY = sL[4].toDouble();
                                        mmRec->Ydot = sL[5].toDouble();
                                        mmRec->rmsYdot = sL[6].toDouble();
                                        mmRec->YdotOC = sL[7].toDouble();
                                        cNum++;
                                break;
                                default:
                                break;
                        }


                }
                else						//oc-string
                {
//			//if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nline\n";
                        ocr = new ocRec(dataLine);
//			ocr->s2rec(dataLine);
                        ocList << ocr;
                }
        }

        dataFile.close();

};

void eqSeriesFile::save()
{
        QString tstr;
        sortColList();
        QFile dataFile(fName);
        dataFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
        QTextStream dataStream;
        dataStream.setDevice(&dataFile);

        int i, sz;

        sz = ocList.size();
        for(i=0;i<sz;i++)
        {
                tstr.clear();
                ocList[i]->rec2s(&tstr);
                dataStream << tstr;
        }

        sz = colList.size();
        for(i=0;i<sz;i++)
        {
                tstr.clear();
                colList[i]->rec2s(&tstr);
                dataStream << tstr << "\n";
        }
        if(mmRec!=NULL)
        {
                tstr.clear();
                mmRec->rec2s(&tstr);
                dataStream << tstr << "\n";
        }

        dataFile.close();
}

void eqSeriesFile::saveAs(QString fname)
{
        sortColList();
        fName.clear();
        fName.insert(0, fname);
        save();
}

int eqSeriesFile::getColRecNum(int colNum)
{
//	if(colRec==NULL) return -1;
        int i;
        int csz = colList.size();

        for(i=0; i<csz; i++)
        {
                if(colList[i]->colNum==colNum)
                {
//			cRec = colList[i];
                        return i;
                }
        }

        return -1;
}

int eqSeriesFile::setColRec(colRec* cRec)
{
        colRec *pRec;
        int rpos = getColRecNum(cRec->colNum);
//	//if(REDSTAT_LOG_LEVEL>0) qDebug() << "rpos= " << rpos << "\n";
        if(rpos==-1)
        {
                pRec = new colRec;
                colList << pRec;
                rpos = colList.size()-1;
        }

        colList[rpos]->colNum = cRec->colNum;
        colList[rpos]->num = cRec->num;
        colList[rpos]->mean = cRec->mean;
        colList[rpos]->rmsMean = cRec->rmsMean;
        colList[rpos]->rmsOne = cRec->rmsOne;

        return 0;
}

void eqSeriesFile::sortColList()
{
        int sz = colList.size();
        int nMin, pMin;

        for(int i=0; i<sz-1; i++)
        {
                nMin = colList[i]->colNum;
                pMin = i;
                for(int j=i+1; j<sz; j++)
                {
                        if(colList[j]->colNum<nMin){pMin = j; nMin = colList[j]->colNum;}
                }

                colList.swap(i, pMin);

        }
}

void eqSeriesFile::delMMrec()
{
        if(mmRec!=NULL) delete(mmRec);
        mmRec = NULL;
}

int eqSeriesFile::countCols(QString colNums)
{
    colList.clear();
    QStringList columns = colNums.split(",");
    int i, j, k, sz, sz1;
    QList <colRec *> cols;
    colRec *cRec;
    QString str;

    sz = ocList.size();
   // if(sz<=2) return 1;

            sz1 = columns.count();
            for(j=0;j<sz1;j++)
            {
                    str = columns[j];
                    cRec = new colRec;
                    cRec->colNum = str.toInt();
    //							means<<0;
    //							serrors<<0;
                    cols << cRec;
            }


            for(j=0;j<sz;j++)
            {
                    for(k=0;k<sz1;k++)
                    {
                            switch(cols[k]->colNum)
                            {
                                    case 0:
                                    cols[k]->mean += ocList[j]->MJday;
                                    cols[k]->num++;
                                    break;
                                    case 1:
                                    cols[k]->mean += ocList[j]->ra;
                                    cols[k]->num++;
                                    break;
                                    case 2:
                                    cols[k]->mean += ocList[j]->de;
                                    cols[k]->num++;
                                    break;
                                    case 3:
                                    cols[k]->mean += ocList[j]->mag0;
                                    cols[k]->num++;
                                    break;
                                    case 4:
                                    cols[k]->mean += ocList[j]->ocRaCosDe;
                                    cols[k]->num++;
                                    break;
                                    case 5:
                                    cols[k]->mean += ocList[j]->ocDe;
                                    cols[k]->num++;
                                    break;
                                    case 6:
                                    cols[k]->mean += ocList[j]->ocMag;
                                    cols[k]->num++;
                                    break;

                            case 7:
                            cols[k]->mean += ocList[j]->x;
                            cols[k]->num++;
                            break;
                            case 8:
                            cols[k]->mean += ocList[j]->y;
                            cols[k]->num++;
                            break;
                            case 9:
                            cols[k]->mean += ocList[j]->pixmag;
                            cols[k]->num++;
                            break;
                            case 10:
                            cols[k]->mean += ocList[j]->Dx;
                            cols[k]->num++;
                            break;
                            case 11:
                            cols[k]->mean += ocList[j]->Dy;
                            cols[k]->num++;
                            break;
                            case 12:
                            cols[k]->mean += ocList[j]->Dpixmag;
                            cols[k]->num++;
                            break;


                                    case 13:
                                    cols[k]->mean += ocList[j]->topDist;
                                    cols[k]->num++;
                                    break;
                                    case 14:
                                    cols[k]->mean += ocList[j]->muRa;
                                    cols[k]->num++;
                                    break;
                                    case 15:
                                    cols[k]->mean += ocList[j]->muDe;
                                    cols[k]->num++;
                                    break;
                                    case 16:
                                    cols[k]->mean += ocList[j]->Vr;
                                    cols[k]->num++;
                                    break;
                                    case 17:
                                    cols[k]->mean += ocList[j]->phase;
                                    cols[k]->num++;
                                    break;
                                    case 18:
                                    cols[k]->mean += ocList[j]->elong;
                                    cols[k]->num++;
                                    break;
                                    case 19:
                                    cols[k]->mean += ocList[j]->expTime;
                                    cols[k]->num++;
                                    break;
                            }
                    }
            }
            for(k=0;k<sz1;k++) cols[k]->mean /= cols[k]->num;

            for(j=0;j<sz;j++)
            {
                    for(k=0;k<sz1;k++)
                    {
                            switch(cols[k]->colNum)
                            {
                                    case 0:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->MJday, 2.0);
                                    break;
                                    case 1:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->ra, 2.0);
                                    break;
                                    case 2:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->de, 2.0);
                                    break;
                                    case 3:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->mag0, 2.0);
                                    break;
                                    case 4:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->ocRaCosDe, 2.0);
                                    break;
                                    case 5:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->ocDe, 2.0);
                                    break;
                                    case 6:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->ocMag, 2.0);
                                    break;

                            case 7:
                            cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->x, 2.0);
                            break;
                            case 8:
                            cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->y, 2.0);
                            break;
                            case 9:
                            cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->pixmag, 2.0);
                            break;
                            case 10:
                            cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->Dx, 2.0);
                            break;
                            case 11:
                            cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->Dx, 2.0);
                            break;
                            case 12:
                            cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->Dpixmag, 2.0);
                            break;

                                    case 13:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->topDist, 2.0);
                                    break;
                                    case 14:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->muRa, 2.0);
                                    break;
                                    case 15:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->muDe, 2.0);
                                    break;
                                    case 16:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->Vr, 2.0);
                                    break;
                                    case 17:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->phase, 2.0);
                                    break;
                                    case 18:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->elong, 2.0);
                                    break;
                                    case 19:
                                    cols[k]->rmsOne += pow(cols[k]->mean-ocList[j]->expTime, 2.0);
                                    break;
                            }
                    }
            }

            for(k=0;k<sz1;k++)
            {
                    cols[k]->rmsOne = sqrt(cols[k]->rmsOne/(cols[k]->num-1.0));
                    cols[k]->rmsMean = cols[k]->rmsOne/sqrt(cols[k]->num);
                    str.clear();
                    cols[k]->rec2s(&str);
//                    //if(REDSTAT_LOG_LEVEL>0) qDebug() << k << ": " << str << "\n";
                    setColRec(cols[k]);
            }

    sortColList();
    return 0;
}

int eqSeriesFile::reCountCols()
{
    QStringList colNums;
    int i, sz;
    sz = colList.size();

    for(i=0; i<sz; i++)
    {
        colNums << QString("%1").arg(colList[i]->colNum);
    }
    if(countCols(colNums.join(","))) return 1;
    return 0;
}

colRec* eqSeriesFile::getColNum(int cNum)
{
    int i, sz;
    sz = colList.size();
    for(i=0; i<sz; i++)
    {
        if(cNum==colList[i]->colNum) return colList[i];
    }
    return NULL;
}

void eqSeriesFile::do3sigma(double proofP, double sigmaMul)
{
    int num0, num1, i, sz;
    int num0E, num1E;
    double meanRa, meanDec;
    double sigmaKsi, sigmaEta;
    double maxOCKsi, maxOCEta;

    num1 = ocList.size();

    if(num1<3) return;

    do
    {
            num0 = num1;

            if((getColRecNum(4)!=-1)&&(getColRecNum(5)!=-1)) reCountCols();
            else countCols("4,5");

            meanRa = getColNum(4)->mean;
            meanDec = getColNum(5)->mean;

            sigmaKsi = getColNum(4)->rmsOne;
            sigmaEta = getColNum(5)->rmsOne;

            //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("\nsigmaKsi= %1\t\tsigmaEta= %2\n").arg(sigmaKsi).arg(sigmaEta);

            maxOCKsi = 3.0*sigmaKsi;
            maxOCEta = 3.0*sigmaEta;


            sz = ocList.size();
            for(i=sz-1; i>=0; i--)
            {
                if((fabs(meanRa-ocList[i]->ocRa)>maxOCKsi)||(fabs(meanDec-ocList[i]->ocDe)>maxOCEta)) ocList.removeAt(i);
            }
            num1 = ocList.size();

            //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("\nnum0= %1\t\tnum1= %2\n").arg(num0).arg(num1);


    }while(abs(num0-num1)>(proofP*num1));

}

int eqSeriesFile::countMM(int fp, int ft, int vflag)
{
    int sz, i, j;
    double t;

    delMMrec();
    sz = ocList.size();

    if(sz<=2) return 1;


        double *X = new double[sz];
        double *Y = new double[sz];
        double *Weights = new double[sz];
        double *C = new double[sz*2];
        double *Dx = new double[2*2];
        double *Dy = new double[2*2];
        double *Zx = new double[2];
        double *Zy = new double[2];
        double *uZx = new double[2];
        double *uZy = new double[2];
        int *excX = new int [sz];
        int *excY = new int [sz];
        double uweX, uweY;
        int rnX, rnY;

        double mt = 0,mxdot=0,mydot=0;
        QVector<double> times;
        for (j=0;j<sz;j++)
        {
                switch(ft)
                {
                        case 0:{t = ocList.at(j)->MJday;break;}
                }
                mt = mt + t;
                times << t;
                if(vflag==1)
                {
                        //elS = dataLine.section('|',xdc,xdc);
                        mxdot = mxdot + ocList.at(j)->muRa;// elS.toDouble();
                        //elS = dataLine.section('|',ydc,ydc);
                        mydot = mydot + ocList.at(j)->muDe;// elS.toDouble();
                }
        }
        mt = mt/(sz*1.0);//dataLines.count();
        if(vflag==1)
        {
                mxdot = mxdot/(1.0*sz);//dataLines.count();
                mydot = mydot/(1.0*sz);//dataLines.count();
        }
        for (j=0;j<sz;j++)
        {
                Weights[j]=1.0;
                //dataLine = dataLines[j];
                switch(fp)
                {
                        case 0:{X[j] = ocList.at(j)->ra;Y[j]=ocList.at(j)->de;break;}
                            default:{X[j] = ocList.at(j)->ra;Y[j]=ocList.at(j)->de;break;}

                }
                C[j*2] = 1;C[j*2+1] = times[j]-mt;
        }
//						matrix<double> Zx = LSM_corr(X, C, Weights, 500);
//						matrix<double> Zy = LSM_corr(Y, C, Weights, 500);
        uweX = uweY = 0.0;
        iLSM(2, sz, 500, excX, Zx, C, X, uweX, Dx, 3, rnX, Weights);
        iLSM(2, sz, 500, excY, Zy, C, Y, uweY, Dy, 3, rnY, Weights);

        if(rnX<=2||rnY<=2) return 2;

/*        lsm(2, sz, Zx, C, X, uweX, Dx, Weights);
        lsm(2, sz, Zy, C, Y, uweY, Dy, Weights);
        rnX = rnY = sz;
/
        //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("Zx: %1 %2\tuweX: %3\n").arg(Zx[0]).arg(Zx[1]/1440.0).arg(uweX);
        //if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("Zy: %1 %2\tuweY: %3\n").arg(Zy[0]).arg(Zy[1]/1440.0).arg(uweY);
        uZx[0] = sqrt(uweX*Dx[0]);
        uZx[1] = sqrt(uweX*Dx[3]);
        uZy[0] = sqrt(uweY*Dy[0]);
        uZy[1] = sqrt(uweY*Dy[3]);
//              slsm(2, int h, double *const z, const double* c, const double* L, const double* W = 0);
        //END apparent motion parameters
        //BEGIN output

        mmRec = new moveModelRec;
        mmRec->Tm = mt;
        mmRec->xTm = Zx[0];
        mmRec->yTm = Zy[0];
        mmRec->nPosX = rnX;
        mmRec->uweX = uweX;
        mmRec->rmsX = uZx[0];
        mmRec->Xdot = Zx[1]/1440.0;
        mmRec->rmsXdot = uZx[1]/1440.0;
        mmRec->XdotOC = Zx[1]/1440.0 - mxdot;
        mmRec->nPosY = rnY;
        mmRec->uweY = uweY;
        mmRec->rmsY = uZy[0];
        mmRec->Ydot = Zy[1]/1440.0;
        mmRec->rmsYdot = uZy[1]/1440.0;
        mmRec->YdotOC = Zy[1]/1440.0 - mydot;

        mmRec->fp = fp;
        mmRec->ft = ft;
        mmRec->vflag = vflag;

        return 0;

}
void eqSeriesFile::sortOClist()
{
    int sz = ocList.size();
    int nMin, pMin;

    for(int i=0; i<sz-1; i++)
    {
            nMin = ocList[i]->MJday;
            pMin = i;
            for(int j=i+1; j<sz; j++)
            {
                    if(ocList[j]->MJday<nMin){pMin = j; nMin = ocList[j]->MJday;}
            }

            ocList.swap(i, pMin);

    }
}

void eqSeriesFile::findSeries(QList <eqFile*> *eqList)
{
    eqFile *eqTemp;
    int i, szi, j;
    szi = ocList.size();

    sortOClist();

    double timeT = 0.0;
    double timeC, dt0, dt1;
    dt1 = 0;
    j=-1;
    for(i=0; i<szi; i++)
    {
        dt0 = dt1;
        timeC = ocList.at(i)->MJday;
        dt1 = timeC-timeT;
        if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("timeC= %1\tdt0= %2\tdt1= %3\n").arg(timeC, 12, 'f', 5).arg(dt0).arg(dt1);
        if(dt1>1.5*dt0)
        {
            if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("\nnew serie\n");
            eqTemp = new eqFile;
            *eqList << eqTemp;
            j++;
        }
        //dt = fabs(timeC-timeT);
        eqList->at(j)->ocList << ocList.at(i);
        timeT = timeC;
    }

    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("eqList size: %1\n").arg(eqList->size());
}

void eqSeriesFile::getSeriesRec(eqSeriesRec *eqsRec)
{
    countCols("0,1,2,3,4,5,6,13,14,15,16,17,18,19");
    sortColList();
    countMM();
    if(REDSTAT_LOG_LEVEL>0) qDebug() << "mmRec: " << mmRec << "\n";

    colRec* cRec;


    cRec = getColNum(0);
    if(cRec!=NULL) eqsRec->MJday = cRec->mean;

    cRec = getColNum(1);
    if(cRec!=NULL) eqsRec->ra = cRec->mean;

    cRec = getColNum(2);
    if(cRec!=NULL) eqsRec->de = cRec->mean;

    cRec = getColNum(3);
    if(cRec!=NULL) eqsRec->mag0 = cRec->mean;

    cRec = getColNum(4);
    if(cRec!=NULL) eqsRec->ocDe = cRec->mean;

    cRec = getColNum(5);
    if(cRec!=NULL) eqsRec->ocRa = cRec->mean;

    cRec = getColNum(6);
    if(cRec!=NULL) eqsRec->ocMag = cRec->mean;

    cRec = getColNum(13);
    if(cRec!=NULL) eqsRec->topDist = cRec->mean;

    cRec = getColNum(14);
    if(cRec!=NULL) eqsRec->muRa = cRec->mean;

    cRec = getColNum(15);
    if(cRec!=NULL) eqsRec->muDe = cRec->mean;

    cRec = getColNum(16);
    if(cRec!=NULL) eqsRec->Vr = cRec->mean;

    cRec = getColNum(17);
    if(cRec!=NULL) eqsRec->phase = cRec->mean;

    cRec = getColNum(18);
    if(cRec!=NULL) eqsRec->elong = cRec->mean;

    cRec = getColNum(19);
    if(cRec!=NULL) eqsRec->expTime = cRec->mean;

    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("\n19 mean\n");

    if(mmRec!=NULL)
    {
        if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("ra: %1\tmmRa: %2\n").arg(eqsRec->ra).arg(mmRec->xTm);
        if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("de: %1\tmmDe: %2\n").arg(eqsRec->de).arg(mmRec->yTm);
        eqsRec->ocRaMM = grad_to_mas(eqsRec->ra - mmRec->xTm);
        eqsRec->ocDeMM = grad_to_mas(eqsRec->de - mmRec->yTm);
    }
    else
    {
        eqsRec->ocRaMM = 0.0;
        eqsRec->ocDeMM = 0.0;
    }

    int i, szi;
    szi = ocList.size();

    eqsRec->name=QString("");
    eqsRec->catName=QString("");
    eqsRec->catMagName=QString("");

    for(i=0; i<szi; i++)
    {
        if((eqsRec->name=="")||(eqsRec->name==ocList.at(i)->name))eqsRec->name=ocList.at(i)->name;
        else eqsRec->name=QString("var");

        if((eqsRec->catName=="")||(eqsRec->catName==ocList.at(i)->catName))eqsRec->catName=ocList.at(i)->catName;
        else eqsRec->catName=QString("var");

        if((eqsRec->catMagName=="")||(eqsRec->catMagName==ocList.at(i)->catMagName))eqsRec->catMagName=ocList.at(i)->catMagName;
        else eqsRec->catMagName=QString("var");
    }
}
*/
//////////////////////////////////////////////////////////////////////
int countColStat(colRec *cRec, double *vect, int sz)
{
    int i, num;
    if(sz<2) return 1;

    double mean, rmsMean, rmsOne;

    mean = rmsMean = rmsOne = 0.0;

    for(i=0; i<sz; i++) mean += vect[i];
    mean /= 1.0*sz;

    if(REDSTAT_LOG_LEVEL>0) qDebug() << QString("mean: %1\n").arg(mean);

    for(i=0; i<sz; i++) rmsOne += pow(mean - vect[i], 2.0);
    rmsOne = sqrt(rmsOne/(sz-1.0));
    rmsMean = rmsOne/sqrt(sz);

    cRec->num = sz;
    cRec->mean = mean;
    cRec->rmsMean = rmsMean;
    cRec->rmsOne = rmsOne;

    return 0;
}

ocRecO::ocRecO() : ocRec()
{
    obsCode = "";
}

void ocRecO::rec2s(QString &str)
{
        str.clear();
//	DATEOBS dobs = getDATEOBSfromMJD(MJday);

        str.insert(0, getStrFromDATEOBS(getDATEOBSfromMJD(MJday), " ", 0, 5) + "|" + deg_to_hms(ra, " ", 4) + "|"+ deg_to_damas(de, " ", 3) + "|" + QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11|%12").arg(mag0, 6, 'f', 3, QLatin1Char(' ')).arg(ocRaCosDe, 8, 'f', 1, QLatin1Char(' ')).arg(ocDe, 8, 'f', 1, QLatin1Char(' ')).arg(ocMag, 8, 'f', 1, QLatin1Char(' ')).arg(obsCode, 3).arg(topDist, 12, 'f', 9, QLatin1Char(' ')).arg(muRaCosDe, 9, 'f', 2, QLatin1Char(' ')).arg(muDe, 9, 'f', 2, QLatin1Char(' ')).arg(Vr, 6, 'f', 2, QLatin1Char(' ')).arg(phase, 8, 'f', 3, QLatin1Char(' ')).arg(elong, 8, 'f', 3, QLatin1Char(' ')).arg(expTime, 8, 'f', 3, QLatin1Char(' ') )+QString("|%1|%2|%3|%4").arg(name, 16).arg(catName, 12).arg(catMagName, 8).arg(mesureTimeCode));

}

void ocRecO::s2rec(QString str)
{
    QStringList sL = str.split("|");
//	//if(REDSTAT_LOG_LEVEL>0) qDebug() << "\nsize=" << sL.size() << "\n";
        /*if(sL.size()!=16&&sL.size()!=18) return;
    if(sL.size()==16)
    {
        vers = 0;
    }
        else if(sL.size()==18){vers = 1;}*/
        int szl = sL.size();
        int k;

    QString tstr = sL[0];

        MJday = getMJDfromYMD(tstr);
        ra = hms_to_deg(sL[1], " ");
        de = damas_to_deg(sL[2], " ");
    mag0 = sL[3].toFloat();
        ocRaCosDe = sL[4].toDouble();
    ocDe = sL[5].toDouble();
    ocMag = sL[6].toDouble();


        k=7;
                obsCode = sL[k++];

                topDist = sL[k++].toDouble();
                muRaCosDe = sL[k++].toDouble();
                muDe = sL[k++].toDouble();
                Vr = sL[k++].toDouble();
                phase = sL[k++].toFloat();
                elong = sL[k++].toFloat();
                expTime = sL[k++].toDouble();
                name = sL[k++];
                catName = sL[k++];
                catMagName = sL[k++];
                mesureTimeCode = sL.at(k++);
};
