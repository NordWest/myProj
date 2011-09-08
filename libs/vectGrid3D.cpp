#include "vectGrid3D.h"

vectGrid3D::vectGrid3D()
{
	xVectField = NULL;
	yVectField = NULL;
	vectNumField = NULL;
	
	axnum = -1;
	ax = NULL;
	pos = NULL;
        numTot = 0;
	
	coefX.setbounds(0, 15);
	coefY.setbounds(0, 15);
	coefN.setbounds(0, 15);
}

vectGrid3D::~vectGrid3D()
{
	xVectField = NULL;
	yVectField = NULL;
	vectNumField = NULL;
	
	axnum = -1;
	ax = NULL;
	pos = NULL;
        numTot = 0;
}

vectGrid3D::vectGrid3D(char* fname)
{
	xVectField = NULL;
	yVectField = NULL;
	vectNumField = NULL;
	
	axnum = -1;
	ax = NULL;
	pos = NULL;
        numTot = 0;
	
	coefX.setbounds(0, 15);
	coefY.setbounds(0, 15);
	coefN.setbounds(0, 15);
	
	init(fname);
}

int vectGrid3D::init(char* fname)
{
	QString fName(fname);
	QFile dataFile;
	dataFile.setFileName(fName);
	dataFile.open(QIODevice::ReadOnly| QIODevice::Text);
	if(!dataFile.isOpen())
	{
		printf("\nfile is not open\n");
		return 2;
	}
	QTextStream dataStream;
	dataStream.setDevice(&dataFile);
	
	double *dimN;
	QString dataLine;
	QString tStr;
	QStringList operands;
	int ikey;
	int i, j, k, sz, cpos;
	long nums;
	double ocX, ocY;
	
	ikey=0;
	
	while(!dataStream.atEnd())
	{
		dataLine = dataStream.readLine();
		
		if(dataLine.at(0)=='#')
		{
			
			tStr.clear();
			tStr.insert(0, dataLine.mid(1));
			qDebug() << "\n" << tStr << "\n";
			operands.clear();
			operands = tStr.split("|");
			sz = operands.size();
			qDebug() << "\nsz= " << sz << "\n";
			dimN = new double[sz];
			dNums << sz;
			for(i=0; i<sz; i++)
			{
				dimN[i] = operands[i].toDouble();
			}
			dims << dimN;
		}
		else
		{
			if(!ikey)
			{
				axnum = dims.size();
//				qDebug() << "axnum= " << axnum << "\n";
				ax = new int[axnum];
				pos = new int[axnum];
				for(i=0; i<axnum; i++)
				{
					ax[i] = dNums[i]-1;
				}
				xVectField = new multidim(DOUBLE_TYPE, ax, axnum);
				yVectField = new multidim(DOUBLE_TYPE, ax, axnum);
				vectNumField = new multidim(LONG_TYPE, ax, axnum);
				xVectField->clear();
				yVectField->clear();
				vectNumField->clear();
				ikey=1;
			}
			
			operands.clear();
			operands = dataLine.split("|");
			
			for(i=0; i<axnum; i++)
			{
				pos[i] = operands[i].toInt();
			}
			ocX = operands[axnum].toDouble();
			ocY = operands[axnum+1].toDouble();
			nums = operands[axnum+2].toLong();
			xVectField->set(ocX, pos);
			yVectField->set(ocY, pos);
			vectNumField->set((long)nums, pos);
		}
	}
	
	qDebug() << dNums[0] << ":" << dNums[1] << "\n";
	
	xarr.setbounds(0, dNums[0]-1);
	yarr.setbounds(0, dNums[1]-1);
	
	for(i=1; i<ax[0]+1; i++) xarr(i-1) = (dims[0][i]+dims[0][i-1])/2.0;
	for(i=1; i<ax[1]+1; i++) yarr(i-1) = (dims[1][i]+dims[1][i-1])/2.0;
	
	fX.setbounds(0, ax[0]-1, 0, ax[1]-1);
	fY.setbounds(0, ax[0]-1, 0, ax[1]-1);
	fN.setbounds(0, ax[0]-1, 0, ax[1]-1);
/*	
	ap::real_1d_array coefXT;
	ap::real_1d_array coefYT;
	ap::real_1d_array coefNT;
*/	
	for(k=0; k<ax[2]; k++)
	{
		pos[2] = k;
	
		for(j=0; j<ax[1]; j++)
		{
			pos[1] = j;
			for(i=0; i<ax[0]; i++)
			{
				pos[0] = i;
				cpos = xVectField->detPos(pos);
				fX(i, j) = xVectField->getD(cpos);
				cpos = yVectField->detPos(pos);
				fY(i, j) = yVectField->getD(cpos);
				cpos = vectNumField->detPos(pos);
				fN(i, j) = vectNumField->getL(cpos);
//				outStream << fX(i, j) << "\t";
			}
//			outStream << "\n";
		}
		qDebug() << k << "\n";

		buildbicubicspline(xarr, yarr, fX, ax[0], ax[1], coefX);
		buildbicubicspline(xarr, yarr, fY, ax[0], ax[1], coefY);
		buildbicubicspline(xarr, yarr, fN, ax[0], ax[1], coefN);
		
/*
		bicubicresamplecartesian(fX, ax[0], ax[1], fXI, axI[0], axI[1]);
		bicubicresamplecartesian(fY, ax[0], ax[1], fYI, axI[0], axI[1]);
		bicubicresamplecartesian(fN, ax[0], ax[1], fNI, axI[0], axI[1]);
*/
/*
		bilinearresamplecartesian(fX, ax[0], ax[1], fXI, axI[0], axI[1]);
		bilinearresamplecartesian(fY, ax[0], ax[1], fYI, axI[0], axI[1]);
		bilinearresamplecartesian(fN, ax[0], ax[1], fNI, axI[0], axI[1]);
*/
//		bilinearresample(fX, ax[0], ax[1], fXI, axI[0], axI[1]);
		
//		double splineinterpolation2d(const ap::real_1d_array& c, double x, double y);
		
		ap::real_1d_array coefXT(coefX);
		ap::real_1d_array coefYT(coefY);
		ap::real_1d_array coefNT(coefN);
		
		coefXM << coefXT;
		coefYM << coefYT;
		coefNM << coefNT;
	}

        return 0;
}

