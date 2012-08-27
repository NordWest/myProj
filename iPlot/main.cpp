#include <QApplication>
#include "./../libs/multidim.h"
#include "./../libs/astro.h"
#include "./../libs/mb.h"
//#include <conio>
#include <QRgb>
#include <QPen>
#include <QBrush>
#include <QTextOption>
#include <QPainter>
#include <QImage>

int infoH;
int infoH1;
int infoW;
int maxIW;
int maxIH;
double *maxV;
double *minV;
double deMin, deMax;
double ocMin, ocMax;
double ocMaxX, ocMinX, ocMaxY, ocMinY;

double *cm;
double *Z;
double *L;
double *we;

double detT2pos(double x, int k)
{
	return(infoH + ((maxV[k]-x)/(maxV[k]-minV[k]))*maxIH);
}

double detT3posX(double x)
{
	return(infoW + (1.0-(deMax-x)/(deMax-deMin))*maxIW);
}

double detT3posY(double x)
{
	return(infoH + ((ocMax-x)/(ocMax-ocMin))*maxIH);
}




int main(int argc, char *argv[])
{  
    QApplication app(argc, argv);
	int gRes;
	if(argc!=2)
	{
		printf("\nno file name\n");
		return 1;
	}
	
	QFile dataFile;
	QSettings *settings = new QSettings("./iPlot.ini",QSettings::IniFormat);
	int taskNum = settings->value("general/taskNum").toInt();
        int fontSize0 = settings->value("general/fontSize0").toInt();
        int fontSize1 = settings->value("general/fontSize1").toInt();
        QString fname(argv[1]);
//        QString fname("res.dat");
	
	QString resPref = fname.mid(fname.lastIndexOf("/"),fname.lastIndexOf(".")-fname.lastIndexOf("/")); 
	
	dataFile.setFileName(fname);
	dataFile.open(QIODevice::ReadOnly| QIODevice::Text);
	
	if(!dataFile.isOpen())
	{
		printf("\nfile is not open\n");
		return 2;
	}
	
	QTextStream dataStream;
	dataStream.setDevice(&dataFile);
	
	QString dataLine;
	QStringList operands;
	QList <double*> dims;
	QList <int> dNums;
	double *dimN;
	int i, j, k, sz, sz1;
	int axnum;
	int *ax;
	int *pos;
	int ikey = 0;
	multidim *md0;
	double ocX, ocY;
	long nums;
	multidim *xVectField;
	multidim *yVectField;
	multidim *vectNumField;
	QString tStr;
	int p0, p1;
	
	double valMin;
	double dX, dY;
	int rk = 0;
	
	double *deVect;
	QList <double*> deList;
	
	double *deVectX;
	QList <double*> deListX;
	double *deVectY;
	QList <double*> deListY;
	int* numsList;
	
	double val;
	int cpos;
	
	double axeXpos;
	
	QImage *baseImg;
	QVector<QRgb> colormap;
	QPen basePen;
	QPen axesPen;
	QPen gridPen;
	QPen dataPen;
	QPen pointPen;
	QPen trendPen;
	
	basePen.setColor(Qt::black);
	basePen.setWidth(5);
	
	axesPen.setColor(Qt::lightGray);
	axesPen.setWidth(5);
	
	gridPen.setColor(Qt::cyan);
	gridPen.setWidth(3);
	gridPen.setStyle(Qt::DashLine);
	
	dataPen.setColor(Qt::blue);
	dataPen.setWidth(5);
	
	pointPen.setColor(Qt::darkGreen);
	pointPen.setWidth(1);
	
	trendPen.setColor(Qt::red);
	trendPen.setWidth(1);

	QBrush baseBrush(Qt::white, Qt::SolidPattern);
	QBrush pointBrush(Qt::darkGreen, Qt::SolidPattern);
     
	QString resFileName;
	QPoint pPos, pPos1;
	
	double scale0, scale1;
	double scMul, scLen;
	long ndots;
	QRect textRect;
	QRect textRectY;
	QString infoStr;
	QTextOption textOpt;
	QTextOption textOptY;
	QTextOption numOpt;
	QTextOption axesOpt;
	QRect axeRect;
	QPoint scPos;
	QPoint scPos0, scPos1;
	QRect scRect;
	QRect numRect;
	QPainter painter;
	double maxY;
	int isDrawGrid;
	int numTot;
	double stepX, stepY;
	
	
	qDebug() << "\nbegin\n";
	qDebug() << "\ntaskNum= " << taskNum << "\n";
	
	if((taskNum==0)||(taskNum==1)||(taskNum==6))
	{
		
		
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
				qDebug() << "\nikey= " << ikey << "\n";
				if(!ikey)
				{
					axnum = dims.size();
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
				vectNumField->set(nums, pos);
			}
		}
	}
	else if(taskNum==2)
	{
		
		ocMax = 0;
		ocMin = 0;
		deMin = 90;
		deMax = -90;
		double *deVect;
		
		while(!dataStream.atEnd())
		{
			deVect = new double[3];
			dataLine = dataStream.readLine();
//			qDebug() << dataLine << "\n";
			operands = dataLine.split("|");
			deVect[0] = operands[0].toDouble();
			deVect[1] = operands[1].toDouble();
			deList << deVect;
			if(ocMax<deVect[1]) ocMax = deVect[1];
			if(deMin>deVect[0]) deMin = deVect[0];
			if(deMax<deVect[0]) deMax = deVect[0];
		}
		
	}
	else if(taskNum==3||taskNum==5||taskNum==7)
	{
		
		maxV = new double[2];
		minV = new double[2];
		maxV[0] = maxV[1] = -10000;
		minV[0] = minV[1] = 10000;
		
		i=0;
		ikey = 0;

		while(!dataStream.atEnd())
		{
			dataLine = dataStream.readLine();
			if(dataLine[0]=='#')
			{
				tStr.clear();
				tStr.insert(0, dataLine.mid(1));
				qDebug() << "\n" << tStr << "\n";
				operands.clear();
				operands = tStr.split("|");
				sz = operands.size();
				qDebug() << "\nsz= " << sz << "\n";
				dimN = new double[sz];
				
				for(j=0; j<sz; j++)
				{
					dimN[j] = operands[j].toDouble();
				}
			}
			else
			{
				qDebug() << "\ni=" << i << "\n";
				if(!ikey)
				{
					deVectX = new double[sz-1];
					deVectY = new double[sz-1];
					numsList = new int[sz-1];
					ikey=1;
				}
			
				operands = dataLine.split("|");
				qDebug() << operands.join("|") << "\n";
				if(operands.size()!=4) break;
				deVectX[i] = operands[1].toDouble();
				deVectY[i] = operands[2].toDouble();
				numsList[i] = operands[3].toInt();
				
				if(maxV[0]<deVectX[i]) maxV[0] = deVectX[i];
				if(minV[0]>deVectX[i]) minV[0] = deVectX[i];
				if(maxV[1]<deVectY[i]) maxV[1] = deVectY[i];
				if(minV[1]>deVectY[i]) minV[1] = deVectY[i];
				i++;
			}
		}
	}
	else if(taskNum==4)
	{
		
		
		maxV = new double[3];
		minV = new double[3];
		maxV[0] = maxV[1] = maxV[2] = -10000;
		minV[0] = minV[1] = minV[2] = 10000;
		deMin = 100000.0;
		deMax = 0.0;
		while(!dataStream.atEnd())
		{
			deVect = new double[4];
			dataLine = dataStream.readLine();
			qDebug() << dataLine << "\n";
			operands = dataLine.split("|");
			deVect[0] = operands[0].toDouble();
			deVect[1] = operands[1].toDouble();
			deVect[2] = operands[2].toDouble();
			deVect[3] = operands[3].toDouble();
			deList << deVect;
			if(maxV[0]<deVect[1]) maxV[0] = deVect[1];
			if(minV[0]>deVect[1]) minV[0] = deVect[1];
			if(maxV[1]<deVect[2]) maxV[1] = deVect[2];
			if(minV[1]>deVect[2]) minV[1] = deVect[2];
			if(maxV[2]<deVect[3]) maxV[2] = deVect[3];
			if(minV[2]>deVect[3]) minV[2] = deVect[3];
			if(deMin>deVect[0]) deMin = deVect[0];
			if(deMax<deVect[0]) deMax = deVect[0];
		}
		
	}
	else if(taskNum==8)
	{
		
		
		maxV = new double[1];
		minV = new double[1];
		maxV[0] = -10000;
		minV[0] = 10000;
		deMin = 100000.0;
		deMax = 0.0;
		while(!dataStream.atEnd())
		{
			deVect = new double[4];
			dataLine = dataStream.readLine();
			qDebug() << dataLine << "\n";
			operands = dataLine.split("|");
			deVect[0] = operands[0].toDouble();
			deVect[1] = operands[1].toDouble();
//			deVect[2] = operands[2].toDouble();
//			deVect[3] = operands[3].toDouble();
			deList << deVect;
			if(maxV[0]<deVect[1]) maxV[0] = deVect[1];
			if(minV[0]>deVect[1]) minV[0] = deVect[1];
			
			if(deMin>deVect[0]) deMin = deVect[0];
			if(deMax<deVect[0]) deMax = deVect[0];
		}
		
	}
	
	qDebug() << "\nInitialization ended\n";
	
	
	
	switch(taskNum)
	{
		case 0:
	///////////////////////////////////////////////////////////////////
		infoH = settings->value("task0/infoH").toInt();
	
                dX = fabs(dims[0][1]+dims[0][0])/2.0;
                dY = fabs(dims[1][1]+dims[1][0])/2.0;
		
	
		qDebug() << "\ndX= " << dX << "\ndY= " << dY;
		
		qDebug() << "\n";
		for(j=0; j<dNums[0]; j++)
		{
                        dims[0][j] += dX;
			qDebug() << dims[0][j] << "\t"; 
		}
		qDebug() << "\n";
		
		for(j=0; j<dNums[1]; j++)
		{
                        dims[1][j] = dims[1][j]+dY;
			qDebug() << dims[1][j] << "\t";
		}
		qDebug() << "\n";
                maxY = dims[1][dNums[1]-1];
		
		maxIW = dims[0][dNums[0]-1];
		maxIH = maxY;
		
		
		for(j=0; j<dNums[1]; j++)
		{
			dims[1][j] = maxY - dims[1][j];
			qDebug() << dims[1][j] << "\t";
		}
		qDebug() << "\n";
		
		qDebug() << "\nmaxIW= " << maxIW << "\n";
		qDebug() << "\nmaxIH= " << maxIH << "\n";
	///////////////////////////////////////////////////////////////////
	
		scale0 = settings->value("task0/scale0").toDouble();
		scale1 = settings->value("task0/scale1").toDouble();
		scMul = settings->value("task0/scMul").toDouble();
		scLen = settings->value("task0/scLen").toDouble();
		
		textRect.setRect(0, maxIH, maxIW/2.0, infoH);
		textOpt.setAlignment(Qt::AlignCenter);
		numOpt.setAlignment(Qt::AlignRight|Qt::AlignBottom);
		axesOpt.setAlignment(Qt::AlignLeft|Qt::AlignTop);
	
		
		scPos.setX(0.75*maxIW);
		scPos.setY(maxIH+0.5*infoH);
		
		scPos0.setX(scPos.x()-scLen*scMul/scale0/2.0);
		scPos0.setY(scPos.y());
		scPos1.setX(scPos.x()+scLen*scMul/scale0/2.0);
		scPos1.setY(scPos.y());
		
		scRect.setRect(maxIW/2.0, maxIH+infoH/2.0, maxIW/2.0, infoH/2.0);
		
		for(i=0; i<ax[2]; i++)
		{
			pos[2] = i;
			resFileName = QString("%1%2.bmp").arg(resPref).arg(i, 2, 10, QLatin1Char('0'));
			baseImg = new QImage(maxIW, maxIH+infoH, QImage::Format_RGB444);
			QPainter painter(baseImg);
			if(!painter.isActive()) 
			{
				qDebug() << "\npainter is\'t active\n"; 
				break;
			}
			painter.fillRect(0, 0, maxIW, maxIH+infoH, baseBrush);
			
			painter.setPen(axesPen);
			painter.drawLine(-dX, 0, -dX, maxIH);
			painter.drawLine(-dX, 0, -dX-5, 25);
			painter.drawLine(-dX, 0, -dX+5, 25);
			painter.drawLine(0, -dY, maxIW, -dY);
			painter.drawLine(maxIW, -dY, maxIW-25, -dY-5);
			painter.drawLine(maxIW, -dY, maxIW-25, -dY+5);
			
			sz = dNums[0];
			
			for(j=0; j<sz; j++)
			{
				painter.setPen(gridPen);
				painter.drawLine(dims[0][j], 0, dims[0][j], maxIH);
				
				painter.setPen(basePen);
				infoStr = QString("%1").arg(dims[0][j]+dX);
                                painter.setFont(QFont("Times", fontSize0, -1, true));
                                axeRect.setRect(dims[0][j]+5, -dY+5, 200, fontSize0+30);
				painter.drawText(axeRect, infoStr, axesOpt);
			}
			painter.setPen(basePen);
			infoStr = QString("%1").arg(dims[0][j]+dX);
                        painter.setFont(QFont("Times", fontSize0, -1, true));
                        axeRect.setRect(dims[0][j]+5, -dY+5, 200, fontSize0+30);
			painter.drawText(axeRect, infoStr, axesOpt);
			
			sz = dNums[1];
			for(j=0; j<sz; j++)
			{
				painter.setPen(gridPen);
				painter.drawLine(0, dims[1][j], maxIW, dims[1][j]);
				
				painter.setPen(basePen);
				infoStr = QString("%1").arg(maxY-dims[1][j]+dY);
                                painter.setFont(QFont("Times", fontSize0, -1, true));
                                axeRect.setRect(-dX+5, dims[1][j]+5, 200, fontSize0+30);
				painter.drawText(axeRect, infoStr, axesOpt);
			}
			
			painter.setPen(basePen);
                        infoStr = QString("M = %1 - %2").arg(dims[2][i], 5, 'f', 2, QLatin1Char(' ')).arg(dims[2][i+1], 5, 'f', 2, QLatin1Char(' '));
                        painter.setFont(QFont("Arial", fontSize1));
			painter.drawText(textRect, infoStr, textOpt);
			
			infoStr = QString("%1 mas").arg((int)scLen);
                        painter.setFont(QFont("Arial", fontSize0));
			painter.drawText(scRect, infoStr, textOpt);
			
			painter.setPen(dataPen);
			painter.drawLine(scPos0, scPos1);
			painter.drawLine(scPos0.x(), scPos0.y()-5, scPos0.x(), scPos0.y()+5);		//left bar
			painter.drawLine(scPos1.x(), scPos1.y()-5, scPos1.x(), scPos1.y()+5);		//right bar
			
			for(j=0; j<ax[1]; j++)
			{
				pos[1] = j;
				for(k=0; k<ax[0]; k++)
				{
					pos[0] = k;
					cpos = xVectField->detPos(pos);
					ocX = xVectField->getD(cpos);
					ocY = yVectField->getD(cpos);
					ndots = vectNumField->getL(cpos);
					pPos.setX((int)((dims[0][k]+dims[0][k+1])/2.0));
					pPos.setY((int)((dims[1][j]+dims[1][j+1])/2.0));
					
					pPos1.setX(pPos.x()+(ocX/scale0)*scMul);
					pPos1.setY(pPos.y()-(ocY/scale1)*scMul);
					painter.setPen(dataPen);
					painter.drawLine(pPos, pPos1);
					
					painter.setPen(pointPen);
					painter.setBrush(pointBrush);
					painter.drawEllipse(pPos, 5, 5);
					
					numRect.setRect(pPos.x(), pPos.y(), (int)((dims[0][k+1]-dims[0][k])/2.0), (int)((dims[1][j+1]-dims[1][j])/2.0));
					infoStr = QString("%1").arg(ndots);
                                        painter.setFont(QFont("Arial", fontSize0));
					painter.setPen(basePen);
					painter.drawText(numRect, infoStr, numOpt);
				}
			}
			
			painter.end();
			baseImg->save(resFileName, "BMP");
		}
	
		break;
		
		case 1:
			maxIH = settings->value("task1/maxIH").toInt();
			maxIW = settings->value("task1/maxIW").toInt();
			infoH = settings->value("task1/infoH").toInt();
			infoW = settings->value("task1/infoW").toInt();
			infoH1 = settings->value("task2/infoH1").toInt();
			
			isDrawGrid = settings->value("task1/isDrawGrid").toInt();
			
			textRect.setRect(infoW, 0, infoW+maxIW, infoH);
			textOpt.setAlignment(Qt::AlignCenter);
			textRectY.setRect(0, 0, infoW, infoH+maxIH);
			textOptY.setAlignment(Qt::AlignCenter);
			numOpt.setAlignment(Qt::AlignRight|Qt::AlignBottom);
			axesOpt.setAlignment(Qt::AlignLeft|Qt::AlignTop);
			
			
			maxV = new double[2];
			minV = new double[2];
			maxV[0] = maxV[1] = -10000;
			minV[0] = minV[1] = 10000;
			
			for(i=0; i<ax[1];i++)
			{
				for(j=0; j<ax[0]; j++)
				{
					pos[0] = j;
					pos[1] = i;
					cpos = xVectField->detPos(pos);
					ocX = xVectField->getD(cpos);
					ocY = yVectField->getD(cpos);
					ndots = vectNumField->getL(cpos);
					if(maxV[0]<ocX) maxV[0] = ocX;
					if(maxV[1]<ocY) maxV[1] = ocY;
					if(minV[0]>ocX) minV[0] = ocX;
					if(minV[1]>ocY) minV[1] = ocY;
				}
			}
			
			stepX = maxIW/(ax[0]*1.0);
			stepY = settings->value("task1/stepY").toDouble();
			
			qDebug() << "\nDrawing\n";
			
			//drawing
			for(k=0; k<2; k++)
			{
				for(i=0; i<ax[1]; i++)
				{
					numTot = 0;
					pos[1] = i;
					resFileName = QString("%1%2_%3.bmp").arg(resPref).arg(k, 2, 10, QLatin1Char('0')).arg(i, 2, 10, QLatin1Char('0'));
					baseImg = new QImage(maxIW+infoW, maxIH+infoH+infoH1, QImage::Format_RGB444);
					QPainter painter(baseImg);
					if(!painter.isActive()) 
					{
						qDebug() << "\npainter is\'t active\n"; 
						break;
					}
					painter.fillRect(0, 0, maxIW+infoW, maxIH+infoH+infoH1, baseBrush);
					
					painter.setPen(axesPen);
					painter.drawLine(infoW, 0, infoW, maxIH+infoH);
					painter.drawLine(infoW, 0, infoW-5, 25);
					painter.drawLine(infoW, 0, infoW+5, 25);
					painter.drawLine(infoW, detT2pos(0, k), infoW+maxIW, detT2pos(0, k));
					painter.drawLine(infoW+maxIW, detT2pos(0, k), infoW+maxIW-25, detT2pos(0, k)-5);
					painter.drawLine(infoW+maxIW, detT2pos(0, k), infoW+maxIW-25, detT2pos(0, k)+5);
					
					sz = ax[0];
					
					for(j=0; j<sz; j++)
					{
						if(isDrawGrid)
						{
							painter.setPen(gridPen);
							painter.drawLine(stepX*j+infoW, infoH, stepX*j+infoW, infoH+maxIH);
						}
						
						painter.setPen(basePen);
						infoStr = QString("%1").arg(dims[0][j]);
                                                painter.setFont(QFont("Times", fontSize0, -1, true));
						axeRect.setRect(stepX*j+infoW, detT2pos(0, k)+5, 50, 40);
						painter.drawText(axeRect, infoStr, axesOpt);
					}
					
					
					if(k)infoStr = QString("OMC(Eta), mas");
					else infoStr = QString("OMC(Ksi), mas");
					
                                        painter.setFont(QFont("Arial", fontSize0));
					painter.drawText(textRectY, infoStr, textOptY);
					
					QPainterPath pPath;
					
					for(j=0; j<sz; j++)
					{
						pos[0] = j;
						
						if(k)
						{
							cpos = yVectField->detPos(pos);
							val = yVectField->getD(cpos);
						}
						else
						{
							cpos = xVectField->detPos(pos);
							val = xVectField->getD(cpos);
						}
						
						
						ndots = vectNumField->getL(cpos);
						if(!ndots) continue;
						pPos.setX(stepX*(j+0.5)+infoW);
						pPos.setY(detT2pos(val, k));
						
						if(j==0) pPath.moveTo(pPos);
						else
						{
							pPath.lineTo(pPos);
							pPath.moveTo(pPos);
						}
					
						painter.setPen(pointPen);
						painter.setBrush(pointBrush);
						painter.drawEllipse(pPos, 5, 5);
						
						
						numRect.setRect(stepX*j+infoW, detT2pos(0.0, k)-20, stepX, 20);
						infoStr = QString("%1").arg(ndots);
                                                painter.setFont(QFont("Arial", fontSize0));
						painter.setPen(basePen);
						painter.drawText(numRect, infoStr, numOpt);
						numTot += ndots;
						
					}
					painter.setPen(pointPen);
					painter.drawPath(pPath);
					painter.setPen(basePen);
					infoStr = QString("Magnitudes, time span = %1 - %2").arg(getStrFromDATEOBS(getDATEOBSfromMJD(dims[1][i]), "-", 0, 0)).arg(getStrFromDATEOBS(getDATEOBSfromMJD(dims[1][i+1]), "-", 0, 0));
                                        painter.setFont(QFont("Arial", fontSize1));
					painter.drawText(textRect, infoStr, textOpt);
					infoStr = QString("\n\nnumTot= %1").arg(numTot);
                                        painter.setFont(QFont("Arial", fontSize0));
					painter.drawText(textRect, infoStr, textOpt);
					
					val = (double)(int)minV[k] + 1;
					do
					{
						if(isDrawGrid)
						{
							painter.setPen(gridPen);
							painter.drawLine(infoW, detT2pos(val, k), maxIW+infoW, detT2pos(val, k));
						}
						if(fabs(val)<1e-4)
						{
							val+=stepY;
							continue;
						}
						painter.setPen(basePen);
						infoStr = QString("%1").arg(val);
                                                painter.setFont(QFont("Times", fontSize0, -1, true));
						axeRect.setRect(infoW+5, detT2pos(val, k)+5, 50, 40);
						painter.drawText(axeRect, infoStr, axesOpt);
						
						val+=stepY;
					}while(val<=maxV[k]);

					painter.end();
					baseImg->save(resFileName, "BMP");
				}
			}
			
		break;
		case 2:
		{
			
		
			maxIH = settings->value("task2/maxIH").toInt();
			maxIW = settings->value("task2/maxIW").toInt();
			infoH = settings->value("task2/infoH").toInt();
			infoH1 = settings->value("task2/infoH1").toInt();
			infoW = settings->value("task2/infoW").toInt();
			
			isDrawGrid = settings->value("task2/isDrawGrid").toInt();
			
			textRect.setRect(infoW, 0, maxIW, infoH);
			textOpt.setAlignment(Qt::AlignCenter);
			textRectY.setRect(0, 0, infoW, infoH+maxIH+infoH1);
			textOptY.setAlignment(Qt::AlignCenter);
			numOpt.setAlignment(Qt::AlignRight|Qt::AlignBottom);
			axesOpt.setAlignment(Qt::AlignLeft|Qt::AlignTop);
			
			
			//drawing
			
					
					
					resFileName = QString("%1.bmp").arg(resPref);
					baseImg = new QImage(maxIW+infoW, maxIH+infoH+infoH1, QImage::Format_RGB444);
					QPainter painter(baseImg);
					if(!painter.isActive()) 
					{
						qDebug() << "\npainter is\'t active\n"; 
						break;
					}
					painter.fillRect(0, 0, maxIW+infoW, maxIH+infoH+infoH1, baseBrush);
					
					painter.setPen(axesPen);
					painter.drawLine(infoW, 0, infoW, maxIH+infoH+infoH1);
					painter.drawLine(infoW, 0, infoW-5, 25);
					painter.drawLine(infoW, 0, infoW+5, 25);
					painter.drawLine(infoW, maxIH+infoH, infoW+maxIW, maxIH+infoH);
					painter.drawLine(infoW+maxIW, maxIH+infoH, infoW+maxIW-25, maxIH+infoH-5);
					painter.drawLine(infoW+maxIW, maxIH+infoH, infoW+maxIW-25, maxIH+infoH+5);
					
					
					stepX = settings->value("task2/stepX").toInt();
					stepY = settings->value("task2/stepY").toInt();
					
					val = stepX*(int)(deMin/stepX);
					do
					{
						if(isDrawGrid)
						{
							painter.setPen(gridPen);
							painter.drawLine(detT3posX(val), infoH, detT3posX(val), detT3posY(ocMin));
						}
						
						painter.setPen(basePen);
						infoStr = QString("%1").arg(val);
                                                painter.setFont(QFont("Times", fontSize0, -1, true));
						axeRect.setRect(detT3posX(val)-10, detT3posY(0)+5, 70, 40);
						painter.drawText(axeRect, infoStr, axesOpt);
						
						val+=stepX;
					}while(val<=deMax);
					
					val = deMin;
					if(isDrawGrid)
					{
						painter.setPen(gridPen);
						painter.drawLine(detT3posX(val), infoH, detT3posX(val), detT3posY(ocMin));
					}
					
					painter.setPen(basePen);
					infoStr = QString("%1").arg(val, 6, 'f', 2, QLatin1Char(' '));
                                        painter.setFont(QFont("Times", fontSize0, -1, true));
					axeRect.setRect(detT3posX(val)+5, detT3posY(0)+5, 50, 40);
					painter.drawText(axeRect, infoStr, axesOpt);
					
					val = deMax;
					if(isDrawGrid)
					{
						painter.setPen(gridPen);
						painter.drawLine(detT3posX(val), infoH, detT3posX(val), detT3posY(ocMin));
					}
					
					painter.setPen(basePen);
					infoStr = QString("%1").arg(val, 6, 'f', 2, QLatin1Char(' '));
                                        painter.setFont(QFont("Times", fontSize0, -1, true));
					axeRect.setRect(detT3posX(val)+5, detT3posY(0)+5, 50, 40);
					painter.drawText(axeRect, infoStr, axesOpt);
					
					axesOpt.setAlignment(Qt::AlignRight|Qt::AlignCenter);
					val = stepY*(int)(ocMax/stepY);
					
					qDebug() << "\nocMax" << ocMax << "\n";
					qDebug() << "\nocMin" << ocMin << "\n";
					
					do
					{
						qDebug() << "\n" << val << "\n";
						if(isDrawGrid)
						{
							painter.setPen(gridPen);
							painter.drawLine(infoW, detT3posY(val), detT3posX(deMax), detT3posY(val));
						}
						
						painter.setPen(basePen);
						infoStr = QString("%1").arg(val);
                                                painter.setFont(QFont("Times", fontSize0, -1, true));
						axeRect.setRect(0, detT3posY(val)-10, infoW, 20);
						
						painter.drawText(axeRect, infoStr, axesOpt);
						
						val-=stepY;
					}while(val>=ocMin);
					
//					painter.setWorldMatrix(QMatrix().rotate(90));
					infoStr = QString("OMC, mas");
					
                                        painter.setFont(QFont("Arial", fontSize0));
					painter.drawText(textRectY, infoStr, textOptY);
//					painter.setWorldMatrix(QMatrix().rotate(-90));
					
					numTot = 0;
					sz = deList.size();
					for(j=0; j<sz; j++)
					{
						
						pPos.setX(detT3posX(deList[j][0]));
						pPos.setY(detT3posY(deList[j][1]));
					
						painter.setPen(pointPen);
						painter.setBrush(pointBrush);
						painter.drawEllipse(pPos, 1, 1);
						
						numTot++;
					}
					
					painter.setPen(basePen);
					infoStr = QString("OMC/DEC");
                                        painter.setFont(QFont("Arial", fontSize1));
					painter.drawText(textRect, infoStr, textOpt);
					infoStr = QString("\n\nnumTot= %1").arg(numTot);
                                        painter.setFont(QFont("Arial", fontSize0));
					painter.drawText(textRect, infoStr, textOpt);
					
					painter.setPen(basePen);
					infoStr = QString("DEC");
                                        painter.setFont(QFont("Arial", fontSize0));
					axeRect.setRect(infoW, infoH+maxIH+infoH/2.0, maxIW, infoH/2.0);
					painter.drawText(axeRect, infoStr, textOpt);


					painter.end();
					baseImg->save(resFileName, "BMP");
		
		}
		break;
		case 3:
		{
			
		
			maxIH = settings->value("task3/maxIH").toInt();
			maxIW = settings->value("task3/maxIW").toInt();
			infoH = settings->value("task3/infoH").toInt();
			infoH1 = settings->value("task3/infoH1").toInt();
			infoW = settings->value("task3/infoW").toInt();
			
			
			int isDetTrend = settings->value("task3/isDetTrend").toInt();
/*			if(isDetTrend)
			{ 
				double *cm = new double[sz*2];
				double *Z = new double[2];
				double *L = new double[sz];
				double *we = new double[sz];
			}
	*/		
			isDrawGrid = settings->value("task3/isDrawGrid").toInt();
			stepY = settings->value("task3/stepY").toInt();
			int stx = settings->value("task3/stx").toInt();
			stepX = maxIW/sz;
			
			textRect.setRect(infoW, 0, maxIW, infoH-20);
			textOpt.setAlignment(Qt::AlignCenter|Qt::AlignTop);
			textRectY.setRect(0, 0, infoW, infoH);
			textOptY.setAlignment(Qt::AlignCenter);
			numOpt.setAlignment(Qt::AlignCenter|Qt::AlignBottom);
			axesOpt.setAlignment(Qt::AlignLeft|Qt::AlignTop);
			
			numTot = 0;
			//drawing
			for(k=0; k<2; k++)
			{
					if(isDetTrend)
					{
						cm = new double[sz*2];
						Z = new double[2];
						L = new double[sz];
						we = new double[sz];
						sz1 = 0;
						for(i=0; i<sz-1; i++)
						{
							if(numsList[i])
							{
								qDebug() << "i="<<i << "\n";
								if(k) L[sz1] = deVectY[i];
								else L[sz1] = deVectX[i];
								qDebug() << "Li="<< L[sz1] << "\n";
								cm[2*sz1] = dimN[i];
								cm[2*sz1+1] = 1;
								we[sz1] = 1.0;
								sz1++;
							}
						}
						slsm(2, sz1, Z, cm, L, we);
						
						qDebug() << "k=" << Z[0] << "\tb=" << Z[1];
				 	}
			
					qDebug() << maxIW+infoW << "x" << maxIH+infoH+infoH1 << "\n";
					
					resFileName = QString("%1_%2.bmp").arg(resPref).arg(k, 2, 10, QLatin1Char(' '));
					baseImg = new QImage(maxIW+infoW, maxIH+infoH+infoH1, QImage::Format_RGB444);
					QPainter painter(baseImg);
					if(!painter.isActive()) 
					{
						qDebug() << "\npainter is\'t active\n"; 
						break;
					}
					painter.fillRect(0, 0, maxIW+infoW, maxIH+infoH+infoH1, baseBrush);
					
					axeXpos = detT2pos(0.0, k);
					if((axeXpos<infoH)||(axeXpos>infoH+maxIH)) axeXpos = detT2pos(minV[k], k);
					
					painter.setPen(axesPen);
					painter.drawLine(infoW, 0, infoW, maxIH+infoH+infoH1);
					painter.drawLine(infoW, 0, infoW-5, 25);
					painter.drawLine(infoW, 0, infoW+5, 25);
					painter.drawLine(infoW, axeXpos, infoW+maxIW, axeXpos);
					painter.drawLine(infoW+maxIW, axeXpos, infoW+maxIW-25, axeXpos-5);
					painter.drawLine(infoW+maxIW, axeXpos, infoW+maxIW-25, axeXpos+5);
					
					
//					stepX = settings->value("task2/stepX").toInt();
					for(i=0; i<sz; i+=stx)
					{
						
						val = infoW+ stepX*i;
						if(isDrawGrid)
						{
							painter.setPen(gridPen);
							painter.drawLine(val, infoH, val, detT2pos(minV[k], k));
						}
						
						painter.setPen(basePen);
						infoStr = QString("%1").arg(dimN[i], 5, 'f', 1, QLatin1Char(' '));
						painter.setFont(QFont("Times", 20, -1, true));
//						axeRect.setRect(val-10, detT2pos(minV[k], k)+5, 70, 40);
						axeRect.setRect(val-10, axeXpos+5, 70, 40);
						painter.drawText(axeRect, infoStr, axesOpt);
						
						
					}
					
					
					
					axesOpt.setAlignment(Qt::AlignRight|Qt::AlignCenter);
//					val = maxV[k];
					val = (((int)(maxV[k])/(int)stepY)+1)*stepY;
					qDebug() << "\nocMax" << maxV[k] << "\n";
					qDebug() << "\nocMin" << minV[k] << "\n";
					qDebug() << "\nval" << val << "\n";
					
					
					do
					{
						qDebug() << "\n" << val << "\n";
						if(isDrawGrid)
						{
							painter.setPen(gridPen);
							painter.drawLine(infoW, detT2pos(val, k), infoW+maxIW, detT2pos(val, k));
						}
						
						painter.setPen(basePen);
						infoStr = QString("%1").arg(val);
						painter.setFont(QFont("Times", 20, -1, true));
						axeRect.setRect(0, detT2pos(val, k)-10, infoW, 20);
						
						painter.drawText(axeRect, infoStr, axesOpt);
						
						val-=stepY;
					}while(val>=minV[k]);
					
//					painter.setWorldMatrix(QMatrix().rotate(90));
					if(k)infoStr = QString("OMC(eta), mas");
					else infoStr = QString("OMC(ksi), mas");
					
					painter.setFont(QFont("Arial", 20));
					painter.drawText(textRectY, infoStr, textOptY);
//					painter.setWorldMatrix(QMatrix().rotate(-90));
					
					
					p0 = p1 = -1;
					ndots=0;
					for(j=0; j<sz-1; j++)
					{
						if(numsList[j])
						{
							pPos.setX(infoW+ stepX*(j+0.5));
							if(k) pPos.setY(detT2pos(deVectY[j], k));
							else pPos.setY(detT2pos(deVectX[j], k));
							
							qDebug() << j << ": "<< "X= " << pPos.x() << " Y= " << pPos.y() << "\tnums=" << numsList[j] << "\n";
						}
//						qDebug() << j << ": "<< "deX= " << deVectX[j] << " deY= " << deVectY[j] << "\n";
					
						painter.setPen(pointPen);
						painter.setBrush(pointBrush);
						painter.drawEllipse(pPos, 5, 5);
					
					
						ndots += numsList[j];
						p1 = j/stx;
						if(p1-p0)
						{
							numRect.setRect(stepX*j+infoW, axeXpos-20, stepX*stx, 20);
							infoStr = QString("%1").arg(ndots);
							painter.setFont(QFont("Arial", 20));
							painter.setPen(basePen);
							painter.drawText(numRect, infoStr, numOpt);
							if(!k) numTot += ndots;
							ndots=0;
							p0=p1;
						}
						
						
					}
					
					if(isDetTrend)
					{
						pPos.setX(infoW+ stepX*0.5);
						pPos.setY(detT2pos(dimN[0]*Z[0]+Z[1], k));
						pPos1.setX(infoW+ stepX*(sz-0.5));
						pPos1.setY(detT2pos(dimN[sz-1]*Z[0]+Z[1], k));
						qDebug() << "trend (" << pPos.x() << ", " << pPos.y() << "):(" << pPos1.x() << ", " << pPos1.y() << ")\n";
						painter.setPen(trendPen);
						painter.drawLine(pPos, pPos1);
					}
					
					painter.setPen(basePen);
					if(k) infoStr = QString("OMC(eta)/DEC");
					else infoStr = QString("OMC(ksi)/DEC");
					painter.setFont(QFont("Arial", 40));
					painter.drawText(textRect, infoStr, textOpt);
					infoStr = QString("\n\nnumTot= %1").arg(numTot);
					painter.setFont(QFont("Arial", 30));
					painter.drawText(textRect, infoStr, textOpt);
					
					painter.setPen(basePen);
					infoStr = QString("DEC, degree");
					painter.setFont(QFont("Arial", 30));
					axeRect.setRect(infoW, infoH+maxIH+infoH/2.0, maxIW, infoH/2.0);
					painter.drawText(axeRect, infoStr, textOpt);


					painter.end();
					baseImg->save(resFileName, "BMP");
			}
		}
		break;
		case 4:
		{
			
		
			maxIH = settings->value("task4/maxIH").toInt();
			maxIW = settings->value("task4/maxIW").toInt();
			infoH = settings->value("task4/infoH").toInt();
			infoH1 = settings->value("task4/infoH1").toInt();
			infoW = settings->value("task4/infoW").toInt();
			
			isDrawGrid = settings->value("task4/isDrawGrid").toInt();
			int isDetTrend = settings->value("task4/isDetTrend").toInt();
			
			textRect.setRect(infoW, 0, maxIW, infoH);
			textOpt.setAlignment(Qt::AlignCenter);
			textRectY.setRect(0, 0, infoW, infoH);
			textOptY.setAlignment(Qt::AlignCenter);
			numOpt.setAlignment(Qt::AlignRight|Qt::AlignBottom);
			axesOpt.setAlignment(Qt::AlignLeft|Qt::AlignTop);
			
			qDebug() << "\ndeMin" << deMin << "\n";
			qDebug() << "\ndeMax" << deMax << "\n";
			//drawing
			sz = deList.size();
			for(k=0; k<3;k++)
			{
					if(isDetTrend)
					{
						cm = new double[sz*2];
						Z = new double[2];
						L = new double[sz];
						we = new double[sz];
						
						for(i=0; i<sz-1; i++)
						{
							qDebug() << "i="<<i << "\n";
							if(k==1) val = deList[i][2];
							if(k==0) val = deList[i][1];
							if(k==2) val = deList[i][3];
							L[i] = val;
							
							qDebug() << "Li="<< L[i] << "\n";
							cm[2*i] = deList[i][0];
							cm[2*i+1] = 1;
							we[i] = 1.0;
						}
						slsm(2, sz-1, Z, cm, L, we);
						
						qDebug() << "k=" << Z[0] << "\tb=" << Z[1];
				 	}
			
					resFileName = QString("%1_%2.bmp").arg(resPref).arg(k, 2, 10, QLatin1Char(' '));
					baseImg = new QImage(maxIW+infoW, maxIH+infoH+infoH1, QImage::Format_RGB444);
					QPainter painter(baseImg);
					if(!painter.isActive()) 
					{
						qDebug() << "\npainter is\'t active\n"; 
						break;
					}
					painter.fillRect(0, 0, maxIW+infoW, maxIH+infoH+infoH1, baseBrush);
					
					painter.setPen(axesPen);
					painter.drawLine(infoW, 0, infoW, maxIH+infoH+infoH1);
					painter.drawLine(infoW, 0, infoW-5, 25);
					painter.drawLine(infoW, 0, infoW+5, 25);
					val = minV[k];
					painter.drawLine(infoW, detT2pos(val, k), infoW+maxIW, detT2pos(val, k));
					painter.drawLine(infoW+maxIW, detT2pos(val, k), infoW+maxIW-25, detT2pos(val, k)-5);
					painter.drawLine(infoW+maxIW, detT2pos(val, k), infoW+maxIW-25, detT2pos(val, k)+5);
					
					
					stepX = settings->value("task4/stepX").toInt();
					stepY = settings->value("task4/stepY").toInt();
					
					val = (int) deMin;
					do
					{
						if(isDrawGrid)
						{
							painter.setPen(gridPen);
							painter.drawLine(detT3posX(val), infoH, detT3posX(val), detT2pos(minV[k], k));
						}
						
						painter.setPen(basePen);
						infoStr = QString("%1").arg(val);
						painter.setFont(QFont("Times", 20, -1, true));
						axeRect.setRect(detT3posX(val)-10, detT2pos(minV[k], k)+5, 70, 40);
						painter.drawText(axeRect, infoStr, axesOpt);
						
						val+=stepX;
					}while(val<=(int)deMax+1);
					
					
					
					painter.setPen(basePen);
					infoStr = QString("%1").arg(val, 6, 'f', 2, QLatin1Char(' '));
					painter.setFont(QFont("Times", 20, -1, true));
					axeRect.setRect(detT3posX(val)+5, detT2pos(0, k)+5, 50, 40);
					painter.drawText(axeRect, infoStr, axesOpt);
					
					val = deMax;
					if(isDrawGrid)
					{
						painter.setPen(gridPen);
						painter.drawLine(detT3posX(val), infoH, detT3posX(val), detT2pos(minV[k], k));
					}
					
					painter.setPen(basePen);
					infoStr = QString("%1").arg(val, 6, 'f', 2, QLatin1Char(' '));
					painter.setFont(QFont("Times", 20, -1, true));
					axeRect.setRect(detT3posX(val)+5, detT3posY(0)+5, 50, 40);
					painter.drawText(axeRect, infoStr, axesOpt);
					
					axesOpt.setAlignment(Qt::AlignRight|Qt::AlignCenter);
//					val = (((int)maxV[k])/stepY)+1)*stepY;
					val = (((int)(maxV[k])/(int)stepY)+1)*stepY;
					
					qDebug() << "\nocMax" << maxV[k] << "\n";
					qDebug() << "\nocMin" << minV[k] << "\n";
					
					do
					{
						qDebug() << "\n" << val << "\n";
						if(isDrawGrid)
						{
							painter.setPen(gridPen);
							painter.drawLine(infoW, detT2pos(val, k), detT3posX(deMax), detT2pos(val, k));
						}
						
						painter.setPen(basePen);
						infoStr = QString("%1").arg(val);
						painter.setFont(QFont("Times", 20, -1, true));
						axeRect.setRect(0, detT2pos(val, k)-10, infoW, 20);
						
						painter.drawText(axeRect, infoStr, axesOpt);
						
						val-=stepY;
					}while(val>=minV[k]);
					
//					painter.setWorldMatrix(QMatrix().rotate(90));
					infoStr = QString("OMC, mas");
					
					painter.setFont(QFont("Arial", 20));
					painter.drawText(textRectY, infoStr, textOptY);
//					painter.setWorldMatrix(QMatrix().rotate(-90));
					
					numTot = 0;
					sz = deList.size();
					for(j=0; j<sz; j++)
					{
						
						if(k==1) val = deList[j][2];
						if(k==0) val = deList[j][1];
						if(k==2) val = deList[j][3];
						pPos.setX(detT3posX(deList[j][0]));
						pPos.setY(detT2pos(val, k));
					
						painter.setPen(pointPen);
						painter.setBrush(pointBrush);
						painter.drawEllipse(pPos, 5, 5);
						
						numTot++;
					}
					
					if(isDetTrend)
					{
						pPos.setX(detT3posX(deMin));
/*						if(k==1) val = deList[0][2];
						if(k==0) val = deList[0][1];
						if(k==2) val = deList[0][3];*/
						pPos.setY(detT2pos(deMin*Z[0]+Z[1], k));
/*						if(k==1) val = deList[sz-1][2];
						if(k==0) val = deList[sz-1][1];
						if(k==2) val = deList[sz-1][3];*/
						pPos1.setX(detT3posX(deMax));
						pPos1.setY(detT2pos(deMax*Z[0]+Z[1], k));
						qDebug() << "trend (" << pPos.x() << ", " << pPos.y() << "):(" << pPos1.x() << ", " << pPos1.y() << ")\n";
						painter.setPen(trendPen);
						painter.drawLine(pPos, pPos1);
					}
					
					painter.setPen(basePen);
					if(k==1) infoStr = QString("OMC(DEC)/Phase");
					if(k==0) infoStr = QString("OMC(RA)/Phase");
					if(k==2) infoStr = QString("OMC(RA&DEC)/Phase");
					painter.setFont(QFont("Arial", 40));
					painter.drawText(textRect, infoStr, textOpt);
					infoStr = QString("\n\nnumTot= %1").arg(numTot);
					painter.setFont(QFont("Arial", 30));
					painter.drawText(textRect, infoStr, textOpt);
					
					painter.setPen(basePen);
					infoStr = QString("Phase, degree");
					painter.setFont(QFont("Arial", 30));
					axeRect.setRect(infoW, infoH+maxIH+infoH/2.0, maxIW, infoH/2.0);
					painter.drawText(axeRect, infoStr, textOpt);


					painter.end();
					baseImg->save(resFileName, "BMP");
			}
		}
		break;
		case 5:					//УБ для астероидов с осреденением по диапазонам и трендом
		{
			
		
			maxIH = settings->value("task5/maxIH").toInt();
			maxIW = settings->value("task5/maxIW").toInt();
			infoH = settings->value("task5/infoH").toInt();
			infoH1 = settings->value("task5/infoH1").toInt();
			infoW = settings->value("task5/infoW").toInt();
			
			
			int isDetTrend = settings->value("task5/isDetTrend").toInt();
/*			if(isDetTrend)
			{ 
				double *cm = new double[sz*2];
				double *Z = new double[2];
				double *L = new double[sz];
				double *we = new double[sz];
			}
	*/		
			isDrawGrid = settings->value("task5/isDrawGrid").toInt();
			stepY = settings->value("task5/stepY").toInt();
			int stx = settings->value("task5/stx").toInt();
			stepX = maxIW/sz;
			
			textRect.setRect(infoW, 0, maxIW, infoH-50);
			textOpt.setAlignment(Qt::AlignCenter|Qt::AlignTop);
			textRectY.setRect(0, 0, infoW, infoH);
			textOptY.setAlignment(Qt::AlignCenter);
			numOpt.setAlignment(Qt::AlignCenter|Qt::AlignBottom);
			axesOpt.setAlignment(Qt::AlignLeft|Qt::AlignTop);
			
			numTot = 0;
			//drawing
			for(k=0; k<2; k++)
			{
				if(isDetTrend)
				{
					cm = new double[sz*2];
					Z = new double[2];
					L = new double[sz];
					we = new double[sz];
					sz1 = 0;
					if(numsList[i])
					{
						qDebug() << "i="<<i << "\n";
						if(k) L[sz1] = deVectY[i];
						else L[sz1] = deVectX[i];
						qDebug() << "Li="<< L[sz1] << "\n";
						cm[2*sz1] = dimN[i];
						cm[2*sz1+1] = 1;
						we[sz1] = 1.0;
						sz1++;
					}
					slsm(2, sz-1, Z, cm, L, we);
					
					qDebug() << "k=" << Z[0] << "\tb=" << Z[1];
			 	}
			
					qDebug() << maxIW+infoW << "x" << maxIH+infoH+infoH1 << "\n";
					
					resFileName = QString("%1_%2.bmp").arg(resPref).arg(k, 2, 10, QLatin1Char(' '));
					baseImg = new QImage(maxIW+infoW, maxIH+infoH+infoH1, QImage::Format_RGB444);
					QPainter painter(baseImg);
					if(!painter.isActive()) 
					{
						qDebug() << "\npainter is\'t active\n"; 
						break;
					}
					painter.fillRect(0, 0, maxIW+infoW, maxIH+infoH+infoH1, baseBrush);
					
					painter.setPen(axesPen);
					painter.drawLine(infoW, 0, infoW, maxIH+infoH+infoH1);
					painter.drawLine(infoW, 0, infoW-5, 25);
					painter.drawLine(infoW, 0, infoW+5, 25);
					painter.drawLine(infoW, detT2pos(0.0, k), infoW+maxIW, detT2pos(0.0, k));
					painter.drawLine(infoW+maxIW, detT2pos(0.0, k), infoW+maxIW-25, detT2pos(0.0, k)-5);
					painter.drawLine(infoW+maxIW, detT2pos(0.0, k), infoW+maxIW-25, detT2pos(0.0, k)+5);
					
					
//					stepX = settings->value("task5/stepX").toInt();
					for(i=0; i<sz; i+=stx)
					{
						
						val = infoW+ stepX*i;
						if(isDrawGrid)
						{
							painter.setPen(gridPen);
							painter.drawLine(val, infoH, val, detT2pos(minV[k], k));
						}
						
						painter.setPen(basePen);
						infoStr = QString("%1").arg(dimN[i], 5, 'f', 1, QLatin1Char(' '));
						painter.setFont(QFont("Times", 20, -1, true));
//						axeRect.setRect(val-10, detT2pos(minV[k], k)+5, 70, 40);
						axeRect.setRect(val-10, detT2pos(0.0, k)+5, 70, 40);
						painter.drawText(axeRect, infoStr, axesOpt);
						
						
					}
					
					
					
					axesOpt.setAlignment(Qt::AlignRight|Qt::AlignCenter);
//					val = maxV[k];
					val = (((int)(maxV[k])/(int)stepY)+1)*stepY;
					qDebug() << "\nocMax" << maxV[k] << "\n";
					qDebug() << "\nocMin" << minV[k] << "\n";
					qDebug() << "\nval" << val << "\n";
					
					
					do
					{
						qDebug() << "\n" << val << "\n";
						if(isDrawGrid)
						{
							painter.setPen(gridPen);
							painter.drawLine(infoW, detT2pos(val, k), infoW+maxIW, detT2pos(val, k));
						}
						
						painter.setPen(basePen);
						infoStr = QString("%1").arg(val);
						painter.setFont(QFont("Times", 20, -1, true));
						axeRect.setRect(0, detT2pos(val, k)-10, infoW, 20);
						
						painter.drawText(axeRect, infoStr, axesOpt);
						
						val-=stepY;
					}while(val>=minV[k]);
					
//					painter.setWorldMatrix(QMatrix().rotate(90));
					if(k)infoStr = QString("OMC(dec), mas");
					else infoStr = QString("OMC(ra), mas");
					
					painter.setFont(QFont("Arial", 20));
					painter.drawText(textRectY, infoStr, textOptY);
//					painter.setWorldMatrix(QMatrix().rotate(-90));
					
					
					p0 = p1 = -1;
					ndots=0;
					for(j=0; j<sz-1; j++)
					{
						if(numsList[j])
						{
							pPos.setX(infoW+ stepX*(j+0.5));
							if(k) pPos.setY(detT2pos(deVectY[j], k));
							else pPos.setY(detT2pos(deVectX[j], k));
						
							qDebug() << j << ": "<< "X= " << pPos.x() << " Y= " << pPos.y() << "\tnums=" << numsList[j] << "\n";
						}
//						qDebug() << j << ": "<< "deX= " << deVectX[j] << " deY= " << deVectY[j] << "\n";
					
						painter.setPen(pointPen);
						painter.setBrush(pointBrush);
						painter.drawEllipse(pPos, 5, 5);
					
					
						ndots += numsList[j];
						p1 = j/stx;
						if(p1-p0)
						{
							numRect.setRect(stepX*j+infoW, detT2pos(0.0, k)-20, stepX*stx, 20);
							infoStr = QString("%1").arg(ndots);
							painter.setFont(QFont("Arial", 20));
							painter.setPen(basePen);
							painter.drawText(numRect, infoStr, numOpt);
							if(!k) numTot += ndots;
							ndots=0;
							p0=p1;
						}
						
						
					}
					
					if(isDetTrend)
					{
						pPos.setX(infoW+ stepX*0.5);
						pPos.setY(detT2pos(dimN[0]*Z[0]+Z[1], k));
						pPos1.setX(infoW+ stepX*(sz-0.5));
						pPos1.setY(detT2pos(dimN[sz-1]*Z[0]+Z[1], k));
						qDebug() << "trend (" << pPos.x() << ", " << pPos.y() << "):(" << pPos1.x() << ", " << pPos1.y() << ")\n";
						painter.setPen(trendPen);
						painter.drawLine(pPos, pPos1);
					}
					
					painter.setPen(basePen);
					if(k) infoStr = QString("OMC(dec)/Phase");
					else infoStr = QString("OMC(ra)/Phase");
					painter.setFont(QFont("Arial", 40));
					painter.drawText(textRect, infoStr, textOpt);
					infoStr = QString("\n\nnumTot= %1").arg(numTot);
					painter.setFont(QFont("Arial", 30));
					painter.drawText(textRect, infoStr, textOpt);
					
					painter.setPen(basePen);
					infoStr = QString("Phase");
					painter.setFont(QFont("Arial", 30));
					axeRect.setRect(infoW, infoH+maxIH+infoH/2.0, maxIW, infoH/2.0);
					painter.drawText(axeRect, infoStr, textOpt);


					painter.end();
					baseImg->save(resFileName, "BMP");
			}
		}
		break;
		case 6:
			maxIH = settings->value("task6/maxIH").toInt();
			maxIW = settings->value("task6/maxIW").toInt();
			infoH = settings->value("task6/infoH").toInt();
			infoW = settings->value("task6/infoW").toInt();
			infoH1 = settings->value("task6/infoH1").toInt();
			
			isDrawGrid = settings->value("task6/isDrawGrid").toInt();
			
			textRect.setRect(infoW, 0, infoW+maxIW, infoH);
			textOpt.setAlignment(Qt::AlignCenter);
			textRectY.setRect(0, 0, infoW, infoH+maxIH);
			textOptY.setAlignment(Qt::AlignCenter);
			numOpt.setAlignment(Qt::AlignRight|Qt::AlignBottom);
			axesOpt.setAlignment(Qt::AlignLeft|Qt::AlignTop);
			
			
			maxV = new double[2];
			minV = new double[2];
			maxV[0] = maxV[1] = -10000;
			minV[0] = minV[1] = 10000;
			
			for(i=0; i<ax[1];i++)
			{
				for(j=0; j<ax[0]; j++)
				{
					pos[0] = j;
					pos[1] = i;
					cpos = xVectField->detPos(pos);
					ocX = xVectField->getD(cpos);
					ocY = yVectField->getD(cpos);
					ndots = vectNumField->getL(cpos);
					if(maxV[0]<ocX) maxV[0] = ocX;
					if(maxV[1]<ocY) maxV[1] = ocY;
					if(minV[0]>ocX) minV[0] = ocX;
					if(minV[1]>ocY) minV[1] = ocY;
				}
			}
			
			stepX = maxIW/(ax[0]*1.0);
			stepY = settings->value("task6/stepY").toDouble();
			
			qDebug() << "\nDrawing\n";
			
			//drawing
			for(k=0; k<2; k++)
			{
				for(i=0; i<ax[1]; i++)
				{
					numTot = 0;
					pos[1] = i;
					resFileName = QString("%1%2_%3.bmp").arg(resPref).arg(k, 2, 10, QLatin1Char('0')).arg(i, 2, 10, QLatin1Char('0'));
					baseImg = new QImage(maxIW+infoW, maxIH+infoH+infoH1, QImage::Format_RGB444);
					QPainter painter(baseImg);
					if(!painter.isActive()) 
					{
						qDebug() << "\npainter is\'t active\n"; 
						break;
					}
					painter.fillRect(0, 0, maxIW+infoW, maxIH+infoH+infoH1, baseBrush);
					
					painter.setPen(axesPen);
					painter.drawLine(infoW, 0, infoW, maxIH+infoH);
					painter.drawLine(infoW, 0, infoW-5, 25);
					painter.drawLine(infoW, 0, infoW+5, 25);
					painter.drawLine(infoW, detT2pos(0, k), infoW+maxIW, detT2pos(0, k));
					painter.drawLine(infoW+maxIW, detT2pos(0, k), infoW+maxIW-25, detT2pos(0, k)-5);
					painter.drawLine(infoW+maxIW, detT2pos(0, k), infoW+maxIW-25, detT2pos(0, k)+5);
					
					sz = ax[0];
					
					for(j=0; j<sz; j++)
					{
						if(isDrawGrid)
						{
							painter.setPen(gridPen);
							painter.drawLine(stepX*j+infoW, infoH, stepX*j+infoW, infoH+maxIH);
						}
						
						painter.setPen(basePen);
						infoStr = QString("%1").arg(dims[0][j], 5, 'f', 1);
						painter.setFont(QFont("Times", 20, -1, true));
						axeRect.setRect(stepX*j+infoW, detT2pos(0, k)+5, 50, 40);
						painter.drawText(axeRect, infoStr, axesOpt);
					}
					
					
					if(k)infoStr = QString("OMC(Eta), mas");
					else infoStr = QString("OMC(Ksi), mas");
					
					painter.setFont(QFont("Arial", 20));
					painter.drawText(textRectY, infoStr, textOptY);
					
					QPainterPath pPath;
					
					for(j=0; j<sz; j++)
					{
						pos[0] = j;
						
						if(k)
						{
							cpos = yVectField->detPos(pos);
							val = yVectField->getD(cpos);
						}
						else
						{
							cpos = xVectField->detPos(pos);
							val = xVectField->getD(cpos);
						}
						
						
						ndots = vectNumField->getL(cpos);
						
						pPos.setX(stepX*(j+0.5)+infoW);
						pPos.setY(detT2pos(val, k));
						if(!ndots)
						{
							pPath.moveTo(pPos);
							continue;
						}
						if(j==0) pPath.moveTo(pPos);
						else
						{
							pPath.lineTo(pPos);
							pPath.moveTo(pPos);
						}
					
						painter.setPen(pointPen);
						painter.setBrush(pointBrush);
						painter.drawEllipse(pPos, 5, 5);
						
						
						numRect.setRect(stepX*j+infoW, detT2pos(0.0, k)-20, stepX, 20);
						infoStr = QString("%1").arg(ndots);
						painter.setFont(QFont("Arial", 20));
						painter.setPen(basePen);
						painter.drawText(numRect, infoStr, numOpt);
						numTot += ndots;
						
					}
					painter.setPen(pointPen);
					painter.drawPath(pPath);
					painter.setPen(basePen);
					infoStr = QString("(u2R-H)-color, Dec diap = %1 - %2").arg(dims[1][i], 5, 'f', 1, QLatin1Char(' ')).arg(dims[1][i+1], 5, 'f', 1, QLatin1Char(' '));
					painter.setFont(QFont("Arial", 40));
					painter.drawText(textRect, infoStr, textOpt);
					infoStr = QString("\n\nnumTot= %1").arg(numTot);
					painter.setFont(QFont("Arial", 30));
					painter.drawText(textRect, infoStr, textOpt);
					
					val = (double)(int)minV[k] - 1;
//					val = (((int)(maxV[k])/(int)stepY)+1)*stepY;
					do
					{
						if(isDrawGrid)
						{
							painter.setPen(gridPen);
							painter.drawLine(infoW, detT2pos(val, k), maxIW+infoW, detT2pos(val, k));
						}
						if(fabs(val)<1e-4) continue;
						painter.setPen(basePen);
						infoStr = QString("%1").arg(val);
						painter.setFont(QFont("Times", 20, -1, true));
						axeRect.setRect(infoW+5, detT2pos(val, k)+5, 50, 40);
						painter.drawText(axeRect, infoStr, axesOpt);
						
						val+=stepY;
					}while(val<=maxV[k]);

					painter.end();
					baseImg->save(resFileName, "BMP");
				}
			}
			
		break;
		case 7:					//УБ для астероидов с осреденением по диапазонам и трендом
		{
			
		
			maxIH = settings->value("task7/maxIH").toInt();
			maxIW = settings->value("task7/maxIW").toInt();
			infoH = settings->value("task7/infoH").toInt();
			infoH1 = settings->value("task7/infoH1").toInt();
			infoW = settings->value("task7/infoW").toInt();
			
			
			int isDetTrend = settings->value("task7/isDetTrend").toInt();
/*			if(isDetTrend)
			{ 
				double *cm = new double[sz*2];
				double *Z = new double[2];
				double *L = new double[sz];
				double *we = new double[sz];
			}
	*/		
			isDrawGrid = settings->value("task7/isDrawGrid").toInt();
			stepY = settings->value("task7/stepY").toInt();
			int stx = settings->value("task7/stx").toInt();
			stepX = maxIW/sz;
			
			textRect.setRect(infoW, 0, maxIW, infoH);
			textOpt.setAlignment(Qt::AlignCenter);
			textRectY.setRect(0, 0, infoW, infoH);
			textOptY.setAlignment(Qt::AlignCenter);
			numOpt.setAlignment(Qt::AlignCenter|Qt::AlignBottom);
			axesOpt.setAlignment(Qt::AlignLeft|Qt::AlignTop);
			
			numTot = 0;
			//drawing
			for(k=0; k<2; k++)
			{
				if(isDetTrend)
				{
					cm = new double[sz*2];
					Z = new double[2];
					L = new double[sz];
					we = new double[sz];
					
					for(i=0; i<sz-1; i++)
					{
						qDebug() << "i="<<i << "\n";
						if(k) L[i] = deVectY[i];
						else L[i] = deVectX[i];
						qDebug() << "Li="<< L[i] << "\n";
						cm[2*i] = dimN[i];
						cm[2*i+1] = 1;
						we[i] = 1.0;
					}
					slsm(2, sz-1, Z, cm, L, we);
					
					qDebug() << "k=" << Z[0] << "\tb=" << Z[1];
			 	}
			
					qDebug() << maxIW+infoW << "x" << maxIH+infoH+infoH1 << "\n";
					
					resFileName = QString("%1_%2.bmp").arg(resPref).arg(k, 2, 10, QLatin1Char(' '));
					baseImg = new QImage(maxIW+infoW, maxIH+infoH+infoH1, QImage::Format_RGB444);
					QPainter painter(baseImg);
					if(!painter.isActive()) 
					{
						qDebug() << "\npainter is\'t active\n"; 
						break;
					}
					painter.fillRect(0, 0, maxIW+infoW, maxIH+infoH+infoH1, baseBrush);
					
					painter.setPen(axesPen);
					painter.drawLine(infoW, 0, infoW, maxIH+infoH+infoH1);
					painter.drawLine(infoW, 0, infoW-5, 25);
					painter.drawLine(infoW, 0, infoW+5, 25);
					painter.drawLine(infoW, detT2pos(0.0, k), infoW+maxIW, detT2pos(0.0, k));
					painter.drawLine(infoW+maxIW, detT2pos(0.0, k), infoW+maxIW-25, detT2pos(0.0, k)-5);
					painter.drawLine(infoW+maxIW, detT2pos(0.0, k), infoW+maxIW-25, detT2pos(0.0, k)+5);
					
					
//					stepX = settings->value("task7/stepX").toInt();
					for(i=0; i<sz; i+=stx)
					{
						
						val = infoW+ stepX*i;
						if(isDrawGrid)
						{
							painter.setPen(gridPen);
							painter.drawLine(val, infoH, val, detT2pos(minV[k], k));
						}
						
						painter.setPen(basePen);
						infoStr = QString("%1").arg(dimN[i], 5, 'f', 1, QLatin1Char(' '));
						painter.setFont(QFont("Times", 20, -1, true));
//						axeRect.setRect(val-10, detT2pos(minV[k], k)+5, 70, 40);
						axeRect.setRect(val-10, detT2pos(0.0, k)+5, 70, 40);
						painter.drawText(axeRect, infoStr, axesOpt);
						
						
					}
					
					
					
					axesOpt.setAlignment(Qt::AlignRight|Qt::AlignCenter);
					val = (((int)(maxV[k])/(int)stepY)+1)*stepY;
					
					qDebug() << "\nocMax" << maxV[k] << "\n";
					qDebug() << "\nocMin" << minV[k] << "\n";
					qDebug() << "\nval" << val << "\n";
					
					
					do
					{
						qDebug() << "\n" << val << "\n";
						if(isDrawGrid)
						{
							painter.setPen(gridPen);
							painter.drawLine(infoW, detT2pos(val, k), infoW+maxIW, detT2pos(val, k));
						}
						
						painter.setPen(basePen);
						infoStr = QString("%1").arg(val);
						painter.setFont(QFont("Times", 20, -1, true));
						axeRect.setRect(0, detT2pos(val, k)-10, infoW, 20);
						
						painter.drawText(axeRect, infoStr, axesOpt);
						
						val-=stepY;
					}while(val>=minV[k]);
					
//					painter.setWorldMatrix(QMatrix().rotate(90));
					if(k)infoStr = QString("OMC(dec), mas");
					else infoStr = QString("OMC(ra), mas");
					
					painter.setFont(QFont("Arial", 20));
					painter.drawText(textRectY, infoStr, textOptY);
//					painter.setWorldMatrix(QMatrix().rotate(-90));
					
					
					p0 = p1 = -1;
					ndots=0;
					for(j=0; j<sz-1; j++)
					{
						
						pPos.setX(infoW+ stepX*(j+0.5));
						if(k) pPos.setY(detT2pos(deVectY[j], k));
						else pPos.setY(detT2pos(deVectX[j], k));
						
						qDebug() << j << ": "<< "X= " << pPos.x() << " Y= " << pPos.y() << "\tnums=" << numsList[j] << "\n";
//						qDebug() << j << ": "<< "deX= " << deVectX[j] << " deY= " << deVectY[j] << "\n";
					
						painter.setPen(pointPen);
						painter.setBrush(pointBrush);
						painter.drawEllipse(pPos, 5, 5);
					
					
						ndots += numsList[j];
						p1 = j/stx;
						if(p1-p0)
						{
							numRect.setRect(stepX*j+infoW, detT2pos(0.0, k)-20, stepX*stx, 20);
							infoStr = QString("%1").arg(ndots);
							painter.setFont(QFont("Arial", 20));
							painter.setPen(basePen);
							painter.drawText(numRect, infoStr, numOpt);
							if(!k) numTot += ndots;
							ndots=0;
							p0=p1;
						}
						
						
					}
					
					if(isDetTrend)
					{
						pPos.setX(infoW+ stepX*0.5);
						pPos.setY(detT2pos(dimN[0]*Z[0]+Z[1], k));
						pPos1.setX(infoW+ stepX*(sz-0.5));
						pPos1.setY(detT2pos(dimN[sz-1]*Z[0]+Z[1], k));
						qDebug() << "trend (" << pPos.x() << ", " << pPos.y() << "):(" << pPos1.x() << ", " << pPos1.y() << ")\n";
						painter.setPen(trendPen);
						painter.drawLine(pPos, pPos1);
					}
					
					painter.setPen(basePen);
					if(k) infoStr = QString("OMC(dec)/Magn");
					else infoStr = QString("OMC(ra)/Magn");
					painter.setFont(QFont("Arial", 40));
					painter.drawText(textRect, infoStr, textOpt);
					infoStr = QString("\n\nnumTot= %1").arg(numTot);
					painter.setFont(QFont("Arial", 30));
					painter.drawText(textRect, infoStr, textOpt);
					
					painter.setPen(basePen);
					infoStr = QString("Magnitudes");
					painter.setFont(QFont("Arial", 30));
					axeRect.setRect(infoW, infoH+maxIH+infoH/2.0, maxIW, infoH/2.0);
					painter.drawText(axeRect, infoStr, textOpt);


					painter.end();
					baseImg->save(resFileName, "BMP");
			}
		}
		break;
		case 8:
		{
			
		
			maxIH = settings->value("task8/maxIH").toInt();
			maxIW = settings->value("task8/maxIW").toInt();
			infoH = settings->value("task8/infoH").toInt();
			infoH1 = settings->value("task8/infoH1").toInt();
			infoW = settings->value("task8/infoW").toInt();
			
			isDrawGrid = settings->value("task8/isDrawGrid").toInt();
			int isDetTrend = settings->value("task8/isDetTrend").toInt();
			
			textRect.setRect(infoW, 0, maxIW, infoH);
			textOpt.setAlignment(Qt::AlignCenter);
			textRectY.setRect(0, 0, infoW, infoH);
			textOptY.setAlignment(Qt::AlignCenter);
			numOpt.setAlignment(Qt::AlignRight|Qt::AlignBottom);
			axesOpt.setAlignment(Qt::AlignLeft|Qt::AlignTop);
			
			qDebug() << "\ndeMin" << deMin << "\n";
			qDebug() << "\ndeMax" << deMax << "\n";
			//drawing
			sz = deList.size();
			k=0;
			
					if(isDetTrend)
					{
						cm = new double[sz*2];
						Z = new double[2];
						L = new double[sz];
						we = new double[sz];
						
						for(i=0; i<sz-1; i++)
						{
							qDebug() << "i="<<i << "\n";
							if(k==1) val = deList[i][2];
							if(k==0) val = deList[i][1];
							if(k==2) val = deList[i][3];
							L[i] = val;
							
							qDebug() << "Li="<< L[i] << "\n";
							cm[2*i] = deList[i][0];
							cm[2*i+1] = 1;
							we[i] = 1.0;
						}
						slsm(2, sz-1, Z, cm, L, we);
						
						qDebug() << "k=" << Z[0] << "\tb=" << Z[1];
				 	}
			
					resFileName = QString("%1_%2.bmp").arg(resPref).arg(k, 2, 10, QLatin1Char(' '));
					baseImg = new QImage(maxIW+infoW, maxIH+infoH+infoH1, QImage::Format_RGB444);
					QPainter painter(baseImg);
					if(!painter.isActive()) 
					{
						qDebug() << "\npainter is\'t active\n"; 
						break;
					}
					painter.fillRect(0, 0, maxIW+infoW, maxIH+infoH+infoH1, baseBrush);
					
					painter.setPen(axesPen);
					painter.drawLine(infoW, 0, infoW, maxIH+infoH+infoH1);
					painter.drawLine(infoW, 0, infoW-5, 25);
					painter.drawLine(infoW, 0, infoW+5, 25);
					painter.drawLine(infoW, detT2pos(0.0, k), infoW+maxIW, detT2pos(0.0, k));
					painter.drawLine(infoW+maxIW, detT2pos(0.0, k), infoW+maxIW-25, detT2pos(0.0, k)-5);
					painter.drawLine(infoW+maxIW, detT2pos(0.0, k), infoW+maxIW-25, detT2pos(0.0, k)+5);
					
					
					stepX = settings->value("task4/stepX").toInt();
					stepY = settings->value("task4/stepY").toInt();
					
					val = (int) deMin;
					do
					{
						if(isDrawGrid)
						{
							painter.setPen(gridPen);
							painter.drawLine(detT3posX(val), infoH, detT3posX(val), detT2pos(minV[k], k));
						}
						
						painter.setPen(basePen);
						infoStr = QString("%1").arg(val);
						painter.setFont(QFont("Times", 20, -1, true));
						axeRect.setRect(detT3posX(val)-10, detT2pos(0, k)+5, 70, 40);
						painter.drawText(axeRect, infoStr, axesOpt);
						
						val+=stepX;
					}while(val<=(int)deMax+1);
					
					
					
					painter.setPen(basePen);
					infoStr = QString("%1").arg(val, 6, 'f', 2, QLatin1Char(' '));
					painter.setFont(QFont("Times", 20, -1, true));
					axeRect.setRect(detT3posX(val)+5, detT2pos(0, k)+5, 50, 40);
					painter.drawText(axeRect, infoStr, axesOpt);
					
					val = deMax;
					if(isDrawGrid)
					{
						painter.setPen(gridPen);
						painter.drawLine(detT3posX(val), infoH, detT3posX(val), detT2pos(minV[k], k));
					}
					
					painter.setPen(basePen);
					infoStr = QString("%1").arg(val, 6, 'f', 2, QLatin1Char(' '));
					painter.setFont(QFont("Times", 20, -1, true));
					axeRect.setRect(detT3posX(val)+5, detT3posY(0)+5, 50, 40);
					painter.drawText(axeRect, infoStr, axesOpt);
					
					axesOpt.setAlignment(Qt::AlignRight|Qt::AlignCenter);
					val = (int)maxV[k];
					
					qDebug() << "\nocMax" << maxV[k] << "\n";
					qDebug() << "\nocMin" << minV[k] << "\n";
					
					do
					{
						qDebug() << "\n" << val << "\n";
						if(isDrawGrid)
						{
							painter.setPen(gridPen);
							painter.drawLine(infoW, detT2pos(val, k), detT3posX(deMax), detT2pos(val, k));
						}
						
						painter.setPen(basePen);
						infoStr = QString("%1").arg(val);
						painter.setFont(QFont("Times", 20, -1, true));
						axeRect.setRect(0, detT2pos(val, k)-10, infoW, 20);
						
						painter.drawText(axeRect, infoStr, axesOpt);
						
						val-=stepY;
					}while(val>=minV[k]);
					
//					painter.setWorldMatrix(QMatrix().rotate(90));
					infoStr = QString("OMC, mas");
					
					painter.setFont(QFont("Arial", 20));
					painter.drawText(textRectY, infoStr, textOptY);
//					painter.setWorldMatrix(QMatrix().rotate(-90));
					
					numTot = 0;
					sz = deList.size();
					for(j=0; j<sz; j++)
					{
						
						if(k==1) val = deList[j][2];
						if(k==0) val = deList[j][1];
						if(k==2) val = deList[j][3];
						pPos.setX(detT3posX(deList[j][0]));
						pPos.setY(detT2pos(val, k));
					
						painter.setPen(pointPen);
						painter.setBrush(pointBrush);
						painter.drawEllipse(pPos, 5, 5);
						
						numTot++;
					}
					
					if(isDetTrend)
					{
						pPos.setX(detT3posX(deMin));
/*						if(k==1) val = deList[0][2];
						if(k==0) val = deList[0][1];
						if(k==2) val = deList[0][3];*/
						pPos.setY(detT2pos(deMin*Z[0]+Z[1], k));
/*						if(k==1) val = deList[sz-1][2];
						if(k==0) val = deList[sz-1][1];
						if(k==2) val = deList[sz-1][3];*/
						pPos1.setX(detT3posX(deMax));
						pPos1.setY(detT2pos(deMax*Z[0]+Z[1], k));
						qDebug() << "trend (" << pPos.x() << ", " << pPos.y() << "):(" << pPos1.x() << ", " << pPos1.y() << ")\n";
						painter.setPen(trendPen);
						painter.drawLine(pPos, pPos1);
					}
					
					painter.setPen(basePen);
					if(k==1) infoStr = QString("OMC(DEC)/Phase");
					if(k==0) infoStr = QString("OMC(RA)/Phase");
					if(k==2) infoStr = QString("OMC(RA&DEC)/Phase");
					painter.setFont(QFont("Arial", 40));
					painter.drawText(textRect, infoStr, textOpt);
					infoStr = QString("\n\nnumTot= %1").arg(numTot);
					painter.setFont(QFont("Arial", 30));
					painter.drawText(textRect, infoStr, textOpt);
					
					painter.setPen(basePen);
					infoStr = QString("Phase, degree");
					painter.setFont(QFont("Arial", 30));
					axeRect.setRect(infoW, infoH+maxIH+infoH/2.0, maxIW, infoH/2.0);
					painter.drawText(axeRect, infoStr, textOpt);


					painter.end();
					baseImg->save(resFileName, "BMP");
			
		}
		break;
		default:
		break;
	}

}

