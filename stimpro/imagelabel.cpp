#include <QtGui>
#include "imagelabel.h"
//#include "mb.hpp"


imageLabel::imageLabel(int maxIW,int maxIH, QWidget *parent):QWidget(parent)
{
	baseImg = new QImage(maxIW, maxIH, QImage::Format_Indexed8);// создаем объект baseImg (виртуальный экран или "буфер прорисовки") как индексированный 8-битный рисунок (QImage::Format_Indexed8)
	QVector<QRgb> colormap;// цветовая палитра для baseImg. То есть вместо того, чтобы иметь четыре байта на пиксель (rgba) у нас один байт на пиксель, и цвект пикселя (rgb) задается указанием индекса в массиве colormap (палитре). Это называется "индексированные цвета". Это очень увеличивает скорость прорисовки.
    for (int k=0; k<256;k++) colormap << qRgb(k,k,k);// заполняем массивчик colormap цветами - градациями серого(от черного (0) до белого (255))
	baseImg->setColorTable(colormap);//"подключаем палитру"
	setAutoFillBackground(false);// запрещаем автоматическое заполнение фоновым цветом для ускорения прорисовки
	maxImgWidth = maxIW;//устанавливаем максимальные размеры для "буфера прорисовки"
	maxImgHeight = maxIH;
	//////////////////////////
	frameIndex = 0;//Начальное значение текущего индекса равно 0
	scale = 1;// масштаб равен 1
	cX = 0;
	cY = 0;
	dX = 0;
	dY = 0;
	aperture = 20.0;

	basePen.setColor(Qt::black);
	basePen.setWidth(1);
        marksGridPen.setColor(Qt::blue);
        marksGridPen.setWidth(2);
        marksIPixPen.setColor(Qt::yellow);
        marksIPixPen.setWidth(2);
        marksRefPen.setColor(Qt::red);
        marksRefPen.setWidth(2);
        marksObjPen.setColor(Qt::white);
        marksObjPen.setWidth(2);
	setMouseTracking(true);
}
/*
void imageLabel::mouseMoveEvent(QMouseEvent *event)
{
	qDebug() << "\nimgLabel->MouseMove\n";
}
*/
void imageLabel::setColors(QColor bgCol, QColor ipixCol, QColor catCol, QColor refCol, QColor objCol)
{
    //bgCol.setRgb(bgCol.toRgb());
    basePen.setColor(bgCol);
    basePen.setWidth(1);
    marksGridPen.setColor(catCol);
    marksGridPen.setWidth(2);
    marksIPixPen.setColor(ipixCol);
    marksIPixPen.setWidth(2);
    marksRefPen.setColor(refCol);
    marksRefPen.setWidth(2);
    marksObjPen.setColor(objCol);
    marksObjPen.setWidth(2);
    setMouseTracking(true);
}

void imageLabel::paintEvent(QPaintEvent *event)//событие прорисовки
{
	QPainter painter(this);// объект QPainter для работы с изображением (копирования с виртуального экрана на реальный (imagelabel)). Использование this как параметра инициализации дает возможность рисовать на реальном экране.
	updateBaseImage(cX,cY,&painter);//выполняем прорисовку, передав указатель на объект QPainter
};

void imageLabel::resizeEvent(QResizeEvent *event)//событие изменение размеров imagelabel
{
	QPainter painter(this);
	updateBaseImage(cX,cY,&painter);
};

void imageLabel::updateBaseImage(int xc, int yc, QPainter *painter)
{
	qDebug() << "\nupdateBaseImage\n";
	qDebug() << "\nxc " << xc << "\tyc " << yc << "\n";
	marksP *xG;
	baseImg->fill(0);
        int xShift, yShift;
        xShift = 0;
        yShift = 0;
	if(fitsdataList.count()>0)//если есть открытые файлы, то рисуем
	{
		fitsdata *fitsd = fitsdataList[frameIndex];// получаем указатель на текущий ПЗС-кадр
		uchar* imgdata = baseImg->bits();// получаем указатель на буфер "виртуального экрана". Если сюда писать данные, то они будут восприниматься как виртуальное изображение, которое хранится в памяти и не показывается на экране
                marksGrid *mGr;


        ///////////////////////////////////////////////
		int Xo = (xc-width()/2)/scale;//определяем левый верхний угол прямоугольника в пространстве пиксельных координат реального ПЗС-кадра, который надо отобразить на виртуальном, а потом реальном экране
		int Yo = (yc-height()/2)/scale;
		int w,h;//переменные для хранения размеров прямоугольника изображения
		if(scale<1)//масштаб меньше 1 - уменьшение
		{
			w = width();//размеры прямоугольника прорисовки равны размеру реального виджета imageLabel
			h = height();
			for(int y=0; y < h; y++)
				for(int x=0; x < w; x++)
                                        imgdata[maxImgWidth*(y+yShift)+x+xShift] = fitsd->getImgPixValue(Xo+x/scale, Yo+y/scale);// прорисовывается каждый 1/scale -тый пиксел реального ПЗС-кадра. Если scale = 0.5, то каждый второй. Это не приводит к значимому ухудшению качества картинки и обеспечивает нужную скорость при прокрутке больших изображений
//					imgdata[maxImgWidth*(y+Yo)+x+Xo] = fitsd->getImgPixValue(Xo+x/scale, Yo+y/scale);
		}
		else//масштаб = 1 (нет увеличения или уменьшения) или масштаб>1 (увеличение)
		{
			w = width()/scale;//размеры прямоугольника прорисовки в scale раз меньше размеров реального виджета imageLabel
			h = height()/scale;
			for(int y=0; y < h; y++)
				for(int x=0; x < w; x++)
                                        imgdata[maxImgWidth*(y+yShift)+x+xShift] = fitsd->getImgPixValue(Xo+x, Yo+y);// рисуем пикселы один к одному
		}
		//painter->setRenderHint(QPainter::Antialiasing);
		painter->drawImage(QRectF(0,0,width(),height()),*baseImg, QRectF(0,0,w,h));//копируем виртуальный экран на реальный. В случае увеличения происходит масштабирование от меньшего виртуального экрана к большему реальному.
                painter->setPen(marksIPixPen);
                mGr = fitsd->marksGIpix;
                for(int i=0; i<mGr->marks.size(); i++)
                {
                        xG = mGr->marks.at(i);
                        painter->drawEllipse(QRectF((xG->mTanImg[0]-Xo-(aperture/2.0))*scale, (xG->mTanImg[1]-Yo-(aperture/2.0))*scale, aperture*scale, aperture*scale));
                }

                painter->setPen(marksGridPen);
                mGr = fitsd->marksG;
		for(int i=0; i<mGr->marks.size(); i++)
		{
			xG = mGr->marks.at(i);
			painter->drawEllipse(QRectF((xG->mTanImg[0]-Xo-(aperture/2.0))*scale, (xG->mTanImg[1]-Yo-(aperture/2.0))*scale, aperture*scale, aperture*scale));
		}
		


                painter->setPen(marksRefPen);
                mGr = fitsd->refMarks;
                for(int i=0; i<mGr->marks.size(); i++)
                {
                        xG = mGr->marks.at(i);
                        painter->drawEllipse(QRectF((xG->mTanImg[0]-Xo-(aperture/2.0))*scale, (xG->mTanImg[1]-Yo-(aperture/2.0))*scale, aperture*scale, aperture*scale));
                }

                painter->setPen(marksObjPen);
                mGr = fitsd->objMarks;
                for(int i=0; i<mGr->marks.size(); i++)
                {
                        xG = mGr->marks.at(i);
                        painter->drawEllipse(QRectF((xG->mTanImg[0]-Xo-(aperture/2.0))*scale, (xG->mTanImg[1]-Yo-(aperture/2.0))*scale, aperture*scale, aperture*scale));
                }
		
		
		painter->setPen(basePen);
		
	}
};

