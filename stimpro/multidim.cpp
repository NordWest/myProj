#include "multidim.h"

multidim::multidim()
{
	ushD = NULL;//0
	shD = NULL;//1
	lD = NULL;//2
	fD = NULL;//3
	dD = NULL;//4
	naxes = NULL;
	size = 0;
	dim = 0;
	dataType = -1;
}

multidim::multidim(const int type, const int *axes, const int axesNum)
{
	ushD = NULL;//0
	shD = NULL;//1
	lD = NULL;//2
	fD = NULL;//3
	dD = NULL;//4
	naxes = new int[axesNum];
	size = 1;
	for(int i=0; i<axesNum; i++)
	{
		naxes[i] = axes[i];
		size *= naxes[i];
	}
	dim = axesNum;
	dataType = type;
	switch(dataType)
	{
	case 0:
		ushD = new unsigned short[size];
		break;
	case 1:
		shD = new short[size];
		break;
	case 2:
		lD = new long[size];
		break;
	case 3:
		fD = new float[size];
		break;
	case 4:
		dD = new double[size];
		break;
	}
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

multidim::multidim(unsigned short *fdata, int const *axes, int axesNum)
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
	for(int i=0; i<size; i++) ushD[i] = fdata[i];
}

multidim::multidim(short *fdata, int const *axes, int axesNum)
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
	for(int i=0; i<size; i++) shD[i] = fdata[i];
}

multidim::multidim(long *fdata, int const *axes, int axesNum)
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
	for(int i=0; i<size; i++) lD[i] = fdata[i];
}

multidim::multidim(float *fdata, int const *axes, int axesNum)
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
	fD = new float[size];
	for(int i=0; i<size; i++) fD[i] = fdata[i];
}

multidim::multidim(double *fdata, int const *axes, int axesNum)
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
	for(int i=0; i<size; i++) dD[i] = fdata[i];
}
///////////////////////////////////////////////////////////////////////////////////////////

unsigned short multidim::getUsh(int pos)
{
	if(pos<size) return ushD[pos];
	return 0;
}

short multidim::getSh(int pos)
{
	if(pos<size) return shD[pos];
	return 0;
}

long multidim::getL(int pos)
{
	if(pos<size) return lD[pos];
	return 0;
}

float multidim::getF(int pos)
{
	if(pos<size) return fD[pos];
	return 0;
}

