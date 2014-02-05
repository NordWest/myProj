//#include "stdafx.h"
//#ifdef EASS
//#include "..\DAster\Eassistant\StdAfx.h"
//#endif

//#include "..\DAster\OMen\StdAfx.h"

//#include "..\DAster\CreatOL\StdAfx.h"
//#include "..\DAster\CreatOL\CreatOLDlg.h"

//#include "..\DAster\MainFrm.h"

/////////////////////////////////////
#include "comfunc.h"
#include <math.h>

int body_num(QString pname)
{
    if(QString().compare(pname, "Mercury")==0) return 1;
    if(QString().compare(pname, "Venus")==0) return 2;
    if(QString().compare(pname, "Earth")==0) return 3;
    if(QString().compare(pname, "Mars")==0) return 4;
    if(QString().compare(pname, "Jupiter")==0) return 5;
    if(QString().compare(pname, "Saturn")==0) return 6;
    if(QString().compare(pname, "Uranus")==0) return 7;
    if(QString().compare(pname, "Neptune")==0) return 8;
    if(QString().compare(pname, "Pluto")==0) return 9;
    if(QString().compare(pname, "Sun")==0) return 10;


    return -1;
}

int installDirCut(char *installDir, char* str, char *strCutted)
{
	if((installDir==NULL)||(strCutted==NULL)||(str==NULL)) return 1;

	char *nstr = new char[255];
	int ntmp, len;
	len = strlen(installDir);
	ntmp = streqv1(str, installDir);
	if(ntmp==len) strcpy(strCutted, &str[ntmp]);
	else strcpy(strCutted, str);

	delete [] nstr;
	return 0;
}

char* del_end_space(char *str)
{
	if(str==NULL) return("");
	char *sim = new char[256];

	int mlen = strlen(str);
	char *rstr = new char[mlen];
	strcpy(rstr, str);
	for(int i=1; i<mlen; i++)
	{
		strcpy(sim, &str[mlen-i]);
		strcpy(&sim[1], "\0");
		if(!streqv(sim, " "))
		{
			strcpy(&rstr[mlen-i+1], "\0");
			break;
		}
	}

	return rstr;
}

int dat2JDN(int year, int month, int day)//month - 1-12;
{
    double jdDate;
    dat2JD(&jdDate, year, month, day+0.5);
    return((int) jdDate);
    /*
    double a = (14-month)/12;
    double y = year + 4800 - a;
    double m = month + 12*a - 3;
    return(day + (153*m+2)/5 + 365*y + y/4 - y/100 + y/400 - 32045);*/
    //return(367*y - (7*(y+ (m+9)/12.0))/4.0 + 285*m/9.0 + day - 730530);
}




/*double det_average(myvector *x)
{
	int i;
	int len = x->getlength();
	double summ = 0.0;
	for(i=0; i<len; i++) summ += x->get(i);
	return(summ/(1.0*len));
}*/

int slovoG(char *source, char *sres, int pos1, int pos2)
{
	strcpy(sres, "");

	strncpy(sres, &source[pos1], pos2-pos1);
	strcpy(&sres[pos2-pos1], "\0");

 	return 0;
}
/*
int dat2JDalt(double *Jday, int year, int mth, double day)
{
	dat2day(Jday, year, mth, day, EPO);
	*Jday += JDEPO;
	return 0;
}
*/
void rad2hms_str(double rad, char *str)
{
	if(str==NULL) return;

	int h, m, znak;
	double s;

	rad2hms(rad, &znak, &h, &m, &s);

	sprintf(str, "%02d %02d %07.4f", h, m, s);
}

void dat2YMD_str(double JD, char *str, int nacc)
{
	if(str==NULL) return;

	int y, m;
	double day;

	dat2YMD(JD, &y, &m, &day);

        char *fStr = new char[255];
        int i0;
        if(nacc>0) i0 = nacc+3;
        else i0 = 2;
        sprintf(fStr, "%%4d %%02d %%0%d.%df", i0, nacc);

//        printf("\nfStr: \"%s\"\n", fStr);

        sprintf(str, fStr, y, m, day);
}
/*
void dat2YMD_str(double JD, char *str)
{
        if(str==NULL) return;

        int y, m;
        double day;

        dat2YMD(JD, &y, &m, &day);

        sprintf(str, "%4d %02d %010.7f", y, m, day);
}
*/

void rad2gms_str(double rad, char *str)
{
	if(str==NULL) return;

	int h, m, znak;
	double s;
	char *sn;
	sn = new char;

	rad2gms(rad, &znak, &h, &m, &s);
	strcpy(sn, "+");
	if(znak<=0) strcpy(sn, "-");

	sprintf(str, "%1s%02d %02d %06.3f", sn, h, m, s);
}

void rad2gms_str1(double rad, char *str)
{
	if(str==NULL) return;

	int h, m, znak;
	double s;
	char *sn;
	sn = new char;

	rad2gms(rad, &znak, &h, &m, &s);
	strcpy(sn, "+");
	if(znak<=0) strcpy(sn, "-");

	sprintf(str, "%03d %02d %06.3f", h, m, s);
}


double str2JD(char *str)
{
	char *tstr;
	tstr = new char[32];

	int h, m;
	double s, res;

	slovoG(str, tstr, 0, 4);
	h = atoi(tstr);

	slovoG(str, tstr, 5, 7);
	m = atoi(tstr);

	slovoG(str, tstr, 8, 13);
	s = atof(tstr);

	dat2JD(&res, h, m, s);

	delete [] tstr;

	return res;
}

double str2JD_time(char *str)
{
	char *tstr;
	tstr = new char[32];

	int year, month, day, hour, min, sec;
	double s, res;

	slovoG(str, tstr, 0, 4);
	year = atoi(tstr);

	slovoG(str, tstr, 5, 7);
	month = atoi(tstr);

	slovoG(str, tstr, 8, 10);
	day = atoi(tstr);

	slovoG(str, tstr, 11, 13);
	hour = atoi(tstr);

	slovoG(str, tstr, 14, 16);
	min = atoi(tstr);

	slovoG(str, tstr, 17, 19);
	sec = atoi(tstr);

	HMS2day(&s, hour, min, sec);
	dat2JD(&res, year, month, day + s);

	delete [] tstr;

	return res;
}

void JD2str(double JD, char *str)
{
	int h, m;
	double s;

	dat2YMD(JD, &h, &m, &s);
	sprintf(str, "%4d/%02d/%5.2f", h, m, s);
//	else sprintf(str, "%4d/%2d/%5.2f", h, m, s);
}

void JD2str_time(double JD, char *str)
{
	int year, month, day;
	int hour, min, sec;
	double s, s1;

	dat2YMD(JD, &year, &month, &s);
	day2HMS(s, &day, &hour, &min, &s1);
	sec = (int)(s1+0.99999999999999);
	if(sec==60)
	{
		sec = 0;
		min++;
	}
	if(min==60)
	{
		min = 0;
		hour++;
	}
	sprintf(str, "%4d/%02d/%2d %02d:%02d:%02d", year, month, day, hour, min, sec);
//	else sprintf(str, "%4d/%2d/%5.2f", h, m, s);
}

/*
int dat2JD(double *Jday, int year, int mth, double day)
{
	int iyt, mct, idat;
	double id;
	id = day;

	idat = year*10000 + mth*100 + day;

	if(mth>2)
	{
		iyt = year;
		mct = mth;
	}
	else
	{
		iyt = year - 1;
		mct = mth + 12;
	}

	int ia, ja;

	ia = iyt/100;
	ja = ia/4;

	int ib, ic;

	if(idat>15821015) ib = 2 - ia + ja;
	else ib=0;
/*	if((year>1582)||((year==1582)&&((mth>10)||((mth==10)&&(day>15))))) ib=2-ia+ja;
	else ib = 0;
//
	ic = 365.25*iyt;

	double jd, dajul;

	jd = 30.6001*(mct+1);
	dajul=ib+ic+jd+id+1720994.5;

	*Jday = dajul;

	return 0;
}
*/

void min2dhms(double min, int *d, int *h, int *m, int *s)
{
	*d = min/(60*24);
	min -= *d*60*24;
	*h = min/60;
	min -= *h*60;
	*m = min/1;
	*s = (int)((min - *m)*60);
}
/*
int dat2JD(double *Jday, int year, int mth, double day)
{
	int y, m;
	

	if(mth<3)
	{
		y = year-1;
		m = mth+12;
	}
	else
	{
		y = year;
		m = mth;
	}

	double A, B;

	A = y/100;
	B = 2 - A + A/4;

//	*Jday = floor(365.25*y) + floor(30.6001*(m+1)) + day + 1720994.5 + B;
        *Jday = floor(365.25*y) + floor(30.6001*(m+1)) + day + 1720993.75 + B;

	return 0;
}
*/
int dat2JD_time(double *Jday, int year, int mth, int day, int hour, int min, double sec)
{
	double s;
	HMS2day(&s, hour, min, sec);
	dat2JD(Jday, year, mth, day+s);
	return 0;
}

int dat2YMD(double Jday, int *year, int *mth, double *day)
{
	double x, Z, F, q, A, B, C, D, E;

	x = Jday + 0.5;
        Z = (int)floor(x);
	F = x - Z;

        q = (int)floor((Z - 1867216.25)/36524.25);
        A = Z + 1 + q - (int)q/4;
	B = A + 1524;
        C = (int)floor((B - 122.1)/365.25);
        D = (int)floor(365.25*C);
        E = (int)floor((B - D)/30.6001);
        *day = B - D - (int)floor(30.6001*E) + F;

	if(*day>=32.0)
	{
		E++;
		*day-=31.0;
	}


	if(E<13.5) *mth = E - 1;
	else *mth = E - 13;

	if(*mth<2.5) *year = C - 4715;
	else *year = C - 4716;

	if(*day<1.0)
	{
		*mth -= 1;
		if(*mth<1)
		{
			*mth += 12;
			*year -= 1;
		}
		*day += dinm(*mth, isVes(*year));
	}

	return 0;
}
/*
void getDATEOBSfromMJD(DATEOBS *date_obs, double mjd)
{
    //DATEOBS date_obs;
    double parth_of_day = mjd - floor(mjd);
    double JD = 2400000.5+mjd;
    double x = JD+0.5;
    int Z = (int)floor(x);
    double F = x - Z;
    int q = (int)floor((Z - 1867216.25)/36524.25);
    int A = Z + 1 + q - (int)(q/4);
    int B = A + 1524;
    int C = (int)floor((B - 122.1)/365.25);
    int D = (int)floor(365.25*C);
    int E = (int)floor((B - D)/30.6001);
    int Day = B - D - (int)floor(30.6001*E) + F;
    int Month;
    if (E<13.5) Month = E - 1;
    else Month = E - 13;
    int Year;
    if (Month<2.5) Year = C - 4715;
    else Year = C - 4716;

    date_obs->year = Year;
    date_obs->month = Month;
    date_obs->day = Day;
    date_obs->pday = Day+parth_of_day;
    date_obs->hour = (int)floor(parth_of_day*24);
    date_obs->min = (int)floor(parth_of_day*1440) - date_obs->hour*60;
    date_obs->sec = parth_of_day*86400 - date_obs->hour*3600 - date_obs->min*60;

    int dayM;

    if(date_obs->sec>=59.99999)
    {
        date_obs->sec = 0.0;
        date_obs->min +=1;
    }
    if(date_obs->min>=60)
    {
        date_obs->min -= 60;
        date_obs->hour++;
    }
    if(date_obs->hour>=24)
    {
        date_obs->hour -= 24;
        date_obs->day++;
    }
    dayM = dinm(date_obs->month, isVes(date_obs->year));
    if(date_obs->day>dayM)
    {
        date_obs->day -= dayM;
        date_obs->month++;
    }
    if(date_obs->month>12)
    {
        date_obs->year++;
        date_obs->month -= 12;
    }


    //return date_obs;
}
*/
void dat2YMD_time(double Jday, int *year, int *mth, int *day, int *hour, int *min, double *sec)
{
	double dday;
	dat2YMD(Jday, year, mth, &dday);
	day2HMS(dday, day, hour, min, sec);
}