void imageLabel::setAperture(double nApe)
{
    qDebug() << QString("\nimageLabel::setAperture(%1)\n").arg(nApe);
    fitsdata *fitsd = getCurFits();
    if(fitsd!=NULL)
    {

        aperture = fitsd->getApeSize(nApe);
        qDebug() << QString("aperture: %1\n").arg(aperture);
    }
}

int imageLabel::closeFitsFile(int num)
{
//	delete fitsdataList[num];
	fitsdataList.removeAt(num);
	if(frameIndex>(fitsdataList.count()-1)) frameIndex = fitsdataList.count()-1;

	return 0;
}



void imageLabel::marksIdent(int cat_type, double fov, double minM, double maxM)
{
//	QMessage();
//	QMessageBox::information(this, tr("My Application"), tr("The document has been modified.\n""Do you want to save your changes?"), QMessageBox::Ok); 
	qDebug() << "\n\nIDENTIFY\n\n"; 

	double ra, de, pmra, pmde, magn;
//	double *xy;
//	xy = new double[2];
	double *ekk, *tank;
	int *itank;
	/*double *ekk = new double[2];
	double *tank = new double[2];
	int *itank = new int[2]; */
	double ksi, eta;
	int i;
	
	QString elS;
	
	
	
	if(fitsdataList.count()>0)
	{
		fitsdata *fitsd = fitsdataList.at(frameIndex);
		marksGrid *mGr = fitsd->marksG;//marksList.at(frameIndex);
//		mGr->clearMarks();
		
//		QMessageBox::information(0, tr("info"), QString("%1").arg(fitsd->WCSdata[12]), QMessageBox::Ok);
		
		fov = fov*0.01*fitsd->imgArr->naxes[0]*mGr->scales[0]/60000.0/fitsd->scFactor;
//		QMessageBox::information(NULL, tr("fov"), QString("%1").arg(fov), QMessageBox::Ok);
			
			
			mGr->ctype = cat_type;
			QProcess outerProcess;
			QStringList outerArguments;
			outerArguments.clear();
			QString ucac2find_folder("");
			switch(mGr->ctype)
			{
			case 0://ucac2
				outerProcess.setWorkingDirectory(ucac2find_folder);
				outerProcess.setProcessChannelMode(QProcess::MergedChannels);
				outerProcess.setReadChannel(QProcess::StandardOutput);
				outerArguments << "-r" << mas_to_hms(grad_to_mas(fitsd->WCSdata[2]), ":", 3) << mas_to_damas(grad_to_mas(fitsd->WCSdata[3]),":",2) << QString(" %1").arg(fov) << QString(" %1").arg(minM) << QString(" %1 ").arg(maxM) << "-stdout";
				outerProcess.start("ucac2find.exe ", outerArguments);
				break;
			case 1://2mass
				break;
			}
			outerProcess.waitForFinished(-1);
			QTextStream catStream(outerProcess.readAllStandardOutput());
			QString catLine;
			
			QList <double*> ekvPos;
//			QList <double*> tanPos;
			
			while (!catStream.atEnd())
			{
				catLine = catStream.readLine();
//				QMessageBox::information(NULL, tr("Marks num"), catLine, QMessageBox::Ok);
				if(catLine.size()<26)
				{
//s					QMessageBox::information(NULL, tr("Marks num"), "ucac2find error", QMessageBox::Ok);
					continue;
				}
				switch(mGr->ctype)
				{
					case 0://ucac2
					{
						elS = catLine.section('|',0,0);ra = hms_to_mas(elS, " ");
						elS = catLine.section('|',1,1);de = damas_to_mas(elS, " ");
						elS = catLine.section('|',11,11);pmra = elS.toDouble();
						elS = catLine.section('|',12,12);pmde = elS.toDouble();
						elS = catLine.section('|',2,2);magn = elS.toDouble();
//						QMessageBox::information(NULL, tr("catLine"), QString("%1").arg(catLine), QMessageBox::Ok);

						ra = ra +pmra*(getYearFromMJD(fitsd->MJD)-2000);//taking proper motion into account
						de = de +pmde*(getYearFromMJD(fitsd->MJD)-2000);//taking proper motion into account
						
						ekk = new double[3];
//						tank = new double[2];
//						itank = new int[2];
						
						
					
						ekk[0] = ra; ekk[1] = de; ekk[2] = magn;
						ekvPos << ekk;
						
//						QMessageBox::information(0, tr("info"), QString("%1 %2 %3\n%4 %5\n%6 %7").arg(ekk[0]).arg(ekk[1]).arg(getYearFromMJD(fitsd->MJD)-2000).arg(tank[0]).arg(tank[1]).arg(itank[0]).arg(itank[1]), QMessageBox::Ok);
						break;
						}
					case 1://2mass
					{
						break;
					}
				}
			}
//		mGr->detTan(&fitsd->WCSdata[0], &fitsd->imgArr->naxes[0]);
		
//		qDebug() << "\n\nstv\n"; 

		double *stv = new double[ekvPos.size()*3];
		for(i=0; i<ekvPos.size(); i++)
		{
			stv[i*3] = ekvPos[i][0];
			stv[i*3+1] = ekvPos[i][1];
			stv[i*3+2] = ekvPos[i][2];
			
//			qDebug() << "stv[" << i << "]:" << stv[i*3] << "|" << stv[i*3+1] << "|" << stv[i*3+2] << "\n";
		}
		
//		qDebug() << "\n\nmkv\n";
		double *mkv = new double[mGr->marks.size()*3];
		tank = new double[2];
		for(i=0; i<mGr->marks.size(); i++)
		{
			tank[0] = (mGr->marks[i]->mTanImg[0]- mGr->xc - fitsd->imgArr->naxes[0]/2)*mGr->scales[0]/1000.0;
			tank[1] = (mGr->marks[i]->mTanImg[1]- mGr->yc - fitsd->imgArr->naxes[1]/2)*mGr->scales[1]/1000.0;
			mkv[i*3+2] = mGr->marks[i]->mPpix->meanADC;
			
			rot2D(tank, -mGr->rotAn);
			mkv[i*3] = tank[0];
			mkv[i*3+1] = tank[1];
		}

		int miRes = marksIdentify(mkv, mGr->marks.size(), stv, ekvPos.size(), 5); 
	}

}

