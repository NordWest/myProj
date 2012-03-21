#include "stdafx.h"
#include "skyarea.h"
//#include "..\DAster\Eassistant\StdAfx.h"
//#include "..\DAster\OMen\StdAfx.h"

TLRecord::TLRecord()
{
	this->name = new char[33];
	this->cat_name = new char[17];
	this->desc = new char[256];
	this->dir_path = new char[256];

	this->set_desc("");
	this->set_dir_path("");
	this->set_name("");
	this->set_cat_name("");

	this->exp = 0.0;
	this->noftask = 0;
	this->dRA = PI;
	this->NinN = 0;
	this->Ntot = 0;
	this->texc = 0.0;
	this->flag_active = 1;
	this->tail = new char[33];
	strcpy(tail, "\x0D");
}

TLRecord::~TLRecord()
{
	delete [] this->name;
	this->name = NULL;
	this->exp = 0.0;
	this->noftask = 0;
	delete [] this->tail;
	delete [] this->desc;
	delete [] this->dir_path;
	delete [] this->cat_name;
}

int TLRecord::set_desc(char *desc)
{
	return(indexB(this->desc, desc, " ", 255));
}

int TLRecord::set_dir_path(char *dir_path)
{
	return(indexB(this->dir_path, dir_path, " ", 255));
}

int TLRecord::set_name(char *name)
{
	return(indexB(this->name, name, " ", 32));
}

int TLRecord::set_cat_name(char *cat_name)
{
	return(indexB(this->cat_name, cat_name, " ", 16));
}


void TLRecord::copyTo(TLRecord *rcp)
{
	rcp->noftask = this->noftask;
	rcp->set_name(this->name);
	rcp->set_cat_name(this->cat_name);
	rcp->exp = this->exp;
	rcp->Ntot = this->Ntot;
	rcp->NinN = this->NinN;
	rcp->texc = this->texc;
	rcp->dRA = this->dRA;
	rcp->set_desc(this->desc);
	rcp->flag_active = this->flag_active;
	rcp->set_dir_path(this->dir_path);
	strcpy(rcp->tail, this->tail);
//	rcp->cat_type = this->cat_type;
}

char* TLRecord::get_name()
{
	return(this->name);
}

char* TLRecord::get_cat_name()
{
	return(this->cat_name);
}

char* TLRecord::get_desc()
{
	return this->desc;
}

char* TLRecord::get_dir_path()
{
//	int pose;
//	char
//	slovo(this->dir_path, dpath, 0, &pose);
	return this->dir_path;
}

void TLRecord::get_dir_path(char *dp)
{
	int pose;
	slovo(this->get_dir_path(), dp, 0, &pose);
}

void TLRecord::get_ini_fname(char *dp)
{
	if(dp==NULL) return;

	this->get_dir_path(dp);
	strcat(dp, "\\ini.lst");
}

void TLRecord::get_cat_fname(char *dp)
{
	if(dp==NULL) return;

	this->get_dir_path(dp);
	strcat(dp, "\\ini.cat");
}


TaskList::TaskList(int sizestr) : FileDynStrBin(sizestr)
{
	this->record = new TLRecord;
	this->ini_lst = new IList(ILIST_LEN);
	this->orb_catalog = new OrbCat;
	this->lspm_catalog = new sscat();
	this->str_cat = new char[255];
	this->str_ini = new char[255];

}

TaskList::TaskList() : FileDynStrBin(TLIST_LEN)
{
	this->record = new TLRecord;
	this->ini_lst = new IList(ILIST_LEN);
	this->orb_catalog = new OrbCat;
	this->lspm_catalog = new sscat();
	this->str_cat = new char[255];
	this->str_ini = new char[255];
}

TaskList::~TaskList()
{
	delete(this->record);
	this->record = NULL;
	delete this->ini_lst;
	delete this->orb_catalog;
	delete this->lspm_catalog;
	delete [] this->str_cat;
	delete [] this->str_ini;
}

int TaskList::init(char *fname)
{

        return(FileDynStrBin::init(fname));
}

int TaskList::init(char *fname, int sizestr)
{
        return(FileDynStrBin::init(fname));
}

int TaskList::AddRec()
{
	char *stmp;
	stmp = new char[SIMPLE_STR];

	this->rec2s(stmp);
        int res = this->AddStr(stmp, nstr());

//	delete [] stmp;

	return(res);
}
/*
int TaskList::AddTask()
{
	int i, pos;

	pos = this->GetTaskNum(ntask->noftask);

//	ntask->copyTo(this->record);

	if(pos==-1) this->AddRec();
	else this->UpdateRec(pos);

//creating dir

	TCHAR szDirPath[] = TEXT("");
	sprintf(szDirPath, "%s", ntask->get_dir_path());

	if (!CreateDirectory(szDirPath, NULL))
	{
		if(GetLastError()==ERROR_PATH_NOT_FOUND) return 1;
	}


	return 0;
}
*/
int TaskList::AddTask(TLRecord *ntask)
{
	int i, pos0, pos1, pose;

	pos0 = this->GetTaskNum(ntask->noftask);
	pos1 = this->GetTaskName(ntask->get_name());

	ntask->copyTo(this->record);

	if((pos0==-1)&&(pos1==-1))
	{
		this->AddRec();

		

	}
	else if(pos0>-1) this->UpdateRec(pos0);
	else if(pos1>-1) this->UpdateRec(pos1);

//creating dir

	return 0;
}

int TaskList::UpdateRec(int pos)
{
	char *nnstr;
	nnstr = new char[SIMPLE_STR];

	this->rec2s(nnstr);
	if(this->Update(nnstr, pos)) return 1;

	return 0;
}


int TaskList::GetRec(int i)
{
	if(this->ReadStr(i)) return 1;

        this->s2rec(str);

	return 0;
}

int TaskList::GetRec(char *name)
{
	int i = 0;

	while(!this->ReadStr(i))
	{
		this->s2rec(this->str);
		if(streqv(this->record->get_name(), name)) return 0;
		i++;
	}

	return 1;
}

int TaskList::GetTaskNum(int noft)
{
	int i = 0;

	while(!this->ReadStr(i))
	{
		this->s2rec(this->str);
		if(noft==this->record->noftask) return i;
		i++;
	}

	return -1;
}

int TaskList::GetTaskName(char *noft)
{
	int i = 0;

	while(!this->ReadStr(i))
	{
		this->s2rec(this->str);
		if(streqv(noft, this->record->get_name())) return this->record->noftask;
		i++;
	}

	return -1;
}



int TaskList::IsTaskActive(int noft)
{
	if(this->GetTaskNum(noft)==-1) return 0;

	return(this->record->flag_active);

}


void TaskList::rec2s(char *nnstr)
{
	sprintf(nnstr, "%5d%32s%+10e%10d%10d%10e%+10e%s%1d%s%s%s", this->record->noftask, this->record->get_name(), this->record->exp, this->record->Ntot, this->record->NinN, this->record->texc, this->record->dRA, this->record->get_desc(), this->record->flag_active, this->record->get_cat_name(), this->record->get_dir_path(), this->record->tail);
}

void TaskList::s2rec(char *nnstr)
{
	char *tempstr;
	tempstr = new char[SIMPLE_STR];

	slovoG(this->str, tempstr, 0, 5);
	this->record->noftask = atoi(tempstr);

	slovoG(this->str, tempstr, 5, 37);
	strcpy(this->record->get_name(), tempstr);
	strcpy(&this->record->get_name()[32], "\0");

	slovoG(this->str, tempstr, 37, 51);
	this->record->exp = atof(tempstr);

	slovoG(this->str, tempstr, 51, 61);
	this->record->Ntot = atoi(tempstr);

	slovoG(this->str, tempstr, 61, 71);
	this->record->NinN = atoi(tempstr);

	slovoG(this->str, tempstr, 71, 84);
	this->record->texc = atof(tempstr);

	slovoG(this->str, tempstr, 84, 98);
	this->record->dRA = atof(tempstr);

	slovoG(this->str, tempstr, 98, 353);
	strcpy(this->record->get_desc(), tempstr);
	strcpy(&this->record->get_desc()[255], "\0");

	slovoG(this->str, tempstr, 353, 354);
	this->record->flag_active = atoi(tempstr);

	slovoG(this->str, tempstr, 354, 370);
	strcpy(this->record->get_cat_name(), tempstr);
	strcpy(&this->record->get_cat_name()[16], "\0");

	slovoG(this->str, tempstr, 370, 625);
	strcpy(this->record->get_dir_path(), tempstr);
	strcpy(&this->record->get_dir_path()[255], "\0");

	slovoG(this->str, tempstr, 625, 626);
	strcpy(this->record->tail, tempstr);
	strcpy(&this->record->tail[1], "\0");

	delete [] tempstr;
	tempstr = NULL;
}

int TaskList::pushTask(TLRecord *newtask)
{
	return 0;
}
int TaskList::remTask(int noft)
{
	int i = 0;

	while(!this->ReadStr(i))
	{
		this->s2rec(this->str);
		if(noft==this->record->noftask)
		{
			this->DelStr(i);
			return 0;
		}
		i++;
	}

	return 1;
	
}


//////////////////////////////////////////////////////////

LRecord::LRecord()
{
	this->name = new char[32];
	this->exp = 0.0;
	this->noftask = 0;
	this->desc = new char[255];
	this->tail = new char[32];

	this->set_desc("");

	strcpy(tail, "\x0D");
}

LRecord::~LRecord()
{
	delete [] this->name;
	this->name = NULL;
	this->exp = 0.0;
	this->noftask = 0;
	delete [] this->desc;
	delete [] this->tail;
}

void LRecord::copyTo(LRecord *rec)
{
	rec->flag0 = this->flag0;
	rec->exp = this->exp;
	rec->noftask = this->noftask;
	rec->t0 = this->t0;
	rec->t1 = this->t1;

	indexB(rec->name, this->name, " ", 16);
//	strcpy(rec->name, this->name);
	strcpy(rec->get_desc(), this->get_desc());
	strcpy(rec->tail, this->tail);
}

int LRecord::IsEqv(LRecord *rec)
{
	double second = 1.0/86400.0;
	if(streqv(this->name, rec->name)&&(fabs(this->t0-rec->t0)<second)&&(fabs(this->t1-rec->t1))<second) return 1;

	return 0;
}

int LRecord::set_desc(char *desc)
{
	return(indexB(this->desc, desc, " ", 32));
}

int LRecord::set_name(char *nm)
{
	return(indexB(this->name, nm, " ", 16));
}

char* LRecord::get_desc()
{
	return this->desc;
}

IList::IList(int sizestr) : fBuffer(sizestr)
{
	this->record = new LRecord;
//	this->ilBuf = new IListBuffer;
}

IList::IList() : fBuffer(ILIST_LEN)
{
	this->record = new LRecord;
//	this->ilBuf = new IListBuffer;
}

