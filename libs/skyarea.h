//#pragma once
#include <stdio.h>
//#include <direct.h>
#include <QtCore>

#ifndef COMF_H
#include "comfunc.h"
#endif

#ifndef ASTRO_H
#include "astro.h"
#endif

#ifndef DA_H
#include "DynArr.h"
#endif

#ifndef OBSRY_H
#include "observatory.h"
#endif

#ifndef ORBCAT_H
#include "orbcat.h"
#endif

#ifndef DELE_H
#include "dele.h"
#endif

#ifndef OBSERV_H
#include "observ.h"
#endif

#ifndef SSCAT_H
#include "sscat.h"
#endif

#ifndef SSCATFB_H
#include "sscatFB.h"
#endif

#ifndef MPCCAT_H
#include "mpccat.h"
#endif


#ifndef FBUFFER_H
#include "fBuffer.h"
#endif

#ifndef SKYAREA_H

//#include "comfunc.h"
#include "orbit.h"
#include "orbcat.h"
//#include "listfile.h"

//#include "atlstr.h"

#define ILISTOLD_LEN 88
#define ILIST_LEN 93
#define RLIST_LEN 82
#define ELIST_LEN 67
#define LLIST_LEN 97
#define OLIST_LEN 29
#define TLIST_LEN 626
#define CATLIST_LEN 278


///////////////////////////////////////////////
class TLRecord;
class TaskList;
class LogRecord;
class LogList;
struct ObsRecord;
class ObsList;
class LRecord;
class IList;
class RLRecord;
class RList;
class ERecord;
class EList;
class CatRecord;
class CatList;
class IListBuf;
class IListBuffer;
class iniRecord;
class IListR;

int addIniListNames(TLRecord task, QString inpStr);


class TLRecord	//Task List record
{
public:
	int noftask;	//number of task

	double exp;			//experience of task
	int Ntot;			//Total number of observation for one object
	double texc;		//exclusion of a object after successful observations
	double dRA;			//maximum distance from celestial meridian
	int NinN;			//number obsrvations during one night
	int flag_active;	//flag for activ task
	
	char *tail;

	TLRecord();
	~TLRecord();

	int set_name(char *name);
	int set_desc(char *desc);
	int set_dir_path(char *dir_path);
	int set_cat_name(char *cat_name);

	char* get_name();
	char* get_desc();
	char* get_dir_path();
	void get_dir_path(char *dp);
	char* get_cat_name();

	void get_ini_fname(char *dp);
	void get_cat_fname(char *dp);

	void copyTo(TLRecord *rcp);

private:
	char *name;
	char *desc;
	char *dir_path;
	char *cat_name;

};

class TaskList : public FileDynStrBin
{
public:
	TLRecord *record;

	TaskList();
	TaskList(int sizestr);
	~TaskList();
	int init(char *fname);
	int init(char *fname, int sizestr);


	int AddRec();
	int AddTask(TLRecord *ntask);

	int UpdateRec(int pos);

	int GetRec(int i);
	int GetRec(char *name);
	int GetTaskNum(int noft);
	int GetTaskName(char *noft);

	int IsTaskActive(int noft);

        void rec2s(char *nnstr);
	void s2rec(char *nnstr);

	int pushTask(TLRecord *newtask);
	int remTask(int noftask);

	char *str_ini, *str_cat;

	IList *ini_lst;
	OrbCat *orb_catalog;
	sscat *lspm_catalog;

//	int UpdateTask();

//	int sort_prior();
};

class tlRecord //: tRecord	//Task List record
{
public:
//	int noftask;	//number of task

        double exp;			//experience of task
        int Ntot;			//Total number of observation for one object
        double texc;		//exclusion of a object after successful observations
        double dRA;			//maximum distance from celestial meridian
        int NinN;			//number obsrvations during one night
        int flag_active;	//flag for activ task
        QString name, desc, dirPath, catName;

//	char *tail;

        tlRecord();
        int fromString(QString tStr);
        void toString(QString &tStr);
        tlRecord& operator=(const tlRecord &rhs);
        tlRecord& operator=(const TLRecord &rhs);

        void getIniName(QString &iniName);
        char* get_cat_name();
        char* get_task_name();
};

