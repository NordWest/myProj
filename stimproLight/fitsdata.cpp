#include "fitsdata.h"
#include <astro.hpp>
#include <mb.hpp>
//#include <mb.hpp>


//#include <QtGui>

fitsdata::fitsdata(QString fitsFileName)
{
	int fitsErrors[20];//buffer to store errors codes. Codes will be added to the buffer after each call of cfitsio function
	fileName = fitsFileName;
	//char *fname = fitsFileName.toAscii().data();//converting of QString to char* for file name
	fitsfile *fptr;//pointer ot FITSfile
	int status = 0;//status
	fits_open_file(&fptr, qPrintable(fileName)/*fname*/ , READONLY,&status);fitsErrors[0]=status;status = 0;//open file
	fits_get_img_equivtype(fptr, &bpix, &status);fitsErrors[1]=status;status = 0;//determination of type of data (bpix)
	fits_get_img_size(fptr, 2, naxes, &status);fitsErrors[2]=status;status = 0;//determination of size of the image
	//BEGIN read header
	int nkeys;//variable to store number of line of the header
	fits_get_hdrspace(fptr, &nkeys, NULL, &status);fitsErrors[3]=status;status = 0;//determination of number of line of the header
	char card[FLEN_CARD];//buffer to read keys from header
	fitsHeader="";//strings from header
	for (int k = 1; k <= nkeys; k++)//reading strings from header
	{
	   fits_read_record(fptr, k, card, &status);// read separate record
	   fitsHeader = fitsHeader + QString(card)+"\n";// add record to fitsHeader
	}
	fitsErrors[4]=status;
	status = 0;
	//END read header
	///////////////////
	//BEGIN read positional and time data
	char strkey[FLEN_CARD];// buffer to read keys "DATE-OBS" and "EXPTIME"
	fits_read_key(fptr, TSTRING, "DATE-OBS", strkey, NULL, &status);fitsErrors[5]=status;status = 0;//read "DATE-OBS"
	fits_read_key(fptr, TDOUBLE, "EXPTIME", &exptime, NULL, &status);fitsErrors[6]=status;status = 0;//read "EXPTIME"
	if(fitsErrors[5]==0)
	{
		QString dateObsStr = QString(strkey);//convert "DATE-OBS" to QString
		MJD = getMJDfromStrFTN(dateObsStr, exptime);//determine MJD
                mjd1 = getMJDfromStrFTN(dateObsStr, 0);//determine mjd1
	}
	else MJD = 0;
        ////////////////////////////////////////////////////////
        fits_read_key(fptr, TSTRING, "DATE-END", strkey, NULL, &status);fitsErrors[5]=status;status = 0;//read "DATE-END"
        mjd2 = getMJDfromStrFTN(QString(strkey), 0);//determine mjd2
        et = (mjd2-mjd1)*86400;
	////////////////////////////////////////////////////////
	fits_read_key(fptr, TSTRING, "CTYPE1", strkey, NULL, &status);// try to read first WCS tag
	if(!status)//if WCS is present
	{
		double wcsV;
		//reading WCS tags and adding WCS values to WCSdata buffer
		fits_read_key(fptr, TDOUBLE, "CRPIX1", &wcsV, NULL, &status); WCSdata[0] = wcsV;
		fits_read_key(fptr, TDOUBLE, "CRPIX2", &wcsV, NULL, &status); WCSdata[1] = wcsV;
		fits_read_key(fptr, TDOUBLE, "CRVAL1", &wcsV, NULL, &status); WCSdata[2] = wcsV;
		fits_read_key(fptr, TDOUBLE, "CRVAL2", &wcsV, NULL, &status); WCSdata[3] = wcsV;
		fits_read_key(fptr, TDOUBLE, "CDELT1", &wcsV, NULL, &status); WCSdata[4] = wcsV;
		fits_read_key(fptr, TDOUBLE, "CDELT2", &wcsV, NULL, &status); WCSdata[5] = wcsV;
		fits_read_key(fptr, TDOUBLE, "CROTA1", &wcsV, NULL, &status); WCSdata[6] = wcsV;
		fits_read_key(fptr, TDOUBLE, "CROTA2", &wcsV, NULL, &status); WCSdata[7] = wcsV;
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		fits_read_key(fptr, TDOUBLE, "CD1_1", &wcsV, NULL, &status); WCSdata[8] = wcsV;
		fits_read_key(fptr, TDOUBLE, "CD1_2", &wcsV, NULL, &status); WCSdata[9] = wcsV;
		fits_read_key(fptr, TDOUBLE, "CD2_1", &wcsV, NULL, &status); WCSdata[10] = wcsV;
		fits_read_key(fptr, TDOUBLE, "CD2_2", &wcsV, NULL, &status); WCSdata[11] = wcsV;
		WCSdata[12] = 1;//flag of WCS: if it is equal to 1 then WCS is present 
		fitsErrors[7]=status;status = 0;
	}
	else 
	{//if WCS is absent
		status=0;
		fits_read_key(fptr, TSTRING, "RA", strkey, NULL, &status);fitsErrors[8]=status;status = 0;// read "RA" key 
		if(fitsErrors[8]==0)WCSdata[2] = hms_to_mas(QString(strkey),":")/3600000;//RA in deg is added to WCSdata buffer
		fits_read_key(fptr, TSTRING, "DEC", strkey, NULL, &status);fitsErrors[9]=status;status = 0;// read "DEC" key
		if(fitsErrors[9]==0)WCSdata[3] = damas_to_mas(QString(strkey),":")/3600000;//DEC in deg is added to WCSdata buffer
		WCSdata[12] = 0;//flag of WCS: if it is equal to 0 then WCS is absent 
	}
	//BEGIN read positional and time data
	//////////////////////////////////////////////////////
	//BEGIN read image data
	LONGLONG nelements = naxes[0]*naxes[1];//calculation of the number of elements (pixels) of the image
	int anynul = 0;//service variable
	long felem = 1;//service variable
	///////////////////////////////////////////////
	/*
	РќР° РјРѕР№ РІР·РіР»СЏРґ, СЃ С‚РѕС‡РєРё Р·СЂРµРЅРёСЏ РїСЂРѕРіСЂР°РјРјРёСЂРѕРІР°РЅРёСЏ, СѓРґРѕР±РЅРѕ Р·Р°РіСЂСѓР·РёС‚СЊ РІСЃРµ РґР°РЅРЅС‹Рµ (РѕС‚СЃС‡РµС‚С‹ РЅР° РїРёРєСЃРµР»СЏС…) РІ РјР°СЃСЃРёРІ double.
	РќРѕ СЌС‚Рѕ РїСЂРёРІРѕРґРёС‚ Рє РЅРµРѕРїСЂР°РІРґР°РЅРЅРѕРјСѓ СЂР°СЃС…РѕРґСѓ РѕРїРµСЂР°С‚РёРІРЅРѕР№ РїР°РјСЏС‚Рё (РЅР°РїСЂРёРјРµСЂ, РІРјРµСЃС‚Рѕ РґРІСѓС… Р±Р°Р№С‚ РЅР° РїРёРєСЃРµР», РјС‹ РІСЃРµРіРґР° РёРјРµРµРј 8) Рё СѓРјРµРЅСЊС€РµРЅРёСЋ СЃРєРѕСЂРѕСЃС‚Рё СЂР°Р±РѕС‚С‹.
	Р¤Р°Р№Р» РјРµРґР»РµРЅРЅРѕ РѕС‚РєСЂС‹РІР°РµС‚СЃСЏ, С‚Р°Рє РєР°Рє РЅРµРѕР±С…РѕРґРёРј С†РёРєР» РїРѕ РІСЃРµРј РїРёРєСЃРµР»СЏРј РґР»СЏ РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёСЏ РІ double. 
	РњРµРґР»РµРЅРЅРѕ РїРµСЂРµСЃС‡РёС‚С‹РІР°СЋС‚СЃСЏ Р·РЅР°С‡РµРЅРёСЏ РЅР° РїРёРєСЃРµР»Р°С… РїСЂРё РїСЂРѕСЂРёСЃРѕРІРєРµ РёР·РѕР±СЂР°Р¶РµРЅРёР№.
	РџРѕ СЌС‚РѕР№ РїСЂРёС‡РёРЅРµ С…СЂР°РЅРµРЅРёРµ Рё РґРѕСЃС‚СѓРї Рє РґР°РЅРЅС‹Рј РёР·РѕР±СЂР°Р¶РµРЅРёСЏ РѕСЃСѓС‰РµСЃС‚РІР»СЏРµС‚СЃСЏ РІ Р·Р°РІРёСЃРёРјРѕСЃС‚Рё РѕС‚ С‚РёРїР° РґР°РЅРЅС‹С… USHORT_IMG, SHORT_IMG, LONG_IMG, FLOAT_IMG РёР»Рё DOUBLE_IMG
	*/
	//QMessageBox::information(0,"stimpro",QString( "%1" ).arg(bpix,6,'f',1,QLatin1Char( ' ' )),QMessageBox::Ok,0,0);
	switch(bpix)
	{
		case USHORT_IMG:
		{
			ushD = new unsigned short[nelements];//allocation of appropriate tamporary buffer
			fits_read_img(fptr, TUSHORT, felem, nelements, NULL, ushD ,&anynul, &status);fitsErrors[10]=status;status = 0;//read data image from file
			break;
		}
		case SHORT_IMG:
		{
			shD = new short[nelements];
			fits_read_img(fptr, TSHORT, felem, nelements, NULL, shD ,&anynul, &status);fitsErrors[10]=status;status = 0;//read data image from file
			break;
		}
		case LONG_IMG:
		{
			lD = new long[nelements];
			fits_read_img(fptr, TLONG, felem, nelements, NULL, lD ,&anynul, &status);fitsErrors[10]=status;status = 0;//read data image from file
			break;
		}
		case FLOAT_IMG:
		{
			fD = new float[nelements];
			fits_read_img(fptr, TFLOAT, felem, nelements, NULL, fD, &anynul, &status);fitsErrors[10]=status;status = 0;//read data image from file
			break;
		}
		case DOUBLE_IMG:
		{
			dD = new double[nelements];
			fits_read_img(fptr, TDOUBLE, felem, nelements, NULL, dD, &anynul, &status);fitsErrors[10]=status;status = 0;//read data image from file
			break;
		}
	}
	///////////////////////////////////////
	//END read image data
	//////////////////////
	//BEGIN histogramm
	maxv = getPixelValue(0);
	minv = getPixelValue(0);
	meanv = 0;
	double pixv;
	for(LONGLONG i=0;i<nelements;i++)//determination of maximum, minimum and mean pixels values
	{
		pixv = getPixelValue(i);
		if(pixv>maxv)maxv=pixv;
		if(pixv<minv)minv=pixv;
		meanv = meanv + pixv;
	}
	meanv = meanv/nelements;
	hsize = (long)(maxv-minv+1);//calculation of size of the image histogramm array
	hD = new long[hsize];//allocate buffer for histogramm data
	for(int k=0;k<hsize;k++) hD[k]=0;//set zero for each pixel value in histogramm
	for(LONGLONG i=0;i<nelements;i++) hD[(LONGLONG)(getPixelValue(i)-minv)]++;//construction of the histogramm
	long fpn=0;//variable to store value of the most frequent pixel value
	for(int k=0;k<hsize;k++) if(hD[k]>hD[fpn])fpn=k;// determination of the most frequent pixel value as maximum of histogramm
	fpix = fpn + minv;// calculation of the most frequent pixel value according to minimum pixel value
	/*
	РґР»СЏ Р±РѕР»СЊС€РёРЅСЃС‚РІР° Р°СЃС‚СЂРѕРЅРѕРјРёС‡РµСЃРєРёС… РёР·РѕР±СЂР°Р¶РµРЅРёР№ С…Р°СЂР°РєС‚РµСЂРЅР° РіРёСЃС‚РѕРіСЂР°РјРјР° СЃ "РѕСЃС‚СЂС‹Рј" РїРёРєРѕРј. РџРѕСЃРєРѕР»СЊРєСѓ РїРѕС‡С‚Рё РІСЃРµ Р·РЅР°С‡РёРјС‹Рµ РґР»СЏ РїСЂРѕСЂРёСЃРѕРІРєРё РїРёРєСЃРµР»Рё Р»РµР¶Р°С‚ РІ РїСЂРµРґРµР»Р°С… 
	РїРёРєР°, СЂР°Р·СѓРјРЅРѕ СЃРґРµР»Р°С‚СЊ С‚Р°Рє, С‡С‚РѕР±С‹ РѕСЃС‚Р°Р»СЊРЅС‹Рµ РїРёРєСЃРµР»Рё РїСЂРё РїСЂРѕСЂРёСЃРѕРІРєРµ РЅРµ СѓС‡РёС‚С‹РІР°Р»РёСЃСЊ. 
	РџР°СЂР°РјРµС‚СЂ hw - РІС‹С‡РёСЃР»СЏРµС‚СЃСЏ РєР°Рє СЃСЂРµРґРЅРµРІР·РІРµС€РµРЅРЅРѕРµ Р·РЅР°С‡РµРЅРёСЏ РјРѕРґСѓР»СЏ СѓРєР»РѕРЅРµРЅРёСЏ РѕС‚ РјР°РєСЃРёРјСѓРјР° РіРёСЃС‚РѕРіСЂР°РјРјС‹. Р­С‚Рѕ РґР°РµС‚ "РєРѕРјС„РѕСЂС‚РЅС‹Рµ" РґР»СЏ РїСЂРѕСЃРјРѕС‚СЂР° РєР°СЂС‚РёРЅРєРё РїР°СЂР°РјРµС‚СЂС‹ "С‰РµР»Рё РїСЂРѕСЂРёСЃРѕРІРєРё"
	*/
	hw=0;
	double summWeights = 0;
	for(long k=0;k<hsize;k++){hw=hw+abs(k-fpn)*hD[k]/hD[fpn];summWeights=summWeights+hD[k]/hD[fpn];}
	hw = hw/summWeights;
	maxp = fpix+hw;
	if(maxp>maxv)maxp=maxv;
	minp = fpix-hw/3;
	if(minp<minv)minp=minv;
	st = 2*hw/25;
	//END histogramm
	p = new double[120];
	for(int k=0;k<8;k++) p[k]=0;
	psfX =0,psfY=0,psfRho=10;psfModel=0;
	p[7] = 1.5;
	p[2] = fpix;
};

