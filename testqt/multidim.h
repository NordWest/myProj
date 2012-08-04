#ifndef ASTRO_H
#include "./../libs/astro.h"
#endif

#include "./../libs/fitsio.h"
#include "ccdmeasurements.h"

#ifndef MULTIDIM_H
#define MULTIDIM_H
#define USHORT_TYPE 0
#define SHORT_TYPE 1
#define LONG_TYPE 2
#define FLOAT_TYPE 3
#define DOUBLE_TYPE 4

struct sizes2d // вектор размеров
{
  int axes[2];
  sizes2d(int n, int m) { axes[0]=n; axes[1]=m; }
  operator int const* () const { return axes; }
};

class axeshelper
{
	int *axes;
	int dim;
public:
    axeshelper(int n, int m){axes = new int[2]; dim = 2; axes[0]=n; axes[1]=m; } // здесь разбираемся со всеми размерностями
	axeshelper(int n){axes = new int[1]; dim = 1; axes[0]=n; }
    // добавить конструкторы для других размерностей по вкусу
	operator int const* () const { return axes; }
};

class segment			//квадратный фрагмент
{
public:
	double *data;
	int x0, y0;			//left top corner
	int size0, size1;
	
	segment();
	segment(int n, int m);
	segment(double *fd, int i, int j, int n, int m);
	segment(int i, int j, int n, int m);
	~segment();
	double get(int i, int j);
//	int getAperture(imgAperture *iApe);
	
};


//multidim
//template <class DTA>
class multidim
{
public:
	unsigned short *ushD;//0
	short          *shD;//1
	long           *lD;//2
	float          *fD;//3
	double         *dD;//4

	int *naxes;
	int dataType;
	int size;
	int dim;
public:	
	multidim();
	multidim(const int type, const int *axes, const int axesNum);
	virtual ~multidim();
	multidim(unsigned short *fdata, int const *axes, int axesNum);
	multidim(short *fdata, int const *axes, int axesNum);
	multidim(long *fdata, int const *axes, int axesNum);
	multidim(float *fdata, int const *axes, int axesNum);
	multidim(double *fdata, int const *axes, int axesNum);
	
	unsigned short getUsh(int pos);
	short getSh(int pos);
	long getL(int pos);
	float getF(int pos);
	double getD(int pos);
	
//4 img2d
/**	multidim(int dType, int n, int m);
	multidim(unsigned short *fdata, int n, int m);
	multidim(short *fdata, int n, int m);
	multidim(long *fdata, int n, int m);
	multidim(float *fdata, int n, int m);
	multidim(double *fdata, int n, int m);*/
//

	int isin(int *pos);
	int detPos(int *pos);
	
	int get(unsigned short *x, int *pos, int *sizes = NULL);		//получить прямоугольный элемент 
	int get(short *x, int *pos, int *sizes = NULL);					//pos - левый верхний угол
	int get(long *x, int *pos, int *sizes = NULL);					//sizes - размеры по осям
	int get(float *x, int *pos, int *sizes = NULL);
	int get(double *x, int *pos, int *sizes = NULL);
	
	int set(unsigned short x, int *pos);							//элементу pos назначить значение x
	int set(short x, int *pos);
	int set(long x, int *pos);
	int set(float x, int *pos);
	int set(double x, int *pos);
	
	void set(unsigned short *fdata, int len = 0);					//заполнить соответствующие массивы данными с первого элемента
	void set(short *fdata, int len = 0);							//len - длинна массива fdata
	void set(long *fdata, int len = 0);
	void set(float *fdata, int len = 0);
	void set(double *fdata, int len = 0);
	
	int set(unsigned short *fdata, int* pos, int *sizes = NULL);	//установить прямоугольный элемент 
	int set(short *fdata, int* pos, int *sizes = NULL);				//pos - левый верхний угол
	int set(long *fdata, int* pos, int *sizes = NULL);				//sizes - размеры по осям
	int set(float *fdata, int* pos, int *sizes = NULL);
	int set(double *fdata, int* pos, int *sizes = NULL);
	
	int getSize();
	
	void clear();
};

class img2d
{
	
public:

	unsigned short *ushD;//0
	short          *shD;//1
	long           *lD;//2
	float          *fD;//3
	double         *dD;//4
	long naxes[2];
	int bpix;
	long size;
	int dim;
	
	int isin(int n, int m);
	int detPos(int n, int m);

	img2d();
	img2d(const int dType, const long n, const long m);
	~img2d();
	img2d(unsigned short *fdata, long n, long m);
	img2d(short *fdata, long n, long m);
	img2d(long *fdata, long n, long m);
	img2d(float *fdata, long n, long m);
	img2d(double *fdata, long n, long m);

	int get(unsigned short *f, int i, int j, int size0 = 1, int size1 = 1);
	int get(short *f, int i, int j, int size0 = 1, int size1 = 1);
	int get(long *f, int i, int j, int size0 = 1, int size1 = 1);
	int get(float *f, int i, int j, int size0 = 1, int size1 = 1);
	int get(double *f, int i, int j, int size0 = 1, int size1 = 1);
	
	int set(unsigned short *f, int i, int j, int size0 = 1, int size1 = 1);
	int set(short *f, int i, int j, int size0 = 1, int size1 = 1);
	int set(long *f, int i, int j, int size0 = 1, int size1 = 1);
	int set(float *f, int i, int j, int size0 = 1, int size1 = 1);
	int set(double *f, int i, int j, int size0 = 1, int size1 = 1);
	
	int getSgt(segment *sg, int i, int j, int size0 = 1, int size1 = 1);
	int getSgtC(segment *sg, int i, int j, int size0 = 1, int size1 = 1);
	double getImgPix(int i, int j);
	double getPos(int pos);
	
	int setVal(double f, long pos);
	
	int getAperture(imgAperture *iApe, int Xc, int Yc, int ro);
/*	
	int set(unsigned short *fdata, int len);
	int set(short *fdata, int len);
	int set(long *fdata, int len);
	int set(float *fdata, int len);
	int set(double *fdata, int len);*/
};

#define MULTIDIM_H
#endif
