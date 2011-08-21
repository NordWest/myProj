#include "ccdtools.h"

multidim::multidim(int *axes, int axesNum);
{
	ushD = NULL;//0
	shD = NULL;//1
	lD = NULL;//2
	fD = NULL;//3
	dD = NULL;//4
	naxes = new int[axesNum];
	size = 0;
	for(int i=0; i<axesNum; i++)
	{
		naxes[i] = axes[i];
		size += naxes[i];
	}
	dim = axesNum;
	dataType = -1;
}

multidim::~multidim()
{
	if(ushD!=NULL) delete [] ushD;//0
	if(shD!=NULL) delete [] shD;//1
	if(lD!=NULL) delete [] lD;//2
	if(fD!=NULL) delete [] fD;//3
	if(dD!=NULL) delete [] dD;//4
	delete [] naxes;
}

multidim::multidim(unsigned short *fdata, int *axes, int axesNum)
{
	dataType = 0;
	naxes = new int[axesNum];
	size = 0;
	for(int i=0; i<axesNum; i++)
	{
		naxes[i] = axes[i];
		size += naxes[i];
	}
	dim = axesNum;
	ushD = new unsigned short[size];
}

multidim::multidim(short *fdata, int *axes, int axesNum)
{
	dataType = 1;
	naxes = new int[axesNum];
	size = 0;
	for(int i=0; i<axesNum; i++)
	{
		naxes[i] = axes[i];
		size += naxes[i];
	}
	dim = axesNum;
	shD = new short[size];
}

multidim::multidim(long *fdata, int *axes, int axesNum)
{
	dataType = 2;
	naxes = new int[axesNum];
	size = 0;
	for(int i=0; i<axesNum; i++)
	{
		naxes[i] = axes[i];
		size += naxes[i];
	}
	dim = axesNum;
	lD = new long[size];
}

multidim::multidim(float *fdata, int *axes, int axesNum)
{
	dataType = 3;
	naxes = new int[axesNum];
	size = 0;
	for(int i=0; i<axesNum; i++)
	{
		naxes[i] = axes[i];
		size += naxes[i];
	}
	dim = axesNum;
	lD = new long[size];
}

multidim::multidim(double *fdata, int *axes, int axesNum)
{
	dataType = 4;
	naxes = new int[axesNum];
	size = 0;
	for(int i=0; i<axesNum; i++)
	{
		naxes[i] = axes[i];
		size += naxes[i];
	}
	dim = axesNum;
	dD = new double[size];
}

int multidim::detPos(int *pos)
{
	int xpos = 0;
	int mul;
	for(int i=0; i<dim; i++)
	{
		mul = 0;
		for(int j=0; j<dim-i; j++) mul *= naxes[j];
		xpos += pos[i]*mul;
	}
	return xpos;
}

int multidim::isin(int *pos)
{
	for(int i=0; i<dim; i++) if(pos[i]>=naxes[i]) return 0;
	return 1;
}

int multidim::get(unsigned short *x, int *pos)
{
	if(dataType!=0) return 1;
	if(!isin(pos)) return 2;
	
	*x = ushD[detPos(pos)];
	return 0;
}

int multidim::get(short *x, int *pos)
{
	if(dataType!=1) return 1;
	if(!isin(pos)) return 2;
	
	*x = shD[detPos(pos)];
	return 0;
}

int multidim::get(long *x, int *pos)
{
	if(dataType!=2) return 1;
	if(!isin(pos)) return 2;
	
	*x = lD[detPos(pos)];
	return 0;
}

int multidim::get(float *x, int *pos)
{
	if(dataType!=3) return 1;
	if(!isin(pos)) return 2;
	
	*x = fD[detPos(pos)];
	
	return 0;
}

int multidim::get(double *x, int *pos)
{
	if(dataType!=4) return 1;
	if(!isin(pos)) return 2;
	
	*x = dD[detPos(pos)];
	return 0;
}

int multidim::set(unsigned short x, int *pos)
{
	if(dataType!=0) return 1;
	if(!isin(pos)) return 2;
	
	ushD[detPos(pos)] = x;
	return 0;
}

int multidim::set(short x, int *pos)
{
	if(dataType!=0) return 1;
	if(!isin(pos)) return 2;
	
	shD[detPos(pos)] = x;
	return 0;
}

int multidim::set(long x, int *pos)
{
	if(dataType!=0) return 1;
	if(!isin(pos)) return 2;
	
	lD[detPos(pos)] = x;
	return 0;
}
int multidim::set(float x, int *pos)
{
	if(dataType!=0) return 1;
	if(!isin(pos)) return 2;
	
	fD[detPos(pos)] = x;
	return 0;
}
int multidim::set(double x, int *pos)
{
	if(dataType!=0) return 1;
	if(!isin(pos)) return 2;
	
	dD[detPos(pos)] = x;
	return 0;
}
	
void multidim::set(unsigned short *fdata, int len)
{
	if(dataType!=0) return;
	if(len>size) return;
	
	for(int i; i<len; i++) ushD[i] = fdata[i];
}

void multidim::set(short *fdata, int len)
{
	if(dataType!=0) return;
	if(len>size) return;
	
	for(int i; i<len; i++) shD[i] = fdata[i];
}

void multidim::set(long *fdata, int len)
{
	if(dataType!=0) return;
	if(len>size) return;
	
	for(int i; i<len; i++) lD[i] = fdata[i];
}

void multidim::set(float *fdata, int len)
{
	if(dataType!=0) return;
	if(len>size) return;
	
	for(int i; i<len; i++) fD[i] = fdata[i];
}

void multidim::set(double *fdata, int len)
{
	if(dataType!=0) return;
	if(len>size) return;
	
	for(int i; i<len; i++) dD[i] = fdata[i];
}


////////////////////////////////////////////////////////////////////////////////////////

img2d::img2d(int *axes, int axesNum);
img2d::~img2d();
img2d::img2d(unsigned short *fdata, int *axes, int axesNum);
img2d::img2d(short *fdata, int *axes, int axesNum);
img2d::img2d(long *fdata, int *axes, int axesNum);
img2d::img2d(float *fdata, int *axes, int axesNum);
img2d::img2d(double *fdata, int *axes, int axesNum);

double img2d::get(int i, int j);
void img2d::set(double f, int i, int j);
void img2d::set(double *fdata, int len);
int img2d::axeLen(int i);