bool operator==( const tlRecord& lhs, const tlRecord& rhs );

class taskList// : listFile <tlRecord>
{
public:
    QList <tlRecord*> recList;
    QString fileName;

    taskList();
    int init(QString fname);
    int s2rec(QString tStr);
    int rec2s(QString &tStr);
    int size();
    tlRecord* at(int i);
    int save();
    int saveAs(QString tfName);
    int getRec(tlRecord* tlRec, int pos);
};

class LogRecord
{
public:
	int year0, year1;
	int month0, month1;
	int day0, day1;
	int hour0, hour1;
	int min0, min1;
	int sec0, sec1;

	char *name;
	int success;
	int noftask;

	char *RA, *DEC;
	double magn;

	LogRecord();
	~LogRecord();
};

class LogList :public FileDynStrBin
{
public:
	LogRecord *record;
	
	int AddObs(LogRecord *er);
//	int RemExc(char *ename);
	int GetObs(char *ename);		//get number of successful observations
	int GetRec(int i);


	int Clear();

	LogList();
	~LogList();

	int init(char *fname);

private:
	int s2rec(char *str);
	int rec2s(LogRecord *er);
};


struct ObsRecord
{
	int noftask;
	char *name;
	int nobs;
};

class ObsList :public fBuffer
{
public:
	ObsRecord *record;
	
	int AddObs(ObsRecord *er);
	int RemObs(char *ename);
	
	int GetObs(int pos);
	int GetObs(char *ename);
	int GetNObs(char *ename);

	int ChObs(LogRecord *nobserv, int num);
	int ChObs(int ntask, char *name, int num);

	int Clear();

	ObsList();
	~ObsList();

private:
	int s2rec(char *str);
	int rec2s(ObsRecord *er);
};

class LRecord
{
public:
	int noftask;
	char *name;
	double exp;
	int flag0;					//0 - DE/LE; 1 - numbered; 2 - rapid stars
	double t0, t1;
	char *desc;
	char *tail;

	LRecord();
	~LRecord();

	int set_desc(char *desc);
	int set_name(char *nm);
	char* get_desc();
	void copyTo(LRecord *rec);
	int IsEqv(LRecord *rec);
};


class IList : public fBuffer
{
public:
	LRecord *record;

//	IListBuffer *ilBuf;

	IList();
	IList(int sizestr);
	~IList();
	int init(char *fname);

	int AddRec();
	int AddRec(LRecord *nrec);
	int AddRec(int pos);

	int GetRec(int i);
	int GetRec(char *nnstr);
	int GetRec(LRecord *rec);

	int DelRec(LRecord *rec);

	int DelRec(char *name);

	int UpdateRec(int pos);
	int UpdateRec(LRecord *rec);

//	int GetRecnc(char *name);

	void rec2s(char *nnstr);
	void s2rec(char *nnstr);

	int sort_prior();
};

class iniRecord// : tRecord
{
public:
        //int noftask;
        QString name;
        double exp;
//        int flag0;					//0 - DE/LE; 1 - numbered; 2 - rapid stars
        double t0, t1;
        QString desc;

        iniRecord();

        int fromString(QString tStr);
        void toString(QString &tStr);

        iniRecord& operator=(const iniRecord &rhs);
//        char *tail;
/*
        //LRecord();
        //~LRecord();

        int set_desc(char *desc);
        int set_name(char *nm);
        char* get_desc();
        void copyTo(LRecord *rec);
        int IsEqv(LRecord *rec);*/
};

bool operator==( const iniRecord& lhs, const iniRecord& rhs );


class IListR// : listFile <iniRecord>
{

/*
public:
        QList <LRecord *> recList;

        QString fileName;
//	IListBuffer *ilBuf;

        IList();
//	IList(int sizestr);
        ~IList();
        int init(QString fname);

//	int AddRec();
        int AddRec(LRecord *nrec);
        int AddRec(LRecord *nrec, int pos);

        int GetRec(LRecord *nrec, int i);
        int GetRec(LRecord *nrec, QString name);
        //int GetRec(LRecord *rec);

        //int DelRec(LRecord *rec);

        int DelRec(char *name);

        int UpdateRec(LRecord *nrec, int pos);
        int UpdateRec(LRecord *nrec);

//	int GetRecnc(char *name);
/*
        void rec2s(char *nnstr);
        void s2rec(char *nnstr);
/
//	int sort_prior();
*/
};

