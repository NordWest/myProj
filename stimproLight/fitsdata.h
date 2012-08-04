#ifndef FITSDATA_H
#define FITSDATA_H
#include <QtCore>
#include "fitsio.h"


class fitsdata: public QObject
{
 Q_OBJECT
 
signals:

    
private slots:
	
	
public:
	//functions
	fitsdata(QString fitsFileName);//РєРѕРЅСЃС‚СЂСѓРєС‚РѕСЂ. Р’ РЅРµРј СЂРµР°Р»РёР·СѓРµС‚СЃСЏ РѕС‚РєСЂС‹С‚РёРµ С„Р°Р№Р»Р° fitsFileName, РІС‹С‡РёСЃР»СЏСЋС‚СЃСЏ СЂР°Р·Р»РёС‡РЅС‹Рµ РїР°СЂР°РјРµС‚СЂС‹ РєР°РґСЂР° Рё Р·Р°РіСЂСѓР¶Р°РµС‚СЃСЏ РјР°СЃСЃРёРІ РґР°РЅРЅС‹С… РёР·РѕР±СЂР°Р¶РµРЅРёСЏ (РѕС‚СЃС‡РµС‚С‹ РЅР° РєР°Р¶РґРѕРј РїРёРєСЃРµР»Рµ)
	~fitsdata();//РґРµСЃС‚СЂСѓРєС‚РѕСЂ. Р’ РЅРµРј РЅР°РґРѕ РѕСЃРІРѕР±РѕРґРёС‚СЊ Р±СѓС„РµСЂР° РїР°РјСЏС‚Рё, С‡С‚РѕР±С‹ РЅРµ Р±С‹Р»Рѕ РїРµСЂРµРїРѕР»РЅРµРЅРёР№ РїСЂРё РѕС‚РєСЂС‹С‚РёРё Рё СѓРґР°Р»РµРЅРёРё Р±РѕР»СЊС€РѕРіРѕ С‡РёСЃР»Р° РєР°РґСЂРѕРІ
	double getPixelValue(LONGLONG k);//РґР°РµС‚ РѕС‚СЃС‡РµС‚ РЅР° РїРёРєСЃРµР»Рµ СЃ РЅРѕРјРµСЂРѕРј k = 0....naxes[0]*naxes[1]-1 РєР°Рє double. k = x+y*naxes[0], x,y - РєРѕРѕСЂРґРёРЅР°С‚С‹ РїРёРєСЃРµР»СЏ;
	double getPixelValueXY(int x, int y);
	uchar getImgPixValue(int x, int y);//РґР°РµС‚ Р·РЅР°С‡РµРЅРёРµ РїРёРєСЃРµР»СЏ РґР»СЏ РїСЂРѕСЂРёСЃРѕРІРєРё РёР·РѕР±СЂР°Р¶РµРЅРёСЏ РЅР° СЌРєСЂР°РЅРµ c СѓС‡РµС‚РѕРј РїР°СЂР°РјРµС‚СЂРѕРІ РїСЂРѕСЂРёСЃРѕРІРєРё. x =0...naxes[0]-1,y=0...naxes[1]-1 - РєРѕРѕСЂРґРёРЅР°С‚С‹ РїРёРєСЃРµР»СЏ
	QString getEqCoordinatesOfPixel(int x, int y);
	void* getPixArray();
	void getAperture(double *const ap, int cx, int cy, int rho);
	void getRing(double *const ap, int cx, int cy, int rho1, int rho2);
	void cm(double *const cm, const double* fd, int cx, int cy, int rho);
        bool psffitting(const double* fd, int rho, int model);
	void getBackground(const double* fd, int rho2);
	void subtractBackground(const double* rfd, double *const fd, int rho1, int rho2);
	double psfdata(int x, int y);
        double corrvalue(int x, int y, int rho, double A);
        double angletr(double X0, double Y0, double X1, double Y1,double X2, double Y2);
        void findStars();
        bool matchStars();
	//data
	int bpix;//bits per pixel (type of the pixels data)
	long naxes[2];//size of the image
	double WCSdata[13];//РјР°СЃСЃРёРІ, РІ РєРѕС‚РѕСЂРѕРј С…СЂР°РЅРёС‚СЃСЏ WCS-СЃРёСЃС‚РµРјР°
        double MJD,mjd1,mjd2;//РјРѕРґРёС„РёС†РёСЂРѕРІР°РЅРЅР°СЏ СЋР»РёР°РЅСЃРєР°СЏ РґР°С‚Р° РґР»СЏ РЅР° С†РµРЅС‚СЂР°Р»СЊРЅРѕРіРѕ РјРѕРјРµРЅС‚Р° СЌРєСЃРїРѕР·РёС†РёРё
        double exptime,et;// СЌРєСЃРїРѕР·РёС†РёСЏ РІ СЃРµРєСѓРЅРґР°С…
	QString fitsHeader, fileName;// СЃС‚СЂРѕРєР° fitsHeader СЃРѕРґРµСЂР¶РёС‚ Р·Р°РіРѕР»РѕРІРѕРє fits-С„Р°Р№Р»Р°, fileName - РёРјСЏ С„Р°Р№Р»Р°
	//begin raw image data
	// СѓРєР°Р·Р°С‚РµР»Рё РЅР° РјР°СЃСЃРёРІС‹, РІ РєРѕС‚РѕС‹Рµ РѕРїС†РёРѕРЅР°Р»СЊРЅРѕ Р·Р°РіСЂСѓР¶Р°СЋС‚СЃСЏ РґР°РЅРЅС‹Рµ РёР·РѕР±СЂР°Р¶РµРЅРёСЏ СЃРѕРѕС‚РІРµС‚СЃС‚РІСѓСЋС‰РµРіРѕ С‚РёРїР°. РќР°РїСЂРёРјРµСЂ, РµСЃР»Рё С‚РёРї РёР·РѕР±СЂР°Р¶РµРЅРёСЏ LONG_IMG, С‚Рѕ РІ РєРѕРЅСЃС‚СЂСѓРєС‚РѕСЂРµ РёРЅРёС†РёР°Р»РёР·РёСЂСѓРµС‚СЃСЏ С‚РѕР»СЊРєРѕ lD, РѕСЃС‚Р°Р»СЊРЅС‹Рµ СѓРєР°Р·Р°С‚РµР»Рё РѕСЃС‚Р°СЋС‚СЃСЏ NULL.
	unsigned short *ushD;//USHORT_IMG
	short          *shD;//SHORT_IMG
	long           *lD;//LONG_IMG
	float          *fD;//FLOAT_IMG
	double         *dD;//DOUBLE_IMG
	//end raw image data
	long *hD;//hisogramm
	long hsize;//size of histogramm
	double maxv,minv,meanv,fpix,maxp,minp,st,hw;
	int psfX,psfY,psfRho, psfModel,n_it;
        QVector<double> cmX;
        QVector<double> cmY;
        QVector<double> flux;
        QVector<double> KSI;
        QVector<double> ETA;
        QVector<double> MAG;
        //int numberOfFoundStrs;
	/*
	maxv - РјР°РєСЃРёРјР°Р»СЊРЅРѕРµ Р·РЅР°С‡РµРЅРёРµ РѕС‚СЃС‡РµС‚РѕРІ РЅР° РїРёРєСЃРµР»СЏС…,
	minv - РјРёРЅРёРјР°Р»СЊРЅРѕРµ Р·РЅР°С‡РµРЅРёРµ РѕС‚СЃС‡РµС‚РѕРІ РЅР° РїРёРєСЃРµР»СЏС…,
	meanv  - СЃСЂРµРґРЅРµРµ Р·РЅР°С‡РµРЅРёРµ РѕС‚СЃС‡РµС‚РѕРІ РЅР° РїРёРєСЃРµР»СЏС…,
	fpix - РЅР°РёР±РѕР»РµРµ С‡Р°СЃС‚Рѕ РІСЃС‚СЂРµС‡Р°СЋС‰РµРµСЃСЏ Р·РЅР°С‡РµРЅРёРµ РѕС‚СЃС‡РµС‚РѕРІ РЅР° РїРёРєСЃРµР»СЏС… (РјР°РєСЃРёРјСѓРј РіРёСЃС‚РѕРіСЂР°РјРјС‹ РёР·РѕР±СЂР°Р¶РµРЅРёСЏ),
	maxp - РјР°РєСЃРёРјР°Р»СЊРЅРѕРµ Р·РЅР°С‡РµРЅРёРµ РґР»СЏ "С‰РµР»Рё РїСЂРѕСЂРёСЃРѕРІРєРё",
	minp - РјРёРЅРёРјР°Р»СЊРЅРѕРµ Р·РЅР°С‡РµРЅРёРµ РґР»СЏ "С‰РµР»Рё РїСЂРѕСЂРёСЃРѕРІРєРё",
	"С‰РµР»Рё РїСЂРѕСЂРёСЃРѕРІРєРё" - СЌС‚Рѕ РёРЅС‚РµСЂРІР°Р» Р·РЅР°С‡РµРЅРёР№ РѕС‚СЃС‡РµС‚РѕРІ РЅР° РїРёРєСЃРµР»Р°С… РёСЃС…РѕРґРЅРѕРіРѕ РёР·РѕР±СЂР°Р¶РµРЅРёСЏ РѕС‚ minp РґРѕ maxp. Р”Р»СЏ СЌС‚РѕРіРѕ РґРёР°РїР°Р·РѕРЅР° РїСЂРё РїСЂРѕСЂРёСЃРѕРІРєРµ РЅР° СЌРєСЂР°РЅРµ Р·РЅР°С‡РµРЅРёРµ РєР°Р¶РґРѕРіРѕ РїРёРєСЃРµР»Р° РЅРѕСЂРјРёСЂСѓРµС‚СЃСЏ РЅР° РґРёР°РїР°Р·РѕРЅ Р±РµР·Р·РЅР°РєРѕРІРѕРіРѕ Р±Р°Р№С‚Р° (0..255).
	st - С€Р°Рі РґР»СЏ СЂСѓС‡РЅРѕРіРѕ РёР·РјРµРЅРµРЅРёСЏ maxp Рё minp,
	hw - "РґРёСЃРїРµСЂСЃРёСЏ" РіРёСЃС‚РѕРіСЂР°РјРјС‹ (С‡С‚Рѕ-С‚Рѕ РІСЂРѕРґРµ РґРёР°РїР°Р·РѕРЅР°, РІ РєРѕС‚РѕСЂРѕРј Р»РµР¶РёС‚ СЃСЂРµРґРЅРµРµ С‡РёСЃР»Рѕ РѕС‚СЃС‡РµС‚РѕРІ РЅР° РїРёРєСЃРµР»Р°С…). Р­С‚РѕС‚ РїР°СЂР°РјРµС‚СЂ РѕРїСЂРµРґРµР»СЏРµС‚ "С‰РµР»Рё РїСЂРѕСЂРёСЃРѕРІРєРё" РїСЂРё РѕС‚РєСЂС‹С‚РёРё С„Р°Р№Р»Р° Рё РїРѕ-СѓРјРѕР»С‡Р°РЅРёСЋ
	*/
	
	int is_scaled;				//С„Р»Р°Рі СЂР°Р±РѕС‚С‹ СЃ СѓРјРµРЅСЊС€РµРЅРЅС‹РјРё РІРµСЂСЃРёСЏРјРё Р±РѕР»СЊС€РёС… С„Р°Р№Р»РѕРІ СЃ С‚СЌРіРѕРј FSOURCE
	double *p;
	
private:
	
	
};
#endif