double multidim::getD(int pos)
{
	if(pos<size) return dD[pos];
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////
/*
multidim::multidim(int type, int n, int m)
{
	int *axs = new int[2];
	axs[0] = n, axs[1] = m;
	
	multidim(type, axs, 2);
	
}

multidim::multidim(unsigned short *fdata, int n, int m)
{
	multidim(USHORT_TYPE, n, m);
	dataType = 0;
	ushD = new unsigned short[size];
	for(int i=0; i<size; i++) ushD[i] = fdata[i];
}

multidim::multidim(short *fdata, int n, int m)
{
	multidim(SHORT_TYPE, n, m);
	dataType = 1;
	shD = new short[size];
	for(int i=0; i<size; i++) shD[i] = fdata[i];
}

multidim::multidim(long *fdata, int n, int m)
{
	multidim(LONG_TYPE, n, m);
	dataType = 2;
	lD = new long[size];
	for(int i=0; i<size; i++) lD[i] = fdata[i];
}

multidim::multidim(float *fdata, int n, int m)
{
	multidim(FLOAT_TYPE, n, m);
	dataType = 3;
	fD = new float[size];
	for(int i=0; i<size; i++) fD[i] = fdata[i];
}

multidim::multidim(double *fdata, int n, int m)
{
	multidim(DOUBLE_TYPE, n, m);
	dataType = 4;
	dD = new double[size];
	for(int i=0; i<size; i++) dD[i] = fdata[i];
}
*/
///////////////////////////////////////////////////////////////////////////////////////////
int multidim::detPos(int *pos)
{
	int xpos = 0;
	int mul;
	int i, j;
	for(i=1; i<=dim; i++)
	{
		mul = 1;
		for(j=0; j<dim-i; j++) mul *= naxes[j];
		xpos += pos[dim-i]*mul;
	}
	return xpos;
}

int multidim::isin(int *pos)
{
	for(int i=0; i<dim; i++) if(pos[i]>=naxes[i]) return 0;
	return 1;
}

int multidim::get(unsigned short *x, int *pos, int *sizes)
{
	int i, j;
	if(dataType!=0) return 1;
	if(sizes==NULL)
	{
		sizes = new int[dim];
		for(i=0; i<dim; i++) sizes[i] = 1;
	}
	
	int tpos = detPos(pos);
	int lpos;
	int szt = 1;
	int *spos = new int[dim];
	int *mway = new int[dim+1];
	int mdif0, mdif1, mdif2;
	mway[0] = 0;
	for(i=0; i<dim; i++)
	{
		szt *= sizes[i];
		spos[i] = 0;
		mdif0 = 1;
		mdif1 = 1;
		for(j=0; j<=i; j++) {mdif0 *= naxes[j]; mdif1 *= sizes[j]; mdif2 = sizes[j]*mway[i];}
		mway[i+1] = mdif0 - mdif1 - mdif2;
	}
	i=0;
	lpos = 0;
	int tmit, k;
	while(i<szt)
	{
		x[i] = getUsh(tpos + i + lpos);
		
		for(j=0; j<dim; j++)
		{
			spos[j]++;
			tmit = 1;
			for(k=0; k<=j; k++) tmit*=sizes[k];
			if(spos[j]==tmit) {spos[j]-= tmit; lpos +=mway[j+1];}
		}
		i++;
	}
	return 0;
}

int multidim::get(short *x, int *pos, int *sizes)
{
	int i, j;
	if(dataType!=0) return 1;
	if(sizes==NULL)
	{
		sizes = new int[dim];
		for(i=0; i<dim; i++) sizes[i] = 1;
	}
	
	int tpos = detPos(pos);
	int lpos;
	int szt = 1;
	int *spos = new int[dim];
	int *mway = new int[dim+1];
	int mdif0, mdif1, mdif2;
	mway[0] = 0;
	for(i=0; i<dim; i++)
	{
		szt *= sizes[i];
		spos[i] = 0;
		mdif0 = 1;
		mdif1 = 1;
		for(j=0; j<=i; j++) {mdif0 *= naxes[j]; mdif1 *= sizes[j]; mdif2 = sizes[j]*mway[i];}
		mway[i+1] = mdif0 - mdif1 - mdif2;
	}
	i=0;
	lpos = 0;
	int tmit, k;
	while(i<szt)
	{
		x[i] = getSh(tpos + i + lpos);
		
		for(j=0; j<dim; j++)
		{
			spos[j]++;
			tmit = 1;
			for(k=0; k<=j; k++) tmit*=sizes[k];
			if(spos[j]==tmit) {spos[j]-= tmit; lpos +=mway[j+1];}
		}
		i++;
	}
	return 0;
}

int multidim::get(long *x, int *pos, int *sizes)
{
	int i, j;
	if(dataType!=0) return 1;
	if(sizes==NULL)
	{
		sizes = new int[dim];
		for(i=0; i<dim; i++) sizes[i] = 1;
	}
	
	int tpos = detPos(pos);
	int lpos;
	int szt = 1;
	int *spos = new int[dim];
	int *mway = new int[dim+1];
	int mdif0, mdif1, mdif2;
	mway[0] = 0;
	for(i=0; i<dim; i++)
	{
		szt *= sizes[i];
		spos[i] = 0;
		mdif0 = 1;
		mdif1 = 1;
		for(j=0; j<=i; j++) {mdif0 *= naxes[j]; mdif1 *= sizes[j]; mdif2 = sizes[j]*mway[i];}
		mway[i+1] = mdif0 - mdif1 - mdif2;
	}
	i=0;
	lpos = 0;
	int tmit, k;
	while(i<szt)
	{
		x[i] = getL(tpos + i + lpos);
		
		for(j=0; j<dim; j++)
		{
			spos[j]++;
			tmit = 1;
			for(k=0; k<=j; k++) tmit*=sizes[k];
			if(spos[j]==tmit) {spos[j]-= tmit; lpos +=mway[j+1];}
		}
		i++;
	}
	return 0;
}

int multidim::get(float *x, int *pos, int *sizes)
{
	int i, j;
	if(dataType!=0) return 1;
	if(sizes==NULL)
	{
		sizes = new int[dim];
		for(i=0; i<dim; i++) sizes[i] = 1;
	}
	
	int tpos = detPos(pos);
	int lpos;
	int szt = 1;
	int *spos = new int[dim];
	int *mway = new int[dim+1];
	int mdif0, mdif1, mdif2;
	mway[0] = 0;
	for(i=0; i<dim; i++)
	{
		szt *= sizes[i];
		spos[i] = 0;
		mdif0 = 1;
		mdif1 = 1;
		for(j=0; j<=i; j++) {mdif0 *= naxes[j]; mdif1 *= sizes[j]; mdif2 = sizes[j]*mway[i];}
		mway[i+1] = mdif0 - mdif1 - mdif2;
	}
	i=0;
	lpos = 0;
	int tmit, k;
	while(i<szt)
	{
		x[i] = getF(tpos + i + lpos);
		
		for(j=0; j<dim; j++)
		{
			spos[j]++;
			tmit = 1;
			for(k=0; k<=j; k++) tmit*=sizes[k];
			if(spos[j]==tmit) {spos[j]-= tmit; lpos +=mway[j+1];}
		}
		i++;
	}
	return 0;
}

int multidim::get(double *x, int *pos, int *sizes)
{
	int i, j;
	if(dataType!=0) return 1;
	if(sizes==NULL)
	{
		sizes = new int[dim];
		for(i=0; i<dim; i++) sizes[i] = 1;
	}
	
	int tpos = detPos(pos);
	int lpos;
	int szt = 1;
	int *spos = new int[dim];
	int *mway = new int[dim+1];
	int mdif0, mdif1, mdif2;
	mway[0] = 0;
	for(i=0; i<dim; i++)
	{
		szt *= sizes[i];
		spos[i] = 0;
		mdif0 = 1;
		mdif1 = 1;
		for(j=0; j<=i; j++) {mdif0 *= naxes[j]; mdif1 *= sizes[j]; mdif2 = sizes[j]*mway[i];}
		mway[i+1] = mdif0 - mdif1 - mdif2;
	}
	i=0;
	lpos = 0;
	int tmit, k;
	while(i<szt)
	{
		x[i] = getD(tpos + i + lpos);
		
		for(j=0; j<dim; j++)
		{
			spos[j]++;
			tmit = 1;
			for(k=0; k<=j; k++) tmit*=sizes[k];
			if(spos[j]==tmit) {spos[j]-= tmit; lpos +=mway[j+1];}
		}
		i++;
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int multidim::set(unsigned short x, int *pos)
{
	if(dataType!=0) return 1;
	if(!isin(pos)) return 2;
	
	ushD[detPos(pos)] = x;
	return 0;
}

int multidim::set(short x, int *pos)
{
	if(dataType!=1) return 1;
	if(!isin(pos)) return 2;
	
	shD[detPos(pos)] = x;
	return 0;
}

int multidim::set(long x, int *pos)
{
	if(dataType!=2) return 1;
	if(!isin(pos)) return 2;
	
	lD[detPos(pos)] = x;
	return 0;
}
int multidim::set(float x, int *pos)
{
	if(dataType!=3) return 1;
	if(!isin(pos)) return 2;
	
	fD[detPos(pos)] = x;
	return 0;
}
int multidim::set(double x, int *pos)
{
	if(dataType!=4) return 1;
	if(!isin(pos)) return 2;
	
	dD[detPos(pos)] = x;
	return 0;
}
	
void multidim::set(unsigned short *fdata, int len)
{
	if(dataType!=0) return;
	if(len>size) return;
	if(len==0) len = size;
	
	for(int i; i<len; i++) ushD[i] = fdata[i];
}

void multidim::set(short *fdata, int len)
{
	if(dataType!=1) return;
	if(len>size) return;
	if(len==0) len = size;
	
	for(int i; i<len; i++) shD[i] = fdata[i];
}

void multidim::set(long *fdata, int len)
{
	if(dataType!=2) return;
	if(len>size) return;
	if(len==0) len = size;
	
	for(int i; i<len; i++) lD[i] = fdata[i];
}

void multidim::set(float *fdata, int len)
{
	if(dataType!=3) return;
	if(len>size) return;
	if(len==0) len = size;
	
	for(int i; i<len; i++) fD[i] = fdata[i];
}

void multidim::set(double *fdata, int len)
{
	if(dataType!=4) return;
	if(len>size) return;
	if(len==0) len = size;
	
	for(int i; i<len; i++) dD[i] = fdata[i];
}
///////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

int multidim::set(unsigned short *x, int *pos, int *sizes)
{
	int i, j;
	if(dataType!=0) return 1;
	if(sizes==NULL)
	{
		sizes = new int[dim];
		for(i=0; i<dim; i++) sizes[i] = 1;
	}
	
	int tpos = detPos(pos);
	int lpos;
	int szt = 1;
	int *spos = new int[dim];
	int *mway = new int[dim+1];
	int mdif0, mdif1, mdif2;
	mway[0] = 0;
	for(i=0; i<dim; i++)
	{
		szt *= sizes[i];
		spos[i] = 0;
		mdif0 = 1;
		mdif1 = 1;
		for(j=0; j<=i; j++) {mdif0 *= naxes[j]; mdif1 *= sizes[j]; mdif2 = sizes[j]*mway[i];}
		mway[i+1] = mdif0 - mdif1 - mdif2;
	}
	i=0;
	lpos = 0;
	int tmit, k;
	while(i<szt)
	{
		if((tpos + i + lpos)<size) ushD[tpos + i + lpos] = x[i];
		
		for(j=0; j<dim; j++)
		{
			spos[j]++;
			tmit = 1;
			for(k=0; k<=j; k++) tmit*=sizes[k];
			if(spos[j]==tmit) {spos[j]-= tmit; lpos +=mway[j+1];}
		}
		i++;
	}
	return 0;
}

int multidim::set(short *x, int *pos, int *sizes)
{
	int i, j;
	if(dataType!=0) return 1;
	if(sizes==NULL)
	{
		sizes = new int[dim];
		for(i=0; i<dim; i++) sizes[i] = 1;
	}
	
	int tpos = detPos(pos);
	int lpos;
	int szt = 1;
	int *spos = new int[dim];
	int *mway = new int[dim+1];
	int mdif0, mdif1, mdif2;
	mway[0] = 0;
	for(i=0; i<dim; i++)
	{
		szt *= sizes[i];
		spos[i] = 0;
		mdif0 = 1;
		mdif1 = 1;
		for(j=0; j<=i; j++) {mdif0 *= naxes[j]; mdif1 *= sizes[j]; mdif2 = sizes[j]*mway[i];}
		mway[i+1] = mdif0 - mdif1 - mdif2;
	}
	i=0;
	lpos = 0;
	int tmit, k;
	while(i<szt)
	{
		if((tpos + i + lpos)<size) shD[tpos + i + lpos] = x[i];
		
		for(j=0; j<dim; j++)
		{
			spos[j]++;
			tmit = 1;
			for(k=0; k<=j; k++) tmit*=sizes[k];
			if(spos[j]==tmit) {spos[j]-= tmit; lpos +=mway[j+1];}
		}
		i++;
	}
	return 0;
}

int multidim::set(long *x, int *pos, int *sizes)
{
	int i, j;
	if(dataType!=0) return 1;
	if(sizes==NULL)
	{
		sizes = new int[dim];
		for(i=0; i<dim; i++) sizes[i] = 1;
	}
	
	int tpos = detPos(pos);
	int lpos;
	int szt = 1;
	int *spos = new int[dim];
	int *mway = new int[dim+1];
	int mdif0, mdif1, mdif2;
	mway[0] = 0;
	for(i=0; i<dim; i++)
	{
		szt *= sizes[i];
		spos[i] = 0;
		mdif0 = 1;
		mdif1 = 1;
		for(j=0; j<=i; j++) {mdif0 *= naxes[j]; mdif1 *= sizes[j]; mdif2 = sizes[j]*mway[i];}
		mway[i+1] = mdif0 - mdif1 - mdif2;
	}
	i=0;
	lpos = 0;
	int tmit, k;
	while(i<szt)
	{
		if((tpos + i + lpos)<size) lD[tpos + i + lpos] = x[i];
		
		for(j=0; j<dim; j++)
		{
			spos[j]++;
			tmit = 1;
			for(k=0; k<=j; k++) tmit*=sizes[k];
			if(spos[j]==tmit) {spos[j]-= tmit; lpos +=mway[j+1];}
		}
		i++;
	}
	return 0;
}

int multidim::set(float *x, int *pos, int *sizes)
{
	int i, j;
	if(dataType!=0) return 1;
	if(sizes==NULL)
	{
		sizes = new int[dim];
		for(i=0; i<dim; i++) sizes[i] = 1;
	}
	
	int tpos = detPos(pos);
	int lpos;
	int szt = 1;
	int *spos = new int[dim];
	int *mway = new int[dim+1];
	int mdif0, mdif1, mdif2;
	mway[0] = 0;
	for(i=0; i<dim; i++)
	{
		szt *= sizes[i];
		spos[i] = 0;
		mdif0 = 1;
		mdif1 = 1;
		for(j=0; j<=i; j++) {mdif0 *= naxes[j]; mdif1 *= sizes[j]; mdif2 = sizes[j]*mway[i];}
		mway[i+1] = mdif0 - mdif1 - mdif2;
	}
	i=0;
	lpos = 0;
	int tmit, k;
	while(i<szt)
	{
		if((tpos + i + lpos)<size) fD[tpos + i + lpos] = x[i];
		
		for(j=0; j<dim; j++)
		{
			spos[j]++;
			tmit = 1;
			for(k=0; k<=j; k++) tmit*=sizes[k];
			if(spos[j]==tmit) {spos[j]-= tmit; lpos +=mway[j+1];}
		}
		i++;
	}
	return 0;
}

int multidim::set(double *x, int *pos, int *sizes)
{
	int i, j;
	if(dataType!=0) return 1;
	if(sizes==NULL)
	{
		sizes = new int[dim];
		for(i=0; i<dim; i++) sizes[i] = 1;
	}
	
	int tpos = detPos(pos);
	int lpos;
	int szt = 1;
	int *spos = new int[dim];
	int *mway = new int[dim+1];
	int mdif0, mdif1, mdif2;
	mway[0] = 0;
	for(i=0; i<dim; i++)
	{
		szt *= sizes[i];
		spos[i] = 0;
		mdif0 = 1;
		mdif1 = 1;
		for(j=0; j<=i; j++) {mdif0 *= naxes[j]; mdif1 *= sizes[j]; mdif2 = sizes[j]*mway[i];}
		mway[i+1] = mdif0 - mdif1 - mdif2;
	}
	i=0;
	lpos = 0;
	int tmit, k;
	while(i<szt)
	{
		if((tpos + i + lpos)<size) dD[tpos + i + lpos] = x[i];
		
		for(j=0; j<dim; j++)
		{
			spos[j]++;
			tmit = 1;
			for(k=0; k<=j; k++) tmit*=sizes[k];
			if(spos[j]==tmit) {spos[j]-= tmit; lpos +=mway[j+1];}
		}
		i++;
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


int multidim::getSize()
{
	return size;
}

void multidim::clear()
{
	long i, sZ;
	sZ = getSize();
	
	for(i=0; i<sZ; i++)
	{
		switch(dataType)
		{
		case 0:
			ushD[i] = 0;
			break;
		case 1:
			shD[i] = 0;
			break;
		case 2:
			lD[i] = 0;
			break;
		case 3:
			fD[i] = 0;
			break;
		case 4:
			dD[i] = 0;
			break;
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////

img2d::img2d()
{
	ushD = NULL;//0
	shD = NULL;//1
	lD = NULL;//2
	fD = NULL;//3
	dD = NULL;//4
//	naxes = NULL;
	size = 0;
	dim = 2;
	bpix = -1;
}

img2d::img2d(const int dType, const long n, const long m)
{
	ushD = NULL;//0
	shD = NULL;//1
	lD = NULL;//2
	fD = NULL;//3
	dD = NULL;//4
//	naxes = new int[dim];
	size = 1;
	naxes[0] = n;
	naxes[1] = m;
	size = n*m;
	
	dim = 2;
	bpix = dType;
	qDebug() << "\nbpix\t" << bpix << "\n";
	qDebug() << "\nsize\t" << size << "\n";
	switch(bpix)
	{
	case USHORT_IMG:
		qDebug() << "\nUSHORT_IMG\t" << USHORT_IMG << "\n";
		
		ushD = new unsigned short[size];
//		ushD = (unsigned short*) calloc(size, 16);
		qDebug() << "\nsizeof " << sizeof(ushD) << "\n";
		break;
	case SHORT_IMG:
		qDebug() << "\nSHORT_IMG\t" << SHORT_IMG << "\n";
		shD = new short[size];
		break;
	case LONG_IMG:
		qDebug() << "\nLONG_IMG\t" << LONG_IMG << "\n";
		lD = new long[size];
		break;
	case FLOAT_IMG:
		qDebug() << "\nFLOAT_IMG\t" << FLOAT_IMG << "\n";
		fD = new float[size];
		break;
	case DOUBLE_IMG:
		qDebug() << "\nDOUBLE_IMG\t" << DOUBLE_IMG << "\n";
		dD = new double[size];
		break;
	default:
		qDebug() << "\ndefault\t" << DOUBLE_IMG << "\n";
		bpix = DOUBLE_IMG;
		dD = new double[size];
		break;
	}
}

img2d::~img2d()
{
	if(ushD!=NULL) delete [] ushD;//0
	if(shD!=NULL) delete [] shD;//1
	if(lD!=NULL) delete [] lD;//2
	if(fD!=NULL) delete [] fD;//3
	if(dD!=NULL) delete [] dD;//4
	delete [] naxes;
}

img2d::img2d(unsigned short *fdata, long n, long m)
{
	bpix = USHORT_IMG;
//	naxes = new int[n*m];
	size = n*m;
	naxes[0] = n;
	naxes[1] = m;
	dim = 2;
	ushD = new unsigned short[size];
	for(int i=0; i<size; i++) ushD[i] = fdata[i];
}

img2d::img2d(short *fdata, long n, long m)
{
	bpix = SHORT_IMG;
//	naxes = new int[n*m];
	size = n*m;
	naxes[0] = n;
	naxes[1] = m;
	dim = 2;
	ushD = new unsigned short[size];
	for(int i=0; i<size; i++) shD[i] = fdata[i];
}

img2d::img2d(long *fdata, long n, long m)
{
	bpix = LONG_IMG;
//	naxes = new int[n*m];
	size = n*m;
	naxes[0] = n;
	naxes[1] = m;
	dim = 2;
	ushD = new unsigned short[size];
	for(int i=0; i<size; i++) lD[i] = fdata[i];
}

img2d::img2d(float *fdata, long n, long m)
{
	bpix = FLOAT_IMG;
//	naxes = new int[n*m];
	size = n*m;
	naxes[0] = n;
	naxes[1] = m;
	dim = 2;
	ushD = new unsigned short[size];
	for(int i=0; i<size; i++) fD[i] = fdata[i];
}

img2d::img2d(double *fdata, long n, long m)
{
	bpix = DOUBLE_IMG;
//	naxes = new int[n*m];
	size = n*m;
	naxes[0] = n;
	naxes[1] = m;
	dim = 2;
	ushD = new unsigned short[size];
	for(int i=0; i<size; i++) dD[i] = fdata[i];
}


int img2d::get(unsigned short *f, int i, int j, int size0, int size1)
{
	if(bpix!=USHORT_IMG) return 1;
	int k, l, m;
	m = 0;
	
	for(k=j; k<j+size1; k++)
	{
		for(l=i; l<i+size0; l++)
		{
			f[m] = ushD[detPos(l, k)]*isin(l,k);
			m++;
		}
	}
	
	return 0;
}

int img2d::get(short *f, int i, int j, int size0, int size1)
{
	if(bpix!=SHORT_IMG) return 1;
	int k, l, m;
	m = 0;
	
	for(k=j; k<j+size1; k++)
	{
		for(l=i; l<i+size0; l++)
		{
			f[m] = shD[detPos(l, k)]*isin(l,k);
			m++;
		}
	}
	
	return 0;
}

int img2d::get(long *f, int i, int j, int size0, int size1)
{
	if(bpix!=LONG_IMG) return 1;
	int k, l, m;
	m = 0;
	
	for(k=j; k<j+size1; k++)
	{
		for(l=i; l<i+size0; l++)
		{
			f[m] = lD[detPos(l, k)]*isin(l,k);
			m++;
		}
	}
	
	return 0;
}

int img2d::get(float *f, int i, int j, int size0, int size1)
{
	if(bpix!=FLOAT_IMG) return 1;
	int k, l, m;
	m = 0;
	
	for(k=j; k<j+size1; k++)
	{
		for(l=i; l<i+size0; l++)
		{
			f[m] = fD[detPos(l, k)]*isin(l,k);
			m++;
		}
	}
	
	return 0;

}
int img2d::get(double *f, int i, int j, int size0, int size1)
{
	if(bpix!=DOUBLE_IMG) return 1;
	int k, l, m;
	m = 0;
	
	for(k=j; k<j+size1; k++)
	{
		for(l=i; l<i+size0; l++)
		{
			f[m] = dD[detPos(l, k)]*isin(l,k);
			m++;
		}
	}
	
	return 0;

}

int img2d::set(unsigned short *f, int i, int j, int size0, int size1)
{
	if(bpix!=USHORT_IMG) return 1;
	int k, l, m;
	m = 0;
	
	for(k=j; k<j+size1; k++)
	{
		for(l=i; l<i+size0; l++)
		{
			if(isin(l,k))
			{
				ushD[detPos(l, k)] = f[m];
				m++;
			}
		}
	}
	
	return 0;
}

int img2d::set(short *f, int i, int j, int size0, int size1)
{
	if(bpix!=SHORT_IMG) return 1;
	int k, l, m;
	m = 0;
	
	for(k=j; k<j+size1; k++)
	{
		for(l=i; l<i+size0; l++)
		{
			if(isin(l,k))
			{
				shD[detPos(l, k)] = f[m];
				m++;
			}
		}
	}
	
	return 0;
}

int img2d::set(long *f, int i, int j, int size0, int size1)
{
	if(bpix!=LONG_IMG) return 1;
	int k, l, m;
	m = 0;
	
	for(k=j; k<j+size1; k++)
	{
		for(l=i; l<i+size0; l++)
		{
			if(isin(l,k))
			{
				lD[detPos(l, k)] = f[m];
				m++;
			}
		}
	}
	
	return 0;
}

int img2d::set(float *f, int i, int j, int size0, int size1)
{
	if(bpix!=FLOAT_IMG) return 1;
	int k, l, m;
	m = 0;
	
	for(k=j; k<j+size1; k++)
	{
		for(l=i; l<i+size0; l++)
		{
			if(isin(l,k))
			{
				fD[detPos(l, k)] = f[m];
				m++;
			}		
		}
	}
	
	return 0;
}

int img2d::set(double *f, int i, int j, int size0, int size1)
{
	if(bpix!=DOUBLE_IMG) return 1;
	int k, l, m;
	m = 0;
	
	for(k=j; k<j+size1; k++)
	{
		for(l=i; l<i+size0; l++)
		{
			if(isin(l,k))
			{
				dD[detPos(l, k)] = f[m];
				m++;
			}
		}
	}
	
	return 0;
}

/*
int img2d::set(unsigned short *fdata, int len);
int img2d::set(short *fdata, int len);
int img2d::set(long *fdata, int len);
int img2d::set(float *fdata, int len);
int img2d::set(double *fdata, int len);
*/

int img2d::detPos(int n, int m)
{
	if(!isin(n, m)) return 0;
	return naxes[0]*m + n;
}

int img2d::isin(int i, int j)
{
	return (i>=0)&&(j>=0)&&(i<naxes[0])&&(j<naxes[1]);
}

int img2d::getSgt(segment *sg, int i, int j, int size0, int size1)
{
//	if(bpix!=DOUBLE_IMG) return 1;
	int k, l, m;
	m = 0;
	
//	sg = new segment(i, j, naxes[0], naxes[1]);
	sg->x0 = i;
	sg->y0 = j;
	sg->size0 = size0;
	sg->size1 = size1;
	
	for(k=j; k<j+size1; k++)
	{
		for(l=i; l<i+size0; l++)
		{
			switch(bpix)
			{
			case USHORT_IMG:
				sg->data[m] = ushD[detPos(l, k)]*isin(l,k);
				break;
			case SHORT_IMG:
				sg->data[m] = shD[detPos(l, k)]*isin(l,k);
				break;
			case LONG_IMG:
				sg->data[m] = lD[detPos(l, k)]*isin(l,k);
				break;
			case FLOAT_IMG:
				sg->data[m] = fD[detPos(l, k)]*isin(l,k);
				break;
			case DOUBLE_IMG:
				sg->data[m] = dD[detPos(l, k)]*isin(l,k);
				break;
			}
			printf("%f\n", sg->data[m]);
			m++;
		}
	}
	
	return 0;

}

int img2d::getSgtC(segment *sg, int i, int j, int size0, int size1)
{
	int i0, j0;
	i0 = i - size0/2;
	j0 = j - size1/2;
	return getSgt(sg, i0, j0, size0, size1);
}

int img2d::getAperture(imgAperture *iApe, int Xc, int Yc, int ro)
{
	int i,j;//counters
	double r;//current distance from Xc,Yc
	qint64 ImgBuffSize = ro*ro; //size of aperture buffer
	PPIX PA = new PIX[ImgBuffSize*sizeof(PIX)];//PPIX buffer
	quint16 n = 0;
  //quint32 k;
	double minI = -1;
	double maxI = -1;
	double meanI = 0;
	quint16 X_max = 0;
	quint16 Y_max = 0;
	quint16 P_max = 0;
//	QMessageBox::information(0,"r", QString("%1 %2\n%3 %4\n%5 %6").arg(Xc-ro).arg(Xc+ro).arg(Yc-ro).arg(Yc+ro).arg(naxes[0]).arg(naxes[1]),QMessageBox::Ok,0,0);
  //
	for (i=Xc-ro;i<Xc+ro;i++)
	{
		for (j=Yc-ro;j<Yc+ro;j++)
		{
			if ((i>=0)&&(i<naxes[0])&&(j>=0)&&(j<naxes[1]))
			{
				r = sqrt(pow(i-Xc,2)+pow(j-Yc,2));
//				QMessageBox::information(0,"r", QString("%1").arg(r),QMessageBox::Ok,0,0);
				if (r<=ro)
				{
					PA[n].X = i;
					PA[n].Y = j;
					PA[n].I = getImgPix(i, j);
					///////////
					if (minI == -1) minI = PA[n].I;
					if (maxI == -1) maxI = PA[n].I;
					if (PA[n].I>maxI) 
					{
						maxI = PA[n].I;
						X_max = i;
						Y_max = j;
						P_max = n;
					}				   
					if (PA[n].I<minI) minI = PA[n].I;
					meanI = meanI + PA[n].I;
					///////////
					n++;
				}
         }			
       }//end for j	   
   }//end for i
   
   if(n==0) return 1;
   meanI = meanI/n;
//   QMessageBox::information(0,"poinT", QString("%1").arg(meanI),QMessageBox::Ok,0,0);
  ///////////////aperture buffer is filled
//   CCDDATA RES;
   iApe->buffer = new PIX[n*sizeof(PIX)];
   double seI = 0;
   for (i=0;i<n;i++)
    {
	  iApe->buffer[i] = PA[i];
	  seI = seI+pow((PA[i].I - meanI),2);
	}
	seI = sqrt(seI/(n-1));
	iApe->Size = n;
	iApe->boxW = 2*ro;
	iApe->boxH = 2*ro;
	iApe->X = Xc;
	iApe->Y = Yc;
	iApe->maxADC = maxI;
	iApe->minADC = minI;
	iApe->meanADC = meanI;
	iApe->seADC = seI;
	iApe->Xmax = X_max;
	iApe->Ymax = Y_max;
	iApe->Pmax = P_max;
//	QMessageBox::information(0,"iApe", QString("%1\t%2\t%3\n%4 %5").arg(iApe->meanADC).arg(iApe->minADC).arg(iApe->maxADC).arg(iApe->Xmax).arg(iApe->Ymax),QMessageBox::Ok,0,0);
	delete[] PA;
	return 0;

}

double img2d::getImgPix(int i, int j)
{
	if(!isin(i, j)) return 0.0;
	switch(bpix)
	{
		case USHORT_IMG:
		 return (double)ushD[detPos(i, j)];
		break;
	case SHORT_IMG:
		return (double)shD[detPos(i, j)];
		break;
	case LONG_IMG:
		return (double)lD[detPos(i, j)];
		break;
	case FLOAT_IMG:
		return (double)fD[detPos(i, j)];
		break;
	case DOUBLE_IMG:
		return (double)dD[detPos(i, j)];
		break;
	}
	return 0.0;
}

double img2d::getPos(int pos)
{
	if(pos>size) return 0.0;
	switch(bpix)
	{
		case USHORT_IMG:
			return (double)ushD[pos];
			break;
		case SHORT_IMG:
			return (double)shD[pos];
			break;
		case LONG_IMG:
			return (double)lD[pos];
			break;
		case FLOAT_IMG:
			return (double)fD[pos];
			break;
		case DOUBLE_IMG:
			return (double)dD[pos];
			break;
	}
	return 0.0;
}

int img2d::setVal(double f, long pos)
{
	if(pos>size) return 1;
	switch(bpix)
	{
		case USHORT_IMG:
			ushD[pos] = f;
			break;
		case SHORT_IMG:
			shD[pos] = f;
			break;
		case LONG_IMG:
			lD[pos] = f;
			break;
		case FLOAT_IMG:
			fD[pos] = f;
			break;
		case DOUBLE_IMG:
			dD[pos] = f;
			break;
	}
	return 0;
}
///////////////////////////////////////////////////////

segment::segment()
{
	size0 = size1 = 0;
	data = NULL;
	x0 = y0 = -1;
}

segment::segment(int n, int m)
{
	x0 = y0 = -1;
	size0 = n; size1 = m;
	data = new double[n*m];
}

segment::segment(double *fd, int i, int j, int n, int m)
{
	x0 = i;
	y0 = j;
	size0 = n; size1 = m;
	data = new double[n*m];
	for(int k=0; k<size0*size1; k++) data[k] = fd[k];
}

segment::segment(int i, int j, int n, int m)
{
	x0 = i;
	y0 = j;
	size0 = n; size1 = m;
	data = new double[n*m];
//	for(int k=0; k<size0*size1; k++) data[k] = fd[k];
}


segment::~segment()
{
	delete [] data;
}

double segment::get(int i, int j)
{
	if(i>=0&&j>=0&&i<size0&&j<size1) return data[i+j*size0];
	return 0.0;
}
