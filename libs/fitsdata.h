#ifndef FITSDATA_H
#define FITSDATA_H

class fitsdata;
class marksGrid;
class insSettings;
class marksP;
struct reductionParams;
//class catFinder;
//class sysCorrParam;
class refractionMaker;

class catFinder;

class sysCorrParam;

struct rsSelector0Sett;
struct rsSelector1Sett;
struct rsSelector2Sett;
struct rsSelector3Sett;
struct rsSelector4Sett;
struct rsSelector5Sett;
struct rsSelector6Sett;

class refractionMaker;
struct reductionParams;
class reductionMaker;

#include "multidim.h"
#ifndef REDSTAT_H
#include "redStat.h"
#endif

#include "sysCorr.h"


#include "fitsio.h"
#include "longnam.h"
//#include "./../libs/astro.h"
#include "identify.h"
#include "./../astro/astro.h"
#include "observatory.h"

#include "./vectGrid3D.h"
#include "ccdmeasurements.h"
#include "./../mb/mb.h"
#include "sscat.h"



#define DEFAULTIMAGE_X 100000
#define DEFAULTIMAGE_Y 100000

#define SCANIMAGE_X 25600
#define SCANIMAGE_Y 20096

#define DAMIMAGE_X 21824
#define DAMIMAGE_Y 21824

#define FANTASYIMAGE_X 28000
#define FANTASYIMAGE_Y 28000

//#define UCAC2_CAT_NUM 0
#define USNOB_CAT_NUM 0
#define UCAC3_CAT_NUM 1
#define LSPMFIND_CAT_NUM 2
#define PPMXL_CAT_NUM 2

#define CDSFIND_PROG_TYPE 0
#define UCAC3FIND_PROG_TYPE 1
#define LSPMFIND_PROG_TYPE 2

void getCatName(QString *cname, int ctype);
void marksDist(marksP *m1, marksP *m2, marksP *mRes);
double marksImgDist(marksP *m1, marksP *m2);
bool getStimp(double *P, unsigned short *fd, int width, int height, double fpx, int x, int y, measureParam params);
bool getStimp(double *P, unsigned short *fd, int width, int height, double fpx, int x, int y, int apr, int rr, int rw, int sg, int lb, int model, int& nofit, double delta);

int initPlateRefParam(refractionParam *refParam, fitsdata *fitsd, obsy *obsPos);
//
void detOCgrid(marksGrid *mG, double *ra_oc, double *de_oc);
//
bool findStars(img2d *imgArr, QVector<double>& cmX, QVector<double>&cmY, QVector<double>& flux, int rho, int bsize);

void rsSelector0(marksGrid *refMarks, QVector<int> &rsindex, rsSelector0Sett rss0sett);
void rsSelector1(marksGrid *refMarks, QVector<int> &rsindex, double cDist, QRect wFrame, int minRefStar);
void rsSelector2(marksGrid *refMarks, QVector<int> &rsindex, marksP *mP, double aper, int minRefStar);
void rsSelector3(marksGrid *refMarks, QVector<int> &rsindex, double objMag, double magDiap, int minStarsNum);
void rsSelector4(marksGrid *refMarks, QVector<int> &rsindex, double muMax, int minRefStars);
void rsSelector5(marksGrid *refMarks, QVector<int> &rsindex, marksP *mP, double dMax, double nMin, double nMax);
void rsSelector6(marksGrid *refMarks, QVector<int> &rsindex, int targ_us1, int minRefStarsNum);
void rsSelector7(marksGrid *refMarks, QVector<int> &rsindex, double cX, double cY, double dMax, double nMin);



class catFinder
{
    public:
    QString exeName;
    QString exePath;
    int catType;
    QString catName;
    QString catMagName;
    QString catPath;
};

int initCatList(QList <catFinder*> *starCatList, QString catiniFileName);



void prerDataVect(marksGrid *mGr, double oc0, double oc1, refractionMaker *refMaker, sysCorrParam *sysCorr);

struct outputLimits
{
    double maxOCMAG;
    double maxOCRA;
    double maxOCDE;
    double maxOCMAGrc;
    double maxOCRArc;
    double maxOCDErc;
    double resSigma;
};

