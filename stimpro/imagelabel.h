//#include <QLabel>
//#include <QGLWidget>
#include <QWidget>
#include "./../libs/fitsdata.h"
//#include "multidim.h"
#include "./../libs/astro.h"
//#include "./../libs/identify.h"
#include "./../libs/comfunc.h"
#include "./../libs/mb.h"

//#include "./../libs/comfunc.h"

class imageLabel: public QWidget//виджет для прорисовки изображения "на экране" наследуется тупо от QWidget
{

public:
	//data
    QList<fitsdata *> fitsdataList;// список указателей на объекты fitsdata, которые содержат данные fits-файла и параметры прорисовки изображения
//	QList<marksGrid *> marksList;
	int frameIndex;// текущий индекс для обозначения ПЗС кадра, который прорисовывается в данный момент
	int cX,cY, imgW, imgH;// cX,cY- координаты центра прямоугольника изображения для прорисовки, imgW, imgH - размеры изображения с учетом масштаба 
	int maxImgWidth, maxImgHeight;// максимальные размеры для "буфера прорисовки"
	double scale;// масштаб для прорисовки на экране
	double aperture;
	QImage *baseImg;// "буфер прорисовки" - указатель на объект QImage. 
	///////////////////////
	imageLabel(QWidget *parent):QWidget(parent) {};
	
	int closeFitsFile(int num);
	imageLabel(int maxIW,int maxIH, QWidget *parent);//конструктор виджета. maxImgWidth = maxIW, maxImgHeight = maxIH и создается "буфер прорисовки"
	void paintEvent(QPaintEvent *event); //переопределение события прорисовки
	void resizeEvent(QResizeEvent *event);//переопределение события изменения размера
	void updateBaseImage(int xc, int yc, QPainter *painter);//перерисовка изображения и выброс его на экран
	
        //void getMarksGrid(int ctype, double fov, double minM, double maxM);
        void setInstProp(double mX, double mY, double fov, double ang);
        //void setRotAngle(double ang);
	void detInstrProp();
        int identAuto(double ang, int identType = 0);
	
	void changeScales(double dSc);
	void setScx(double Scx);
	void setScy(double Scy);
	void setAperture(double nApe);
//	void setFov(double fov);
	void reMarks();

        void setColors(QColor bgCol, QColor ipixCol, QColor catCol, QColor refCol, QColor objCol);
	
	fitsdata *getCurFits();
	marksGrid *getCurMarks();
	marksGrid *getCurMarksIpix();
	void marksIdent(int cat_type, double fov, double minM, double maxM);
	
	QPen basePen;
        QPen marksGridPen;
        QPen marksIPixPen;
        QPen marksRefPen;
        QPen marksObjPen;
        QColor bgCol;
	
	int dX, dY;

//	void mouseMoveEvent(QMouseEvent *event);

};