int vectGrid3D::initAxes(int *axes, int axesNum)
{
//init
    axnum = axesNum;					//Р§РёСЃР»Рѕ РѕСЃРµР№
    for(int i=0; i<axnum; i++) ax[i] = axes[i];

    xVectField = new multidim(DOUBLE_TYPE, ax, axnum);
    yVectField = new multidim(DOUBLE_TYPE, ax, axnum);
    vectNumField = new multidim(LONG_TYPE, ax, axnum);
    xVectField->clear();
    yVectField->clear();
    vectNumField->clear();

    return 0;
}

int vectGrid3D::setLevels(double *xLevels, int xLevNum, double *yLevels, int yLevNum, double *mLevels, int mLevNum)
{
    int i, j;
    axnum = 3;
    ax = new int[axnum];
    pos = new int[axnum];
    ax[0] = xLevNum - 1;
    ax[1] = yLevNum - 1;
    ax[2] = mLevNum - 1;

    //size = ax[0]*ax[1]*ax[2];

    initAxes(ax, axnum);

    double *vect;

    dNums << xLevNum << yLevNum << mLevNum;

//    qDebug() <<

    vect = new double[dNums[0]];
    for(i=0; i<dNums[0]; i++) vect[i] = xLevels[i];
    dims << vect;
    vect = new double[dNums[1]];
    for(i=0; i<dNums[1]; i++) vect[i] = yLevels[i];
    dims << vect;
    vect = new double[dNums[2]];
    for(i=0; i<dNums[2]; i++) vect[i] = mLevels[i];
    dims << vect;


    for(i=0; i<axnum; i++)
    {
        pos[i] = -1;
        //qDebug() << QString("dNums[i]: %1\n").arg(dNums.at(i));
        qDebug() << QString("ax: %1\ndims: ").arg(i);
        for(j=0; j<dNums.at(i); j++)
        {
            qDebug() << QString("%1\t").arg(dims.at(i)[j]);
        }
        qDebug() << QString("\n");
    }


    return 0;
}