/*
class LRecordOld
{
public:
	int noftask;
	char *name;
	double exp;
	int etype;					//0 - DE/LE; 1 - numbered; 2 - rapid stars
	double t0, t1;
	char *desc;
	char *tail;

	LRecord();
	~LRecord();

	int set_desc(char *desc);
	int set_name(char *nm);
	char* get_desc();
	void copyTo(LRecord *rec);
	int IsEqv(LRecord *rec);
};
*/
/*
class IListOld : public fBuffer
{
public:
	LRecord *record;

//	IListBuffer *ilBuf;

	IListOld();
	IListOld(int sizestr);
	~IListOld();
	int init(char *fname);

	int AddRec();
	int AddRec(LRecord *nrec);

	int GetRec(int i);
	int GetRec(char *nnstr);
	int GetRec(LRecord *rec);

	int DelRec(LRecord *rec);

	int UpdateRec(int pos);

//	int GetRecInc(char *name);

	void rec2s(char *nnstr);
	void s2rec(char *nnstr);

	int sort_prior();
};
*/
class IListBuf
{
public:
	LRecord *record;
	IListBuf *next;

	IListBuf();
	~IListBuf();
};

class IListBuffer
{
public:
	IListBuf *root;
	int num;

	IListBuffer();
	~IListBuffer();

	int addElem(LRecord *npar);
	int remElem(int num);
	int clear();
};

class RLRecord
{
public:
        QString taskName;
//	char *name;
        QString name;
        int num;
        double exp;
//	char *tail;

        //double mJD;
        double ra, dec, magn, muRa, muDe;
        int nofobs;

	RLRecord();
	~RLRecord();

        int fromString(QString str);
        int toString(QString &str);
/*
        void get_RA(QString str);
        void get_DEC(QString str);
*/
        //void set_name(QString sname);
        //char* get_name();
};

class RList// :public fBuffer	//Results list
{
public:
    QList <RLRecord *> resList;
        //record;
    QString fileName;
    double mJD;

        int GetRec(RLRecord *resRec, int i);
        int GetRec(RLRecord *resRec, QString name);
        int GetRec(RLRecord *resRec, QString name, QString taskName);

        //int GetRecName(char *name);
        //int GetRecNum(int num);

	int AddRec(RLRecord *nrec);
        int updateRec(RLRecord *nrec);

        void Clear();

	int sort_meridian(double s);
	int sort_magnitude(int direct);		//1-vozrastanie; 0-ubyvanie
	int sort_obs();
	int sort_RA_min2max(double RAmin);
	int sort_prior();
        //int sort_num();

	RList();
//	RList(int sizestr);
	~RList();
        int init(QString fname);

        int save();
        int saveAs(QString fname);
private:
//	int s2rec(QString str);
//	int rec2s(QString str);
};

class ERecord
{
	char *desc;
	
public:
    QString name;
    QString taskName;
	double Tun;
//	char *tail;
	
	ERecord();
	~ERecord();

	void copyTo(ERecord *recp);
/*
	int set_desc(char *desc);
    int set_name(char *name);
	char* get_desc();
	char* get_name();
*/
};

class EList :public fBuffer
{
private:
	int AddExc();
public:
	ERecord *record;

	double ts;	//стандартный фол в минутах. 20 по-умолчанию.
	
	int AddExc(ERecord *er);
	int AddExc(int noftask, char *ename, double cTimeJD);
	
	int GetExc(int pos);

	int RemExc(int noftask, char *ename);
	int RemExc(char *ename);

    int IsExc(char *taskName, char *ename);
	int IsExc(char *ename);

	int UpdateExc(double cTime);

	int Clear();

	EList();
	EList(int sizestr);
	~EList();
	int init(char *fname);

private:
	int s2rec(char *str);
	int rec2s(char *tstr);

};

class CatRecord
{
public:
	int cat_type;			//0-dele; 1-orbcat; 2-sscat; 3-mpccat
    QString name;
    QString path;