int YMD2JD(double *Jday, int ymd)
{
	int y, m;
	double d;

	div_t dt;

	dt = div(ymd, 100);
	ymd = dt.quot;
	d = dt.rem*1.0;
	dt = div(ymd, 100);
	y = dt.quot;
	m = dt.rem;

	dat2JD(Jday, y, m, d);

	return 0;
}

int JD2YMD(double Jday, int *ymd)
{
	int y, m;
	double d;

	dat2YMD(Jday, &y, &m, &d);

	*ymd = y*10000 + m*100 + (int)d;
	if(fabs(d-(int)d)<EPS) return 0;

	return 1;
}

void day2HMS(double day, int *iday, int *hour, int *min, double *sec)
{
	*iday = (int) day;

	day -= *iday;

	*iday++;

	day *= 24.0;

	*hour = (int)day;

	day -= *hour;

	day *= 60.0;

	*min = (int)day;

	day -= *min;

	*sec = day*60.0;
}

void HMS2day(double *day, int hour, int min, double sec)
{
	*day = (((1.0*sec)/60.0 + min)*1.0/60.0 + hour)*1.0/24.0;
}

int dat2JD(double *Jday, int year, int mth, double day) //dat2JDalt2
{
	int iy, mc;
	double id;

	iy = year;
	mc = mth;
	id = day;

	int iyt, mct;

	if(mc>2)
	{
		iyt = iy;
		mct = mc-3;
	}
	else
	{
		iyt = iy-1;
		mct=mc+9;
	}

	int ia, ja;

	ia = iyt/100;
	ja = iyt-100*ia;

	int js, jt, jm;

	js = 146097*ia/4;
	jt = 1461*ja/4;

	jm = (153*mct+2)/5;

	double jday;

	jday = js+jt+jm+id+1721118.5;

	*Jday = jday;

	return 0;
}

int dat2YMDalt(double Jday, int *year, int *mth, double *day)
{
	int y, m;
	double d;
	y = EPO;
	Jday -= JDEPO;
	while(Jday>(365.0+isVes(y)))
	{
		if(isVes(y)) Jday -=366;
		else Jday -= 365;
		y++;
	}
	m = 1;
	while(Jday>=dinm(m, isVes(y)))
	{
		Jday -= dinm(m, isVes(y));
		m++;
		if(m>12)
		{
			y++;
			m=1;
		}
	}

//	Jday++;
	d = Jday + 1.0;

	*year = y;
	*mth = m;
	*day = d;

	return 0;
}
/*
int dat2YMD(double Jday, int *year, int *mth, double *day)
{
	int *juldat;
	double djdp, ddd, ddx;

	djdp = Jday;
	ddd = djdp-2415078.5;
	ddx = (ddd*4.0-1.0)/1461.0;

	int iy, it, id, im, jd;

	iy = (int)ddx;
	it = (int)ddd;
	id = (4*it+3-1461*iy)/4;
	im = (5*id-3)/153;
	jd = (5*id+2-153*im)/5;

	if(im>9)
	{
		im = im - 9;
		iy += 1;
	}
	else im += 3;

	int kg;

	kg = 1900+iy;

	*year = kg;
	*mth = im;
	*day = jd;

//	*juldat = kg*10000+im*100+jd;

	return 0;
}
*/
int dat2day(double *day, int y, int m, double d, int epo)
{
	double cday;
	int ty, tm;

	cday = 0.0;
	ty = epo;
	tm = 1;

	while(ty<y)
	{
		if(isVes(ty)) cday +=366;
			else cday += 365;
		ty++;
	}
//	ty++;
	while(tm<m)
	{
		cday += dinm(tm, isVes(ty));
		tm++;
		if(tm>12)
		{
			ty++;
			tm = 1;
		}
	}
//	cday += d;
	*day = cday + (d - 1.0);
	return 0;
}
/*
int isVes(int year)
{
	int k1, k2;
	int d1, d2, d3, d4, ym;

	k1 = k2 = 0;

	k1 = ((year/4.0)==(year/4));

	d4 = year;
	d1 = year/1000;
	d4 -= 1000*d1;
	d2 = d4/100;
	d4 -= 100*d2;
	d3 = d4/10;
	d4 -= 10*d3;

	ym = d1*10 + d2;

	k2 = k1;
	if((d4==0)&&(d3==0))
	{
		k2 = (ym/4.0)==(ym/4);
	}

	return((k1&&k2));
}

double dinm(int mounth, int isves)
{
	//mounth--;
	double m[12];
	m[0] = 31.0;
	m[1] = 28.0;
	m[2] = 31.0;
	m[3] = 30.0;
	m[4] = 31.0;
	m[5] = 30.0;
	m[6] = 31.0;
	m[7] = 31.0;
	m[8] = 30.0;
	m[9] = 31.0;
	m[10] = 30.0;
	m[11] = 31.0;
//	m[12] = m[0];

	if(mounth!=2) return(m[mounth-1]);
	else return(m[mounth-1] + isves);
}
*/
int upackDigStr(char *pstr, char *upstr)
{
    if(streqv(pstr, "-")) strcpy(upstr, "-\0");
    if(streqv(pstr, "+")) strcpy(upstr, "+\0");
        if(streqv(pstr, " ")) strcpy(upstr, " \0");
        if(streqv(pstr, "0")) strcpy(upstr, "0\0");
        if(streqv(pstr, "1")) strcpy(upstr, "1\0");
        if(streqv(pstr, "2")) strcpy(upstr, "2\0");
        if(streqv(pstr, "3")) strcpy(upstr, "3\0");
        if(streqv(pstr, "4")) strcpy(upstr, "4\0");
        if(streqv(pstr, "5")) strcpy(upstr, "5\0");
        if(streqv(pstr, "6")) strcpy(upstr, "6\0");
        if(streqv(pstr, "7")) strcpy(upstr, "7\0");
        if(streqv(pstr, "8")) strcpy(upstr, "8\0");
        if(streqv(pstr, "9")) strcpy(upstr, "9\0");

        if(streqv(pstr, "A")) strcpy(upstr, "10\0");
        if(streqv(pstr, "B")) strcpy(upstr, "11\0");
        if(streqv(pstr, "C")) strcpy(upstr, "12\0");
        if(streqv(pstr, "D")) strcpy(upstr, "13\0");
        if(streqv(pstr, "E")) strcpy(upstr, "14\0");
        if(streqv(pstr, "F")) strcpy(upstr, "15\0");
        if(streqv(pstr, "G")) strcpy(upstr, "16\0");
        if(streqv(pstr, "H")) strcpy(upstr, "17\0");
        if(streqv(pstr, "I")) strcpy(upstr, "18\0");
        if(streqv(pstr, "J")) strcpy(upstr, "19\0");
        if(streqv(pstr, "K")) strcpy(upstr, "20\0");
        if(streqv(pstr, "L")) strcpy(upstr, "21\0");
        if(streqv(pstr, "M")) strcpy(upstr, "22\0");
        if(streqv(pstr, "N")) strcpy(upstr, "23\0");
        if(streqv(pstr, "O")) strcpy(upstr, "24\0");
        if(streqv(pstr, "P")) strcpy(upstr, "25\0");
        if(streqv(pstr, "Q")) strcpy(upstr, "26\0");
        if(streqv(pstr, "R")) strcpy(upstr, "27\0");
        if(streqv(pstr, "S")) strcpy(upstr, "28\0");
        if(streqv(pstr, "T")) strcpy(upstr, "29\0");
        if(streqv(pstr, "U")) strcpy(upstr, "30\0");
        if(streqv(pstr, "V")) strcpy(upstr, "31\0");

	return 0;
}

int packDigStr(char *upstr, char *pstr)
{
        //if(streqv(upstr, " ")) strcpy(pstr, " \0");
        if(streqv(upstr, "00")) strcpy(pstr, "0\0");
        if(streqv(upstr, "01")) strcpy(pstr, "1\0");
        if(streqv(upstr, "02")) strcpy(pstr, "2\0");
        if(streqv(upstr, "03")) strcpy(pstr, "3\0");
        if(streqv(upstr, "04")) strcpy(pstr, "4\0");
        if(streqv(upstr, "05")) strcpy(pstr, "5\0");
        if(streqv(upstr, "06")) strcpy(pstr, "6\0");
        if(streqv(upstr, "07")) strcpy(pstr, "7\0");
        if(streqv(upstr, "08")) strcpy(pstr, "8\0");
        if(streqv(upstr, "09")) strcpy(pstr, "9\0");

        if(streqv(upstr, "10")) strcpy(pstr, "A\0");
        if(streqv(upstr, "11")) strcpy(pstr, "B\0");
        if(streqv(upstr, "12")) strcpy(pstr, "C\0");
        if(streqv(upstr, "13")) strcpy(pstr, "D\0");
        if(streqv(upstr, "14")) strcpy(pstr, "E\0");
        if(streqv(upstr, "15")) strcpy(pstr, "F\0");
        if(streqv(upstr, "16")) strcpy(pstr, "G\0");
        if(streqv(upstr, "17")) strcpy(pstr, "H\0");
        if(streqv(upstr, "18")) strcpy(pstr, "I\0");
        if(streqv(upstr, "19")) strcpy(pstr, "J\0");
        if(streqv(upstr, "20")) strcpy(pstr, "K\0");
        if(streqv(upstr, "21")) strcpy(pstr, "L\0");
        if(streqv(upstr, "22")) strcpy(pstr, "M\0");
        if(streqv(upstr, "23")) strcpy(pstr, "N\0");
        if(streqv(upstr, "24")) strcpy(pstr, "O\0");
        if(streqv(upstr, "25")) strcpy(pstr, "P\0");
        if(streqv(upstr, "26")) strcpy(pstr, "Q\0");
        if(streqv(upstr, "27")) strcpy(pstr, "R\0");
        if(streqv(upstr, "28")) strcpy(pstr, "S\0");
        if(streqv(upstr, "29")) strcpy(pstr, "T\0");
        if(streqv(upstr, "30")) strcpy(pstr, "U\0");
        if(streqv(upstr, "31")) strcpy(pstr, "V\0");

        return 0;
}

int packString(QString upStr, QString *pStr, int targLen)
{
    int i, j;
    char *pstr = new char[2];
    char *upstr = new char[2];
    pStr->clear();
    int iLen = upStr.size();
    int dLen = iLen - targLen;
    for(i=dLen; i<0; i++) pStr->append("0");
    for(i=0;i<dLen;i++)
    {
        j= i*2;
        strncpy(upstr, &upStr.toAscii().constData()[j], 2);
        strncpy(&upstr[2], "\0", 1);
        qDebug() << "upstr:" << upstr << "\n";
        packDigStr(upstr, pstr);
        qDebug() << "pstr:" << pstr << "\n";
        pStr->append(QByteArray(pstr));
    }
    pStr->append(upStr.right(iLen-j-2));
    //for(i=j; i<iLen; i++)

    return 0;
}

int unpackString(QString *upStr, QString pStr)
{
    upStr->clear();
    char *fstr = new char[2];
    char *ustr = new char[2];

    int sz = pStr.size();
    upStr->clear();
    for(int i=0; i<sz; i++)
    {
        strncpy(ustr, "\0", 1);
        strncpy(ustr, &pStr.toAscii().constData()[i], 1);
        strncpy(&ustr[1], "\0", 1);
        upackDigStr(ustr, fstr);
        upStr->append(fstr);
    }

    delete [] fstr;
    delete [] ustr;

    return 0;
}


