#ifndef ASTRO_H
#include "..\libs\astro.h"
#endif
//multidim
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
	
	multidim(int *axes, int axesNum);
	~multidim();
	multidim(unsigned short *fdata, int *axes, int axesNum);
	multidim(short *fdata, int *axes, int axesNum);
	multidim(long *fdata, int *axes, int axesNum);
	multidim(float *fdata, int *axes, int axesNum);
	multidim(double *fdata, int *axes, int axesNum);

	int isin(int *pos);
	int detPos(int *pos);
	
	int get(unsigned short *x, int *pos);
	int get(short *x, int *pos);
	int get(long *x, int *pos);
	int get(float *x, int *pos);
	int get(double *x, int *pos);
	
	int set(unsigned short x, int *pos);
	int set(short x, int *pos);
	int set(long x, int *pos);
	int set(float x, int *pos);
	int set(double x, int *pos);
	
	void set(unsigned short *fdata, int len);
	void set(short *fdata, int len);
	void set(long *fdata, int len);
	void set(float *fdata, int len);
	void set(double *fdata, int len);
};

class img2d : public multidim
{
	img2d(int *axes, int axesNum);
	~img2d();
	img2d(unsigned short *fdata, int *axes, int axesNum);
	img2d(short *fdata, int *axes, int axesNum);
	img2d(long *fdata, int *axes, int axesNum);
	img2d(float *fdata, int *axes, int axesNum);
	img2d(double *fdata, int *axes, int axesNum);

	double get(int i, int j);
	void set(double f, int i, int j);
	void set(double *fdata, int len);
	int axeLen(int i);
};
