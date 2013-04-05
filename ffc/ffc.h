#ifndef FFC_H
#define FFC_H

#include <QtCore>
#include <QtGui>
#include <fitsio.h>
#include <mb.h>

extern "C"
{
    double getMJD(double yr, double mm, double dd, double hr, double min, double sec);

    double getMJDfromStr(QString str);

    QString DATEfromMJD(double mjd, QString spl_symb, int format, int ndig);

    QString RAtoHMS(double ra, QString spl, int ndig);

    QString DEtoDAMAS(double de, QString spl, int ndig);

    double HMStoRA(QString strRA, QString sep);

    double DAMAStoDE(QString strDE, QString sep);

    void tangFromRADE(double ra, double de, double RA, double DE, double& ksi, double& eta);

    void RADEFromTang(double ksi, double eta, double RA, double DE, double& ra, double& de);

    double ptp_anvle(double ra1, double de1, double ra2, double de2);

    //void setWCS(double *WCSdata, double *Ax, double *Ay);

    void triangSRA(double X0, double Y0, double X1, double Y1,double X2, double Y2, double& angle, double& ratio);

    double sortVector(QVector<double> d, QVector<int>& n);

    void convertEqtoGal(double ra, double dec, double& l, double& b);//ra,dec in redians

    void convertPM_EqtoGal(double DEC, double MUra, double MUdec, double l, double b, double& MUl, double& MUb);

    QString ucac3find(double ra, double dec, double rhox, double rhoy, int tw,  QString ucac3path);

    void tangFromUCAC3(QString u3str,
                       QVector<double>& ksi, QVector<double>& eta,
                       QVector<double>& mag, double RA, double DE,
                       double year, double mag1, double mag2);
    //////////////////////////////////////////////////////////////////////
    QString ucac4find(double ra, double dec, double rhox, double rhoy, int tw,  QString ucac4path);

    void tangFromUCAC4(QString u4str,
                       QVector<double>& ksi, QVector<double>& eta,
                       QList<double*>& mag, double RA, double DE,
                       double year, double mag1, double mag2, int nmag);

    /////////////////////////////////////////////////////////////////////////
    void cms(const double* fd, int rho, double& cx, double& cy, double& cJ, int& N);

    void ringSumm(const double* fd, int rho2, double& sJ, int& N);

    double mfv(QVector<double> S,double step);//most frequent value

    int comparePointsSets(QVector<double>x,QVector<double>y,
                          QVector<double>X,QVector<double>Y,
                          double *Ax, double *Ay,
                          double pr, int N);

    class wcsData: public QObject
    {
    private:
        bool isWcsExist;
        double crpix1;
        double crpix2;
        double crval1;
        double crval2;
        double cdelt1;
        double cdelt2;
        double crota1;
        double crota2;
        /////////////
        double cd1_1;
        double cd1_2;
        double cd2_1;
        double cd2_2;
        int wcs_Type,w,h;
        //
        double yr,mm,dd,hr,min,sec,mjd,exptime,Year;
        QString fileName;
        QString Target;
    public:
        wcsData(QString filename);
        wcsData();
        ~wcsData();
        bool isExist();
        int wcsType();
        double MJD();
        void pixFromRADE(double ra, double de, double& x, double& y);
        void  RADEFromPix(int x, int y, double& ra, double& de);
        void pixFromXY(double X, double Y, double& x, double& y);
        void  XYFromPix(int x, int y, double& X, double& Y);
        double refpixRA();
        double refpixDE();
        double refpixX();
        double refpixY();
        double scalex();
        double scaley();
        double year();
        double ExpTime();
        int width();
        int height();
        QString target();
        void FOV(double& fovW, double& fovH);
        void setWCS(double *Ax, double *Ay);
        void saveWCS(QString fpath, int flag, QString newfname="");
        QString wcstostring();
    };


    class fitsData : public QObject//class for work with fits-image
    {
    public:
        fitsData(QString filename);
        ~fitsData();
        int width();//width of the image
        int height();//height of the image
        QString filename();//file name
        int datatype();//get type of data
        int error();//return last error code
        double pix(int x, int y);//get pixel value for point with coordinates x,y
        void getAperture(double *ap, int cx, int cy, int rho);
        void getRing(double *ap, int cx, int cy, int rho1, int rho2);
        void getBox(double *ap, int cx, int cy, int bsz);
        double leftlimit();
        double rightlimit();
        double mostprobpix();
        QPixmap histPixmap();
        QPixmap profilePixmap(int X1, int Y1, int X2, int Y2);
        void setRightLimit(int W);
        void setLeftLimit(int W);
        int leftlimitN();
        int rightlimitN();
        QString header();
        QString propertiesAsText();

    protected:
        void Hist();//function to construct of the histogramm
        void readHeader(fitsfile *fptr);
    private:
        QString fileName;
        QString Header;
        QString ptext;
        int w,h;//width and height of the image
        int bpix;//bits per pixel (type of the pixels data)
        int errCode;//code of the error: number from status variable
        /////////////pixel data
        unsigned short *ushD;//USHORT_IMG
        short          *shD;//SHORT_IMG
        long           *lD;//LONG_IMG
        float          *fD;//FLOAT_IMG
        double         *dD;//DOUBLE_IMG
        /////////////
        unsigned long *hist;//buffer of historgamm of the image
        int nhist;//size of buffer of historgamm
        double minp,maxp;//minimum and maximum of the image values
        double leftlim,rightlim,mpel;//contrast/brightness window limits of the histogramm
        int w1,w2;//limits of histogram as numbers of array elements
        int maxhistel;//maximum of histogramm
    };



    void findStars(fitsData *fd, QVector<int>& cmX, QVector<int>&cmY);

    int matchStars(QVector<double> X, QVector<double> Y,
                    QVector<double> x, QVector<double>y,
                    double* Ax, double* Ay,
                    double e, int Nmin, int Nmax);

}



#endif // FFC_H
