#ifndef VECTGRID3D_H
#define VECTGRID3D_H

#ifndef MULTIDIM_H
#include "multidim.h"
#endif

#include "ap.h"
#include "spline3.h"
#include "spline2d.h"

#include <QList>

class vectGrid3D
{
public:
	vectGrid3D();
	~vectGrid3D();
	vectGrid3D(char* fname);
	
	int init(char* fname);
        int initAxes(int *axes, int axesNum);
        int setLevels(double *xLevels, int xLevNum, double *yLevels, int yLevNum, double *mLevels, int mLevNum);
        int initLevels(QList <double*> dims, QList <int> dNums);
	
	int printCoefs();
	
	int int2D(double x, double y, double m, double *xint, double *yint, long *nint);
        int int2Drad(double x, double y, double m, double *xint, double *yint, double rMax, double nmin);
        //int int2Drad1(double x, double y, double m, double *xint, double *yint, double rMax, double nmin);
        //int int2DradM(double x, double y, double *xVect, double *yVect, double rMax, double nmin);
        int int2DradM(double x, double y, double m, double *xint, double *yint, double rMax, double nmin);

        void setPoint(double *vect, double x, double y, long num);    //vect = {x|y|m}
        void addPoint(double *vect, double x, double y);    //vect = {x|y|m}
        void detMean();
        void doWeght();
        void initVF();

        void getVect(int i, int j, int k, double *px, double *py, double *pm, double *x, double *y, long *num);

        void clearDims();

        int saveVF(QString fName);
        int saveDotList(QString resDir, QString colSep, QString prefStr = "", double mult = 1, double nmin = 1);

        long getNumMax();

        multidim *xVectField;
        multidim *yVectField;
        multidim *vectNumField;

        int axnum;					//Число осей
        int *ax;					//Размерности осей
        int *pos;                                       //Вектор положения
        long numTot;

        QList <int> dNums;		//Количество границ диапазонов (= Размерности осей+1)
        QList <double*> dims;		//Границы диапазонов ([dNums])


	
	ap::real_1d_array xarr;		//Массивы координат опорных точек функции f(x, y) по обеим осям
	ap::real_1d_array yarr;
	
	QList <ap::real_2d_array> fXM;
	ap::real_2d_array fX;		//Массивы значений
	ap::real_2d_array fY;
	ap::real_2d_array fN;
	
	ap::real_1d_array coefX;	//Массивы коеффициентов бикубической интерполяции
	ap::real_1d_array coefY;
	ap::real_1d_array coefN;
	
	QList <ap::real_1d_array> coefXM;
	QList <ap::real_1d_array> coefYM;
	QList <ap::real_1d_array> coefNM;
};












#endif
