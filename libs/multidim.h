#ifndef ASTRO_H

#include "../astro/astro.h"

#endif

#include "../mb/mb.h"

#include <inttypes.h>

//#include "./../libs/fitsio.h"

#include "./../libs/ccdmeasurements.h"



#ifndef MULTIDIM_H

#define USHORT_TYPE 0
#define SHORT_TYPE 1
#define LONG_TYPE 2
#define FLOAT_TYPE 3
#define DOUBLE_TYPE 4

#define BYTE_IMG      8  /* BITPIX code values for FITS image types */
#define SHORT_IMG    16
#define LONG_IMG     32
#define LONGLONG_IMG 64
#define FLOAT_IMG   -32
#define DOUBLE_IMG  -64
                         /* The following 2 codes are not true FITS         */
                         /* datatypes; these codes are only used internally */
                         /* within cfitsio to make it easier for users      */
                         /* to deal with unsigned integers.                 */
#define SBYTE_IMG    10
#define USHORT_IMG   20
#define ULONG_IMG    40



struct measureParam;

struct measureParam
{
    int model;
    int nofit;
    double delta;
    int ringradius;
    int ringwidth;
    int lb;
    int sg;
    int apRadius;   //aperture radius
};


/*
apr - aperture radius (in pixels)
rr - ring internal radius (in pixels)
rw - ring width (in pixels)
sg = 1 if subtraction of gradient is requared. overwise sg=0
lb = 1 if fpix must be calculated within the ring around aperture as mean pixel value, overwise lb =0
model = {0,1,2} - model of PSF (see description of psffit function)
nofit - number of iteration (see description of psffit function)
delta - order of the model (see description of psffit function)
*/

class histogramData
{
public:
    long *hD;//hisogramm
    long hsize;//size of histogramm
    double maxv,minv,meanv,fpix,maxp,minp,st,hw;
    long maxH;
    long nelements;

    histogramData();
    ~histogramData();
};




struct sizes2d // РІРµРєС‚РѕСЂ СЂР°Р·РјРµСЂРѕРІ

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

    axeshelper(int n, int m){axes = new int[2]; dim = 2; axes[0]=n; axes[1]=m; } // Р·РґРµСЃСЊ СЂР°Р·Р±РёСЂР°РµРјСЃСЏ СЃРѕ РІСЃРµРјРё СЂР°Р·РјРµСЂРЅРѕСЃС‚СЏРјРё

	axeshelper(int n){axes = new int[1]; dim = 1; axes[0]=n; }

    // РґРѕР±Р°РІРёС‚СЊ РєРѕРЅСЃС‚СЂСѓРєС‚РѕСЂС‹ РґР»СЏ РґСЂСѓРіРёС… СЂР°Р·РјРµСЂРЅРѕСЃС‚РµР№ РїРѕ РІРєСѓСЃСѓ

	operator int const* () const { return axes; }

};



class segment			//РєРІР°РґСЂР°С‚РЅС‹Р№ С„СЂР°РіРјРµРЅС‚

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
/*
	unsigned short *ushD;//0
	short          *shD;//1
	long           *lD;//2
	float          *fD;//3
	double         *dD;//4
*/

    uint16_t *ushD;//0
    int16_t *shD;//1
    int32_t *lD;//2
    float *fD;//3
    double *dD;//4



	int *naxes;

	int dataType;

        long size;

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

/*	multidim(int dType, int n, int m);

	multidim(unsigned short *fdata, int n, int m);

	multidim(short *fdata, int n, int m);

	multidim(long *fdata, int n, int m);

	multidim(float *fdata, int n, int m);

	multidim(double *fdata, int n, int m);*/

//



	int isin(int *pos);

        long detPos(int *pos);

	

	int get(unsigned short *x, int *pos, int *sizes = NULL);		//РїРѕР»СѓС‡РёС‚СЊ РїСЂСЏРјРѕСѓРіРѕР»СЊРЅС‹Р№ СЌР»РµРјРµРЅС‚ 

	int get(short *x, int *pos, int *sizes = NULL);					//pos - Р»РµРІС‹Р№ РІРµСЂС…РЅРёР№ СѓРіРѕР»

	int get(long *x, int *pos, int *sizes = NULL);					//sizes - СЂР°Р·РјРµСЂС‹ РїРѕ РѕСЃСЏРј

	int get(float *x, int *pos, int *sizes = NULL);

	int get(double *x, int *pos, int *sizes = NULL);

	

	int set(unsigned short x, int *pos);							//СЌР»РµРјРµРЅС‚Сѓ pos РЅР°Р·РЅР°С‡РёС‚СЊ Р·РЅР°С‡РµРЅРёРµ x

	int set(short x, int *pos);

	int set(long x, int *pos);

	int set(float x, int *pos);

	int set(double x, int *pos);

	

	void set(unsigned short *fdata, int len = 0);					//Р·Р°РїРѕР»РЅРёС‚СЊ СЃРѕРѕС‚РІРµС‚СЃС‚РІСѓСЋС‰РёРµ РјР°СЃСЃРёРІС‹ РґР°РЅРЅС‹РјРё СЃ РїРµСЂРІРѕРіРѕ СЌР»РµРјРµРЅС‚Р°

	void set(short *fdata, int len = 0);							//len - РґР»РёРЅРЅР° РјР°СЃСЃРёРІР° fdata

	void set(long *fdata, int len = 0);

	void set(float *fdata, int len = 0);

	void set(double *fdata, int len = 0);

	

	int set(unsigned short *fdata, int* pos, int *sizes = NULL);	//СѓСЃС‚Р°РЅРѕРІРёС‚СЊ РїСЂСЏРјРѕСѓРіРѕР»СЊРЅС‹Р№ СЌР»РµРјРµРЅС‚ 

	int set(short *fdata, int* pos, int *sizes = NULL);				//pos - Р»РµРІС‹Р№ РІРµСЂС…РЅРёР№ СѓРіРѕР»

	int set(long *fdata, int* pos, int *sizes = NULL);				//sizes - СЂР°Р·РјРµСЂС‹ РїРѕ РѕСЃСЏРј

	int set(float *fdata, int* pos, int *sizes = NULL);

	int set(double *fdata, int* pos, int *sizes = NULL);

	

        long getSize();

	

	void clear();

};



class img2d

{

	

public:



        uint16_t *ushD;//0 unsigned short

        int16_t          *shD;//1

        int32_t           *lD;//2

	float          *fD;//3

	double         *dD;//4

	long naxes[2];

	int bpix;

	long size;

	int dim;

        histogramData *histD;

        long getNelements();
        int updateHist();
	

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

        uchar getImgPixValue(int x, int y);

        uchar getPixValue(int x, int y);



	double getPos(int pos);

	

	int setVal(double f, long pos);

	

	int getAperture(imgAperture *iApe, int Xc, int Yc, int ro);
        void getRing(double *const ap, int cx, int cy, int rho1, int rho2, int width, int height);


        bool getStimp(double *P, int width, int height, double fpx, int x, int y, measureParam params);
        bool getStimp(double *P, int width, int height, double fpx, int x, int y, int apr, int rr, int rw, int sg, int lb, int model, int& nofit, double delta);

/*	

	int set(unsigned short *fdata, int len);

	int set(short *fdata, int len);

	int set(long *fdata, int len);

	int set(float *fdata, int len);

	int set(double *fdata, int len);*/

};



#define MULTIDIM_H

#endif

