#include "skyarealf.h"

tlRecord::tlRecord()
{
    this->desc = "";
    this->exp = 1.0;
    this->name = "";
    this->catName = "";
    this->dirPath = "";
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
    this->dirPath = rhs.dirPath;
    this->dRA = rhs.dRA;
    this->flag_active = rhs.flag_active;
    this->NinN = rhs.NinN;
    this->Ntot = rhs.Ntot;
    this->texc = rhs.texc;


    return *this;
}

void tlRecord::getIniName(QString &iniName)
{
    QDir tDir(dirPath);
    iniName = QString("%1/ini.lst").arg(tDir.absolutePath());
}

void tlRecord::getCatName(QString &catName)
{
    QDir tDir(dirPath);
    catName = QString("%1ini.cat").arg(tDir.absolutePath());
}

int tlRecord::getIniList(iniList *iList)
{
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

int taskList::addRec(tlRecord& nRec)
{
    int i, sz;
    sz = recList.size();
    for(i=0; i<sz; i++)
    {
        if(nRec==recList.at(i)) return 1;
    }
    append(nRec);

    QDir().mkdir(nRec.dirPath);
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
    tStr.append(QString("%1|%2|%3").arg(name, -16).arg(exp, 12, 'e'));
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
    ra = dec = mu_ra = mu_dec = magn = exp = 0.0;
}

int resRecord::fromString(QString tStr)
{
    QStringList strL;
    QString tStr0;
    strL = tStr.split("|");
    if(strL.size()!=9) return 1;
    number = strL.at(0).toInt();
    name = strL.at(1);
    ra = strL.at(2).toDouble();
    dec = strL.at(3).toDouble();
    magn = strL.at(4).toDouble();
    mu_ra = strL.at(5).toDouble();
    mu_dec = strL.at(6).toDouble();
    tStr0 = strL.at(7);
    tasks = tStr0.split(",");
    exp = strL.at(8).toDouble();

    return 0;
}

void resRecord::toString(QString &tStr)
{
    tStr.clear();
    tStr.append(QString("%1|%2|%3|%4|%5|%6|%7|%8|%9").arg(number, 8).arg(name, 16).arg(ra, 14, 'f', 6).arg(dec, 14, 'f', 6).arg(magn, 5, 'f', 2).arg(mu_ra, 14, 'f', 6).arg(mu_dec, 14, 'f', 6).arg(tasks.join(",")).arg(exp, 10));
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
    name = rhs.name;
    this->tasks = rhs.tasks;
    //this->desc = rhs.desc;
    this->exp = rhs.exp;
    this->ra = rhs.ra;
    this->dec = rhs.dec;
    this->mu_ra = rhs.mu_ra;
    this->mu_dec = rhs.mu_dec;
    this->magn = rhs.magn;

    return *this;
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
    catTypes << "mpccat";
    catTypes << "lspm";
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

int skyAreaLF::removeObj(QString taskName, QString objName)
{
    return(task_list.removeObj(taskName, objName));
}

int skyAreaLF::initVisualProp(double jDay)
{
    double tr, ts;
    this->init_time(jDay);

//	if(this->obs_pos->place->detR(&this->params.sunX, &this->params.sunY, &this->params.sunZ, this->obs_pos->otime, SUN_NUM, 0, this->obs_pos->center, obs_pos->sk)) return 1;
    detRDnumGC(&this->params.sunRA, &this->params.sunDE, 0.0, 0.0, 0.0, this->obs_pos->ox, this->obs_pos->oy, this->obs_pos->oz, this->obs_pos->obs->dcx, this->obs_pos->obs->dcy, this->obs_pos->obs->dcz);

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

int skyAreaLF::init_time(double jDay)
{
    this->timeCur = jDay;
    if(this->obs_pos->det_observ(jDay)) return 1;

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
    iniList *iL;
    iniRecord *iR;
    resRecord *resRec;
    mpccat *mpc_catalog;
    sscatFB *tsscat;
    OrbCat *orb_catalog;
    orbit orb_elem;

    rList.clear();

    double x, y, z, vx, vy, vz, Sdist, Edist;
    int plaNum;
    //mpcCat

    int taskNum = task_list.size();

    for(tn=0; tn<taskNum; tn++)
    {
        tl = task_list.at(tn);
        catR = cat_list.getCatByName(tl->catName);


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
            tsscat->init(catR->catFile.toAscii().data());
            break;
        case 3:
            mpc_catalog = new mpccat();
            mpc_catalog->init(catR->catFile.toAscii().data());
            break;
        }

        tl->getIniList(iL);
        sz = iL->size();

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
                    if(this->obs_pos->place->detState(&x, &y, &z, &vx, &vy, &vz, this->obs_pos->otime, plaNum, CENTER_SUN, SK_EKVATOR))
                    {
        //				AfxMessageBox("c0");
                        continue;
                    }


                    resRec->number = plaNum;
                    resRec->name = iR->name;
                                    //sprintf(this->res_list->record->num, "%8d", planet_num(this->ini_list->record->name));
                                    //resRec->num = planet_num(iR->name);

                    det_res_list(resRec, this->obs_pos, x, y, z, vx, vy, vz, &Sdist, &Edist, catR->catType);
                    resRec->exp = iR->exp;
                    rList.addRec(resRec);
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
                    orb_elem.detRecEkv(&x, &y, &z, this->obs_pos->otime);
                    orb_elem.detRecEkvVel(&vx, &vy, &vz, this->obs_pos->otime);

                    resRec->number = orb_catalog->record->number;
                    resRec->name = QString(orb_catalog->record->name);

                    det_res_list(resRec, this->obs_pos, x, y, z, vx, vy, vz, &Sdist, &Edist, catR->catType, orb_catalog->record->H);
                    resRec->exp = iR->exp;
                    rList.addRec(resRec);

                }
                break;
                case LSPM_CAT_TYPE:
                {
                    if(tsscat->GetName(iR->name.toAscii().data())) continue;
    //				if(tsscat->GetRecNameLSPM(this->ini_list->record->name)) continue;

                    resRec->ra = tsscat->record->RAdeg + mas_to_grad(1000*tsscat->record->pmRA/cos(grad2rad(tsscat->record->DEdeg))*(getYearFromMJD(jd2mjd(this->obs_pos->otime))-2000));//taking proper motion into account
                    resRec->dec = tsscat->record->DEdeg + mas_to_grad(1000*tsscat->record->pmDE*(getYearFromMJD(jd2mjd(this->obs_pos->otime))-2000));

                    resRec->magn = tsscat->record->BJmag;

                    resRec->mu_ra = 0.0;
                    resRec->mu_dec = 0.0;

                    resRec->name = QString(iR->name);
                    resRec->number = 0;
                    rList.addRec(resRec);
                }
                break;
                case MPC_CAT_TYPE:
                {
                    if(mpc_catalog->GetRecName(iR->name.toAscii().data()))
                    {
//				AfxMessageBox("c1");
                        continue;
                    }
                    orb_elem.get(mpc_catalog);
                    orb_elem.detRecEkv(&x, &y, &z, this->obs_pos->otime);
                    orb_elem.detRecEkvVel(&vx, &vy, &vz, this->obs_pos->otime);

                    resRec->number = orb_catalog->record->number;
                    resRec->name = QString(orb_catalog->record->name);

                    det_res_list(resRec, this->obs_pos, x, y, z, vx, vy, vz, &Sdist, &Edist, catR->catType, orb_catalog->record->H);
                    resRec->exp = iR->exp;
                    rList.addRec(resRec);

                }
                break;
            }
        }
    }
}