void imageLabel::setInstProp(double mX, double mY, double fov, double ang)
{
	if(fitsdataList.count()>0)
	{
                fitsdataList[frameIndex]->setInstParam(mX, mY, fov, ang);
                //fitsdataList[frameIndex]->marksGIpix->setInstrProp(mX, mY, ang);
	}
}
/*
void imageLabel::setRotAngle(double ang)
{
	if(fitsdataList.count()>0)
	{
		fitsdataList[frameIndex]->marksG->rotAn = ang;
	}
	
}
*/
fitsdata* imageLabel::getCurFits()
{
	if(fitsdataList.count()>0) return fitsdataList[frameIndex];
	return NULL;
}

marksGrid* imageLabel::getCurMarks()
{
	if(fitsdataList.count()>0) return fitsdataList[frameIndex]->marksG;
	return NULL;
}

marksGrid* imageLabel::getCurMarksIpix()
{
	if(fitsdataList.count()>0) return fitsdataList[frameIndex]->marksGIpix;
	return NULL;
}

void imageLabel::changeScales(double dSc)
{
	marksGrid *mGrid = getCurMarks();
	mGrid->setScales(mGrid->scales[0]+dSc, mGrid->scales[1]+dSc);
	mGrid = getCurMarksIpix();
	mGrid->setScales(mGrid->scales[0]+dSc, mGrid->scales[1]+dSc);
}

void imageLabel::setScx(double Scx)
{
	marksGrid *mGrid = getCurMarks();
	mGrid->setScales(Scx, mGrid->scales[1]);
	mGrid = getCurMarksIpix();
	mGrid->setScales(Scx, mGrid->scales[1]);
}

void imageLabel::setScy(double Scy)
{
	marksGrid *mGrid = getCurMarks();
	mGrid->setScales(mGrid->scales[0], Scy);
	mGrid = getCurMarksIpix();
	mGrid->setScales(mGrid->scales[0], Scy);
}


void imageLabel::reMarks()
{
	fitsdata *fTmp = getCurFits();
//	marksGrid *gTmp = getCurMarks();
	
	fTmp->detTan();
//	gTmp->detTan(&fTmp->WCSdata[0], &fTmp->imgArr->naxes[0]);
}

/*
    objl.clear();
	refToStar.clear();
	QPoint pnt;
	double ksi,eta;
	vector<double> pixPos(2);
	for(int i = 0; i<imgF.p_tangpos->size1(); i++)
	{
	   
	   //str.setNum(ksi);
	   //QMessageBox::information(0,"debug",str,QMessageBox::Ok,0,0);
	   if (imgF.wcsD[12]==1)
	   {
			pixPos = getPixPosFromWCS(MEFP(*imgF.p_refcat,i,0),MEFP(*imgF.p_refcat,i,1),imgF.wcsD);
			pnt.setX((int)pixPos(0));
		    pnt.setY((int)pixPos(1));
	   }
	   else
	   {
		   ksi = MEFP(*imgF.p_tangpos,i,0)*cos(rot_angle)+MEFP(*imgF.p_tangpos,i,1)*sin(rot_angle);
		   eta = -MEFP(*imgF.p_tangpos,i,0)*sin(rot_angle)+MEFP(*imgF.p_tangpos,i,1)*cos(rot_angle);
		   pnt.setX(imgF.xSize/2 + (int)(ksi/(206.265*imgF.xpsize/imgF.flength)));
		   pnt.setY(imgF.ySize/2 - (int)(eta/(206.265*imgF.ypsize/imgF.flength)));
	   }
	   objl << pnt;
	   refToStar << i;
	}
	////
	if(imgF.p_cel_pos->size1()!=0)
	{
		m_objl.clear();
		vector<double> EP(2);
		for(int i = 0; i<imgF.p_cel_pos->size1(); i++)
		{
			if (imgF.wcsD[12]==1)
			{
				pixPos = getPixPosFromWCS(MEFP(*imgF.p_cel_pos,i,0),MEFP(*imgF.p_cel_pos,i,1),imgF.wcsD);
				pnt.setX((int)pixPos(0));
			    pnt.setY((int)pixPos(1));
			}
			else
			{
				EP = getRaDeToTang(MEFP(*imgF.p_cel_pos,i,0),
			                                    	MEFP(*imgF.p_cel_pos,i,1), 
													imgF.ra_oc, imgF.de_oc);
				ksi = EP(0)*cos(rot_angle)+EP(1)*sin(rot_angle);
				eta = -EP(0)*sin(rot_angle)+EP(1)*cos(rot_angle);
				pnt.setX(imgF.xSize/2 + (int)(ksi/(206.265*imgF.xpsize/imgF.flength)));
				pnt.setY(imgF.ySize/2 - (int)(eta/(206.265*imgF.ypsize/imgF.flength)));
			}
			m_objl << pnt;
		}
	}
	////
	deltaX=0;
	deltaY=0;
	rot_angle = 0;
	slotUpdateImg();
*/