IList::~IList()
{
	delete(this->record);
//	delete(this->ilBuf);
	this->record = NULL;
}

int IList::init(char *fname)
{
	return(fBuffer::init(fname, ILIST_LEN));
}

int IList::AddRec()
{
	char *stmp;
	stmp = new char[SIMPLE_STR];

	this->rec2s(stmp);
	int res = this->AddStr(stmp, this->nstr);

//	if(!res) this->nstr++;

	delete [] stmp;

	return(res);
}

int IList::AddRec(int pos)
{
	char *stmp;
	stmp = new char[SIMPLE_STR];

	this->rec2s(stmp);
	int res = this->AddStr(stmp, pos);

//	if(!res) this->nstr++;

	delete [] stmp;

	return(res);
}

int IList::AddRec(LRecord *nrec)
{
	if(this->GetRec(nrec)!=-1) return -1;
	int rpos = this->GetRec(nrec);

	nrec->copyTo(this->record);

	if(rpos==-1) this->AddRec();
	else this->UpdateRec(rpos);

	return 0;
}

int IList::UpdateRec(int pos)
{
	char *nnstr;
	nnstr = new char[SIMPLE_STR];

	this->rec2s(nnstr);
	this->Update(nnstr, pos);

	return 0;	
}

int IList::UpdateRec(LRecord *rec)
{
	char *nnstr;
	nnstr = new char[SIMPLE_STR];

	int pos = this->GetRec(rec);
	if(pos==-1) return 1;
	rec->copyTo(this->record);

	this->rec2s(nnstr);
	this->Update(nnstr, pos);

	return 0;	
}



int IList::GetRec(int i)
{
	if(this->ReadStr(i)) return 1;

	this->s2rec(this->str);

	return 0;
}

int IList::GetRec(char *nnstr)
{
	int i = 0;

	while(!this->ReadStr(i))
	{
		this->s2rec(this->str);
		if(streqv(this->record->name, nnstr)) return i;
		i++;
	}

	return -1;
}

int IList::GetRec(LRecord *rec)
{
	int i = 0;

	while(!this->ReadStr(i))
	{
		this->s2rec(this->str);
		if(this->record->IsEqv(rec)) return i;
		i++;
	}

	return -1;
}

int IList::DelRec(LRecord *rec)
{
	int rpos = this->GetRec(rec);

	if(rpos>=0) this->DelStr(rpos);

	return -1;
}


int IList::DelRec(char *name)
{
	

	int i = nstr;

	while(!this->ReadStr(i))
	{
		this->s2rec(this->str);
		if(streqv(this->record->name, name)) this->DelStr(i);
		i--;
	}

	return 0;

}

void IList::rec2s(char *nnstr)
{
//	sprintf(nnstr, "%5d%s%10e%14.6f%14.6f%3d%s%s", this->record->noftask, this->record->name, this->record->exp, this->record->t0, this->record->t1, this->record->etype, this->record->desc, this->record->tail);
	sprintf(nnstr, "%16s%10e%14.6f%14.6f%3d%s%s", this->record->name, this->record->exp, this->record->t0, this->record->t1, this->record->flag0, this->record->desc, this->record->tail);
}

void IList::s2rec(char *nnstr)
{
	char *tempstr;
	tempstr = new char[SIMPLE_STR];
/*
	slovoG(this->str, tempstr, 0, 5);
	this->record->noftask = atoi(tempstr);
*/
//	std::string
	slovoG(this->str, tempstr, 0, 16);
	strcpy(this->record->name, tempstr);
	strcpy(&this->record->name[16], "\0");

	slovoG(this->str, tempstr, 16, 29);
	this->record->exp = atof(tempstr);

	slovoG(this->str, tempstr, 29, 43);
	this->record->t0 = atof(tempstr);

	slovoG(this->str, tempstr, 43, 57);
	this->record->t1 = atof(tempstr);

	slovoG(this->str, tempstr, 57, 60);
	this->record->flag0 = atoi(tempstr);

	slovoG(this->str, tempstr, 60, 92);
	strcpy(this->record->desc, tempstr);
//	strcpy(&this->record->name[32], "\0");

	delete [] tempstr;
	tempstr = NULL;
}
/*
int IList::GetRecInc(char *name)
{
	int i = 0;

	do
	{
		this->GetRec(i);
		if(strstr(this->record->name, name)!=NULL) return 0;
		i++;
	}while(i<this->nstr);

	return 1;
}
*/
int IList::sort_prior()
{
	int i, j, bnum;
	double bds, ds;

	for(i=0; i<this->nstr; i++)
	{
		bds = 0.0;
		bnum = i;

		for(j=i; j<this->nstr; j++)
		{
			this->GetRec(j);

			ds = this->record->exp;
			if(ds>bds)
			{
				bds = ds;
				bnum = j;
			}
		}
		this->Exchange(i, bnum);
	}

	return 0;
}


/////////////////////////////////////////////////

int iniRecord::fromString(QString tStr)
{
    QStringList strL;
    strL = tStr.split("|");
    if(strL.size()!=5) return 1;
    name = strL.at(0);
    exp = strL.at(1).toDouble();
    t0 = strL.at(2).toDouble();
    t1 = strL.at(3).toDouble();
    desc = strL.at(4);
}

int iniRecord::toString(QString &tStr)
{
    tStr.clear();
    tStr.append(QString("%1|%2|%3|%4|%5").arg(name, 16).arg(exp, 12, 'e').arg(t0, 14, 'f', 6).arg(t0, 14, 'f', 6).arg(desc));
}

bool operator== ( const iniRecord& lhs, const iniRecord& rhs )
{
    return(QString().compare(lhs.name, rhs.name, Qt::CaseSensitive)==0);
}

iniRecord& iniRecord::operator=(const iniRecord &rhs) {
    // Check for self-assignment!
    if (this == &rhs)      // Same object?
      return *this;        // Yes, so skip assignment, and just return *this.

    //... // Deallocate, allocate new space, copy values...
    this->desc = rhs.desc;
    this->exp = rhs.exp;
    this->name = rhs.name;
    this->t0 = rhs.t0;
    this->t1 = rhs.t1;

    return *this;
}
/*
iniRecord& operator= ( const iniRecord& ust )
{
    iniRecord res;


    return(res);
}
*/
/*
IListR::IListR()
{
//	this->record = new LRecord;
//	this->ilBuf = new IListBuffer;
}

IListR::~IListR()
{
    recList.clear();
//	delete(this->record);
//	delete(this->ilBuf);
//	this->record = NULL;
}

int IListR::init(QString fname)
{


        return 0;
}

int IList::AddRec()
{
        char *stmp;
        stmp = new char[SIMPLE_STR];

        this->rec2s(stmp);
        int res = this->AddStr(stmp, this->nstr);

//	if(!res) this->nstr++;

        delete [] stmp;

        return(res);
}

int IList::AddRec(int pos)
{
        char *stmp;
        stmp = new char[SIMPLE_STR];

        this->rec2s(stmp);
        int res = this->AddStr(stmp, pos);

//	if(!res) this->nstr++;

        delete [] stmp;

        return(res);
}

int IList::AddRec(LRecord *nrec)
{
        if(this->GetRec(nrec)!=-1) return -1;
        int rpos = this->GetRec(nrec);

        nrec->copyTo(this->record);

        if(rpos==-1) this->AddRec();
        else this->UpdateRec(rpos);

        return 0;
}

int IList::UpdateRec(int pos)
{
        char *nnstr;
        nnstr = new char[SIMPLE_STR];

        this->rec2s(nnstr);
        this->Update(nnstr, pos);

        return 0;
}

int IList::UpdateRec(LRecord *rec)
{
        char *nnstr;
        nnstr = new char[SIMPLE_STR];

        int pos = this->GetRec(rec);
        if(pos==-1) return 1;
        rec->copyTo(this->record);

        this->rec2s(nnstr);
        this->Update(nnstr, pos);

        return 0;
}



int IList::GetRec(int i)
{
        if(this->ReadStr(i)) return 1;

        this->s2rec(this->str);

        return 0;
}

int IList::GetRec(char *nnstr)
{
        int i = 0;

        while(!this->ReadStr(i))
        {
                this->s2rec(this->str);
                if(streqv(this->record->name, nnstr)) return i;
                i++;
        }

        return -1;
}

int IList::GetRec(LRecord *rec)
{
        int i = 0;

        while(!this->ReadStr(i))
        {
                this->s2rec(this->str);
                if(this->record->IsEqv(rec)) return i;
                i++;
        }

        return -1;
}

int IList::DelRec(LRecord *rec)
{
        int rpos = this->GetRec(rec);

        if(rpos>=0) this->DelStr(rpos);

        return -1;
}


int IList::DelRec(char *name)
{


        int i = nstr;

        while(!this->ReadStr(i))
        {
                this->s2rec(this->str);
                if(streqv(this->record->name, name)) this->DelStr(i);
                i--;
        }

        return 0;

}
*/

/////////////////////////////////////////////////


/////////////////////////////////////////////////


IListOld::IListOld(int sizestr) : fBuffer(sizestr)
{
	this->record = new LRecord;
//	this->ilBuf = new IListBuffer;
}

IListOld::IListOld() : fBuffer(ILISTOLD_LEN)
{
	this->record = new LRecord;
//	this->ilBuf = new IListBuffer;
}

IListOld::~IListOld()
{
	delete(this->record);
//	delete(this->ilBuf);
	this->record = NULL;
}

int IListOld::init(char *fname)
{
	return(fBuffer::init(fname, ILISTOLD_LEN));
}

int IListOld::AddRec()
{
	char *stmp;
	stmp = new char[SIMPLE_STR];

	this->rec2s(stmp);
	int res = this->AddStr(stmp, this->nstr);

//	if(!res) this->nstr++;

	delete [] stmp;

	return(res);
}

int IListOld::AddRec(LRecord *nrec)
{
	if(this->GetRec(nrec)!=-1) return -1;
	int rpos = this->GetRec(nrec);

	nrec->copyTo(this->record);

	if(rpos==-1) this->AddRec();
	else this->UpdateRec(rpos);

	return 0;
}

int IListOld::UpdateRec(int pos)
{
	char *nnstr;
	nnstr = new char[SIMPLE_STR];

	this->rec2s(nnstr);
	this->Update(nnstr, pos);

	return 0;	
}


int IListOld::GetRec(int i)
{
	if(this->ReadStr(i)) return 1;

	this->s2rec(this->str);

	return 0;
}

int IListOld::GetRec(char *nnstr)
{
	int i = 0;

	while(!this->ReadStr(i))
	{
		this->s2rec(this->str);
		if(streqv(this->record->name, nnstr)) return i;
		i++;
	}

	return -1;
}