fitsdata::~fitsdata()
{
	switch(bpix)
	{
		case USHORT_IMG:{delete[] ushD;break;}
		case SHORT_IMG: {delete[] shD;break;}
		case LONG_IMG:  {delete[] lD;break;}
		case FLOAT_IMG: {delete[] fD;break;}
		case DOUBLE_IMG:{delete[] dD;break;}
	}
	delete[] hD;
	delete[] p;
	//QMessageBox::information(0,"stimpro",QString("%1").arg(bpix,8,'f',0,QLatin1Char( ' ' )),QMessageBox::Ok,0,0);
};

double fitsdata::getPixelValue(LONGLONG k)
{
	double outRes = 0;
	switch(bpix)
	{
		case USHORT_IMG:{outRes = (double)ushD[k];break;}
		case SHORT_IMG: {outRes = (double)shD[k];break;}
		case LONG_IMG:  {outRes = (double)lD[k];break;}
		case FLOAT_IMG: {outRes = (double)fD[k];break;}
		case DOUBLE_IMG:{outRes = dD[k];break;}
	}
	return outRes;
};

double fitsdata::getPixelValueXY(int x, int y)
{
	double outRes = 0;
	if((x>=0)&&(x<naxes[0])&&(y>=0)&&(y<naxes[1]))
	{
		LONGLONG k = naxes[0]*y+x;
		switch(bpix)
		{
			case USHORT_IMG:{outRes = (double)ushD[k];break;}
			case SHORT_IMG: {outRes = (double)shD[k];break;}
			case LONG_IMG:  {outRes = (double)lD[k];break;}
			case FLOAT_IMG: {outRes = (double)fD[k];break;}
			case DOUBLE_IMG:{outRes = dD[k];break;}
		}
	}
	return outRes;
};

