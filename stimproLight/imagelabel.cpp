#include <QtGui>
#include "imagelabel.h"


imageLabel::imageLabel(int maxIW,int maxIH, QScrollArea *parentScrollArea,QStatusBar *parentStatusBar, QWidget *parent):QWidget(parent)
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
	sa = parentScrollArea;
	sb = parentStatusBar;
	cX = 0;
	cY = 0;
	aperture = 10;
	focusX = 0;
	focusY = 0;
	viewHist = 0;
	viewAperture = 0;
        viewFoundStars=1;
	viewPSF = 0;
	psfmodel = 1;
}

void imageLabel::paintEvent(QPaintEvent *event)//событие прорисовки
{
	QPainter painter(this);// объект QPainter для работы с изображением (копирования с виртуального экрана на реальный (imagelabel)). Использование this как параметра инициализации дает возможность рисовать на реальном экране.
	updateBaseImage(sa->horizontalScrollBar()->value(),sa->verticalScrollBar()->value(),&painter);//выполняем прорисовку, передав указатель на объект QPainter
	//painter.drawEllipse(QRectF(sa->horizontalScrollBar()->value()+20 ,sa->verticalScrollBar()->value()+20,20,20));
	QWidget::paintEvent(event);
	//QMessageBox::information(0,"stimpro","-",QMessageBox::Ok,0,0);
};

void imageLabel::mouseMoveEvent(QMouseEvent * event)
{
	if(fitsdataList.count()>0)//если есть открытые файлы, то показываем пиксельную информацию
	{
		fitsdata *fitsd = fitsdataList[frameIndex];
		int x = event->x()/scale;
		int y = event->y()/scale;
		double J = fitsd->getPixelValue(y*fitsd->naxes[0]+x);
		QString msg = 
			"x = "+QString("%1").arg( x,5,10,QLatin1Char( ' ' ))+"; y = "+
			QString("%1").arg( y,5,10,QLatin1Char( ' ' ))+"; J = "+
			QString("%1").arg( J,10,'f',3,QLatin1Char( ' ' ))+ "; "+
			fitsd->getEqCoordinatesOfPixel(x,y);
		sb->showMessage(msg);
	}
};

void imageLabel::mousePressEvent(QMouseEvent *event)
{
	if(fitsdataList.count()>0)
	{
		cX = event->x()/scale;
		cY = event->y()/scale;
		//QPaintEvent *pevent = new QPaintEvent(QRegion(0,0,100,100,QRegion::Rectangle));
		//QApplication::sendEvent(this, pevent);
		repaint();
	}
};