void det_res_list(resRecord *resRec, observ *obs_pos, double x, double y, double z, double vx, double vy, double vz, double *Sdist, double *Edist, int ctype, double H)
{
    double normR, norm_sA;
    double *s, *sA, *R, *sV;
    double *v0, *v1, *v2;

    s = new double[3];
    sA = new double[3];
    sV = new double[3];
    R = new double[3];
    v0 = new double[3];
    v1 = new double[3];
    v2 = new double[3];

    R[0] = x - obs_pos->ox;
    R[1] = y - obs_pos->oy;
    R[2] = z - obs_pos->oz;

    normR = norm(R);

    s[0] = R[0]/normR;
    s[1] = R[1]/normR;
    s[2] = R[2]/normR;

    sV[0] = vx - obs_pos->ovx;
    sV[1] = vy - obs_pos->ovy;
    sV[2] = vz - obs_pos->ovz;

    Vmul3(v0, s, sV);
    Vmul3(v1, s, v0);

    sA[0] = s[0] + v1[0]*(1.0/CAU);
    sA[1] = s[1] + v1[1]*(1.0/CAU);
    sA[2] = s[2] + v1[2]*(1.0/CAU);

    norm_sA = norm(sA);

    rdsys(&resRec->ra, &resRec->dec, sA[0], sA[1], sA[2]);
        //detRDnumGC(&resRec->ra, &resRec->dec, x, y, z, obs_pos->ox, obs_pos->oy, obs_pos->oz, obs_pos->obs->dcx, obs_pos->obs->dcy, obs_pos->obs->dcz);

        resRec->ra = rad2grad(resRec->ra);
        resRec->dec = rad2grad(resRec->dec);

    detRDnumGC(&resRec->mu_ra, &resRec->mu_dec, vx, vy, vz, obs_pos->ox, obs_pos->oy, obs_pos->oz, obs_pos->obs->dcx, obs_pos->obs->dcy, obs_pos->obs->dcz);

    resRec->mu_ra = grad_to_mas(rad2grad(resRec->mu_ra))/1000.0/86400.0/cos(grad2rad(resRec->dec));
    resRec->mu_dec = grad_to_mas(rad2grad(resRec->mu_dec))/1000.0/86400.0;

    *Sdist = sqrt(x*x + y*y + z*z);
    *Edist = sqrt((obs_pos->ox - x)*(obs_pos->ox - x) + (obs_pos->oy - y)*(obs_pos->oy - y) + (obs_pos->oz - z)*(obs_pos->oz - z));

    if(ctype==1) resRec->magn = det_m(H, *Sdist, *Edist, 5.8, detPhase(obs_pos->ox, obs_pos->oy, obs_pos->oz, x, y, z));
    if(ctype==3) resRec->magn = det_m(H, *Sdist, *Edist, 5.8, detPhase(obs_pos->ox, obs_pos->oy, obs_pos->oz, x, y, z));
    if(ctype==0) resRec->magn = det_m(det_planet_H(resRec->number)-5.5, *Sdist, *Edist, 5.8, detPhase(obs_pos->ox, obs_pos->oy, obs_pos->oz, x, y, z));

    delete [] s;
    delete [] sA;
    delete [] sV;
    delete [] R;
    delete [] v0;
    delete [] v1;
    delete [] v2;
}