uchar fitsdata::getImgPixValue(int x, int y)
{
	uchar outRes = 0;
	if((x>=0)&&(x<naxes[0])&&(y>=0)&&(y<naxes[1]))
	{
		LONGLONG k = naxes[0]*y+x;
		double diap = maxp-minp;
		switch(bpix)
		{
			case USHORT_IMG:{outRes = (uchar)(255*(((ushD[k]<=maxp)&&(ushD[k]>=minp))*(ushD[k]-minp)/diap
							+(ushD[k]>maxp)));break;}
			case SHORT_IMG: {outRes = (uchar)(255*(((shD[k]<=maxp)&&(shD[k]>=minp))*(shD[k]-minp)/diap
							+(shD[k]>maxp)));break;}
			case LONG_IMG:  {outRes = (uchar)(255*(((lD[k]<=maxp)&&(lD[k]>=minp))*(lD[k]-minp)/diap
							+(lD[k]>maxp)));break;}
			case FLOAT_IMG: {outRes = (uchar)(255*(((fD[k]<=maxp)&&(fD[k]>=minp))*(fD[k]-minp)/diap
							+(fD[k]>maxp)));break;}
			case DOUBLE_IMG:{outRes = (uchar)(255*(((dD[k]<=maxp)&&(dD[k]>=minp))*(dD[k]-minp)/diap
							+(dD[k]>maxp)));break;}
		}
	}
	return outRes;
};

