/*#ifndef DELE_H
#include <dele.h>
#endif*/
#include "skyarealf.h"

tlRecord::tlRecord()
{
    this->desc = "";
    this->exp = 1.0;
    this->name = "";
    this->catName = "";
    //this->dirPath = "";
    this->dRA = 180.0;
    this->flag_active = 1;
    this->NinN = 0;
    this->Ntot = 0;
    this->texc = 0;
}

int tlRecord::fromString(QString tStr)
{
    QStringList strL;
    strL = tStr.split("|");
    if(strL.size()!=9) return 1;
    name = strL.at(0).simplified();
    catName = strL.at(1).simplified();
    flag_active = strL.at(2).toInt();
    exp = strL.at(3).toDouble();
    dRA = strL.at(4).toDouble();
    NinN = strL.at(5).toInt();
    Ntot = strL.at(6).toInt();
    texc = strL.at(7).toDouble();
    //dirPath = strL.at(8);
    desc = strL.at(8);

    //iList = NULL;

    return 0;
}

void tlRecord::toString(QString &tStr)
{
    tStr.clear();
    tStr.append(QString("%1|%2|%3|%4|%5|%6|%7|%8|%9").arg(name, 16).arg(catName, 16).arg(flag_active).arg(exp, 12, 'e').arg(dRA).arg(NinN).arg(Ntot).arg(texc).arg(desc));
}


bool operator== ( const tlRecord& lhs, const tlRecord& rhs )
{
    return(QString().compare(lhs.name, rhs.name, Qt::CaseSensitive)==0);
}

bool operator== ( const tlRecord& lhs, const tlRecord* rhs )
{
    return(QString().compare(lhs.name, rhs->name, Qt::CaseSensitive)==0);
}

tlRecord& tlRecord::operator=(const tlRecord &rhs) {
    // Check for self-assignment!
    if (this == &rhs)      // Same object?
      return *this;        // Yes, so skip assignment, and just return *this.

    this->desc = rhs.desc;
    this->exp = rhs.exp;
    this->name = rhs.name;
    this->catName = rhs.catName;
    //this->dirPath = rhs.dirPath;
    this->dRA = rhs.dRA;
    this->flag_active = rhs.flag_active;
    this->NinN = rhs.NinN;
    this->Ntot = rhs.Ntot;
    this->texc = rhs.texc;


    return *this;
}

void tlRecord::getIniName(QString &iniName)
{
    QString dirPath = QString("./%1").arg(name);
    QDir tDir(dirPath);
    iniName = QString("%1/ini.lst").arg(tDir.absolutePath());
}

void tlRecord::getCatName(QString &catName)
{
    QString dirPath = QString("./%1").arg(name);
    QDir tDir(dirPath);
    catName = QString("%1/ini.cat").arg(tDir.absolutePath());
}

int tlRecord::getIniList(iniList *iList)
{
    QString dirPath = QString("./%1").arg(name);
    QDir taskDir;
    taskDir.setPath(dirPath);
    if(!taskDir.exists()) QDir().mkpath(dirPath);

    QString iniName;

    getIniName(iniName);

    iList->init(iniName);
}

int tlRecord::updateIniList(iniList iList)
{
    QString iFileName, cFileName;

}

int tlRecord::removeObj(QString name)
{
    return(iList.removeObj(name));
}


void tlRecord::initIList()
{
    getIniList(&iList);
}

tlRecord* taskList::getTaskName(QString name)
{
    int i, p, sz;
    tlRecord* tRec;// = new tlRecord;
    sz = size();//recList.size();
    for(i=0; i<sz; i++)
    {
        //p = getRec(tRec, i);
        //if(p) return NULL;
        tRec = recList.at(i);

        if(QString().compare(name, tRec->name)==0) return tRec;
    }
    return NULL;
}

int taskList::getTaskName(QString name, tlRecord *tRec)
{
    int i, p, sz;
    tlRecord *tlLoc;
    sz = recList.size();
    for(i=0; i<sz; i++)
    {
        //p = getRec(tRec, i);
        //if(p) return NULL;
        tlLoc = recList[i];

        if(QString().compare(name, tlLoc->name)==0)
        {
            *tRec = *tlLoc;
            return 0;
        }
    }
    return 1;
}

int taskList::addRec(tlRecord& nRec)
{
    int i, sz;
    sz = recList.size();
    for(i=0; i<sz; i++)
    {
        if(nRec==recList.at(i)) return 1;
    }
    append(nRec);

    QString dirPath = QString("./%1").arg(nRec.name);
    QDir().mkdir(dirPath);
    return 0;
}

int taskList::addRec(tlRecord* nRec)
{
    int i, sz;
    sz = recList.size();
    for(i=0; i<sz; i++)
    {
        if(nRec==recList.at(i)) return 1;
    }
    append(*nRec);
    return 0;
}

int taskList::removeObj(QString taskName, QString objName)
{
    tlRecord *tr;
    tr = getTaskName(taskName);
    return(tr->removeObj(objName));
}
/*
int taskList::init(QString fname)
{
    listFile<tlRecord>::init(fname);

    int i, sz;
    sz = recList.size();

    for(i=0; i<sz; i++)
    {
        recList.at(i)->initIList();
    }


}

int taskList::save() //: listFile::save()
{
    listFile<tlRecord>::save();

    int i, sz;
    sz = recList.size();

    for(i=0; i<sz; i++)
    {
        recList.at(i)->iList.save();
    }


}
*/