int IListOld::GetRec(LRecord *rec)
{
	int i = 0;

	while(!this->ReadStr(i))
	{
		this->s2rec(this->str);
		if(this->record->IsEqv(rec)) return i;
		i++;
	}

	return -1;
}

int IListOld::DelRec(LRecord *rec)
{
	int rpos = this->GetRec(rec);

	if(rpos>=0) this->DelStr(rpos);

	return -1;
}

void IListOld::rec2s(char *nnstr)
{
	sprintf(nnstr, "%5d%s%10e%9.1f%9.1f%3d%s%s", this->record->noftask, this->record->name, this->record->exp, this->record->t0, this->record->t1, this->record->flag0, this->record->desc, this->record->tail);
}

void IListOld::s2rec(char *nnstr)
{
	char *tempstr;
	tempstr = new char[SIMPLE_STR];

	slovoG(this->str, tempstr, 0, 5);
	this->record->noftask = atoi(tempstr);

	slovoG(this->str, tempstr, 5, 21);
	strcpy(this->record->name, tempstr);
	strcpy(&this->record->name[32], "\0");

	slovoG(this->str, tempstr, 21, 34);
	this->record->exp = atof(tempstr);

	slovoG(this->str, tempstr, 34, 43);
	this->record->t0 = atof(tempstr);

	slovoG(this->str, tempstr, 43, 52);
	this->record->t1 = atof(tempstr);

	slovoG(this->str, tempstr, 52, 55);
	this->record->flag0 = atoi(tempstr);

	slovoG(this->str, tempstr, 55, 87);
	strcpy(this->record->desc, tempstr);
//	strcpy(&this->record->name[32], "\0");

	delete [] tempstr;
	tempstr = NULL;
}

int IListOld::sort_prior()
{
	int i, j, bnum;
	double bds, ds;

	for(i=0; i<this->nstr; i++)
	{
		bds = 0.0;
		bnum = i;

		for(j=i; j<this->nstr; j++)
		{
			this->GetRec(j);

			ds = this->record->exp;
			if(ds>bds)
			{
				bds = ds;
				bnum = j;
			}
		}
		this->Exchange(i, bnum);
	}

	return 0;
}

//	RLRecord	//////////////////////////////////

RLRecord::RLRecord()
{
//	this->name = new char[128];
//	this->num = new char[128];
//	this->tail = new char[128];
//	strcpy(tail, "\x0D");
}

RLRecord::~RLRecord()
{
//	delete [] this->name;
//	delete [] this->num;
//	delete [] tail;
}

int RLRecord::fromString(QString str)
{
    QStringList strL;
    strL = str.split("|");

    if(strL.size()!=9) return 1;

    //mJD = getMJDfromYMD(strL.at(0));
    name = strL.at(0);
    hms_to_deg(&ra, strL.at(1), " ");
    damas_to_deg(&dec, strL.at(2), " ");
    magn = strL.at(3).toDouble();
    muRa = strL.at(4).toDouble();
    muDe = strL.at(5).toDouble();
    nofobs = strL.at(6).toInt();

    taskName = strL.at(7);
    exp = strL.at(8).toDouble();

    return 0;
}

int RLRecord::toString(QString &str)
{
    QString raStr, deStr;
    deg_to_hms(&raStr, ra, " ", 2);
    deg_to_damas(&deStr, dec, " ", 2);
    str.clear();
    //str.append(QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10").arg(getStrFromDATEOBS(getDATEOBSfromMJD(mJD), " ", 0, 5)).arg(raStr).arg(deStr).arg(magn).arg(muRa).arg(muDe).arg(nofobs).arg(name).arg(taskName).arg(exp));
    str.append(QString("%1|%2|%3|%4|%5|%6|%7|%8|%9").arg(name).arg(raStr).arg(deStr).arg(magn).arg(muRa).arg(muDe).arg(nofobs).arg(taskName).arg(exp));
}

/*

void RLRecord::get_RA(char *str)
{
	int h, m, sn;
	double s;
	char *csn;
        QString str;
	csn = new char[1];

        deg_to_hms(&str, ra, " ", 1);
	rad2hms(this->RA, &sn, &h, &m, &s);
	if(h<10) sprintf(str, "0%1d %2d %6.3f", h, m, s);
	else sprintf(str, "%2d %2d %6.3f", h, m, s);
	
	delete [] csn;
}

void RLRecord::get_DEC(char *str)
{
	int h, m, sn;
	double s;
	char *csn;
	csn = new char[1];

	rad2gms(this->DEC, &sn, &h, &m, &s);
	if(sn) strcpy(csn, "+");
	else strcpy(csn, "-");
	sprintf(str, "%s%2d %2d %5.2f", csn, h, m, s);

	delete [] csn;
}

void RLRecord::set_name(char* sname)
{
	indexB(this->name, sname, " ", 16);
}

char* RLRecord::get_name()
{
	return(this->name);
}
*/
//	RList	/////////////////////////////////////

RList::RList()// : fBuffer(RLIST_LEN)
{
    resList.clear();
//	this->record = new RLRecord();
//	this->record->name = new char[255];
//	this->record->num = new char[255];
}

//RList::RList(int sizestr) : FileDynStrBin(sizestr)
/*
RList::RList(int sizestr) : fBuffer(sizestr)
{
	this->record = new RLRecord();
//	this->record->name = new char[255];
//	this->record->num = new char[255];
}
*/
RList::~RList()
{
    Clear();
        //delete(this->record);
        //this->record = NULL;
}

int RList::init(QString fname)
{
    fileName = fname;
    QFile iniFile(fileName);
    iniFile.open(QIODevice::ReadOnly);
    QTextStream iniStm(&iniFile);
    QString tStr;
    RLRecord *tRec;
    tRec =  new RLRecord;

    while(!iniStm.atEnd())
    {
        tStr = iniStm.readLine();
        if(tStr.at(0)=='@')
        {
            tStr.remove(0, 1);
            mJD = tStr.toDouble();
            continue;
        }
        if(tRec->fromString(tStr)) continue;
        resList << tRec;
        tRec =  new RLRecord;
    }

    delete tRec;

    iniFile.close();


    return 0;
}

int RList::save()
{
    int i, sz;
    QFile iniFile(fileName);
    iniFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream iniStm(&iniFile);
    QString tStr;

    iniStm << QString("\@%1\n").arg(mJD);

    sz = resList.size();
    for(i=0; i<sz; i++)
    {
        resList.at(i)->toString(tStr);
        iniStm << tStr << "\n";
    }
    return 0;
}

int RList::saveAs(QString fname)
{
    fileName = fname;
    return(save());
}

int RList::GetRec(RLRecord *resRec, int i)
{
    int sz = resList.size();
    if(i<0||i>=sz) return 1;
    resRec = resList.at(i);
    return 0;
}
/*
int RList::GetRecNum(int num)
{
	int i = 0;

	do
	{
		this->GetRec(i);
		if(num==atoi(this->record->num)) return 0;
		i++;
	}while(i<this->nstr);

	return 1;
}
*/
int RList::GetRec(RLRecord *resRec, QString name)
{
	int i = 0;
        int sz = resList.size();
        RLRecord *tRec;

        for(i=0;i<sz;i++)
        {
            tRec = resList.at(i);
            if(QString().compare(tRec->name, name)==0)
            {
                resRec = tRec;
                return 0;
            }
        }
	return 1;
}

int RList::GetRec(RLRecord *resRec, QString name, QString taskName)
{
    int i = 0;
    int sz = resList.size();
    RLRecord *tRec;

    for(i=0;i<sz;i++)
    {
        tRec = resList.at(i);
        if((QString().compare(tRec->name, name)==0)&&(QString().compare(tRec->taskName, taskName)==0))
        {
            resRec = tRec;
            return 0;
        }
    }
    return 1;
}

int RList::AddRec(RLRecord *nrec)
{
    RLRecord *tRec = new RLRecord;
    QString tStr;
    nrec->toString(tStr);
    tRec->fromString(tStr);
    resList.append(tRec);
    return 0;
}

int RList::updateRec(RLRecord *nrec)
{
    RLRecord *tRec;

    if(GetRec(tRec, nrec->name, nrec->taskName)) return 1;

    tRec->dec = nrec->dec;
    tRec->ra = nrec->ra;
    tRec->exp = nrec->exp;
    tRec->magn = nrec->magn;
    tRec->muDe = nrec->muDe;
    tRec->muRa = nrec->muRa;
    tRec->nofobs = nrec->nofobs;
    return 0;
}

/*
int RList::rec2s(char *nnstr)
{
	char *tstr;
	tstr = new char[SIMPLE_STR];

	strcpy(tstr, this->record->num);
	index(this->record->num, tstr, " ", 8);

	strcpy(tstr, this->record->get_name());
	indexB(this->record->get_name(), tstr, " ", 16);
	sprintf(nnstr, "%5d%s%s%+10e%+10e%+10e%+6.2f%5d%s", this->record->noftask, this->record->num, this->record->get_name(), this->record->exp, this->record->RA, this->record->DEC, this->record->magn, this->record->nofobs, this->record->tail);

	delete [] tstr;
	return 0;
}

*/
void RList::Clear()
{
    resList.clear();
}
/*
int RList::s2rec(char *str)
{
	char *tstr;
	tstr = new char[SIMPLE_STR];

	slovoG(str, tstr, 0, 5);
	this->record->noftask = atoi(tstr);

	slovoG(str, tstr, 5, 13);
	strcpy(this->record->num, tstr);
	strcpy(&this->record->num[9], "\0");

	slovoG(str, tstr, 13, 29);
	this->record->set_name(tstr);
//	strcpy(&this->record->get_name()[32], "\0");

	slovoG(str, tstr, 29, 43);
	this->record->exp = atof(tstr);

	slovoG(str, tstr, 43, 57);
	this->record->RA = atof(tstr);

	slovoG(str, tstr, 57, 71);
	this->record->DEC = atof(tstr);

	slovoG(str, tstr, 71, 77);
	this->record->magn = atof(tstr);

	slovoG(str, tstr, 77, 82);
	this->record->nofobs = atoi(tstr);

	slovoG(str, tstr, 82, 83);
	strcpy(this->record->tail, tstr);

	delete [] tstr;
	return 0;
}
*/
int RList::sort_meridian(double s)
{
        int i, j, bnum, sz;
	double bds, ds;
        RLRecord *tRec;

        sz = resList.size();

        for(i=0; i<sz; i++)
	{
		bds = PI;
		bnum = i;

                for(j=i; j<sz; j++)
		{
                        //this->GetRec(j);
                    tRec = resList.at(j);

                        ds = fabs(sin(tRec->ra)-sin(s));

			if(ds<bds)
			{
				bds = ds;
				bnum = j;
			}

		}
                resList.swap(i, bnum);
                //this->Exchange(i, bnum);
	}

	return 0;
}