QString fitsdata::getEqCoordinatesOfPixel(int x, int y)
{
	if((x>=0)&&(x<naxes[0])&&(y>=0)&&(y<naxes[1]))
	{
		if(WCSdata[12]>0)
		{
			double cpos[2];
			getCposFromWCS(x, y, WCSdata, cpos);
			return "RA = "+mas_to_hms(cpos[0], ":", 3)+
                                ", Dec = "+mas_to_damas(cpos[1], ":", 2);
		}
		else return "WCS is not defined";
	}
	else return "";
};

void* fitsdata::getPixArray()
{
	switch(bpix)
		{
			case USHORT_IMG:{return (void*)ushD;break;}
			case SHORT_IMG: {return (void*)shD;break;}
			case LONG_IMG:  {return (void*)lD;break;}
			case FLOAT_IMG: {return (void*)fD;break;}
			case DOUBLE_IMG:{return (void*)dD;break;}
		}
	return 0;
};

void fitsdata::getAperture(double *const ap, int cx, int cy, int rho)
{
	int ulX = cx-rho;
	int ulY = cy-rho;
	double r;
	for(int y = 0; y<2*rho;y++)
		for(int x = 0; x<2*rho;x++)
		{
			r = sqrt(pow(x-rho,2)+pow(y-rho,2));
			if((ulX+x>=0)&&(ulX+x<naxes[0])&&(ulY+y>=0)&&(ulY+y<naxes[1])&&(r<=rho))
				switch(bpix)
				{
				  case USHORT_IMG:{ap[y*2*rho+x] = ushD[(ulY+y)*naxes[0]+(ulX+x)];break;}
				  case SHORT_IMG: {ap[y*2*rho+x] = shD[(ulY+y)*naxes[0]+(ulX+x)];break;}
				  case LONG_IMG:  {ap[y*2*rho+x] = lD[(ulY+y)*naxes[0]+(ulX+x)];break;}
				  case FLOAT_IMG: {ap[y*2*rho+x] = fD[(ulY+y)*naxes[0]+(ulX+x)];break;}
				  case DOUBLE_IMG:{ap[y*2*rho+x] = dD[(ulY+y)*naxes[0]+(ulX+x)];break;}
				}
			else ap[y*2*rho+x] = -10E-9;
		}
};

void fitsdata::getRing(double *const ap, int cx, int cy, int rho1, int rho2)
{
	int ulX = cx-rho2;
	int ulY = cy-rho2;
	double r;
	for(int y = 0; y<2*rho2;y++)
		for(int x = 0; x<2*rho2;x++)
		{
			r = sqrt(pow(x-rho2,2)+pow(y-rho2,2));
			if((ulX+x>=0)&&(ulX+x<naxes[0])&&(ulY+y>=0)&&(ulY+y<naxes[1])&&(r>=rho1)&&(r<=rho2))
				switch(bpix)
				{
				  case USHORT_IMG:{ap[y*2*rho2+x] = ushD[(ulY+y)*naxes[0]+(ulX+x)];break;}
				  case SHORT_IMG: {ap[y*2*rho2+x] = shD[(ulY+y)*naxes[0]+(ulX+x)];break;}
				  case LONG_IMG:  {ap[y*2*rho2+x] = lD[(ulY+y)*naxes[0]+(ulX+x)];break;}
				  case FLOAT_IMG: {ap[y*2*rho2+x] = fD[(ulY+y)*naxes[0]+(ulX+x)];break;}
				  case DOUBLE_IMG:{ap[y*2*rho2+x] = dD[(ulY+y)*naxes[0]+(ulX+x)];break;}
				}
			else ap[y*2*rho2+x] = -10E-9;
		}	
};