int vectGrid3D::initLevels(QList <double*> dims0, QList <int> dNums0)
{
    int i, j;
    double *vect;
    dNums << dNums0;//.toVector();
    for(i=0; i<dNums.size(); i++)
    {
       vect = new double[dNums.at(i)];
       for(j=0; j<dNums.at(i); j++)
       {
           vect[j] = dims0.at(i)[j];
       }
       dims << vect;
    }
    axnum = dims.size();
//				qDebug() << "axnum= " << axnum << "\n";
    ax = new int[axnum];
    pos = new int[axnum];
    //size = 1;
    for(i=0; i<axnum; i++)
    {
            ax[i] = dNums[i]-1;
            //size += ax[i];
    }
    xVectField = new multidim(DOUBLE_TYPE, ax, axnum);
    yVectField = new multidim(DOUBLE_TYPE, ax, axnum);
    vectNumField = new multidim(LONG_TYPE, ax, axnum);
    xVectField->clear();
    yVectField->clear();
    vectNumField->clear();
//    ikey=1;

    return 0;
}

void vectGrid3D::addPoint(double *vect, double x, double y)
{
    int i, j;
    long cpos;

    //qDebug() << "setPoint\n";

    for(i=0; i<axnum; i++)
    {
        pos[i] = -1;
        //qDebug() << QString("dNums[i]: %1\n").arg(dNums.at(i));
        for(j=1; j<dNums.at(i); j++)
        {
 //           qDebug() << QString("vect= %1\t d1= %2\t d0= %3\n").arg(vect[i]).arg(dims.at(i)[j]).arg(dims.at(i)[j-1]);
                if((vect[i]<=dims.at(i)[j])&&(vect[i]>dims.at(i)[j-1]))
                {
                        pos[i] = j-1;
                        break;
                }
        }
        //qDebug() << QString("pos[%1]= %2\n").arg(i).arg(pos[i]);
        if(pos[i]==-1)
        {
  //          qDebug() << "-1\n";
            return;
        }
    }

    //qDebug() << QString("pos: %1 %2 %3 | %4\t%5\n").arg(pos[0]).arg(pos[1]).arg(pos[2]).arg(x).arg(y);
     /*   pos[1] = -1;
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
        }*/
    //					out_stream << "\n" << pos[0] << ":" << pos[1] << ":" << pos[2] << "\t" << resRec->etaOC << ":" << resRec->ksiOC << "\n";



                numTot++;
                cpos = xVectField->detPos(pos);
                xVectField->dD[cpos] += x;
                cpos = yVectField->detPos(pos);
                yVectField->dD[cpos] += y;
                cpos = vectNumField->detPos(pos);
                vectNumField->lD[cpos] = vectNumField->lD[cpos] + 1;
    /*						xVectField->set(resRec->etaOC, pos);
                yVectField->set(resRec->ksiOC, pos);
                vectNumField->get(&nums, pos);
                vectNumField->set((long)(nums+1), pos);*/


}

void vectGrid3D::setPoint(double *vect, double x, double y, long num)
{
    int i, j;
    long cpos;

    //qDebug() << "setPoint\n";
    //qDebug() << QString("vect: %1\t%2\t%3\n").arg(vect[0]).arg(vect[1]).arg(vect[2]);

    for(i=0; i<axnum; i++)
    {
        pos[i] = -1;
        //qDebug() << QString("dNums[i]: %1\n").arg(dNums.at(i));
        for(j=1; j<=dNums.at(i); j++)
        {
            //qDebug() << QString("vect= %1\t d1= %2\t d0= %3\n").arg(vect[i]).arg(dims.at(i)[j]).arg(dims.at(i)[j-1]);
                if((vect[i]<=dims.at(i)[j])&&(vect[i]>dims.at(i)[j-1]))
                {
                        pos[i] = j-1;
                        break;
                }
        }
    //    qDebug() << QString("pos[%1]= %2\n").arg(i).arg(pos[i]);
        if(pos[i]==-1) return;
    }

    //qDebug() << QString("pos: %1 %2 %3\n").arg(pos[0]).arg(pos[1]).arg(pos[2]);
     /*   pos[1] = -1;
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
        }*/
    //					out_stream << "\n" << pos[0] << ":" << pos[1] << ":" << pos[2] << "\t" << resRec->etaOC << ":" << resRec->ksiOC << "\n";



                numTot++;
                cpos = xVectField->detPos(pos);
          //      qDebug() << QString("x cpos: %1\n").arg(cpos);
                xVectField->dD[cpos] = x;
                cpos = yVectField->detPos(pos);
           //     qDebug() << QString("y cpos: %1\n").arg(cpos);
                yVectField->dD[cpos] = y;
                cpos = vectNumField->detPos(pos);
           //     qDebug() << QString("m cpos: %1\n").arg(cpos);
                vectNumField->lD[cpos] = num;
    /*						xVectField->set(resRec->etaOC, pos);
                yVectField->set(resRec->ksiOC, pos);
                vectNumField->get(&nums, pos);
                vectNumField->set((long)(nums+1), pos);*/


}