int RList::sort_magnitude(int direct)
{
        int i, j, bnum, sz;
	double bds, ds;
        RLRecord *tRec;

        sz = resList.size();

        for(i=0; i<sz; i++)
	{
		bds = pow(-1.0, direct+1)*30.0;
		bnum = i;

                for(j=i; j<sz; j++)
		{
                        tRec = resList.at(j);

                        ds = tRec->magn;
			if((pow(-1.0, direct)*ds)>(pow(-1.0, direct)*bds))
			{
				bds = ds;
				bnum = j;
			}

		}
                resList.swap(i, bnum);
                //this->Exchange(i, bnum);
	}

	return 0;
}

int RList::sort_obs()
{
        int i, j, bnum, sz;
	int bds, ds;
        RLRecord *tRec;

        sz = resList.size();

        for(i=0; i<sz; i++)
	{
		bds = 0;
		bnum = i;

                for(j=i; j<sz; j++)
		{
                        tRec = resList.at(j);

                        ds = tRec->nofobs;
			if(ds>bds)
			{
				bds = ds;
				bnum = j;
			}

		}
                resList.swap(i, bnum);
                //this->Exchange(i, bnum);
	}

	return 0;
}

int RList::sort_RA_min2max(double RAmin)
{
        int i, j, bnum, sz;
	double bds, ds, tvalue;
        RLRecord *tRec;

        sz = resList.size();

        for(i=0; i<sz; i++)
	{
		bds = 2.0*PI;
		bnum = i;

                for(j=i; j<sz; j++)
		{
                        //this->GetRec(j);
                        tRec = resList.at(j);

                        tvalue = tRec->ra;
			if(tvalue<RAmin) tvalue+=2.0*PI;
			ds = tvalue-RAmin;

			if(ds<bds)
			{
				bds = ds;
				bnum = j;
			}

		}
                resList.swap(i, bnum);
                //this->Exchange(i, bnum);
	}

	return 0;
}

int RList::sort_prior()
{
        int i, j, bnum, sz;
	double bds, ds;
        RLRecord *tRec;
        sz = resList.size();

        for(i=0; i<sz; i++)
	{
		bds = 0.0;
		bnum = i;

                for(j=i; j<sz; j++)
		{
                        //this->GetRec(j);
                    tRec = resList.at(j);

                        ds = tRec->exp;
			if(ds>bds)
			{
				bds = ds;
				bnum = j;
			}
		}
                resList.swap(i, bnum);
                //this->Exchange(i, bnum);
	}

	return 0;
}
/*
int RList::sort_num()
{
	int i, j, bnum;
	int bds, ds;

	for(i=0; i<this->nstr; i++)
	{
		bds = 1000000000.0;
		bnum = i;

		for(j=i; j<this->nstr; j++)
		{
			this->GetRec(j);

			ds = atoi(this->record->num);
			if(ds<bds)
			{
				bds = ds;
				bnum = j;
			}
		}
		this->Exchange(i, bnum);
	}

	return 0;
}
*/
//////////////	ERecord	///////////////////

ERecord::ERecord()
{
	this->name = new char[16];
	this->desc = new char[32];
	this->tail = new char[32];
	strcpy(tail, "\x0D");
}

ERecord::~ERecord()
{
	delete [] this->name;
	this->name = NULL;
	delete [] this->desc;
	delete [] this->tail;
}

void ERecord::copyTo(ERecord *recp)
{
	strcpy(recp->name, this->name);
	strcpy(recp->desc, this->desc);
	recp->noftask = this->noftask;
	recp->Tun = this->Tun;
}

int ERecord::set_desc(char *desc)
{
	return(indexB(this->desc, desc, " ", 32));

}

int ERecord::set_name(char *name)
{
	return(indexB(this->name, name, " ", 16));
}

char* ERecord::get_desc()
{
	return this->desc;
}

char* ERecord::get_name()
{
	return this->name;
}

////////////// EList /////////////////////

EList::EList() : fBuffer(ELIST_LEN)
{
	this->record = new ERecord;
	this->ts = 20.0;
}

EList::EList(int sizestr) : fBuffer(sizestr)
{
	this->record = new ERecord;
	this->ts = 20.0;
//	this->record->name = new char[32];
}

EList::~EList()
{
	delete [] this->record->name;
	this->record->name = NULL;

	delete(this->record);
	this->record = NULL;
}

int EList::init(char *fname)
{
	return(fBuffer::init(fname, ELIST_LEN));
}

int EList::AddExc(ERecord *er)
{
	if(this->IsExc(er->noftask, er->name)) return 0;
	char *nnstr;
	nnstr = new char[SIMPLE_STR];

	er->copyTo(this->record);

	this->rec2s(nnstr);

	int res = this->AddStr(nnstr, 0);
	Save();

	delete [] nnstr;

	return res;
}

int EList::AddExc()
{
//	if(this->IsExc(this->record->noftask, this->record->name)) return 0;
	int res;
	char *tstr; 
	tstr = new char[SIMPLE_STR];

	this->rec2s(tstr);

	res = this->AddStr(tstr, 0);
	
	delete [] tstr;
	return res;
}

int EList::AddExc(int noft, char *ename, double cTimeJD)
{
	if(this->IsExc(noft, ename)) return 0;

	this->record->noftask = noft;
	strcpy(this->record->name, ename);
	this->record->Tun = cTimeJD;

	if(this->AddExc()) return 1;
	return 0;
}

int EList::RemExc(int noft, char *ename)
{
	int i = 0;

	while(i<this->nstr)
	{
		this->ReadStr(i);
		this->s2rec(this->str);

		if((streqv(ename, this->record->name))&&(noft==this->record->noftask))
		{
			this->DelStr(i);
			return 0;
		}

		i++;
	}

	return 1;
}

int EList::GetExc(int pos)
{
	this->ReadStr(pos);
	this->s2rec(this->str);
	return 0;
}

int EList::RemExc(char *ename)
{
	int i = 0;

	for(i=this->nstr-1; i>=0; i--)
	{
		this->GetExc(i);

		if(streqv(ename, this->record->name)) this->DelStr(i);
	}

	return 0;
}

int EList::IsExc(char *ename)
{
	int i = 0;

	while(i<this->nstr)
	{
		this->GetExc(i);

		if(streqv(ename, this->record->name)) return 1;

		i++;
	}

	return 0;
}

int EList::IsExc(int noft, char *ename)
{
	int i = 0;

	while(i<this->nstr)
	{
		this->GetExc(i);

		if((streqv(ename, this->record->name))&&(noft==this->record->noftask)) return 1;

		i++;
	}

	return 0;
}

int EList::UpdateExc(double cTime)
{
	int i;
	for(i=this->nstr-1; i>=0; i--)
	{
		this->GetExc(i);

		if((this->record->Tun<cTime)&&(this->record->Tun!=0.0)) this->DelStr(i);
	}

	return 0;
}

int EList::Clear()
{
	this->Free();

	return 0;
}

int EList::s2rec(char *str)
{
	char *tempstr;
	tempstr = new char[SIMPLE_STR];

	slovoG(str, tempstr, 0, 5);
	this->record->noftask = atoi(tempstr);

	slovoG(str, tempstr, 5, 21);
	strcpy(this->record->name, tempstr);
	strcpy(&this->record->name[16], "\0");

	slovoG(str, tempstr, 21, 34);
	this->record->Tun = atof(tempstr);

	slovoG(str, tempstr, 34, 66);
	this->record->set_desc(tempstr);

	slovoG(str, tempstr, 66, 67);
	strcpy(this->record->tail, tempstr);

	delete [] tempstr;
	return 0;
}

int EList::rec2s(char *tstr)
{
	sprintf(tstr, "%5d%s%13.5f%s%s", this->record->noftask, this->record->name, this->record->Tun, this->record->get_desc(), this->record->tail);
	return 0;
}

////////////////////////////////////////////

LogRecord::LogRecord()
{
	this->name = new char[16];
	this->RA = new char[12];
	this->DEC = new char[12];
}

LogRecord::~LogRecord()
{
	delete [] this->name;
	this->name = NULL;

	delete [] this->RA;
	this->RA = NULL;

	delete [] this->DEC;
	this->DEC = NULL;
}


////////////// LogList /////////////////////
LogList::LogList() : FileDynStrBin(LLIST_LEN)
{
	this->record = new LogRecord;
/*	this->record->name = new char[16];
	this->record->RA = new char[12];
	this->record->DEC = new char[12];*/
}

LogList::~LogList()
{
	delete(this->record);
	this->record = NULL;
}

int LogList::init(char *fname)
{
        if(FileDynStrBin::init(fname)) return 1;
	return 0;
}

int LogList::AddObs(LogRecord *er)
{
	char *nnstr;
	nnstr = new char[SIMPLE_STR];

	this->rec2s(er);
	strcpy(nnstr, this->str);
	if(this->AddStr(nnstr, 0)) return 1;

	delete [] nnstr;
	return 0;
}

int LogList::GetRec(int i)
{
	if(this->ReadStr(i)) return 1;
	this->s2rec(this->str);

	return 0;
}

int LogList::GetObs(char *ename)
{
	int i = 0;
	int nobs = 0;

        while(i<nstr())
	{
		this->ReadStr(i);
		this->s2rec(this->str);

		if(streqv(ename, this->record->name))
		{
			nobs += this->record->success;
		}

		i++;
	}

	return nobs;
}

int LogList::Clear()
{
	this->Free();

	return 0;
}

int LogList::s2rec(char *str)
{
	char *tempstr;
	tempstr = new char[SIMPLE_STR];

	slovoG(str, tempstr, 0, 4);
	this->record->year0 = atoi(tempstr);
	slovoG(str, tempstr, 5, 7);
	this->record->month0 = atoi(tempstr);
	slovoG(str, tempstr, 8, 10);
	this->record->day0 = atoi(tempstr);
	slovoG(str, tempstr, 11, 13);
	this->record->hour0 = atoi(tempstr);
	slovoG(str, tempstr, 14, 16);
	this->record->min0 = atoi(tempstr);
	slovoG(str, tempstr, 17, 19);
	this->record->sec0 = atoi(tempstr);

	slovoG(str, tempstr, 20, 24);
	this->record->year1 = atoi(tempstr);
	slovoG(str, tempstr, 25, 27);
	this->record->month1 = atoi(tempstr);
	slovoG(str, tempstr, 28, 30);
	this->record->day1 = atoi(tempstr);
	slovoG(str, tempstr, 31, 33);
	this->record->hour1 = atoi(tempstr);
	slovoG(str, tempstr, 34, 36);
	this->record->min1 = atoi(tempstr);
	slovoG(str, tempstr, 37, 39);
	this->record->sec1 = atoi(tempstr);
	
	slovoG(str, tempstr, 40, 45);
	this->record->noftask = atoi(tempstr);
		
	slovoG(str, tempstr, 46, 62);
	strcpy(this->record->name, tempstr);
	strcpy(&this->record->name[32], "\0");

	slovoG(str, tempstr, 63, 64);
	this->record->success = atoi(tempstr);

	slovoG(str, tempstr, 65, 77);
	strncpy(this->record->RA, tempstr, 12);

	slovoG(str, tempstr, 78, 90);
	strncpy(this->record->DEC, tempstr, 12);

	slovoG(str, tempstr, 91, 98);
	this->record->magn = atof(tempstr);

	delete [] tempstr;
	return 0;
}