	CatRecord();
	~CatRecord();

//	void Set(int num, char *name, char *path);
//	void get_dir_path(char *dp);
};

class CatList :public FileDynStrBin
{
public:
    QList <CatRecord *> recList;
	
	int Add(CatRecord *er);
    int Rem(QString cname);
	int Get(int num);

    int GetByName(QString ename);
    int GetCatType(QString cat_name);
//	int GetByNum(int num);


	int Clear();

	CatList();
	~CatList();

	int init(char *fname);

private:
	int s2rec(char *str);
	int rec2s(CatRecord *er);
};

//////////////////////////////////////////////////////////////////////////
//	ObsProgram

class ObsProgramParam
{
public:
	int noftask;
	double duration;
//	double amount;

	ObsProgramParam(){};
	~ObsProgramParam(){};
};

class ObsProgram
{
public:
	ObsProgramParam *opParam;

	ObsProgram();
	~ObsProgram();
};

//////////////////////////////////////////////////////////////////////////

class EA_obj
{
public:
	IList *ini_list;
	OrbCat *orb_catalog;

	EA_obj();
	~EA_obj();

	int init(char *fn_ilist, char *fn_ocat);

	int Add(IList *new_il, char* fn_ccat);
	int RemIL(int pos);
	int RemIL(LRecord *irecord);

	int GetRec(LRecord *irecord);
	int GetRec(int pos);

	int RemAllILName(char *name);
	int RemAllILTask(int noftask);

	int RefreshOC(OrbCat *oc);
	
};

//////////////////////////////////////////////////////////////////////////

class saParams
{
public:
	double magn_min, magn_max;
	double RAmin, RAmax, DECmin, DECmax;
	double taskRAmin, taskRAmax, taskDECmin, taskDECmax;

	int isAutoRA;
	double dRA0, dRA1;
	int max_obj;

	int is_log_oneday;

	int ignore_task_rules;

//	char *install_dir;
//	Visual properties
        double timeSunSet, timeSunRise;	//JD times;
//	double sunX, sunY, sunZ;
	double sunRA, sunDE;
//

	saParams();
	~saParams();
};

class SkyArea
{
public:
	double timeCur;

//	OrbCat *orb_ini_catalog;
	
//Каталоги
        OrbCat *orb_catalog;
	mpccat *mpc_catalog;
//	sscatFB *lspm_catalog;

//	OrbCat *orb_catalog;
	orbit *orb_elem;
	observ *obs_pos;

//	IList *ini_list;
//	EA_obj *obj_list;

	IList *ini_list;
	RList *res_list;
	EList *exc_list;
	LogList *log_list;
	LogList *log_list_oneday;
	ObsList *obs_list;
    taskList *task_list;
	CatList *cat_list;

//	char *fn_cat;
	char *install_dir;
	char *catalog_dir;

	SkyArea(char *dele_header);
	SkyArea();
	~SkyArea();

        int init(char *fn_obs, char *fn_dele_bin);
	int init_log_one_day(char *fn_lod_list);
	int init_time(double UTC);
	int initVisualProp(double T);

	int prep_wcat();

	int set_opt(double RAmin, double RAmax, double DECmin, double DECmax, double magn_min, double magn_max);
        int is_skyarea(RLRecord *resRec);
	double isTaskSA(double RA, double DEC);
	int SetAutoRA();
	void SetTaskAutoRA();
        void det_res_list(RLRecord *resRec, double x, double y, double z, double vx, double vy, double vz, double *Sdist, double *Edist, int ctype);
	int AddIniList(IList *iniadd, int noft);
	int RemIniList(LRecord *remrec, int noft);
	int UpdateIniList(LRecord *remrec0, LRecord *remrec1);
	int GetIniList(int noft);
	void UpdateObsList();

	int AddTask(TLRecord *ntask);

	int UpdateTaskCatalog(int tasknum);

	int Grade();
	int RL_sort(int stype);
	int CutMaxObj();

	int GetTaskIniFNames(char *ini_lst, char *ini_cat);

        double DetExp(RLRecord *resRec, int mode);

	saParams *params;

	char *err_log;
	int err_level;
};

#define SKYAREA_H
#endif