void vectGrid3D::detMean()
{
    qDebug() << QString("detMean\n");
    long i;
    long cpos;
    cpos = xVectField->getSize();
    for(i=0; i<cpos; i++)
    {
        //qDebug() << QString("%1: %2\t%3\t%4\n").arg(i).arg(xVectField->dD[i]).arg(yVectField->dD[i]).arg(vectNumField->lD[i]);
        if(vectNumField->lD[i]!=0)
        {
            xVectField->dD[i] /= 1.0*vectNumField->lD[i];
            yVectField->dD[i] /= 1.0*vectNumField->lD[i];
        }
        else
        {
            xVectField->dD[i] = 0;
            yVectField->dD[i] = 0;
        }

        //qDebug() << QString("%1: %2\t%3\t%4\n").arg(i).arg(xVectField->dD[i]).arg(yVectField->dD[i]).arg(vectNumField->lD[i]);
    }
}

void vectGrid3D::doWeght()
{
    qDebug() << QString("doWeght\n");
    long i, j, k;
    long cpos;
//    cpos = xVectField->getSize();
 /*   for(i=0; i<cpos; i++)
    {
        //qDebug() << QString("%1: %2\t%3\t%4\n").arg(i).arg(xVectField->dD[i]).arg(yVectField->dD[i]).arg(vectNumField->lD[i]);
        if(vectNumField->lD[i]!=0)
        {
            xVectField->dD[i] /= 1.0*vectNumField->lD[i];
            yVectField->dD[i] /= 1.0*vectNumField->lD[i];
        }
        else
        {
            xVectField->dD[i] = 0;
            yVectField->dD[i] = 0;
        }

        //qDebug() << QString("%1: %2\t%3\t%4\n").arg(i).arg(xVectField->dD[i]).arg(yVectField->dD[i]).arg(vectNumField->lD[i]);
    }*/
    long summK, Ni;
    double mult;
    for(k=0; k<ax[2]; k++)
    {
            pos[2] = k;
            qDebug() << k << "\n";

            summK = 0;
            for(j=0; j<ax[1]; j++)
            {
                    pos[1] = j;
                    for(i=0; i<ax[0]; i++)
                    {
                            pos[0] = i;
                            cpos = vectNumField->detPos(pos);
                            qDebug() << QString("%1:%2:%3\n").arg(i).arg(j).arg(cpos);
                            summK += vectNumField->getL(cpos);
//				outStream << fX(i, j) << "\t";
                    }
//			outStream << "\n";
            }

            qDebug() << QString("summK= %1\n").arg(summK);

            for(j=0; j<ax[1]; j++)
            {
                    pos[1] = j;
                    for(i=0; i<ax[0]; i++)
                    {
                            pos[0] = i;

                            cpos = vectNumField->detPos(pos);
                            qDebug() << QString("%1:%2:%3\n").arg(i).arg(j).arg(cpos);
                            Ni = vectNumField->getL(cpos);
                            qDebug() << QString("Ni= %1\n").arg(Ni);
                            mult = Ni*1.0/(1.0*summK);
                            qDebug() << QString("mult= %1\n").arg(mult);
                            cpos = xVectField->detPos(pos);
                            xVectField->dD[cpos] *= mult;
                            cpos = yVectField->detPos(pos);
                            yVectField->dD[cpos] *= mult;
                            cpos = vectNumField->detPos(pos);
                            vectNumField->lD[cpos] = summK/(1.0*ax[0]*ax[1]);
                            qDebug() << QString("lD[cpos]= %1\n").arg(vectNumField->lD[cpos]);
//				outStream << fX(i, j) << "\t";
                    }
//			outStream << "\n";
            }
        }
}