void imageLabel::updateBaseImage(int xc, int yc, QPainter *painter)
{
	if(fitsdataList.count()>0)//если есть открытые файлы, то рисуем
	{
		fitsdata *fitsd = fitsdataList[frameIndex];// получаем указатель на текущий ПЗС-кадр
		uchar* imgdata = baseImg->bits();// получаем указатель на буфер "виртуального экрана". Если сюда писать данные, то они будут восприниматься как виртуальное изображение, которое хранится в памяти и не показывается на экране
        ///////////////////////////////////////////////
		int Xo = xc/scale;//определяем левый верхний угол прямоугольника в пространстве пиксельных координат реального ПЗС-кадра, который надо отобразить на виртуальном, а потом реальном экране
		int Yo = yc/scale;//
		int w,h;//переменные для хранения размеров прямоугольника изображения
		int W = sa->frameRect().width();//размеры прямоугольника прорисовки равны размеру реального виджета imageLabel
		int H = sa->frameRect().height(); 
		if(scale<1)//масштаб меньше 1 - уменьшение
		{
			w = sa->frameRect().width();//размеры прямоугольника прорисовки равны размеру реального виджета imageLabel
			h = sa->frameRect().height();
			for(int y=0; y < h; y++)
				for(int x=0; x < w; x++)
					imgdata[maxImgWidth*y+x] = fitsd->getImgPixValue(Xo+x/scale, Yo+y/scale);// прорисовывается каждый 1/scale -тый пиксел реального ПЗС-кадра. Если scale = 0.5, то каждый второй. Это не приводит к значимому ухудшению качества картинки и обеспечивает нужную скорость при прокрутке больших изображений
		}
		else//масштаб = 1 (нет увеличения или уменьшения) или масштаб>1 (увеличение)
		{
			w =  sa->frameRect().width()/scale;//размеры прямоугольника прорисовки в scale раз меньше размеров реального виджета imageLabel
			h =  sa->frameRect().height()/scale;
			for(int y=0; y < h; y++)
				for(int x=0; x < w; x++)
					imgdata[maxImgWidth*y+x] = fitsd->getImgPixValue(Xo+x, Yo+y);// рисуем пикселы один к одному
		}
		painter->drawImage(QRectF(xc,yc,W,H),*baseImg, QRectF(0,0,w,h));//копируем виртуальный экран на реальный. В случае увеличения происходит масштабирование от меньшего виртуального экрана к большему реальному.
		QString msg;
		if(focusX)
		{
			painter->drawLine(xc,cY*scale,xc+W,cY*scale);
			updateProfileX(painter, fitsd,0,Qt::green);
			msg += 
			" y = "+QString("%1").arg( cY,5,10,QLatin1Char( ' ' ))+"; mean value Jy = "+
			QString("%1").arg( meanX,10,'f',3,QLatin1Char( ' ' ))+ "; se Jy = "+
			QString("%1").arg( seX,10,'f',3,QLatin1Char( ' ' ))+".";
		}
		if(focusY)
		{
			painter->drawLine(cX*scale,yc,cX*scale,yc+H);
			updateProfileY(painter, fitsd,0,Qt::green);
			msg +=
			" x = "+QString("%1").arg( cX,5,10,QLatin1Char( ' ' ))+"; mean value Jx = "+
			QString("%1").arg( meanY,10,'f',3,QLatin1Char( ' ' ))+ "; se Jx = "+
			QString("%1").arg( seY,10,'f',3,QLatin1Char( ' ' ))+".";
		}
		if((focusX)||(focusY)||((focusX)&&(focusY)))sb->showMessage(msg);
		if(viewHist)updateHist(painter, fitsd);
		if(viewAperture)
			{
				QPen defaultPen = painter->pen(); 
				painter->setPen(Qt::yellow);
				painter->drawEllipse(cX*scale-aperture*scale/2,cY*scale-aperture*scale/2,aperture*scale,aperture*scale);
				painter->setPen(defaultPen);
			}
		if(viewPSF)
		{
			//painter->drawLine(xc,cY*scale,xc+W,cY*scale);
			updateProfileX(painter, fitsd,1,Qt::blue);
			//painter->drawLine(cX*scale,yc,cX*scale,yc+H);
			updateProfileY(painter, fitsd,1,Qt::blue);
		} 
                if(viewFoundStars)
                {
                    QPen defaultPen = painter->pen();
                    painter->setPen(Qt::blue);
                    for(int i=0;i<fitsd->cmX.count();i++)
                        painter->drawEllipse(fitsd->cmX[i]*scale-aperture*scale/2,fitsd->cmY[i]*scale-aperture*scale/2,
                                             aperture*scale,aperture*scale);
                    painter->setPen(defaultPen);
                }
	}
};

void imageLabel::updateProfileX(QPainter *painter, fitsdata *fitsd, int profileType, QColor color)
{
	QVector<double> Xdata;
	QVector<double> Xpsfd;
	int w,h;
	int W = sa->frameRect().width();//размеры прямоугольника прорисовки равны размеру реального виджета imageLabel
	int H = sa->frameRect().height();
	int Xo = sa->horizontalScrollBar()->value();//
	int Yo = sa->verticalScrollBar()->value();//
	if(scale<1)
	{
		w = W;h = H*scale;
		for(int x = 0; x<w; x++)
		{
			if (profileType) Xpsfd << fitsd->psfdata((Xo+x)/scale, cY);
			Xdata << fitsd->getPixelValueXY((Xo+x)/scale, cY);
		}
	}
	else
	{
		w = W/scale;h = H;
		for(int x = 0; x<w; x++)
		{
			if (profileType) Xpsfd << fitsd->psfdata(Xo/scale+x, cY);
			Xdata << fitsd->getPixelValueXY(Xo/scale+x, cY);
		}
	}
	////////////////////////
	double maxvX = Xdata[0];
	double minvX = Xdata[0];
	meanX = 0;
	for (int i=0;i<Xdata.count();i++)
	{
		if(Xdata[i]>maxvX) maxvX = Xdata[i];
		if(Xdata[i]<minvX) minvX = Xdata[i];
		meanX+=Xdata[i];
	}
	meanX = meanX/Xdata.count();
	seX=0;
	for (int i=0;i<Xdata.count();i++) seX+=pow(Xdata[i]-meanX,2);
	seX = sqrt(seX/(Xdata.count()-1));
	///////////////////////////
	int xlevel = Yo+0.85*h;
	int maxD = 0.5*h;
	int  vX1 = 0,vX2;
	QPen defaultPen = painter->pen(); 
	painter->setPen(color);
	for (int i=0;i<Xdata.count();i++)
	{
		if(profileType)
                        vX2 = (int)(maxD*(Xpsfd[i]-minvX)/(maxvX-minvX));
		else
			vX2 = (int)(maxD*(Xdata[i]-minvX)/(maxvX-minvX));
                if(i==0)vX1 = vX2;
		if(scale<1)
		{
			painter->drawLine(Xo+i,xlevel-vX1,Xo+i,xlevel-vX2);
			painter->drawLine(Xo+i,xlevel-vX2,Xo+i+1,xlevel-vX2);
		}
		else
		{
			painter->drawLine(Xo+i*scale,xlevel-vX1,Xo+i*scale,xlevel-vX2);
			painter->drawLine(Xo+i*scale,xlevel-vX2,Xo+(i+1)*scale,xlevel-vX2);
		}
		vX1 = vX2;
	}
	painter->setPen(defaultPen);
};