int LogList::rec2s(LogRecord *er)
{
	sprintf(this->str, "%4d:%2d:%2d:%2d:%2d:%2d|%4d:%2d:%2d:%2d:%2d:%2d|%5d|%s|%1d|%12s|%12s|%+6.2f", er->year0, er->month0, er->day0, er->hour0, er->min0, er->sec0, er->year1, er->month1, er->day1, er->hour1, er->min1, er->sec1, er->noftask, er->name, er->success, er->RA, er->DEC, er->magn);

	return 0;
}

////////////////////////////////////////////
////////////// ObsList /////////////////////
ObsList::ObsList() : fBuffer(OLIST_LEN)
{
	this->record = new ObsRecord;
	this->record->name = new char[16];
}

ObsList::~ObsList()
{
	delete [] this->record->name;
	this->record->name = NULL;

	delete(this->record);
	this->record = NULL;
}

int ObsList::AddObs(ObsRecord *er)
{
	char *nnstr;
	nnstr = new char[255];

	this->rec2s(er);
	strcpy(nnstr, this->str);
	if(this->AddStr(nnstr, 0)) return 1;

	delete [] nnstr;
	return 0;
}

int ObsList::RemObs(char *ename)
{
	int i = 0;

	while(i<this->nstr-1)
	{
		this->ReadStr(i);
		this->s2rec(this->str);

		if(streqv(ename, this->record->name))
		{
			this->DelStr(i);
			return 0;
		}

		i++;
	}

	return 1;
}

int ObsList::GetObs(int pos)
{
	if(this->ReadStr(pos)) return 1;
	this->s2rec(this->str);

	return 0;
}

int ObsList::GetObs(char *ename)
{
	int i = 0;

	while(i<this->nstr)
	{
		this->GetObs(i);

		if(streqv(ename, this->record->name))
		{
			return 0;
		}

		i++;
	}

	return 1;

}

int ObsList::GetNObs(char *ename)
{
	int i = 0;
	char *tname = new char[16];
	indexB(tname, ename, " ", 16);

	while(i<this->nstr)
	{
		this->ReadStr(i);
		this->s2rec(this->str);

		if(streqv(tname, this->record->name))
		{
			return this->record->nobs;
		}

		i++;
	}

	return 0;

}

int ObsList::ChObs(LogRecord *nobserv, int num)
{
//	if(num<1) return 0;
	int i = 0;
	int isk = -1;

	char *chstr;
	chstr = new char[SIMPLE_STR];

	this->record->nobs = 0;
	strcpy(this->record->name, nobserv->name);
	this->record->noftask = nobserv->noftask;

	while(i<this->nstr)
	{
		this->ReadStr(i);
		this->s2rec(this->str);

		if(streqv(nobserv->name, this->record->name))
		{
			isk = i;
			break;
		}

		i++;
	}
	
	if(isk==-1)
	{
		this->record->nobs = 0;
		this->record->noftask = nobserv->noftask;
		strcpy(this->record->name, nobserv->name);
	}
	this->record->nobs += num;
	this->rec2s(this->record);
	strcpy(chstr, this->str);
	if(isk>=0) this->Update(chstr, isk);
	else this->AddObs(this->record);

	delete [] chstr;

	return 0;
}

int ObsList::ChObs(int ntask, char *name, int num)
{
//	if(num<1) return 0;
	int i = 0;
	int isk = -1;

	char *chstr;
	chstr = new char[SIMPLE_STR];

	this->record->nobs = 0;
	strcpy(this->record->name, name);
	this->record->noftask = ntask;

	while(i<this->nstr)
	{
		this->ReadStr(i);
		this->s2rec(this->str);

		if(streqv(name, this->record->name))
		{
			isk = i;
			break;
		}

		i++;
	}
	
	if(isk==-1)
	{
		this->record->nobs = 0;
		this->record->noftask = ntask;
		strcpy(this->record->name, name);
	}
	this->record->nobs += num;
	this->rec2s(this->record);
	strcpy(chstr, this->str);

	if(isk>=0)
	{
		if(this->record->nobs>0) this->Update(chstr, isk);
		else this->DelStr(isk);
	}
	else if(this->record->nobs>0) this->AddObs(this->record);

	delete [] chstr;

	return 0;
}

int ObsList::Clear()
{
	this->Free();

	return 0;
}

int ObsList::s2rec(char *str)
{
	char *tstr;
	tstr = new char[SIMPLE_STR];

	slovoG(str, tstr, 0, 5);
	this->record->noftask = atoi(tstr);
	slovoG(str, tstr, 6, 22);
	strcpy(this->record->name, tstr);
	slovoG(str, tstr, 23, 30);
	this->record->nobs = atoi(tstr);
	
	delete [] tstr;
	return 0;
}

int ObsList::rec2s(ObsRecord *er)
{
	sprintf(this->str, "%5d|%s|%6d", er->noftask, er->name, er->nobs);
	return 0;
}

////////////////////////////////////////////
ObsProgram::ObsProgram()
{
	this->opParam = new ObsProgramParam;
}

ObsProgram::~ObsProgram()
{
	delete(this->opParam);
}



////////////////////////////////////////////

CatRecord::CatRecord()
{
	this->cat_type= -1;
	this->name = new char[16];
	this->path = new char[255];
}

CatRecord::~CatRecord()
{
	delete [] this->name;
	this->name = NULL;

	delete [] this->path;
	this->path = NULL;
}

void CatRecord::Set(int cat_type, char *name, char *path)
{
	this->cat_type = cat_type;
	indexB(this->name, name, " ", 16);
	indexB(this->path, path, " ", 255);
}

void CatRecord::get_dir_path(char *dp)
{
	int pose;
	slovo(this->path, dp, 0, &pose);
}


////////////// CatList /////////////////////
CatList::CatList() : FileDynStrBin(CATLIST_LEN)
{
	this->record = new CatRecord;
/*	this->record->name = new char[16];
	this->record->RA = new char[12];
	this->record->DEC = new char[12];*/
}

CatList::~CatList()
{
	delete(this->record);
	this->record = NULL;
}

int CatList::init(char *fname)
{
        return(FileDynStrBin::init(fname));
}

int CatList::Add(CatRecord *er)
{
	char *nnstr;
	nnstr = new char[SIMPLE_STR];

	if(!this->GetByName(er->name)==-1) return 1;

	this->rec2s(er);
	strcpy(nnstr, this->str);
	if(this->AddStr(nnstr, 0)) return 1;

	delete [] nnstr;
	return 0;
}

int CatList::Get(int i)
{
	if(this->ReadStr(i)) return 1;
	this->s2rec(this->str);
	return 0;
}

int CatList::Rem(char *cname)
{
	int rempos = this->GetByName(cname);

	if(!this->DelStr(rempos)) return 1;

	return 0;
}

int CatList::GetByName(char *ename)
{
	int i = 0;
	int nobs = 0;

        while(i<nstr())
	{
		this->Get(i);

		if(streqv(ename, this->record->name)) return i;

		i++;
	}

	return -1;
}

int CatList::GetCatType(char *cat_name)
{
	if(this->GetByName(cat_name)==-1) return -1;
	return(this->record->cat_type);

//	return -1;
}


/*
int CatList::GetByNum(int num)
{
	int i = 0;
	int nobs = 0;
/*
	while(i<this->nstr)
	{
		this->Get(i);

		if(num == this->record->num) return i;

		i++;
	}/

	return -1;
}
*/ 
int CatList::Clear()
{
	this->Free();

	return 0;
}

int CatList::s2rec(char *str)
{
	char *tempstr;
	tempstr = new char[SIMPLE_STR];

	slovoG(str, tempstr, 0, 5);
	this->record->cat_type = atoi(tempstr);
	
	slovoG(str, tempstr, 6, 22);
	strcpy(this->record->name, tempstr);
	strcpy(&this->record->name[16], "\0");

	slovoG(str, tempstr, 23, 276);
	strcpy(this->record->path, tempstr);
	strcpy(&this->record->path[255], "\0");

	delete [] tempstr;
	return 0;
}

int CatList::rec2s(CatRecord *er)
{
	sprintf(this->str, "%5d|%s|%s", er->cat_type, er->name, er->path);

	return 0;
}



////////////////////////////////////////////

EA_obj::EA_obj()
{
	this->ini_list = new IList;
	this->orb_catalog = new OrbCat;
}

EA_obj::~EA_obj()
{
	delete(this->ini_list);
	delete(this->orb_catalog);
}

int EA_obj::init(char *fn_ilist, char *fn_ocat)
{
	int res_ilist = this->ini_list->init(fn_ilist);
	int res_ocat = this->orb_catalog->init(fn_ocat);

	return(res_ilist + res_ocat);
}

int EA_obj::Add(IList *new_il, char* fn_ccat)
{
	int i;
	OrbCat *tcat;
	tcat = new OrbCat;
	int res = 0;

	for(i=0; i<new_il->nstr; i++)
	{
		new_il->GetRec(i);

		if(this->ini_list->GetRec(new_il->record)!=-1) continue;
		
		if(this->orb_catalog->GetRecName(new_il->record->name)==-1)
		{
			tcat->init(fn_ccat);
			if(tcat->GetRecName(new_il->record->name)==-1)
			{
				res = 1;
				continue;
			}
			tcat->record->copyTo(this->orb_catalog->record);
			this->orb_catalog->AddRec(this->orb_catalog->nstr);
		}
		
		if(this->ini_list->AddRec(new_il->record)==-1) continue;
		
		
	}

	return res;
}

int EA_obj::RemIL(int pos)
{
	this->ini_list->GetRec(pos);
	int rpos = this->orb_catalog->GetRecName(this->ini_list->record->name);
	if(rpos==-1) return 1;
	this->orb_catalog->DelStr(rpos);

	this->ini_list->DelStr(pos);

	return 0;
}

int EA_obj::RemIL(LRecord *irecord)
{
	this->ini_list->DelRec(irecord);

	int rpos;
	if(this->ini_list->GetRec(irecord->name)==-1)
	{
		rpos = this->orb_catalog->GetRecName(irecord->name);
		this->orb_catalog->DelStr(rpos);
	}

	return 0;
}

