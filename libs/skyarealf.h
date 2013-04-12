#ifndef SKYAREALF_H
#define SKYAREALF_H

#include "listfile.h"

#include <stdio.h>
#include <QtCore>


#ifndef COMF_H
#include "comfunc.h"
#endif

#ifndef ASTRO_H
#include "astro.h"
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

#ifndef ORB_H
#include "orbit.h"
#endif

#ifndef ORBCAT_H
#include "orbcat.h"
#endif

#define DELE_CAT_TYPE 0
#define ORB_CAT_TYPE 1
#define LSPM_CAT_TYPE 2
#define MPC_CAT_TYPE 3

class tlRecord;
class taskList;
//class logRecord;
class iniRecord;
class iniList;
class catRecord;
class catList;
class resRecord;
class resList;

void det_res_list(resRecord *resRec, observ *obs_pos, double x, double y, double z, double vx, double vy, double vz, double *Sdist, double *Edist, int ctype, double H = 0.0);
void det_res_list(resRecord *resRec, observ *obs_pos, double *state, double *Sdist, double *Edist, int ctype, double H = 0.0);

class iniRecord : tRecord
{
public:
        //int noftask;
        QString name;
        double exp;
//        int flagEvent;					//0 - DE/LE; 1 - numbered; 2 - rapid stars
//        double t0, t1;
//        QString desc;

        iniRecord();

        int fromString(QString tStr);
        void toString(QString &tStr);

        iniRecord& operator=(const iniRecord &rhs);

};

bool operator==( const iniRecord& lhs, const iniRecord& rhs );
bool operator==( const iniRecord& lhs, const iniRecord* rhs );

class iniList : public listFile <iniRecord>
{
public:
    int addRec(iniRecord &nRec);
    int addRec(iniRecord* nRec);
    void addIniList(iniList newIniList);
    void updateIniList(iniList newIniList);
    int getRecName(iniRecord* nRec, QString name);
    int removeObj(QString name);

};

class resRecord : tRecord
{
public:
        //int noftask;
        int number;
        QString name;
        QStringList tasks;

        double ra, dec, mu_ra, mu_dec, magn, exp;

        resRecord();

        int fromString(QString tStr);
        void toString(QString &tStr);

        resRecord& operator=(const resRecord &rhs);

};

bool operator==( const resRecord& lhs, const resRecord& rhs );
bool operator==( const resRecord& lhs, const resRecord* rhs );

class resList : public listFile <resRecord>
{
public:
    double jdTime;
    int addRec(resRecord &nRec);
    int addRec(resRecord* nRec);

    int addTaskName(QString name, QString taskName);

    int saveAs(QString tfName);
    int save();
    int init(QString fname);

    int sortT(double s);    //[s] = rad
    int sortDec();
    int sortMagn();

/*    void addIniList(resList newIniList);
    void updateIniList(iniList newIniList);
    int getRecName(resRecord* nRec, QString name);
    int removeObj(QString name);
*/
};

class tlRecord : tRecord	//Task List record
{
public:
//	int noftask;	//number of task

    QString name, desc, catName;

    double texc;		//exclusion of a object after successful observations
    double dRA;			//maximum distance from celestial meridian
    int NinN;			//number obsrvations during one night
    int Ntot;			//Total number of observation for one object
    int flag_active;	//flag for activ task
    int exp;			//experience of task
//	char *tail;
    iniList iList;


    tlRecord();
    int fromString(QString tStr);
    void toString(QString &tStr);
    tlRecord& operator=(const tlRecord &rhs);
//        tlRecord& operator=(const TLRecord &rhs);
    void initIList();

    void getIniName(QString &iniName);
    void getCatName(QString &catName);
 //   char* get_cat_name();
 //   char* get_task_name();

    int getIniList(iniList *iList);

    int updateIniList(iniList iList);
//    int
    int removeObj(QString name);
};



bool operator==( const tlRecord& lhs, const tlRecord& rhs );
bool operator== ( const tlRecord& lhs, const tlRecord* rhs );

class taskList : public listFile <tlRecord>
{
public:
    tlRecord* getTaskName(QString name);
    int getTaskName(QString name, tlRecord *tRec);
    int addRec(tlRecord &nRec);
    int addRec(tlRecord* nRec);

    int removeObj(QString taskName, QString objName);

    //int init(QString fname);
    //int save();
};
/*
class logRecord : public tRecord
{
public:
    QDateTime timeObs;

    QString name;
    int success;
    QString taskName;

    double RA, DEC;
    double magn;

    //LogRecord();

    int fromString(QString tStr);
    void toString(QString &tStr);
    LogRecord& operator=(const LogRecord &rhs);

};
*/


class catRecord : tRecord
{
public:
    int catType;			//0-dele; 1-orbcat; 2-sscat; 3-mpccat
    QString name;
    QString catFile;

//	CatRecord();
//	~CatRecord();

    int fromString(QString tStr);
    void toString(QString &tStr);

    catRecord& operator=(const catRecord &rhs);

    QString getCatTypeName();
//	void Set(int num, char *name, char *path);
//	void get_dir_path(char *dp);
};

bool operator==( const catRecord& lhs, const catRecord& rhs );

class catList : public listFile <catRecord>
{
public:
    int addRec(catRecord &cRec);
    int addRec(catRecord *cRec);

    catRecord* getCatByName(QString cName);
    int getCatByName(QString cName, catRecord* cRec);


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

    double timeSunSet, timeSunRise;	//JD times;
    double sunRA, sunDE;

    saParams();
    ~saParams();
};


class skyAreaLF
{
public:
    taskList task_list;
    catList cat_list;

    observ *obs_pos;
    saParams params;

    QDir installDir;

    double timeCur;

    skyAreaLF();

    int init(QString obsCat, QString deleCat, QString insDir);
    int initInstallDir(QString insDir);

    int updateIniList(iniList iList, QString taskName);
    int addIniList(iniList iList, QString taskName);

    int save();

    QString getTaskIniName(QString taskName);
    QString getTaskCatName(QString taskName);

    int getTaskCat(QString taskName, tlRecord *tRec, catRecord *cRec);

    int removeObj(QString taskName, QString objName);

    int initVisualProp(double jDay);
    int init_time(double jDay);
    int set_opt(double RAmin, double RAmax, double DECmin, double DECmax, double magn_min, double magn_max);

    int grade(resList &rList);
/*
    int addIniList(iniList *iniadd, QString taskName);
    int remIniList(iniRecord *remrec, QString taskName);
    int updateIniList(iniRecord *remrec0, LRecord *remrec1);*/
};

#endif // SKYAREALF_H