void imageLabel::detInstrProp()
{
	fitsdata *fitsd = getCurFits();
	marksGrid *mGr = getCurMarks();
	marksGrid *mGrI = getCurMarksIpix();
//	fitsd->detPpix(aperture, 0);
	int szP0 = mGrI->marks.size();
	int szP1 = mGr->marks.size();
	double* const pX = new double[szP0];
	double* const pY = new double[szP0];
//   double *P = new double[szP0];
//	double *wcsKsi = new double[szP0];
//	double *wcsEta = new double[szP0];
   double* const pixP = new double[3*szP0];
   double* const We = new double[szP0];
//   wcsRA = new double[szP];
//   wcsDE = new double[szP];
	int i, j, x0, x1, y0, y1, df;
	int szP = 0;
//	marksGrid *grI, *grCel;
//	QList <marksP*> grI;
//	QList <marksP*> grCel;
	double *raW, *deW;
	raW = new double[szP0];
	deW = new double[szP0];
	double *magnW = new double[szP0];
	
	qDebug() << "szP0\t" << szP0 << "\n";
	qDebug() << "szP1\t" << szP1 << "\n";
	
        marksGrid *rGrid = fitsd->refMarks;
        rGrid->marks.clear();

	for(j=0; j<szP1; j++)
	{
		qDebug() << "Ksi " << mGr->marks[j]->mTan[0] << "\tEta " << mGr->marks[j]->mTan[1] << "\n";
	}
	
	for(i=0; i<szP0; i++)
	{
		x0 = mGrI->marks[i]->mTanImg[0];
		y0 = mGrI->marks[i]->mTanImg[1];
		
		for(j=0; j<szP1; j++)
		{
			x1 = mGr->marks[j]->mTanImg[0];
			y1 = mGr->marks[j]->mTanImg[1];
			df = sqrt((x1-x0)*(x1-x0) + (y1-y0)*(y1-y0));
			
			
			qDebug() << "df\t" << df << "\n";
			if(df<(aperture/4.0))
			{
/*
				pixP[szP*3] = x1;
				pixP[szP*3+1] = y1;
				pixP[szP*3+2] = 1;
				pX[szP] = mGr->marks[j]->mTan[0];
				pY[szP] = mGr->marks[j]->mTan[1];
				We[szP] = 1.0;
				raW[szP] = mGr->marks[j]->mEkv[0];
				deW[szP] = mGr->marks[j]->mEkv[1];
				magnW[szP] =  mGr->marks[j]->mEkv[2];
				qDebug() << "x0: " << x0 << "\tx1: " << x1 << "\n";
				qDebug() << "y0: " << y0 << "\ty1: " << y1 << "\n";
//				qDebug() << "a1*" << pixP[szP*3] << " + b1*" << pixP[szP*3+1] << " + c1*" << pixP[szP*3+2] << " = " << pX[szP] << " = " << pY[szP] << "\n";
*/
				rGrid->addEkvMark(mGr->marks[j]->mEkv[0], mGr->marks[j]->mEkv[1], mGr->marks[j]->mEkv[2]);
				rGrid->marks[szP]->mTan[0] = mGr->marks[j]->mTan[0];
				rGrid->marks[szP]->mTan[1] = mGr->marks[j]->mTan[1];
				rGrid->marks[szP]->mTanImg[0] = x0;
				rGrid->marks[szP]->mTanImg[1] = y0;
				
				
				szP++;
				break;
			}
		}
	}
	qDebug() << "\nszP\t" << szP << "\n";
	
        fitsd->detWCS(szP, 500);
	
	
	
	
//	STDATA stdata;
//	double ra_oc, de_oc;
/*	stdata = StDataFromVector(raW, szP);
	ra_oc = mas_to_grad(stdata.mean);
	stdata = StDataFromVector(deW, szP);
	de_oc = mas_to_grad(stdata.mean);*/
//	qDebug() << "\nra_oc\t" << ra_oc << "\t" << "de_oc\t" << de_oc << "\n";
/*	
	double OCp0, OCp1, tp0, tp1;
	getTangToRaDe1(&OCp0, &OCp1, 0, 0, grad_to_mas(ra_oc), grad_to_mas(de_oc));
	qDebug() << "\nOCp0\t" << OCp0 << "\t" << "OCp1\t" << OCp1 << "\n";
	
	for(i=0; i<szP; i++)
	{
		getRaDeToTang1(&tp0, &tp1, raW[i], deW[i], OCp0, OCp1);
		pX[szP]=tp0;pY[szP]=tp1;
//		pX[szP]/=1000.0;pY[szP]/=1000.0;
		qDebug() << "\npX\t" << pX[i] << "\tpY\t" << pY[i] << "\n";
	}
	/

	ra_oc = fitsd->WCSdata[2];
	de_oc = fitsd->WCSdata[3];
	
	double* const Zx = new double[3];
	double* const Zy = new double[3];
	
	slsm(3, szP, Zx, pixP, pX, We);
	QMessageBox::information(0, "Zx", QString("%1\t%2\t%3").arg(Zx[0]).arg(Zx[1]).arg(Zx[2]));
	slsm(3, szP, Zy, pixP, pY, We);
	QMessageBox::information(0, "Zy", QString("%1\t%2\t%3").arg(Zy[0]).arg(Zy[1]).arg(Zy[2]));
	
	qDebug() << "\nZx\t" << Zx[0] << "\t" << Zx[1] << "\t" << Zx[2] << "\n";
	qDebug() << "\nZy\t" << Zy[0] << "\t" << Zy[1] << "\t" << Zy[2] << "\n";

	double Mx = sqrt(Zx[0]*Zx[0] + Zy[0]*Zy[0]);
	double My = sqrt(Zx[1]*Zx[1] + Zy[1]*Zy[1]);
	
	double teta = atan2(Zx[1], Zy[1]);
	double gam = atan2((Zx[0]*Zx[1] + Zy[0]*Zy[1]), -(Zx[0]*Zy[1] + Zy[0]*Zx[1]));
	double xT = (Zx[1]*Zy[2] - Zy[1]*Zx[2])/(Zx[0]*Zy[1] + Zy[0]*Zx[1]);
	double yT = (Zy[0]*Zx[2] - Zx[0]*Zy[2])/(Zx[0]*Zy[1] + Zy[0]*Zx[1]);
	
	qDebug() << "\nMx\t" << Mx << "\n";
	qDebug() << "\nMy\t" << My << "\n";
	qDebug() << "\nteta\t" << teta << "\n";
	qDebug() << "\ngam\t" << gam << "\n";
	qDebug() << "\nxT\t" << xT << "\n";
	qDebug() << "\nyT\t" << yT << "\n";
	
	QMessageBox::information(0, "Param", QString("Mx %1\nMy %2\nteta %3\ngam %4\nxT %5\nyT %6\n").arg(Mx).arg(My).arg(teta).arg(gam).arg(xT).arg(yT));
	
	double *Eta = new double[szP];
	double *Ksi = new double[szP];
	double ra, de;
	mGr->clearMarks();
	mGrI->clearMarks();

	double ksi_oc = Zx[0]*xT + Zx[1]*yT + Zx[2];
	double eta_oc = Zy[0]*xT + Zy[1]*yT + Zy[2];
	QMessageBox::information(0, "Ksi Eta", QString("%1\t%2\n").arg(ksi_oc*1000.0, 12, 'f', 2).arg(eta_oc*1000.0, 12, 'f', 2));
	getTangToRaDe1(&ra, &de, ksi_oc*1000.0, eta_oc*1000.0, grad_to_mas(ra_oc), grad_to_mas(de_oc));
	QMessageBox::information(0, "RA DE", QString("%1\t%2\n%3\t%4\n%5\t%6").arg(ra, 12, 'f', 2).arg(de, 12, 'f', 2).arg(grad_to_mas(ra_oc), 12, 'f', 2).arg(grad_to_mas(de_oc), 12, 'f', 2).arg(ra - grad_to_mas(ra_oc), 12, 'f', 2).arg(de - grad_to_mas(de_oc), 12, 'f', 2));
	ra_oc = mas_to_grad(ra);
	de_oc = mas_to_grad(de);


bool singulAR=true;
 double Sx=0, Sy=0;
   for (i=0;i<szP;i++)//считаем ошибки единицы веса.
   {
      Sx=Sx+pow(pX[i]-(Zx[0]*pixP[i*3]+Zx[1]*pixP[i*3+1]+Zx[2]),2);
      Sy=Sy+pow(pY[i]-(Zy[0]*pixP[i*3]+Zy[1]*pixP[i*3+1]+Zy[2]),2);
   }
   Sx = sqrt(Sx/(szP-1));
   Sy = sqrt(Sy/(szP-1));
   qDebug() << "\nSx\t" << Sx  << "\tSy\t" << Sy << "\n";
   if(Sx>10000||Sy>10000) singulAR *= false;
   double *mCD = new double[4];// матрица для хранения постоянных ПЗС-кадра в стиле WCS
   double *ImCD = new double[4];
//   double *
 //  mCD(0,0)=Zx[0];mCD(0,1)=Zx(1);
//   mCD(1,0)=Zy[0];mCD(1,1)=Zy(1);
	mCD[0] = Zx[0]; mCD[1] = Zx[1];
	mCD[2] = Zy[0]; mCD[3] = Zy[1];
	qDebug() << "mCD\n" << mCD[0] << " " << mCD[1] << "\n" << mCD [2] << " " << mCD[3] << "\n"; 
   double *shC = new double[2];
   shC[0]=-Zx[2];shC[1]=-Zy[2];//вместо свободного параметра хранятся координаты опорного (центрального) пиксела
   qDebug() << "shC\n" << shC[0] << " " << shC[1] << "\n";
   
   double *rPix = new double[2];
//   prod(gjinverse(mCD, singulAR),shC); //вычисление координат опорного пиксела с помощью постоянных кадра
   singulAR *= invgj(ImCD, mCD, 2);
   qDebug() << "ImCD\n" << ImCD[0] << " " << ImCD[1] << "\n" << ImCD [2] << " " << ImCD[3] << "\n";
   singulAR *= mvprod(rPix, ImCD, shC, 2, 2);
   qDebug() << "\nrPix\t" << rPix[0] << " " << rPix[1] << "\n";
   
   QMessageBox::information(0, "Singular", QString("%1").arg(singulAR));
   ///////////
//	if(singulAR)
//	{
	   fitsd->WCSdata[12]=0;//показывает, что WCS будет вписана
	   fitsd->WCSdata[0]=rPix[0];//+fitsd->imgArr->naxes[0]/2;//опорный пиксель при смещении начала координат в левый верхний угол кадра (правило такое есть при вписывании WCS)
	   fitsd->WCSdata[1]=rPix[1];//+fitsd->imgArr->naxes[1]/2;
//	   fitsd->WCSdata[2]=ra_oc/3600000;// RA, DEC опорного пиксела в градусах
//	   fitsd->WCSdata[3]=de_oc/3600000;
	   fitsd->WCSdata[4]=sqrt(pow(Zx[0]/3600,2)+pow(Zy[0]/3600,2));// расчет масштабов вдоль осей (градусы на пиксель - неудобно, а что делать - стандарт надо уважать)
	   fitsd->WCSdata[5]=sqrt(pow(Zx[1]/3600,2)+pow(Zy[1]/3600,2));
	   double rho1 =180*atan2(-Zy[0]/fitsd->WCSdata[4],Zx[0]/fitsd->WCSdata[4])/(atan(1)*4);//расчет углов между осями тангенциальных и пиксельных координат
	   double rho2 = 180*atan2(Zx[1]/fitsd->WCSdata[5],Zy[1]/fitsd->WCSdata[5])/(atan(1)*4);// углов два, так как в общем случае возможна косоугольность 
	   if (rho1<0) rho1 = 360+rho1;
	   if (rho2<0) rho2 = 360+rho2;
	   fitsd->WCSdata[6]=rho1;//записываем углы
	   fitsd->WCSdata[7]=rho2;
	   fitsd->WCSdata[8]=Zx[0];//записываем постоянные кадра (масштаб+поворот)
	   fitsd->WCSdata[9]=Zx[1];
	   fitsd->WCSdata[10]=Zy[0];
	   fitsd->WCSdata[11]=Zy[1];
   
	
	qDebug() << "\nWCSdata[12]\t" << fitsd->WCSdata[12] << "\n";
	qDebug() << "\nWCSdata[0]\t" << fitsd->WCSdata[0] << "\n";
	qDebug() << "\nWCSdata[1]\t" << fitsd->WCSdata[1] << "\n";
	qDebug() << "\nWCSdata[2]\t" << ra_oc/3600000 << "\n";
	qDebug() << "\nWCSdata[3]\t" << de_oc/3600000 << "\n";
	qDebug() << "\nWCSdata[4]\t" << fitsd->WCSdata[4] << "\n";
	qDebug() << "\nWCSdata[5]\t" << fitsd->WCSdata[5] << "\n";
	qDebug() << "\nWCSdata[6]\t" << fitsd->WCSdata[6] << "\n";
	qDebug() << "\nWCSdata[7]\t" << fitsd->WCSdata[7] << "\n";
	qDebug() << "\nWCSdata[8]\t" << fitsd->WCSdata[8] << "\n";
	qDebug() << "\nWCSdata[9]\t" << fitsd->WCSdata[9] << "\n";
	qDebug() << "\nWCSdata[10]\t" << fitsd->WCSdata[10] << "\n";
	qDebug() << "\nWCSdata[11]\t" << fitsd->WCSdata[11] << "\n";
	
	
//	mGr->xc = 0;
//	mGr->yc = 0;
	double *xtf = new double[2]; 
	for(i=0; i<szP; i++)
	{
		Ksi[i] = Zx[0]*pixP[i*3] + Zx[1]*pixP[i*3+1] + Zx[2];
		Eta[i] = Zy[0]*pixP[i*3] + Zy[1]*pixP[i*3+1] + Zy[2];
		xtf[0] = Ksi[i];
		xtf[1] = Eta[i];
//		rot2D(xtf, -mGr->rotAn);
		QMessageBox::information(0, "Ksi Eta", QString("%1\t%2\n%3\t%4").arg(Ksi[i], 12, 'f', 2).arg(Eta[i], 12, 'f', 2).arg(pX[i], 12, 'f', 2).arg(pY[i], 12, 'f', 2));
		getTangToRaDe1(&ra, &de, Ksi[i], Eta[i], grad_to_mas(fitsd->WCSdata[2]), grad_to_mas(fitsd->WCSdata[3]));
		QMessageBox::information(0, "RA DE", QString("%1\t%2\n%3\t%4\n%5\t%6").arg(ra, 12, 'f', 2).arg(de, 12, 'f', 2).arg(raW[i], 12, 'f', 2).arg(deW[i], 12, 'f', 2).arg(ra - raW[i], 12, 'f', 2).arg(de - deW[i], 12, 'f', 2));
		mGr->addEkvMark(ra, de, magnW[i]);
//	}
	}
	*/
//	reMarks();
	
}