void fitsdata::cm(double *const cm, const double* fd, int cx, int cy, int rho)
{
	centerofmass(cm,fd,rho);
	cm[0] = cx-rho+cm[0];
	cm[1] = cy-rho+cm[1];
};

bool fitsdata::psffitting(const double* fd, int rho, int model)
{
        return psffit(p,fd,rho,model,25,0.05,0.005,5,n_it,0);
	//void psffit(double *const p, const double* fd, int rho, int model, int nit,
	// double kappa, double lambda, double regk, int& it)
}

double fitsdata::psfdata(int x, int y)
{
	 x = x -(psfX-psfRho);
	 y = y -(psfY-psfRho);
	 return getpsf(p,x,y,psfModel);
};

void fitsdata::getBackground(const double* fd, int rho2)
{
	p[2] = meanRingValue(fd, rho2);
};

void fitsdata::subtractBackground(const double* rfd, double *const fd, int rho1, int rho2)
{
	double *rp = new double[6];	
	ringfit(rp, rfd,rho2);
	QTextStream stream(stdout);
	stream <<"============\n";
	for (int i=0; i<6; i++)
			stream << QString("%1").arg( rp[i],10,'f',3,QLatin1Char( ' ' )) <<'\n';
	stream <<"============\n";
	subtractSurface(rp,fd, p[2],rho1);
	delete[] rp;
};

double fitsdata::corrvalue(int x, int y, int rho, double A)
{
    int nel = 4*rho*rho;
    double* fd = new double[nel];
    getAperture(fd, x, y, rho);
    double maxv,minv; for(int i=0;i<nel;i++)if(fd[i]!=-10E-9){maxv=fd[i];minv=fd[i];break;}
    int q,xm,ym;//for(int i=0;i<nel;i++) if(fd[i]!=-10E-9){n++;if(fd[i]>maxv){maxv=fd[i];im=i;}if(fd[i]<minv)minv=fd[i];}
    for(int y=0;y<2*rho;y++)
        for(int x=0;x<2*rho;x++)
        {
            q = y*2*rho+x;
            if(fd[q]!=-10E-9)
            {
                if(fd[q]>maxv){maxv=fd[q];xm=x;ym=y;}
                if(fd[q]<minv)minv=fd[q];
            }
        }
    int xc = rho;
    int yc = rho;
    q = yc*2*rho+xc;
    double Jc = (fd[q]-minv)/(maxv-minv);
    //double R = sqrt((xm-rho)*(xm-rho)+(ym-rho)*(ym-rho));
    //if((R==0)||(R>2)) {delete[] fd;return 0;}
    double mu1=0,mu2=0,Jm;
    for(int y=0;y<2*rho;y++)
        for(int x=0;x<2*rho;x++)
        {
            q = y*2*rho+x;
            if(fd[q]!=-10E-9)
            {
                Jm = 1/(1+sqrt((x-xc)*(x-xc)+(y-yc)*(y-yc)));
                mu1+=(Jc-(fd[q]-minv)/(maxv-minv))*(1-Jm);
                mu2+=(1-Jm)*(1-Jm);
            }
        }
    delete[] fd;
    return mu1/mu2;
};

double fitsdata::angletr(double X0, double Y0, double X1, double Y1,double X2, double Y2)
{
    double r1 = sqrt((X1-X0)*(X1-X0)+(Y1-Y0)*(Y1-Y0));
    double r2 = sqrt((X2-X0)*(X2-X0)+(Y2-Y0)*(Y2-Y0));
    double V1x = (X1-X0)/r1;
    double V1y = (Y1-Y0)/r1;
    double V2x = (X2-X0)/r2;
    double V2y = (Y2-Y0)/r2;
    return acos(V1x*V2x+V1y*V2y)*180/M_PI;
};