int EA_obj::GetRec(LRecord *irecord)
{
	if(this->ini_list->GetRec(irecord)==-1) return -1;

	if(this->orb_catalog->GetRecName(irecord->name)==-1) return -1;

	return 0;
}

int EA_obj::GetRec(int pos)
{
	if(this->ini_list->GetRec(pos)==-1) return -1;
	if(this->ini_list->record->flag0!=1) return 0;
	if(this->orb_catalog->GetRecName(this->ini_list->record->name)==-1) return -1;

	return 0;
}


int EA_obj::RemAllILName(char *name)
{
	int rpos;
	do
	{
		rpos = this->ini_list->GetRec(name);
		if(rpos>=0) this->ini_list->DelStr(rpos);
		else break;
	}while(1);
	rpos = this->orb_catalog->GetRecName(name);
	if(rpos!=-1) this->orb_catalog->DelStr(rpos);

	return 0;
}

int EA_obj::RemAllILTask(int noftask)
{
	int rpos;
	for(int i=this->ini_list->nstr; i>=0; i--)
	{
		this->ini_list->GetRec(i);
		if(this->ini_list->record->noftask==noftask)
		{
			rpos = this->orb_catalog->GetRecName(this->ini_list->record->name);
			this->orb_catalog->DelStr(rpos);
			this->ini_list->DelStr(i);
		}
	}

	return 0;
}

int EA_obj::RefreshOC(OrbCat *oc)
{
	int i;

	for(i=this->orb_catalog->nstr; i>=0; i--)
	{
		this->orb_catalog->GetRec(i);

		if(this->ini_list->GetRec(this->orb_catalog->record->name)==-1)
		{
			this->orb_catalog->DelStr(i);
		}
	}

	for(i=0; i<this->ini_list->nstr; i++)
	{
		this->ini_list->GetRec(i);
		if(this->ini_list->record->flag0!=1) continue;
		if(this->orb_catalog->GetRecName(this->ini_list->record->name)==-1)
		{
			if(oc->GetRecName(this->ini_list->record->name)==-1) continue;
			oc->record->copyTo(this->orb_catalog->record);
			this->orb_catalog->AddRec();
		}
	}

	return 0;
}

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

////////////////////////////////////////////

SkyArea::SkyArea(char *dele_header)
{
	this->orb_catalog = new OrbCat();
//	this->lspm_catalog = new sscatFB();
//	this->orb_ini_catalog = new OrbCat();
	this->orb_elem = new orbit();
	this->obs_pos = new observ(dele_header);

//	this = new EA_obj();
	this->ini_list = new IList(ILIST_LEN);
        this->res_list = new RList();
	this->exc_list = new EList(ELIST_LEN);
	this->log_list = new LogList();
	this->log_list_oneday = new LogList();
	this->obs_list = new ObsList();
	this->task_list = new TaskList;
	this->cat_list = new CatList;
	this->params = new saParams;
	err_log = new char[255];
	err_level = 0;

//	fn_cat = new char[255];
	install_dir = new char[255];
	strcpy(install_dir, "");
	catalog_dir = new char[255];
	strcpy(catalog_dir, "");

	this->params->is_log_oneday = 0;

}

SkyArea::SkyArea()
{
//	this->orb_catalog = new OrbCat();
	this->mpc_catalog = new mpccat();
//	this->lspm_catalog = new sscatFB();
//	this->orb_ini_catalog = new OrbCat();
	this->orb_elem = new orbit();
	this->obs_pos = new observ();

//	this = new EA_obj();
	this->ini_list = new IList(ILIST_LEN);
        this->res_list = new RList();
	this->exc_list = new EList(ELIST_LEN);
	this->log_list = new LogList();
	this->log_list_oneday = new LogList();
	this->obs_list = new ObsList();
	this->task_list = new TaskList;
	this->cat_list = new CatList;
	this->params = new saParams;
	err_log = new char[256];
	err_level = 0;
	obs_list->is_buff = 1;

//	fn_cat = new char[256];
	install_dir = new char[256];
	strcpy(install_dir, "");
	catalog_dir = new char[256];
	strcpy(catalog_dir, "");

	this->params->is_log_oneday = 0;
	this->params->ignore_task_rules = 0;

}

SkyArea::~SkyArea()
{
//        obs_list->Save();
        //if(orb_catalog!=NULL) delete(this->orb_catalog);
        //orb_catalog = NULL;
//	delete(this->orb_ini_catalog);
//	delete(this->ini_list);
	delete(this->task_list);
	delete(this->res_list);
	delete(this->exc_list);
	delete(this->obs_list);
	delete(this->obs_pos);
//	delete(this);

        delete [] err_log;
//	delete [] this->fn_cat;
	delete [] this->install_dir;
        delete [] this->catalog_dir;
}

int SkyArea::init(char *fn_obs, char *fn_dele_bin)
{
	char *tempstr;
	tempstr = new char[1024];
        if(this->obs_pos->init(fn_obs, fn_dele_bin)) return 2;

//	this->lspm_catalog->is_buff = 1;
//	this->orb_catalog->is_buff = 1;
//res
	sprintf(tempstr, "%sres.lst\0", install_dir);
        this->res_list->init(tempstr);
//	res_list->is_buff = 1;
//exc
	sprintf(tempstr, "%sexc.lst\0", install_dir);
	this->exc_list->init(tempstr);
//log
	sprintf(tempstr, "%slog.lst\0", install_dir);
	this->log_list->init(tempstr);
//obs
	sprintf(tempstr, "%sobs.lst\0", install_dir);
	this->obs_list->init(tempstr, OLIST_LEN);
//tesk
	sprintf(tempstr, "%stask.lst", install_dir);
	this->task_list->init(tempstr);
//cat
	sprintf(tempstr, "%scat.lst", install_dir);
	this->cat_list->init(tempstr);

	this->obs_pos->set_obs_parpam(GEOCENTR_NUM, CENTER_BARY, SK_EKVATOR, "084");

	this->UpdateObsList();

	delete [] tempstr;

	return 0;
}

int SkyArea::prep_wcat()
{

/*
	if(this->orb_catalog->init("wcat.elm")) return 2;
	int i = 0;

	this->RefreshOC(this->orb_ini_catalog);
*/
/*
	int etypenum = 0;
	

	while(!this->ini_list->GetRec(i))
	{
		etypenum += (int)(this->ini_list->record->etype==1);

		i++;
	}


	if(etypenum!=this->orb_catalog->nstr)
	{
		while(!this->ini_list->GetRec(i))
		{
			if(this->ini_list->record->etype==1)
			{
				if(this->orb_catalog->GetRecName(ini_list->record->name))
				{
					this->orb_ini_catalog->GetRecName(this->ini_list->record->name);

					this->orb_ini_catalog->record->copyTo(this->orb_catalog->record);
					this->orb_catalog->AddRec(this->orb_catalog->nstr);
				}
			}
			i++;

		}
	}
/*
	if(this->orb_catalog->nstr!=this->ini_list->nstr)
	{
		if(this->orb_ini_catalog->init(fn_cat, EPOS5_CAT)) return 1;

		while(!this->ini_list->GetRec(i))
		{
			if(this->orb_catalog->GetRecName(ini_list->record->name))
			{
				this->orb_ini_catalog->GetRecName(this->ini_list->record->name);

				this->orb_ini_catalog->record->copyTo(this->orb_catalog->record);
				this->orb_catalog->AddRec(this->orb_catalog->nstr);
			}

			i++;
		}

		i=0;

		
	}
/
	int suc, k;

	while(!this->orb_catalog->GetRec(i)&&(this->orb_catalog->nstr!=this->ini_list->nstr))
	{
		suc = 0;
		k = 0;

		while(!suc&&!this->ini_list->GetRec(k))
		{
			if(streqv(this->orb_catalog->record->name, this->ini_list->record->name))
			{
				suc = 1;
				break;
			}

			k++;
		}

		if(suc==0) this->orb_catalog->DelStr(i);

		i++;
	}

*/
	return 0;
}

int SkyArea::init_log_one_day(char *fn_lod_list)
{
	this->params->is_log_oneday = !this->log_list_oneday->init(fn_lod_list);

	return 0;
}

