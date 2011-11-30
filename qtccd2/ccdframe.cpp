#include <QtGui>
#include "ccdframe.h"
#include "math.h"
#include "./../libs/fitsio.h"
#include "./../astro/astro.h"

CCDFrame::CCDFrame(QString FileName)
{
    QFile file(FileName);
    if (file.open(QIODevice::ReadOnly)){
        CCDFileName = FileName;
        //
        QDataStream in(&file);
        in.setByteOrder(QDataStream::LittleEndian);
        in >> XSize;
        in >> YSize;
        in >> CCDpixbits;
        quint16 CCDempty1;
        quint8 CCDempty2;
        in >> CCDempty1;
        in >> CCDempty2;
        char *header=new char[30];
        file.read(header,30);
        CCDhead.append(header);
        delete[] header;    
        qint64 CCDSize = XSize*YSize*sizeof(quint16); 
        CCDbuffer = new char[CCDSize];
        CCDdata = (quint16*)CCDbuffer;
        file.read(CCDbuffer,CCDSize);
        file.close();
        ///////////////////////////////
        getRange(CCDdata);
        quint32 imsize = XSize*YSize;
        CCDImage = new uchar[imsize];
        ////////////////////////
        colormap.resize(256);
        for (quint16 j=0; j<256;j++) colormap[j]=qRgb(j,j,j);
    }
    else QMessageBox::information(0,"qtccd message","can't open file",QMessageBox::Ok,0,0);

};

CCDFrame::CCDFrame(uchar *FrameBuff, quint16 x_size, quint16 y_size, unsigned short digOffset)
{
  YSize = x_size;
  XSize = y_size;
  offset = digOffset;
  //qint64 CCDSize = XSize*YSize*sizeof(quint16); 
  //FrameBuffer = new uchar[CCDSize];
  quint16* fr_data = (quint16*)FrameBuff;
  long nelements = XSize*YSize;
  CCDdata = new unsigned short[nelements];
  CCDdata_im = new unsigned short[nelements];
  //
   quint32 idx1,idx2;
   for (int x=0;x<x_size;x++)
   {
      for (int y=0;y<y_size;y++)
	  {
	    idx1 = y*x_size + x;
		idx2 = x*y_size + y;
		CCDdata[idx2] = fr_data[idx1]+offset;
	  }
   }
  ////////////////////////
  int wa32 = XSize + (4*(XSize%4!=0)-XSize%4);
  quint32 imsize = wa32*YSize;
  CCDImage = new uchar[imsize];
  colormap.resize(256);
  for (quint16 j=0; j<256;j++) colormap[j]=qRgb(j,j,j);
}//camera constructor

CCDFrame::CCDFrame(unsigned short *FrameBuff, quint16 x_size, quint16 y_size)
{
  XSize = x_size;
  YSize = y_size;
  long nelements = XSize*YSize;
  CCDdata = new unsigned short[nelements];
  CCDdata_im = new unsigned short[nelements];
   for (long i=0;i<nelements;i++)
   {
		CCDdata[i] = FrameBuff[i];
   }
  ////////////////////////
  int wa32 = XSize + (4*(XSize%4!=0)-XSize%4);
  quint32 imsize = wa32*YSize;
  CCDImage = new uchar[imsize];
  colormap.resize(256);
  for (quint16 j=0; j<256;j++) colormap[j]=qRgb(j,j,j);
}//camera constructor

void CCDFrame::setData_im()
{
  long nelements = XSize*YSize; 
  for (long i=0; i<nelements; i++) CCDdata_im[i] = CCDdata[i];
};


void CCDFrame::applyDark(unsigned short *CCDd, unsigned short *dark_d)
{
 long nelements = XSize*YSize;
 for (long i=0;i<nelements;i++) CCDd[i] = CCDd[i] - dark_d[i];	
};

   
void CCDFrame::applyFlat(unsigned short *CCDd, double *flat_d)
{
long nelements = XSize*YSize;
 for (long i=0;i<nelements;i++) CCDd[i] = (unsigned short)((double)CCDd[i]*flat_d[i]); 
};



quint16 CCDFrame::getValue(unsigned short *CCDd, quint16 x_pos, quint16 y_pos)
{
     quint32 i = y_pos*XSize+x_pos;
     return CCDd[i];
};

void CCDFrame::getRange(unsigned short *CCDd)
{
    MaxV = CCDd[0];
    MinV = CCDd[0];
    double meanV = 0;
	quint32 i;
    quint32 imsize = XSize*YSize;
    for (i=0;i<imsize;i++) 
       {
		 if (MaxV<CCDd[i])MaxV=CCDd[i];
         if (MinV>CCDd[i])MinV=CCDd[i];
         meanV = meanV+CCDdata[i];
       }//end for
    MeanV = (quint16)(meanV/imsize);
	histsize = quint32(MaxV - MinV+1);
    hist = new quint32[histsize];
    for (i=0;i<histsize;i++) hist[i] = 0;
    for (i=0;i<imsize;i++) 
         {
           hist[(int)(CCDd[i]-MinV)]++; 
         }//end for
	maxHist = 0;
	for (i=0;i<histsize;i++) if (hist[i]>hist[maxHist]) maxHist = i;
	maxHist = maxHist + MinV;
	//if (histsize<255)
	//	offsetBC = histsize/2;
	//else 
	    offsetBC = 127;//(quint16)histsize/100;
};//end getRange()