void fitsdata::findStars()
{
    cmX.clear();cmY.clear();
    int bsize = 40;
    int n = naxes[0]/bsize;
    int m = naxes[1]/bsize;
    int flag;
    int N=8;
    int XM[N];
    int YM[N];
    int xm,ym;
    double xmean,ymean,ex,ey,D,r;
    //
    for(int i=0;i<m;i++)
        for(int j=0;j<n;j++)
        {
           //XM.clear();YM.clear();
           for(int k=0;k<N;k++)
            {
               ym = i*bsize;xm =j*bsize;
               for(int y=i*bsize;y<(i+1)*bsize;y++)
                   for(int x=j*bsize;x<(j+1)*bsize;x++)
                       if(getPixelValueXY(x,y)>getPixelValueXY(xm,ym))
                        {
                            flag=0; for(int q=0;q<k;q++) if((XM[q]==x)&&(YM[q]==y)) flag=1;
                            if(flag==0){xm=x;ym=y;}
                        }
               XM[k] = xm; YM[k] = ym;
               //cmX<<xm;cmY<<ym;
            }
            xmean = 0; ymean = 0;for(int k=0;k<N;k++){xmean+=XM[k]; ymean+=YM[k];}
            xmean = xmean/N; ymean = ymean/N;
            ex = 0; ey = 0;for(int k=0;k<N;k++){ex+=(XM[k]-xmean)*(XM[k]-xmean); ey+=(YM[k]-ymean)*(YM[k]-ymean);}
            D = sqrt(ex/(N-1)+ey/(N-1));
            if(D<2)
            {
                flag=0;
                for(int k=0;k<cmX.count();k++)
                {
                    r=sqrt((xmean-cmX[k])*(xmean-cmX[k])+(ymean-cmY[k])*(ymean-cmY[k]));
                    if(r<bsize/4){cmX[k]=(cmX[k]+xmean)/2; cmY[k]=(cmY[k]+ymean)/2; flag=1; break;}
                }
                if(flag==0){cmX<<xmean;cmY<<ymean;}
            }
            //for(int k=0;){cmX<<xm;cmY<<ym;}
        }
   /* QVector<int> X,Y,nmatch;
    double r,mu;
    int rho=7;
    bool drecord;
    for(int y=0;y<naxes[1];y++)
        for(int x=0;x<naxes[0];x++)
        {
            mu = corrvalue(x,y,rho,0.2);
            if(mu>0.6)//)&&(mu<1.5))
            {
                drecord = false;
                for(int j=0;j<X.count();j++)
                {
                    r = (x-X[j])*(x-X[j])+(y-Y[j])*(y-Y[j]);
                    if(r<100){X[j]=(X[j]+x)/2; Y[j]=(Y[j]+y)/2; nmatch[j]=nmatch[j]+1;drecord = true; break;}
                }
                if(!drecord)
                    {X<<x;Y<<y;nmatch<<1;}
            }
        }
    for(int i=0;i<X.count();i++)if(nmatch[i]>2) {cmX<<X[i];cmY<<Y[i];}*/
    /*QVector<int> X,Y;
    //begin create bin map
    for(int y=0;y<naxes[1];y++)
        for(int x=0;x<naxes[0];x++)
            if(getPixelValueXY(x,y)>fpix+9) {X<<x;Y<<y;}
    //end create bin map
    QTextStream stream(stdout);
    //begin corr filter
    int rho = 20;
    bool drecord;
    double r,mu;
    //QVector<int> corrX,corrY,corrN;
    cmX.clear();cmY.clear();
    for(int i=0;i<X.count();i++)
    {
        mu = corrvalue(X[i],Y[i],rho,0.2);
        if((mu>0.8)&&(mu<1.5))
        {
            drecord = false;
            for(int j=0;j<cmX.count();j++)
            {
                r = (X[i]-cmX[j])*(X[i]-cmX[j])+(Y[i]-cmY[j])*(Y[i]-cmY[j]);
                if(r<100){cmX[j]=(cmX[j]+X[i])/2; cmY[j]=(cmY[j]+Y[i])/2; drecord = true; break;}
            }
            if(!drecord)
                {cmX<<X[i];cmY<<Y[i];
                    stream
                            << QString("%1").arg( X[i],10,'f',5,QLatin1Char( ' ' )) <<'|'
                            << QString("%1").arg( Y[i],10,'f',5,QLatin1Char( ' ' )) <<'|'
                            << QString("%1").arg( mu,10,'f',5,QLatin1Char( ' ' )) <<'\n';}
        }
    }
    //for(int i=0;i<corrX.count();i++)if(corrN[i]>5){cmX<<corrX[i];cmY<<corrY[i];}
    //end corr filter*/
    //begin estimate flux
    int s,rho=5;
    int nel = 4*rho*rho;
    double* fd = new double[nel];
    double* fdcm = new double[4*(rho+2)*(rho+2)];
    double flu;
    double cmass[2];
    for(int i=0;i<cmX.count();i++)
    {
        getAperture(fdcm, cmX[i], cmY[i], rho+2);
        cm(cmass,fdcm,cmX[i],cmY[i],rho+2);
        cmX[i] = (int)cmass[0];cmY[i] = (int)cmass[1];
        //
        getAperture(fd, cmX[i], cmY[i], rho);
        flu = 0;
        s=0;for(int j=0;j<nel;j++) if(fd[j]!=-10E-9){flu+=fd[j];s++;}
        flux << flu/s;
    }
    delete[] fd;
    delete[] fdcm;
    //end estimate flux*/
};