int sign(double x)
{
	if(fabs(x)>EPS*1.e-5) return(fabs(x)/x);
	return 0;
}

int hms2rad(double *rad, int sn, int h, int m, double s)
{
	*rad = ((s/60.0 + m*1.0)/60.0 + h*1.0)*15.0*PI/180.0;
	if(!sn) *rad = -*rad;
	return 0;
}

double hms2grad(int h, int m, double s)
{
        return(((s/60.0 + m*1.0)/60.0 + h*1.0)*15.0);
}

void hms2rad(double *rad, char* str)
{
	char *slo = new char[32];
	int posb = 0;
	int pose;
	slovo_stopsim(str, slo, " ", posb, &pose, 5);
	int y = atoi(slo);
	posb = ++pose;
	slovo_stopsim(str, slo, " ", posb, &pose, 3);
	int m = atoi(slo);
	posb = ++pose;
	slovo_stopsim(str, slo, " ", posb, &pose, 30);
	double d = atoi(slo);
	hms2rad(rad, 1, y, m, d);
}

double hms2rad(char* str)
{
	char *slo = new char[32];
	int posb = 0;
	int pose;
	slovo_stopsim(str, slo, " ", posb, &pose, 5);
	int y = atoi(slo);
	posb = ++pose;
	slovo_stopsim(str, slo, " ", posb, &pose, 3);
	int m = atoi(slo);
	posb = ++pose;
	slovo_stopsim(str, slo, " ", posb, &pose, 30);
	double d = atoi(slo);
	double rad;
	hms2rad(&rad, 1, y, m, d);
	return rad;
}

int gms2rad(double *rad, int sn, int g, int m, double s)
{
	*rad = ((s/60.0 + m*1.0)/60.0 + g*1.0)*PI/180.0;
	if(!sn) *rad = -*rad;
	return 0;
}

double gms2grad(int plus, int g, int m, double s)
{
        double grad = ((s/60.0 + m*1.0)/60.0 + g*1.0);
        if(!plus) grad *= -1.0;
        return grad;
}

void gms2rad(double *rad, char *str)
{
	char *slo = new char[32];
	int posb = 0;
	int pose;
	slovo_stopsim(str, slo, " ", posb, &pose, 5);
	int y = atoi(slo);
	int sn = y>0;
	y = abs(y);
	posb = ++pose;
	slovo_stopsim(str, slo, " ", posb, &pose, 3);
	int m = atoi(slo);
	posb = ++pose;
	slovo_stopsim(str, slo, " ", posb, &pose, 30);
	double d = atoi(slo);
	gms2rad(rad, sn, y, m, d);
}

double gms2rad(char *str)
{
	double res;

	gms2rad(&res, str);
	return res;
}


int rad2hms(double rad, int *sn, int *h, int *m, double *s)
{
	*sn = (sign(rad)==1.0);
	rad = fabs(rad);
	rad *= 180.0/PI/15.0;

	*h = rad/1;
	rad -= *h;

	rad *= 60.0;
	*m = rad/1;
	rad -= *m;

	rad*= 60.0;
	*s = rad;

	if(*s>=59.999)
	{
		*s = 0.0;
		*m+=1;
	}

	if(*m>=60)
	{
		*m -= 60;
		*h+=1;
	}

	if(*h>=24) *h -= 24;

	return 0;
}

int rad2gms(double rad, int *sn, int *g, int *m, double *s)
{
	*sn = (sign(rad)==1.0);
	rad = fabs(rad);
	rad *= 180.0/PI;

	*g = rad/1;
	rad -= *g;

	rad *= 60.0;
	*m = rad/1;
	rad -= *m;

	rad*= 60.0;
	*s = rad;


	if(*s>=59.999)
	{
		*s = 0.0;
		*m+=1;
	}

	if(*m>=60)
	{
		*m -= 60;
		*g+=1;
	}

//	if(*h>=24) *h -= 24;

	return 0;
}

int index(char *sInd, char *sIni, char *fil, int len)
{
	int nlen;

	nlen = strlen(sIni);

	strcpy(sInd, "");

	for(int i=0; i<len-nlen; i++)
	{
		strcpy(&sInd[i], fil);
		
	}

	strcat(sInd, sIni);
	strcpy(&sInd[len], "\0");

	return 0;
}

int indexB(char *sInd, char *sIni, char *fil, int len)
{
	int nlen;

	nlen = strlen(sIni);

	strcpy(sInd, "");

	strcat(sInd, sIni);

	for(int i=nlen; i<len; i++)
	{
		strcpy(&sInd[i], fil);
		
	}

//	strcat(sInd, sIni);
	strcpy(&sInd[len], "\0");

	return 0;
}

double fatan(double sinx, double cosx)
{
	double x;

	x = atan(sinx/cosx);

	if((sinx>0.0)&&(cosx<0.0)) x = PI - fabs(x);
	if((sinx<0.0)&&(cosx<0.0)) x = PI + fabs(x);
	if((sinx<0.0)&&(cosx>0.0)) x = 2.0*PI - fabs(x);

	return x;
}


int rdsys(double *aVrad, double *dVrad, double P1, double P2, double P3)
{
    *aVrad = atan2(P2, P1);
    if(*aVrad<0.0) *aVrad += 2.0*PI;
            if(*aVrad>2.0*PI) *aVrad -= 2.0*PI;
            *dVrad = atan2(P3, sqrt(P1*P1 + P2*P2));
    if(*dVrad>PI/2.0) *dVrad -= PI;
    if(*dVrad<-PI/2.0) *dVrad += PI;

    return 0;
}


int piNsys(double *p, double *inc, double *Node, double P1, double P2, double P3)
{
    *Node = atan2(P1, P2);
    *inc = atan2(P2, P3*cos(*Node));
    if(*Node<0.0) *Node+=2.0*PI;
    if(*inc<0.0) *inc += PI;

    *p = pow(P3/(ka*cos(*inc)), 2.0);

	return 0;
}

int Ekv2Ecl(double *eclinc, double *eclNode, double *eclPer, double ekvinc, double ekvNode, double ekvPer)
{
	double se, ce, d;
	double P1, P2, P3, P4, P5;

	se = sin(EKV);
	ce = cos(EKV);

	P1 = sin(ekvinc)*sin(ekvNode);
	P2 = -cos(ekvinc)*se + sin(ekvinc)*ce*cos(ekvNode);
	P3 = cos(ekvinc)*ce + sin(ekvinc)*se*cos(ekvNode);
	P4 = se*sin(ekvNode);
	P5 = sin(ekvinc)*ce - cos(ekvinc)*se*cos(ekvNode);

	*eclNode = atan2(P1, P2);
	*eclinc = atan2(P1, P3*sin(*eclNode));
	if(*eclNode<0.0) *eclNode += 2.0*PI;
	if(*eclinc<0.0) *eclinc += PI;//-*eclinc;

	d = atan2(P4, P5);
	if(d<0.0) d +=2.0*PI;

	*eclPer = ekvPer - d;

	return 0;
}

int Ecl2Ekv(double *ekvinc, double *ekvNode, double *ekvPer, double eclinc, double eclNode, double eclPer)
{
	double se, ce, d;
	double P1, P2, P3, P4, P5;

	se = sin(EKV);
	ce = cos(EKV);

	P1 = sin(eclinc)*sin(eclNode);
	P2 = cos(eclinc)*se + sin(eclinc)*ce*cos(eclNode);
	P3 = cos(eclinc)*ce - sin(eclinc)*se*cos(eclNode);
	P4 = se*sin(eclNode);
	P5 = sin(eclinc)*ce + cos(eclinc)*se*cos(eclNode);

	*ekvNode = atan2(P1, P2);
	if(*ekvNode<0.0) *ekvNode += 2.0*PI;
	*ekvinc = atan2(P1, P3);
	if(*ekvinc<0.0) *ekvinc = -*ekvinc;

	d = atan2(P4, P5);

	*ekvPer = eclPer + d;

	return 0;
}

int word2one(char *word, char *one)
{
	int len = strlen(word);

	for(int i=0; i<len; i++)
	{
		if(strncmp(&word[i], "0", 1)!=0) strcpy(&one[i], "1");
		else strcpy(&one[i], "0");
	}

	return 0;
}

int eKepler(double *E, double M, double ec)
{
	double Et, Ett;
	Et = M;

	do
	{
		Ett = Et;
		Et = M + ec*sin(Et);
	}while(fabs(Et-Ett)>EPS);

	*E = Et;
	return 0;
}

int pKepler(double *s, double B)
{
	double ctgb, tgg, b, g;
	
	ctgb = 3.0*ka*B/(2.0*sqrt(2.0));
	b = atan2(1, ctgb);
	tgg = pow(tan(b/2.0), 1.0/3.0);
	g = atan(tgg);

	*s = 2.0/tan(2.0*g);

	return 0;
}

int npKepler(double *s, double B, double ecc)
{
	double ctgb, tgg, b, g;
	double sg0, sg, ksi, K, eps, B0;

	eps = (1.0-ecc)/2.0;

	pKepler(&sg, B);

	do
	{
		sg0 = sg;
		ksi = eps*sg0*sg0;
		B0 = sg0*npU(ksi) + sg0*sg0*sg0*npV(ksi);
		K = (ka*ka/2.0)*((npU(ksi))/(1.0+ecc*sg0*sg0));
		sg = K*(B-B0) + sg0;
	}
	while(fabs(sg-sg0)>EPS);
	
	*s = sg;

	return 0;
}

double npU(double ksi)
{
	return(sqrt(2.0)*sqrt(1.0 - ksi)/ka);
}

double npG(double g)
{
	return((2.0*g-sin(2.0*g))/(4.0*pow(sin(g), 3.0)));
}

double npV(double ksi)
{
	if(fabs(ksi)<EPS) ksi = EPS;
	return(sqrt(2.0)*npG(asin(sqrt(ksi)))/ka);
}

int gKepler(double *H, double M, double ec)
{
	double Et, Ett;
	Et = fabs(M);

	do
	{
		Ett = Et;
		Et = ec*sinh(Et) - M;
	}while(fabs(Et-Ett)>EPS);

	*H = Et;
	return 0;
}

int sqEqv(double A, double B, double C, double *x1, double *x2)
{
	double D;

	D = B*B - 4.0*A*C;
	if(D<-EPS)
	{
		*x1 = -B/(2.0*A);
		*x2 = sqrt(-D)/(2.0*A);
		return 2;
	}
	else if(fabs(D)<EPS)
	{
		*x1 = *x2 = -B/(2.0*A);
		return 1;
	}
	else
	{
		*x1 = (-B + sqrt(D))/(2.0*A);
		*x2 = (-B - sqrt(D))/(2.0*A);
	}

	return 0;
}

int fakt(int n)
{
	double res;
	res = 1.0;
	for(int i=0; i<n; i++)
	{
		res *= i+1.0;
	}
	return res;
}