void vectGrid3D::clearDims()
{
    xVectField->clear();
    yVectField->clear();
    vectNumField->clear();
}

void vectGrid3D::initVF()
{
    int i, k, j;
    long cpos;

    xarr.setbounds(0, dNums[0]-1);
    yarr.setbounds(0, dNums[1]-1);

    for(i=1; i<ax[0]+1; i++) xarr(i-1) = (dims[0][i]+dims[0][i-1])/2.0;
    for(i=1; i<ax[1]+1; i++) yarr(i-1) = (dims[1][i]+dims[1][i-1])/2.0;

    fX.setbounds(0, ax[0]-1, 0, ax[1]-1);
    fY.setbounds(0, ax[0]-1, 0, ax[1]-1);
    fN.setbounds(0, ax[0]-1, 0, ax[1]-1);
/*
    ap::real_1d_array coefXT;
    ap::real_1d_array coefYT;
    ap::real_1d_array coefNT;
*/
    qDebug() << "ax[2]= " << ax[2] << "\n";
    for(k=0; k<ax[2]; k++)
    {
            pos[2] = k;

            for(j=0; j<ax[1]; j++)
            {
                    pos[1] = j;
                    for(i=0; i<ax[0]; i++)
                    {
                            pos[0] = i;
                            cpos = xVectField->detPos(pos);
                            fX(i, j) = xVectField->getD(cpos);
                            cpos = yVectField->detPos(pos);
                            fY(i, j) = yVectField->getD(cpos);
                            cpos = vectNumField->detPos(pos);
                            fN(i, j) = vectNumField->getL(cpos);
//				outStream << fX(i, j) << "\t";
                    }
//			outStream << "\n";
            }
            qDebug() << k << "\n";

            buildbicubicspline(xarr, yarr, fX, ax[0], ax[1], coefX);
            buildbicubicspline(xarr, yarr, fY, ax[0], ax[1], coefY);
            buildbicubicspline(xarr, yarr, fN, ax[0], ax[1], coefN);

/*
            bicubicresamplecartesian(fX, ax[0], ax[1], fXI, axI[0], axI[1]);
            bicubicresamplecartesian(fY, ax[0], ax[1], fYI, axI[0], axI[1]);
            bicubicresamplecartesian(fN, ax[0], ax[1], fNI, axI[0], axI[1]);
*/
/*
            bilinearresamplecartesian(fX, ax[0], ax[1], fXI, axI[0], axI[1]);
            bilinearresamplecartesian(fY, ax[0], ax[1], fYI, axI[0], axI[1]);
            bilinearresamplecartesian(fN, ax[0], ax[1], fNI, axI[0], axI[1]);
*/
//		bilinearresample(fX, ax[0], ax[1], fXI, axI[0], axI[1]);

//		double splineinterpolation2d(const ap::real_1d_array& c, double x, double y);

            ap::real_1d_array coefXT(coefX);
            ap::real_1d_array coefYT(coefY);
            ap::real_1d_array coefNT(coefN);

            coefXM << coefXT;
            coefYM << coefYT;
            coefNM << coefNT;
    }


}

int vectGrid3D::printCoefs()
{
	int i, j, sz;
	sz = coefXM.size();
	printf("coefXM.size()= %d\n", sz);
	for(i=0; i<sz; i++)
	{
		ap::real_1d_array coefTemp(coefXM.at(i));
		for(j=0; j<16; j++)
		{
			qDebug() << coefTemp(j) << "|";
		}
		qDebug() << "\n";
	}
}