bool fitsdata::matchStars()
{
     QProcess ucac3findProcess;
     ucac3findProcess.setWorkingDirectory("/home/ymax/bin");
     ucac3findProcess.setProcessChannelMode(QProcess::MergedChannels);
     ucac3findProcess.setReadChannel(QProcess::StandardOutput);
     ucac3findProcess.start("/home/ymax/bin/ucac3find",
                QStringList::QStringList()<<mas_to_hms( WCSdata[2]*3600000,":",4)
                                           <<mas_to_damas(WCSdata[3]*3600000,":",3)
                                           <<"b=15x15"<<"/windows/E/cats/ucac3");
     ucac3findProcess.waitForFinished(-1);
    QTextStream ucac3Stream(ucac3findProcess.readAllStandardOutput());
    QTextStream stream(stdout);
    KSI.clear();ETA.clear();MAG.clear();//arrays of tangential positions of the reference stars
    double ra,dec,mag;
    double tang[2];
    QString u3Line;
    //cmX.clear();cmY.clear();
    while(!ucac3Stream.atEnd())
    {
        //stream <<ucac3Stream.readLine() <<'\n';
        u3Line = ucac3Stream.readLine();
        ra = hms_to_mas(u3Line.section('|',0,0),":");
        dec = damas_to_mas(u3Line.section('|',1,1),":");
        mag = u3Line.section('|',2,2).toDouble();
        getRaDeToTang(ra,dec,WCSdata[2]*3600000,WCSdata[3]*3600000,tang);
        if((mag>5)&&(mag<17)){KSI << tang[0];ETA << tang[1];MAG<<mag;}//select stars within 5 to 12 mag
        /*if((mag>0)&&(mag<15))
        {
            stream << QString("%1").arg( tang[0],10,'f',3,QLatin1Char( ' ' )) <<'|'
                    << QString("%1").arg( tang[1],10,'f',3,QLatin1Char( ' ' )) <<'|'
                    << QString("%1").arg( mag,10,'f',3,QLatin1Char( ' ' ))
                    <<'\n';
            //cmX << tang[0]+500;cmY <<-tang[1]+500;
        }*/
    }
    //begin sort arrays (by flux and MAG)
    QVector<int> snPix,snTang;
    for(int i=0;i<flux.count();i++)snPix<<-1;
    for(int i=0;i<MAG.count();i++)snTang<<-1;
    int currF;
    for(int i=0;i<snPix.count();i++)
    {
        for(int j=0;j<flux.count();j++)
        {
            currF=0;
            for(int k=0;k<snPix.count();k++) if(j==snPix[k]){currF=1;break;}
            if(!currF)
            {
                if(snPix[i]==-1)
                    snPix[i]=j;
                else
                    if(flux[j]>flux[snPix[i]])snPix[i]=j;
            }
        }
    }
    //
    for(int i=0;i<snTang.count();i++)
    {
        for(int j=0;j<MAG.count();j++)
        {
            currF=0;
            for(int k=0;k<snTang.count();k++) if(j==snTang[k]){currF=1;break;}
            if(!currF)
            {
                if(snTang[i]==-1)
                    snTang[i]=j;
                else
                    if(MAG[j]<MAG[snTang[i]])snTang[i]=j;
            }
        }
    }
    //end sort arrays
    //begin match stars
    /*stream << "\npix data\n";
    QStringList outList;
    for(int i=0;i<snPix.count();i++)
        outList
                <<QString("%1").arg( flux[i],10,'f',3,QLatin1Char( ' ' )) + '|'
                + QString("%1").arg( flux[snPix[i]],10,'f',3,QLatin1Char( ' ' )) + '\n';
     for(int i=0;i<snPix.count();i++) stream << outList[i];
     //
     stream << "\nstars data\n";
    QStringList outStList;
    for(int i=0;i<snTang.count();i++)
        outStList
                <<QString("%1").arg( MAG[i],10,'f',3,QLatin1Char( ' ' )) + '|'
                + QString("%1").arg( MAG[snTang[i]],10,'f',3,QLatin1Char( ' ' )) + '\n';
     for(int i=0;i<snTang.count();i++) stream << outStList[i];*/
    QVector<double>X,Y;for(int i=0;i<snPix.count();i++){X<<cmX[snPix[i]];Y<<cmY[snPix[i]];}//pixel positions sorted by flux
    QVector<double>ksi,eta;for(int i=0;i<snTang.count();i++){ksi<<KSI[snTang[i]];eta<<ETA[snTang[i]];}//tangential positins sorted by MAG
    //
    double angles[3];//triangulums angles in pixels data
    double ANGLES[3];//triangulums angles in tangential data
    double angPrec = 0.5;//deg precision of matched angles
    double distPrec = 4;//precision of matched distances between stars images
    QVector<int> pairs,PAIRS;//arrays of pairs of stars
    //vector and matrixs to solve the equation of pixel-to-sky transformation
    double pX[3];
    double pY[3];
    double CM[9];
    double Cx[3];
    double Cy[3];
    //
    QVector<double> rX,rY;
    QVector<int> matchN;
    double dist;
    int nOfMatches;
    int nOfFoundSt = 6;// maximal number of stars to match stellar images
    for(int i=0;i<nOfFoundSt-2;i++)
        for(int j=i+1;j<nOfFoundSt-1;j++)
            for(int k=j+1;k<nOfFoundSt;k++)
            {
                angles[0]=angletr(X[i],Y[i],X[j],Y[j],X[k],Y[k]);
                angles[1]=angletr(X[j],Y[j],X[i],Y[i],X[k],Y[k]);
                angles[2]=angletr(X[k],Y[k],X[i],Y[i],X[j],Y[j]);
                for(int I=0;I<ksi.count()-2;I++)
                    for(int J=I+1;J<ksi.count()-1;J++)
                        for(int K=J+1;K<ksi.count();K++)
                        {
                            //angles[0]=angletr(X[i],Y[i],X[j],Y[j],X[k],Y[k]);
                           // angles[1]=angletr(X[j],Y[j],X[i],Y[i],X[k],Y[k]);
                            //angles[2]=angletr(X[k],Y[k],X[i],Y[i],X[j],Y[j]);
                            //
                            ANGLES[0]=angletr(ksi[I],eta[I],ksi[J],eta[J],ksi[K],eta[K]);
                            ANGLES[1]=angletr(ksi[J],eta[J],ksi[I],eta[I],ksi[K],eta[K]);
                            ANGLES[2]=angletr(ksi[K],eta[K],ksi[I],eta[I],ksi[J],eta[J]);
                            //
                            pairs.clear();PAIRS.clear();
                            for(int m=0;m<3;m++)
                                for(int n=0;n<3;n++)
                                    if(fabs(angles[m]-ANGLES[n])<angPrec)//store in pairs and PAIRS arrays corresponding number of stellar images and stars from calalog subset
                                    {
                                        if(m==0)pairs<<i;
                                        if(m==1)pairs<<j;
                                        if(m==2)pairs<<k;
                                        if(n==0)PAIRS<<I;
                                        if(n==1)PAIRS<<J;
                                        if(n==2)PAIRS<<K;
                                    }
                            if(PAIRS.count()==3)//three pairs found and sky-to-pixel transformation may be found
                            {

                                for(int n=0;n<3;n++)
                                {
                                    pX[n]=X[pairs[n]];
                                    pY[n]=Y[pairs[n]];
                                    CM[0+n*3] = ksi[PAIRS[n]];CM[1+n*3] = eta[PAIRS[n]];CM[2+n*3] = 1;
                                }
                                ssleg(CM,Cx,pX,3);
                                ssleg(CM,Cy,pY,3);
                                rX.clear();rY.clear();
                                for(int m=0; m<KSI.count();m++)
                                {
                                    rX << Cx[0]*KSI[m]+Cx[1]*ETA[m]+Cx[2];
                                    rY << Cy[0]*KSI[m]+Cy[1]*ETA[m]+Cy[2];
                                }
                                nOfMatches = 0;
                                matchN.resize(cmX.count());
                                for(int n=0;n<cmX.count();n++)
                                {
                                    matchN[n]=-1;
                                    for(int m=0;m<rX.count();m++)
                                    {
                                        dist = sqrt((cmX[n]-rX[m])*(cmX[n]-rX[m])+(cmY[n]-rY[m])*(cmY[n]-rY[m]));
                                        if(dist<distPrec){matchN[n]=m;nOfMatches++;}
                                    }
                                }
                                if(nOfMatches>nOfFoundSt)
                                {

                                    //
                                    cmX.clear();cmY.clear();
                                    for(int m=0; m<KSI.count();m++)
                                    {
                                        cmX << Cx[0]*KSI[m]+Cx[1]*ETA[m]+Cx[2];
                                        cmY << Cy[0]*KSI[m]+Cy[1]*ETA[m]+Cy[2];
                                    }
                                    stream
                                        <<QString("%1").arg(nOfMatches,5,10,QLatin1Char( ' ' ))<<'\n'
                                        <<QString("%1").arg(et,10,'f',3,QLatin1Char( ' ' ))<<'\n';
                                    //
                                    int rho = 10;
                                    int nel = 4*rho*rho;
                                    double flux;
                                   // double uweFl;
//                                    QVector<double> magD,fluxD,etD;
                                    for(int m=0; m<cmX.count();m++)
                                    {
                                        if(matchN[m]!=-1)
                                        {
                                            //begin estimate flux
                                            double* fd = new double[nel];
                                            flux = 0;
                                            getAperture(fd, cmX[m], cmY[m], rho);
                                            for(int n=0;n<nel;n++) if(fd[n]!=-10E-9)flux+=fd[n];
                                            flux = flux/nel;
                                            delete[] fd;
                                            if((flux!=0)&&(MAG[matchN[m]]<10))
                                            {
                                                stream
                                                    <<QString("%1").arg(et,8,'f',3,QLatin1Char( ' ' ))<<'|'
                                                    <<QString("%1").arg(flux,12,'f',5,QLatin1Char( ' ' ))<<'|'
                                                    <<QString("%1").arg(MAG[matchN[m]],10,'f',3,QLatin1Char( ' ' ))<<'\n';
                                                //magD << MAG[matchN[m]];
                                                //fluxD << flux;
                                                //etD << et;
                                            }
                                            //end estimate flux
                                        }
                                    }
                                    /*
                                    double* magv = new double[magD.count()];
                                    double* CMV = new double[magD.count()*3];
                                    for(int m=0;m<magD.count();m++)
                                    {
                                        magv[m] = magD[m];
                                        CMV[0+m*3] = log10(fluxD[m]);
                                        CMV[1+m*3] = etD[m];
                                        CMV[2+m*3] = 1;
                                    }
                                    double* ZM = new double[3];
                                    double* DM = new double[9];
                                    lsm(3,magD.count(),ZM,CMV,magv,uweFl,DM);
                                    stream
                                        <<QString("%1").arg(sqrt(uweFl),8,'f',3,QLatin1Char( ' ' ))<<'\n'
                                        <<QString("%1").arg(ZM[0],12,'f',5,QLatin1Char( ' ' ))<<'|'
                                        <<QString("%1").arg(sqrt(uweFl*DM[0]),12,'f',5,QLatin1Char( ' ' ))<<'\n'
                                        <<QString("%1").arg(ZM[1],12,'f',5,QLatin1Char( ' ' ))<<'|'
                                        <<QString("%1").arg(sqrt(uweFl*DM[4]),12,'f',5,QLatin1Char( ' ' ))<<'\n'
                                        <<QString("%1").arg(ZM[2],12,'f',5,QLatin1Char( ' ' ))<<'|'
                                        <<QString("%1").arg(sqrt(uweFl*DM[8]),12,'f',5,QLatin1Char( ' ' ))<<'\n';
                                    */
                                    return true;
                                }

                            }
                        }
            }
                /*stream
                        <<QString("%1").arg(i,5,10,QLatin1Char( ' ' ))<<'|'
                        <<QString("%1").arg(j,5,10,QLatin1Char( ' ' ))<<'|'
                        <<QString("%1").arg(k,5,10,QLatin1Char( ' ' ))<<'\n';*/
    //end match stars
    return false;
};