struct whatOutput
{
    int errorreport;
    int residuals;
    int eqreport;
    int mpcreport;
    int sstarreport;
    int objresreport;
    int namereport;
    int ocreject;
    int ipixPos;
};

struct ouputParams
{
    QString mpcObsType;
};

int makeErrReports(marksGrid *refMarks, QVector<int> rsindex, reductionMaker *redMake, errBudgetRec* ebRec, QString resFolder, QString suff, outputLimits outLim, whatOutput whatOut, ouputParams outPar);
int makeObjErrReports(marksP *mObj, reductionMaker *redMake, errBudgetRec* ebRec, QString resFolder, outputLimits outLim, whatOutput whatOut, ouputParams outPar);
int makeIpixReports(marksGrid *ipixMarks, reductionMaker *redMake, errBudgetRec* ebRec, QString resFolder, QString suff);

//objects   ////////////////////////////////
int getMpephObject(mpephRec *mpcObj, double mJD, QString objStr, int useObjNum, procData mpephProcData);
//void getMpephObject(marksGrid *objMarks, QStringList outerArguments, QString ast_eph_prog, QString ast_eph_prog_folder, double mag0, double mag1, int mpeWaitTime=-1);
//void getMpephNum(marksGrid *objMarks, double MJD, QString mpeNum, QString ast_eph_prog, QString ast_eph_prog_folder, double mag0, double mag1, int mpeWaitTime=-1);
//void getMpephName(marksGrid *objMarks, double MJD, QString mpeName, QString ast_eph_prog, QString ast_eph_prog_folder, double mag0, double mag1, int mpeWaitTime=-1);

void getMpephGrid(marksGrid *objMarks, double mJD, QStringList objList, int useObjNum, double mag0, double mag1, procData mpephProcData);

int getMiriadeObject(mpephRec *mpcObj, double mJD, QString objStr, procData miriadeProcData, QString objType="");
void getMiriadeGrid(marksGrid *objMarks, double mJD, QStringList objList, double mag0, double mag1, procData miriadeProcData, QString objType="");

void getLspmObject(marksGrid *objMarks, double MJD, QString lspmName, QString lspm_prog, QString lspm_prog_folder, QString lspm_cat_file, int lspmWaitTime=-1);
void findLspmCat(marksGrid *objMarks, double ra, double de, double MJD, QString lspm_prog, QString lspm_prog_folder, QString lspm_cat_file, double fov, double mag0, double mag1, int lspmWaitTime=-1);
/*
ra - [deg]
de - [deg]
MJD - [day]
fov - field of view [arcmin]
*/

void findSkybot(marksGrid *objMarks, double ra, double de, double MJD, QString prog, QString progFolder, double fov, QString obsCode, double mag0, double mag1, int sbWaitTime=-1);
void findSkybotNamesList(QStringList *namesList, double ra, double de, double MJD, QString prog, QString progFolder, double fov, QString obsCode, double mag0, double mag1, int sbWaitTime=-1);

void detTan6const(double *ksi, double *eta, double x, double y, plateConstRec xParams, plateConstRec yParams);
void detXcYc6const(double *x, double *y, double ksi, double eta, plateConstRec xParams, plateConstRec yParams);

////////////////////////////////////////////



//selectors //////////////////////////
struct rsSelector0Sett
{
    double minMag, maxMag;
    int minStarsNum;
};

struct rsSelector1Sett
{
    double cDist;
    int minStarsNum;
};

struct rsSelector2Sett
{
    double aper;
    int minStarsNum;
};

struct rsSelector3Sett
{
    double magDiap;
    int minStarsNum;
};

struct rsSelector4Sett
{
    double muMax;
    int minStarsNum;
};

struct rsSelector5Sett
{
    double dMax;
    int minStarsNum, maxStarsNum;
};

struct rsSelector6Sett
{
    QString colSep;
    int corrModel;
    int targEnum;
    int minStarsNum;
};

struct rsSelector7Sett
{
    double dMax;
    int minStarsNum;
};

