#include <QApplication>
#include <QtGui>
//#include "OpenFileExDlg.h"
//#include "fitsio.h"
#include <iostream>
#include <QtCore>
#include <QDataStream>

//#include "conio.h"
#include "math.h"
#include "./../libs/astro.h"
#include "./../libs/redStat.h"
#include "./../libs/multidim.h"
#include "./../libs/vectGrid3D.h"
//#include "gammaf.h"
/*
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
*/
QTextStream out_stream;

struct seriesOne
{
    QList <colRec*> colRa;
    QList <colRec*> colDec;
    QList <ocRec*> ocList;

    void removeAt(int pos);
};

void seriesOne::removeAt(int pos)
{
    ocList.removeAt(pos);
    colRa.removeAt(pos);
    colDec.removeAt(pos);
}

double qU(QList <colRec*> *colList)
{
    int i;
    int k = colList->size();
    double summ1, summ2;
    summ1 = summ2 = 0.0;
    for(i=0; i<k; i++)
    {
        summ1 += 1.0/(colList->at(i)->num-1.0);
        summ2 += colList->at(i)->num;
    }
    summ2 = 1.0/(summ2 - k);

    return(1.0/(1.0 + (1.0/3.0/(k-1.0))*(summ1 - summ2)));
}

double sigmaN(QList <colRec*> *colList)
{
    int i;
    double summ0, summ1, rms;
    int k = colList->size();
    summ0 = summ1 = 0.0;
    for(i=0; i<k; i++)
    {
        summ0 += (colList->at(i)->num-1)*pow(colList->at(i)->rmsOne, 2.0);
        summ1 += colList->at(i)->num;
    }
    summ1 -= k;
  //  out_stream << QString("summ0: %1\tsumm1: %2\n").arg(summ0).arg(summ1);

    rms = summ0/summ1;
    return(rms);
}

double lambdA(QList <colRec*> *colList)
{
    int i;
    int k = colList->size();
    double q, s2;

    s2 = sigmaN(colList);
    out_stream << QString("s2: %1\n").arg(s2, 8, 'f', 3);
    q = qU(colList);
 //   qDebug() << QString("%1:%2\t%3\n").arg(i, 4).arg(data->colDec.at(i)->rmsOne, 8, 'f', 3).arg(data->colDec.at(i)->num, 4);
    out_stream << QString("q: %1\n").arg(q, 8, 'f', 3);
  //  out_stream << QString("s2: %1\n").arg(s2);

    double summ = 0.0;

    for(i=0; i<k; i++) summ += (colList->at(i)->num - 1.0)*log(s2/pow(colList->at(i)->rmsOne, 2));
    return(q*summ);
}

void removeMax(seriesOne* data, int t)
{
    double maxS;
    double val;
    int i, pMax;
    int k=data->colRa.size();
    if(t==0) val = data->colRa[0]->rmsOne;
    if(t==1) val = data->colDec[0]->rmsOne;
    maxS = val;
    pMax = 0;

    for(i=1; i<k; i++)
    {
        if(t==0) val = data->colRa[i]->rmsOne;
        if(t==1) val = data->colDec[i]->rmsOne;
        if(maxS<val)
        {
            maxS = val;
            pMax = i;
        }
    }
    qDebug() << QString("maxS= %1\t\tpMax= %2\n").arg(maxS).arg(pMax);
    out_stream << QString("maxS= %1\t\tpMax= %2\n").arg(maxS).arg(pMax);
    data->removeAt(pMax);
    /*
    data->ocList.removeAt(pMax);
    data->colRa.removeAt(pMax);
    data->colDec.removeAt(pMax);*/
}


int bartLet(seriesOne* data, QList <double> xi2)
{
    double lambdaRa, lambdaDec, qRa, qDec;
    int i, j;
    int k = data->colRa.size();

    qDebug() << QString("Bartlet\n\n");
    out_stream << QString("Bartlet\n\n");

    do
    {

        qDebug() << QString("colRa, rmsOne:\n");
        out_stream << QString("colRa, rmsOne:\n");
        for(i=0; i<data->colRa.size(); i++)
        {
            qDebug() << QString("%1:%2\t%3\n").arg(i, 4).arg(data->colRa.at(i)->rmsOne, 8, 'f', 3).arg(data->colRa.at(i)->num, 4);
            out_stream << QString("%1:%2\t%3\n").arg(i, 4).arg(data->colRa.at(i)->rmsOne, 8, 'f', 3).arg(data->colRa.at(i)->num, 4);
        }
        //
        lambdaRa = lambdA(&data->colRa);
       //
        qDebug() << QString("colDec, rmsOne:\n");
        out_stream << QString("colDec, rmsOne:\n");
        for(i=0; i<data->colRa.size(); i++)
        {
            qDebug() << QString("%1:%2\t%3\n").arg(i, 4).arg(data->colDec.at(i)->rmsOne, 8, 'f', 3).arg(data->colDec.at(i)->num, 4);
            out_stream << QString("%1:%2\t%3\n").arg(i, 4).arg(data->colDec.at(i)->rmsOne, 8, 'f', 3).arg(data->colDec.at(i)->num, 4);
        }
        //
        lambdaDec = lambdA(&data->colDec);

         //
        qDebug() << QString("\nlambdaRa= %1\n").arg(lambdaRa);
        out_stream << QString("\nlambdaRa= %1\n").arg(lambdaRa);
        //
        //
        qDebug() << QString("lambdaDec= %1\n").arg(lambdaDec);
        out_stream << QString("lambdaDec= %1\n").arg(lambdaDec);
        //
        qDebug() << QString("k: %1\txi2= %2\n\n").arg(k).arg(xi2[k-1]);
        out_stream << QString("k: %1\txi2= %2\n\n").arg(k).arg(xi2[k-1]);
        //


        if(lambdaRa>=xi2[k-1])
        {
            removeMax(data, 0);
        }
        else if(lambdaDec>=xi2[k-1])
        {
            removeMax(data, 1);
        }
        else
        {
            return 0;   //good
        }

        k = data->colRa.size();

    }while(k>2);
    return 1; //bad
}

void detMeanCols(QString fName)
{
    QFile fData(fName);
    fData.open(QIODevice::ReadWrite | QIODevice::Text);
    QTextStream dataS(&fData);

    QList <colRec*> colList;
    colRec *cRec;

    while(!dataS.atEnd())
    {
        cRec = new colRec;
        cRec->s2rec(dataS.readLine());
        colList << cRec;
    }
    dataS << "\n==========================\n";

    int sz = colList.size();
    qDebug() << "\nsz(fName)= " << sz << "\n";
    cRec = new colRec;
    cRec->colNum = colList[0]->colNum;
    for(int i=0; i<sz; i++)
    {
        cRec->mean += colList[i]->mean;
        cRec->rmsMean += colList[i]->rmsMean;
        cRec->rmsOne += colList[i]->rmsOne;
    }
    cRec->mean /= 1.0*sz;
    cRec->rmsMean /= 1.0*sz;
    cRec->rmsOne /= 1.0*sz;
    QString tstr;

    cRec->rec2s(&tstr);
    dataS << tstr << "\n";

    fData.close();

}