void CCDFrame::setImgData(unsigned short *CCDd, quint16 minV, quint16 maxV, double Gamma, int inverse)
{
	quint32 imsize = XSize*YSize;
    double valueI=0;
    double diap=maxV-minV;
    double valueU=0;
	Gamma = 1/Gamma;
	int wa32 = XSize + (4*(XSize%4!=0)-XSize%4);
	quint32 i,j;
    if (diap>0){
       for (int x=0;x<XSize;x++)
       {	   
	    for (int y=0;y<YSize;y++)
         {
		    i = x+y*XSize;
            valueU = (double)CCDd[i];
            if (valueU<minV)valueU=minV;
            if (valueU>maxV)valueU=maxV;       
            if (inverse)
			   valueI = 255 - 255*pow((valueU-minV)/diap,Gamma);
			else 
			   valueI = 255*pow((valueU-minV)/diap,Gamma);
			j = x + y*wa32;
            CCDImage[j] = (uchar)valueI;
         }//end for
	   }
    }//diap>0

}//setImgData



void CCDFrame::saveFitsFile(QString FileName)
{
    FileName = "!"+FileName;
    QByteArray fn = FileName.toAscii();
    char *fname = fn.data();
	//////////////
	fitsfile *fptr; /* pointer to the FITS file; defined in fitsio.h */
	int status;
	int  naxis = 2;
	LONGLONG nelements;
	long naxes[2] = { XSize, YSize }; 
	status = 0; /* initialize status before calling fitsio routines */
	fits_create_file(&fptr, fname, &status); /* create new file */
	/* Write a keyword; must pass the ADDRESS of the value */
	fits_create_hdu(fptr, &status);
	fits_create_img(fptr, USHORT_IMG, naxis, naxes, &status);
	//=================================
	fits_update_key(fptr, TDOUBLE, "EXPTIME", &exptime, "total exposure time in sek", &status);
	fits_update_key(fptr, TSHORT, "CCDCSTEMP", &cstemp, "temperature of cold side (deg C)", &status);
	fits_update_key(fptr, TSHORT, "CCDHSTEMP", &hstemp, "temperature of hot side (deg C)", &status);
	fits_update_key(fptr, TDOUBLE, "FOCALSETTING", &focalLength, "focusing tube position in mm", &status);
	fits_update_key(fptr, TINT, "AIRTEMP", &airTemperature, "temperature of outer air (deg C)", &status);
	QByteArray baTarget = sTarget.toAscii();
	char *chTarget = baTarget.data();
	fits_update_key(fptr, TSTRING, "TARGET", chTarget, "target", &status);
	QByteArray baRA = sRA.toAscii();
	char *chRA = baRA.data();
	fits_update_key(fptr, TSTRING, "RA", chRA, "right ascension J2000", &status);
	QByteArray baDE = sDE.toAscii();
	char *chDE = baDE.data();
	fits_update_key(fptr, TSTRING, "DEC", chDE, "declination J2000", &status);
	QByteArray baUTC = sUTC.toAscii();
	char *chUTC = baUTC.data();
	fits_update_key(fptr, TSTRING, "DATE-OBS", chUTC, "UTC of start(Trimble GPS and Accutime)", &status);
	QByteArray baeUTC = eUTC.toAscii();
	char *cheUTC = baeUTC.data();
	fits_update_key(fptr, TSTRING, "DATE-END", cheUTC, "UTC of end(Trimble GPS and Accutime)", &status);
	char chOrigin[26] = "Pulkovo Normal astrograph";
	fits_update_key(fptr, TSTRING, "ORIGIN",chOrigin , "instrument", &status);
	//
	QByteArray baFilterName = filterName.toAscii();
	char *chFilterName = baFilterName.data();
	fits_update_key(fptr, TSTRING, "FILTER", chFilterName, "filter", &status);
	QByteArray baObserverNames = observerNames.toAscii();
	char *chObserverNames = baObserverNames.data();
	fits_update_key(fptr, TSTRING, "OBSERVER", chObserverNames, "observer(s) name(s)", &status);
	//
	LONGLONG fpixel = 1; 
	nelements = naxes[0] * naxes[1]; /* number of pixels to write */
	fits_write_img(fptr, TUSHORT, fpixel, nelements, CCDdata, &status);
	fits_close_file(fptr, &status); /* close the file */
};

CCDFrame::~CCDFrame()
{
  if(CCDImage) delete CCDImage;
  if(CCDdata) delete CCDdata;
  if(CCDdata_im) delete CCDdata_im;
  if(hist) delete hist;
  if(CCDbuffer) delete CCDbuffer;
};