class refractionMaker
{
public:
    double n1;
    double *aij;
    double *ksi0, *eta0, *zeta0;
    double *ksi1, *eta1, *zeta1;
    double A0, D0;
    double A1, D1;
    double dA, dD;
    double *rT0, *rT1, *rA;
    double s, t;
    double kaT0, kaT1, mT0, mT1;
    double ksiA0, etaA0, cosRoA0;
    double ksiA1, etaA1, cosRoA1;
    double Fi, Long;
    double ka1;
    double T;
    double lam;

    refractionMaker(refractionParam refParam);
    ~refractionMaker();

    void forvRef(double *ra1, double *dec1, double ra0, double dec0);
    void forvRefRel(double *ra1, double *dec1, double ra0, double dec0);
    void backRef(double *ra0, double *dec0, double ra1, double dec1);

};

class reductionMaker
{
public:
    double *KSI, *ETA, *MAG, *WEksi, *WEeta, *Cke, *Cksi, *Ceta, *CMAG, *BVect, *CVect, *ZVect, *WE, *DVect;
    int *EXCLINDKSI, *EXCLINDETA, *EXCLINDMAG, *EXCLIND;

    double UWE;
    int RN;
    double *ZKSI;
    double *DKSI;
    double UWEKSI;
    int RNKSI;
    double *ZETA;
    double *DETA;
    double UWEETA;
    int RNETA;
    double *ZMAG;
    double *DMAG;
    double UWEMAG;
    int RNMAG;

    int posKsiMax, posEtaMax;
    double errKsiMax, errEtaMax;
    double Sx, Sy;

    int reductionType;

    int useRefraction;
    refractionMaker *refMaker;

    reductionMaker();
    reductionMaker(int redType);
    ~reductionMaker();

    int makeReduction(QString &mTimeCode, marksGrid *refMarks, QVector<int> &rsindex, reductionParams params, refractionParam *refParam = NULL);
    void makeErrorBudget(errBudgetRec *errBud);
    void detTan(double *ksi, double *eta, double x, double y);
    void detMagn(double *mag, double pixmag);
    void detXcYc(double *x, double *y, double ksi, double eta);
    void detPmag(double *pmag, double mag);
    void getCenterPos(double *Xoc, double *Yoc);
    void getParams(errBudgetRec *errBud);
    //void clear();

    //6 const
    int make6const(marksGrid *refMarks, QVector<int> &rsindex, reductionParams params);
    void getCenterPos6const(double *Xoc, double *Yoc);
    void getParams6const(errBudgetRec *errBud);
    void detTan6const(double *ksi, double *eta, double x, double y);
    void detMagn6const(double *mag, double pixmag);
    void detXcYc6const(double *x, double *y, double ksi, double eta);
    void detPmag6const(double *pmag, double mag);
    void detSmax6const(QVector<int> &rsindex);

    //8 const
    int make8const(marksGrid *refMarks, QVector<int> &rsindex, reductionParams params);
    void getCenterPos8const(double *Xoc, double *Yoc);
    void getParams8const(errBudgetRec *errBud);
    void detTan8const(double *ksi, double *eta, double x, double y);
    //void detMagn8const(double *mag, double pixmag);
    void detXcYc8const(double *x, double *y, double ksi, double eta);
    //void detPmag6const(double *pmag, double mag);
    void detSmax8const(QVector<int> &rsindex);
};


struct HeadRecord
{
    QString keyName, keyValue, keyComment;
};

class HeadList
{
public:
    QList <HeadRecord*> headList;
    HeadList();
    ~HeadList();
    int getKeyName(QString kName, QString *kVal);

    int getKeyNum(HeadRecord* resRec, int pos);
    void clear();
    int size();

    void readHeader(QString headStr);
};

struct ExposureRec
{
    double expTime;     //exp moment
    double expVal;      //expoture duration
    QString timeStr();
};

struct ExposureList
{
    QList <ExposureRec*> exps;
    void getMean(ExposureRec *eRec);
    void getExp(ExposureRec *eRec, int pos);
    double getExpTime(int pos);
    double getExpVal(int pos);
    QString getExpTimeStr(int pos);
    void clear();
    int expNum();
    ~ExposureList();
};