void imageLabel::updateProfileY(QPainter *painter, fitsdata *fitsd, int profileType, QColor color)
{
	QVector<double> Ydata;
	QVector<double> Ypsfd;
	int w,h;
	int W = sa->frameRect().width();//размеры прямоугольника прорисовки равны размеру реального виджета imageLabel
	int H = sa->frameRect().height();
	int Xo = sa->horizontalScrollBar()->value();//
	int Yo = sa->verticalScrollBar()->value();//
	if(scale<1)
	{
		w = W*scale; h = H;
		for(int y = 0; y<h; y++)
		{
			if (profileType) Ypsfd << fitsd->psfdata(cX, (Yo+y)/scale);
			Ydata << fitsd->getPixelValueXY(cX, (Yo+y)/scale);
		}
	}
	else
	{
		w = W; h = H/scale;
		for(int y = 0; y<h; y++)
		{
			if (profileType) Ypsfd << fitsd->psfdata(cX, Yo/scale+y);
			Ydata << fitsd->getPixelValueXY(cX, Yo/scale+y);
		}
	}
	////////////////////////
	double maxvY = Ydata[0];
	double minvY = Ydata[0];
	meanY = 0;
	for (int i=0;i<Ydata.count();i++)
	{
		if(Ydata[i]>maxvY) maxvY = Ydata[i];
		if(Ydata[i]<minvY) minvY = Ydata[i];
		meanY+=Ydata[i];
	}
	meanY = meanY/Ydata.count();
	seY=0;
	for (int i=0;i<Ydata.count();i++) seY+=pow(Ydata[i]-meanY,2);
	seY = sqrt(seY/(Ydata.count()-1));
	///////////////////////////
	int ylevel = Xo+0.85*w;
	int maxD = 0.5*w;
	int  vY1 = 0,vY2;
	QPen defaultPen = painter->pen();
	painter->setPen(color);
	for (int i=0;i<Ydata.count();i++)
	{
		if(profileType)
			vY2 = (int)(maxD*(Ypsfd[i]-minvY)/(maxvY-minvY));
		else
			vY2 = (int)(maxD*(Ydata[i]-minvY)/(maxvY-minvY));
                if(i==0)vY1 = vY2;
		if(scale<1)
		{
			painter->drawLine(ylevel-vY1,Yo+i,ylevel-vY2,Yo+i);
			painter->drawLine(ylevel-vY2,Yo+i,ylevel-vY2,Yo+i+1);
		}
		else
		{
			painter->drawLine(ylevel-vY1,Yo+i*scale,ylevel-vY2,Yo+i*scale);
			painter->drawLine(ylevel-vY2,Yo+i*scale,ylevel-vY2,Yo+(i+1)*scale);
		}
		vY1 = vY2;
	}
	painter->setPen(defaultPen);
};

void imageLabel::updateHist(QPainter *painter, fitsdata *fitsd)
{
	int W = sa->frameRect().width()/2;//размеры прямоугольника прорисовки равны размеру реального виджета imageLabel
	int H = sa->frameRect().height()/2;
	int Xo = sa->horizontalScrollBar()->value();//
	int Yo = sa->verticalScrollBar()->value()+0.85*sa->frameRect().height();//
	int xh,yh;
	double maxhist = log10(fitsd->hD[(long)(fitsd->fpix-fitsd->minv)]);
	//int yho=(int)H*fitsd->hD[0]/maxhist;
	//
	QPen defaultPen = painter->pen();
	//painter->setPen(Qt::yellow);
	for(int i=0;i<fitsd->hsize;i++)
	{
		xh = Xo+W*(log10(i+1)/log10(fitsd->hsize));
		yh = H*log10(fitsd->hD[i]+1)/maxhist;
		if((i+fitsd->minv>=fitsd->minp)&&(i+fitsd->minv<=fitsd->maxp))
			painter->setPen(Qt::white);
		else
			painter->setPen(Qt::gray);
		painter->drawLine(xh,Yo,xh,Yo-yh);
	}
	painter->setPen(defaultPen);
	QString msg =
		//QString("%1").arg( fitsd->st,7,10,QLatin1Char( ' ' ));//+" "+
		QString("%1").arg(fitsd->st/fitsd->hsize,10,'f',3,QLatin1Char( ' ' ));//+ " "+
		//QString("%1").arg( seY,10,'f',3,QLatin1Char( ' ' ))+".";
	sb->showMessage(msg);
};