int wTable(char* str)
{
	if((strstr(str, "A")!=NULL)||(strstr(str, "a")!=NULL)) return 11;
	if((strstr(str, "B")!=NULL)||(strstr(str, "b")!=NULL)) return 12;
	if((strstr(str, "C")!=NULL)||(strstr(str, "c")!=NULL)) return 13;
	if((strstr(str, "D")!=NULL)||(strstr(str, "d")!=NULL)) return 14;
	if((strstr(str, "E")!=NULL)||(strstr(str, "e")!=NULL)) return 15;
	if((strstr(str, "F")!=NULL)||(strstr(str, "f")!=NULL)) return 16;
	if((strstr(str, "G")!=NULL)||(strstr(str, "g")!=NULL)) return 17;
	if((strstr(str, "H")!=NULL)||(strstr(str, "h")!=NULL)) return 18;
	if((strstr(str, "I")!=NULL)||(strstr(str, "i")!=NULL)) return 19;
	if((strstr(str, "J")!=NULL)||(strstr(str, "j")!=NULL)) return 20;
	if((strstr(str, "K")!=NULL)||(strstr(str, "k")!=NULL)) return 21;
	if((strstr(str, "L")!=NULL)||(strstr(str, "l")!=NULL)) return 22;
	if((strstr(str, "M")!=NULL)||(strstr(str, "m")!=NULL)) return 23;
	if((strstr(str, "N")!=NULL)||(strstr(str, "n")!=NULL)) return 24;
	if((strstr(str, "O")!=NULL)||(strstr(str, "o")!=NULL)) return 25;
	if((strstr(str, "P")!=NULL)||(strstr(str, "p")!=NULL)) return 26;
	if((strstr(str, "Q")!=NULL)||(strstr(str, "q")!=NULL)) return 27;
	if((strstr(str, "R")!=NULL)||(strstr(str, "r")!=NULL)) return 28;
	if((strstr(str, "S")!=NULL)||(strstr(str, "s")!=NULL)) return 29;
	if((strstr(str, "T")!=NULL)||(strstr(str, "t")!=NULL)) return 30;
	if((strstr(str, "U")!=NULL)||(strstr(str, "u")!=NULL)) return 31;
	if((strstr(str, "V")!=NULL)||(strstr(str, "v")!=NULL)) return 32;
	if((strstr(str, "W")!=NULL)||(strstr(str, "w")!=NULL)) return 33;
	if((strstr(str, "X")!=NULL)||(strstr(str, "x")!=NULL)) return 34;
	if((strstr(str, "Y")!=NULL)||(strstr(str, "y")!=NULL)) return 35;
	if((strstr(str, "Z")!=NULL)||(strstr(str, "z")!=NULL)) return 36;

	return(atoi(str));
}

void UwTable(char* str, int i)
{
	div_t dt;
	if(i<1000) sprintf(str, "%d", i);
	else
	{
		dt = div(i, 100);

		sprintf(str, "%c%d", dt.quot + 54, dt.rem);
	}
}

int str2int(char *str)
{
	int len, i;
	char *temp, *sim, *slo;

	len = strlen(str);

	temp = new char[len];
	sim = new char[2];
	slo = new char[1];

	strcpy(temp, "");
	strcpy(sim, "");

	for(i=0; i<len; i++)
	{
		slovoG(str, slo, i, i+1);
		sprintf(sim, "%d", wTable(slo));
		strcat(temp, sim);
		strcpy(sim, "");
		strcpy(slo, "");
	}

	return(atoi(temp));

//	printf("%d\n", str);

	return 0;
}

int int2str(char *str, int i)
{
	UwTable(str, i);

	return 0;
}
/*
int RotZ(myvector *x, myvector *x0, double angr)
{
	myvector *xt;
	xt = new myvector(3);

	xt->set(0, x0->get(0));
	xt->set(1, x0->get(1));
	xt->set(2, x0->get(2));

	x->set(0, cos(angr)*xt->get(0) - sin(angr)*xt->get(1));
	x->set(1, sin(angr)*xt->get(0) + cos(angr)*xt->get(1));
	x->set(2, xt->get(2));

	return 0;
}

int RotX(myvector *x, myvector *x0, double angr)
{
	myvector *xt;
	xt = new myvector(3);

	xt->set(0, x0->get(0));
	xt->set(1, x0->get(1));
	xt->set(2, x0->get(2));

	x->set(0, xt->get(0));
	x->set(1, cos(angr)*xt->get(1) - sin(angr)*xt->get(2));
	x->set(2, sin(angr)*xt->get(1) + cos(angr)*xt->get(2));

	return 0;
}
*/
logstuff::logstuff()
{
	this->FLOG = NULL;
	this->suff = new char[255];
	strcpy(suff, "");
}

logstuff::~logstuff()
{
	fclose(this->FLOG);
	this->FLOG = NULL;
	delete suff;
	this->suff = NULL;
}

int streqv(char *str1, char *str2)
{
	int len1 = strlen(str1);
	int len2 = strlen(str2);
	if(len1!=len2) return 0;
	for(int i=0; i<len1; i++)
	{
                if(strcmp(&str1[i], &str2[i])!=0) return 0;
	}

	return 1;
}

int streqv1(char *str, char *strBase)
{
	int i;
	int len = strlen(str);
	int lenB = strlen(strBase);
	if(len<lenB) return 0;
	for(i=0; i<lenB; i++)
	{
                if(strncmp(&str[i], &strBase[i], 1)!=0) return i;
	}

	return lenB;
}

/*
int slovoG(char *str, char *slo, int pos1, int pos2)
{
	strcpy(slo, "");

	strncpy(slo, &str[pos1], pos2-pos1);
	strcpy(&slo[pos2-pos1], "\0");

 	return 0;
}
*/

int slovo(char *str, char *slo, int posb, int *pose)
{
	char *sim;
	sim = new char[2];

	strcpy(slo, "");
	strcpy(sim, "");

	int len = strlen(str);

	int i = posb-1;

	do
	{
		i++;

		strcat(slo, sim);

		strncpy(sim, &str[i], 1);
		strcpy(&sim[1], "\0");

	}while((i<len)&&(streqv(sim, " ")));

	if(i>=len-1) return 1;

	strcpy(slo, "");
	strcpy(sim, "");

	do
	{
		strcat(slo, sim);

		strncpy(sim, &str[i], 1);
		strcpy(&sim[1], "\0");

		i++;
	}while((i<=len)&&(!streqv(sim, " "))&&(!streqv(sim, "\n")));

	*pose = i;

	delete [] sim;

	return 0;
}

int slovo_stopsim(char *str, char *slo, char* stopsim, int posb, int *pose, int nmax)
{
/*
	str - пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ
	slo - пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
	stopsom - пїЅпїЅпїЅпїЅпїЅпїЅ-пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
	posb - пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅпїЅпїЅ
	pose - пїЅпїЅпїЅпїЅпїЅпїЅпїЅ, пїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ-пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
	nmax - пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ slo
*/
	char *sim;
	sim = new char[2];

	strcpy(slo, "");
	strcpy(sim, "");

	int len = strlen(str);

	int i = posb-1;
	int k=0;

	do
	{
		i++;

//		strcat(slo, sim);

		strncpy(sim, &str[i], 1);
		strcpy(&sim[1], "\0");

		k++;
	}while((k<=nmax)&&(i<len)&&(streqv(sim, stopsim)));

	do
	{
		i++;

		strcat(slo, sim);

		strncpy(sim, &str[i], 1);
		strcpy(&sim[1], "\0");

		k++;
	}while((k<=nmax)&&(i<len)&&(!streqv(sim, stopsim)));

	if(i>=len-1) return 1;

	*pose = i;

	delete [] sim;

	return 0;
}

void det_suff(char *fn, char *suff)
{
	if(suff==NULL) return;
	int len = strlen(fn);
//	for(int i=0; i<128; i++) printf("%d %c\n", i, i);
	char *pos = strrchr(fn, 46);
	int len1 = strlen(pos);
	strcpy(suff, "");
	strncpy(suff, fn, len-len1);
	strcpy(&suff[len-len1], "\0");

	return;
}


int chet(int val)
{
	double tval, tval1;

	tval = val/2;
	tval1 = val/2.0;

	if(fabs(tval-tval1)<EPS) return 1;

	return 0;
}

double round( double val, int signs ) //[1]
{
double p = pow( 10., signs );
return floor( val * p + .5 ) / p;
}

/*
double UTC2TDB(double jdUTC)
{
    double jdTDT;
    jdTDT = UTC2TDT(jdUTC);
    double T=(jdTDT-2451545)/36525.0;
    double g = (357.528 + 35999.050*T)*2.0*PI/360.0;
    return(jdTDT + 0.001658*sin(g+0.0167*sin(g))/86400.0);
}

double UTC2TDT(double jdUTC)
{
    return(jdUTC + TAImUTC(jd2mjd(jdUTC)) + 32.184/86400.0);
}

double TDT2UTC(double jdTDT)
{
    return(jdTDT - TAImUTC(jd2mjd(jdTDT)) - 32.184/86400.0);
}

double TDB2TDT(double jdTDB)
{
    double T=(jdTDB-2451545)/36525.0;
    double g = (357.528 + 35999.050*T)*2.0*PI/360.0;
    double res = jdTDB - 0.001658*sin(g+0.0167*sin(g))/86400.0;
    return(res);
}

double TDT2TDB(double jdTDT)
{
    double T=(jdTDT-2451545)/36525.0;
    double g = (357.528 + 35999.050*T)*2.0*PI/360.0;
    double res = jdTDT + 0.001658*sin(g+0.0167*sin(g))/86400.0;
    return(res);
}

double TDB2UTC(double jdTDB)
{
    return(TDT2UTC(TDB2TDT(jdTDB)));
}

double TDB2TT(double jdTDB)
{
    double T=(jdTDB-2451545)/36525;
    double g = (357.528 + 35999.050*T)*2.0*PI/360.0;
    return(jdTDB - 0.001658*sin(g+0.0167*sin(g))/86400.0);
}

double dUT1() {return -0.39;}

double TAImUTC(double mjd) // [day]
{
    double res;

    if(mjd>=jd2mjd(2437300.5)&&mjd<jd2mjd(2437512.5)) res = 1.4228180 + (mjd - 37300.0)*0.001296;
    if(mjd>=jd2mjd(2437512.5)&&mjd<jd2mjd(2437665.5)) res = 1.3728180 + (mjd - 37300.0)*0.001296;
    if(mjd>=jd2mjd(2437665.5)&&mjd<jd2mjd(2438334.5)) res = 1.8458580 + (mjd - 37665.0)*0.0011232;
    if(mjd>=jd2mjd(2438334.5)&&mjd<jd2mjd(2438395.5)) res = 1.9458580 + (mjd - 37665.0)*0.0011232;
    if(mjd>=jd2mjd(2438395.5)&&mjd<jd2mjd(2438486.5)) res = 3.2401300 + (mjd - 38761.0)*0.001296;
    if(mjd>=jd2mjd(2438486.5)&&mjd<jd2mjd(2438639.5)) res = 3.3401300 + (mjd - 38761.0)*0.001296;
    if(mjd>=jd2mjd(2438639.5)&&mjd<jd2mjd(2438761.5)) res = 3.4401300 + (mjd - 38761.0)*0.001296;
    if(mjd>=jd2mjd(2438761.5)&&mjd<jd2mjd(2438820.5)) res = 3.5401300 + (mjd - 38761.0)*0.001296;
    if(mjd>=jd2mjd(2438820.5)&&mjd<jd2mjd(2438942.5)) res = 3.6401300 + (mjd - 38761.0)*0.001296;
    if(mjd>=jd2mjd(2438942.5)&&mjd<jd2mjd(2439004.5)) res = 3.7401300 + (mjd - 38761.0)*0.001296;
    if(mjd>=jd2mjd(2439004.5)&&mjd<jd2mjd(2439126.5)) res = 3.8401300 + (mjd - 38761.0)*0.001296;
    if(mjd>=jd2mjd(2439126.5)&&mjd<jd2mjd(2439887.5)) res = 4.3131700 + (mjd - 39126.0)*0.002592;
    if(mjd>=jd2mjd(2439887.5)&&mjd<jd2mjd(2441317.5)) res = 4.2131700 + (mjd - 39126.0)*0.002592;
    if(mjd>=jd2mjd(2441317.5)&&mjd<jd2mjd(2441499.5)) res = 10.0;
    if(mjd>=jd2mjd(2441499.5)&&mjd<jd2mjd(2441683.5)) res = 11.0;
    if(mjd>=jd2mjd(2441683.5)&&mjd<jd2mjd(2442048.5)) res = 12.0;
    if(mjd>=jd2mjd(2442048.5)&&mjd<jd2mjd(2442413.5)) res = 13.0;
    if(mjd>=jd2mjd(2442413.5)&&mjd<jd2mjd(2442778.5)) res = 14.0;
    if(mjd>=jd2mjd(2442778.5)&&mjd<jd2mjd(2443144.5)) res = 15.0;
    if(mjd>=jd2mjd(2443144.5)&&mjd<jd2mjd(2443509.5)) res = 16.0;
    if(mjd>=jd2mjd(2443509.5)&&mjd<jd2mjd(2443874.5)) res = 17.0;
    if(mjd>=jd2mjd(2443874.5)&&mjd<jd2mjd(2444239.5)) res = 18.0;
    if(mjd>=jd2mjd(2444239.5)&&mjd<jd2mjd(2444786.5)) res = 19.0;
    if(mjd>=jd2mjd(2444786.5)&&mjd<jd2mjd(2445151.5)) res = 20.0;
    if(mjd>=jd2mjd(2445151.5)&&mjd<jd2mjd(2445516.5)) res = 21.0;
    if(mjd>=jd2mjd(2445516.5)&&mjd<jd2mjd(2446247.5)) res = 22.0;
    if(mjd>=jd2mjd(2446247.5)&&mjd<jd2mjd(2447161.5)) res = 23.0;
    if(mjd>=jd2mjd(2447161.5)&&mjd<jd2mjd(2447892.5)) res = 24.0;
    if(mjd>=jd2mjd(2447892.5)&&mjd<jd2mjd(2448257.5)) res = 25.0;
    if(mjd>=jd2mjd(2448257.5)&&mjd<jd2mjd(2448804.5)) res = 26.0;
    if(mjd>=jd2mjd(2448804.5)&&mjd<jd2mjd(2449169.5)) res = 27.0;
    if(mjd>=jd2mjd(2449169.5)&&mjd<jd2mjd(2449534.5)) res = 28.0;
    if(mjd>=jd2mjd(2449534.5)&&mjd<jd2mjd(2450083.5)) res = 29.0;
    if(mjd>=jd2mjd(2450083.5)&&mjd<jd2mjd(2450630.5)) res = 30.0;
    if(mjd>=jd2mjd(2450630.5)&&mjd<jd2mjd(2451179.5)) res = 31.0;
    if(mjd>=jd2mjd(2451179.5)&&mjd<jd2mjd(2453736.5)) res = 32.0;
    if(mjd>=jd2mjd(2453736.5)&&mjd<jd2mjd(2454832.5)) res = 33.0;
    if(mjd>=jd2mjd(2454832.5)&&mjd<jd2mjd(2456109.5)) res = 34.0;
    if(mjd>=jd2mjd(2456109.5)) res = 35.0;

    return(res/86400.0);
}
*/