class ResidualsData
{
public:
    double UWEKSI, UWEETA, UWEMAG;
    double *DKSI, *DETA, *DMAG;//3x3
    double *ZKSI, *ZETA, *ZMAG;//3
};

struct tmuRec
{
    double epoch, smul, epoch0, smul0;
    int fromStr(QString dstr);
};

class TAImUTC
{
public:
    QStringList tmuStrL;
    double getTmu(double mjd);
    int initTmsFile(QString fName);
};


int makeReduction(QString iniFile, QString resFolder);
int initExpList(ExposureList *expList, HeadList headList, obsy *obsPos);
void locStime2mjd(double *mjd, double sTime, double Long, DATEOBS dateObs0);

int detCtype(QString catName);
int getMarksGrid(marksGrid *catMarks, catFinder *sCat, int catProgType, double mjd, double raOc, double deOc, double fov, double minM, double maxM, int sNum);
int identAuto(marksGrid *resGrid, marksGrid *ekvGr, marksGrid *ipixGr, double *WCSdata, int targNum = 6, int pType = 0, int maxNum = 100);
void copyGrid(marksGrid *mgSource, marksGrid *mgRes);
void copyImgGrid(marksGrid *mgSource, marksGrid *mgRes);
void remCrossedMarks(marksGrid *mg0, marksGrid *mgRem, double dMin);
void remCrossedMarks(marksGrid *mg0, double dMin);
void findCloserMarks(marksGrid *mg0, marksGrid *mg1, marksGrid *mgRes, double dMax);
//marks
int measureMark(img2d *imgArr, marksP *mP, measureParam params);                //0-bad; 1-good
int markMassCenter(img2d *imgArr, marksP *mP, int aperture);                    //0-bad; 1-good
int detMarkPpix(img2d *imgArr, marksP *mP, int apeSize);                        //0-bad; 1-good
int detHeadType(HeadList hList);


class fitsdata//: public QObject
{
 //Q_OBJECT
 
//signals:

    
//private slots:
	
	
public:

        QString fileName, fitsHeader;
        img2d *imgArr;
        QRect workFrame;

        double WCSdata[13];
        double MJD;
        double exptime;

        obsy *obsPos;       //observatory

        TAImUTC *tmu;
        int is_empty;

        double Sx, Sy;


        //functions
        fitsdata();
        fitsdata(QString fitsFileName);
        ~fitsdata();

        uchar getImgPixValue(int x, int y);
	uchar getPixValue(int x, int y);



        int openFile(QString fitsFileName, int headType = 0); //headType: 0 - FTN, default; 1 - SDSS;
        void clear();

        void initTmu(QString tmuFile);

        void initObsPos(obsy *obsp);

        HeadList headList;


        int getApeSize(double apeSec);
        double getScaleX();
        double getScaleY();
        double getMeanScale();

        void setInstrSettings(insSettings instr);


        marksGrid *catMarks;        //catalog marks
        marksGrid *ipixMarks;       //image pixels marks

        marksGrid *refMarks;        //reference marks
        marksGrid *objMarks;        //objects marks


        int detWCS(int wcsN, double sMax);	//0 = ok
        int detWCS1(reductionParams params);	//0 = ok

	void detTan();
        void detTanObj();

        double detFov();//field of view in [']

	int detResiduals();
        void detIpixWorkFrame();
        void detCatWorkFrame();
        void setRpix();

        int loadHeaderFile(QString headFName);
        int loadHeaderFile1(QString headFName);
        int saveHeaderFile(QString headFName);

 //       int readHttpHeader(QString httpStr);
        int readHeader(QString headStr);
        int initHeadList();

        int saveFitsHeader();
        int saveFits();
        int saveFitsAs(QString fitsFileName);
        int saveWCS();

        void readWCShead();
        int loadWCSFile(QString headFName);
        int saveWCSFile(QString headFName);

        void dropWCS();

