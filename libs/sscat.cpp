#include "stdafx.h"
#include "sscat.h"

sscat_rec::sscat_rec()
{
	LPSM = new char[128];
	LSPM = new char[128];
	LHS = new char[128];
	OName = new char[128];
	MASS2 = new char[128];
	USNOB1 = new char[128];
	Aflag = new char[128];
}

sscat_rec::~sscat_rec()
{
	delete [] LPSM;
	delete [] LSPM;
	delete [] LHS;
	delete [] OName;
	delete [] MASS2;
	delete [] USNOB1;
	delete [] Aflag;
}

void sscat_rec::setLSPM(char *name)
{
	indexB(this->LSPM, name, " ", 11);
	strcpy(&LSPM[11], "\0");

}

char* sscat_rec::getLSPM()
{
	return this->LSPM;
}

void sscat_rec::copyTo(sscat_rec *nrec)
{
	strcpy(nrec->LPSM, LPSM);
	strcpy(nrec->LSPM, LSPM);
	strcpy(nrec->LHS, LHS);

	nrec->NLTT = NLTT;
	nrec->HIP = HIP;
	nrec->TYC1 = TYC1;
	nrec->TYC2 = TYC2;
	nrec->TYC3 = TYC3;
	nrec->ASCC25 = ASCC25;
	nrec->UCAC2 = UCAC2;

	strcpy(nrec->OName, OName);
	strcpy(nrec->MASS2, MASS2);
	strcpy(nrec->USNOB1, USNOB1);

	nrec->RAdeg = RAdeg;
	nrec->DEdeg = DEdeg;
	nrec->rPM = rPM;
	nrec->rPMRA = rPMRA;
	nrec->rPMDE = rPMDE;
	nrec->pm = pm;
	nrec->pmRA = pmRA;
	nrec->pmDE = pmDE;
	
	strcpy(nrec->Aflag, Aflag);

	nrec->Bmag = Bmag;
	nrec->Vmag = Vmag;
	nrec->BJmag = BJmag;
	nrec->RFmag = RFmag;
	nrec->INmag = INmag;
	nrec->Jmag = Jmag;
	nrec->Hmag = Hmag;
	nrec->Kmag = Kmag;
	nrec->Vemag = Vemag;
	nrec->VJ = VJ;
}

int sscat_rec::fromString(char *str_in)
{
//    printf("str_in: %s", str_in);
    printf("str_in len: %d <? %d", strlen(str_in), SSCAT_REC);
    if(strlen(str_in)<SSCAT_REC)
    {
        printf("err string len");
        return 1;
    }
    char *tstr;
    tstr = new char[SIMPLE_STR];

    slovoG(str_in, tstr, 0, 4);
    strcpy(LPSM, tstr);
    strcpy(&LPSM[4], "\0");

    slovoG(str_in, tstr, 5, 16);
    setLSPM(tstr);
//	strcpy(&LSPM[11], "\0");

    slovoG(str_in, tstr, 17, 22);
    strcpy(LHS, tstr);
    strcpy(&LHS[5], "\0");

    slovoG(str_in, tstr, 23, 28);
    NLTT = atoi(tstr);

    slovoG(str_in, tstr, 29, 35);
    HIP = atoi(tstr);

    slovoG(str_in, tstr, 36, 40);
    TYC1 = atoi(tstr);

    slovoG(str_in, tstr, 41, 45);
    TYC2 = atoi(tstr);

    slovoG(str_in, tstr, 46, 47);
    TYC3 = atoi(tstr);

    slovoG(str_in, tstr, 48, 55);
    ASCC25 = atoi(tstr);

    slovoG(str_in, tstr, 56, 64);
    UCAC2 = atoi(tstr);

    slovoG(str_in, tstr, 65, 95);
    strcpy(OName, tstr);
    strcpy(&OName[30], "\0");

    slovoG(str_in, tstr, 96, 112);
    strcpy(MASS2, tstr);
    strcpy(&MASS2[16], "\0");

    slovoG(str_in, tstr, 113, 125);
    strcpy(USNOB1, tstr);
    strcpy(&USNOB1[12], "\0");

    slovoG(str_in, tstr, 127, 137);
    RAdeg = atof(tstr);

    slovoG(str_in, tstr, 138, 148);
    DEdeg = atof(tstr);

    slovoG(str_in, tstr, 149, 156);
    rPM = atof(tstr);

    slovoG(str_in, tstr, 157, 164);
    rPMRA = atof(tstr);

    slovoG(str_in, tstr, 165, 172);
    rPMDE = atof(tstr);

    slovoG(str_in, tstr, 173, 180);
    pm = atof(tstr);

    slovoG(str_in, tstr, 181, 188);
    pmRA = atof(tstr);

    slovoG(str_in, tstr, 189, 196);
    pmDE = atof(tstr);

    slovoG(str_in, tstr, 197, 199);
    strcpy(Aflag, tstr);
    strcpy(&Aflag[2], "\0");

    slovoG(str_in, tstr, 200, 205);
    Bmag = atof(tstr);

    slovoG(str_in, tstr, 206, 211);
    Vmag = atof(tstr);

    slovoG(str_in, tstr, 212, 216);
    BJmag = atof(tstr);

    slovoG(str_in, tstr, 217, 221);
    RFmag = atof(tstr);

    slovoG(str_in, tstr, 222, 226);
    INmag = atof(tstr);

    slovoG(str_in, tstr, 227, 232);
    Jmag = atof(tstr);

    slovoG(str_in, tstr, 233, 238);
    Hmag = atof(tstr);

    slovoG(str_in, tstr, 239, 244);
    Kmag = atof(tstr);

    slovoG(str_in, tstr, 245, 251);
    Vemag = atof(tstr);

    slovoG(str_in, tstr, 252, 257);
    VJ = atof(tstr);
//printf("\nlast\n");
    delete [] tstr;
//    printf("\nlast\n");
    return 0;
}