int vectGrid3D::int2D(double x, double y, double m, double *xint, double *yint, long *nint)
{
	int i, j, k;
	
	k=-1;
	for(i=0; i<ax[2]; i++)
	{
		if((m<dims[2][i+1])&&(m>=dims[2][i])){k=i; break;} 
	}
//	qDebug() << "k=" << k  << "\tx= " << x << "\ty= " << y << "\tm= " << m << "\n";
	if(k==-1) return 1;
	ap::real_1d_array coefTempX(coefXM.at(k));
	*xint = splineinterpolation2d(coefTempX, x, y);
	ap::real_1d_array coefTempY(coefYM.at(k));
	*yint = splineinterpolation2d(coefTempY, x, y);
	ap::real_1d_array coefTempN(coefNM.at(k));
	*nint = (long)splineinterpolation2d(coefTempN, x, y);
	
	return 0;
}

int vectGrid3D::int2Drad(double x, double y, double m, double *xint, double *yint, double rMax, double nmin)
{
    //qDebug() << "\nint2Drad\n";
        int i, j, k;
        double px, py, pm, ocX, ocY, r;
        long nums;

        k=-1;
        for(i=0; i<ax[2]; i++)
        {
                if((m<dims[2][i+1])&&(m>=dims[2][i])){k=i; break;}
        }
        //qDebug() << "k=" << k  << "\tx= " << x << "\ty= " << y << "\tm= " << m << "\n";
        if(k==-1) return 1;

        //qDebug() << QString("rMax= %1\tnmin= %2\n").arg(rMax).arg(nmin);

        double dx, dy;
        int nTot;
        nTot = 0;
        dx = dy = 0;
        double nC;

        for(i=0; i<ax[0]; i++)
        {
            for(j=0; j<ax[1]; j++)
            {
                pos[0] = i;
                pos[1] = j;
                pos[2] = k;
                getVect(i, j, k, &px, &py, &pm, &ocX, &ocY, &nums);
                r = sqrt(pow(px-x, 2.0) + pow(py-y, 2.0));
                //qDebug() << QString("r= %1\tnums= %2\n").arg(r).arg(nums);
                nC = 1.0;

                if(r<rMax&&(nums>=nmin))
                {
                    //if(nums<nmin) nC = (1 + log(nums/(1.0*nmin)));
                    dx += ocX*(1.0-(r/rMax))*nC;
                    dy += ocY*(1.0-(r/rMax))*nC;
                    nTot++;
                }
            }
        }

        //qDebug() << QString("nTot= %1\n").arg(nTot);
        if(nTot==0) return 1;
        *xint = dx/(nTot*1.0);
        *yint = dy/(nTot*1.0);

        //qDebug() << QString("dx= %1\tdy= %2\n").arg(dx).arg(dy);
        //qDebug() << QString("x= %1\txint= %2\n").arg(x).arg(*xint);
        //qDebug() << QString("y= %1\tyint= %2\n").arg(y).arg(*yint);


        return 0;
}