        int loadIpixMarks(QString fName, QString mSep, QString mCol);
        int saveIpixMarks(QString fName, QString mSep, QString mCol);

        void findCloserStars(double dMax);
        void findCloserObjects(double dMax);

        void initRaDeHeadList();
        void setMJD(double mjd);

        int ruler3(QString iniFile, QString resFolder, refractionParam *refParam = NULL, sysCorrParam *sysCorr = NULL);

        void setPos(double mJD, double ra, double de);

        void addObjMark(marksP *mP);

        void cleanObjects(int aper);
        void cleanEkvMarks(int aper);

//find stars
        int findHstars(int apeDiam, int targNum);

        int updateHist();

    ////////

        void detPpix(marksGrid *mGr, int apeDiam);
        void moveCenter(marksGrid *mGr);
        void moveMassCenter(marksGrid *mk, int apeDiam);

        void measureMarksGrid(marksGrid *mgr, measureParam params);
//        int measureMark(marksP *mP, measureParam params);//0-bad; 1-good



	
private:
	
	
};

class marksGrid//: public QObject
{
    /*
 Q_OBJECT
 
signals:

private slots:
*/
	
public:
	QList <marksP*> marks;
        errBudgetRec* ebRec;
	
	int num;
	int ctype;
	int xc, yc;
	
	QString catdata;
	
	marksGrid();
	~marksGrid();
	
        void detTan(double *WCSdata);
	void moveMarks(int dx, int dy);
	void setMarksCenter(int x0, int y0);
	
	void clearMarks();
        marksP* addMark(marksP *source);
        marksP* addEkvMark(double ra, double de, double magn);
        marksP* addImgMark(double i, double j, double flux = 0);
	
	void detCenter(double *xc, double *yc);
	void flipH();
	void flipV();
	
	int saveTanImg(QString fName, QString mSep, QString mCol);
	int loadTanImg(QString fName, QString mSep, QString mCol);
        void moveToCorner(long *naxes);
        void copy(marksGrid* source);
	void remMark(int i);
	long detNaxes(long *naxes0=NULL, long *naxes1=NULL);	//define maximum marks resolution
        int detRect(QRect *iRect);	//define maximum marks resolution
        void detOCmean(double *oc0, double *oc1);
        void detXYmean(double *xMean, double *yMean);

        int getNum();

	
        void sortMagn(int direction=0);					//сортировка по звездной величине (marks[]->mEkv[2])
                                                                        //direction - направление сортировки
                                                                        //0 - от меньшей к большей (сначала яркие)
        void sortAper(int direction=0);					//сортировать по пиксельной яркости (marks[]->mTanImg[2])
                                                                        //direction - направление сортировки,
                                                                        //0 - от меньшей к большей (сначала слабые)

//private:
	
	
};

class marksP
{
public:
        double *mEkv;
        double *mTan;
        double *mTanImg;
        imgAperture *mPpix;

        marksP();
        marksP(int oType);
        marksP(marksP *sMark);
        ~marksP();

//cat
      QString catName, catMagName;
//objType
        int objType;
        ucac3Rec *u3Rec;        //0
        mpephRec *mpcObj;       //1
        sscat_rec *sstar;       //2
        skybotRec *sbot;        //3
        usnoRec *usnobRec;   //4
        ppmxl_rec *ppmxlRec;   //5

//work
        double *data;
//res
        residualsRec *resRec;
        ocRec *ocObj;
        sstarRes *ssResRec;
        double *P;



        //void copy(const marksP &source);
        void copy(const marksP *source);
        //void copyImg(const marksP &source);
        void copyImg(const marksP *source);

        void createP();


        marksP& operator=(const marksP &source);
};

class insSettings
{
public:
        insSettings(QString sFname);
        ~insSettings();

        QString curName;
        double scx, scy, fov, rang;
        int curInst;
        int isEdt;

        QSettings *sett;
        QStringList namesList, scxList, scyList, fovList, rangList;

        void getNumIns(int num);
        void getNameIns(char *iname);
        int getNameIns(QString iname);

        void addIns(QString name);
        void save();
        void delIns(int num);
        void delIns(QString name);
};


#endif