////////////////////////////

iniRecord::iniRecord()
{
    name = "";
    exp = 0.0;
    //flagEvent = 0;
    //t0 = 0;
    //t1 = 0;
    //desc = "";
}

int iniRecord::fromString(QString tStr)
{
    QStringList strL;
    strL = tStr.split("|");
    if(strL.size()!=2) return 1;
    name = strL.at(0).simplified();
    exp = strL.at(1).toDouble();
    //flagEvent = strL.at(2).toInt();
    //t0 = strL.at(3).toDouble();
    //t1 = strL.at(4).toDouble();
    //desc = strL.at(2);

    return 0;
}

void iniRecord::toString(QString &tStr)
{
    tStr.clear();
    //tStr.append(QString("%1|%2|%3|%4|%5|%6").arg(name, 16).arg(exp, 12, 'e').arg(flagEvent).arg(t0, 14, 'f', 6).arg(t1, 14, 'f', 6).arg(desc));
    tStr.append(QString("%1|%2").arg(name, -16).arg(exp, 12, 'e'));
}

bool operator== ( const iniRecord& lhs, const iniRecord& rhs )
{
    return(QString().compare(lhs.name, rhs.name, Qt::CaseSensitive)==0);
}

bool operator== ( const iniRecord& lhs, const iniRecord* rhs )
{
    return(QString().compare(lhs.name, rhs->name, Qt::CaseSensitive)==0);
}




iniRecord& iniRecord::operator=(const iniRecord &rhs) {
    // Check for self-assignment!
    if (this == &rhs)      // Same object?
      return *this;        // Yes, so skip assignment, and just return *this.

    //... // Deallocate, allocate new space, copy values...
    //this->desc = rhs.desc;
    this->exp = rhs.exp;
    this->name = rhs.name;
//    this->t0 = rhs.t0;
//    this->t1 = rhs.t1;

    return *this;
}

int iniList::addRec(iniRecord& nRec)
{
    int i, sz;
    sz = recList.size();
    for(i=0; i<sz; i++)
    {
        if(&nRec==recList.at(i)) return 1;
    }
    append(nRec);
    return 0;
}

int iniList::addRec(iniRecord* nRec)
{
    int i, sz;
    sz = recList.size();
    for(i=0; i<sz; i++)
    {
        if(*nRec==recList.at(i)) return 1;
    }
    append(*nRec);
    return 0;
}

void iniList::addIniList(iniList newIniList)
{
    int i, sz;
    sz = newIniList.size();
    for(i=0; i<sz; i++)
    {
        addRec(newIniList.at(i));
    }
}

void iniList::updateIniList(iniList newIniList)
{
    int i, sz;
    recList.clear();
    recList << newIniList.recList;
}

int iniList::getRecName(iniRecord *nRec, QString name)
{
    int i, sz;
    sz = recList.size();
    for(i=0; i<sz; i++)
    {
        if(QString().compare(name,recList.at(i)->name)==0)
        {
            nRec = recList.at(i);
            return 0;
        }
    }
    return 1;
}

int iniList::removeObj(QString name)
{
    int i, sz;
    QString tStr;
    sz = recList.size();
    for(i=0; i<sz; i++)
    {
        tStr = recList.at(i)->name;
        if(QString().compare(name,tStr)==0)
        {
            recList.removeAt(i);
            return 0;
        }
    }
    return 1;
}

/////////////////////////////////////////////////

resRecord::resRecord()
{
    name = "";
    //taskName = "";
    //desc = "";
    ra = dec = muRacosD = muDec = magn = exp = 0.0;
}

int resRecord::fromString(QString tStr)
{
    QStringList strL;
    QString tStr0;
    strL = tStr.split("|");
    if(strL.size()!=10) return 1;
    number = strL.at(0).toInt();
    name = strL.at(1).simplified();
    ra = mas_to_grad(hms_to_mas(strL.at(2), " "));
    dec = mas_to_grad(damas_to_mas(strL.at(3), " "));
    magn = strL.at(4).toDouble();
    muRacosD = strL.at(5).toDouble();
    muDec = strL.at(6).toDouble();
    exp = strL.at(7).toDouble();
    catName = strL.at(8);
    tStr0 = strL.at(9);
    tasks = tStr0.split(",");


    return 0;
}