int main(int argc, char *argv[])
{  
	///////////////////////////
	//QTranslator translator;
       //translator.load("astrored2_ru");
       //app.installTranslator(&translator);
       /////////////////////////////////
/*         COpenFileExDlg *mainWin = new COpenFileExDlg;
       mainWin->show(); */
       
	
	QApplication app(argc, argv);
	
	
	int i, j, k;

	QFile fout("./fout.dat");
	fout.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
	
	out_stream.setDevice(&fout);

	QSettings *settings = new QSettings("./mpcstat.ini",QSettings::IniFormat);
	QString workingFolder = settings->value("general/workingFolder").toString();
	QString outputFolder = settings->value("general/outputFolder").toString();
	int taskNum = settings->value("general/taskNum").toInt();
	
//	double velMin = settings->value("task14/velMin").toDouble();
	double velMax = settings->value("general/velMax").toDouble();
	
	int isSysCorr = settings->value("general/isSysCorr").toInt();
	QString sysCorrFile = settings->value("general/sysCorrFile").toString();
        QStringList excNames = settings->value("general/excNames").toString().split("|");
        int expMin = settings->value("general/expMin").toInt();
        int sigmaTest = settings->value("general/sigmaTest", 0).toInt();


 	QStringList flist;
        QStringList dirlist;
    
        int mLevNum, xLevNum, yLevNum, tLevNum;
        double mLevNumBeg, mLevStep;
//	int taskNum;
	int numTot;
	
	double *mLevels;
	double *xLevels;
	double *yLevels;
	double *decLevels;
	
	double *tLevels;
	
	long naxeX[2];
	long naxeY[2];
	double naxeDe[2];
	double naxeDe0[2];
	
	double ocMagMax, magMin;
	double ksiVal, etaVal;
	
	double dX, dY;
	
	QString timeDiapsStr;
	QStringList timeDiapsSL;
	

	multidim *xVectField;
	multidim *xDispVectField;
	multidim *yVectField;
	multidim *yDispVectField;
	multidim *vectNumField;
	int axesNum;
	int *axes;
	int *pos;
	QString dataLine;
	QFile *resDataF;
//	QFile dataFile;
	
	double stepDe;
        int p0, sz, sz1;
	residualsRec *resRec;
	long nums;
	int cpos;
	int numMin;
	QTextStream resStream;
	QTextStream resStream0;
	QStringList xDiap;
	QStringList yDiap;
	QStringList mDiap;
	
	double ocX, ocY;
        QTextStream resDispStream;
	
	QFile resFile;
	QFile resFile0;
	QFile resDispFile;
        QFile errFile0, errFile1;
        QTextStream errDataStream0, errDataStream1;
        QFile errFileCor0, errFileCor1;
        QTextStream errDataStreamCor0, errDataStreamCor1;
	
	ocMagMax = settings->value("general/ocMagMax").toDouble();
	magMin = settings->value("general/magMin").toDouble();
	
//        QList <colRec*> colList0, colList1;
//        colRec *cRec;

	out_stream << "taskNum= " << taskNum << "\n";
/*
	QDir dir(workingFolder);
	QStringList filters;
	filters << "*.txt";
	dir.setNameFilters(filters); 
   
    dirlist << dir.entryList();
    
//	for(i=0;i<di;i++) out_stream << flist[i] << "\n";
	
	
*/
	QString tstr;
	QDirIterator it(workingFolder, QDirIterator::Subdirectories);

        int objType = 0;            //stars
        if(taskNum==0||taskNum==2||taskNum==7||taskNum==8||taskNum==9||taskNum==10||taskNum==12||taskNum==13||taskNum==14||taskNum==15||taskNum==16) objType = 1; //aster
        if(taskNum==1) objType = 2; //txt



	while (it.hasNext()) 
	{
		tstr = it.next();
                if(objType==1)
		{
			if((tstr.lastIndexOf("/..")!=tstr.lastIndexOf("/"))&&(tstr.lastIndexOf("/.")!=tstr.lastIndexOf("/"))&&(tstr.lastIndexOf("_eq.txt")==(tstr.size()-7))) flist << tstr;
                        out_stream.setDevice(&fout);
		}
                else if(objType==2){if((tstr.lastIndexOf("/..")!=tstr.lastIndexOf("/"))&&(tstr.lastIndexOf("/.")!=tstr.lastIndexOf("/"))&&(tstr.lastIndexOf(".txt")==(tstr.size()-4))) flist << tstr;}
		else{if((tstr.lastIndexOf("/..")!=tstr.lastIndexOf("/"))&&(tstr.lastIndexOf("/.")!=tstr.lastIndexOf("/"))&&(tstr.lastIndexOf("residuals.txt")==(tstr.size()-13))) flist << tstr;}
	}
	
	vectGrid3D *vectF;// = new vectGrid3D;
	QByteArray tmp;
	double dKsi, dEta;
	long ni;
	
	if(isSysCorr)
	{
		vectF = new vectGrid3D;
		tmp = sysCorrFile.toAscii();
		vectF->init(tmp.data());
		vectF->printCoefs();
	}
	qDebug() << "\nInitial end\n";
	int di = flist.count();
        qDebug() << QString("di=%1\n").arg(di);
        out_stream << QString("Total number of files = %1\n").arg(di);
//	for (i=0;i<di;i++)flist <<workingFolder+"/"+dirlist.at(i);
	
	int isFindDiap;
	double maxOC;
	double maxOCK, maxOCE;
	double maxKsi, maxEta;
	double sigmaKsi, meanKsi;
	double sigmaEta, meanEta;
	
	const char *fname;
//	QByteArray tmp;
	QString tstr1;
	QString dstr, resStr, eqStr;
        double dDec;
	
	int ri;
	int rti;
	int kp;
	int diniKey;
        eqFile *eqAsts = new eqFile();
	eqFile *eqData = new eqFile();
        eqFile *eqTemp = new eqFile();

	residualFile *resData = new residualFile();
	double tTm;
	double axd;
	qDebug() << "\n";

        ocRec *ocMean;

        QList <colRec*> ocRa0;
        QList <colRec*> ocRa1;
        QList <colRec*> ocDec0;
        QList <colRec*> ocDec1;

        QString curName;

        QList <ocRec*> ocTempList;
        QList <ocRec*> ocIniList;
        QList <ocRec*> ocResList;
        colRec* cRec;
        ocRec *ocr;
        int szCols;
  /*      QList <colRec*> colList;
        for(i=0;i<15;i++)
        {
            cRec
        }
*/
	int isDetSigma = settings->value("general/isDetSigma").toInt();
	int counterK;
	int counterE;
	int num0K, num1K;
	int num0E, num1E;
        double proofP = settings->value("general/proofPer").toDouble()*0.01;
        qDebug() << "\nproofR= " << proofP << "\n";
	double meanRa, meanDec;
	double sigmaRa, sigmaDec;
	double calcRa, calcDec;
	double obsRa, obsDec;
	double ksiObs, etaObs;
        double ksiCalc, etaCalc;
	int count;
        int ocSizeMax = settings->value("general/ocSizeMax").toInt();
        int ocSizeMin = settings->value("general/ocSizeMin").toInt();

        int inext;
        double t0, dT;

        out_stream << QString("\nexcluded names:%1\n").arg(excNames.join(","));



        if(objType==1)
        {
       /*
                eqData->clear();
                tmp = flist[i].toAscii();
                fname = tmp.data();
                eqData->init(fname);
                inext=0;
                for(j=0;j<excNames.size();j++)
                    if(eqData->ocList[0]->name==excNames[j])
                    {
                        inext=1;
                        out_stream << "\n" << eqData->ocList[0]->name << " - excluded\n";
                        continue;
                    }
                if(inext) continue;
                if((int)(eqData->ocList[0]->expTime)<expMin) continue;
//                                       out_stream << "\nfn: " << fname << "\n";
//                eqData->reCountCols();

                sz = eqData->ocList.size();
                eqData->do3sigma(proofP);
                eqData->reCountCols();


                if((eqData->ocList.size()>2)&&(eqData->ocList.size()<=ocSizeMax)&&(eqData->colList.size()>=2)&&(eqData->colList[6]->mean<ocMagMax)&&(eqData->colList[3]->mean>magMin)&&(sqrt(pow(eqData->mmRec->Xdot, 2.0) + pow(eqData->mmRec->Ydot, 2.0))<velMax))//&&(eqData->colList[6]->mean<ocMagMax)&&(eqData->colList[3]->mean>magMin))
                {
                    cRec = new colRec;
                    cRec = eqData->colList[4];
//                    c = eqData->ocList.size();
                    ocRa0 << cRec;
                    cRec = new colRec;
                    cRec = eqData->colList[5];
    //                cRec->colNum = eqData->ocList.size();
                    ocDec0 << cRec;


                    sz1 = eqData->ocList.size();

                    if(sz!=sz1) out_stream << QString("\nsz0= %1\t\tsz1= %2\n").arg(sz).arg(sz1);


                    if(isSysCorr)
                    {
 //                      sz1 = eqData->ocList.size();

                        for(j=0; j<sz1; j++)
                        {
                            calcRa = eqData->ocList[j]->ra - eqData->ocList[j]->ocRa*cos(mas_to_rad(eqData->ocList[j]->de));//>colList[4]->mean;
                            calcDec = eqData->ocList[j]->de - eqData->ocList[j]->ocDe;//eqData->colList[2]->mean - eqData->colList[5]->mean;

                            //dDec = -0.5082077*rad2grad(mas_to_rad(eqData->ocList[j]->de)) + 18.1648793;
    //								out_stream << "calcRa " << calcRa << "\tcalcDec " << calcDec << "\n";

                            getRaDeToTang1(&ksiObs, &etaObs, eqData->ocList[j]->ra, eqData->ocList[j]->de, eqData->ocList[j]->ra_oc, eqData->ocList[j]->dec_oc);
                            getRaDeToTang1(&ksiCalc, &etaCalc, calcRa, calcDec, eqData->ocList[j]->ra_oc, eqData->ocList[j]->dec_oc);
    //								out_stream << "ksiObs " << ksiObs << "\tetaObs " << etaObs << "\tmagn " << eqData->colList[3]->mean <<  "\n";
                            if(eqData->ocList[j]->mag0<0.0) eqData->ocList[j]->mag0 = 0.1;
                            vectF->int2D(ksiObs, etaObs, eqData->ocList[j]->mag0, &dKsi, &dEta, &ni);
    //								out_stream << "dKsi " << dKsi << "\tdEta " << dEta << "\n";
                            dKsi /= 1000.0;
                            dEta /= 1000.0;
                            ksiObs -= dKsi;
                            etaObs -= dEta;
    //								out_stream << "ksiObs " << ksiObs << "\tetaObs " << etaObs << "\n";
                            getTangToRaDe1(&obsRa, &obsDec, ksiObs, etaObs, eqData->ocList[j]->ra_oc, eqData->ocList[j]->dec_oc);
    //								out_stream << "obsRa " << obsRa << "\tobsDec " << obsDec << "\n";
                            eqData->ocList[j]->ra = obsRa;
                            eqData->ocList[j]->de = obsDec - dDec;
                            eqData->ocList[j]->ocRa = (ksiObs - ksiCalc)*1000.0;
                            eqData->ocList[j]->ocDe = (etaObs - etaCalc)*1000.0;// - dDec;
                        }
                    }


                    eqData->reCountCols();

                    ////////////////////////////////////////////////////////

                    cRec = new colRec;
                    cRec = eqData->colList[4];
      //              cRec->colNum = eqData->ocList.size();
                    ocRa1 << cRec;
                    cRec = new colRec;
                    cRec = eqData->colList[5];
       //             cRec->colNum = eqData->ocList.size();
                    ocDec1 << cRec;

                    sz1 = eqData->colList.size();

                    ocMean = new ocRec;
//               MJday|ra|de|mag0|ocRa|ocDe|ocMag|ra_oc|de_oc|topDist|muRa|muDe|Vr|phase|elong|name|catNum|expTime
//Tm|xTm|yTm|nPos|uweX|uweY|rmsX|rmsY|Xdot|rmsXdot|XdotOC|Ydot|rmsYdot|YdotOC
//#2007 01 17.95120|08 16 58.1650|+10 45 29.429|  4| 231.2| 147.7| 113.6|   -607.97|  33.88|  73.9|    284.14|  22.04
                    ocMean->MJday = eqData->colList[0]->mean;
                    ocMean->ra = eqData->colList[1]->mean;
                    ocMean->de = eqData->colList[2]->mean;
                    ocMean->mag0 = eqData->colList[3]->mean;
                    ocMean->ocRa = eqData->colList[4]->mean;
                    ocMean->ocDe = eqData->colList[5]->mean;
                    ocMean->ocMag = eqData->colList[6]->mean;
                    ocMean->ra_oc = eqData->colList[7]->mean;
                    ocMean->dec_oc = eqData->colList[8]->mean;
                    ocMean->topDist = eqData->colList[9]->mean;
                    ocMean->muRa = eqData->mmRec->Xdot;//>colList[10]->mean;
                    ocMean->muDe = eqData->mmRec->Ydot;//colList[11]->mean;
                    ocMean->Vr = eqData->colList[12]->mean;
                    ocMean->phase = eqData->colList[13]->mean;
                    ocMean->elong = eqData->colList[14]->mean;
                    ocMean->catNum = eqData->ocList[1]->catNum;
                    ocMean->name = QString(eqData->ocList[1]->name);
                    ocMean->expTime = eqData->ocList[1]->expTime;
                    eqAsts->ocList << ocMean;
                }
                */


            for(i=0;i<di;i++)
            {
                qDebug() << QString("%1:fname=%2\n").arg(i).arg(flist[i]);
                out_stream << i << ": " << flist[i] << "\n";
                eqData->clear();
                tmp = flist[i].toAscii();
                fname = tmp.data();
                eqData->init(fname);
                eqData->countMM();
                eqData->save();
                if(eqData->ocList.size()==0) continue;
                inext=0;
                for(j=0;j<excNames.size();j++)
                    if(eqData->ocList[0]->name==excNames[j])
                    {
                        inext=1;
                        out_stream << "\n" << eqData->ocList[0]->name << " - excluded\n";
                        continue;
                    }
                if(inext) continue;
                if((int)(eqData->ocList[0]->expTime)<expMin) continue;

                eqData->do3sigma(proofP, 3.0);
                eqData->reCountCols();


                sz = eqData->ocList.size();
                for(j=0; j<sz; j++)
                {
                    ocr = new ocRec;
/*

                    if(isSysCorr)
                    {
                        calcRa = eqData->ocList[j]->ra - eqData->ocList[j]->ocRa*cos(mas_to_rad(eqData->ocList[j]->de));//>colList[4]->mean;
                        calcDec = eqData->ocList[j]->de - eqData->ocList[j]->ocDe;//eqData->colList[2]->mean - eqData->colList[5]->mean;

                        getRaDeToTang1(&ksiObs, &etaObs, eqData->ocList[j]->ra, eqData->ocList[j]->de, eqData->ocList[j]->ra_oc, eqData->ocList[j]->dec_oc);
                        getRaDeToTang1(&ksiCalc, &etaCalc, calcRa, calcDec, eqData->ocList[j]->ra_oc, eqData->ocList[j]->dec_oc);
                        if(eqData->ocList[j]->mag0<0.0) eqData->ocList[j]->mag0 = 0.1;
                        vectF->int2D(ksiObs, etaObs, eqData->ocList[j]->mag0, &dKsi, &dEta, &ni);
                        dKsi /= 1000.0;
                        dEta /= 1000.0;
                        ksiObs -= dKsi;
                        etaObs -= dEta;
                        getTangToRaDe1(&obsRa, &obsDec, ksiObs, etaObs, eqData->ocList[j]->ra_oc, eqData->ocList[j]->dec_oc);
                        eqData->ocList[j]->ra = obsRa;
                        eqData->ocList[j]->de = obsDec - dDec;
                        eqData->ocList[j]->ocRa = (ksiObs - ksiCalc)*1000.0;
                        eqData->ocList[j]->ocDe = (etaObs - etaCalc)*1000.0;// - dDec;
                    }
*/
                    ocr = eqData->ocList[j];
                    ocIniList << ocr;
                }
            }

            //
            eqData->clear();
            eqAsts->clear();
            sz = ocIniList.size();
            while(sz>0)
            {
                ocTempList.clear();
                curName = QString("");
                eqTemp->clear();

                qDebug() << QString("ocIniList size=%1\n").arg(sz);
                out_stream << QString("ocIniList size=%1\n").arg(sz);

                for(i=sz-1; i>=0; i--)
                {
                    if(curName=="")
                    {
                        curName = ocIniList.at(i)->name;
                        qDebug() << QString("curName= %1\n").arg(curName);
                        out_stream << QString("curName= %1\n").arg(curName);
                    }

                    if(curName==ocIniList.at(i)->name)
                    {
                        ocr = new ocRec;
                        ocr = ocIniList.at(i);
                        eqTemp->ocList << ocr;
//                        ocTempList << ocr;
                        ocIniList.removeAt(i);
                    }
                }

                /////////Грубая отбраковка
                sz = eqTemp->ocList.size();
                qDebug() << QString("ocTempList Before size=%1\n").arg(sz);
                out_stream << QString("ocTempList Before size=%1\n").arg(sz);


                eqTemp->do3sigma(proofP, 3.0);

                /////////Отбраковка коротких серий
                sz = eqTemp->ocList.size();
                qDebug() << QString("ocTempList After size=%1\n").arg(sz);
                out_stream << QString("ocTempList After size=%1\n").arg(sz);

                if(sz<ocSizeMin)
                {
                    out_stream << QString("EXCLUDED by ocSizeMin\n\n");
                    sz = ocIniList.size();
                    continue;
                }

                /////////Выделение среднего по сериям
                /////////Сборка оставшихся серий
                sz = eqTemp->ocList.size();

                while(sz>2)
                {
                    eqData->clear();
                    ocr = new ocRec;
                    ocr = eqTemp->ocList.at(0);
                    eqData->ocList << ocr;
                    t0 = ocr->MJday;
                    dT = 2.0*ocr->expTime/1440.0;

                    qDebug() << "t0 " << t0 << "\n";

                    for(i=1; i<sz; i++)
                    {
                        qDebug() << QString("%1 - %2 = %3 >? %4").arg(eqTemp->ocList.at(i)->MJday, 12, 'f', 4).arg(t0, 12, 'f', 4).arg(eqTemp->ocList.at(i)->MJday-t0).arg(dT);
                        if((fabs(eqTemp->ocList.at(i)->MJday-t0)>dT)||(i>ocSizeMax)) break;

                        ocr = new ocRec;
                        ocr = eqTemp->ocList.at(i);
                        eqData->ocList << ocr;
                        t0 = eqTemp->ocList.at(i)->MJday;
  //
                    }
                    qDebug() << QString("i= %1\n").arg(i);
                    for(k=0; k<i; k++)eqTemp->ocList.removeAt(0);
                    sz = eqTemp->ocList.size();
                    qDebug() << QString("ocTempList remain size=%1\n").arg(sz);
                    out_stream << QString("ocTempList remain size=%1\n").arg(sz);
                    if(eqData->countCols("0,1,2,3,4,5,6,7,8,9,10,11,12,13,14")) continue;
                    if(eqData->countMM()) continue;

                    cRec = new colRec;
                    cRec = eqData->getColNum(4);
                    ocRa0 << cRec;
                    cRec = new colRec;
                    cRec = eqData->getColNum(5);
                    ocDec0 << cRec;

                    ///////////
                    if(isSysCorr)
                    {
                        sz = eqData->ocList.size();
                        for(j=0; j<sz; j++)
                        {
                            calcRa = eqData->ocList[j]->ra - eqData->ocList[j]->ocRa*cos(mas_to_rad(eqData->ocList[j]->de));//>colList[4]->mean;
                            calcDec = eqData->ocList[j]->de - eqData->ocList[j]->ocDe;//eqData->colList[2]->mean - eqData->colList[5]->mean;

                            getRaDeToTang1(&ksiObs, &etaObs, eqData->ocList[j]->ra, eqData->ocList[j]->de, eqData->ocList[j]->ra_oc, eqData->ocList[j]->dec_oc);
                            getRaDeToTang1(&ksiCalc, &etaCalc, calcRa, calcDec, eqData->ocList[j]->ra_oc, eqData->ocList[j]->dec_oc);
                            if(eqData->ocList[j]->mag0<0.0) eqData->ocList[j]->mag0 = 0.1;
                            vectF->int2D(ksiObs, etaObs, eqData->ocList[j]->mag0, &dKsi, &dEta, &ni);
                            dKsi /= 1000.0;
                            dEta /= 1000.0;
                            ksiObs -= dKsi;
                            etaObs -= dEta;
                            getTangToRaDe1(&obsRa, &obsDec, ksiObs, etaObs, eqData->ocList[j]->ra_oc, eqData->ocList[j]->dec_oc);
                            eqData->ocList[j]->ra = obsRa;
                            eqData->ocList[j]->de = obsDec - dDec;
                            eqData->ocList[j]->ocRa = (ksiObs - ksiCalc)*1000.0;
                            eqData->ocList[j]->ocDe = (etaObs - etaCalc)*1000.0;// - dDec;
                        }
                        if(eqData->countCols("0,1,2,3,4,5,6,7,8,9,10,11,12,13,14")) continue;
                        if(eqData->countMM()) continue;

                        cRec = new colRec;
                        cRec = eqData->getColNum(4);
                        ocRa1 << cRec;
                        cRec = new colRec;
                        cRec = eqData->getColNum(5);
                        ocDec1 << cRec;
                    }
                    sz = eqTemp->ocList.size();
                    //////////
                    ocMean = new ocRec;
                    ocMean->MJday = eqData->colList[0]->mean;
                    ocMean->ra = eqData->colList[1]->mean;
                    ocMean->de = eqData->colList[2]->mean;
                    ocMean->mag0 = eqData->colList[3]->mean;
                    ocMean->ocRa = eqData->colList[4]->mean;
                    ocMean->ocDe = eqData->colList[5]->mean;
                    ocMean->ocMag = eqData->colList[6]->mean;
                    ocMean->ra_oc = eqData->colList[7]->mean;
                    ocMean->dec_oc = eqData->colList[8]->mean;
                    ocMean->topDist = eqData->colList[9]->mean;
                    ocMean->muRa = eqData->mmRec->Xdot;//>colList[10]->mean;
                    ocMean->muDe = eqData->mmRec->Ydot;//colList[11]->mean;
                    ocMean->Vr = eqData->colList[12]->mean;
                    ocMean->phase = eqData->colList[13]->mean;
                    ocMean->elong = eqData->colList[14]->mean;
                    ocMean->catNum = eqData->ocList[1]->catNum;
                    ocMean->name = QString(eqData->ocList[1]->name);
                    ocMean->expTime = eqData->ocList[1]->expTime;
                    eqAsts->ocList << ocMean;
                    //

                 //   sz = eqTemp->ocList.size();
                }
                sz = eqAsts->ocList.size();
                qDebug() << QString("ocResList size=%1\n\n").arg(sz);
                out_stream << QString("ocResList size=%1\n\n").arg(sz);

                sz = ocIniList.size();
            }

        }

        qDebug() << "\nend of cleaning\n";
        out_stream << "\nend of cleaning\n";
         int num0, num1;//, i, sz;
//    int num0E, num1E;
 //   double meanRa, meanDec;
 //   double sigmaKsi, sigmaEta;
    double maxOCKsi, maxOCEta;
//	eqAsts->getColNum(2)->

	if(isDetSigma)
	{
            switch(sigmaTest)
            {
                case 0:
                {

                    maxOCE = maxOCK = settings->value("general/maxres").toDouble();
    //		proofP = settings->value("general/proofPer").toDouble()*0.01;
                    di = flist.count();
                    num1K = 0;
                    num1E = 0;
                    if(objType==1)
                    {
      //                      eqAsts->do3sigma(proofP);
                            out_stream << QString("\n%1 ?= %2 ?= %3 ?= %4 ?= %5\n").arg(eqAsts->ocList.size()).arg(ocRa0.size()).arg(ocRa1.size()).arg(ocDec0.size()).arg(ocDec1.size());
                            num1 = eqAsts->ocList.size();


    //                        if(num1<3) break;

                            do
                            {
                                num0 = num1;
                                sz = ocRa0.size();
                                qDebug() << "sz= " << sz << "\n";
                                meanRa = meanDec = 0.0;
                                for(i=0;i<sz;i++)
                                {
                                    meanRa += ocRa0[i]->rmsMean;
                                    meanDec += ocDec0[i]->rmsMean;
                                }
                                meanRa /= sz*1.0;
                                meanDec /= sz*1.0;
                                qDebug() << QString("\nmeanRa= %1\t\tmeanDec= %2\n").arg(meanRa).arg(meanDec);
                                sigmaKsi = sigmaEta = 0.0;
                                for(i=0;i<sz;i++)
                                {
                                    sigmaKsi += pow(ocRa0[i]->rmsMean-meanRa, 2.0);
                                    sigmaEta += pow(ocDec0[i]->rmsMean-meanDec, 2.0);
                                }
                                sigmaKsi = sqrt(sigmaKsi/(1.0*sz-1));
                                sigmaEta = sqrt(sigmaEta/(1.0*sz-1));
    /*
                                if((eqAsts->getColRecNum(4)!=-1)&&(eqAsts->getColRecNum(5)!=-1)) eqAsts->reCountCols();
                                else eqAsts->countCols("4,5");

                                meanRa = eqAsts->getColNum(4)->mean;
                                meanDec = eqAsts->getColNum(5)->mean;

                                sigmaKsi = eqAsts->getColNum(4)->rmsOne;
                                sigmaEta = eqAsts->getColNum(5)->rmsOne;
    */
                                qDebug() << QString("\nsigmaKsi= %1\t\tsigmaEta= %2\n").arg(sigmaKsi).arg(sigmaEta);

                                maxOCKsi = 3.0*sigmaKsi;
                                maxOCEta = 3.0*sigmaEta;
                                qDebug() << QString("\nmaxOCKsi= %1\t\tmaxOCEta= %2\n").arg(maxOCKsi).arg(maxOCEta);

     //                           sz = eqAsts->ocList.size();
                                for(i=sz-1; i>=0; i--)
                                {
                                    if((fabs(ocRa0[i]->rmsMean)>maxOCKsi)||(fabs(ocDec0[i]->rmsMean)>maxOCEta))
                                    {
                                        eqAsts->ocList.removeAt(i);
                                        ocRa0.removeAt(i);
                                        ocRa1.removeAt(i);
                                        ocDec0.removeAt(i);
                                        ocDec1.removeAt(i);
                                    }
                                }
                                num1 = eqAsts->ocList.size();

                                qDebug() << QString("\nnum0= %1\t\tnum1= %2\n").arg(num0).arg(num1);


                            }while(abs(num0-num1)>(proofP*num1));
                            out_stream << "------------------\n";
                            out_stream << QString("\n%1 ?= %2 ?= %3 ?= %4 ?= %5\n").arg(eqAsts->ocList.size()).arg(ocRa0.size()).arg(ocRa1.size()).arg(ocDec0.size()).arg(ocDec1.size());
                            //
    //                        sigmaKsi = eqAsts->getColNum(4)->rmsOne;
    //                        sigmaEta = eqAsts->getColNum(5)->rmsOne;



                    }
                    else
                    {

                            resRec = new residualsRec;

                            do
                            {
                                    num0K = num1K;
                                    num0E = num1E;

                                    sigmaKsi = 0.0;
                                    sigmaEta = 0.0;
                                    meanKsi = 0.0;
                                    meanEta = 0.0;
                                    counterK = 0;
                                    counterE = 0;
                                    for(i=0; i<di; i++)
                                    {
            //				printf("\nfind meanOC: %f\%\n", (double)i*100.0/(double)di);
                                            QFile diapFile(flist[i]);
                                            diapFile.open(QIODevice::ReadOnly| QIODevice::Text);
                                            QTextStream dataStream;
                                            dataStream.setDevice(&diapFile);
                                            while (!dataStream.atEnd())
                                            {
                                                    dataLine = dataStream.readLine();
                                                    resRec->s2rec(dataLine);
                                                    if(resRec->exptime<expMin) continue;
                                                    if(isSysCorr)
                                                    {
                                                            if(!vectF->int2D(resRec->ksi, resRec->eta, resRec->mag, &dKsi, &dEta, &ni))
                                                            {
                                                                    resRec->ksiOC-= dKsi;
                                                                    resRec->etaOC-= dEta;
                                                            }
                                                    }
                                                    if(fabs(resRec->ksiOC)<maxOCK)
                                                    {
                                                            meanKsi += resRec->ksiOC;
                                                            counterK+=1;
                                                    }
                                                    if(fabs(resRec->etaOC)<maxOCE)
                                                    {
                                                            meanEta += resRec->etaOC;
                                                            counterE+=1;
                                                    }
                                            }
                                            diapFile.close();
                                    }
                                    meanKsi = meanKsi/(double)counterK;
                                    meanEta = meanEta/(double)counterE;
                                    out_stream << "meanKsi " << meanKsi << "\tmeanEta" << meanEta << "\n";
                                    qDebug() << "meanKsi " << meanKsi << "\tmeanEta" << meanEta << "\n";


                                    for(i=0; i<di; i++)
                                    {
            //				printf("\nfind Sigma: %f\%\n", (double)i*100.0/(double)di);
                                            QFile diapFile(flist[i]);
                                            diapFile.open(QIODevice::ReadOnly| QIODevice::Text);
                                            QTextStream dataStream;
                                            dataStream.setDevice(&diapFile);
                                            while (!dataStream.atEnd())
                                            {
                                                    dataLine = dataStream.readLine();
                                                    resRec->s2rec(dataLine);
                                                    if(resRec->exptime<expMin) continue;
                                                    if(isSysCorr)
                                                    {
                                                            if(!vectF->int2D(resRec->ksi, resRec->eta, resRec->mag, &dKsi, &dEta, &ni))
                                                            {
                                                                    resRec->ksiOC-= dKsi;
                                                                    resRec->etaOC-= dEta;
                                                            }
                                                    }
                                                    if(fabs(resRec->ksiOC)<maxOCK) sigmaKsi += pow(resRec->ksiOC - meanKsi, 2.0);
                                                    if(fabs(resRec->etaOC)<maxOCE) sigmaEta += pow(resRec->etaOC - meanEta, 2.0);

                                            }
                                            diapFile.close();
                                    }
                                    sigmaKsi = sqrt(sigmaKsi/(counterK-1.0));
                                    sigmaEta = sqrt(sigmaEta/(counterE-1.0));
                                    out_stream << "sigmaKsi " << sigmaKsi << "\tsigmaEta " << sigmaEta << "\n";
                                    qDebug() << "sigmaKsi " << sigmaKsi << "\tsigmaEta " << sigmaEta << "\n";

                                    num1K = counterK;
                                    num1E = counterE;
                                    maxOCE = 3.0*sigmaEta;
                                    maxOCK = 3.0*sigmaKsi;

                                    out_stream << "maxOC " << maxOCK << "\tnum0: " << num0K << "\tnum1: " << num1K << "\n";
                                    qDebug() << "maxOC " << maxOCK << "\tnum0: " << num0K << "\tnum1: " << num1K << "\n";
                                    out_stream << "maxOC " << maxOCE << "\tnum0: " << num0E << "\tnum1: " << num1E << "\n";
                                    qDebug() << "maxOC " << maxOCE << "\tnum0: " << num0E << "\tnum1: " << num1E << "\n";

                            }while((abs(num0K-num1K)>(proofP*num1K))&&(abs(num0E-num1E)>(proofP*num1E)));

                    break;
                    }
                    break;
                }
                case 1: //hi^2
                {


                    break;
                }
            }
                maxKsi = 3.0*sigmaKsi;
                maxEta = 3.0*sigmaEta;
	}
	else
	{
                if(objType==1)
		{
                     /*   maxKsi = maxOCKsi = settings->value("general/maxresRa").toDouble();
                        maxEta = maxOCEta = settings->value("general/maxresDec").toDouble();

                       sz = eqAsts->ocList.size();
                        for(i=sz-1; i>=0; i--)
                        {
                            if((fabs(ocRa0[i]->rmsOne)>maxOCKsi)||(fabs(ocDec0[i]->rmsOne)>maxOCEta))
                            {
                                eqAsts->ocList.removeAt(i);
                                ocRa0.removeAt(i);
                                ocRa1.removeAt(i);
                                ocDec0.removeAt(i);
                                ocDec1.removeAt(i);
                            }
                        }*/
		}
                else
                {
			maxKsi = settings->value("general/maxresKsi").toDouble();
			maxEta = settings->value("general/maxresEta").toDouble();
                }
	}
	out_stream << "maxKsi " << maxKsi << "\tmaxEta " << maxEta << "\n";
	
	switch(taskNum)
	{
		case 0:
			
			di = flist.count();
			for(i=0; i<di; i++)
			{
				qDebug() << flist[i] << "\n";
				tmp = flist[i].toAscii();
				fname = tmp.data();
				eqData->initOld(fname);
				eqData->save();
			}
		break;
		case 1:
	
			do
			{
				ri = -1;
				rti = -1;
				di = flist.count();
				for(i=0; i<di; i++)
				{
					if(flist[i].lastIndexOf("residuals.txt")==(flist[i].lastIndexOf("/")+1))
					{
						ri=i;
						break;
					}
					
				}
				qDebug() << "ri: " << ri;
				if(ri!=-1)
				{
					resStr.clear();
					resStr.insert(0, flist[ri]);
					
					kp = flist[ri].lastIndexOf("/");
					dstr = flist[ri].left(kp);
					out_stream << "\n\nDstr:\t" << dstr;
					
					for(j=0;j<di;j++)
					{
						if(j==ri) continue;
						if((flist[j].lastIndexOf(dstr)==0)&&(flist[j].lastIndexOf("_eq.txt")==(flist[j].size()-7)))
						{
							rti = j;
							break;
						}
					}
					qDebug() << "rti: " << rti;
					if(rti!=-1)
					{
						eqStr.clear();
						eqStr.insert(0, flist[rti]);
						
					}
					
					for(j=di-1;j>=0;j--)
					{
						if(flist[j].lastIndexOf(dstr)==0) flist.removeAt(j);
					}
					
					out_stream << "\n" << ri << ": resStr: " << resStr;
					out_stream << "\n" << rti << ": eqStr: " << eqStr;
					
					qDebug() << "eqStr: " <<  eqStr << "\n";
					tmp = eqStr.toAscii();
					fname = tmp.data();
					eqData->init(fname);
					if(eqData->ocList.size()>0) 
					{
						tTm = eqData->ocList[0]->MJday;
					}
					else tTm = 0.0;
					qDebug() << "resStr: " <<  resStr << "\n";
					tmp = resStr.toAscii();
					fname = tmp.data();
					resData->initOld(fname);
					di = resData->resList.size();
					for(j=0; j<di; j++)
					{
						resData->resList[j]->mJD = tTm;
					}
					resData->save();
					
				}
			}while(ri!=-1);
			
		break;
		case 2:
			
			di = flist.count();
			for(i=0; i<di; i++)
			{
				tmp = flist[i].toAscii();
				fname = tmp.data();
//				eqData->clear();
				eqData->initOld(fname);
//				eqData->save();
			}
		break;
                case 3:                     //нмъпомелуе аеипмолмжм нмйз
                    {
                        di = flist.count();
                        resRec = new residualsRec;
			
			isFindDiap = settings->value("diap/isFindDiap").toInt();
			
		
			out_stream << "isFindDiap " << isFindDiap << "\n";
			out_stream << "di " << di << "\n";
			
//				out_stream << "\n" << flist[i];
			diniKey = 1;

                        QFile diapFile;
                        QFile dataFile;
                        QTextStream dataStream;
			if(isFindDiap)
			{
				for(i=0; i<di; i++)
				{
					printf("\nfindDiap: %f\%\n", (double)i*100.0/(double)di);
                                        diapFile.setFileName(flist[i]);
					diapFile.open(QIODevice::ReadOnly| QIODevice::Text);

					dataStream.setDevice(&diapFile);
					while (!dataStream.atEnd())
					{
						dataLine = dataStream.readLine();
						resRec->s2rec(dataLine);
                                                if(resRec->exptime<expMin) continue;
						if(isSysCorr)
						{
							if(!vectF->int2D(resRec->ksi, resRec->eta, resRec->mag, &dKsi, &dEta, &ni))
							{
                                           //                 dDec = -0.5082077*resRec->de + 18.1648793;
                                                                dDec = 0.0;
								resRec->ksiOC-= dKsi;
                                                                resRec->etaOC-= (dEta+dDec);

                                                                resRec->ksi-= dKsi;
                                                                resRec->eta-= (dEta+dDec);
							}
						}
						if((fabs(resRec->ksiOC)<maxEta)&&(fabs(resRec->etaOC)<maxEta))
						{
							if(diniKey)
							{
								naxeX[1] = naxeX[0] = resRec->ksi;
								naxeY[1] = naxeY[0] = resRec->eta;
								diniKey = 0;
							}
							else
							{
								if(naxeX[0]>resRec->ksi) naxeX[0] = resRec->ksi;
								if(naxeX[1]<resRec->ksi) naxeX[1] = resRec->ksi;
								if(naxeY[0]>resRec->eta) naxeY[0] = resRec->eta;
								if(naxeY[1]<resRec->eta) naxeY[1] = resRec->eta;
							}
						}
					}
					diapFile.close();
				}
				if(fabs(naxeX[0])>fabs(naxeX[1])) axd = fabs(naxeX[1]);
				else axd = fabs(naxeX[0]);
				naxeX[0] = -axd;
				naxeX[1] = axd;
				
				if(fabs(naxeY[0])>fabs(naxeY[1])) axd = fabs(naxeY[1]);
				else axd = fabs(naxeY[0]);
				naxeY[0] = axd;
				naxeY[1] = -axd;
			}
			else
			{
				naxeX[0] = settings->value("diap/naxeX0").toDouble();
				naxeX[1] = settings->value("diap/naxeX1").toDouble();
				naxeY[0] = settings->value("diap/naxeY0").toDouble();
				naxeY[1] = settings->value("diap/naxeY1").toDouble();
			}
			
			
			out_stream << "naxeX= " << naxeX[0] << " - " << naxeX[1] << "\n";
			out_stream << "naxeY= " << naxeY[0] << " - " << naxeY[1] << "\n";

                        QStringList mLevelsList = settings->value("resStat/mLevels").toString().split("|");
                        mLevNum = mLevelsList.count();

                        //mLevNum = settings->value("resStat/mLevNum").toInt();
                        //mLevNumBeg = settings->value("resStat/mLevNumBeg").toDouble();
			xLevNum = settings->value("resStat/xLevNum").toInt();
			yLevNum = settings->value("resStat/yLevNum").toInt();

			numTot = 0;
			
			mLevels = new double[mLevNum];
			xLevels = new double[xLevNum];
			yLevels = new double[yLevNum];
			
			dX = fabs(naxeX[1] - naxeX[0])/(double)(xLevNum-1);
			for(i=0; i<xLevNum; i++) xLevels[i] = naxeX[0] + dX*i;
			
			dY = fabs(naxeY[1] - naxeY[0])/(double)(yLevNum-1);
			for(i=0; i<yLevNum; i++) yLevels[i] = naxeY[0] - dY*i;
			
                        /*mLevels[0] = 0.0;
			for(i=1; i<mLevNum-1; i++) mLevels[i] = (mLevNumBeg+i-1)*1.0;
                        mLevels[mLevNum-1] = 20.0;*/
                        for(i=0; i<mLevNum; i++) mLevels[i] = mLevelsList.at(i).toDouble();
			
//			dX = (double)(naxeX[1] - naxeX[0])/2.0;
//			dY = (double)(naxeY[1] - naxeY[0])/2.0;
			
			
			axesNum = 3;
			axes = new int[axesNum];
			pos = new int[axesNum];
			axes[0] = xLevNum-1;
			axes[1] = yLevNum-1;
			axes[2] = mLevNum-1;
			xVectField = new multidim(DOUBLE_TYPE, axes, axesNum);
			yVectField = new multidim(DOUBLE_TYPE, axes, axesNum);
			xDispVectField = new multidim(DOUBLE_TYPE, axes, axesNum);
			yDispVectField = new multidim(DOUBLE_TYPE, axes, axesNum);
			vectNumField = new multidim(LONG_TYPE, axes, axesNum);
			
			xVectField->clear();
			yVectField->clear();
			xDispVectField->clear();
			yDispVectField->clear();
			vectNumField->clear();
			
			sz = xVectField->getSize();
			
			
//			resRec = new residualsRec;
			
			for(i=0; i<di; i++)
			{
				printf("\ndet Mean: %f\%\n", (double)i*100.0/(double)di);
//				out_stream << "\n" << flist[i];
                                dataFile.setFileName(flist[i]);
				dataFile.open(QIODevice::ReadOnly| QIODevice::Text);

				dataStream.setDevice(&dataFile);
				while (!dataStream.atEnd())
				{
					dataLine = dataStream.readLine();
					resRec->s2rec(dataLine);
                                        if(resRec->exptime<expMin) continue;
					if(isSysCorr)
					{
						if(!vectF->int2D(resRec->ksi, resRec->eta, resRec->mag, &dKsi, &dEta, &ni))
						{
//                                                        dDec = -0.5082077*resRec->de + 18.1648793;
                                                        dDec = 0.0;
                                                                resRec->ksiOC-= dKsi;
                                                                resRec->etaOC-= (dEta+dDec);

                                                                resRec->ksi-= dKsi;
                                                                resRec->eta-= (dEta+dDec);
						}
					}
					
					pos[0] = -1;
					for(j=1; j<xLevNum; j++)
					{
						if(((resRec->ksi)<=xLevels[j])&&((resRec->ksi)>xLevels[j-1])&&(fabs(resRec->ksiOC)<maxKsi))
						{
							pos[0] = j-1;
							break;
						}
					}
					pos[1] = -1;
					for(j=1; j<yLevNum; j++)
					{
						if(((resRec->eta)>=yLevels[j])&&((resRec->eta)<yLevels[j-1])&&(fabs(resRec->etaOC)<maxEta))
						{
							pos[1] = j-1;
							break;
						}
					}
					pos[2] = -1;
					for(j=1; j<mLevNum; j++)
					{
						if((resRec->mag<=mLevels[j])&&(resRec->mag>mLevels[j-1]))
						{
							pos[2] = j-1;
							break;
						}
					}
//					out_stream << "\n" << pos[0] << ":" << pos[1] << ":" << pos[2] << "\t" << resRec->etaOC << ":" << resRec->ksiOC << "\n";
					
					if((pos[0]!=-1)&&(pos[1]!=-1)&&(pos[2]!=-1))
					{
						numTot++;
						cpos = xVectField->detPos(pos);
						xVectField->dD[cpos] += resRec->ksiOC;
						cpos = yVectField->detPos(pos);
						yVectField->dD[cpos] += resRec->etaOC;
						cpos = vectNumField->detPos(pos);
						vectNumField->lD[cpos] = vectNumField->lD[cpos] + 1; 
/*						xVectField->set(resRec->etaOC, pos);
						yVectField->set(resRec->ksiOC, pos);
						vectNumField->get(&nums, pos);
						vectNumField->set((long)(nums+1), pos);*/
					}
				}
                                dataFile.close();
                                dataStream.reset();
			}
			
			sz = xVectField->getSize();
			out_stream << "\nstat\n";
			numMin = vectNumField->lD[0];
			for(p0=0; p0<sz; p0++)
			{
				nums = vectNumField->lD[p0];
				if(numMin>nums) numMin = nums;
				if(nums)
				{
					xVectField->dD[p0] = xVectField->dD[p0]/(nums*1.0);
					yVectField->dD[p0] = yVectField->dD[p0]/(nums*1.0);
				}
				
				out_stream << "\n" << p0 << ":" << QString("%1").arg(xVectField->dD[p0]) << ":" << yVectField->dD[p0] << ":" << nums << "\n";
			}
			
			resFile.setFileName(outputFolder+"/"+"res.dat");
			resFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
			resStream.setDevice(&resFile);
			
			resStream << "#";
			
			for(k=0;k<xLevNum;k++)
			{
				 xDiap << QString("%1").arg(xLevels[k]);
			}
			resStream << xDiap.join("|");
			resStream << "\n";
			
			resStream << "#";
			
			for(k=0;k<yLevNum;k++)
			{
				 yDiap << QString("%1").arg(yLevels[k]);
			}
			resStream << yDiap.join("|");
			resStream << "\n";
			
			resStream << "#";
			
			for(k=0;k<mLevNum;k++)
			{
				 mDiap << QString("%1").arg(mLevels[k]);
			}
			resStream << mDiap.join("|");
			resStream << "\n";
			
			
			
			for(i=0;i<mLevNum-1; i++)
			{
				for(j=0;j<yLevNum-1;j++)
				{
					for(k=0;k<xLevNum-1;k++)
					{
						pos[0] = k;
						pos[1] = j;
						pos[2] = i;
						ocX = xVectField->getD(xVectField->detPos(pos));
						ocY = yVectField->getD(yVectField->detPos(pos));
//						yVectField->get(&ocY, pos);
//						vectNumField->get(&nums, pos);
						nums = vectNumField->getL(vectNumField->detPos(pos));
						resStream << QString("%1").arg(k, 5, 10, QLatin1Char(' ')) << "|" << QString("%1").arg(j, 5, 10, QLatin1Char(' ')) << "|" << QString("%1").arg(i, 5, 10, QLatin1Char(' ')) << "|" << QString("%1").arg(ocX, 12, 'f', 6, QLatin1Char(' ')) << "|" << QString("%1").arg(ocY, 12, 'f', 6, QLatin1Char(' ')) << "|" << QString("%1").arg(nums, 8, 10, QLatin1Char(' ')) << "\n";
					}
				}
			}
			
			
///////////////////////////////////////////////////////////////	Dispersion
			for(i=0; i<di; i++)
			{
				printf("\ndet Disp: %f\n", (double)i*100.0/(double)di);
//				out_stream << "\n" << flist[i];
                                QFile dispFile(flist[i]);
//                                if(dataFile.isOpen())dataFile.close();
                                if(!dispFile.open(QIODevice::ReadOnly| QIODevice::Text)) qDebug() << QString("\nFile %1 not open\n").arg(flist[i]);
                                dataStream.setDevice(&dispFile);
				while (!dataStream.atEnd())
				{
					dataLine = dataStream.readLine();
					resRec->s2rec(dataLine);
                                        if(resRec->exptime<expMin) continue;
					if(isSysCorr)
					{
						if(!vectF->int2D(resRec->ksi, resRec->eta, resRec->mag, &dKsi, &dEta, &ni))
						{
							resRec->ksiOC-= dKsi;
							resRec->etaOC-= dEta;

                                                        resRec->ksi-= dKsi;
                                                        resRec->eta-= dEta;
						}
					}
					pos[0] = -1;
					for(j=1; j<xLevNum; j++)
					{
						if(((resRec->ksi)<=xLevels[j])&&((resRec->ksi)>xLevels[j-1])&&(fabs(resRec->ksiOC)<maxKsi))
						{
							pos[0] = j-1;
							break;
						}
					}
					pos[1] = -1;
					for(j=1; j<yLevNum; j++)
					{
						if(((resRec->eta)<=yLevels[j])&&((resRec->eta)>yLevels[j-1])&&(fabs(resRec->etaOC)<maxEta))
						{
							pos[1] = j-1;
							break;
						}
					}
					pos[2] = -1;
					for(j=1; j<mLevNum; j++)
					{
						if((resRec->mag<=mLevels[j])&&(resRec->mag>mLevels[j-1]))
						{
							pos[2] = j-1;
							break;
						}
					}
//					out_stream << "\n" << pos[0] << ":" << pos[1] << ":" << pos[2] << "\t" << resRec->etaOC << ":" << resRec->ksiOC << "\n";
					
					if((pos[0]!=-1)&&(pos[1]!=-1)&&(pos[2]!=-1))
					{
//						numTot++;
						cpos = xDispVectField->detPos(pos);
						xDispVectField->dD[cpos] = xDispVectField->dD[cpos]+pow(xVectField->dD[cpos]-resRec->ksiOC, 2.0);
                                                out_stream << "\n" << cpos << ":" << xVectField->dD[cpos]-resRec->ksiOC << "\t";
						cpos = yDispVectField->detPos(pos);
						yDispVectField->dD[cpos] = yDispVectField->dD[cpos]+pow(yVectField->dD[cpos]-resRec->etaOC, 2.0);
                                                out_stream << cpos << ":" << yVectField->dD[cpos]-resRec->etaOC << "\n";
//						cpos = vectNumField->detPos(pos);
//						vectNumField->lD[cpos] = vectNumField->lD[cpos] + 1; 
/*						xVectField->set(resRec->etaOC, pos);
						yVectField->set(resRec->ksiOC, pos);
						vectNumField->get(&nums, pos);
						vectNumField->set((long)(nums+1), pos);*/
					}
				}
                                dispFile.close();
			}
			
			sz = xDispVectField->getSize();
			out_stream << "\nstat\n";
//			int numMin = vectNumField->lD[0];
			for(p0=0; p0<sz; p0++)
			{
				nums = vectNumField->lD[p0];
//				if(numMin>nums) numMin = nums;
				if(nums)
				{
					xDispVectField->dD[p0] = sqrt(xDispVectField->dD[p0]/(nums*1.0 - 1.0));
					yDispVectField->dD[p0] = sqrt(yDispVectField->dD[p0]/(nums*1.0 - 1.0));
				}
				
				out_stream << "\n" << p0 << ":" << QString("%1").arg(xDispVectField->dD[p0]) << ":" << yDispVectField->dD[p0] << ":" << nums << "\n";
			}
			
			resDispFile.setFileName(outputFolder+"/"+"resDisp.dat");
			resDispFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
			
			resDispStream.setDevice(&resDispFile);
			
			resDispStream << "#";
			xDiap.clear();
			for(k=0;k<xLevNum;k++)
			{
				 xDiap << QString("%1").arg(xLevels[k]);
			}
			resDispStream << xDiap.join("|");
			resDispStream << "\n";
			
			yDiap.clear();
			for(k=0;k<yLevNum;k++)
			{
				 yDiap << QString("%1").arg(yLevels[k]);
			}
			resDispStream << yDiap.join("|");
			resDispStream << "\n";
			
			resDispStream << "#";
			mDiap.clear();
			for(k=0;k<mLevNum;k++)
			{
				 mDiap << QString("%1").arg(mLevels[k]);
			}
			resDispStream << mDiap.join("|");
			resDispStream << "\n";
			
			for(i=0;i<mLevNum-1; i++)
			{
				for(j=0;j<yLevNum-1;j++)
				{
					for(k=0;k<xLevNum-1;k++)
					{
						pos[0] = k;
						pos[1] = j;
						pos[2] = i;
                                                ocX = xDispVectField->dD[xDispVectField->detPos(pos)];
                                                ocY = yDispVectField->dD[yDispVectField->detPos(pos)];
//						yVectField->get(&ocY, pos);
//						vectNumField->get(&nums, pos);
						nums = vectNumField->getL(vectNumField->detPos(pos));
						resDispStream << QString("%1").arg(k, 5, 10, QLatin1Char(' ')) << "|" << QString("%1").arg(j, 5, 10, QLatin1Char(' ')) << "|" << QString("%1").arg(i, 5, 10, QLatin1Char(' ')) << "|" << QString("%1").arg(ocX, 12, 'f', 6, QLatin1Char(' ')) << "|" << QString("%1").arg(ocY, 12, 'f', 6, QLatin1Char(' ')) << "|" << QString("%1").arg(nums, 8, 10, QLatin1Char(' ')) << "\n";
					}
				}
			}		
////////////////////////////////////////////////////////////////			
			
			
			
			
			printf("\nnumTot=%d\n", numTot);
			printf("\nnumMin=%d\n", numMin);
                    }
		break;
//////////////////////////CASE 4
		case 4:							//Яочалелуе юйеъич
			di = flist.count();
//			resRec = new residualsRec;
			
			out_stream << "di " << di << "\n";
			
			mLevNum = settings->value("resUB/mLevNum").toInt();
			mLevNumBeg = settings->value("resUB/mLevNumBeg").toDouble();
			timeDiapsStr = settings->value("resUB/timeDiaps").toString();
			
			timeDiapsSL = timeDiapsStr.split("|");
			tLevNum = timeDiapsSL.size();

			numTot = 0;
			
			mLevels = new double[mLevNum];
			tLevels = new double[tLevNum];
			
			mLevels[0] = 0.0;
			for(i=1; i<mLevNum-1; i++) mLevels[i] = (mLevNumBeg+i-1)*1.0;
			mLevels[mLevNum-1] = 20.0;
			
			for(i=0; i<tLevNum; i++)
			{
				tLevels[i] = getMJDfromYMD(timeDiapsSL[i]);
			}
			
			axesNum = 2;
			axes = new int[axesNum];
			pos = new int[axesNum];
			axes[0] = mLevNum-1;
			axes[1] = tLevNum-1;
			xVectField = new multidim(DOUBLE_TYPE, axes, axesNum);
			yVectField = new multidim(DOUBLE_TYPE, axes, axesNum);
			xDispVectField = new multidim(DOUBLE_TYPE, axes, axesNum);
			yDispVectField = new multidim(DOUBLE_TYPE, axes, axesNum);
			vectNumField = new multidim(LONG_TYPE, axes, axesNum);
			
			xVectField->clear();
			yVectField->clear();
			xDispVectField->clear();
			yDispVectField->clear();
			vectNumField->clear();
			
			sz = xVectField->getSize();
			
			
			resRec = new residualsRec;
			
			for(i=0; i<di; i++)
			{
				printf("\ndet Mean UB: %f\%\n", (double)i*100.0/(double)di);
//				out_stream << "\n" << flist[i];
				QFile dataFile(flist[i]);
				dataFile.open(QIODevice::ReadOnly| QIODevice::Text);
				QTextStream dataStream;
				dataStream.setDevice(&dataFile);
				while (!dataStream.atEnd())
				{
					dataLine = dataStream.readLine();
					resRec->s2rec(dataLine);
                                        if(resRec->exptime<expMin) continue;
					if(isSysCorr)
					{
						if(!vectF->int2D(resRec->ksi, resRec->eta, resRec->mag, &dKsi, &dEta, &ni))
						{
							resRec->ksiOC-= dKsi;
							resRec->etaOC-= dEta;
						}
					}
					
					pos[0] = -1;
					for(j=1; j<mLevNum; j++)
					{
						if((resRec->mag<=mLevels[j])&&(resRec->mag>mLevels[j-1]))
						{
							pos[0] = j-1;
							break;
						}
					}
					pos[1] = -1;
					for(j=1; j<tLevNum; j++)
					{
						if(((resRec->mJD)<=tLevels[j])&&((resRec->mJD)>tLevels[j-1]))
						{
							pos[1] = j-1;
							break;
						}
					}
//					out_stream << "\n" << pos[0] << ":" << pos[1] << ":" << pos[2] << "\t" << resRec->etaOC << ":" << resRec->ksiOC << "\n";
					
					if((pos[0]!=-1)&&(pos[1]!=-1)&&(fabs(resRec->ksiOC)<maxKsi)&&(fabs(resRec->etaOC)<maxEta))
					{
						numTot++;
						cpos = xVectField->detPos(pos);
						xVectField->dD[cpos] += resRec->ksiOC;
//						cpos = yVectField->detPos(pos);
						yVectField->dD[cpos] += resRec->etaOC;
						cpos = vectNumField->detPos(pos);
						vectNumField->lD[cpos] = vectNumField->lD[cpos] + 1; 
						
//						if(pos[0]==4&&pos[1]==3) out_stream << "\n" << vectNumField->lD[cpos]  << "\t" << QString("%1|%2").arg(resRec->ksiOC, 7, 'f', 1, QLatin1Char(' ')).arg(resRec->etaOC, 7, 'f', 1, QLatin1Char(' ')) << "\n";
/*						xVectField->set(resRec->etaOC, pos);
						yVectField->set(resRec->ksiOC, pos);
						vectNumField->get(&nums, pos);
						vectNumField->set((long)(nums+1), pos);*/
					}
				}
			}
			
			sz = xVectField->getSize();
			out_stream << "\nstat\n";
			numMin = vectNumField->lD[0];
			for(p0=0; p0<sz; p0++)
			{
				nums = vectNumField->lD[p0];
				if(numMin>nums) numMin = nums;
				if(nums)
				{
					xVectField->dD[p0] = xVectField->dD[p0]/(nums*1.0);
					yVectField->dD[p0] = yVectField->dD[p0]/(nums*1.0);
				}
				
				out_stream << "\n" << p0 << ":" << QString("%1").arg(xVectField->dD[p0]) << ":" << yVectField->dD[p0] << ":" << nums << "\n";
			}
			
			resFile.setFileName(outputFolder+"/"+"resUB.dat");
			resFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
			resStream.setDevice(&resFile);
			
			resStream << "#";
			mDiap.clear();
			for(k=0;k<mLevNum;k++) mDiap << QString("%1").arg(mLevels[k]);
			resStream << mDiap.join("|");
			resStream << "\n";
			
			resStream << "#";
			yDiap.clear();
			for(k=0;k<tLevNum;k++) yDiap << QString("%1").arg(tLevels[k], 8, 'f', 2, QLatin1Char(' '));
			resStream << yDiap.join("|");
			resStream << "\n";
			
			for(i=0;i<tLevNum-1; i++)
			{
				for(j=0;j<mLevNum-1;j++)
				{
					
						pos[0] = j;
						pos[1] = i;
						
						ocX = xVectField->getD(xDispVectField->detPos(pos));
						ocY = yVectField->getD(yDispVectField->detPos(pos));
//						yVectField->get(&ocY, pos);
						nums = vectNumField->getL(vectNumField->detPos(pos));
						resStream << QString("%1").arg(j, 5, 10, QLatin1Char(' ')) << "|" << QString("%1").arg(i, 5, 10, QLatin1Char(' ')) << "|" << QString("%1").arg(ocX, 12, 'f', 6, QLatin1Char(' ')) << "|" << QString("%1").arg(ocY, 12, 'f', 6, QLatin1Char(' ')) << "|" << QString("%1").arg(nums, 8, 10, QLatin1Char(' ')) << "\n";
					}
			}
					
			
			
///////////////////////////////////////////////////////////////	Dispersion
			for(i=0; i<di; i++)
			{
				printf("\ndet Disp UB: %f\n", (double)i*100.0/(double)di);
//				out_stream << "\n" << flist[i];
				QFile dataFile(flist[i]);
				dataFile.open(QIODevice::ReadOnly| QIODevice::Text);
				QTextStream dataStream;
				dataStream.setDevice(&dataFile);
				while (!dataStream.atEnd())
				{
					dataLine = dataStream.readLine();
					resRec->s2rec(dataLine);
                                        if(resRec->exptime<expMin) continue;
					if(isSysCorr)
					{
						if(!vectF->int2D(resRec->ksi, resRec->eta, resRec->mag, &dKsi, &dEta, &ni))
						{
							resRec->ksiOC-= dKsi;
							resRec->etaOC-= dEta;
						}
					}
					pos[0] = -1;
					for(j=1; j<mLevNum; j++)
					{
						if((resRec->mag<=mLevels[j])&&(resRec->mag>mLevels[j-1]))
						{
							pos[0] = j-1;
							break;
						}
					}
					pos[1] = -1;
					for(j=1; j<tLevNum; j++)
					{
						if(((resRec->mJD)<=tLevels[j])&&((resRec->mJD)>tLevels[j-1]))
						{
							pos[1] = j-1;
							break;
						}
					}
//					out_stream << "\n" << pos[0] << ":" << pos[1] << ":" << pos[2] << "\t" << resRec->etaOC << ":" << resRec->ksiOC << "\n";
					
					if((pos[0]!=-1)&&(pos[1]!=-1)&&(fabs(resRec->ksiOC)<maxKsi)&&(fabs(resRec->etaOC)<maxEta))
					{
//						numTot++;
						cpos = xDispVectField->detPos(pos);
						xDispVectField->dD[cpos] = xDispVectField->dD[cpos]+pow(xVectField->dD[cpos]-resRec->ksiOC, 2.0);
						cpos = yDispVectField->detPos(pos);
						yDispVectField->dD[cpos] = yDispVectField->dD[cpos]+pow(yVectField->dD[cpos]-resRec->etaOC, 2.0);
//						cpos = vectNumField->detPos(pos);
//						vectNumField->lD[cpos] = vectNumField->lD[cpos] + 1; 
/*						xVectField->set(resRec->etaOC, pos);
						yVectField->set(resRec->ksiOC, pos);
						vectNumField->get(&nums, pos);
						vectNumField->set((long)(nums+1), pos);*/
					}
				}
			}
			
			sz = xDispVectField->getSize();
			out_stream << "\nstat\n";
//			int numMin = vectNumField->lD[0];
			for(p0=0; p0<sz; p0++)
			{
				nums = vectNumField->lD[p0];
//				if(numMin>nums) numMin = nums;
				if(nums)
				{
					xDispVectField->dD[p0] = sqrt(xDispVectField->dD[p0]/(nums*1.0 - 1.0));
					yDispVectField->dD[p0] = sqrt(yDispVectField->dD[p0]/(nums*1.0 - 1.0));
				}
				
				out_stream << "\n" << p0 << ":" << QString("%1").arg(xDispVectField->dD[p0]) << ":" << yDispVectField->dD[p0] << ":" << nums << "\n";
			}
			
			resDispFile.setFileName(outputFolder+"/"+"resDispUB.dat");
			resDispFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
			
			resDispStream.setDevice(&resDispFile);
			
			resDispStream << "#";
			mDiap.clear();
			for(k=0;k<mLevNum;k++) mDiap << QString("%1").arg(mLevels[k]);
			resDispStream << mDiap.join("|");
			resDispStream << "\n";
			
			resDispStream << "#";
			yDiap.clear();
			for(k=0;k<tLevNum;k++) yDiap << QString("%1").arg(tLevels[k], 8, 'f', 2, QLatin1Char(' '));
			resDispStream << yDiap.join("|");
			resDispStream << "\n";
			
			for(i=0;i<tLevNum-1; i++)
			{
				for(j=0;j<mLevNum-1;j++)
				{
					
						pos[0] = j;
						pos[1] = i;
						
						ocX = xDispVectField->getD(xDispVectField->detPos(pos));
						ocY = yDispVectField->getD(yDispVectField->detPos(pos));
//						yVectField->get(&ocY, pos);
						nums = vectNumField->getL(vectNumField->detPos(pos));
						resDispStream << QString("%1").arg(j, 5, 10, QLatin1Char(' ')) << "|" << QString("%1").arg(i, 5, 10, QLatin1Char(' ')) << "|" << QString("%1").arg(ocX, 12, 'f', 6, QLatin1Char(' ')) << "|" << QString("%1").arg(ocY, 12, 'f', 6, QLatin1Char(' ')) << "|" << QString("%1").arg(nums, 8, 10, QLatin1Char(' ')) << "\n";
					}
			}
					
////////////////////////////////////////////////////////////////			
		
			
			
			
			printf("\nnumTot=%d\n", numTot);
			printf("\nnumMin=%d\n", numMin);

		break;
		case 5:						//OMC мп ъиймлелуз, юец мъоедлелуз
			di = flist.count();
			resRec = new residualsRec;
			resFile.setFileName(outputFolder+"/"+"decOMC.dat");
			resFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
			resStream.setDevice(&resFile);
				
			for(i=0; i<di; i++)
			{
				printf("\ndet OMC/DEC: %f\%\n", (double)i*100.0/(double)di);
//				out_stream << "\n" << flist[i];
				QFile dataFile(flist[i]);
				dataFile.open(QIODevice::ReadOnly| QIODevice::Text);
				QTextStream dataStream;
				dataStream.setDevice(&dataFile);
				
				while (!dataStream.atEnd())
				{
					dataLine = dataStream.readLine();
					resRec->s2rec(dataLine);
                                        if(resRec->exptime<expMin) continue;
					if(isSysCorr)
					{
						if(!vectF->int2D(resRec->ksi, resRec->eta, resRec->mag, &dKsi, &dEta, &ni))
						{
							resRec->ksiOC-= dKsi;
							resRec->etaOC-= dEta;
						}
					}
					if((fabs(resRec->ksiOC)<=maxKsi)&&(fabs(resRec->etaOC)<=maxEta))
					{
						resStream << QString("%1").arg(resRec->de, 12, 'f', 8, QLatin1Char(' '))  << "|" << QString("%1").arg(sqrt(resRec->ksiOC*resRec->ksiOC + resRec->etaOC*resRec->etaOC), 7, 'f', 1, QLatin1Char(' ')) << "\n";
					}
				}
			}
		break;
		case 6:					//Цчауъукмъпщ KsiOC у EtaOC мп Ъиймлелуз ъ мъоедлелуек
		{
			
		
			di = flist.count();
			resRec = new residualsRec;
			
			out_stream << "di " << di << "\n";
			
			
			int deLevNum;
			int deLevStep = settings->value("task06/deDiaps").toInt();
			
			
			double stepDe;
			
			
			diniKey = 1;
			for(i=0; i<di; i++)
			{
				printf("\nfindDiaps: %f\%\n", (double)i*100.0/(double)di);
				QFile diapFile(flist[i]);
				diapFile.open(QIODevice::ReadOnly| QIODevice::Text);
				QTextStream dataStream;
				dataStream.setDevice(&diapFile);
				while (!dataStream.atEnd())
				{
					dataLine = dataStream.readLine();
					resRec->s2rec(dataLine);
                                        if(resRec->exptime<expMin) continue;
					if(isSysCorr)
					{
						if(!vectF->int2D(resRec->ksi, resRec->eta, resRec->mag, &dKsi, &dEta, &ni))
						{
                                                        /*resRec->ksiOC-= dKsi;
                                                        resRec->etaOC-= dEta;*/
                                                    qDebug() << "dec: " << resRec->de << "\n";
                                                    dDec = -0.5082077*resRec->de + 18.1648793;
                                                    qDebug() << "dDec: " << dDec << "\n";

                                                                resRec->ksiOC-= dKsi;
                                                                resRec->etaOC-= (dEta+dDec);

                                                                resRec->ksi-= dKsi;
                                                                resRec->eta-= (dEta+dDec);
						}
					}
					if((fabs(resRec->ksiOC)<maxKsi)&&(fabs(resRec->etaOC)<maxEta))
					{
						if(diniKey)
						{
							naxeX[1] = naxeX[0] = resRec->ksi;
							naxeY[1] = naxeY[0] = resRec->eta;
							naxeDe[1] = naxeDe[0] = resRec->de;
							diniKey = 0;
						}
						else
						{
							if(naxeX[0]>resRec->ksi) naxeX[0] = resRec->ksi;
							if(naxeX[1]<resRec->ksi) naxeX[1] = resRec->ksi;
							if(naxeY[0]>resRec->eta) naxeY[0] = resRec->eta;
							if(naxeY[1]<resRec->eta) naxeY[1] = resRec->eta;
							if(naxeDe[0]>resRec->de) naxeDe[0] = resRec->de;
							if(naxeDe[1]<resRec->de) naxeDe[1] = resRec->de;
						}
					}
				}
				diapFile.close();
			}
			
			int sgn = fabs(naxeDe[0])/naxeDe[0];
			int levMin = sgn*((int)(fabs(naxeDe[0]))+1);
			sgn = fabs(naxeDe[1])/naxeDe[1];
			int levMax = sgn*((int)(fabs(naxeDe[1]))+1);
			deLevNum = ((levMax-levMin)/deLevStep)+2;
			
			
			double* deLevels = new double[deLevNum];
			double* deValuesX = new double[deLevNum-1];
			double* deValuesY = new double[deLevNum-1];
			int* deNums = new int[deLevNum-1];
/*			if(fabs(naxeX[0])>fabs(naxeX[1])) axd = fabs(naxeX[1]);
//			else axd = fabs(naxeX[0]);
			naxeX[0] = -axd;
			naxeX[1] = axd;
			
			if(fabs(naxeY[0])>fabs(naxeY[1])) axd = fabs(naxeY[1]);
			else axd = fabs(naxeY[0]);
			naxeY[0] = axd;
			naxeY[1] = -axd;
*/		
			stepDe = (naxeDe[1]-naxeDe[0])/(double)(deLevNum-1);
			
			out_stream << "de diaps: " << naxeDe[0] << "::" << naxeDe[1] << "::" << stepDe << "\n";
			
			for(i=0; i<deLevNum; i++) deLevels[i] = levMin+deLevStep*i;
			for(i=0; i<deLevNum-1; i++) {deValuesX[i] = deValuesY[i] = 0.0;deNums[i]=0;}

			numTot = 0;
			
			
			resRec = new residualsRec;
			
			for(i=0; i<di; i++)
			{
				printf("\ndet Mean UB: %f\%\n", (double)i*100.0/(double)di);
//				out_stream << "\n" << flist[i];
				QFile dataFile(flist[i]);
				dataFile.open(QIODevice::ReadOnly| QIODevice::Text);
				QTextStream dataStream;
				dataStream.setDevice(&dataFile);
				while (!dataStream.atEnd())
				{
					dataLine = dataStream.readLine();
					resRec->s2rec(dataLine);
                                        if(resRec->exptime<expMin) continue;
					if(isSysCorr)
					{
						if(!vectF->int2D(resRec->ksi, resRec->eta, resRec->mag, &dKsi, &dEta, &ni))
						{
							resRec->ksiOC -= dKsi;
							resRec->etaOC -= dEta;
						}
					}
					
					for(j=1; j<deLevNum; j++)
					{
						if((resRec->de<=deLevels[j])&&(resRec->de>deLevels[j-1])&&(fabs(resRec->ksiOC)<maxKsi)&&(fabs(resRec->etaOC)<maxEta))
						{
							deValuesX[j-1] += resRec->ksiOC;
							deValuesY[j-1] += resRec->etaOC;
							deNums[j-1] += 1;
							numTot++;
							break;
						}
					}
					
				}
			}
			
			
			out_stream << "\nstat\n";
			numMin = deNums[0];
			for(i=0; i<deLevNum-1; i++) 
			{
				nums = deNums[i];
				if(numMin>nums) numMin = nums;
				if(nums)
				{
					deValuesX[i] = deValuesX[i]/(nums*1.0);
					deValuesY[i] = deValuesY[i]/(nums*1.0);
				}
				
			}
			
			resFile.setFileName(outputFolder+"/"+"resDD.dat");
			resFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
			resStream.setDevice(&resFile);
			
			resStream << "#";
			mDiap.clear();
			for(k=0;k<deLevNum;k++) mDiap << QString("%1").arg(deLevels[k]);
			resStream << mDiap.join("|");
			resStream << "\n";
			
			
			
			for(i=0;i<deLevNum-1; i++)
			{
//						resStream << i << "|" << deValuesX[i] << "|" << deValuesY[i] << "|" << deNums[i] << "\n";
                                                resStream << QString("%1|%2|%3|%4|%5\n").arg(i, 2).arg(deValuesX[i], 9, 'f', 4).arg(deValuesY[i], 9, 'f', 4).arg(sqrt(deValuesX[i]*deValuesX[i] + deValuesY[i]*deValuesY[i]), 9, 'f', 4).arg(deNums[i]);
			}

			
			/*
///////////////////////////////////////////////////////////////	Dispersion
			for(i=0; i<di; i++)
			{
				printf("\ndet Disp UB: %f\n", (double)i*100.0/(double)di);
//				out_stream << "\n" << flist[i];
				QFile dataFile(flist[i]);
				dataFile.open(QIODevice::ReadOnly| QIODevice::Text);
				QTextStream dataStream;
				dataStream.setDevice(&dataFile);
				while (!dataStream.atEnd())
				{
					dataLine = dataStream.readLine();
					resRec->s2rec(dataLine);
                                        if(resRec->exptime<expMin) continue;
					if(isSysCorr)
					{
						if(!vectF->int2D(resRec->ksi, resRec->eta, resRec->mag, &dKsi, &dEta, &ni))
						{
							resRec->ksiOC-= dKsi;
							resRec->etaOC-= dEta;
						}
					}
					pos[0] = -1;
					for(j=1; j<mLevNum; j++)
					{
						if((resRec->mag<=mLevels[j])&&(resRec->mag>mLevels[j-1]))
						{
							pos[0] = j-1;
							break;
						}
					}
					pos[1] = -1;
					for(j=1; j<tLevNum; j++)
					{
						if(((resRec->mJD)<=tLevels[j])&&((resRec->mJD)>tLevels[j-1]))
						{
							pos[1] = j-1;
							break;
						}
					}
//					out_stream << "\n" << pos[0] << ":" << pos[1] << ":" << pos[2] << "\t" << resRec->etaOC << ":" << resRec->ksiOC << "\n";
					
					if((pos[0]!=-1)&&(pos[1]!=-1)&&(fabs(resRec->ksiOC)<maxOC)&&(fabs(resRec->etaOC)<maxOC))
					{
//						numTot++;
						cpos = xDispVectField->detPos(pos);
						xDispVectField->dD[cpos] = xDispVectField->dD[cpos]+pow(xVectField->dD[cpos]-resRec->ksiOC, 2.0);
						cpos = yDispVectField->detPos(pos);
						yDispVectField->dD[cpos] = yDispVectField->dD[cpos]+pow(yVectField->dD[cpos]-resRec->etaOC, 2.0);
//						cpos = vectNumField->detPos(pos);
//						vectNumField->lD[cpos] = vectNumField->lD[cpos] + 1; 
/*						xVectField->set(resRec->etaOC, pos);
						yVectField->set(resRec->ksiOC, pos);
						vectNumField->get(&nums, pos);
						vectNumField->set((long)(nums+1), pos);*
					}
				}
			}
			
			sz = xDispVectField->getSize();
			out_stream << "\nstat\n";
//			int numMin = vectNumField->lD[0];
			for(p0=0; p0<sz; p0++)
			{
				nums = vectNumField->lD[p0];
//				if(numMin>nums) numMin = nums;
				if(nums)
				{
					xDispVectField->dD[p0] = sqrt(xDispVectField->dD[p0]/(nums*1.0 - 1.0));
					yDispVectField->dD[p0] = sqrt(yDispVectField->dD[p0]/(nums*1.0 - 1.0));
				}
				
				out_stream << "\n" << p0 << ":" << QString("%1").arg(xDispVectField->dD[p0]) << ":" << yDispVectField->dD[p0] << ":" << nums << "\n";
			}
			
			resDispFile.setFileName(outputFolder+"/"+"resDispUB.dat");
			resDispFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
			
			resDispStream.setDevice(&resDispFile);
			
			resDispStream << "#";
			mDiap.clear();
			for(k=0;k<mLevNum;k++) mDiap << QString("%1").arg(mLevels[k]);
			resDispStream << mDiap.join("|");
			resDispStream << "\n";
			
			resDispStream << "#";
			yDiap.clear();
			for(k=0;k<tLevNum;k++) yDiap << QString("%1").arg(tLevels[k], 8, 'f', 2, QLatin1Char(' '));
			resDispStream << yDiap.join("|");
			resDispStream << "\n";
			
			for(i=0;i<tLevNum-1; i++)
			{
				for(j=0;j<mLevNum-1;j++)
				{
					
						pos[0] = j;
						pos[1] = i;
						
						ocX = xDispVectField->getD(xDispVectField->detPos(pos));
						ocY = yDispVectField->getD(yDispVectField->detPos(pos));
//						yVectField->get(&ocY, pos);
						vectNumField->get(&nums, pos);
						resDispStream << QString("%1").arg(j, 5, 10, QLatin1Char(' ')) << "|" << QString("%1").arg(i, 5, 10, QLatin1Char(' ')) << "|" << QString("%1").arg(ocX, 12, 'f', 6, QLatin1Char(' ')) << "|" << QString("%1").arg(ocY, 12, 'f', 6, QLatin1Char(' ')) << "|" << QString("%1").arg(nums, 5, 10, QLatin1Char(' ')) << "\n";
					}
			}
					
////////////////////////////////////////////////////////////////			
		
			
			*/
			
			printf("\nnumTot=%d\n", numTot);
			printf("\nnumMin=%d\n", numMin);

		}
		break;
		case 7:					//Яочалелуе юйеъич дйз чъпеомудма юец мъоедлелуз
		{
			di = flist.count();
//			resRec = new residualsRec;
			
			 
			resFile.setFileName(outputFolder+"/"+"ubAster.dat");
			resFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
			resStream.setDevice(&resFile);
//			ocMagMax = settings->value("task07/ocMagMax").toDouble();
//			magMin = settings->value("task07/magMin").toDouble();
                        sz = eqAsts->ocList.size();
                        for(i=0; i<sz; i++)
			{
				printf("\ndet OMC/DEC: %f\%\n", (double)i*100.0/(double)di);
                                resStream << QString("%1").arg(eqAsts->ocList[i]->mag0, 6, 'f', 3, QLatin1Char(' '))  << "|" << QString("%1|%2").arg(eqAsts->ocList[i]->ocRa , 10, 'f', 3, QLatin1Char(' ')).arg(eqAsts->ocList[i]->ocDe, 10, 'f', 3, QLatin1Char(' ')) << "\n";
			}
		}
		break;
		case 8:					//Яочалелуе юйеъич дйз чъпеомудма c мъоедлелуек
		{

//			resRec = new residualsRec;
			
                        out_stream << "\n\ntask8\n\n";
                        qDebug() << "\n\ntask8\n\n";
        /*
			int deLevNum;
			int deLevStep = settings->value("task08/deDiaps").toInt();
			out_stream << "deLevStep " << deLevStep << "\n";
			double stepDe;
          */






                        if(isSysCorr)
                        {
                            errFile0.setFileName("./errCorRA.dat");
                            errFile0.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
                            errDataStream0.setDevice(&errFile0);
                            errFile1.setFileName("./errCorDEC.dat");
                            errFile1.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
                            errDataStream1.setDevice(&errFile1);


                        }
                        else
                        {
                            errFile0.setFileName("./errRA.dat");
                            errFile0.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
                            errDataStream0.setDevice(&errFile0);
                            errFile1.setFileName("./errDEC.dat");
                            errFile1.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
                            errDataStream1.setDevice(&errFile1);
                        }

                        szCols = ocRa0.size();
                        for(i=0; i<szCols; i++)
                        {
                            ocRa0[i]->rec2s(&tstr);
                            errDataStream0 << tstr << "\n";
                            ocDec0[i]->rec2s(&tstr);
                            errDataStream1 << tstr << "\n";
                        }

                        errFile0.close();
                        errFile1.close();

                        if(isSysCorr)
                        {
                            detMeanCols("./errCorRA.dat");
                            detMeanCols("./errCorDEC.dat");
                        }
                        else
                        {
                            detMeanCols("./errRA.dat");
                            detMeanCols("./errDEC.dat");
                        }


                        sz = eqAsts->ocList.size();
                        out_stream << "num of OMC: " << sz << "\n";
                        qDebug() << "num of OMC: " << sz << "\n";

                        mLevNum = settings->value("task08/mLevNum").toInt();
                        mLevNumBeg = settings->value("task08/mLevNumBeg").toDouble();
                        mLevStep = settings->value("task08/mLevStep").toDouble();
                        mLevels = new double[mLevNum];

                        mLevels[0] = 0.0;
                        for(i=1; i<mLevNum-1; i++) mLevels[i] = (mLevNumBeg+i*mLevStep-1)*1.0;
                        mLevels[mLevNum-1] = 20.0;


			diniKey = 1;

                        for(i=0; i<sz; i++)
			{
                            printf("\ndet Diaps UB: %f\%\n", (double)i*100.0/(double)sz);
                                if(diniKey)
                                {
                                        naxeX[1] = naxeX[0] = eqAsts->ocList.at(i)->ocRa;
                                        naxeY[1] = naxeY[0] = eqAsts->ocList.at(i)->ocDe;
                                        naxeDe[1] = naxeDe[0] = eqAsts->ocList.at(i)->mag0;
                                        diniKey = 0;
                                }
                                else
                                {
                                        if(naxeX[0]>eqAsts->ocList.at(i)->ocRa) naxeX[0] = eqAsts->ocList.at(i)->ocRa;
                                        if(naxeX[1]<eqAsts->ocList.at(i)->ocRa) naxeX[1] = eqAsts->ocList.at(i)->ocRa;
                                        if(naxeY[0]>eqAsts->ocList.at(i)->ocDe) naxeY[0] = eqAsts->ocList.at(i)->ocDe;
                                        if(naxeY[1]<eqAsts->ocList.at(i)->ocDe) naxeY[1] = eqAsts->ocList.at(i)->ocDe;
                                        if(naxeDe[0]>eqAsts->ocList.at(i)->mag0) naxeDe[0] = eqAsts->ocList.at(i)->mag0;
                                        if(naxeDe[1]<eqAsts->ocList.at(i)->mag0) naxeDe[1] = eqAsts->ocList.at(i)->mag0;
                                }

			}

			
/*			int sgn = fabs(naxeDe[0])/naxeDe[0];
			int levMin = sgn*((int)(fabs(naxeDe[0]))+1);
			sgn = fabs(naxeDe[1])/naxeDe[1];
			int levMax = sgn*((int)(fabs(naxeDe[1]))+1);
                        deLevNum = ((levMax-levMin)/deLevStep)+1;
			
			out_stream << "levMin " << levMin << "\n";
			out_stream << "levMax " << levMax << "\n";
			out_stream << "deLevNum " << deLevNum << "\n";
			
                        double* deLevels = new double[deLevNum];*/
                        double* mValuesX = new double[mLevNum-1];
                        double* mValuesY = new double[mLevNum-1];
                        int* mNums = new int[mLevNum-1];
			

                        for(i=0; i<mLevNum-1; i++) {mValuesX[i] = mValuesY[i] = 0.0;mNums[i]=0;}

			numTot = 0;

                        for(i=0; i<sz; i++)
			{
                            printf("\ndet Mean UB: %f\%\n", (double)i*100.0/(double)sz);
//
                            for(j=1; j<mLevNum; j++)
                            {
                                if((eqAsts->ocList.at(i)->mag0<=mLevels[j])&&(eqAsts->ocList.at(i)->mag0>mLevels[j-1]))
                                {
                                    mValuesX[j-1] += eqAsts->ocList.at(i)->ocRa;
                                    mValuesY[j-1] += eqAsts->ocList.at(i)->ocDe;
                                    mNums[j-1] += 1;
                                    numTot++;
                                    break;
                                }
                            }
							

			}
			
			out_stream << "\nstat\n";
                        numMin = mNums[0];
                        for(i=0; i<mLevNum-1; i++)
			{
                                nums = mNums[i];
				if(numMin>nums) numMin = nums;
				if(nums)
				{
                                        mValuesX[i] = mValuesX[i]/(nums*1.0);
                                        mValuesY[i] = mValuesY[i]/(nums*1.0);
				}
				
			}
			
			resFile.setFileName(outputFolder+"/"+"resUBA.dat");
			resFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
			resStream.setDevice(&resFile);
			
			resStream << "#";
			mDiap.clear();
                        for(k=0;k<mLevNum;k++) mDiap << QString("%1").arg(mLevels[k]);
			resStream << mDiap.join("|");
			resStream << "\n";
			
			
			
                        for(i=0;i<mLevNum-1; i++)
			{
//                                                resStream << i << "|" << mValuesX[i] << "|" << mValuesY[i] << "|" << mNums[i] << "\n";
                                                resStream << QString("%1|%2|%3|%4|%5\n").arg(i, 2).arg(mValuesX[i], 9, 'f', 4).arg(mValuesY[i], 9, 'f', 4).arg(sqrt(mValuesX[i]*mValuesX[i] + mValuesY[i]*mValuesY[i]), 9, 'f', 4).arg(mNums[i]);
			}
    //                    eqAsts->saveAs("total_eq.txt");
		}
		break;
                case 9:					//Рчцч дйз чъпеомудма c мъоедлелуек
		{
                        di = eqAsts->ocList.size();
//			resRec = new residualsRec;
			
			out_stream << "di " << di << "\n";
			int deLevNum;
			int deLevStep = settings->value("task09/deDiaps").toInt();
			
			double stepDe;
		
//			int deLevNum = settings->value("task09/magnDiaps").toInt()+1;
//			ocMagMax = settings->value("task09/ocMagMax").toDouble();
//			magMin = settings->value("task09/magMin").toDouble();
			double phaseMax = settings->value("task09/phaseMax").toDouble();
/*			
			double* deLevels = new double[deLevNum];
			double* deValuesX = new double[deLevNum-1];
			double* deValuesY = new double[deLevNum-1];
			int* deNums = new int[deLevNum-1];
			double stepDe;
*/			
			
			diniKey = 1;
			for(i=0; i<di; i++)
			{
				printf("\nfindDiaps: %f\%\n", (double)i*100.0/(double)di);
				

                                if(diniKey)
                                {
/*                                        naxeX[1] = naxeX[0] = eqAsts->ocList[i]->ocRa;
                                        naxeY[1] = naxeY[0] = eqAsts->ocList[i]->ocDe;*/
                                        naxeDe[1] = naxeDe[0] = eqAsts->ocList[i]->phase;
                                        diniKey = 0;
                                }
                                else
                                {
/*                                        if(naxeX[0]>eqAsts->ocList[i]->ocRa) naxeX[0] = eqAsts->ocList[i]->ocRa;
                                        if(naxeX[1]<eqAsts->ocList[i]->ocRa) naxeX[1] = eqAsts->ocList[i]->ocRa;
                                        if(naxeY[0]>eqAsts->ocList[i]->ocDe) naxeY[0] = eqAsts->ocList[i]->ocDe;
                                        if(naxeY[1]<eqAsts->ocList[i]->ocDe) naxeY[1] = eqAsts->ocList[i]->ocDe;*/
                                        if(naxeDe[0]>eqAsts->ocList[i]->phase) naxeDe[0] = eqAsts->ocList[i]->phase;
                                        if(naxeDe[1]<eqAsts->ocList[i]->phase) naxeDe[1] = eqAsts->ocList[i]->phase;
                                }

				
			}

                        if(naxeDe[1]>phaseMax) naxeDe[1]=phaseMax;

			int sgn = fabs(naxeDe[0])/naxeDe[0];
			int levMin = sgn*((int)(fabs(naxeDe[0]))+1);
			sgn = fabs(naxeDe[1])/naxeDe[1];
			int levMax = sgn*((int)(fabs(naxeDe[1]))+1);
			deLevNum = ((levMax-levMin)/deLevStep)+2;
			
			
			double* deLevels = new double[deLevNum];
			double* deValuesX = new double[deLevNum-1];
			double* deValuesY = new double[deLevNum-1];
                        double* deValuesZ = new double[deLevNum-1];
			int* deNums = new int[deLevNum-1];
			
			
			stepDe = (naxeDe[1]-naxeDe[0])/(double)(deLevNum-1);
			
			out_stream << "de diaps: " << naxeDe[0] << "::" << naxeDe[1] << "::" << stepDe << "\n";
			
			for(i=0; i<deLevNum; i++) deLevels[i] = levMin+deLevStep*i;
                        for(i=0; i<deLevNum-1; i++) {deValuesX[i] = deValuesY[i] = deValuesZ[i] = 0.0;deNums[i]=0;}

			numTot = 0;
			
			
	//		resRec = new residualsRec;
			
			for(i=0; i<di; i++)
			{
				printf("\ndet Mean UB: %f\%\n", (double)i*100.0/(double)di);
                                for(j=1; j<deLevNum; j++)
                                {
                                        if((eqAsts->ocList[i]->phase<=deLevels[j])&&(eqAsts->ocList[i]->phase>deLevels[j-1]))
                                        {
                                                deValuesX[j-1] += eqAsts->ocList[i]->ocRa;
                                                deValuesY[j-1] += eqAsts->ocList[i]->ocDe;
                                                deValuesZ[j-1] += sqrt(eqAsts->ocList[i]->ocRa*eqAsts->ocList[i]->ocRa + eqAsts->ocList[i]->ocDe*eqAsts->ocList[i]->ocDe);
                                                deNums[j-1] += 1;
                                                numTot++;
                                                break;
                                        }
                                }
			}
			
			out_stream << "\nstat\n";
			numMin = deNums[0];
			for(i=0; i<deLevNum-1; i++) 
			{
				nums = deNums[i];
				if(numMin>nums) numMin = nums;
				if(nums)
				{
					deValuesX[i] = deValuesX[i]/(nums*1.0);
					deValuesY[i] = deValuesY[i]/(nums*1.0);
                                        deValuesZ[i] = deValuesZ[i]/(nums*1.0);
				}
				
			}
			
			resFile.setFileName(outputFolder+"/"+"resPhaseA.dat");
			resFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
			resStream.setDevice(&resFile);
			
			resStream << "#";
			mDiap.clear();
			for(k=0;k<deLevNum;k++) mDiap << QString("%1").arg(deLevels[k]);
			resStream << mDiap.join("|");
			resStream << "\n";
			
			
			
			for(i=0;i<deLevNum-1; i++)
			{
                            resStream << QString("%1|%2|%3|%4|%5\n").arg(i, 2).arg(deValuesX[i], 9, 'f', 4).arg(deValuesY[i], 9, 'f', 4).arg(deValuesZ[i], 9, 'f', 4).arg(deNums[i]);
			}
		}
		break;
		case 10:					//Цчауъукмъпщ мп ъимомъпу дйз чъпеомудма c мъоедлелуек
		{
                        di = eqAsts->ocList.size();
//			resRec = new residualsRec;
			
			out_stream << "di " << di << "\n";
			
			
			int deLevNum = settings->value("task10/magnDiaps").toInt()+1;
			double phaseMax = settings->value("task10/phaseMax").toDouble();
			
			double* deLevels = new double[deLevNum];
			double* deLevels0 = new double[deLevNum];
			double* deValuesX = new double[deLevNum-1];
			double* deValuesY = new double[deLevNum-1];
			int* deNums = new int[deLevNum-1];
			int* deNums0 = new int[deLevNum-1];
			double stepDe, stepDe0;
			int numTot0;
			
			
			diniKey = 1;
			for(i=0; i<di; i++)
			{
				printf("\nfindDiaps: %f\%\n", (double)i*100.0/(double)di);

                                if(diniKey)
                                {
                                        naxeX[1] = naxeX[0] = eqAsts->ocList[i]->ocRa;
                                        naxeY[1] = naxeY[0] = eqAsts->ocList[i]->ocDe;
                                        naxeDe[1] = naxeDe[0] = eqAsts->ocList[i]->muRa;
                                        naxeDe0[1] = naxeDe0[0] = eqAsts->ocList[i]->muDe;
                                        diniKey = 0;
                                }
                                else
                                {
                                        if(naxeX[0]>eqAsts->ocList[i]->ocRa) naxeX[0] = eqAsts->ocList[i]->ocRa;
                                        if(naxeX[1]<eqAsts->ocList[i]->ocRa) naxeX[1] = eqAsts->ocList[i]->ocRa;
                                        if(naxeY[0]>eqAsts->ocList[i]->ocDe) naxeY[0] = eqAsts->ocList[i]->ocDe;
                                        if(naxeY[1]<eqAsts->ocList[i]->ocDe) naxeY[1] = eqAsts->ocList[i]->ocDe;
                                        if(naxeDe[0]>eqAsts->ocList[i]->muRa) naxeDe[0] = eqAsts->ocList[i]->muRa;
                                        if(naxeDe[1]<eqAsts->ocList[i]->muRa) naxeDe[1] = eqAsts->ocList[i]->muRa;
                                        if(naxeDe0[0]>eqAsts->ocList[i]->muDe) naxeDe0[0] = eqAsts->ocList[i]->muDe;
                                        if(naxeDe0[1]<eqAsts->ocList[i]->muDe) naxeDe0[1] = eqAsts->ocList[i]->muDe;
                                }
                                out_stream << "mu_ra: " << eqAsts->ocList[i]->muRa << "\n";

			}

			stepDe = (naxeDe[1]-naxeDe[0])/(double)(deLevNum-1);
			stepDe0 = (naxeDe0[1]-naxeDe0[0])/(double)(deLevNum-1);
			
			out_stream << "de diaps: " << naxeDe[0] << "::" << naxeDe[1] << "::" << stepDe << "\n";
			out_stream << "de0 diaps: " << naxeDe0[0] << "::" << naxeDe0[1] << "::" << stepDe << "\n";
			
			for(i=0; i<deLevNum; i++){deLevels[i] = naxeDe[0]+stepDe*i;deLevels0[i] = naxeDe0[0]+stepDe0*i;}
			for(i=0; i<deLevNum-1; i++) {deValuesX[i] = deValuesY[i] = 0.0;deNums[i]=0;}

			numTot = 0;
			
			
	//		resRec = new residualsRec;
			
			for(i=0; i<di; i++)
			{
                            printf("\ndet Mean Vel: %f\%\n", (double)i*100.0/(double)di);
                            for(j=1; j<deLevNum; j++)
                            {
                                if((eqAsts->ocList[i]->muRa<=deLevels[j])&&(eqAsts->ocList[i]->muRa>deLevels[j-1]))
                                {
                                    deValuesX[j-1] += eqAsts->ocList[i]->ocRa;
                                    deNums[j-1] += 1;
                                    numTot++;
                                    break;
                                }
                                if((eqAsts->ocList[i]->muDe<=deLevels0[j])&&(eqAsts->ocList[i]->muDe>deLevels0[j-1]))
                                {
                                    deValuesY[j-1] += eqAsts->ocList[i]->ocDe;
                                    deNums0[j-1] += 1;
                                    numTot0++;
                                    break;
                                }
                            }
			}
			
			out_stream << "\nstat\n";
			numMin = deNums[0];
			for(i=0; i<deLevNum-1; i++) 
			{
				nums = deNums[i];
				if(numMin>nums) numMin = nums;
                                if(nums) deValuesX[i] = deValuesX[i]/(nums*1.0);

				nums = deNums0[i];
				if(numMin>nums) numMin = nums;
                                if(nums) deValuesY[i] = deValuesY[i]/(nums*1.0);
			}
			
			resFile.setFileName(outputFolder+"/"+"resVelRa.dat");
			resFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
			resStream.setDevice(&resFile);
			
			resStream << "#";
			mDiap.clear();
			for(k=0;k<deLevNum;k++) mDiap << QString("%1").arg(deLevels[k]);
			resStream << mDiap.join("|");
			resStream << "\n";
			
                        for(i=0;i<deLevNum-1; i++) resStream << i << "|" << deValuesX[i] << "|" << deNums[i] << "\n";
			resFile.close();
			
			resFile.setFileName(outputFolder+"/"+"resVelDec.dat");
			resFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
			resStream.setDevice(&resFile);
			
			resStream << "#";
			mDiap.clear();
			for(k=0;k<deLevNum;k++) mDiap << QString("%1").arg(deLevels0[k]);
			resStream << mDiap.join("|");
			resStream << "\n";
			
                        for(i=0;i<deLevNum-1; i++) resStream << i << "|" << deValuesY[i] << "|" << deNums0[i] << "\n";
                        resFile.close();
		}
		break;
		case 11:					//Цчауъукмъпщ KsiOC у EtaOC мп (u2R-H)-фаепч ъ мъоедлелуек
		{
			resRec = new residualsRec;
			di = flist.count();
			int colLevNum = settings->value("task11/colDiaps").toInt()+1;
			double* colLevels = new double[colLevNum];
			int* colNums = new int[colLevNum-1];
			double stepCol;
			
			mLevNum = settings->value("task11/mLevNum").toInt();
			
			diniKey = 1;
			for(i=0; i<di; i++)
			{
				printf("\nfindDiaps: %f\%\n", (double)i*100.0/(double)di);
				QFile diapFile(flist[i]);
				diapFile.open(QIODevice::ReadOnly| QIODevice::Text);
				QTextStream dataStream;
				dataStream.setDevice(&diapFile);
				while (!dataStream.atEnd())
				{
					dataLine = dataStream.readLine();
					resRec->s2rec(dataLine);
                                        if(resRec->exptime<expMin) continue;
					if(isSysCorr)
					{
						if(!vectF->int2D(resRec->ksi, resRec->eta, resRec->mag, &dKsi, &dEta, &ni))
						{
							resRec->ksiOC-= dKsi;
							resRec->etaOC-= dEta;
						}
					}
					if((fabs(resRec->ksiOC)<maxKsi)&&(fabs(resRec->etaOC)<maxEta)&&(resRec->u2R>mLevNum)&&(resRec->J>mLevNum)&&(fabs(resRec->u2R-resRec->J)<mLevNum))
					{
						if(diniKey)
						{

							naxeDe[1] = naxeDe[0] = (resRec->u2R-resRec->J);
							diniKey = 0;
						}
						else
						{
							if(naxeDe[0]>(resRec->u2R-resRec->J)) naxeDe[0] = (resRec->u2R-resRec->J);
							if(naxeDe[1]<(resRec->u2R-resRec->J)) naxeDe[1] = (resRec->u2R-resRec->J);
//							out_stream << "u2R: " << (resRec->u2R-resRec->Ks) << "\n";
						}
					}
				}
				diapFile.close();
			}
			
			stepCol = (naxeDe[1]-naxeDe[0])/(double)(colLevNum-1);
			
			out_stream << "CI diaps: " << naxeDe[0] << "::" << naxeDe[1] << "::" << stepCol << "\n";
			
			for(i=0; i<colLevNum; i++) colLevels[i] = naxeDe[0]+stepCol*i;


			QString decDiapsStr = settings->value("task11/decDiaps").toString();
			QStringList decDiapsSL = decDiapsStr.split("|");
			int decLevNum = decDiapsSL.size();
			
			out_stream << "DEC diaps: " << decDiapsSL.join("|") << "\n";

			decLevels = new double[decLevNum];

			for(i=0; i<decLevNum; i++)
			{
				decLevels[i] = decDiapsSL[i].toDouble();
				out_stream << decLevels[i] << "\n";
			}
			
			axesNum = 2;
			axes = new int[axesNum];
			pos = new int[axesNum];
			axes[0] = colLevNum-1;
			axes[1] = decLevNum-1;
			xVectField = new multidim(DOUBLE_TYPE, axes, axesNum);
			yVectField = new multidim(DOUBLE_TYPE, axes, axesNum);
			xDispVectField = new multidim(DOUBLE_TYPE, axes, axesNum);
			yDispVectField = new multidim(DOUBLE_TYPE, axes, axesNum);
			vectNumField = new multidim(LONG_TYPE, axes, axesNum);
			
			xVectField->clear();
			yVectField->clear();
			xDispVectField->clear();
			yDispVectField->clear();
			vectNumField->clear();
			
			sz = xVectField->getSize();
			
			numTot = 0;
			for(i=0; i<di; i++)
			{
				printf("\ndet Mean UCOL: %f\%\n", (double)i*100.0/(double)di);
//				out_stream << "\n" << flist[i];
				QFile dataFile(flist[i]);
				dataFile.open(QIODevice::ReadOnly| QIODevice::Text);
				QTextStream dataStream;
				dataStream.setDevice(&dataFile);
				while (!dataStream.atEnd())
				{
					dataLine = dataStream.readLine();
					resRec->s2rec(dataLine);
                                        if(resRec->exptime<expMin) continue;
					
					if(isSysCorr)
					{
						if(!vectF->int2D(resRec->ksi, resRec->eta, resRec->mag, &dKsi, &dEta, &ni))
						{
							resRec->ksiOC-= dKsi;
							resRec->etaOC-= dEta;
						}
					}
					
					pos[0] = -1;
					for(j=1; j<colLevNum; j++)
					{
						if(((resRec->u2R-resRec->J)<=colLevels[j])&&((resRec->u2R-resRec->J)>colLevels[j-1]))
						{
							pos[0] = j-1;
							break;
						}
					}
					pos[1] = -1;
					for(j=1; j<decLevNum; j++)
					{
						if(((resRec->de)<=decLevels[j])&&((resRec->de)>decLevels[j-1]))
						{
							pos[1] = j-1;
							break;
						}
					}
//					out_stream << "\n" << pos[0] << ":" << pos[1]  << "\t" << resRec->etaOC << ":" << resRec->ksiOC << "\n";
					
					if((pos[0]!=-1)&&(pos[1]!=-1)&&(fabs(resRec->ksiOC)<maxKsi)&&(fabs(resRec->etaOC)<maxEta)&&(resRec->u2R>mLevNum)&&(resRec->J>mLevNum)&&(fabs(resRec->u2R-resRec->J)<mLevNum))
					{
						numTot++;
						cpos = xVectField->detPos(pos);
						xVectField->dD[cpos] += resRec->ksiOC;
						yVectField->dD[cpos] += resRec->etaOC;
						cpos = vectNumField->detPos(pos);
						vectNumField->lD[cpos] = vectNumField->lD[cpos] + 1; 
        //case
					}
				}
			}
			
			sz = xVectField->getSize();
			out_stream << "\nstat\n";
			numMin = vectNumField->lD[0];
			for(p0=0; p0<sz; p0++)
			{
				nums = vectNumField->lD[p0];
				if(numMin>nums) numMin = nums;
				if(nums)
				{
					xVectField->dD[p0] = xVectField->dD[p0]/(nums*1.0);
					yVectField->dD[p0] = yVectField->dD[p0]/(nums*1.0);
				}
				
				out_stream << "\n" << p0 << ":" << QString("%1").arg(xVectField->dD[p0]) << ":" << yVectField->dD[p0] << ":" << nums << "\n";
			}
			
			resFile.setFileName(outputFolder+"/"+"resUC.dat");
			resFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
			resStream.setDevice(&resFile);
			
			resStream << "#";
			mDiap.clear();
			for(k=0;k<colLevNum;k++) mDiap << QString("%1").arg(colLevels[k]);
			resStream << mDiap.join("|");
			resStream << "\n";
			
			resStream << "#";
			yDiap.clear();
			for(k=0;k<decLevNum;k++) yDiap << QString("%1").arg(decLevels[k], 8, 'f', 2, QLatin1Char(' '));
			resStream << yDiap.join("|");
			resStream << "\n";
			
			for(i=0;i<decLevNum-1; i++)
			{
				for(j=0;j<colLevNum-1;j++)
				{
					
						pos[0] = j;
						pos[1] = i;
						
						ocX = xVectField->getD(xDispVectField->detPos(pos));
						ocY = yVectField->getD(yDispVectField->detPos(pos));
//						yVectField->get(&ocY, pos);
						nums = vectNumField->getL(vectNumField->detPos(pos));
						resStream << QString("%1").arg(j, 5, 10, QLatin1Char(' ')) << "|" << QString("%1").arg(i, 5, 10, QLatin1Char(' ')) << "|" << QString("%1").arg(ocX, 12, 'f', 6, QLatin1Char(' ')) << "|" << QString("%1").arg(ocY, 12, 'f', 6, QLatin1Char(' ')) << "|" << QString("%1").arg(nums, 8, 10, QLatin1Char(' ')) << "\n";
					}
			}
			
			printf("\nnumTot=%d\n", numTot);
			printf("\nnumMin=%d\n", numMin);
		}
		break;
		case 12:					//Цчауъукмъпщ OMC мп рчць дйз мпдейщльс чъпеомудма юец мъоедлелуз
		{
                        di = eqAsts->ocList.size();
//			resRec = new residualsRec;
			
			 
			
//			ocMagMax = settings->value("task12/ocMagMax").toDouble();
//			magMin = settings->value("task12/magMin").toDouble();

  //                      eqFile *curAst = new eqFile;
			
				
			for(i=0; i<di; i++)
			{
                            printf("\ndet OMC/Phase: %f\%\n", (double)i*100.0/(double)di);
					
                            resFile.setFileName(outputFolder+"/"+QString("%1").arg(eqAsts->ocList[i]->name)+".pha");
//                            curAst->init(outputFolder+"/"+QString("%1").arg(eqAsts->ocList[i]->name)+"_eq.txt");
                            resFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Append);
                            resStream.setDevice(&resFile);
                            resStream << QString("%1").arg(eqAsts->ocList[i]->phase, 6, 'f', 3, QLatin1Char(' '))  << "|" << QString("%1|%2|%3").arg(eqAsts->ocList[i]->ocRa , 10, 'f', 3, QLatin1Char(' ')).arg(eqAsts->ocList[i]->ocDe, 10, 'f', 3, QLatin1Char(' ')).arg(sqrt(eqAsts->ocList[i]->ocDe*eqAsts->ocList[i]->ocDe+eqAsts->ocList[i]->ocRa*eqAsts->ocList[i]->ocRa), 10, 'f', 3, QLatin1Char(' ')) << "\n";
                            resFile.close();
                            resFile.setFileName(outputFolder+"/"+QString("%1").arg(eqAsts->ocList[i]->name)+"_eq.txt");
                            resFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Append);
                            resStream.setDevice(&resFile);
                            eqAsts->ocList[i]->rec2s(&dataLine);
                            resStream << dataLine << "\n";
                            resFile.close();
                            if(isSysCorr)
                            {
                                resFile.setFileName(outputFolder+"/"+QString("%1").arg(eqAsts->ocList[i]->name)+"_col.txt");
                                resFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Append);
                                resStream.setDevice(&resFile);
       //                         eqAsts->ocList[i]->rec2s(&dataLine);
                                resStream << QString("%1|%2#%3|%4#%5|%6##").arg(ocRa0[i]->mean, 8, 'f', 3).arg(ocRa1[i]->mean, 8, 'f', 3).arg(ocRa0[i]->rmsMean, 8, 'f', 3).arg(ocRa1[i]->rmsMean, 8, 'f', 3).arg(ocRa0[i]->rmsOne, 8, 'f', 3).arg(ocRa1[i]->rmsOne, 8, 'f', 3);
                                resStream << QString("%1|%2#%3|%4#%5|%6##").arg(ocDec0[i]->mean, 8, 'f', 3).arg(ocDec1[i]->mean, 8, 'f', 3).arg(ocDec0[i]->rmsMean, 8, 'f', 3).arg(ocDec1[i]->rmsMean, 8, 'f', 3).arg(ocDec0[i]->rmsOne, 8, 'f', 3).arg(ocDec1[i]->rmsOne, 8, 'f', 3);
                                resStream << QString("%1\n").arg(ocDec0[i]->num);
                                resFile.close();
                            }
			}
		}
		break;
		case 13:					//Цчауъукмъпщ OMC мп аоекелу дйз чъпеомудма юец мъоедлелуз
		{
                        di = eqAsts->ocList.size();
//			resRec = new residualsRec;



//			ocMagMax = settings->value("task12/ocMagMax").toDouble();
//			magMin = settings->value("task12/magMin").toDouble();


                        for(i=0; i<di; i++)
                        {
                            printf("\ndet OMC/Phase: %f\%\n", (double)i*100.0/(double)di);

                            resFile.setFileName(outputFolder+"/"+QString("%1").arg(eqAsts->ocList[i]->name)+".tm");
                            resFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Append);
                            resStream.setDevice(&resFile);
                            resStream << QString("%1").arg(eqAsts->ocList[i]->MJday, 6, 'f', 3, QLatin1Char(' '))  << "|" << QString("%1|%2|%3").arg(eqAsts->ocList[i]->ocRa , 10, 'f', 3, QLatin1Char(' ')).arg(eqAsts->ocList[i]->ocDe, 10, 'f', 3, QLatin1Char(' ')).arg(sqrt(eqAsts->ocList[i]->ocDe*eqAsts->ocList[i]->ocDe+eqAsts->ocList[i]->ocRa*eqAsts->ocList[i]->ocRa), 10, 'f', 3, QLatin1Char(' ')) << "\n";
                            resFile.close();
                        }
		}
		break;
		case 14:					//Цчауъукмъпщ OMC мп ъимомъпу дйз чъпеомудма юец мъоедлелуз
		{
			di = flist.count();
//			resRec = new residualsRec;
			
			 
			
//			ocMagMax = settings->value("task14/ocMagMax").toDouble();
//			magMin = settings->value("task14/magMin").toDouble();
			
			resFile.setFileName(outputFolder+"/"+"resMuRa.dat");
			resFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
			resStream.setDevice(&resFile);
			
			resFile0.setFileName(outputFolder+"/"+"resMuDec.dat");
			resFile0.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
			resStream0.setDevice(&resFile0);
			
			for(i=0; i<di; i++)
			{
				printf("\ndet OMC/Time: %f\%\n", (double)i*100.0/(double)di);
//				out_stream << "\n" << flist[i];
				eqData->clear();
				tmp = flist[i].toAscii();
				fname = tmp.data();
				eqData->init(fname);
				
				if((eqData->ocList.size()>1)&&(eqData->colList.size()>=2))//&&(eqData->colList[6]->mean<ocMagMax)&&(eqData->colList[3]->mean>magMin))
				{
					out_stream << "\n\n" << flist[i] << ":\n\n";
//					qDebug() << flist[i] << ":\n\n";
					if(isSysCorr)
					{
							sz = eqData->colList.size();
	//						out_stream << "sz= " << sz << "\n";
							if(sz>7)
							{
								calcRa = eqData->colList[1]->mean - eqData->colList[4]->mean;
								calcDec = eqData->colList[2]->mean - eqData->colList[5]->mean;
								
//								out_stream << "calcRa " << calcRa << "\tcalcDec " << calcDec << "\n";
								
								getRaDeToTang1(&ksiObs, &etaObs, eqData->colList[1]->mean, eqData->colList[2]->mean, eqData->colList[7]->mean, eqData->colList[8]->mean);
//								out_stream << "ksiObs " << ksiObs << "\tetaObs " << etaObs << "\tmagn " << eqData->colList[3]->mean <<  "\n";
								if(eqData->colList[3]->mean<0.0) eqData->colList[3]->mean = 0.1;
								vectF->int2D(ksiObs, etaObs, eqData->colList[3]->mean, &dKsi, &dEta, &ni);
//								out_stream << "dKsi " << dKsi << "\tdEta " << dEta << "\n";
								dKsi /= 1000.0;
								dEta /= 1000.0;
								ksiObs -= dKsi;
								etaObs -= dEta;
//								out_stream << "ksiObs " << ksiObs << "\tetaObs " << etaObs << "\n";
								getTangToRaDe1(&obsRa, &obsDec, ksiObs, etaObs, eqData->colList[7]->mean, eqData->colList[8]->mean);
//								out_stream << "obsRa " << obsRa << "\tobsDec " << obsDec << "\n";
							}
							
							meanRa = obsRa-calcRa;
							meanDec = obsDec-calcDec;
						
						if((fabs(meanRa)>maxKsi)||(fabs(meanDec)>maxEta)) continue;
//						qDebug() << "count= " << count << "\n";
						count = 0;
						
					}
					else
					{
						meanRa = eqData->colList[4]->mean;
						meanDec = eqData->colList[5]->mean;
                                                sigmaRa = eqData->colList[4]->rmsOne;
                                                sigmaDec = eqData->colList[5]->rmsOne;
					}
					
                                        if((sqrt(pow(eqData->mmRec->Xdot, 2.0) + pow(eqData->mmRec->Ydot, 2.0))<velMax))
					{
                                                resStream << QString("%1").arg(fabs(eqData->mmRec->Xdot), 6, 'f', 3, QLatin1Char(' '))  << "\t" << QString("%1").arg(meanRa , 10, 'f', 3, QLatin1Char(' ')) << "\n";
                                                resStream0 << QString("%1").arg(fabs(eqData->mmRec->Ydot), 6, 'f', 3, QLatin1Char(' '))  << "\t" << QString("%1").arg(meanDec, 10, 'f', 3, QLatin1Char(' ')) << "\n";
					}
					//resFile.close();
				}
				
			}
		}
		break;
		case 15:					//Цчауъукмъпщ мп ъимомъпу дйз чъпеомудма c мъоедлелуек
		{
			di = flist.count();
//			resRec = new residualsRec;
			
			out_stream << "di " << di << "\n";
			int deLevNum;
			int deLevStep = settings->value("task15/deDiaps").toInt();
			
			double stepDe, stepDe0;
		
//			int deLevNum = settings->value("task09/magnDiaps").toInt()+1;
//			ocMagMax = settings->value("task09/ocMagMax").toDouble();
//			magMin = settings->value("task09/magMin").toDouble();
			double phaseMax = settings->value("task15/phaseMax").toDouble();
/*			
			double* deLevels = new double[deLevNum];
			double* deValuesX = new double[deLevNum-1];
			double* deValuesY = new double[deLevNum-1];
			int* deNums = new int[deLevNum-1];
			double stepDe;
*/			
			
			diniKey = 1;
			for(i=0; i<di; i++)
			{
				printf("\nfindDiaps: %f\%\n", (double)i*100.0/(double)di);
				
				eqData->clear();
				tmp = flist[i].toAscii();
				fname = tmp.data();
				eqData->init(fname);
				
/*				QFile diapFile(flist[i]);
				diapFile.open(QIODevice::ReadOnly| QIODevice::Text);
				QTextStream dataStream;
				dataStream.setDevice(&diapFile);
				while (!dataStream.atEnd())
				{
					dataLine = dataStream.readLine();
                                        resRec->s2rec(dataLine);
                                        if(resRec->exptime<expMin) continue;
*/
				if((eqData->ocList.size()>1)&&(eqData->colList.size()>=2)&&(eqData->colList[6]->mean<ocMagMax)&&(eqData->colList[3]->mean>magMin))
				{
                                        if((sqrt(pow(eqData->mmRec->Xdot, 2.0) + pow(eqData->mmRec->Ydot, 2.0))<velMax))
					{
						if(isSysCorr)
						{
								sz = eqData->colList.size();
		//						out_stream << "sz= " << sz << "\n";
								if(sz>7)
								{
									calcRa = eqData->colList[1]->mean - eqData->colList[4]->mean;
									calcDec = eqData->colList[2]->mean - eqData->colList[5]->mean;
									
	//								out_stream << "calcRa " << calcRa << "\tcalcDec " << calcDec << "\n";
									
									getRaDeToTang1(&ksiObs, &etaObs, eqData->colList[1]->mean, eqData->colList[2]->mean, eqData->colList[7]->mean, eqData->colList[8]->mean);
	//								out_stream << "ksiObs " << ksiObs << "\tetaObs " << etaObs << "\tmagn " << eqData->colList[3]->mean <<  "\n";
									if(eqData->colList[3]->mean<0.0) eqData->colList[3]->mean = 0.1;
									vectF->int2D(ksiObs, etaObs, eqData->colList[3]->mean, &dKsi, &dEta, &ni);
	//								out_stream << "dKsi " << dKsi << "\tdEta " << dEta << "\n";
									dKsi /= 1000.0;
									dEta /= 1000.0;
									ksiObs -= dKsi;
									etaObs -= dEta;
	//								out_stream << "ksiObs " << ksiObs << "\tetaObs " << etaObs << "\n";
									getTangToRaDe1(&obsRa, &obsDec, ksiObs, etaObs, eqData->colList[7]->mean, eqData->colList[8]->mean);
	//								out_stream << "obsRa " << obsRa << "\tobsDec " << obsDec << "\n";
								}
								
								meanRa = obsRa-calcRa;
								meanDec = obsDec-calcDec;
							
							if((fabs(meanRa)>maxKsi)||(fabs(meanDec)>maxEta)) continue;
	//						qDebug() << "count= " << count << "\n";
							count = 0;
						
							
						}
						else
						{
							meanRa = eqData->colList[4]->mean;
							meanDec = eqData->colList[5]->mean;
                                                        sigmaRa = eqData->colList[4]->rmsOne;
                                                        sigmaDec = eqData->colList[5]->rmsOne;
						}
						if((fabs(meanRa)<maxKsi)&&(fabs(meanDec)<maxEta))
						{
							if(diniKey)
							{
								naxeX[1] = naxeX[0] = meanRa;
								naxeY[1] = naxeY[0] = meanDec;
								naxeDe[1] = naxeDe[0] = eqData->colList[13]->mean;
								diniKey = 0;
							}
							else
							{
								if(naxeX[0]>meanRa) naxeX[0] = meanRa;
								if(naxeX[1]<meanRa) naxeX[1] = meanRa;
								if(naxeY[0]>meanDec) naxeY[0] = meanDec;
								if(naxeY[1]<meanDec) naxeY[1] = meanDec;
                                                                if(naxeDe[0]>fabs(eqData->mmRec->Xdot)) naxeDe[0] = fabs(eqData->mmRec->Xdot);
                                                                if(naxeDe[1]<fabs(eqData->mmRec->Xdot)) naxeDe[1] = fabs(eqData->mmRec->Xdot);
                                                                if(naxeDe0[0]>fabs(eqData->mmRec->Ydot)) naxeDe0[0] = fabs(eqData->mmRec->Ydot);
                                                                if(naxeDe0[1]<fabs(eqData->mmRec->Ydot)) naxeDe0[1] = fabs(eqData->mmRec->Ydot);
							}
						}
					}
				}
				
			}
/*			if(fabs(naxeX[0])>fabs(naxeX[1])) axd = fabs(naxeX[1]);
//			else axd = fabs(naxeX[0]);
			naxeX[0] = -axd;
			naxeX[1] = axd;
			
			if(fabs(naxeY[0])>fabs(naxeY[1])) axd = fabs(naxeY[1]);
			else axd = fabs(naxeY[0]);
			naxeY[0] = axd;
			naxeY[1] = -axd;
			
*/		

			int sgn = fabs(naxeDe[0])/naxeDe[0];
			int levMin = sgn*((int)(fabs(naxeDe[0]))+1);
			sgn = fabs(naxeDe[1])/naxeDe[1];
			int levMax = sgn*((int)(fabs(naxeDe[1]))+1);
			deLevNum = ((levMax-levMin)/deLevStep)+2;
			
			sgn = fabs(naxeDe0[0])/naxeDe0[0];
			levMin = sgn*((int)(fabs(naxeDe0[0]))+1);
			sgn = fabs(naxeDe0[1])/naxeDe0[1];
			levMax = sgn*((int)(fabs(naxeDe0[1]))+1);
			int deLevNum0 = ((levMax-levMin)/deLevStep)+2;
			
			
			double* deLevels = new double[deLevNum];
			double* deLevels0 = new double[deLevNum0];
			double* deValuesX = new double[deLevNum-1];
			double* deValuesY = new double[deLevNum0-1];
			int* deNums = new int[deLevNum-1];
			int* deNums0 = new int[deLevNum0-1];
			
			
			stepDe = (naxeDe[1]-naxeDe[0])/(double)(deLevNum-1);
			stepDe0 = (naxeDe0[1]-naxeDe0[0])/(double)(deLevNum0-1);
			
			out_stream << "de diaps: " << naxeDe[0] << "::" << naxeDe[1] << "::" << stepDe << "\n";
			out_stream << "de diaps: " << naxeDe0[0] << "::" << naxeDe0[1] << "::" << stepDe0 << "\n";
			
			for(i=0; i<deLevNum; i++) deLevels[i] = levMin+deLevStep*i;
			for(i=0; i<deLevNum-1; i++) {deValuesX[i] = 0.0;deNums[i]=0;}
			
			for(i=0; i<deLevNum0; i++) deLevels0[i] = levMin+deLevStep*i;
			for(i=0; i<deLevNum0-1; i++) {deValuesY[i] = 0.0;deNums0[i]=0;}

			numTot = 0;
			int numTot0 = 0;
			
			
	//		resRec = new residualsRec;
			
			for(i=0; i<di; i++)
			{
				printf("\ndet Speed: %f\%\n", (double)i*100.0/(double)di);
//				out_stream << "\n" << flist[i];
/*				QFile dataFile(flist[i]);
				dataFile.open(QIODevice::ReadOnly| QIODevice::Text);
				QTextStream dataStream;
				dataStream.setDevice(&dataFile);*/
				
				eqData->clear();
				tmp = flist[i].toAscii();
				fname = tmp.data();
				eqData->init(fname);
				
/*				while (!dataStream.atEnd())
				{
					dataLine = dataStream.readLine();
					resRec->s2rec(dataLine);
                                        if(resRec->exptime<expMin) continue;
	*/				
				
				if((eqData->ocList.size()>1)&&(eqData->colList.size()>=2)&&(eqData->colList[6]->mean<ocMagMax)&&(eqData->colList[3]->mean>magMin))
				{
                                        if((sqrt(pow(eqData->mmRec->Xdot, 2.0) + pow(eqData->mmRec->Ydot, 2.0))<velMax))
					{
						if(isSysCorr)
						{
								sz = eqData->colList.size();
		//						out_stream << "sz= " << sz << "\n";
								if(sz>7)
								{
									calcRa = eqData->colList[1]->mean - eqData->colList[4]->mean;
									calcDec = eqData->colList[2]->mean - eqData->colList[5]->mean;
									
	//								out_stream << "calcRa " << calcRa << "\tcalcDec " << calcDec << "\n";
									
									getRaDeToTang1(&ksiObs, &etaObs, eqData->colList[1]->mean, eqData->colList[2]->mean, eqData->colList[7]->mean, eqData->colList[8]->mean);
	//								out_stream << "ksiObs " << ksiObs << "\tetaObs " << etaObs << "\tmagn " << eqData->colList[3]->mean <<  "\n";
									if(eqData->colList[3]->mean<0.0) eqData->colList[3]->mean = 0.1;
									vectF->int2D(ksiObs, etaObs, eqData->colList[3]->mean, &dKsi, &dEta, &ni);
	//								out_stream << "dKsi " << dKsi << "\tdEta " << dEta << "\n";
									dKsi /= 1000.0;
									dEta /= 1000.0;
									ksiObs -= dKsi;
									etaObs -= dEta;
	//								out_stream << "ksiObs " << ksiObs << "\tetaObs " << etaObs << "\n";
									getTangToRaDe1(&obsRa, &obsDec, ksiObs, etaObs, eqData->colList[7]->mean, eqData->colList[8]->mean);
	//								out_stream << "obsRa " << obsRa << "\tobsDec " << obsDec << "\n";
								}
								
								meanRa = obsRa-calcRa;
								meanDec = obsDec-calcDec;
							
							if((fabs(meanRa)>maxKsi)||(fabs(meanDec)>maxEta)) continue;
	//						qDebug() << "count= " << count << "\n";
							count = 0;
						
							
						}
						else
						{
							meanRa = eqData->colList[4]->mean;
							meanDec = eqData->colList[5]->mean;
                                                        sigmaRa = eqData->colList[4]->rmsOne;
                                                        sigmaDec = eqData->colList[5]->rmsOne;
						}
						if((fabs(meanRa)<maxKsi)&&(fabs(meanDec)<maxEta))
						{	
							
							
							for(j=1; j<deLevNum; j++)
							{
                                                                if((fabs(eqData->mmRec->Xdot)<=deLevels[j])&&(fabs(eqData->mmRec->Xdot)>deLevels[j-1]))
								{
									deValuesX[j-1] += meanRa;
									
									deNums[j-1] += 1;
									numTot++;
									break;
								}
							}
							
												
							
							for(j=1; j<deLevNum0; j++)
							{
                                                                if((fabs(eqData->mmRec->Ydot)<=deLevels0[j])&&(fabs(eqData->mmRec->Ydot)>deLevels0[j-1]))
								{
								
									deValuesY[j-1] += meanDec;
									deNums0[j-1] += 1;
									numTot0++;
									break;
								}
							}
							
						}
					}
				}
			}
			
			out_stream << "\nstat\n";
			numMin = deNums[0];
			for(i=0; i<deLevNum-1; i++) 
			{
				nums = deNums[i];
				if(numMin>nums) numMin = nums;
				if(nums)
				{
					deValuesX[i] = deValuesX[i]/(nums*1.0);
				}
				
			}
			
			for(i=0; i<deLevNum0-1; i++) 
			{
				nums = deNums0[i];
				if(numMin>nums) numMin = nums;
				if(nums)
				{
					deValuesY[i] = deValuesY[i]/(nums*1.0);
				}
				
			}
			
		
			
			resFile.setFileName(outputFolder+"/"+"resSpeedA_Ra.dat");
			resFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
			resStream.setDevice(&resFile);
			
			resFile0.setFileName(outputFolder+"/"+"resSpeedA_Dec.dat");
			resFile0.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
			resStream0.setDevice(&resFile0);
			
			resStream << "#";
			mDiap.clear();
			for(k=0;k<deLevNum;k++) mDiap << QString("%1").arg(deLevels[k]);
			resStream << mDiap.join("|");
			resStream << "\n";
			
			resStream0 << "#";
			mDiap.clear();
			for(k=0;k<deLevNum0;k++) mDiap << QString("%1").arg(deLevels0[k]);
			resStream0 << mDiap.join("|");
			resStream0 << "\n";
			
			
			
			for(i=0;i<deLevNum-1; i++)
			{
						resStream << i << "|" << deValuesX[i]  << "|" << deNums[i] << "\n";
			}
			
			for(i=0;i<deLevNum0-1; i++)
			{
						resStream0 << i << "|"  << deValuesY[i] << "|" << deNums0[i] << "\n";
			}
		}
		break;
                case 16:					//UB dlya asteroidov
                {
                        sz = eqAsts->ocList.size();
//			resRec = new residualsRec;

                        out_stream << "sz " << sz << "\n";
        /*
                        int deLevNum;
                        int deLevStep = settings->value("task08/deDiaps").toInt();
                        out_stream << "deLevStep " << deLevStep << "\n";
                        double stepDe;
          */







                        mLevNum = settings->value("task08/mLevNum").toInt();
                        mLevNumBeg = settings->value("task08/mLevNumBeg").toDouble();
                        mLevels = new double[mLevNum];

                        mLevels[0] = 0.0;
                        for(i=1; i<mLevNum-1; i++) mLevels[i] = (mLevNumBeg+i-1)*1.0;
                        mLevels[mLevNum-1] = 20.0;

                        int isW = settings->value("task16/isW", 1).toInt();
                        int isB = settings->value("task16/isB", 0).toInt();


        /*		diniKey = 1;

                        for(i=0; i<sz; i++)
                        {
                            printf("\ndet Diaps UB: %f\%\n", (double)i*100.0/(double)sz);
                                if(diniKey)
                                {
                                        naxeX[1] = naxeX[0] = eqAsts->ocList[i]->ocRa;
                                        naxeY[1] = naxeY[0] = eqAsts->ocList[i]->ocDe;
                                        naxeDe[1] = naxeDe[0] = eqAsts->ocList[i]->mag0;
                                        diniKey = 0;
                                }
                                else
                                {
                                        if(naxeX[0]>eqAsts->ocList[i]->ocRa) naxeX[0] = eqAsts->ocList[i]->ocRa;
                                        if(naxeX[1]<eqAsts->ocList[i]->ocRa) naxeX[1] = eqAsts->ocList[i]->ocRa;
                                        if(naxeY[0]>eqAsts->ocList[i]->ocDe) naxeY[0] = eqAsts->ocList[i]->ocDe;
                                        if(naxeY[1]<eqAsts->ocList[i]->ocDe) naxeY[1] = eqAsts->ocList[i]->ocDe;
                                        if(naxeDe[0]>eqAsts->ocList[i]->mag0) naxeDe[0] = eqAsts->ocList[i]->mag0;
                                        if(naxeDe[1]<eqAsts->ocList[i]->mag0) naxeDe[1] = eqAsts->ocList[i]->mag0;
                                }

                        }*/


/*			int sgn = fabs(naxeDe[0])/naxeDe[0];
                        int levMin = sgn*((int)(fabs(naxeDe[0]))+1);
                        sgn = fabs(naxeDe[1])/naxeDe[1];
                        int levMax = sgn*((int)(fabs(naxeDe[1]))+1);
                        deLevNum = ((levMax-levMin)/deLevStep)+1;

                        out_stream << "levMin " << levMin << "\n";
                        out_stream << "levMax " << levMax << "\n";
                        out_stream << "deLevNum " << deLevNum << "\n";

                        double* deLevels = new double[deLevNum];*/
                      double* mValuesX = new double[mLevNum-1];
                      double* mValuesY = new double[mLevNum-1];
                      int* mNums = new int[mLevNum-1];
                        QList <seriesOne*> magDiaps;
                        seriesOne *sOne;


                        for(i=0; i<mLevNum-1; i++)
                        {
                            mValuesX[i] = mValuesY[i] = 0.0;
                            mNums[i]=0;
                            sOne = new seriesOne;
                            magDiaps << sOne;
                         }

                        numTot = 0;

                 //       qDebug() << QString("sz eqAsts:%1\nsz ocRa0:%2\nsz ocDec").arg(i).arg(j);

                        for(i=0; i<sz; i++)
                        {
                                printf("\ndet Mean UB: %f\%\n", (double)i*100.0/(double)sz);
//
                                for(j=1; j<mLevNum; j++)
                                {
                                        if(( eqAsts->ocList[i]->mag0<=mLevels[j])&&( eqAsts->ocList[i]->mag0>mLevels[j-1]))
                                        {
                                  //          qDebug() << QString("i:%1\tj:%2\n").arg(i).arg(j);
                                            /*if(isSysCorr)
                                            {
                                                magDiaps[j-1]->colRa << ocRa1[i];
                                                magDiaps[j-1]->colDec << ocDec1[i];
                                            }
                                            else
                                            {*/
                                                magDiaps[j-1]->colRa << ocRa0[i];
                                                magDiaps[j-1]->colDec << ocDec0[i];
                                            //}
                                            magDiaps[j-1]->ocList << eqAsts->ocList[i];

                                            break;
                                        }
                                }


                        }

                        QList <double> xi2;
                        double val;
                        int pNum = settings->value("task16/pNum").toInt();

                        QFile xiFile("xi2.dat");
                        xiFile.open(QIODevice::ReadOnly| QIODevice::Text);
                        QTextStream xiStr(&xiFile);
                        QStringList xiLine;

                        while(!xiStr.atEnd())
                        {
                            xiLine = xiStr.readLine().split("\t");
                            xi2 << xiLine.at(pNum).toDouble();
                        }
                        xiFile.close();

                        double summ0, summ1;
                        int k0, k1;

                        for(i=0; i<mLevNum-1; i++)
                        {


                            k0=magDiaps[i]->ocList.size();
                            out_stream << QString("\n\nmagn diap: %1\tk0: %2\n\n").arg(i).arg(k0);
                            if(isB) if(bartLet(magDiaps[i], xi2)) continue;
                            k1=magDiaps[i]->ocList.size();
                            out_stream << QString("\n\nmagn diap: %1\tk0: %2\tk1: %3\n\n").arg(i).arg(k0).arg(k1);
                            summ0 = summ1 = 0.0;
                            for(j=0;j<k1;j++)
                            {
                                if(isW)
                                {
                                    mValuesX[i] += magDiaps[i]->ocList.at(j)->ocRa*pow(magDiaps[i]->colRa.at(j)->rmsMean, 2.0);
                                    mValuesY[i] += magDiaps[i]->ocList.at(j)->ocDe*pow(magDiaps[i]->colDec.at(j)->rmsMean, 2.0);
                                    summ0 += pow(magDiaps[i]->colRa.at(j)->rmsMean, 2.0);
                                    summ1 += pow(magDiaps[i]->colDec.at(j)->rmsMean, 2.0);
                                }
                                else
                                {
                                    mValuesX[i] += magDiaps[i]->ocList.at(j)->ocRa;
                                    mValuesY[i] += magDiaps[i]->ocList.at(j)->ocDe;
                                    summ0++;
                                    summ1++;
                                }

                            }
                            mValuesX[i] /= summ0;
                            mValuesY[i] /= summ1;
                            mNums[i] = k1;
                        }
                        if((tstr.lastIndexOf("/..")!=tstr.lastIndexOf("/"))&&(tstr.lastIndexOf("/.")!=tstr.lastIndexOf("/"))&&(tstr.lastIndexOf("_eq.txt")==(tstr.size()-7))) flist << tstr;

                        out_stream << "\nstat\n";
                        numMin = mNums[0];

                        sz = eqAsts->ocList.size();
                        if(isSysCorr)
                        {
                            errFile0.setFileName("./errCorRA.dat");
                            errFile0.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
                            errDataStream0.setDevice(&errFile0);
                            errFile1.setFileName("./errCorDEC.dat");
                            errFile1.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
                            errDataStream1.setDevice(&errFile1);


                        }
                        else
                        {
                            errFile0.setFileName("./errRA.dat");
                            errFile0.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
                            errDataStream0.setDevice(&errFile0);
                            errFile1.setFileName("./errDEC.dat");
                            errFile1.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
                            errDataStream1.setDevice(&errFile1);
                        }

                        for(i=0; i<mLevNum-1; i++)
                        {
                            k1=magDiaps[i]->ocList.size();
                            for(j=0;j<k1;j++)
                            {
                                magDiaps[i]->colRa.at(j)->rec2s(&tstr);
                                errDataStream0 << tstr << "\n";
                                magDiaps[i]->colDec.at(j)->rec2s(&tstr);
                                errDataStream1 << tstr << "\n";

                            }
                        }


                        errFile0.close();
                        errFile1.close();

                        if(isSysCorr)
                        {
                            detMeanCols("./errCorRA.dat");
                            detMeanCols("./errCorDEC.dat");
                        }
                        else
                        {
                            detMeanCols("./errRA.dat");
                            detMeanCols("./errDEC.dat");
                        }


                        resFile.setFileName(outputFolder+"/"+"resUBA.dat");
                        resFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
                        resStream.setDevice(&resFile);

                        resStream << "#";
                        mDiap.clear();
                        for(k=0;k<mLevNum;k++) mDiap << QString("%1").arg(mLevels[k]);
                        resStream << mDiap.join("|");
                        resStream << "\n";



                        for(i=0;i<mLevNum-1; i++)
                        {
//                                                resStream << i << "|" << mValuesX[i] << "|" << mValuesY[i] << "|" << mNums[i] << "\n";
                            resStream << QString("%1|%2|%3|%4|%5\n").arg(i, 2).arg(mValuesX[i], 9, 'f', 4).arg(mValuesY[i], 9, 'f', 4).arg(sqrt(mValuesX[i]*mValuesX[i] + mValuesY[i]*mValuesY[i]), 9, 'f', 4).arg(mNums[i]);
                        }
                        eqAsts->saveAs("total_eq.txt");
                }
                break;
		default:
		break;
	}

	
	printf("end\n");
//	getch();
	return 0; 
}
	