int UTC2s_alt(double utc, double lam, double *s)
{
	double ut1, S0m, LNode, S;
	double T;

//	ut1 = utc + dUT1()/86400.0;

	T = (utc - T0J2000)/36525;

	double A, B, C, D;

	A = 24110.54841;
	B = 8640184.812866;
	C = 0.093104;
	D = -6.2e-6;

	S0m = A + (B + C*T + D*T*T)*(utc - T0J2000);

	S0m -= 86400.0*((int)(S0m/86400.0));

//	S0m *= 15.0/SECINRAD;

	double dT, dSm, Sm;

	double utcT, eJD;
	utcT=utc*2.0;
	utcT-=chet(utcT);
	eJD = ((double)(int)utcT)/2.0;
	utcT = utc - eJD;

//utc[JD]->sec
	utcT *= 86400.0;

	dT = (utcT + dUT1());
//	dT = utcT;

	A  = 1.002737909350795;
	B = 5.9006e-11;
	C = -5.9e-15;

	dSm = (A + B*T + C*T*T)*dT;

	Sm = S0m + dSm;
/*
	LNode = detLNode(T);

	S = Sm - (6.857/15.0)*sin(LNode)/tan(EKV);
*/
	S = Sm;
	
//WARN

//	if (S>86400){S=S-86400;}
	*s = (S/86400.0)*2.0*PI + lam;

//	*s = S/13750.9870831397570104 + lam;	//пїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅ 13000???

//	*s = S/13713.4422575944044781 + lam;

//	*s = S/206265.0 + lam;

	if(*s>2.0*PI) *s -= 2.0*PI;
	if(*s<0.0) *s += 2.0*PI;

	return 0;
}

int UTC2s(double utc, double lam, double *s)    //[utc] = day; [lam] = rad; [s] = rad
{
	double d_day, eJD;
	d_day=utc*2.0;
	d_day-=chet(d_day);
	eJD = ((double)(int)d_day)/2.0;
	d_day = utc - eJD;

	double JD = eJD;
//utc[JD]->sec
	d_day *= 86400.0;

//	double d_day;
	
	double T=(JD-2451545)/36525;
	//double S0=6*3600+41*60+50.54841+8640184.812866*T+0.093104*T*T-0.0000062*T*T*T;
	double S0=6*3600+41*60+50.54841+236.555367908*(JD-2451545)+0.093104*T*T-0.0000062*T*T*T;
	S0=S0-floor(S0/86400)*86400;
	double S = S0+d_day*366.25/365.25+7278;
	if (S>86400){S=S-86400;}
        *s = (S/86400.0)*2.0*PI;
        *s+=lam;
//	if(*s>2.0*PI) *s -= 2.0*PI;
//	if(*s<0.0) *s += 2.0*PI;

	return 0;
}

double gmst0(double T)//day
{
    double g0 = (6*3600+41*60+50.54841)/SECINDAY+(8640184.812866/SECINDAY)*T+(0.093104/SECINDAY)*T*T-(0.0000062/SECINDAY)*T*T*T;
    g0 = g0 - (int)g0;
    if(g0<0) g0+=1.0;
    if(g0>1.0) g0 -= 1.0;
    return(g0);
}

double pdayUT1(double jdUT1, double *pday)
{
    double d_day, jd0;
    d_day=jdUT1*2.0;
    d_day-=chet(d_day);
    jd0 = ((double)(int)d_day)/2.0;

    *pday = jdUT1 - jd0;
    return jd0;
}

double detT(double jd0)
{
    return((jd0-2451545)/36525.0);
}

double gmst0jd(double jd0)
{
    double gm0 = gmst0(detT(jd0));
    gm0 = gm0 - (int)gm0;
    if(gm0<0)gm0 += 1.0;
    return(gm0);
}



double gmst1(double ut1, double g0)
{
    double gm1 = g0 + ut1 + MIU*ut1;
    gm1 = gm1 - (int)gm1;
    if(gm1<0)gm1 += 1.0;
    if(gm1>1.0) gm1 -= 1.0;
    return(gm1);
}

void jdUT1_to_GMST1(double *gm1, double jdUT1)
{
    double ut1;
    double jd0 = pdayUT1(jdUT1, &ut1);
    //printf("jd0= %f\tT= %f\tut1= %f\n", jd0, detT(jd0), ut1);
    double gm0 = gmst0jd(jd0);
    //printf("gm0 %f\n", gm0);//deg_to_hms(rad2grad(gm0), " ", 2));
    *gm1 = gmst1(ut1, gm0);
    //printf("gm0= %f\tgm1= %f\n", gm0, *gm1);
    *gm1 = *gm1 - (int)(*gm1);
    //printf("gm1= %f\n", *gm1);
    if(*gm1<0.0) *gm1 += 1.0;
    //printf("gm1= %f\n", *gm1);
    //returm(gm1);
}

void GMST1_to_jdUT1(double *jdUT1, double gm1, double jd0)//[jd0] = day; [gm1] = day; [jdUT1] = day(0UT1)
{
    double gm0 = gmst0jd(jd0);
    //qDebug() << QString("jd0= %1\tgm0= %2\n").arg(jd0, 12, 'f', 4).arg(gm0);
   // gm0 = gm0 - (int)gm0;
    //qDebug() << QString("gm0= %1\n").arg(gm0);
    double dgm = gm1 - gm0;
    //qDebug() << QString("dgm= %1\n").arg(dgm);
    if(dgm<0.0) dgm += 1.0;
    //qDebug() << QString("dgm= %1\n").arg(dgm);
    *jdUT1 = dgm - VIU*dgm;
    //qDebug() << QString("jdUT1= %1\n").arg(*jdUT1, 12, 'f', 4);
    //if(*jdUT1<0) *jdUT1 +=1.0;
    //if(*jdUT1>1) *jdUT1 -=1.0;
    *jdUT1 += jd0;
}

int s2UTC(double s, double lam, double *utc)
{

    double d_day, eJD;
        d_day=s*2.0;
        d_day-=chet(d_day);
        eJD = ((double)(int)d_day)/2.0;
        d_day = (s - eJD)*2*PI; //s - rad
//        d_day = s*2.0*PI;        //[s] =rad
        double dS = d_day - lam;//GMST

//        double JD = eJD;
//utc[JD]->sec
     /*   d_day -= lam/2.0/PI;
        d_day *= 86400.0;*/

//	double d_day;

        double T=(eJD-2451545)/36525;
        //double S0=6*3600+41*60+50.54841+8640184.812866*T+0.093104*T*T-0.0000062*T*T*T;
        double GMST0= gmst0(T);//(6*3600+41*60+50.54841)/SECINDAY+(8640184.812866/SECINDAY)*T+(0.093104/SECINDAY)*T*T-(0.0000062/SECINDAY)*T*T*T;
        GMST0=GMST0-floor(GMST0);
        GMST0 *= 2.0*PI;
        double v = 0.0027304336;//0.997269566329084 - 5.8684e-11*T+5.9e-15*T*T;
        //printf("s=%f\tJD=%f\tdS=%f\tS0=%f\tv=%f\td_day=%f\tlam=%f\n", s, eJD, dS, GMST0, v, d_day, lam);
        double UT1 = dS - GMST0 - v*(dS - GMST0);
        //printf("UT1=%f\n", UT1);
        if(UT1<0.0) UT1 += 2.0*PI;
        if(UT1>2.0*PI) UT1 -= 2.0*PI;
        //if(UT1<0.0) UT1 = fabs(UT1);
        //printf("UT1=%f\n", UT1);
        UT1 /= 2.0*PI;
        *utc = eJD + UT1;
//        *utc = JD+(d_day-S0)*v/86400.0;
        /*double S = S0+d_day*366.25/365.25+7278;
        if (S>86400){S=S-86400;}
        *s = (S/86400.0)*2.0*PI;*/
        /*if(s>2.0*PI) s -= 2.0*PI;
        if(s<0.0) s += 2.0*PI;*/

        return 0;
}