int SkyArea::Grade()
{
	this->res_list->Clear();
	this->exc_list->UpdateExc(this->obs_pos->otime);
	int i, k;
	double x, y, z;
        double vx, vy, vz;

	if(this->params->isAutoRA) this->SetAutoRA();

        res_list->mJD = jd2mjd(this->timeCur);

	double Sdist, Edist;

        int ctask = this->task_list->nstr();

	IList temp_ini;
        RLRecord *resRec;

	char *str_ini, *str_cat;
	str_ini = new char[255];
	str_cat = new char[255];

	int pose;
	eposrec *erec = new eposrec;
	int lspm_num;

	sscatFB *tsscat;
        QString nameStr;
//	sscat *tsscat = new sscat;

	for(i=0; i<ctask; i++)
	{
		if(this->task_list->GetRec(i)) break;
		if(this->task_list->record->flag_active==0) continue;

		this->GetTaskIniFNames(str_ini, str_cat);
		this->ini_list->init(str_ini);

		this->cat_list->GetByName(this->task_list->record->get_cat_name());

		switch(this->cat_list->record->cat_type)
		{
		case 1:
			this->orb_catalog = new OrbCat();
			this->orb_catalog->is_buff = 1;
			this->orb_catalog->init(str_cat);
			break;
		case 2:
			tsscat = new sscatFB;
//			this->lspm_catalog->init(str_cat);
			tsscat->init(str_cat);
			break;
		case 3:
			
                        /*this->orb_catalog = new OrbCat();
			this->orb_catalog->is_buff = 1;
			this->orb_catalog->init(str_cat);
                        this->orb_catalog->is_buff = 1;*/
                        this->mpc_catalog = new mpccat();
                        this->mpc_catalog->init(str_cat);
			break;
		}

		this->SetTaskAutoRA();
		

		for(k=0; k<ini_list->nstr; k++)
		{
			ini_list->GetRec(k);
                        resRec = new RLRecord;

			if(!ini_list->record->flag0) continue;
			if(exc_list->IsExc(task_list->record->noftask, ini_list->record->name)) continue;


                        resRec->nofobs = this->obs_list->GetNObs(this->ini_list->record->name);

			switch(this->cat_list->record->cat_type)
			{
			case 0:
				if(this->obs_pos->place->detR(&x, &y, &z, this->obs_pos->otime, this->ini_list->record->name, 0, CENTER_SUN, SK_EKVATOR))
				{
	//				AfxMessageBox("c0");
					continue;
				}

                                if(this->obs_pos->place->detR(&vx, &vy, &vz, this->obs_pos->otime, this->ini_list->record->name, 1, CENTER_SUN, SK_EKVATOR))
                                {
        //				AfxMessageBox("c0");
                                        continue;
                                }

                                resRec->name = QString(this->ini_list->record->name);
                                //sprintf(this->res_list->record->num, "%8d", planet_num(this->ini_list->record->name));
                                resRec->num = planet_num(this->ini_list->record->name);

                                det_res_list(resRec, x, y, z, vx, vy, vz, &Sdist, &Edist, this->cat_list->record->cat_type);

				break;
			case 1:
				
				if(this->orb_catalog->GetRecName(this->ini_list->record->name)==-1)
				{
	//				AfxMessageBox("c1");
					continue;
				}

				this->orb_elem->get(this->orb_catalog);
				this->orb_elem->detRecEkv(&x, &y, &z, this->obs_pos->otime);
                                resRec->name = QString(this->orb_catalog->record->name);
                                //sprintf(resRec->num, "%8d", this->orb_catalog->record->number);
                                resRec->num = this->orb_catalog->record->number;

                                det_res_list(resRec, x, y, z, 0, 0, 0, &Sdist, &Edist, this->cat_list->record->cat_type);

				break;
			case 2:
				
				if(tsscat->GetName(this->ini_list->record->name)) continue;
//				if(tsscat->GetRecNameLSPM(this->ini_list->record->name)) continue;

                                resRec->ra = grad2rad(tsscat->record->RAdeg);
                                resRec->dec = grad2rad(tsscat->record->DEdeg);
                                resRec->magn = tsscat->record->BJmag;

                                resRec->name = QString(this->ini_list->record->name);
                                //sprintf(this->res_list->record->num, "        ");
                                resRec->num = 0;
				
				break;
			case 3:
                            nameStr = QString(this->ini_list->record->name).simplified();
                            if(this->mpc_catalog->GetRecName(nameStr.toAscii().data()))
				{
	//				AfxMessageBox("c1");
					continue;
				}

                                this->orb_elem->get(this->mpc_catalog);
				this->orb_elem->detRecEkv(&x, &y, &z, this->obs_pos->otime);
                                this->orb_elem->detRecEkvVel(&vx, &vy, &vz, this->obs_pos->otime);

                                resRec->name = QString(this->mpc_catalog->record->name);

                                qDebug() << QString("%1|%2|%3|%4|%5|%6|%7\n").arg(resRec->name).arg(x).arg(y).arg(z).arg(vx).arg(vy).arg(vz);
                                //sprintf(this->res_list->record->num, "%8d", this->orb_catalog->record->number);
                                resRec->num = atoi(this->mpc_catalog->record->number);
                                det_res_list(resRec, x, y, z, vx, vy, vz, &Sdist, &Edist, this->cat_list->record->cat_type);

				/*if(this->mpc_catalog->GetRecName(this->ini_list->record->name)==-1)
				{
					continue;
				}
				
				this->orb_elem->get(this->mpc_catalog);
				this->orb_elem->detRecEkv(&x, &y, &z, this->obs_pos->otime);
				this->res_list->record->set_name(this->mpc_catalog->record->name);
				sprintf(this->res_list->record->num, "%s", this->orb_catalog->record->number);

				det_res_list(x, y, z, &Sdist, &Edist, 3);*/
				
				break;
                        }



                        resRec->taskName = QString(this->task_list->record->get_name());

                        resRec->exp = this->DetExp(resRec, this->params->ignore_task_rules);

//			this->res_list->record->exp = 1.0;

                        if(this->is_skyarea(resRec))
			{
//				this->res_list->record->nofobs = this->obs_list->GetNObs(this->res_list->record->get_name());
                                this->res_list->AddRec(resRec);
			}
		}


		switch(this->cat_list->record->cat_type)
		{
		case 1:
			delete this->orb_catalog;
			break;
		case 2:
			if(tsscat!=NULL)delete tsscat;
			break;
		case 3:
                        delete this->mpc_catalog;
			break;
		}
		
	}

//	this->CutMaxObj();
//	delete tsscat;
	delete erec;

	delete [] str_ini;
	delete [] str_cat;

	return 0;
}

int SkyArea::GetTaskIniFNames(char *ini_lst, char *ini_cat)
{
	char *dp = new char[255];
	task_list->record->get_dir_path(dp);

        if(ini_lst!=NULL) sprintf(ini_lst, "%s%s/ini.lst", this->install_dir, dp);
        if(ini_cat!=NULL) sprintf(ini_cat, "%s%s/ini.cat", this->install_dir, dp);
	
	delete [] dp;

	return 0;
}

void SkyArea::det_res_list(RLRecord *resRec, double x, double y, double z, double vx, double vy, double vz, double *Sdist, double *Edist, int ctype)
{
        detRDnumGC(&resRec->ra, &resRec->dec, x, y, z, this->obs_pos->ox, this->obs_pos->oy, this->obs_pos->oz, this->obs_pos->obs->dcx, this->obs_pos->obs->dcy, this->obs_pos->obs->dcz);

        resRec->ra = rad2grad(resRec->ra);
        resRec->dec = rad2grad(resRec->dec);

        detRDnumGC(&resRec->muRa, &resRec->muDe, vx, vy, vz, this->obs_pos->ox, this->obs_pos->oy, this->obs_pos->oz, this->obs_pos->obs->dcx, this->obs_pos->obs->dcy, this->obs_pos->obs->dcz);

        resRec->muRa = grad_to_mas(rad2grad(resRec->muRa))/1000.0/86400.0/cos(grad2rad(resRec->dec));
        resRec->muDe = grad_to_mas(rad2grad(resRec->muDe))/1000.0/86400.0;

	*Sdist = sqrt(x*x + y*y + z*z);
	*Edist = sqrt((this->obs_pos->ox - x)*(this->obs_pos->ox - x) + (this->obs_pos->oy - y)*(this->obs_pos->oy - y) + (this->obs_pos->oz - z)*(this->obs_pos->oz - z));

        if(ctype==1) resRec->magn = det_m(this->orb_catalog->record->H, *Sdist, *Edist, 5.8, detPhase(this->obs_pos->ox, this->obs_pos->oy, this->obs_pos->oz, x, y, z));
        if(ctype==3) resRec->magn = det_m(this->mpc_catalog->record->H, *Sdist, *Edist, 5.8, detPhase(this->obs_pos->ox, this->obs_pos->oy, this->obs_pos->oz, x, y, z));
        if(ctype==0) resRec->magn = det_m(det_planet_H(resRec->num)-5.5, *Sdist, *Edist, 5.8, detPhase(this->obs_pos->ox, this->obs_pos->oy, this->obs_pos->oz, x, y, z));
}

double SkyArea::DetExp(RLRecord *resRec, int mode)
{
	double rexp = 1.0;

	rexp *= this->ini_list->record->exp;
	rexp *= (this->obs_pos->otime<=this->ini_list->record->t1)&&(this->obs_pos->otime>=this->ini_list->record->t0);
	rexp *= this->task_list->record->exp;
//	rexp *= (this->res_list->record->RA<=this->ini_list->record->t1)&&(this->obs_pos->otime>=this->ini_list->record->t0);
        if((mode!=1)&&(!isTaskSA(resRec->ra, resRec->dec))) rexp *= 0.0;
        if((mode!=2)&&(resRec->nofobs>=this->task_list->record->Ntot)) rexp *= 0.0;
	rexp *= 0.99999;

//	if(

	return(rexp);
}

int SkyArea::init_time(double UTC)
{
    this->timeCur = UTC;
	if(this->obs_pos->det_observ(UTC)) return 1;

	return 0;
}

int SkyArea::set_opt(double RAmin, double RAmax, double DECmin, double DECmax, double magn_min, double magn_max)
{
	this->params->RAmin = RAmin;
	this->params->RAmax = RAmax;
	this->params->DECmin = DECmin;
	this->params->DECmax = DECmax;
	this->params->magn_min = magn_min;
	this->params->magn_max = magn_max;

	return 0;
}

int SkyArea::is_skyarea(RLRecord *resRec)
{
	double res = 0;
	int cs = 0;
	if(this->params->RAmax<=this->params->RAmin) cs = 1;
        if((resRec->dec<=this->params->DECmax)&&(resRec->dec>=this->params->DECmin)&&(resRec->magn<=this->params->magn_max)&&(resRec->magn>=this->params->magn_min)) res = 1;
	
        if(cs) res *= (resRec->ra<this->params->RAmax)||(resRec->ra>this->params->RAmin);
        else res *= (resRec->ra<this->params->RAmax)&&(resRec->ra>this->params->RAmin);
        res *= resRec->exp;
	return (fabs(res)>0.0);
}

int SkyArea::RL_sort(int stype)
{
	switch(stype)
	{
	case 0:
                //this->res_list->sort_num();
		break;
	case 1:
		this->res_list->sort_meridian(this->obs_pos->obs->stime);
		break;
	case 2:
		this->res_list->sort_magnitude(1);
		break;
	case 3:
		this->res_list->sort_magnitude(0);
		break;
	case 4:
		this->res_list->sort_obs();
		break;
	case 5:
		this->res_list->sort_RA_min2max(this->params->RAmin);
		break;
	case 6:
		this->res_list->sort_prior();
		break;
	default:
		break;
	}

	return 0;
}

int SkyArea::SetAutoRA()
{
	this->params->RAmin = this->obs_pos->obs->stime + this->params->dRA0;
	this->params->RAmax = this->obs_pos->obs->stime + this->params->dRA1;

	if(this->params->RAmin<0.0) this->params->RAmin += 2.0*PI;
	if(this->params->RAmax>2.0*PI) this->params->RAmax -= 2.0*PI;
/*
	if(this->params->RAmax<this->params->RAmin)
	{
		this->params->RAmax+=2.0*PI;
		if(this->params->RAmax<0.0) this->params->RAmax+=2.0*PI;
		else if(this->params->RAmin>2.0*PI) this->params->RAmin-=2.0*PI;
	}
*/
	return 0;
}

void SkyArea::SetTaskAutoRA()
{
	this->params->taskRAmin = this->obs_pos->obs->stime - this->task_list->record->dRA;
	this->params->taskRAmax = this->obs_pos->obs->stime + this->task_list->record->dRA;

	if(this->params->taskRAmin<0.0) this->params->taskRAmin += 2.0*PI;
	if(this->params->taskRAmax>2.0*PI) this->params->taskRAmax -= 2.0*PI;
/*
	if(this->params->taskRAmax<this->params->taskRAmin)
	{
		this->params->taskRAmax+=2.0*PI;
		if(this->params->taskRAmax<0.0) this->params->taskRAmax+=2.0*PI;
		else if(this->params->taskRAmin>2.0*PI) this->params->taskRAmin-=2.0*PI;
	}
*/
//	return 0;
}


int SkyArea::CutMaxObj()
{
	if(this->params->max_obj==0) return 0;

        while(this->res_list->resList.size()>this->params->max_obj) this->res_list->resList.removeLast();//this->res_list->resList.size()-1);

	return 0;
}

