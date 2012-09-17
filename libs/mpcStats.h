#ifndef MPCSTATS_H
#define MPCSTATS_H

struct obsCodeCounter
{
    QString obsCode;
    int count;
    obsCodeCounter()
    {
        obsCode="";
        count = 0;
    };
};

struct objCounter
{
    QString objNum;
    int count;
    objCounter()
    {
        objNum="";
        count = 0;
    };
};

struct yearCounter
{
    int year;
    int count;
    yearCounter()
    {
        year=0;
        count = 0;
    };
};

struct catFlagCounter
{
    QString catFlag;
    int count;
    catFlagCounter()
    {
        catFlag="";
        count = 0;
    };
};

void addObsCode(QList <obsCodeCounter*> &obsList, QString obsCode)
{
    int i, sz;
    obsCodeCounter* obsRec;
    sz = obsList.count();
    for(i=0; i<sz; i++)
    {
        if(QString().compare(obsList.at(i)->obsCode, obsCode)==0)
        {
            obsList.at(i)->count++;
            return;
        }

    }
    obsRec = new obsCodeCounter;
    obsRec->obsCode = obsCode;
    obsRec->count=1;
    obsList << obsRec;

}

void addCatFlag(QList <catFlagCounter*> &cfList, QString catFlag)
{
    int i, sz;
    catFlagCounter* cfRec;
    sz = cfList.count();
    for(i=0; i<sz; i++)
    {
        if(QString().compare(cfList.at(i)->catFlag, catFlag)==0)
        {
            cfList.at(i)->count++;
            return;
        }

    }
    cfRec = new catFlagCounter;
    cfRec->catFlag = catFlag;
    cfRec->count=1;
    cfList << cfRec;

}

void addYear(QList <yearCounter*> &yrList, int year)
{
    int i, sz;
    yearCounter* yrRec;
    sz = yrList.count();
    for(i=0; i<sz; i++)
    {
        if(yrList.at(i)->year==year)
        {
            yrList.at(i)->count++;
            return;
        }

    }
    yrRec = new yearCounter;
    yrRec->year = year;
    yrRec->count=1;
    yrList << yrRec;

}

void addObjNum(QList <objCounter*> &objList, QString objNum)
{
    int i, sz;
    objCounter* objRec;
    sz = objList.count();
    for(i=0; i<sz; i++)
    {
        if(QString().compare(objList.at(i)->objNum, objNum)==0)
        {
            objList.at(i)->count++;
            return;
        }

    }
    objRec = new objCounter;
    objRec->objNum = objNum;
    objRec->count=1;
    objList << objRec;

}

void sortObsNum(QList <obsCodeCounter*> &obsList)
{
    int i, j, sz;
    sz = obsList.size();
    for(i=0;i<sz-1;i++)
    {
        for(j=i+1;j<sz;j++)
        {
            if(obsList.at(i)->count<obsList.at(j)->count) obsList.swap(i, j);
        }
    }
}

void sortYear(QList <yearCounter*> &yrList, int dir = 1)
{
    int i, j, sz;
    sz = yrList.size();
    for(i=0;i<sz-1;i++)
    {
        for(j=i+1;j<sz;j++)
        {
            if(dir*yrList.at(i)->year<yrList.at(j)->year*dir) yrList.swap(i, j);
        }
    }
}

void sortObjNum(QList <objCounter*> &objList)
{
    int i, j, sz;
    sz = objList.size();
    for(i=0;i<sz-1;i++)
    {
        for(j=i+1;j<sz;j++)
        {
            if(objList.at(i)->count<objList.at(j)->count) objList.swap(i, j);
        }
    }
}

#endif // MPCSTATS_H
