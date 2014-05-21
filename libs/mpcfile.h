#ifndef MPCFILE_H
#define MPCFILE_H

#include <QtCore>

#ifndef ASTRO_H
#include <astro.h>
#endif
#ifndef COMF_H
#include "comfunc.h"
#endif

QString trCatFlag(QString catFlag);

class mpcRec
{
    QString dataStr;
public:
    void getMpNumber(QString &mpNumber);
    void getMpUPackNumber(QString &mpNumber);
    //void getMpPackNumber(QString &mpNumber);
    //QString& getMpNumber();
    int mpNumber();
    void getProvDest(QString &provDest);
    void getDiscAst(QString &discAst);
    int discAstKey();
    void getNote1(QString &note1);
    void getNote2(QString &note2);
    double mjd();
    double ra();
    double dec();
    double magn();
    void getBand(QString &bandStr);
    void getCatFlag(QString &catFlag);
    void getCatName(QString &catName);
    //void getCatFamName(QString &catName);
    void getObsCode(QString &obsCode);
    int  getObsNum();

    void setPos(double radeg, double deDeg);
    void setTimePos(double mjdt, double radeg, double deDeg);
    void setMagn(double magn);
    void setName(QString name);
    void setProvDest(QString pName);

    int fromStr(QString dStr);
    int fromRec(mpcRec& source);
    QString toStr();
    mpcRec();
    mpcRec(QString nStr);
};

class mpcFile
{

public:
    QList <mpcRec*> recList;
    mpcFile();
    QString fileName;
    int init(QString fName);
    void addObs(QString mpNumber, QString provDest, double mjd, double ra, double de, double magn, QString discAster = " ", QString note1 = " ", QString note2 = " ", QString magnBand = " ");
    mpcRec* at(int i);
    int size();
    int removeAt(int pos);
    void addRec(mpcRec& nRec);
    int saveAs(QString fName);
    int save();
};



struct mpcObjRec
{
    QString objName;
    QList <mpcRec*> mpc_list;
    void sortByTime()
    {
        double tMin;
        int pMin;
        int mNum = mpc_list.size();

        for(int i=0; i<mNum-1;i++)
        {
            tMin = mpc_list.at(i)->mjd();
            pMin = i;
            for(int j=i+1; j<mNum; j++)
            {
                if(mpc_list.at(j)->mjd()<tMin)
                {
                    tMin = mpc_list.at(j)->mjd();
                    pMin = j;
                }
            }
            if(pMin!=i) mpc_list.swap(i, pMin);
        }
    }
    int size()
    {
        return mpc_list.size();
    }
};

class mpcObjList
{
public:
    QList <mpcObjRec*> obj_list;
    void addMpcRec(mpcRec* mRec, QString objName)
    {
        //QString objName;
        //mRec->getMpNumber(objName);
        int nobj =1;
        for(int i=0; i<obj_list.size(); i++)
        {
            if(QString().compare(objName, obj_list.at(i)->objName)==0)
            {
                obj_list.at(i)->mpc_list << mRec;
                nobj = 0;
                break;
            }
        }
        if(nobj)
        {
            mpcObjRec* mnRec = new mpcObjRec;
            mnRec->objName = objName;
            mnRec->mpc_list << mRec;
            obj_list << mnRec;
        }
    }

    void sortByTime()
    {
        for(int i=0; i<obj_list.size();i++) obj_list.at(i)->sortByTime();
    }

    int size()
    {
        return obj_list.size();
    }

};

#endif // MPCFILE_H