int sJD2UT1(double utcJDay, double s, double lam, double *utc)
{

    double d_day, eJD;
  /*      d_day=s*2.0;
        d_day-=chet(d_day);
        eJD = ((double)(int)d_day)/2.0;
        d_day = (s - eJD)*2*PI; //s - rad*/
        d_day = s*2.0*PI;        //[s] = rad
        double dS = d_day - lam;//GMST
        double T, GMST0;

        double v = 0.0027304336;

//        double JD = eJD;
//utc[JD]->sec
     /*   d_day -= lam/2.0/PI;
        d_day *= 86400.0;*/

//	double d_day;

        T=(utcJDay-2451545)/36525.0;
        //printf("T=%f\n", T);
        //double S0=6*3600+41*60+50.54841+8640184.812866*T+0.093104*T*T-0.0000062*T*T*T;
        GMST0=(6*3600+41*60+50.54841)/SECINDAY+(8640184.812866/SECINDAY)*T+(0.093104/SECINDAY)*T*T-(0.0000062/SECINDAY)*T*T*T;
        //printf("GMST0=%f\n", GMST0);
        GMST0=GMST0+sign(GMST0)*floor(GMST0);
        //printf("GMST0=%f\n", GMST0);
        GMST0 *= 2.0*PI;
        //printf("GMST0=%f\n", GMST0);
        //0.997269566329084 - 5.8684e-11*T+5.9e-15*T*T;
        //printf("s=%f\tJD=%f\tdS=%f\tS0=%f\tv=%f\td_day=%f\tlam=%f\n", s, utcJDay, dS, GMST0, v, d_day, lam);
        if((dS-GMST0)<0.0)
        {
            printf("POVTOR\n");
            utcJDay+=1.0;
            T=(utcJDay-2451545)/36525.0;
            GMST0=(6*3600+41*60+50.54841)/SECINDAY+(8640184.812866/SECINDAY)*T+(0.093104/SECINDAY)*T*T-(0.0000062/SECINDAY)*T*T*T;
            printf("GMST0=%f\n", GMST0);
            GMST0=GMST0+sign(GMST0)*floor(GMST0);
            printf("GMST0=%f\n", GMST0);
            GMST0 *= 2.0*PI;
            printf("GMST0=%f\n", GMST0);
        }
        double UT1 = dS - GMST0 - v*(dS - GMST0);
        //printf("UT1=%f\n", UT1);
        if(UT1<0.0) UT1 += 2.0*PI;
        if(UT1>2.0*PI) UT1 -= 2.0*PI;
        //if(UT1<0.0) UT1 = fabs(UT1);
        //printf("UT1=%f\n", UT1);
        UT1 /= 2.0*PI;
        *utc = UT1;
//        *utc = JD+(d_day-S0)*v/86400.0;
        /*double S = S0+d_day*366.25/365.25+7278;
        if (S>86400){S=S-86400;}
        *s = (S/86400.0)*2.0*PI;*/
        /*if(s>2.0*PI) s -= 2.0*PI;
        if(s<0.0) s += 2.0*PI;*/

        return 0;
}

double detLNode(double T)
{
	return(2.18243919661567096 - (6962890.2665/SECINRAD)*T + (7.4722/SECINRAD)*T*T + (0.007702/SECINRAD)*T*T*T - (0.00005939/SECINRAD)*T*T*T*T);
}

double UT0JD(double utJD, double *part_day)
{
    double d_day, jd0;
    d_day=utJD*2.0;
    d_day-=chet(d_day);
    jd0 = ((double)(int)d_day)/2.0;
    d_day = (utJD - jd0);
    if(part_day!=NULL) *part_day = d_day;
    return jd0;
}



double VEll(double A, double B, double C)
{
	return(4.0*PI*A*B*C/3.0);
}

int redAber(double *x, double *y, double *z, double xt, double yt, double zt, double dt)
{
	*x += xt*dt;
	*y += yt*dt;
	*z += zt*dt;

	return 0;
}

double det_r(double Qmax, double D)
{
	return(D*tan(Qmax));
}

double detPhase(double X, double Y, double Z, double x, double y, double z)
{
    double *v1, *v2;
    v1 = new double[3];
    v2 = new double[3];

    v1[0] = x-X;
    v1[1] = y-Y;
    v1[2] = z-Z;

    v2[0] = x;
    v2[1] = y;
    v2[2] = z;

    double angl = VectAng(v1, v2);
    /*
	myvector *V1, *V2;
	V1 = new myvector(3);
	V2 = new myvector(3);

	V1->set(0, x-X);
	V1->set(1, y-Y);
	V1->set(2, z-Z);

	V2->set(0, x);
	V2->set(1, y);
	V2->set(2, z);

	double angl = VectAng(V1, V2);
*/
        delete []v1;
        delete [] v2;

	return(angl);
}

double VectAng(double *V1, double *V2)
{
        return(acos(Smul3(V1, V2)/(norm(V1)*norm(V2))));
}

double norm(double *v)
{
        double nm = 0.0;
        int len;
        len = 3;

        for(int i = 0; i<len; i++)
        {
                nm += v[i]*v[i];
        }

        return(sqrt(nm));
}

double Smul3(double *V1, double *V2)
{
/*	if(V2->getlength()!=V1->getlength())
        {
                return 1;
        }*/

        int i;
        double val;

        val = 0.0;
        for(i=0; i<3; i++)
        {
                val += V1[i]*V2[i];
        }
        return(val);
}

double detElong(double X, double Y, double Z, double x, double y, double z)
{
        double *V1, *V2;
        V1 = new double[3];
        V2 = new double [3];

        V1[0] = -X;
        V1[1] = -Y;
        V1[2] = -Z;

        V2[0] = x;
        V2[1] = y;
        V2[2] = z;

	double angl = VectAng(V1, V2);

        delete [] V1;
        delete [] V2;

	return(PI-angl);
}

void FFT(const double* dIn, double *dOut, int dSz, int nn, int beginData)
{
    int i, j, n, m, mmax, istep;
    double tempr, tempi, wtemp, theta, wpr, wpi, wr, wi;

    int isign = -1;
    double* data = new double[nn*2 + 1];

    j = 0;
    for (i = beginData; i < beginData + nn; i++)
    {
            if (i < dSz)
            {
                    data[j*2]   = 0;
                    data[j*2+1] = dIn[i];
            }
            else
            {
                    data[j*2]   = 0;
                    data[j*2+1] = 0;
            }
            j++;
    }

    n = nn << 1;
    j = 1;
    i = 1;
    while (i < n)
    {
            if (j > i)
            {
                    tempr = data[i];   data[i]   = data[j];   data[j]   = tempr;
                    tempr = data[i+1]; data[i+1] = data[j+1]; data[j+1] = tempr;
            }
            m = n >> 1;
            while ((m >= 2) && (j > m))
            {
                    j = j - m;
                    m = m >> 1;
            }
            j = j + m;
            i = i + 2;
    }
    mmax = 2;
    while (n > mmax)
    {
            istep = 2 * mmax;
            theta = 2.0*PI / (isign * mmax);
            wtemp = sin(0.5 * theta);
            wpr   = -2.0 * wtemp * wtemp;
            wpi   = sin(theta);
            wr    = 1.0;
            wi    = 0.0;
            m    = 1;
            while (m < mmax)
            {
                    i = m;
                    while (i < n)
                    {
                            j         = i + mmax;
                            tempr     = wr * data[j] - wi * data[j+1];
                            tempi     = wr * data[j+1] + wi * data[j];
                            data[j]   = data[i] - tempr;
                            data[j+1] = data[i+1] - tempi;
                            data[i]   = data[i] + tempr;
                            data[i+1] = data[i+1] + tempi;
                            i         = i + istep;
                    }
                    wtemp = wr;
                    wr    = wtemp * wpr - wi * wpi + wr;
                    wi    = wi * wpr + wtemp * wpi + wi;
                    m     = m + 2;
            }
            mmax = istep;
    }
//    double* dOut = new double[nn / 2];

    for (i = 0; i < (nn / 2); i++)
    {
            dOut[i] = sqrt( data[i*2] * data[i*2] + data[i*2+1] * data[i*2+1] );
    }

//    return dOut;

}

/*
double VectAng(myvector *V1, myvector *V2)
{
	return(acos(Smul(V1, V2)/(V1->norm()*V2->norm())));
}
/*
int IntAlongCurv(myvector *C, myvector *A, myvector *B, double ca, double cb, int direct)
{
	double sg, sga, sgb, cosA, sinA;
	double sinS, cosS;
	int res = 0;

	myvector *tmp;
	tmp = new myvector(3);
//	tmp = 

	sg = acos(Smul(A, B)/Smul(A, A)/Smul(B, B));
//	printf("sg = %15.13f = ", sg);
	sg = VectAng(A, B);
//	printf("%15.13f\n", sg);
	sinS = sin(sg);
	cosS = cos(sg);
	sga = acos(ca);
	sgb = acos(cb);

	if(sga+sgb<sg) res += 1;
	if(fabs(sin(sg))<EPS) res += 10;
	if(fabs(sg)<EPS||fabs(sg-PI)<EPS) res += 100;
	
	myvector *Q;
	Q = new myvector(3);
	myvector *P;
	P = new myvector(3);

	*Q = Vmul(A, B);

//	double nQ = Q->norm();

	Q->set(0, Q->get(0)/sinS);
	Q->set(1, Q->get(1)/sinS);
	Q->set(2, Q->get(2)/sinS);

//	nQ = Q->norm();

	*P = Vmul(Q, A);

//	double nP = P->norm();
/*
	P->set(0, (B->get(0) - A->get(0)*cos(sg))/sin(sg));
	P->set(1, (B->get(1) - A->get(1)*cos(sg))/sin(sg));
	P->set(2, (B->get(2) - A->get(2)*cos(sg))/sin(sg));
/
	double sm1 = Smul(P, Q);
	double sm2 = Smul(P, A);


	cosA = (cb - ca*cosS)/(sinS*sin(sga));
	sinA = pow(-1.0, direct)*sqrt(1.0-cosA*cosA);

	printf("cosA= %f\tsinA= %f\n", cosA, sinA);
/*
	if(cosA<0.0)
	{
//		cosA = PI + cosA;
		sinA = -sinA;
	}
/
//	printf("cosA= %f\tsinA= %f\n", cosA, sinA);

//	sinA = sin(sgb)*
//	double sA = rad2grad(atan2(sinA, cosA));

	myvector *C0;
	C0 = new myvector(3);

	C0->set(0, P->get(0)*cosA + Q->get(0)*sinA);
	C0->set(1, P->get(1)*cosA + Q->get(1)*sinA);
	C0->set(2, P->get(2)*cosA + Q->get(2)*sinA);

	double sm3 = Smul(C0, A);
	printf("sin(sga)= %f\n", sin(sga));

	C->set(0, A->get(0)*ca + C0->get(0)*sin(sga));
	C->set(1, A->get(1)*ca + C0->get(1)*sin(sga));
	C->set(2, A->get(2)*ca + C0->get(2)*sin(sga));
	
	double sm4 = Smul(C, A);
	double sm5 = Smul(C, B);

	return res;
}

int IntAlongCurv1(myvector *V, myvector *R, myvector *M, double vm, double rm)
{
	double ssg, sa, cosm, sinm, csg, ca;
//	double sinS, sa, sb;


	csg = rm;
	ssg = sqrt(1 - rm*rm);
	ca = vm;
	sa = sqrt(1 - vm*vm);

	cosm = -(csg*ca)/(ssg*sa);
	sinm = sqrt(1.0 - cosm*cosm);


	myvector *M0;
	M0 = new myvector(3);
	myvector *E0;
	E0 = new myvector(3);
	myvector *E;
	E = new myvector(3);



	M0->set(0, -M->get(0)*csg/ssg + R->get(0)/ssg);
	M0->set(1, -M->get(1)*csg/ssg + R->get(1)/ssg);
	M0->set(2, -M->get(2)*csg/ssg + R->get(2)/ssg);

	*E0 = Vmul(M, M0);

//	printf("E0*M %e E0*M0 %e\n", Smul(E0, M), Smul(E0, M0));

	E->set(0, M0->get(0)*cosm + E0->get(0)*sinm);
	E->set(1, M0->get(1)*cosm + E0->get(1)*sinm);
	E->set(2, M0->get(2)*cosm + E0->get(2)*sinm);


	V->set(0, M->get(0)*vm + E->get(0)*sa);
	V->set(1, M->get(1)*vm + E->get(1)*sa);
	V->set(2, M->get(2)*vm + E->get(2)*sa);

	return 0;
}
*/


double det_L(double R, double ro, double r, double F)
{
	return(pow(4*PI*R*R/3.0, 2.0)*pow(cos(F/2.0), 2.0)/(ro*ro*r*r));
}