int imageLabel::identAuto(double ang, int identType)
{
        qDebug() << "\nidentAuto\n";

	
	fitsdata *fitsd = getCurFits();

        if(fitsd->identAuto(fitsd->refMarks, ang, 4, identType)) return 1;

/*	marksGrid *mGr = getCurMarks();
	marksGrid *mGrI = getCurMarksIpix();
	
	mGr->sortMagn();
	mGrI->sortAper(1);
	
	nSize = mGrI->marks.size();
	kSize = mGr->marks.size();
	
	if(nSize>20) nSize = 20;
	if(kSize>100) kSize = 100;
	
	OCp0 = grad_to_mas(fitsd->WCSdata[2]);
	OCp1 = grad_to_mas(fitsd->WCSdata[3]);

	double *x = new double[nSize];
	double *y = new double[nSize];
	
	double *ksiI = new double[kSize];
	double *etaI = new double[kSize];
	double *ksi = new double[kSize];
	double *eta = new double[kSize];
	double *magn = new double[kSize];
	double *ra = new double[kSize];
	double *de = new double[kSize];
	
	qDebug() << "\nx y\n";
	for(i = 0; i< nSize; i++)
	{
		x[i] = mGrI->marks[i]->mTanImg[0];
		y[i] = mGrI->marks[i]->mTanImg[1];
		qDebug() << "\n" << x[i] << "\t" << y[i] << "\t" << mGrI->marks[i]->mTanImg[2];
	}
	qDebug() << "\nksi eta\n";
	for(i = 0; i< kSize; i++)
	{
		ksi[i] = mGr->marks[i]->mTan[0];
		eta[i] = mGr->marks[i]->mTan[1];
		ksiI[i] = mGr->marks[i]->mTanImg[0];
		etaI[i] = mGr->marks[i]->mTanImg[1];
		ra[i] = mGr->marks[i]->mEkv[0];
		de[i] = mGr->marks[i]->mEkv[1];
		magn[i] = mGr->marks[i]->mEkv[2];
		qDebug() << "\n" << ksi[i] << "\t" << eta[i] << "\t" << magn[i];
	}
	double xscale, yscale;
//	xscale = 1.0;//mGr->scales[0]/1000.0;
//	yscale = 1.0;//mGr->scales[1]/1000.0;
	
	xscale = mGr->scales[0]/1000.0;
	yscale = mGr->scales[1]/1000.0;
	
	qDebug() << "\nxscale " << xscale << "\tyscale " << yscale;
	
	bool fvm=false;
	int *sootv = new int[nSize];
	
        otogdestv(xscale, yscale, kSize, nSize, fvm, ksi, eta, x, y, sootv, 1, ang, 5);
/*        int otogdestv(double xscale,double yscale,int t100, int t10s,
                          bool newinstrument,
                          double* ksi, double* eta, double* x, double* y,int* sootv );
        int otogdestv(double xscale,double yscale,int t100, int t10s,
                          bool newinstrument,
                          double* ksi, double* eta, double* xn10, double* yn10,int* sootv,
                          bool bio,
                          double tet,
                          double eps
                                        );
/
	int oSize = 0;
	
	double *xW = new double[nSize];
	double *yW = new double[nSize];
	
	double *ksiW = new double[nSize];
	double *etaW = new double[nSize];
	double *raW = new double[nSize];
	double *deW = new double[nSize];
	double *magnW = new double[nSize];
	
	int k = 0;	
	
	for(i = 0; i< nSize; i++)
	{
		qDebug() << "\n" << i << ":" << sootv[i] << "\n";
		if(sootv[i]!=-1)
		{
			xW[oSize] = x[i];
			yW[oSize] = y[i];
			ksiW[oSize] = ksi[sootv[i]];
			etaW[oSize] = eta[sootv[i]];
			raW[oSize] = ra[sootv[i]];
			deW[oSize] = de[sootv[i]];
			magnW[oSize] = magn[sootv[i]];
			qDebug() << "\n" << ksiW[oSize] << "\t" << etaW[oSize] << "\t" << magnW[oSize];
			oSize++;
		}
	}
	qDebug() << "\noSize\t" << oSize << "\n";
	
//	marksGrid *maG = new marksGrid;
        qDebug() << "\nResGrid\nra\tde\tmagn\tksi\teta\tx\ty\t\n";
	for(i = 0; i< oSize; i++)
	{
		resGrid->addEkvMark(raW[i], deW[i], magnW[i]);
		resGrid->marks[i]->mTan[0] = ksiW[i];
		resGrid->marks[i]->mTan[1] = etaW[i];
		resGrid->marks[i]->mTanImg[0] = xW[i];
		resGrid->marks[i]->mTanImg[1] = yW[i];
		
		qDebug() << "\n" << QString("%1\t%2\t").arg(resGrid->marks[i]->mEkv[0], 12, 'f', 2).arg(resGrid->marks[i]->mEkv[1], 12, 'f', 2) << resGrid->marks[i]->mEkv[2] << "\t" << resGrid->marks[i]->mTan[0] << "\t" << resGrid->marks[i]->mTan[1] << "\t" << resGrid->marks[i]->mTanImg[0] << "\t" << resGrid->marks[i]->mTanImg[1] << "\n";
	}
	
	if(oSize<3) return 1;
	return 0;
/*	
	mGr->marks.clear();
	mGrI->marks.clear();
	
//	fitsd->detWCS()

	double ra_oc, de_oc;
	
	ra_oc = fitsd->WCSdata[2];
	de_oc = fitsd->WCSdata[3];
	double *pixP = new double[oSize*3];
	double *We = new double[oSize];
	
	qDebug() << "\npixP\n";
	for(i = 0; i< oSize; i++)
	{
		pixP[i*3] = xW[i];
		pixP[i*3+1] = yW[i];
		pixP[i*3+2] = 1;
		We[i] = 1;
		qDebug() << "\n" << pixP[i*3] << "\t" << pixP[i*3+1] << "\t" << pixP[i*3+2] << "\n";
	}
	
	double* const Zx = new double[3];
	double* const Zy = new double[3];
	double *mCD = new double[4];// матрица для хранения постоянных ПЗС-кадра в стиле WCS
	double *ImCD = new double[4];
/*	double Dx[3][3];
	double Dy[3][3];
	int exclind[oSize];
	double uwe;
	int rn;/
	
	slsm(3, oSize, Zx, pixP, ksiW, We);
//	iLSM(3, oSize, 0.3, &exclind[0], Zx, pixP, ksiW, uwe, &Dx[0][0], 3.0, rn, We);
	QMessageBox::information(0, "Zx", QString("%1\t%2\t%3").arg(Zx[0]).arg(Zx[1]).arg(Zx[2]));
//	qDebug() << "\nrn\t" << rn << "\n"
	slsm(3, oSize, Zy, pixP, etaW, We);
//	iLSM(3, oSize, 0.5, &exclind[0], Zy, pixP, etaW, uwe, &Dy[0][0], 3.0, rn, We);
	QMessageBox::information(0, "Zy", QString("%1\t%2\t%3").arg(Zy[0]).arg(Zy[1]).arg(Zy[2]));
//	qDebug() << "\nrn\t" << rn << "\n"
	
	qDebug() << "\nZx\t" << Zx[0] << "\t" << Zx[1] << "\t" << Zx[2] << "\n";
	qDebug() << "\nZy\t" << Zy[0] << "\t" << Zy[1] << "\t" << Zy[2] << "\n";

	double Mx = sqrt(Zx[0]*Zx[0] + Zy[0]*Zy[0]);
	double My = sqrt(Zx[1]*Zx[1] + Zy[1]*Zy[1]);
	
	double teta = atan2(Zx[1], Zy[1]);
	double gam = atan2((Zx[0]*Zx[1] + Zy[0]*Zy[1]), -(Zx[0]*Zy[1] + Zy[0]*Zx[1]));
	double xT = (Zx[1]*Zy[2] - Zy[1]*Zx[2])/(Zx[0]*Zy[1] + Zy[0]*Zx[1]);
	double yT = (Zy[0]*Zx[2] - Zx[0]*Zy[2])/(Zx[0]*Zy[1] + Zy[0]*Zx[1]);
	
	qDebug() << "\nMx\t" << Mx << "\n";
	qDebug() << "\nMy\t" << My << "\n";
	qDebug() << "\nteta\t" << teta << "\n";
	qDebug() << "\ngam\t" << gam << "\n";
	qDebug() << "\nxT\t" << xT << "\n";
	qDebug() << "\nyT\t" << yT << "\n";
	
	double Sx=0, Sy=0;
   for (i=0;i<oSize;i++)//считаем ошибки единицы веса.
   {
      Sx=Sx+pow(ksiW[i]-(Zx[0]*xW[i]+Zx[1]*yW[i]+Zx[2]),2);
      Sy=Sy+pow(etaW[i]-(Zy[0]*xW[i]+Zy[1]*yW[i]+Zy[2]),2);
   }
   Sx = 1000*sqrt(Sx/(oSize-1));
   Sy = 1000*sqrt(Sy/(oSize-1));

	qDebug() << "\nSx\t" << Sx << "\tSy\y" << Sy << "\n"; 
	
	mCD[0] = Zx[0]; mCD[1] = Zx[1];
	mCD[2] = Zy[0]; mCD[3] = Zy[1];
	qDebug() << "mCD\n" << mCD[0] << " " << mCD[1] << "\n" << mCD [2] << " " << mCD[3] << "\n"; 
   double *shC = new double[2];
   shC[0]=-Zx[2];shC[1]=-Zy[2];//вместо свободного параметра хранятся координаты опорного (центрального) пиксела
   qDebug() << "shC\n" << shC[0] << " " << shC[1] << "\n";
   
   double *rPix = new double[2];
   bool singulAR = invgj(ImCD, mCD, 2);
   qDebug() << "ImCD\n" << ImCD[0] << " " << ImCD[1] << "\n" << ImCD [2] << " " << ImCD[3] << "\n";
   singulAR *= mvprod(rPix, ImCD, shC, 2, 2);
   qDebug() << "\nrPix\t" << rPix[0] << " " << rPix[1] << "\n";
      
   for(int j=0; j<3; j++)
   	{
		Zx[j] /= 3600.0; Zy[j] /= 3600.0;
  	}
  	
//  	double *WCSdata = new double[12];
  	
	fitsd->WCSdata[12]=1;//показывает, что WCS будет вписана
	fitsd->WCSdata[0]=rPix[0];//+curF->imgArr->naxes[0]/2;//опорный пиксель при смещении начала координат в левый верхний угол кадра (правило такое есть при вписывании WCS)
	fitsd->WCSdata[1]=rPix[1];//+curF->imgArr->naxes[1]/2;
	fitsd->WCSdata[2]=OCp0/3600000;// RA, DEC опорного пиксела в градусах
	fitsd->WCSdata[3]=OCp1/3600000;
	fitsd->WCSdata[4]=sqrt(pow(Zx[0],2)+pow(Zy[0],2));// расчет масштабов вдоль осей (градусы на пиксель - неудобно, а что делать - стандарт надо уважать)
	fitsd->WCSdata[5]=sqrt(pow(Zx[1],2)+pow(Zy[1],2));
	double rho1 =180*atan2(-Zy[0]/fitsd->WCSdata[4],Zx[0]/fitsd->WCSdata[4])/(atan(1)*4);//расчет углов между осями тангенциальных и пиксельных координат
	double rho2 = 180*atan2(Zx[1]/fitsd->WCSdata[5],Zy[1]/fitsd->WCSdata[5])/(atan(1)*4);// углов два, так как в общем случае возможна косоугольность 
	if(rho1<0) rho1 = 360+rho1;
	if (rho2<0) rho2 = 360+rho2;
	fitsd->WCSdata[6]=rho1;//записываем углы
	fitsd->WCSdata[7]=rho2;
	fitsd->WCSdata[8]=Zx[0];//записываем постоянные кадра (масштаб+поворот)
	fitsd->WCSdata[9]=Zx[1];
	fitsd->WCSdata[10]=Zy[0];
	fitsd->WCSdata[11]=Zy[1];

	qDebug() << "\nWCSdata[12]\t" << fitsd->WCSdata[12] << "\n";
	qDebug() << "\nWCSdata[0]\t" << fitsd->WCSdata[0] << "\n";
	qDebug() << "\nWCSdata[1]\t" << fitsd->WCSdata[1] << "\n";
	qDebug() << "\nWCSdata[2]\t" << fitsd->WCSdata[2] << "\n";
	qDebug() << "\nWCSdata[3]\t" << fitsd->WCSdata[3] << "\n";
	qDebug() << "\nWCSdata[4]\t" << fitsd->WCSdata[4] << "\n";
	qDebug() << "\nWCSdata[5]\t" << fitsd->WCSdata[5] << "\n";
	qDebug() << "\nWCSdata[6]\t" << fitsd->WCSdata[6] << "\n";
	qDebug() << "\nWCSdata[7]\t" << fitsd->WCSdata[7] << "\n";
	qDebug() << "\nWCSdata[8]\t" << fitsd->WCSdata[8] << "\n";
	qDebug() << "\nWCSdata[9]\t" << fitsd->WCSdata[9] << "\n";
	qDebug() << "\nWCSdata[10]\t" << fitsd->WCSdata[10] << "\n";
	qDebug() << "\nWCSdata[11]\t" << fitsd->WCSdata[11] << "\n";
/

	delete [] ksiW;
	delete [] etaW;
	delete [] raW;
	delete [] deW;
	delete [] magnW;
/*
	delete [] pixP;
	delete [] We;
	delete [] Zx;
	delete [] Zy;
	delete [] mCD;
	delete [] ImCD;
	*/
	return 0;
}
