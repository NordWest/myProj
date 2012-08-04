#ifndef CCDFRAME_H
#define CCDFRAME_H
#include <QObject>


class CCDFrame: public QObject
{

public:
    CCDFrame(QString FileName);
    CCDFrame(uchar *FrameBuff, quint16 x_size, quint16 y_size, unsigned short digOffset);
	CCDFrame(unsigned short *FrameBuff, quint16 x_size, quint16 y_size);
    ~CCDFrame();
	void setData_im();
    quint16 getValue(unsigned short *CCDd, quint16 x_pos, quint16 y_pos);
	void applyDark(unsigned short *CCDd, unsigned short *dark_d);
	void applyFlat(unsigned short *CCDd, double *flat_d);
    void getRange(unsigned short *CCDd);
    void setImgData(unsigned short *CCDd, quint16 minV, quint16 maxV, double Gamma, int inverse);
	void saveFitsFile(QString FileName);
    ////////////////////////////////////////////
    QString CCDFileName;
    QString CCDhead;
	QString observerNames;
	//QString targetName;
	int airTemperature;
	QString filterName;
	double focalLength;
    quint16 XSize, YSize;
    uchar* CCDImage;//view buffer
    QVector<QRgb> colormap;
    unsigned short *CCDdata;//ccd image data buffer
	unsigned short *CCDdata_im;//ccd image data buffer
    quint16 MaxV, MinV, MeanV, maxHist, offsetBC;
	quint32 *hist;//histogramm
	int histsize;
	//////////////////////////
	QString sRA, sDE, sUTC, eUTC, sTarget;
	double RA, DE, TA, ST, MJD, dUTC;
	double exptime; 
	short int cstemp, hstemp;
	unsigned short offset;

private:
    char*  CCDbuffer;
    quint8 CCDpixbits;
    
        
    
};

#endif