double det_m(double M, double r, double po, double c1, double F)
{
	return(M - log10(pow(cos(F/2.0)/po/r, 2.0))/log10(2.512)+1);
//	return(M - logbase(pow(cos(F/2.0)/po/r, 2.0), 2.512));
}

double detRfromM(double M, double A, double Sdist)
{
//warn
	return(Sdist*sqrt(2.0*pow(2.512, SUN_MAGNITUDE-M)/A));
}


void detABC(double *A, double *B, double *C, double R)
{
	*A = R;
	*B = R;
	*C = R;
}

int interp(double x1, double *x, double x2, double t1, double t, double t2)
{
	if((x==NULL)||(t<t1)||(t>t2)) return 1;

	double k;

	k = (t-t1)/(t2-t1);

	*x = (x2-x1)*k + x1;

	return 0;
}

double partOfYear(int year, int mth, double day)
{
    return(year*1.0+mth*1.0/12.0+day/dinm(mth, isVes(year))/12.0);
}

int strnsep(char *str, int nb, int nmax, char *sep, char *slo)
{
	char *sim;
	sim = new char[1];
//	char slo[10];

	int i=nb;
	int len = strlen(str);
	int k=0;
	int len1;
	
/*	do
	{*/
		strcpy(slo, "");
		do
		{
			strcpy(sim, "");

			strcpy(sim, &str[i]);
			strcpy(&sim[1], "\0");
			strcat(slo, sim);

			i++;

		}while((!streqv(sim, sep))&&(i<len)&&(i<nmax));
		len1 = strlen(slo);
		strcpy(&slo[len1-1], "\0");
		if(i==len) return 1;
/*		k++;
		if((i==len)&&(k<=n)) return 1;
	}while(k<=n);
*/
	delete [] sim;

	return 0;

}

int findT(double *T, double M, double eJD, double q, double t)
{
	double n;
	n = ka*pow(q, -1.5)/sqrt(2.0);
	*T = eJD - M/n;
/*
	double tgv;

	tgv = tan(M/2.0);
	*T = t - sqrt(2.0)*pow(q, 1.5)*(tgv +tgv*tgv*tgv/3.0)/ka;
*/
	return 0;
}

double Tk(int k, double t)
{
	if(k<0) return(0.0);
	if(k==0) return(1.0);
	if(k==1) return(t);
	if(k==2) return(2.0*t*t - 1.0);

	return(2.0*t*Tk(k-1, t) - Tk(k-2, t));
}

double Uk(int k, double t)
{
	if(k<=0) return(0.0);
	if(k==1) return(1.0);
	if(k==2) return(4.0*t);

	return(2.0*t*Uk(k-1, t) + 2.0*Tk(k-1, t) - Uk(k-2, t));
}

double Rx(double t, int n, double *ak)
{
	double x = 0.0;
	double tk;

//	printf("\n");

	for(int k = 0; k < n; k++)
	{
		tk = Tk(k, t);
//		printf("%20.18e\n", tk);
//		getch();
		x += ak[k]*tk;
	}

	return x;
}

double Vx(double t, int n, double *ak)
{
	double x = 0.0;

	for(int k = 0; k < n; k++) x += ak[k]*Uk(k, t);

	return x;
}

int detRDnumGC(double *RA, double *DEC, double x, double y, double z, double xc, double yc, double zc, double xo, double yo, double zo)
{
	double P1, P2, P3;

	P1 = x - (xc + xo);
	P2 = y - (yc + yo);
	P3 = z - (zc + zo);

	rdsys(RA, DEC, P1, P2, P3);

	return 0;
}

int detRDnumGC_vel(double *muRAcosD, double *muDEC, double x, double y, double z, double xc, double yc, double zc, double xo, double yo, double zo, double *range)
{
    double *R, *R1, *V;
    R = new double[3];
    R1 = new double[3];
    V = new double[3];

    V[0] = (x - (xc - xo));
    V[1] = (y - (yc - yo));
    V[2] = (z - (zc - zo));
    R[0] = range[0];
    R[1] = range[1];
    R[2] = range[2];

    R1[0] = R[0] + V[0];
    R1[1] = R[1] + V[1];
    R1[2] = R[2] + V[2];


    double ra, dec, ra1, dec1;

    rdsys(&ra, &dec, R[0], R[1], R[2]);
    rdsys(&ra1, &dec1, R1[0], R1[1], R1[2]);

    *muRAcosD = (ra1-ra)*cos(dec);
    *muDEC = (dec1-dec);

    delete [] R;
    delete [] V;
    delete [] R1;
    return 0;
}

int detAhnumGC(double *A, double *h, double s, double Cfi, double Sfi, double RA, double DEC)
{
	double P1, P2, P3;

//	double s;

//	UTC2s(tUTC, lam, &s);

    P1 = cos(DEC)*sin(s - RA);
    P2 = cos(DEC)*cos(s - RA)*Sfi - sin(DEC)*Cfi;
	P3 = cos(DEC)*cos(s - RA)*Cfi + sin(DEC)*Sfi;

	*A = atan2(P1, P2);
	if(*A<0.0) *A+=2.0*PI;
	if(*A>2.0*PI) *A-=2.0*PI;

	*h = atan2(P3*sin(*A), P1);

	if(*h>PI/2.0)
	{
//		*h = PI - *h;
		*h -=PI;
//		*A += PI;
	}
	if(*h<-PI/2.0)
	{
		*h += PI;// - *h;
//		*A += PI;
	}

	if(*A<0.0) *A+=2.0*PI;
	if(*A>2.0*PI) *A-=2.0*PI;

//	double t_A, t_h;

//	rdsys(A, h, P1, P2, P3);

	return 0;
}

void RotX(double *targ, double *sour, double angr)
{
	targ[0] =  sour[0];
	targ[1] = cos(angr)*sour[1] - sin(angr)*sour[2];
	targ[2] = sin(angr)*sour[1] + cos(angr)*sour[2];
}

void RotX(double *vect, double angr)
{
    double *v = new double[3];
    v[0] = vect[0];
    v[1] = vect[1];
    v[2] = vect[2];
    RotX(vect, v, angr);
    delete [] v;
}

void Vmul3(double *vectRes, double *vect0, double *vect1)
{
    vectRes[0] = vect0[1]*vect1[2] - vect0[2]*vect1[1];
    vectRes[1] = vect0[2]*vect1[0] - vect0[0]*vect1[2];
    vectRes[2] = vect0[0]*vect1[2] - vect0[1]*vect1[0];
}

void sortX(double *x, double *dx, int num)
{
    int i, j;
    double val;
    for(i=0; i<num-1; i++)
    {
        for(j=i+1; j<num; j++)
        {
            if(x[j]<x[i])
            {
                val = x[i];
                x[i] = x[j];
                x[j] = val;
                val = dx[i];
                dx[i] = dx[j];
                dx[j] = val;
            }
        }
    }
}

void desc2NumName(QString desc, int *num, QString *name)
{
    int i;
    name->clear();
    qDebug() << QString("desc: %1\n").arg(desc);
    int p0, p1;
    p0 = desc.lastIndexOf('[');
    p1 = desc.lastIndexOf(']');
    QString tName = desc.mid(p0+1, p1-p0-1).toLower();
    qDebug() << QString("tName: %1\n").arg(tName);
    if(tName.lastIndexOf("pluto")==0)
    {
        name->append("pluto");
        *num = 134340;
        return;
    }

    QStringList words = desc.split(" ", QString::SkipEmptyParts);
  //  qDebug() << words.join("|");
    int sz0 = words.size();
    if(sz0<4)
    {
        qDebug() << "\nToo small words\n";
        return;
    }

    name->append(words.at(2).toLower());
    //name->toLower();
    //qDebug() << QString("astName: %1\n").arg(*name);
    QString mpName = words.at(3);
    mpName.chop(1);
    sz0 = mpName.size();
    bool isOk;
    QString numStr;
    for(i=3; i<sz0; i++)
    {
        numStr = mpName.right(sz0-i);
        *num = numStr.toInt(&isOk);
        qDebug() << QString("numStr: %1\t%2\n").arg(numStr).arg(*num);
        if(isOk) break;
    }
    if(i==sz0) *num = -1;
    //return 0;
}





///////////////////////////////////////////////////////////////

///////////////////////////////////////////

MVpar::MVpar()
{
	this->param = new Vparams;
	this->next = NULL;
}

MVpar::~MVpar()
{
	delete(this->param);
	this->param = NULL;
	this->next = NULL;
}

///////////////////////////////////////////

VPList::VPList()
{
	root = new MVpar;
	num = 0;
}

VPList::~VPList()
{
	delete(root);
	root = NULL;
}

int VPList::addElem(Vparams *npar)
{
	MVpar *troot, *tnew;

	troot = root;

	while(root->next!=NULL)
	{
		root = root->next;
	}

	tnew = new MVpar;
	tnew->next = NULL;
	tnew->param->RAL = npar->RAL;
	tnew->param->RAR = npar->RAR;
	tnew->param->DECU = npar->DECU;
	tnew->param->DECB = npar->DECB;
	tnew->param->magn_max = npar->magn_max;
	tnew->param->magn_min = npar->magn_min;

	root->next = tnew;

	root = troot;

	this->num++;
	
	return 0;
}