void sscat_rec::toString(char *str_out)
{
    char *tstr;
    tstr = new char[SIMPLE_STR];
    char *lspm = new char[12];

    strcpy(tstr, "");
    strcpy(str_out, "");

    sprintf(tstr, "%s ", LPSM);
    strncat(str_out, tstr, 5);


    sprintf(tstr, "%s ", getLSPM());
    strncat(str_out, tstr, 12);

    sprintf(tstr, "%s ", LHS);
    strncat(str_out, tstr, 6);

    if(NLTT==0) sprintf(tstr, "      ");
    else sprintf(tstr, "%5d ", NLTT);
    strncat(str_out, tstr, 6);

    if(HIP==0) sprintf(tstr, "       ");
    else sprintf(tstr, "%6d ", HIP);
    strncat(str_out, tstr, 7);

    if(TYC1==0) sprintf(tstr, "     ");
    else sprintf(tstr, "%4d ", TYC1);
    strncat(str_out, tstr, 5);

    if(TYC2==0) sprintf(tstr, "     ");
    else sprintf(tstr, "%4d ", TYC2);
    strncat(str_out, tstr, 5);

    if(TYC3==0) sprintf(tstr, "  ");
    else sprintf(tstr, "%1d ", TYC3);
    strncat(str_out, tstr, 2);

    if(ASCC25==0) sprintf(tstr, "        ");
    else sprintf(tstr, "%7d ", ASCC25);
    strncat(str_out, tstr, 8);

    if(UCAC2==0) sprintf(tstr, "         ");
    else sprintf(tstr, "%8d ", UCAC2);
    strncat(str_out, tstr, 9);

    sprintf(tstr, "%s ", OName);
    strncat(str_out, tstr, 31);

    sprintf(tstr, "%s ", MASS2);
    strncat(str_out, tstr, 18);

    int len = strlen(USNOB1);
    sprintf(tstr, "%s  ", USNOB1);
    strncat(str_out, tstr, 15);

    sprintf(tstr, "%10.6f ", RAdeg);
    strncat(str_out, tstr, 11);

    sprintf(tstr, "%10.6f ", DEdeg);
    strncat(str_out, tstr, 11);

    sprintf(tstr, "%7.3f ", rPM);
    strncat(str_out, tstr, 8);

    sprintf(tstr, "%7.3f ", rPMRA);
    strncat(str_out, tstr, 8);

    sprintf(tstr, "%7.3f ", rPMDE);
    strncat(str_out, tstr, 8);

    sprintf(tstr, "%7.3f ", pm);
    strncat(str_out, tstr, 8);

    sprintf(tstr, "%7.3f ", pmRA);
    strncat(str_out, tstr, 8);

    sprintf(tstr, "%7.3f ", pmDE);
    strncat(str_out, tstr, 8);

    sprintf(tstr, "%s ", Aflag);
    strncat(str_out, tstr, 3);

    sprintf(tstr, "%5.2f ", Bmag);
    strncat(str_out, tstr, 6);

    sprintf(tstr, "%5.2f ", Vmag);
    strncat(str_out, tstr, 6);

    sprintf(tstr, "%4.1f ", BJmag);
    strncat(str_out, tstr, 5);

    sprintf(tstr, "%4.1f ", RFmag);
    strncat(str_out, tstr, 5);

    sprintf(tstr, "%4.1f ", INmag);
    strncat(str_out, tstr, 5);

    sprintf(tstr, "%5.2f ", Jmag);
    strncat(str_out, tstr, 6);

    sprintf(tstr, "%5.2f ", Hmag);
    strncat(str_out, tstr, 6);

    sprintf(tstr, "%5.2f ", Kmag);
    strncat(str_out, tstr, 6);

    sprintf(tstr, "%6.2f ", Vemag);
    strncat(str_out, tstr, 7);

    sprintf(tstr, "%5.2f", VJ);
    strncat(str_out, tstr, 6);

    strcat(&str_out[256], "\0");

    delete [] tstr;
}

sscat::sscat() : fBuffer(SSCAT_REC)
{
	this->record = new sscat_rec;
}

sscat::~sscat()
{
	delete record;
}

int sscat::init(char *fname)
{
	return(fBuffer::init(fname, SSCAT_REC));
}

void sscat::s2rec(char *str_in)
{
        this->record->fromString(str_in);
}

void sscat::rec2s(char *str_out)
{
        this->record->toString(str_out);
}

int sscat::GetRec(int pos)
{
	if(this->ReadStr(pos)) return 1;
	this->s2rec(this->str);
	return 0;
}

int sscat::GetRecNameLSPM(char *oname)
{
	strcpy(&oname[11], "\0");
	sscat_rec *trec = new sscat_rec;
	trec->setLSPM(oname);

	for(int i=0; i<this->nstr; i++)
	{
		this->GetRec(i);
		if(streqv(this->record->getLSPM(), trec->getLSPM())) return i;

	}

	return -1;
}

int sscat::AddRec(int pos)
{
	char *new_str;
	new_str = new char[STRLEN];
	this->rec2s(new_str);
	this->AddStr(new_str, pos);

	delete [] new_str;
	return 0;
}

int sscat::AddRec()
{
	char *new_str;
	new_str = new char[STRLEN];
	this->rec2s(new_str);
	this->AddStr(new_str, this->nstr);

	delete [] new_str;
	return 0;
}
/*
int sscat::GetRecName(char *name);
int sscat::GetRecNum(int num);
int sscat::AddRec(int pos);
int sscat::AddRec();
*/