int vectGrid3D::int2DradM(double x, double y, double m, double *xint, double *yint, double rMax, double nmin)
{
    //qDebug() << "\nint2Drad\n";
        int i, j, k;
        double px, py, pm, ocX, ocY, r;
        long nums;
/*
        k=-1;
        for(i=0; i<ax[2]; i++)
        {
                if((m<dims[2][i+1])&&(m>=dims[2][i])){k=i; break;}
        }
        //qDebug() << "k=" << k  << "\tx= " << x << "\ty= " << y << "\tm= " << m << "\n";
        if(k==-1) return 1;
*/
        //qDebug() << QString("rMax= %1\tnmin= %2\n").arg(rMax).arg(nmin);

        double dx, dy;
        int nTot;
        nTot = 0;
        dx = dy = 0;
        double nC;
        //double xint, yint;
        double *xVect, *yVect, *mVect;
        int mnum = ax[2];


        xVect = new double[mnum];
        yVect = new double[mnum];
        mVect = new double[mnum];


        for(k=0; k<mnum; k++)
        {




                xVect[k] = 0.0;
                yVect[k] = 0.0;

                dx = 0.0;
                dy = 0.0;
                nTot = 0;

                for(i=0; i<ax[0]; i++)
                {
                    for(j=0; j<ax[1]; j++)
                    {
                        pos[0] = i;
                        pos[1] = j;
                        pos[2] = k;
                        getVect(i, j, k, &px, &py, &pm, &ocX, &ocY, &nums);
                        r = sqrt(pow(px-x, 2.0) + pow(py-y, 2.0));
                        //qDebug() << QString("r= %1\tnums= %2\n").arg(r).arg(nums);
                        nC = 1.0;

                        if(r<rMax&&(nums>=nmin))
                        {
                            //if(nums<nmin) nC = (1 + log(nums/(1.0*nmin)));
                            dx += ocX*(1.0-(r/rMax))*nC;
                            dy += ocY*(1.0-(r/rMax))*nC;
                            nTot++;
                        }
                    }
                }

                if(nTot==0)
                {
                    xVect[k] = 0.0;
                    yVect[k] = 0.0;
                }
                else
                {
                    xVect[k] = dx/(nTot*1.0);
                    yVect[k] = dy/(nTot*1.0);
                }

                mVect[k] = (dims[2][k+1]+dims[2][k])/2.0;
                //qDebug() << QString("vect: %1\t%2\t%3\n").arg(xVect[k]).arg(yVect[k]).arg(mVect[k]);

        }

        double *Cx, *Cy;
        double *Zx, *Zy;
        Cx = new double[2*mnum];
        Cy = new double[2*mnum];
        Zx = new double[2];
        Zy = new double[2];

        for(i=0; i<mnum; i++)
        {
            //Cx[i*mnum] = mVect[i]*mVect[i];
            Cx[i*mnum] = mVect[i];
            Cx[i*mnum+1] = 1.0;

            //Cy[i*mnum] = mVect[i]*mVect[i];
            Cy[i*mnum] = mVect[i];
            Cy[i*mnum+1] = 1.0;

            //qDebug() << QString("x: %1 = Ax*%2 + Bx*%3\n").arg(xVect[i]).arg(Cx[i*mnum]).arg(Cx[i*mnum+1]);
            //qDebug() << QString("y: %1 = Ay*%2 + By*%3\n").arg(yVect[i]).arg(Cy[i*mnum]).arg(Cy[i*mnum+1]);
        }

        slsm(2, mnum, Zx, Cx, xVect);
        //qDebug() << QString("Zx: %1\t%2\t%3\n").arg(Zx[0]).arg(Zx[1]).arg(Zx[2]);
        slsm(2, mnum, Zy, Cy, yVect);
        //qDebug() << QString("Zy: %1\t%2\t%3\n").arg(Zy[0]).arg(Zy[1]).arg(Zy[2]);

        *xint = Zx[0]*m + Zx[1];
        *yint = Zy[0]*m + Zy[1];

        //qDebug() << QString("xint= %1\tyint= %2\n").arg(*xint).arg(*yint);



        //qDebug() << QString("nTot= %1\n").arg(nTot);
        //if(nTot==0) return 1;

        //qDebug() << QString("dx= %1\tdy= %2\n").arg(dx).arg(dy);
        //qDebug() << QString("x= %1\txint= %2\n").arg(x).arg(*xint);
        //qDebug() << QString("y= %1\tyint= %2\n").arg(y).arg(*yint);


        delete [] Cx;
        delete [] Cy;
        delete [] Zx;
        delete [] Zy;
        delete [] xVect;
        delete [] yVect;
        delete [] mVect;



        return 0;
}