int VPList::remElem(int num)
{
	if(num>=this->num) return 1;

	MVpar *troot, *tnext;
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

int VPList::clear()
{
	int nnn;

	for(nnn=this->num-1;nnn>=0;nnn--)
	{
		if(this->remElem(nnn)) return 1;
	}

	return 0;
}

///////////////////////////////////////////
///////////////////////////////////////////

Etop::Etop()
{
	this->name = new char[32];
}

Etop::~Etop()
{
	delete [] this->name;
	this->name = NULL;
}

///////////////////////////////////////////

MEtop::MEtop()
{
	this->top = new Etop;
	this->next = NULL;
}

MEtop::~MEtop()
{
	delete(this->top);
	this->top = NULL;

	this->next = NULL;
}

///////////////////////////////////////////

EtopList::EtopList()
{
	root = new MEtop;
	num = 0;
}

EtopList::~EtopList()
{
	delete(root);
	root = NULL;
}

int EtopList::addElem(Etop *npar)
{
	MEtop *troot, *tnew;

	troot = root;

	while(root->next!=NULL)
	{
		root = root->next;
	}

	tnew = new MEtop;
	tnew->next = NULL;
	tnew->top->Ntask = npar->Ntask;
	strncpy(tnew->top->name, npar->name, 32);
	tnew->top->RA = npar->RA;
	tnew->top->DEC = npar->DEC;
	tnew->top->magn = npar->magn;
	tnew->top->bonus = npar->bonus;

	root->next = tnew;

	root = troot;

	this->num++;
	
	return 0;
}

int EtopList::remElem(int num)
{
	if(num>=this->num) return 1;

	MEtop *troot, *tnext;
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

int EtopList::clear()
{
	int nnn;

	for(nnn=this->num-1;nnn>=0;nnn--)
	{
		if(this->remElem(nnn)) return 1;
	}

	return 0;
}


int detRiseSet(double *tr, double *ts, double de, double fi)
{
	*tr = atan2(-sqrt(cos(fi-de)), sqrt(cos(fi+de)));
	*ts = atan2(sqrt(cos(fi-de)), sqrt(cos(fi+de)));

    double at2 = atan2(cos(fi-de), cos(fi+de));

    *tr = -2.0*sqrt(at2);
    *ts = 2.0*sqrt(at2);

    if(*tr<0) *tr += 2.0*PI;
    if(*ts<0) *ts += 2.0*PI;

    if(*tr>2.0*PI) *tr -= 2.0*PI;
    if(*ts>2.0*PI) *ts -= 2.0*PI;
/*
	if((tt<=2.0*PI)&&(tt>=PI)) tr = tt;
	if((tt<=2.0*PI)&&(tt>=PI)) ts = -tt;
*/
	return 0;
}

int detCurf3(QString *resFunc, double *Zi, double *x, double *y, int num, int deg, double *We)
{
    int i, j, k;
    deg++;
    double *z = new double[deg];
    double *c = new double[deg*num];
    double *we = new double[num];

    int *EXCLIND;
    double *D = new double[deg*deg];
    int RN;
    //QStringList tStrL;

    for(i=0; i<num; i++)
    {
        //tStrL.clear();

        for(j=0; j<deg; j++)
        {
            c[i*deg+j] = pow(x[i], j);

            //tStrL << QString("Z0*%1").arg(c[i+j]);
        }

        we[i] = 1;
        if(We!=NULL) we[i] = We[i];
    }

    EXCLIND = new int [num];
    D=new double[deg*deg];
    RN=0;
    double UWE;
    //iLSM(deg, num, 500, EXCLIND, z, c, y, UWE, D, 3, RN, we);
    lsm(deg, num, z, c, y, UWE, D);
    //slsm(deg, num, z, c, y);

    QStringList opers;
    QStringList opers1;

    qDebug() << QString("RN: %1\tUWE: %2\n").arg(RN).arg(UWE);

    int sLog;
    for(i=0;i<deg; i++)
    {
        Zi[i] = z[i];
        if(z[i]>0) opers << "+";
        else opers << "-";
        sLog = (int)(log10(fabs(z[i]))/fabs(log10(fabs(z[i]))));
        opers << QString("(%1*10^(%2))*x^%3").arg(fabs(z[i])*pow(10, -((int)log10(fabs(z[i])))+1)).arg((int)log10(fabs(z[i]))+sLog).arg(i);
        qDebug() << QString("sLog: %1\n").arg(sLog);

        opers1 << QString("((%1)*x^%3").arg(z[i]).arg(i);
    }

    resFunc->clear();
    resFunc->insert(0, opers.join(""));
    qDebug() << QString("resFunc: %1\n").arg(opers.join(""));
    qDebug() << opers1.join("+") << "\n";

    delete [] we;
    delete [] D;
    delete [] z;
    delete [] c;
}

void summNum(double *x, double *dx, int num, int sNum)
{
    int i, j, p;
    j=0;
    p=0;
    qDebug() << QString("sNum= %1\n").arg(sNum);

    for(i=0; i<num; i++)
    {


        if(p==0)
        {
            x[j] = x[i];
            dx[j] = dx[i];
        }
        else
        {
            x[j] += x[i];
            dx[j] += dx[i];
        }
        p++;
        if(p==sNum||(i==num-1))
        {
            x[j] /= 1.0*p;
            dx[j] /= 1.0*p;

            //qDebug() << QString("j= %1\tp= %2\tx= %3\tdx= %4\n").arg();

            j++;
            //i++;
            //x[j] = x[i];
            //dx[j] = dx[i];

            p=0;
        }

    }
}

double polyY(double *Zi, double x, int deg)
{
    int j;
    double res;
    res = 0;
    for(j=0; j<deg+1; j++)
    {
        res += Zi[j]*pow(x, j);
    }
    return res;
}

void mCoefFunc(QString coefFileName, QString mCoefFileName, QString dataSep, int coefDeg)
{
    int i, j;
    double *x, *dx;
    double *mCoeffX;
    int dNum, coefNum;
    QString resFunc;
    //QVector <double> vect0, vect1;
    QStringList opers, coefLines, dataStrL;

    QFile r2FileXcoef, rFile;
    QTextStream dataStream2Xcoef, dataStream;

    r2FileXcoef.setFileName(coefFileName);
    r2FileXcoef.open(QIODevice::ReadOnly|QIODevice::Text);
    dataStream2Xcoef.setDevice(&r2FileXcoef);
/*
    r2FileYcoef.setFileName(r3NameYcoef);
    r2FileYcoef.open(QIODevice::ReadOnly|QIODevice::Text);
    dataStream2Ycoef.setDevice(&r2FileYcoef);
*/
    rFile.setFileName(mCoefFileName);
    rFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
    dataStream.setDevice(&rFile);

    coefLines.clear();
    while(!dataStream2Xcoef.atEnd())
    {
        coefLines << dataStream2Xcoef.readLine();
/*
        opers = dataStr.split(r3data.colSep);
        X << opers.at(0).toDouble();
        paramsNum = opers.size() - 1;
        X << dataStr.section(r3data.colSep, 0, 0);
        dX << dataStr.section(r3data.colSep, 1, 1);*/
    }

    dNum = coefLines.size();
    opers = coefLines.at(0).split(dataSep);
    coefNum = opers.size() - 1;

    qDebug() << QString("coefNum= %1\tdNum= %2\n").arg(coefNum).arg(dNum);

    for(i=1; i<coefNum+1; i++)
    {
        x = new double[dNum];
        dx = new double[dNum];

        for(j=0; j<dNum; j++)
        {
            opers = coefLines.at(j).split(dataSep);
            x[j] = opers.at(0).toDouble();
            dx[j] = opers.at(i).toDouble();

            qDebug() << QString("%1: x= %2\tdx=%3\n").arg(j).arg(x[j]).arg(dx[j]);
        }


        mCoeffX = new double[coefDeg+1];

        detCurf3(&resFunc, mCoeffX, x, dx, dNum, coefDeg);

        qDebug() << QString("resFunc M: %1\n").arg(resFunc);

        dataStrL.clear();
        //dataStrL << QString("%1").arg((magEq.diaps[k]+magEq.diaps[k+1])/2.0);
        for(j=0; j<coefDeg+1; j++)
        {
             dataStrL << QString("%1").arg(mCoeffX[j], 10, 'e');
        }

        dataStream << dataStrL.join(dataSep) << "\n";



        delete [] mCoeffX;
        delete [] x;
        delete [] dx;
    }

    r2FileXcoef.close();
    rFile.close();
}

int initMdCoef(QString mdCorr, QList<double*> *mdCoef, int *mdCoefsz, QString colSep)
{

    QFile mdFile(mdCorr);
    if(!mdFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "\ncant open dmCoef file\n";
        return 1;
    }
    QTextStream mdStm(&mdFile);

    QString tStr;
    QStringList opers;

    int sz, k;

    double *vect;

    k = 0;

    while(!mdStm.atEnd())
    {
        tStr = mdStm.readLine();
        qDebug() << tStr << "\n";
        opers = tStr.split(colSep);
        sz = opers.size();
        vect = new double[sz];
        qDebug() << QString("vect[%1]:\n").arg(k);
        for(int i=0; i<sz; i++)
        {
            vect[i] = opers.at(i).toDouble();

        }

        mdCoef->append(vect);
        k++;
    }

    qDebug() << QString("coefNum: %1\n").arg(k);

    *mdCoefsz = sz;

    qDebug() << QString("mdCoefsz= %1\n").arg(*mdCoefsz);

    mdFile.close();

    return 0;
}

void detCoefM(QVector <double> *dCoef, QList <double*> mdCoef, int mdCoefsz, double mag)
{
    int i, j;
    int szc = mdCoef.size();
    double res;
    for(i=0; i<szc; i++)
    {
        res = 0;
        for(j=0; j<mdCoefsz; j++) res += mdCoef[i][j]*pow(mag, j);

        //qDebug() << QString("coefM[%1] = %2\n").arg(i).arg(res);
        dCoef->append(res);
    }

}

int detDcorr(int corrModel, double x, double y, double *dx, double *dy, QVector <double> dCoefX, QVector <double> dCoefY)
{
    double res;

    switch(corrModel)
    {
    case 0:
        if((dCoefX.size()!=5)||(dCoefY.size()!=5)) return 1;

        *dx = dCoefX[0]*x + dCoefX[1]*y + dCoefX[2]*x*x + dCoefX[3]*x*y + dCoefX[4]*x*(x*x+y*y);
        *dy = dCoefY[0]*x + dCoefY[1]*y + dCoefY[2]*y*y + dCoefY[3]*x*y + dCoefY[4]*y*(x*x+y*y);

        break;

    }

    return 0;
}

int epm_planet_num(QString name)
{
    if(QString().compare(name, "MERCURY", Qt::CaseInsensitive)==0) return 1;
    if(QString().compare(name, "VENUS", Qt::CaseInsensitive)==0) return 2;
    if(QString().compare(name, "EARTH", Qt::CaseInsensitive)==0) return 3;
    if(QString().compare(name, "GEOCENTR", Qt::CaseInsensitive)==0) return 3;
    if(QString().compare(name, "MARS", Qt::CaseInsensitive)==0) return 4;
    if(QString().compare(name, "JUPITER", Qt::CaseInsensitive)==0) return 5;
    if(QString().compare(name, "SATURN", Qt::CaseInsensitive)==0) return 6;
    if(QString().compare(name, "URANUS", Qt::CaseInsensitive)==0) return 7;
    if(QString().compare(name, "NEPTUNE", Qt::CaseInsensitive)==0) return 8;
    if(QString().compare(name, "PLUTO", Qt::CaseInsensitive)==0) return 9;
    if(QString().compare(name, "MOON", Qt::CaseInsensitive)==0) return 10;
    if(QString().compare(name, "SUN", Qt::CaseInsensitive)==0) return 11;
    if(QString().compare(name, "SOL", Qt::CaseInsensitive)==0) return 11;
    if(QString().compare(name, "SSB", Qt::CaseInsensitive)==0) return 12;
    if(QString().compare(name, "SOLAR-SYSTEM BARYCENTER", Qt::CaseInsensitive)==0) return 12;
    if(QString().compare(name, "EMB", Qt::CaseInsensitive)==0) return 13;
    if(QString().compare(name, "EARTH-MOON BARYCENTER", Qt::CaseInsensitive)==0) return 13;
    return -1;
}

void redRefraction(double *ra, double *dec, refractionParam refParam)
{
    double ra0 = grad_to_rad(*ra);
    double dec0 = grad_to_rad(*dec);

    qDebug() << QString("ra= %1\tdec = %2\n").arg(*ra).arg(*dec);
    qDebug() << QString("ra0= %1\tdec0= %2\n").arg(ra0).arg(dec0);

    double a, b;
    double s;

    a = 2.871e-04;
    b = 0.00567;

    double k0 = a*(1.0 + (b/pow(refParam.lam, 2.0)));
    qDebug() << QString("k0= %1\n").arg(k0);
    double n1 = k0*(refParam.press/760.0)*(273.0/(refParam.temp + 273.0));
    qDebug() << QString("n1= %1\n").arg(n1);
    double gm1;
    jdUT1_to_GMST1(&gm1, mjd2jd(refParam.utc));
    //UTC2s(mjd2jd(refParam.utc), refParam.Long, &s);
    s = gm1 + refParam.Long;
    //s /= 2.0*PI;

    double t = s - ra0;

    qDebug() << QString("s= %1\tt= %2\n").arg(s).arg(t);

    double dRa, dDec;
    double cosd = cos(dec0);

    dDec = n1*(cos(dec0)*sin(refParam.Fi) - sin(dec0)*cos(refParam.Fi)*cos(t))/(sin(dec0)*sin(refParam.Fi) + cos(dec0)*cos(refParam.Fi)*cos(t));
    dRa = n1*(cos(refParam.Fi)*sin(t))/(sin(dec0) + cos(dec0)*cos(refParam.Fi)*cos(t));

    qDebug() << QString("dRa= %1\tdDec = %2\n").arg(dRa).arg(dDec);
    qDebug() << QString("dRa= %1\tdDec = %2\n").arg(rad2mas(dRa)).arg(rad2mas(dDec));

    *ra = rad2grad(ra0 + dRa/cosd);
    *dec = rad2grad(dec0 + dDec);

}

double grad2mas(double grad)
{
    return(grad*3600000);
}

double mas2grad(double mas)
{
    return(mas/3600000.0);
}