void resRecord::toString(QString &tStr)
{
    tStr.clear();
    tStr.append(QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10").arg(number, 8).arg(name, -16).arg(mas_to_hms(grad_to_mas(ra), " ", 3)).arg(mas_to_damas(grad_to_mas(dec), " ", 2)).arg(magn, 5, 'f', 2).arg(muRacosD, 14, 'f', 6).arg(muDec, 14, 'f', 6).arg(exp, 10).arg(catName, 16).arg(tasks.join(",")));
}


bool operator== ( const resRecord& lhs, const resRecord& rhs )
{
    return(QString().compare(lhs.name, rhs.name, Qt::CaseSensitive)==0);
}

bool operator== ( const resRecord& lhs, const resRecord* rhs )
{
    return(QString().compare(lhs.name, rhs->name, Qt::CaseSensitive)==0);
}

resRecord& resRecord::operator=(const resRecord &rhs) {
    // Check for self-assignment!
    if (this == &rhs)      // Same object?
      return *this;        // Yes, so skip assignment, and just return *this.

    //... // Deallocate, allocate new space, copy values...
    number = rhs.number;
    name = rhs.name;
    this->tasks = rhs.tasks;
    this->catName = rhs.catName;
    this->exp = rhs.exp;
    this->ra = rhs.ra;
    this->dec = rhs.dec;
    this->muRacosD = rhs.muRacosD;
    this->muDec = rhs.muDec;
    this->magn = rhs.magn;

    return *this;
}

void resRecord::getRaDec(double dTime, double &raRes, double &decRes)
{
    decRes = dec + mas_to_grad(muDec*1440.0*dTime);
    raRes = ra + mas_to_grad(muRacosD/cos(grad2rad(dec))*1440.0*dTime);
}

int resList::addRec(resRecord& nRec)
{
    int i, sz;
    sz = recList.size();
    for(i=0; i<sz; i++)
    {
        if(&nRec==recList.at(i)) return 1;
    }
    append(nRec);
    return 0;
}

int resList::addRec(resRecord* nRec)
{
    int i, sz;
    sz = recList.size();
    for(i=0; i<sz; i++)
    {
        if(*nRec==recList.at(i)) return 1;
    }
    append(*nRec);
    return 0;
}

int resList::addTaskName(QString name, QString taskName)
{
    int i, sz, j, szj;
    sz = recList.size();
    for(i=0; i<sz; i++)
    {
        if(QString().compare(name, recList.at(i)->name)==0)
        {
            szj = recList.at(i)->tasks.size();
            if(!recList.at(i)->tasks.contains(taskName)) recList.at(i)->tasks << taskName;
            return 1;
        }
    }
    return 0;
}

int resList::saveAs(QString tfName)
{
    fileName = tfName;
    return(save());
}

int resList::save()
{
    int i, sz;
    QString tstr;
    sz = recList.size();

    QFile resFile(fileName);
    if(!resFile.open(QFile::WriteOnly | QFile::Truncate)) return 1;
    QTextStream resStm(&resFile);

    resStm << QString("\%%1\n").arg(jdUTC, 15, 'f', 8);

    for(i=0; i<sz; i++)
    {
        recList.at(i)->toString(tstr);
        resStm << tstr << "\n";
    }

    resFile.close();
}

int resList::init(QString fname)
{
    fileName = fname;
    QFile iniFile(fileName);
    if(!iniFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "init ini.lst error\n";
        return 1;
    }
    QTextStream iniStm(&iniFile);

    QString tStr;

    recList.clear();
    resRecord* tRec;
    tRec = new resRecord;

    while(!iniStm.atEnd())
    {
        tStr = iniStm.readLine();

        if(tStr.indexOf("\%")==0)
        {
            jdUTC = tStr.mid(1, -1).toDouble();
            continue;
        }

        if(tRec->fromString(tStr)) continue;
        recList << tRec;
        tRec = new resRecord;
    }

    iniFile.close();

    delete tRec;

    return 0;
};

int resList::sortT(double s)
{
    int i, j, sz, pMin;
    sz = recList.size();
    double meriI, meriJ, meriMin;

    for(i=0; i<sz-1;i++)
    {
        meriI = recList.at(i)->ra - rad2grad(s);
        if(meriI<-180) meriI+=360;
        if(meriI>180) meriI-=360;
        pMin = i;
        meriMin = meriI;
        for(j=i+1;j<sz;j++)
        {
            meriJ = recList.at(j)->ra - rad2grad(s);
            if(meriJ<-180) meriJ+=360;
            if(meriJ>180) meriJ-=360;
            if(meriJ<meriMin)
            {
                meriMin = meriJ;
                pMin = j;
            }
        }
        if(pMin>i) recList.swap(i, pMin);
    }
}

int resList::sortDec()
{
    int i, j, sz, pMin;
    sz = recList.size();
    double valMin;

    for(i=0; i<sz-1;i++)
    {
        pMin = i;
        valMin = recList.at(i)->dec;
        for(j=i+1;j<sz;j++)
        {

            if(recList.at(j)->dec<valMin)
            {
                valMin = recList.at(j)->dec;
                pMin = j;
            }
        }
        if(pMin>i) recList.swap(i, pMin);
    }
}

int resList::sortMagn()
{
    int i, j, sz, pMin;
    sz = recList.size();
    double valMin;

    for(i=0; i<sz-1;i++)
    {
        pMin = i;
        valMin = recList.at(i)->magn;
        for(j=i+1;j<sz;j++)
        {

            if(recList.at(j)->magn<valMin)
            {
                valMin = recList.at(j)->magn;
                pMin = j;
            }
        }
        if(pMin>i) recList.swap(i, pMin);
    }
}

/////////////////////////////////////////////////

logRecord::logRecord()
{
    name = "";
    tasks.clear();
    jdTime = ra = dec = magn = exp = 0.0;
}

int logRecord::fromString(QString tStr)
{
    QStringList strL;
    QString tStr0;
    strL = tStr.split("|");
    if(strL.size()!=7) return 1;
    jdTime = mjd2jd(getMJDfromStrT(strL.at(0)));
    name = strL.at(1).simplified();
    ra = mas_to_grad(hms_to_mas(strL.at(2), " "));
    dec = mas_to_grad(damas_to_mas(strL.at(3), " "));
    magn = strL.at(4).toDouble();
    exp = strL.at(5).toDouble();
    tStr0 = strL.at(6);
    tasks = tStr0.split(",");


    return 0;
}

void logRecord::toString(QString &tStr)
{
    QString timeStr;
    tStr.clear();
    getStrTfromMJD(&timeStr, jd2mjd(jdTime));
    tStr.append(QString("%1|%2|%3|%4|%5|%6|%7").arg(timeStr).arg(name, 16).arg(mas_to_hms(grad_to_mas(ra), " ", 3)).arg(mas_to_damas(grad_to_mas(dec), " ", 2)).arg(magn, 5, 'f', 2).arg(exp, 10).arg(tasks.join(",")));
}


bool operator== ( const logRecord& lhs, const logRecord& rhs )
{
    return((QString().compare(lhs.name, rhs.name, Qt::CaseSensitive)==0)&&lhs.jdTime==rhs.jdTime);
}

bool operator== ( const logRecord& lhs, const logRecord* rhs )
{
    return((QString().compare(lhs.name, rhs->name, Qt::CaseSensitive)==0)&&lhs.jdTime==rhs->jdTime);
}

logRecord& logRecord::operator=(const logRecord &rhs) {
    // Check for self-assignment!
    if (this == &rhs)      // Same object?
      return *this;        // Yes, so skip assignment, and just return *this.

    //... // Deallocate, allocate new space, copy values...
    jdTime = rhs.jdTime;
    name = rhs.name;
    this->tasks = rhs.tasks;
    //this->desc = rhs.desc;
    this->exp = rhs.exp;
    this->ra = rhs.ra;
    this->dec = rhs.dec;
    this->magn = rhs.magn;

    return *this;
}



////////////////////////////////////////////////

int catRecord::fromString(QString tStr)
{
    QStringList strL;
    strL = tStr.split("|");
    if(strL.size()!=3) return 1;
    name = strL.at(0).simplified();
    catType = strL.at(1).toInt();
    catFile = strL.at(2).simplified();

    return 0;
}

void catRecord::toString(QString &tStr)
{
    tStr.clear();
    tStr.append(QString("%1|%2|%3").arg(name, 16).arg(catType).arg(catFile));
}

catRecord& catRecord::operator=(const catRecord &rhs) {
    // Check for self-assignment!
    if (this == &rhs)      // Same object?
      return *this;        // Yes, so skip assignment, and just return *this.

    //... // Deallocate, allocate new space, copy values...
    this->catType = rhs.catType;
    this->catFile = rhs.catFile;
    this->name = rhs.name;

    return *this;
}

bool operator== ( const catRecord& lhs, const catRecord& rhs )
{
    return(QString().compare(lhs.name, rhs.name, Qt::CaseSensitive)==0);
}

bool operator== ( const catRecord& lhs, const catRecord* rhs )
{
    return(QString().compare(lhs.name, rhs->name, Qt::CaseSensitive)==0);
}

QString catRecord::getCatTypeName()
{
    QStringList catTypes;
    catTypes << "dele";
    catTypes << "orbcat";
    catTypes << "lspm";
    catTypes << "mpccat";
    return(catTypes[catType]);
}

int catList::addRec(catRecord& nRec)
{
    int i, sz;
    sz = recList.size();
    for(i=0; i<sz; i++)
    {
        if(nRec==recList.at(i)) return 1;
    }
    append(nRec);
    return 0;
}

int catList::addRec(catRecord* nRec)
{
    int i, sz;
    sz = recList.size();
    for(i=0; i<sz; i++)
    {
        if(nRec==recList.at(i)) return 1;
    }
    append(*nRec);
    return 0;
}

catRecord* catList::getCatByName(QString cName)
{
    int i, sz;
    sz = recList.size();
    for(i=0; i<sz; i++)
    {
        if(QString().compare(cName, recList.at(i)->name)==0)
        {
            return(recList.at(i));
        }
    }
    return NULL;
}

int catList::getCatByName(QString cName, catRecord* cRec)
{
    int i, sz;
    sz = recList.size();
    //catRecord *cR;
    for(i=0; i<sz; i++)
    {
        if(QString().compare(cName, recList.at(i)->name)==0)
        {
            *cRec = *recList.at(i);
            return 0;
        }
    }
    return 1;
}

////////////////////////////////////////////////

////////////////////////////////////////////

saParams::saParams()
{
//	install_dir = new char[255];
//	strcpy(install_dir, ".\\\\");

    isAutoRA = 0;
    dRA0 = -0.78539816339744830961566084581988;
    dRA1 = 0.78539816339744830961566084581988;
    max_obj = 0;

    RAmin = 0.0;
    RAmax = 2.0*PI;
    DECmin = -PI/18.0;
    DECmax = PI/2.0;
    magn_min = 0.0;
    magn_max = 16.0;
}

saParams::~saParams()
{
//	delete [] this->install_dir;
}

////////////////////////////////////////////////

skyAreaLF::skyAreaLF()
{
    obs_pos = new observ();
    ctime = &obs_pos->ctime;
}

int skyAreaLF::init(QString obsCat, QString deleCat, QString insDir)
{
    obs_pos->init(obsCat.toAscii().data(), deleCat.toAscii().data());
    initInstallDir(insDir);




}

int skyAreaLF::initInstallDir(QString insDir)
{
    installDir.setPath(insDir);
    QString tlFile = QString("%1/task.lst").arg(installDir.absolutePath());
    if(task_list.init(tlFile)) return 1;

    int i, sz;
    sz = task_list.recList.size();

    for(i=0; i<sz; i++)
    {
        task_list.recList.at(i)->iList.init(getTaskIniName(task_list.recList.at(i)->name));
    }

    QString clFile = QString("%1/cat.lst").arg(installDir.absolutePath());
    if(cat_list.init(clFile)) return 1;
    return 0;
}

int skyAreaLF::updateIniList(iniList iList, QString taskName)
{
    tlRecord *tlRec = task_list.getTaskName(taskName);
    if(tlRec==NULL) return 1;
    iniList ini_list;
    QString iListName, cListName;
    tlRec->getIniName(iListName);
    ini_list.init(iListName);
    ini_list.updateIniList(iList);
    ini_list.save();

    return 0;
}

int skyAreaLF::addIniList(iniList iList, QString taskName)
{
    tlRecord *tlRec = task_list.getTaskName(taskName);
    if(tlRec==NULL) return 1;
    iniList ini_list;
    QString iListName, cListName;
    tlRec->getIniName(iListName);
    ini_list.init(iListName);
    ini_list.addIniList(iList);
    ini_list.save();

    return 0;
}

int skyAreaLF::updateLocalCats()
{
    QString globCatName, locCatName, iniName;
    int i, szT, catType, j, sz;
    tlRecord *tTemp = new tlRecord;
    catRecord *cTemp = new catRecord;
    iniList *iList = new iniList;

    sscatFB lspmCat;
    mpccat mpcCat;
    QFile outFile;
    QTextStream outStm;

    szT = task_list.size();

    for(i=0; i<szT; i++)
    {
        tTemp = task_list.at(i);
        tTemp->getIniList(iList);
        tTemp->getIniName(iniName);
        tTemp->getCatName(locCatName);
        cTemp = cat_list.getCatByName(tTemp->catName);
        globCatName = cTemp->catFile;

        if((cTemp->catType==DELE_CAT_TYPE)||(cTemp->catType==ORB_CAT_TYPE)) continue;

        outFile.setFileName(locCatName);
        outFile.open(QFile::WriteOnly | QFile::Truncate);
        outStm.setDevice(&outFile);

        sz = iList->size();
        switch(cTemp->catType)
        {
        case LSPM_CAT_TYPE:
            if(lspmCat.init(globCatName.toAscii().data()))
            {
                qDebug() << "\nglobal lspm cat error\n";
                continue;
            }
            //taskR->getCatName(locCatName);


            for(j=0; j<sz; j++)
            {
                if(lspmCat.GetName(iList->at(j)->name.toAscii().data())) continue;
                outStm << lspmCat.str << "\n";
            }



            break;
        case MPC_CAT_TYPE:
            if(mpcCat.init(globCatName.toAscii().data()))
            {
                qDebug() << "\nglobal MPC cat error\n";
                continue;
            }

            for(j=0; j<sz; j++)
            {
                if(mpcCat.GetRecName(iList->at(j)->name.toAscii().data())) continue;
                outStm << mpcCat.str << "\n";
            }

            break;

        }
        outFile.close();
    }


}

int skyAreaLF::save()
{
    cat_list.save();
    task_list.save();

    int i, sz;
    sz = task_list.recList.size();

    for(i=0; i<sz; i++)
    {
        task_list.recList.at(i)->iList.save();
    }


}

QString skyAreaLF::getTaskIniName(QString taskName)
{
    return(QString("%1/%2/ini.lst").arg(installDir.absolutePath()).arg(taskName));
}

QString skyAreaLF::getTaskCatName(QString taskName)
{
    return(QString("%1/%2/ini.cat").arg(installDir.absolutePath()).arg(taskName));
}

int skyAreaLF::getTaskCat(QString taskName, tlRecord *tRec, catRecord *cRec)
{
    if(task_list.getTaskName(taskName, tRec)) return 1;
    if(cat_list.getCatByName(tRec->catName, cRec)) return 1;
    return 0;
}

int skyAreaLF::removeObj(QString taskName, QString objName)
{
    return(task_list.removeObj(taskName, objName));
}

int skyAreaLF::initVisualProp(double jdUTC)
{
    double tr, ts;
    //qDebug() << "init_time_utc\n";
    if(this->init_time_utc(jdUTC))
    {
        qDebug() << "init_time_utc error\n";
        return 1;
    }

    //qDebug() << "detRDnumGC\n";
//	if(this->obs_pos->place->detR(&this->params.sunX, &this->params.sunY, &this->params.sunZ, this->obs_pos->otime, SUN_NUM, 0, this->obs_pos->center, obs_pos->sk)) return 1;
    detRDnumGC(&this->params.sunRA, &this->params.sunDE, 0.0, 0.0, 0.0, this->obs_pos->ox, this->obs_pos->oy, this->obs_pos->oz, this->obs_pos->obs->dcx, this->obs_pos->obs->dcy, this->obs_pos->obs->dcz);

    //qDebug() << "detRiseSet\n";
    detRiseSet(&tr, &ts, params.sunDE, this->obs_pos->obs->record->getFi());

        params.timeSunRise = tr;
        params.timeSunSet = ts;

/*
        params.timeSunRise = tr + this->params.sunRA;
        params.timeSunSet = ts + this->params.sunRA;

        if(params.timeSunRise>2.0*PI) params.timeSunRise -= 2.0*PI;
        if(params.timeSunSet>2.0*PI) params.timeSunSet -= 2.0*PI;
*/

    return 0;
}

int skyAreaLF::init_time_utc(double jDay)   //
{
    //qDebug() << "obs_pos: " << obs_pos << "\n";
    if(this->obs_pos->setUTC(jDay)) return 1;
    //this->timeCurUTC = this->obs_pos->ctime.UTC();
    return 0;
}

int skyAreaLF::set_opt(double RAmin, double RAmax, double DECmin, double DECmax, double magn_min, double magn_max)
{
    this->params.RAmin = RAmin;
    this->params.RAmax = RAmax;
    this->params.DECmin = DECmin;
    this->params.DECmax = DECmax;
    this->params.magn_min = magn_min;
    this->params.magn_max = magn_max;

    return 0;
}

int skyAreaLF::grade(resList &rList)
{
    int i, tn, sz;
    tlRecord *tl;
    catRecord *catR;
    iniList *iL = new iniList;
    iniRecord *iR;
    resRecord *resRec;
    mpccat *mpc_catalog;
    sscatFB *tsscat;
    OrbCat *orb_catalog;
    orbit orb_elem;
    QString strT;

    setlocale(LC_NUMERIC, "C");

    rList.clear();
    rList.jdUTC = this->obs_pos->ctime.UTC();

    getStrTfromMJD(&strT, jd2mjd(rList.jdUTC));
    qDebug() << QString("jdTime: %1\n").arg(strT);

    double x, y, z, vx, vy, vz, Sdist, Edist;
    int plaNum;
    double *state = new double[6];
    //mpcCat
    /*
    QString mpcFileName = "./mpc.txt";
    QFile mpcFile(mpcFileName);
    mpcFile.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream mpcStm(&mpcFile);

    QFile xyFile("xy.log");
    xyFile.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream xyStm(&xyFile);

//    mpc mrec;*/
    char *astr = new char[256];

    int taskNum = task_list.size();

    QTime timer;
    QString locCatName;

    timer.start();

    qDebug() << QString("%1: begin\n").arg(timer.elapsed());

    for(tn=0; tn<taskNum; tn++)
    {
        tl = task_list.at(tn);
        if(!tl->flag_active) continue;
        tl->getCatName(locCatName);
        catR = cat_list.getCatByName(tl->catName);

        qDebug() << QString("%1: task: %2\n").arg(timer.elapsed()).arg(tl->name);

        switch(catR->catType)
        {
        case ORB_CAT_TYPE:
            orb_catalog = new OrbCat();
            orb_catalog->is_buff = 1;
            orb_catalog->init(catR->catFile.toAscii().data());
            break;
        case LSPM_CAT_TYPE:
            tsscat = new sscatFB;
//			this->lspm_catalog->init(str_cat);
            tsscat->init(locCatName.toAscii().data());
            break;
        case MPC_CAT_TYPE:
            mpc_catalog = new mpccat();
            mpc_catalog->init(locCatName.toAscii().data());
            break;
        }

        tl->getIniList(iL);
        sz = iL->size();

        qDebug() << QString("%1: objnum: %2\n").arg(timer.elapsed()).arg(sz);


        for(i=0; i<sz; i++)
        {
            iR = iL->at(i);

            if(rList.addTaskName(iR->name, tl->name)) continue;


            resRec = new resRecord;

            switch(catR->catType)
            {
                case DELE_CAT_TYPE:
                {
                    plaNum = planet_num(iR->name.toAscii().data());
                    if(this->obs_pos->place->detState(&x, &y, &z, &vx, &vy, &vz, this->obs_pos->ctime.TDB(), plaNum, CENTER_SUN, SK_EKVATOR))
                    {
        //				AfxMessageBox("c0");
                        continue;
                    }


                    resRec->number = plaNum;
                    resRec->name = iR->name;
                                    //sprintf(this->res_list->record->num, "%8d", planet_num(this->ini_list->record->name));
                                    //resRec->num = planet_num(iR->name);

                    //det_res_list(resRec, this->obs_pos, x, y, z, vx, vy, vz, &Sdist, &Edist, catR->catType);
                    state[0] = x;
                    state[1] = y;
                    state[2] = z;
                    state[3] = vx;
                    state[4] = vy;
                    state[5] = vz;
                    det_res_list(resRec, this->obs_pos, state, &Sdist, &Edist, catR->catType, 0.0);


                    resRec->catName = "Planets";

                }
                break;
                case ORB_CAT_TYPE:
                {
                    if(orb_catalog->GetRecName(iR->name.toAscii().data())==-1)
                    {
                        qDebug() << QString("object %1 not found\n").arg(iR->name);
                        continue;
                    }

                    orb_elem.get(orb_catalog);
                    orb_elem.detRecEkv(&x, &y, &z, this->obs_pos->ctime.TDT());
                    orb_elem.detRecEkvVel(&vx, &vy, &vz, this->obs_pos->ctime.TDT());

                    resRec->number = orb_catalog->record->number;
                    resRec->name = QString(orb_catalog->record->name);

                    det_res_list(resRec, this->obs_pos, x, y, z, vx, vy, vz, &Sdist, &Edist, catR->catType, orb_catalog->record->H);
                    resRec->catName = "orbcat";

                }
                break;
                case LSPM_CAT_TYPE:
                {
                    if(tsscat->GetName(iR->name.toAscii().data())) continue;
    //				if(tsscat->GetRecNameLSPM(this->ini_list->record->name)) continue;

                    resRec->ra = tsscat->record->RAdeg + mas_to_grad(1000*tsscat->record->pmRA/cos(grad2rad(tsscat->record->DEdeg))*(getYearFromMJD(jd2mjd(this->obs_pos->ctime.TDB()))-2000));//taking proper motion into account
                    resRec->dec = tsscat->record->DEdeg + mas_to_grad(1000*tsscat->record->pmDE*(getYearFromMJD(jd2mjd(this->obs_pos->ctime.TDB()))-2000));

                    resRec->magn = tsscat->record->BJmag;

                    resRec->muRacosD = 0.0;
                    resRec->muDec = 0.0;

                    resRec->name = QString(iR->name);
                    resRec->number = 0;

                    resRec->catName = "LSPM";
                }
                break;
                case MPC_CAT_TYPE:
                {
                    if(mpc_catalog->GetRecName(iR->name.toAscii().data()))
                    {
                        qDebug() << QString("mpc_catalog object not found\n");
                        continue;
                    }
                    orb_elem.get(mpc_catalog);
                    orb_elem.detRecEkv(&x, &y, &z, this->obs_pos->ctime.TDT());
                    orb_elem.detRecEkvVel(&vx, &vy, &vz, this->obs_pos->ctime.TDT());

                    resRec->number = mpc_catalog->record->getNum();
                    resRec->name = QString(mpc_catalog->record->name).simplified();

                    //det_res_list(resRec, this->obs_pos, x, y, z, vx, vy, vz, &Sdist, &Edist, catR->catType, mpc_catalog->record->H);

                    state[0] = x;
                    state[1] = y;
                    state[2] = z;
                    state[3] = vx;
                    state[4] = vy;
                    state[5] = vz;
                    det_res_list(resRec, this->obs_pos, state, &Sdist, &Edist, catR->catType, mpc_catalog->record->H);

                    resRec->catName = "MPCCAT";
/*
                    mrec.r = grad2rad(resRec->ra);// + dRa;
                    mrec.d = grad2rad(resRec->dec);// + dDec;

                    mrec.eJD = this->obs_pos->ctime.UTC();
                    //mrec.num = 1;
                    mpc_catalog->record->getNumStr(mrec.head->Snum);
                    //strcpy(, mCat.record->getNumStr(>number);
                    mrec.tail->set_numOfObs(obs_pos->obs->record->num);
                    mrec.toString(astr);

                    mpcStm << astr << "\n";

                    xyStm << QString("%1|%2|%3|%4|%5|%6|%7|%8\n").arg(resRec->name).arg(obs_pos->ctime.TDB(), 15, 'f',7).arg(state[0], 15, 'e', 10).arg(state[1], 15, 'e', 10).arg(state[2], 15, 'e', 10).arg(state[3], 15, 'e', 10).arg(state[4], 15, 'e', 10).arg(state[5], 15, 'e', 10);*/
                }
                break;
            }
            resRec->exp = iR->exp;
            resRec->tasks << tl->name;
            rList.addRec(resRec);
        }
    }
    //mpcFile.close();
}


void det_res_list(resRecord *resRec, observ *obs_pos, double x, double y, double z, double vx, double vy, double vz, double *Sdist, double *Edist, int ctype, double H)
{

    double normP, normE, normQ;
    double *P, *E, *Q, *X;
    double ct1, ct0, tau, muc2;

    muc2 = 9.8704e-9;

    X = new double[3];
    P = new double[3];
    E = new double[3];
    Q = new double[3];

    X[0] = x;
    X[1] = y;
    X[2] = z;

    Q[0] = x;
    Q[1] = y;
    Q[2] = z;

    E[0] = obs_pos->ox + obs_pos->obs->dcx;
    E[1] = obs_pos->oy + obs_pos->obs->dcy;
    E[2] = obs_pos->oz + obs_pos->obs->dcz;

    P[0] = Q[0] - E[0];
    P[1] = Q[1] - E[1];
    P[2] = Q[2] - E[2];

    normP = norm(P);
    normE = norm(E);
    normQ = norm(Q);

    ct1 = 0.0;

    do
    {
        ct0 = ct1;

        ct1 = normP+2.0*muc2*log((normE+normQ+normP)/(normE+normQ-normP));
        //qDebug() << QString("ct1: %1\n").arg(ct1, 10);
        tau = ct1/CAU;
        Q[0] = X[0] - vx*tau;
        Q[1] = X[1] - vy*tau;
        Q[2] = X[2] - vz*tau;
        normQ = norm(Q);

    }while((fabs(ct1-ct0)/fabs(ct1))>1e-10);

    tau = ct1/CAU;

    P[0] = Q[0] - E[0];
    P[1] = Q[1] - E[1];
    P[2] = Q[2] - E[2];

    rdsys(&resRec->ra, &resRec->dec, P[0], P[1], P[2]);

    //detRDnumGC(&resRec->ra, &resRec->dec, P[0], P[1], P[2], 0, 0, 0, 0, 0, 0);

        //detRDnumGC(&resRec->ra, &resRec->dec, x, y, z, obs_pos->ox, obs_pos->oy, obs_pos->oz, obs_pos->obs->dcx, obs_pos->obs->dcy, obs_pos->obs->dcz);

        resRec->ra = rad2grad(resRec->ra);
        resRec->dec = rad2grad(resRec->dec);

    detRDnumGC(&resRec->muRacosD, &resRec->muDec, vx, vy, vz, obs_pos->ox, obs_pos->oy, obs_pos->oz, obs_pos->obs->dcx, obs_pos->obs->dcy, obs_pos->obs->dcz);


    resRec->muRacosD = rad2mas(resRec->muRacosD)*1440.0;
    resRec->muDec = rad2mas(resRec->muDec)*1440.0;

    *Sdist = sqrt(x*x + y*y + z*z);
    *Edist = sqrt((obs_pos->ox - x)*(obs_pos->ox - x) + (obs_pos->oy - y)*(obs_pos->oy - y) + (obs_pos->oz - z)*(obs_pos->oz - z));

    if(ctype==1) resRec->magn = det_m(H, *Sdist, *Edist, 5.8, detPhase(obs_pos->ox, obs_pos->oy, obs_pos->oz, x, y, z));
    if(ctype==3) resRec->magn = det_m(H, *Sdist, *Edist, 5.8, detPhase(obs_pos->ox, obs_pos->oy, obs_pos->oz, x, y, z));
    if(ctype==0) resRec->magn = det_m(det_planet_H(resRec->number)-5.5, *Sdist, *Edist, 5.8, detPhase(obs_pos->ox, obs_pos->oy, obs_pos->oz, x, y, z));


    delete [] P;
    delete [] Q;
    delete [] E;

}

void det_res_list(resRecord *resRec, observ *obs_pos, double *state, double *Sdist, double *Edist, int ctype, double H)
{
    double *range = new double[3];
    double ra, dec;

    obs_pos->det_vect_radec(state, &ra, &dec, range);

    //rdsys(&resRec->ra, &resRec->dec, P[0], P[1], P[2]);

    //detRDnumGC(&resRec->ra, &resRec->dec, P[0], P[1], P[2], 0, 0, 0, 0, 0, 0);

        //detRDnumGC(&resRec->ra, &resRec->dec, x, y, z, obs_pos->ox, obs_pos->oy, obs_pos->oz, obs_pos->obs->dcx, obs_pos->obs->dcy, obs_pos->obs->dcz);

        resRec->ra = rad2grad(ra);
        resRec->dec = rad2grad(dec);

        //rdsys(&resRec->muRacosD, &resRec->muDec, , P2, P3);
        detRDnumGC_vel(&resRec->muRacosD, &resRec->muDec, state[3], state[4], state[5], obs_pos->ovx, obs_pos->ovy, obs_pos->ovz, obs_pos->obs->state[3], obs_pos->obs->state[4], obs_pos->obs->state[5], range);

        resRec->muRacosD = rad2mas(resRec->muRacosD)/1440.0;
        resRec->muDec = rad2mas(resRec->muDec)/1440.0;

    *Sdist = sqrt(state[0]*state[0] + state[1]*state[1] + state[2]*state[2]);
    *Edist = sqrt(range[0]*range[0] + range[1]*range[1] + range[2]*range[2]);
    //*Edist = sqrt((obs_pos->ox - state[0])*(obs_pos->ox - state[0]) + (obs_pos->oy - state[1])*(obs_pos->oy - state[1]) + (obs_pos->oz - state[2])*(obs_pos->oz - state[2]));

    if(ctype==1) resRec->magn = det_m(H, *Sdist, *Edist, 5.8, detPhase(obs_pos->ox, obs_pos->oy, obs_pos->oz, state[0], state[1], state[2]));
    if(ctype==3) resRec->magn = det_m(H, *Sdist, *Edist, 5.8, detPhase(obs_pos->ox, obs_pos->oy, obs_pos->oz, state[0], state[1], state[2]));
    if(ctype==0) resRec->magn = det_m(det_planet_H(resRec->number)-5.5, *Sdist, *Edist, 5.8, detPhase(obs_pos->ox, obs_pos->oy, obs_pos->oz, state[0], state[1], state[2]));

/*
    delete [] P;
    delete [] Q;
    delete [] E;
*/
}