int vectGrid3D::saveVF(QString fName)
{
    int i, j, k;
    QStringList xDiap;
    QFile resFile;
    QTextStream resStream;
    double ocX, ocY;
    long nums;

    resFile.setFileName(fName);
    resFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
    resStream.setDevice(&resFile);

    for(i=0; i<axnum; i++)
    {
        resStream << "#";
        xDiap.clear();

        for(k=0;k<dNums[i];k++)
        {
                 xDiap << QString("%1").arg(dims.at(i)[k], 12, 'g');
        }
        resStream << xDiap.join("|");
        resStream << "\n";
    }
 /*   resStream << "#";

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

   */

    for(i=0;i<ax[2]; i++)
    {
            for(j=0;j<ax[1];j++)
            {
                    for(k=0;k<ax[0];k++)
                    {
                            pos[0] = k;
                            pos[1] = j;
                            pos[2] = i;
                            ocX = xVectField->getD(xVectField->detPos(pos));
                            ocY = yVectField->getD(yVectField->detPos(pos));
//						yVectField->get(&ocY, pos);
//						vectNumField->get(&nums, pos);
                            nums = vectNumField->getL(vectNumField->detPos(pos));
                            resStream << QString("%1").arg(k, 5, 10, QLatin1Char(' ')) << "|" << QString("%1").arg(j, 5, 10, QLatin1Char(' ')) << "|" << QString("%1").arg(i, 5, 10, QLatin1Char(' ')) << "|" << QString("%1").arg(ocX, 12, 'g', 6, QLatin1Char(' ')) << "|" << QString("%1").arg(ocY, 12, 'g', 6, QLatin1Char(' ')) << "|" << QString("%1").arg(nums, 8, 10, QLatin1Char(' ')) << "\n";
                    }
            }
    }
    resFile.close();
}

int vectGrid3D::saveDotList(QString resDir, QString colSep, QString prefStr, double mult, double nmin)
{
    int i, j, k;
    //QStringList xDiap;
    QFile resFile;
    QTextStream resStream;
    double ocX, ocY, m;
    double px, py, pm;
    long nums;
    QString fName;



    QStringList resList;

    for(i=0;i<ax[2]; i++)
    {
        fName.clear();
        fName = QString("%1/%2%3.txt").arg(resDir).arg(prefStr).arg((dims[2][i]+dims[2][i+1])/2.0, 6, 'f', 2, QLatin1Char('0'));
        resFile.setFileName(fName);
        resFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
        resStream.setDevice(&resFile);
            for(j=0;j<ax[1];j++)
            {
                    for(k=0;k<ax[0];k++)
                    {
                            pos[0] = k;
                            pos[1] = j;
                            pos[2] = i;
                            getVect(k, j, i, &px, &py, &pm, &ocX, &ocY, &nums);
                            if(nums<nmin) continue;
                            resList.clear();
                            resList << QString("%1").arg(px);
                            resList << QString("%1").arg(py);
                            resList << QString("%1").arg(ocX, 12, 'g');
                            resList << QString("%1").arg(ocY, 12, 'g');
                            resList << QString("%1").arg(px+ocX*mult);
                            resList << QString("%1").arg(py+ocY*mult);
                            resList << QString("%1").arg(nums);
                            resStream << QString("%1\n").arg(resList.join(colSep));
                    }
            }
        resFile.close();
    }

}

long vectGrid3D::getNumMax()
{
    int i, j, k;
    double ocX, ocY, m;
    double px, py, pm;
    long nums, numMax;
    numMax = 0;
    for(i=0;i<ax[2]; i++)
    {
        for(j=0;j<ax[1];j++)
        {
                for(k=0;k<ax[0];k++)
                {
                    getVect(k, j, i, &px, &py, &pm, &ocX, &ocY, &nums);
                    if(nums>numMax) numMax = nums;
                }
        }
    }
    return numMax;
}

void vectGrid3D::getVect(int i, int j, int k, double *px, double *py, double *pm, double *x, double *y, long *num)
{
    //if((i>dNums[0]-1)||(j>dNums[1]-1)||(k>dNums[2]-1)||(i<0)||(j<0)||(k<0)) return;
    *px = (dims[0][i+1]+dims[0][i])/2.0;
    *py = (dims[1][j+1]+dims[1][j])/2.0;
    *pm = (dims[2][k+1]+dims[2][k])/2.0;

    pos[0] = i;
    pos[1] = j;
    pos[2] = k;

    *x = xVectField->getD(xVectField->detPos(pos));
    *y = yVectField->getD(yVectField->detPos(pos));
    *num = vectNumField->getL(vectNumField->detPos(pos));

  //  qDebug() << QString("p: %1|%2|%3|%4|%5|%6\n").arg(*px).arg(*py).arg(*pm).arg(*x).arg(*y).arg(*num);
}