int SkyArea::AddIniList(IList *iniadd, int noft)
{
	int i, pose;
	char *tempfn, *dir_path, *catfn, *inifn;
	OrbCat *ocat_task;
	tempfn = new char[255];
	catfn = new char[255];
	inifn = new char[255];
	dir_path = new char[255];
	strcpy(tempfn, "");
	strcpy(dir_path, "");

	IList *ilist_task;
	ilist_task = new IList;

	sscat_rec *ssrec;

	this->task_list->GetTaskNum(noft);
	this->cat_list->GetByName(task_list->record->get_cat_name());
//	this->task_list->record->get_dir_path(dir_path);
	this->GetTaskIniFNames(inifn, catfn);
//	sprintf(catfn, "%s%s\\ini.cat", this->install_dir, dir_path);
//	sprintf(inifn, "%s%s\\ini.lst", this->install_dir, dir_path);

	ilist_task->init(inifn);
/*
	for(i=0; i<iniadd->nstr; i++)
	{
		iniadd->GetRec(i);
*/
		switch(cat_list->record->cat_type)
		{
		case 0:
			for(i=0; i<iniadd->nstr; i++)
			{
				iniadd->GetRec(i);
				iniadd->record->copyTo(ilist_task->record);
				ilist_task->AddRec();
				ilist_task->Save();
			}
			break;
		case 1:
			
			OrbCat *ocat_ini;
			ocat_task = new OrbCat;
			ocat_ini = new OrbCat;
			if(ocat_task->init(catfn)) return 1;
			this->cat_list->record->get_dir_path(dir_path);
			sprintf(tempfn, "%s%s", this->install_dir, dir_path);
//			sprintf(tempfn, "%s%s", this->install_dir, cat_list->record->path);
///		slovo(cat_list->record->path, tempfn, 0, &pose);
			ocat_ini->is_buff = 1;
			if(ocat_ini->init(tempfn)) return 1;

			for(i=0; i<iniadd->nstr; i++)
			{
				iniadd->GetRec(i);

				if(ocat_ini->GetRecName(iniadd->record->name)==-1) continue;
				ocat_ini->record->copyTo(ocat_task->record);
				ocat_task->AddRec();
				ocat_task->Save();

				iniadd->record->copyTo(ilist_task->record);
				ilist_task->AddRec();
				ilist_task->Save();
			}

			break;
		case 2:
			sscat *ss_task;
			sscatFB *ss_ini;
			ss_ini = new sscatFB;
			ss_task = new sscat;
//			ss_ini->is_buff = 1;

			ss_task->init(catfn);
			this->cat_list->record->get_dir_path(dir_path);
			sprintf(tempfn, "%s%s", this->install_dir, dir_path);
			if(ss_ini->init(tempfn)) return 1;

			ssrec = new sscat_rec;
			
			for(i=0; i<iniadd->nstr; i++)
			{
				iniadd->GetRec(i);
				ssrec->setLSPM(iniadd->record->name);
				if(ss_ini->GetName(ssrec->getLSPM())) continue;
				ss_ini->record->copyTo(ss_task->record);
				ss_task->AddRec();
				
				iniadd->record->copyTo(ilist_task->record);
				ilist_task->AddRec();
				
			}
			ss_task->Save();
			ilist_task->Save();
			break;
		case 3:
			
			mpccat *mcat_ini;
			ocat_task = new OrbCat;
			mcat_ini = new mpccat;
			if(ocat_task->init(catfn)) return 1;
			this->cat_list->record->get_dir_path(dir_path);
			sprintf(tempfn, "%s%s", this->install_dir, dir_path);
			if(mcat_ini->init(tempfn)) return 1;

			for(i=0; i<iniadd->nstr; i++)
			{
				iniadd->GetRec(i);
				if(mcat_ini->GetRecName(iniadd->record->name)) continue;
				mcat_ini->record->copyTo(ocat_task->record);
				ocat_task->AddRec();
				ocat_task->Save();

				iniadd->record->copyTo(ilist_task->record);
				ilist_task->AddRec();
				ilist_task->Save();
			}
			
			break;
		}

	return 0;
}

int SkyArea::RemIniList(LRecord *remrec, int noft)
{
	IList *ilst;
	ilst = new IList;

	char *tempstr;
	tempstr = new char[255];

	this->task_list->GetTaskNum(noft);
	this->GetTaskIniFNames(tempstr, NULL);
	ilst->init(tempstr);
//1	ilst->GetRec(remrec);
	ilst->DelRec(remrec);
	ilst->Save();

	return 0;
}

int SkyArea::UpdateIniList(LRecord *remrec0, LRecord *remrec1)
{
	IList *ilst;
	ilst = new IList;

	this->GetIniList(remrec0->noftask);
	int ipos = this->ini_list->GetRec(remrec0);
	if(ipos==-1) return 1;

	char *tempstr;
	tempstr = new char[255];

	this->task_list->GetTaskNum(remrec0->noftask);
	this->GetTaskIniFNames(tempstr, NULL);
	ilst->init(tempstr);
//1	ilst->GetRec(remrec);
	remrec1->copyTo(ilst->record);
	ilst->UpdateRec(ipos);
	ilst->Save();

	return 0;
}

int SkyArea::UpdateTaskCatalog(int tasknum)
{
	int i;
	char *siname = new char[255];
	char *scname = new char[255];
	char *tstr = new char[255];

	
//	this->ini_list->init(tstr);
//	this->ini_list->Free();

	if(this->task_list->GetTaskNum(tasknum)==-1) return 1;

	this->GetTaskIniFNames(siname, scname);
/*	this->install_dir, this->task_list->record->get_ini_fname(tstr);
	sprintf(siname, "%s%s", this->install_dir, tstr);
	this->install_dir, this->task_list->record->get_cat_fname(tstr);
	sprintf(scname, "%s%s", this->install_dir, tstr);
*/
	sprintf(tstr, "%sini.lst", this->install_dir);
	IList *ulist = new IList;
	this->ini_list->init(siname);
	this->ini_list->SaveAs(tstr);

	ulist->init(siname);
	ulist->Free();
	ulist->Save();

	FILE *FE = fopen(scname, "w");
	fclose(FE);

	return(this->AddIniList(this->ini_list, tasknum));
}

int SkyArea::AddTask(TLRecord *ntask)
{
//	CAtlString tcstr;
	char *tstrdir, *tstr;
	tstrdir = new char[255];
	tstr = new char[255];

	ntask->get_dir_path(tstrdir);
        sprintf(tstr, "mkdir %s%s", this->install_dir, tstrdir);

        if(system(tstr)) return 1;
        /*if(mkdir(tstr))
	{
		return 1;
        }*/
//	tcstr = tstr;
/*
        if (!CreateDirectory(tstr, NULL))
	{
		if(GetLastError()==ERROR_PATH_NOT_FOUND) return 1;
/*		if(GetLastError()==ERROR_ALREADY_EXISTS) break;
		{
			AfxMessageBox("Already Exists", MB_OK);
		}/
	}*/
	

	this->task_list->AddTask(ntask);

	return 0;
}

int SkyArea::GetIniList(int noft)
{
	char *inifn = new char[255];
	char *dir_path = new char[255];
	this->task_list->GetTaskNum(noft);
	this->task_list->record->get_dir_path(dir_path);
	sprintf(inifn, "%s%s\\ini.lst", this->install_dir, dir_path);
	this->ini_list->init(inifn);

	return 0;
}

double SkyArea::isTaskSA(double RA, double DEC)
{
	double res = 1.0;
	int cs = 0;
	if(this->params->taskRAmax<=this->params->taskRAmin) cs = 1;
	
	if(cs) res *= (RA<this->params->taskRAmax)||(RA>this->params->taskRAmin);
	else res *= (RA<this->params->taskRAmax)&&(RA>this->params->taskRAmin);


	return res;
}

int SkyArea::initVisualProp(double T)
{
//	double x, y, z;
	double tr, ts;
	this->init_time(T);

//	if(this->obs_pos->place->detR(&this->params->sunX, &this->params->sunY, &this->params->sunZ, this->obs_pos->otime, SUN_NUM, 0, this->obs_pos->center, obs_pos->sk)) return 1;
	detRDnumGC(&this->params->sunRA, &this->params->sunDE, 0.0, 0.0, 0.0, this->obs_pos->ox, this->obs_pos->oy, this->obs_pos->oz, this->obs_pos->obs->dcx, this->obs_pos->obs->dcy, this->obs_pos->obs->dcz);

	detRiseSet(&tr, &ts, this->params->sunDE, this->obs_pos->obs->record->getFi());

        params->timeSunRise = tr;
        params->timeSunSet = ts;

/*
        params->timeSunRise = tr + this->params->sunRA;
        params->timeSunSet = ts + this->params->sunRA;

        if(params->timeSunRise>2.0*PI) params->timeSunRise -= 2.0*PI;
        if(params->timeSunSet>2.0*PI) params->timeSunSet -= 2.0*PI;
*/

	return 0;
}

void SkyArea::UpdateObsList()
{
	this->obs_list->Clear();

    int sz = this->log_list->nstr();
	for(int i=0; i<sz; i++)
	{
		log_list->GetRec(i);
		if(log_list->record->success) obs_list->ChObs(log_list->record->noftask, log_list->record->name, 1);
	}
}

///////////////////////////////////////////

IListBuf::IListBuf()
{
	this->record = new LRecord;
	this->next = NULL;
}

IListBuf::~IListBuf()
{
	delete(this->record);
	this->record = NULL;
	this->next = NULL;
}

///////////////////////////////////////////

IListBuffer::IListBuffer()
{
	root = new IListBuf;
	num = 0;
}

IListBuffer::~IListBuffer()
{
	delete(root);
	root = NULL;
}

int IListBuffer::addElem(LRecord *npar)
{
	IListBuf *troot, *tnew;

	troot = root;

	while(root->next!=NULL)
	{
		root = root->next;
	}

	tnew = new IListBuf;
	tnew->next = NULL;

	npar->copyTo(tnew->record);

	root->next = tnew;

	root = troot;

	this->num++;
	
	return 0;
}

int IListBuffer::remElem(int num)
{
	if(num>=this->num) return 1;

	IListBuf *troot, *tnext;
	troot = root;
	tnext=NULL;

	int i = 0;
	while((i<num)&&root!=NULL)
	{
		root = root->next;
		i++;
	}

	if(root==NULL) return 1;

	if(root->next!=NULL) tnext=root->next->next;

	delete(root->next);

	root->next = tnext;
	root = troot;

	this->num--;



	return 0;
}

int IListBuffer::clear()
{
	int nnn;

	for(nnn=this->num-1;nnn>=0;nnn--)
	{
		if(this->remElem(nnn)) return 1;
	}

	return 0;
}

///////////////////////////////////////////
