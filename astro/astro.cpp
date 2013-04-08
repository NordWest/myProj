#include "astro.h"



double mas_to_rad(double angle)
{
  return acos(-1)*angle/(180*3600000);
};

double mas_to_grad(double angle)
{
	return angle/MAS_IN_GRAD;
}

double ang_b_dir(double a1, double d1, double a2, double d2)
{
  double r1[3];
        r1[0] = cos(mas_to_rad(a1))*cos(mas_to_rad(d1)); 
		r1[1] = sin(mas_to_rad(a1))*cos(mas_to_rad(d1));
		r1[2] = sin(mas_to_rad(d1));
 double r2[3];
        r2[0] = cos(mas_to_rad(a2))*cos(mas_to_rad(d2)); 
		r2[1] = sin(mas_to_rad(a2))*cos(mas_to_rad(d2));
		r2[2] = sin(mas_to_rad(d2));
    return rad_to_mas(acos(r1[0]*r2[0]+r1[1]*r2[1]+r1[2]*r2[2]));
};

double hms_to_mas(QString ra, QString spl_symb)
{
 ra = ra.trimmed(); 
 QStringList ra_list = ra.split(spl_symb, QString::SkipEmptyParts);
 QString nums = ra_list.at(0);
 double h = nums.toDouble();
 nums = ra_list.at(1);
 double m = nums.toDouble();
 nums = ra_list.at(2);
 double s = nums.toDouble();
 return (h*3600.0+m*60.0+s)*15000.0;
};

double damas_to_mas(QString de, QString spl_symb)
{
 de = de.trimmed();
 double sign;
 if (de[0]=='-') sign = -1; else sign = 1; 
 QStringList de_list = de.split(spl_symb, QString::SkipEmptyParts);
 QString nums = de_list.at(0);
 double deg = nums.toDouble();
 deg = fabs(deg);		
 nums = de_list.at(1);
 double arcmin = nums.toDouble();
 nums = de_list.at(2);
 double arcsec = nums.toDouble();
 return sign*(deg*3600+arcmin*60+arcsec)*1000;
};

QString mas_to_hms(double ra, QString spl_symb, int ndig)
{
 int h = (int)(ra/54000000);
 ra = ra-h*54000000;
 int m = (int)(ra/900000);
 ra = ra-m*900000;
 double s = ra/15000;
 QString str = QString( "%1" ).arg( h,2,10,QLatin1Char( '0' ))+spl_symb;
   str = str + QString( "%1" ).arg( m,2,10,QLatin1Char( '0' ))+spl_symb;
   str = str + QString( "%1" ).arg( s,3+ndig,'f',ndig,QLatin1Char( '0' ));
  return str;
};


QString mas_to_damas(double de, QString spl_symb, int ndig)
{
int sign = 0;
if (de<0) sign = 1;
de = fabs(de);
int d = (int)(de/3600000);
de = de - d*3600000;
int am = (int)(de/60000);
de = de - am*60000;
double as = de/1000;
QString str = QString( "%1" ).arg( d,2,10,QLatin1Char( '0' ))+spl_symb;
  str = str + QString( "%1" ).arg( am,2,10,QLatin1Char( '0' ))+spl_symb;
  str = str + QString( "%1" ).arg( as,3+ndig,'f',ndig,QLatin1Char( '0' ));
  if (sign==1)
    str = "-"+str;
  else
    str = "+"+str;
  return str;
};

DATEOBS timeFromStrNA(QString strT)
{
  DATEOBS date_obs;
  QList<QString> ml;
  ml << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun" << "Jul" << "Aug" << "Sep" << "Oct" << "Nov" << "Dec";
  QString s;
  //get month
  s = strT.mid(15,3);
  for (int i=0;i<12;i++)
  {
    if (s == ml.at(i))
	{
	  date_obs.month = i+1;
	  break;
	}
  }
  //get day
  s = strT.mid(19,2);
  date_obs.day = s.toInt();  
  //get year
  s = strT.mid(35,4);
  date_obs.year = s.toInt();
  //get time
  QString sect;
  s = strT.mid(22,12);
  sect = s.section(':',0,0);
  date_obs.hour = sect.toInt();
  sect = s.section(':',1,1);
  date_obs.min = sect.toInt();
  sect = s.section(':',2,2);
  date_obs.sec = sect.toDouble();
  date_obs.pday = date_obs.day+(date_obs.hour*3600+date_obs.min*60+date_obs.sec)/86400;
  return date_obs;
};

DATEOBS timeFromStrFTN(QString strT)
{
DATEOBS date_obs;
  QString s;
  //get month
  s = strT.mid(5,2);
  date_obs.month = s.toInt();
  //get day
  s = strT.mid(8,2);
  date_obs.day = s.toInt();  
  //get year
  s = strT.mid(0,4);
  date_obs.year = s.toInt();
  //get time
  QString sect;
  s = strT.mid(11,12);
  sect = s.section(':',0,0);
  date_obs.hour = sect.toInt();
  sect = s.section(':',1,1);
  date_obs.min = sect.toInt();
  sect = s.section(':',2,2);
  date_obs.sec = sect.toDouble();
  date_obs.pday = date_obs.day+(date_obs.hour*3600+date_obs.min*60+date_obs.sec)/86400;
  return date_obs;

};

int timeFromStrFTN(DATEOBS *date_obs, QString strT)
{
//DATEOBS date_obs;
  //QString s;
  //get month
  QStringList pDateStr;
  pDateStr = strT.split("T");
  if(pDateStr.size()!=2) return 1;
  QStringList dParts;
  dParts = pDateStr.at(0).split("-");
  if(dParts.size()!=3) return 1;
  //s = strT.mid(5,2);
  date_obs->month = dParts.at(1).toInt();
  //get day
  //s = strT.mid(8,2);
  date_obs->day = dParts.at(2).toInt();
  //get year
  //s = strT.mid(0,4);
  date_obs->year = dParts.at(0).toInt();
  //get time
  //QString sect;
  dParts.clear();
  dParts = pDateStr.at(1).split(":");
  if(dParts.size()!=3) return 1;
  //s = strT.mid(11,12);
  //sect = s.section(':',0,0);
  date_obs->hour = dParts.at(0).toInt();
  //sect = s.section(':',1,1);
  date_obs->min = dParts.at(1).toInt();
  //sect = s.section(':',2,2);
  date_obs->sec = dParts.at(2).toDouble();
  date_obs->pday = date_obs->day+(date_obs->hour*3600+date_obs->min*60+date_obs->sec)/86400;
  return 0;

};


DATEOBS timeFromStrSDSS(QString strT1, QString strT2)
{
	DATEOBS date_obs;
	QString s;
	s = strT1.section('/',0,0);
	date_obs.day = s.toInt();
	s = strT1.section('/',1,1);
	date_obs.month = s.toInt();
	s = strT1.section('/',2,2);
	date_obs.year = s.toInt();
	if (date_obs.year<10) date_obs.year = date_obs.year+ 2000;
	else date_obs.year = date_obs.year+ 1900;
	///
	s = strT2.section(':',0,0);
	date_obs.hour = s.toInt();
	s = strT2.section(':',1,1);
	date_obs.min = s.toInt();
	s = strT2.section(':',2,2);
	date_obs.sec = s.toDouble();
	date_obs.pday = date_obs.day+(date_obs.hour*3600+date_obs.min*60+date_obs.sec)/86400;
	return date_obs;
}

double getMJDfromYMD(QString strT)
{
	DATEOBS date_obs;
	QString elS = strT.section(' ',0,0); date_obs.year = elS.toInt();
	//QMessageBox::information(0,"debug",elS,QMessageBox::Ok,0,0);
	elS = strT.section(' ',1,1); date_obs.month = elS.toInt();
	//QMessageBox::information(0,"debug",elS,QMessageBox::Ok,0,0);
	elS = strT.section(' ',2,2); date_obs.pday = elS.toDouble();
	//QMessageBox::information(0,"debug",elS,QMessageBox::Ok,0,0);
	date_obs.day = (int)floor(date_obs.pday);
	double hh =   (date_obs.pday - date_obs.day)*24;
	date_obs.hour = (int)floor(hh);
	double mm = (hh - date_obs.hour)*60;
	date_obs.min = (int)floor(mm);
	date_obs.sec = (mm - date_obs.min)*60;
	double mjd = getJD(date_obs) - 2400000.5;
	return mjd;
}

double jd2mjd(double jd)
{
    return(jd-2400000.5);
}
double mjd2jd(double mjd)
{
    return(mjd+2400000.5);
}



double getJD(DATEOBS date_obs)
{
double d_day = date_obs.day;
double d_hour = date_obs.hour;
double d_min = date_obs.min;
double d_sec = date_obs.sec;
d_day = d_day + (d_hour+(d_min+d_sec/60.0)/60.0)/24.0;
int y;
int m;
if (date_obs.month<3)
{
	 y =date_obs.year-1;
	 m =date_obs.month+12;
}
else
{
	 y =date_obs.year;
	 m =date_obs.month;
}

int A = y/100;
double B = 2-A+floor(A/4);
double jd =floor(365.25*y)+floor(30.6001*(m+1))+d_day+1720994.5+B;
/*int A = y + 4716 -(int)((14-m)/12);
double B = fmod(m-3, 12);
double C = date_obs.day - 1;
double g = (int)((int)((A+184)/100)-38);
double jd = int(1461*A/4) + (int)((153*B+2)/5) + C - 1401 -0.5 - g + (d_hour+(d_min+d_sec/60.0)/60.0)/24.0;*/
return jd;
};

double getMJDfromStrNA(QString strT, double exp_time)
{
  DATEOBS date_obs = timeFromStrNA(strT);
  /*
  QString str;
  str = QString( "%1" ).arg( date_obs.year,4,10,QLatin1Char( ' ' ))+" ";
  str = str + QString( "%1" ).arg( date_obs.month,2,10,QLatin1Char( ' ' ))+" ";
  str = str + QString( "%1" ).arg( date_obs.day,2,10,QLatin1Char( ' ' ))+" ";
  str = str + QString( "%1" ).arg( date_obs.hour,2,10,QLatin1Char( ' ' ))+" ";
  str = str + QString( "%1" ).arg( date_obs.min,2,10,QLatin1Char( ' ' ))+" ";
  str = str + QString( "%1" ).arg( date_obs.sec,8,'f',4,QLatin1Char( ' ' ))+" ";
  QMessageBox::information(0,"debug",str,QMessageBox::Ok,0,0);
  */
  double mjd = getJD(date_obs) - 2400000.5 - exp_time/(2*86400);
  return mjd;
};

double getMJDfromStrFTN(QString strT, double exp_time)
{
  DATEOBS date_obs = timeFromStrFTN(strT);
  double mjd = getJD(date_obs) - 2400000.5 + exp_time/(2*86400);
  return mjd;
};

int getMJDfromStrFTN(double *mjd, QString strT, double exp_time)
{
  DATEOBS date_obs;
  if(timeFromStrFTN(&date_obs, strT)) return 1;
  *mjd = getJD(date_obs) - 2400000.5 + exp_time/(2*86400);
  return 0;
}

double getMJDfromStrT(QString strT)
{
  DATEOBS date_obs = timeFromStrFTN(strT);
  return(jd2mjd(getJD(date_obs)));
};

int getStrFTNfromMJD(QString *strT, double mjd, double exp_time)
{
    DATEOBS date_obs;
    getDATEOBSfromMJD(&date_obs, mjd - (exp_time/2.0)/86400.0);
    getStrFromDATEOBS(strT, date_obs, "", 2, 3);
    return 0;
}

int getStrTfromMJD(QString *strT, double mjd)
{
    DATEOBS date_obs;
    getDATEOBSfromMJD(&date_obs, mjd);
    getStrFromDATEOBS(strT, date_obs, "", 2, 3);
    return 0;
}

double getMJDfromStrSDSS(QString strT1, QString strT2, double exp_time)
{
  DATEOBS date_obs = timeFromStrSDSS(strT1, strT2);
  double mjd = getJD(date_obs) - 2400000.5 + exp_time/(2*86400);
  return mjd;
};

double getMJDfromStrSDSS(QString strT, double exp_time)
{
  DATEOBS date_obs = timeFromStrFTN(strT);
  double mjd = getJD(date_obs) - 2400000.5 + exp_time/(2*86400);
  return mjd;
};

double getYearFromMJD(double mjd)
{
   return 1858.87885010267 + mjd/365.25;
};

double getMJDFromYear(double year)
{
   return (year-1858.87885010267)*365.25;
};

DATEOBS getDATEOBSfromMJD(double mjd, int rnsec)
{
   DATEOBS date_obs;

    getDATEOBSfromMJD(&date_obs, mjd);

    roundDATEOBS(&date_obs, rnsec);

   return date_obs;
};

int roundDATEOBS(DATEOBS *date_obs, int nsec)
{
    int dayM;
    double val;

    val = 60.0 - pow(10, -nsec);

//qDebug() << QString("sec: %1\nnsec: %2\nres: %3\n").arg(date_obs->sec*pow(10, nsec)).arg(floor(59.999999*pow(10, nsec))).arg(((int)(date_obs->sec*pow(10, nsec)))>=((int)(59.999999*pow(10, nsec))));

    if(date_obs->sec*pow(10, nsec)>=floor(val*pow(10, nsec)))
    {
        date_obs->sec = 0.0;
        date_obs->min +=1;
    }
//qDebug() << QString("secn: %1\n").arg(date_obs->sec);
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

}

void getDATEOBSfromMJD(DATEOBS *date_obs, double mjd, int rnsec)
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

    roundDATEOBS(date_obs, rnsec);


    //return date_obs;
}

QString getStrFromDATEOBS(DATEOBS date_obs, QString spl_symb, int format, int ndig)
{
  QString str;
  switch(format)
  {
    case 0:
	{
	  str = QString( "%1" ).arg( date_obs.year,4,10,QLatin1Char( '0' ))+spl_symb;
	  str = str + QString( "%1" ).arg( date_obs.month,2,10,QLatin1Char( '0' ))+spl_symb;
	  str = str + QString( "%1" ).arg( date_obs.pday,3+ndig,'f',ndig,QLatin1Char( '0' ));
	  break;	  
    }
	case 1:
	{
	  str = QString( "%1" ).arg( date_obs.year,4,10,QLatin1Char( '0' ))+spl_symb;
	  str = str + QString( "%1" ).arg( date_obs.month,2,10,QLatin1Char( '0' ))+spl_symb;
	  str = str + QString( "%1" ).arg( date_obs.day,2,10,QLatin1Char( '0' ))+" ";
	  str = str + QString( "%1" ).arg( date_obs.hour,2,10,QLatin1Char( '0' ))+spl_symb;
	  str = str + QString( "%1" ).arg( date_obs.min,2,10,QLatin1Char( '0' ))+spl_symb;
	  str = str + QString( "%1" ).arg( date_obs.sec,3+ndig,'f',ndig,QLatin1Char( '0' ));
	  break;
	}
	case 2:
	{
	  str = QString( "%1" ).arg( date_obs.year,4,10,QLatin1Char( '0' ))+"-";
	  str = str + QString( "%1" ).arg( date_obs.month,2,10,QLatin1Char( '0' ))+"-";
	  str = str + QString( "%1" ).arg( date_obs.day,2,10,QLatin1Char( '0' ))+"T";
	  str = str + QString( "%1" ).arg( date_obs.hour,2,10,QLatin1Char( '0' ))+":";
	  str = str + QString( "%1" ).arg( date_obs.min,2,10,QLatin1Char( '0' ))+":";
	  str = str + QString( "%1" ).arg( date_obs.sec,3+ndig,'f',ndig,QLatin1Char( '0' ));
	  break;
	}
  }
  return str;
};

void getStrFromDATEOBS(QString *rstr, DATEOBS date_obs, QString spl_symb, int format, int ndig)
{
  QString str;
  switch(format)
  {
    case 0:
        {
          str = QString( "%1" ).arg( date_obs.year,4,10,QLatin1Char( '0' ))+spl_symb;
          str = str + QString( "%1" ).arg( date_obs.month,2,10,QLatin1Char( '0' ))+spl_symb;
          str = str + QString( "%1" ).arg( date_obs.pday,3+ndig,'f',ndig,QLatin1Char( '0' ));
          break;
    }
        case 1:
        {
          str = QString( "%1" ).arg( date_obs.year,4,10,QLatin1Char( '0' ))+spl_symb;
          str = str + QString( "%1" ).arg( date_obs.month,2,10,QLatin1Char( '0' ))+spl_symb;
          str = str + QString( "%1" ).arg( date_obs.day,2,10,QLatin1Char( '0' ))+" ";
          str = str + QString( "%1" ).arg( date_obs.hour,2,10,QLatin1Char( '0' ))+spl_symb;
          str = str + QString( "%1" ).arg( date_obs.min,2,10,QLatin1Char( '0' ))+spl_symb;
          str = str + QString( "%1" ).arg( date_obs.sec,3+ndig,'f',ndig,QLatin1Char( '0' ));
          break;
        }
        case 2:
        {
          str = QString( "%1" ).arg( date_obs.year,4,10,QLatin1Char( '0' ))+"-";
          str = str + QString( "%1" ).arg( date_obs.month,2,10,QLatin1Char( '0' ))+"-";
          str = str + QString( "%1" ).arg( date_obs.day,2,10,QLatin1Char( '0' ))+"T";
          str = str + QString( "%1" ).arg( date_obs.hour,2,10,QLatin1Char( '0' ))+":";
          str = str + QString( "%1" ).arg( date_obs.min,2,10,QLatin1Char( '0' ))+":";
          str = str + QString( "%1" ).arg( date_obs.sec,3+ndig,'f',ndig,QLatin1Char( '0' ));
          break;
        }
  }
  rstr->clear();
  rstr->append(str);
};

double getAngleDeg(double cos_x, double sin_x)
{
//  double PI = atan(1)*4;
  double ang = 180*atan2(cos_x, sin_x)/PI;
  if (ang<0)ang = 360 + ang;
  return ang;
};


double* getTangToRaDe(double ksi, double eta, double ra_c, double de_c)
{
    double rd_vector[2];
	ra_c = mas_to_rad(ra_c);
	de_c = mas_to_rad(de_c);
	ksi = mas_to_rad(ksi*1000);
        eta = mas_to_rad(eta*1000);
	double secD = 1/cos(de_c);
	double commP = 1-eta*tan(de_c);
	double ra = atan((ksi*secD)/(commP))+ra_c;
	double de = atan((eta+tan(de_c))*cos(ra - ra_c)/commP);
	rd_vector[0] = rad_to_mas(ra);
	rd_vector[1] = rad_to_mas(de);
	return rd_vector;
};

void getTangToRaDe1(double *rd0, double *rd1, double ksi, double eta, double ra_c, double de_c)
{
//    double rd_vector[2];
	ra_c = mas_to_rad(ra_c);
	de_c = mas_to_rad(de_c);
	ksi = mas_to_rad(ksi*1000);
        eta = mas_to_rad(eta*1000);
	double secD = 1/cos(de_c);
	double commP = 1-eta*tan(de_c);
	double ra = atan((ksi*secD)/(commP))+ra_c;
	double de = atan((eta+tan(de_c))*cos(ra - ra_c)/commP);
	*rd0 = rad_to_mas(ra);
	*rd1 = rad_to_mas(de);
//	return rd_vector;
};

void getTangToRaDe(double ksi, double eta, double ra_c, double de_c, double *const ep)
{
	ra_c = mas_to_rad(ra_c);
	de_c = mas_to_rad(de_c);
	ksi = mas_to_rad(ksi*1000);
    eta = mas_to_rad(eta*1000);
	double secD = 1/cos(de_c);
	double commP = 1-eta*tan(de_c);
	double ra = atan((ksi*secD)/(commP))+ra_c;
	double de = atan((eta+tan(de_c))*cos(ra - ra_c)/commP);
	ep[0] = rad_to_mas(ra);
	ep[1] = rad_to_mas(de);
};

double* getRaDeToTang(double ra, double de, double ra_c, double de_c)
{
double rd_vector[2];
ra_c = mas_to_rad(ra_c);
de_c = mas_to_rad(de_c);
ra = mas_to_rad(ra);
de = mas_to_rad(de);
double ksi = cos(de)*sin(ra-ra_c)/(sin(de)*sin(de_c)+cos(de)*cos(de_c)*cos(ra - ra_c));
double eta = (sin(de)*cos(de_c)-cos(de)*sin(de_c)*cos(ra - ra_c))/(sin(de)*sin(de_c)+cos(de)*cos(de_c)*cos(ra - ra_c));
rd_vector[0] = rad_to_mas(ksi)/1000;
rd_vector[1] = rad_to_mas(eta)/1000;
return rd_vector;
};

void getRaDeToTang1(double *ksi, double *eta, double ra, double de, double ra_c, double de_c)
{
//double rd_vector[2];
ra_c = mas_to_rad(ra_c);
de_c = mas_to_rad(de_c);
ra = mas_to_rad(ra);
de = mas_to_rad(de);
*ksi = cos(de)*sin(ra-ra_c)/(sin(de)*sin(de_c)+cos(de)*cos(de_c)*cos(ra - ra_c));
*eta = (sin(de)*cos(de_c)-cos(de)*sin(de_c)*cos(ra - ra_c))/(sin(de)*sin(de_c)+cos(de)*cos(de_c)*cos(ra - ra_c));
*ksi = rad_to_mas(*ksi)/1000.0;
*eta = rad_to_mas(*eta)/1000.0;
//return rd_vector;
};

void getRaDeToTang(double ra, double de, double ra_c, double de_c, double *const tang)
{
      ra_c = mas_to_rad(ra_c);
      de_c = mas_to_rad(de_c);
      ra = mas_to_rad(ra);
      de = mas_to_rad(de);
      double ksi = cos(de)*sin(ra-ra_c)/(sin(de)*sin(de_c)+cos(de)*cos(de_c)*cos(ra - ra_c));
      double eta = (sin(de)*cos(de_c)-cos(de)*sin(de_c)*cos(ra - ra_c))/(sin(de)*sin(de_c)+cos(de)*cos(de_c)*cos(ra - ra_c));
      tang[0] = rad_to_mas(ksi)/1000;
      tang[1] = rad_to_mas(eta)/1000;
};


double* getCposFromWCS(double x, double y, double *WCSD)
{
   double ksi = (x - WCSD[0])*WCSD[8] + (y - WCSD[1])*WCSD[9];
   double eta = (x - WCSD[0])*WCSD[10] + (y - WCSD[1])*WCSD[11];
   double* posC = getTangToRaDe(ksi*3600, eta*3600, WCSD[2]*3600000, WCSD[3]*3600000);
   return posC;
};

void getCposFromWCS1(double *ra, double *de, double x, double y, double *WCSD)
{
   double ksi = (x - WCSD[0])*WCSD[8] + (y - WCSD[1])*WCSD[9];
   double eta = (x - WCSD[0])*WCSD[10] + (y - WCSD[1])*WCSD[11];
   getTangToRaDe1(ra, de, ksi*3600, eta*3600, WCSD[2]*3600000, WCSD[3]*3600000);
}
/*
double* getPixPosFromWCS(double ra, double de, double *WCSD)
{
	double* tPos;
	tPos = getRaDeToTang(ra, de, WCSD[2]*3600000, WCSD[3]*3600000);
	double t1 = tPos[0]/3600; double t2 = tPos[1]/3600;
	double a1 = WCSD[8];double b1 = WCSD[9];
	double a2 = WCSD[10];double b2 = WCSD[11];
	double rPix[2];
	rPix[0] = (t2-t1*b2/b1)/(a2-a1*b2/b1);
	rPix[1] = (t1-a1*rPix[0])/b1;
	rPix[0] = rPix[0]+WCSD[0];
	rPix[1] = rPix[1]+WCSD[1];
	return rPix;
};
*/
void getPixPosFromWCS(double *x, double *y, double ra, double de, double *WCSD)
{
	double* tPos;
	tPos = new double[2];
	getRaDeToTang1(&tPos[0], &tPos[1], ra, de, WCSD[2]*3600000, WCSD[3]*3600000);
        //qDebug() << QString("ra_c= %1\tde_c= %2\n").arg(mas_to_hms(grad_to_mas(WCSD[2]), " " , 5)).arg(mas_to_damas(grad_to_mas(WCSD[3]), " ", 5));
	double t1 = tPos[0]/3600.0; double t2 = tPos[1]/3600.0;
	double a1 = WCSD[8];double b1 = WCSD[9];
	double a2 = WCSD[10];double b2 = WCSD[11];
	double rPix[2];
/*	qDebug() << "\n\n";
	qDebug() << "\nt1\t" << t1;
	qDebug() << "\nt2\t" << t2;
	qDebug() << "\na1\t" << a1;
	qDebug() << "\na2\t" << a2;
	qDebug() << "\nb1\t" << b1;
	qDebug() << "\nb2\t" << b2;*/
	rPix[0] = (t2-t1*b2/b1)/(a2-a1*b2/b1);
	rPix[1] = (t1-a1*rPix[0])/b1;
/*	qDebug() << "\nrPix[0]\t" << rPix[0];
	qDebug() << "\nrPix[1]\t" << rPix[1];
	qDebug() << "\n\n";*/
	rPix[0] = rPix[0]+WCSD[0];
	rPix[1] = rPix[1]+WCSD[1];
//	qDebug() << "\nrPix[0]\t" << rPix[0];
//	qDebug() << "\nrPix[1]\t" << rPix[1];
	
	*x = rPix[0];
	*y = rPix[1];
};



void getPixPosFromWCS(double ra, double de, double *WCSD, double *const pixp)
{
    double tPos[2];
    getRaDeToTang(ra, de, WCSD[2]*3600000, WCSD[3]*3600000, tPos);
    double t1 = tPos[0]/3600; double t2 = tPos[1]/3600;
    double a1 = WCSD[8];double b1 = WCSD[9];
    double a2 = WCSD[10];double b2 = WCSD[11];
    //double rPix[2];
    pixp[0] = (t2-t1*b2/b1)/(a2-a1*b2/b1);
    pixp[1] = (t1-a1*pixp[0])/b1;
    pixp[0] = pixp[0]+WCSD[0];
    pixp[1] = pixp[1]+WCSD[1];
};

void mjdDateCode(QString *dateCode, double mJD)
{
    DATEOBS dObs;
    dObs = getDATEOBSfromMJD(mJD, 1);

    dateCode->clear();

    dateCode->append(QString("%1").arg((int)dObs.year, 4, 10, QLatin1Char( '0' )));
    dateCode->append(QString("%1").arg((int)dObs.month, 2, 10, QLatin1Char( '0' )));
    dateCode->append(QString("%1").arg((int)dObs.day, 2, 10, QLatin1Char( '0' )));
    dateCode->append(QString("%1").arg((int)dObs.hour, 2, 10, QLatin1Char( '0' )));
    dateCode->append(QString("%1").arg((int)dObs.min, 2, 10, QLatin1Char( '0' )));
    //dateCode->append(QString("%1").arg((int)dObs.sec, 2, 10, QLatin1Char( '0' )));
    dateCode->append(QString("%1").arg((int)floor(dObs.sec*10+0.5), 3, 10, QLatin1Char( '0' )));
}

void mjdDateCode_file(QString *dateCode, double mJD)
{
    DATEOBS dObs;
    dObs = getDATEOBSfromMJD(mJD, 1);
    dateCode->clear();
    dateCode->append(QString("%1").arg((int)dObs.year, 4, 10, QLatin1Char( '0' )));
    dateCode->append(QString("%1").arg((int)dObs.month, 2, 10, QLatin1Char( '0' )));
    dateCode->append(QString("%1-").arg((int)dObs.day, 2, 10, QLatin1Char( '0' )));
    dateCode->append(QString("%1").arg((int)dObs.hour, 2, 10, QLatin1Char( '0' )));
    dateCode->append(QString("%1").arg((int)dObs.min, 2, 10, QLatin1Char( '0' )));
    //dateCode->append(QString("%1").arg((int)dObs.sec, 2, 10, QLatin1Char( '0' )));
    dateCode->append(QString("%1").arg((int)floor(dObs.sec*1000 + 0.5), 5, 10, QLatin1Char( '0' )));
}

//My
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
 /*
int make_fits_header(fitsfile *fptr, char *fheadfn)
{
	int i, status;
	char *str = new char[255];
	char *str_name = new char[255];
	char *str_value = new char[255];
	char *str_date_obs = new char[255];
	char *str_time_obs = new char[255];
	char *str_date_end = new char[255];
	char *str_time_end = new char[255];
	
//	QTextStream out_stream;
	
	FILE *FI = fopen(fheadfn, "r");
	if(FI==NULL) return 1;
	
	int posb, pose, tobs = 0;
	QString *qstr = new QString;
	QByteArray qbarr;
	
	QTextCodec *codec1 = QTextCodec::codecForName("windows-1251");
	QTextCodec *codec2 = QTextCodec::codecForName("IBM 866");
	
	QTextStream out_stream;
	QFile fout("tiff2fits.log");
	fout.open(QIODevice::Append);
	out_stream.setDevice(&fout);
	
	
/*	QTextDecoder *decoder = codec1->makeDecoder();
	QTextEncoder *encoder = codec2->makeEncoder();
/
    QString string;
	QChar *cstr = new QChar;
		
	while(fgets(str, 82, FI))
	{
		qbarr.clear();
		qbarr.insert(0, str);

//		out_stream << "qbarr\t" << qbarr.data() << endl;
		string = codec1->toUnicode(qbarr);
//		out_stream << "string\t" << string << endl;
		qbarr = codec2->fromUnicode(string);
		str = qbarr.data();
	
//		printf("_%s_\n", str);
//		out_stream << "qbarr\t" << qbarr << endl;
//		out_stream << "str\t" << str << endl;
	
		if(!slovo_stopsim(str, str_name, " ", 0, &pose, 8)&&!slovo_stopsim(str, str_value, "\'", 11, &pose, 68))
		{
//			printf("_%s_ _%s_\n", str_name, str_value);
//			out_stream << "_" << str_name << "_\t_" << str_value << "_" << endl;
			if(streqv(str_name, "DATE-OBS"))
			{
				strcpy(str_date_obs, str_value);
				strcpy(str_date_end, str_value);
			}
			else if(streqv(str_name, "TIME-OBS")&&tobs==0)
			{
				strcpy(str_time_obs, str_value);
				tobs++;
			}
			else if(streqv(str_name, "TIME-OBS")&&tobs==1)
			{
				strcpy(str_time_end, str_value);
				make_date_obs(str_date_obs, str_time_obs, str_date_end, str_time_end);
				status = 0;
//				fits_write_key_str(fptr, "DATE-OBS", str_date_obs, "", &status);
				fits_update_key(fptr, TSTRING, "DATE-OBS", str_date_obs, "date of observations", &status);
				out_stream << "write DATE-OBS = _" << str_name << "_\t_" << str_value << "_" << QString("write_key %1").arg(status)<< endl;
				status = 0;
				fits_write_key_str(fptr, "DATE-END", str_date_end, "", &status);
				status = 0;
			}
			else if(streqv(str_name, "RA")&&tobs==1)
			{
				make_good_RADEC(str_value, 0);
				fits_write_key_str(fptr, str_name, str_value, "", &status);
				status = 0;
			}
			else if(streqv(str_name, "DEC")&&tobs==1)
			{
				make_good_RADEC(str_value, 1);
				fits_write_key_str(fptr, str_name, str_value, "", &status);
				status = 0;
			}
			else if(streqv(str_name, "TELESCOP")&&tobs==1)
			{
				make_good_RADEC(str_value, 1);
				fits_write_key_str(fptr, str_name, str_value, "", &status);
				status = 0;
			}
			else
			{
/*				fits_write_key_str(fptr, str_name, str_value, "", &status);
				out_stream << str_name << "\t" << str_value << "\t" << QString("stat=%1").arg(status) << endl;
                                status = 0;/
			}
		}
	}
	
	fout.close();

	return 0;
}

int make_good_fits_header(char *fheader_name)
{
	return 0;
}
 
void make_good_RADEC(char *str_ra, int type)
{
	int hr, min;
	double sec;
	
	char *str = new char[255];
	char *slo = new char[255];
	char *ssim = new char[255];
	int posb, pose, nmax;
	
	strcpy(ssim, " ");
	
	posb = 0;
	nmax = 2;
	slovo_stopsim(str_ra, slo, ssim, posb, &pose, nmax);
	hr = atoi(slo);
	
	posb = pose+1;
	nmax = 2;
	slovo_stopsim(str_ra, slo, ssim, posb, &pose, nmax);
	min = atoi(slo);
	
	posb = pose+1;
	nmax = 5;
	slovo_stopsim(str_ra, slo, ssim, posb, &pose, nmax);
	sec = atof(slo);
	
	if(type) sprintf(str_ra, "%+02d:%02d:%06.3f", hr, min, sec);
	else sprintf(str_ra, "%02d:%02d:%06.3f", hr, min, sec);
}

void make_date_obs(char *str_date_obs, char *str_time_obs, char *str_date_end, char *str_time_end)
{
	int yr_obs, mth_obs, h_obs, m_obs, iday;
	double day, jd_obs, jd_end, s_obs;
	int h_end, m_end;
	double s_end;
	
	char *str = new char[255];
	char *slo = new char[255];
	char *ssim = new char[255];
	int posb, pose, nmax;
		
	strcpy(ssim, " ");
	
	posb = 0;
	nmax = 5;
	slovo_stopsim(str_date_obs, slo, ssim, posb, &pose, nmax);
	yr_obs = atoi(slo);
	
	posb = pose+1;
	nmax = 2;
	slovo_stopsim(str_date_obs, slo, ssim, posb, &pose, nmax);
	mth_obs = atoi(slo);
	
	posb = pose+1;
	nmax = 2;
	slovo_stopsim(str_date_obs, slo, ssim, posb, &pose, nmax);
	iday = atoi(slo);
	
	posb = 0;
	nmax = 2;
	slovo_stopsim(str_time_obs, slo, ssim, posb, &pose, nmax);
	h_obs = atoi(slo);
	
	posb = pose+1;
	nmax = 2;
	slovo_stopsim(str_time_obs, slo, ssim, posb, &pose, nmax);
	m_obs = atoi(slo);
	
	posb = pose+1;
	nmax = 5;
	slovo_stopsim(str_time_obs, slo, ssim, posb, &pose, nmax);
	s_obs = atof(slo);
	
	HMS2day(&day, h_obs, m_obs, s_obs);
	dat2JD(&jd_obs, yr_obs, mth_obs, iday+day);
	
	posb = 0;
	nmax = 2;
	slovo_stopsim(str_time_end, slo, ssim, posb, &pose, nmax);
	h_end = atoi(slo);
	
	posb = pose+1;
	nmax = 2;
	slovo_stopsim(str_time_end, slo, ssim, posb, &pose, nmax);
	m_end = atoi(slo);
	
	posb = pose+1;
	nmax = 5;
	slovo_stopsim(str_time_end, slo, ssim, posb, &pose, nmax);
	s_end = atof(slo);
	
	HMS2day(&day, h_end, m_end, s_end);
	dat2JD(&jd_end, yr_obs, mth_obs, iday+day);

	if(jd_end<jd_obs) jd_end++;

	dat2YMD(jd_obs, &yr_obs, &mth_obs, &day);
	
	sprintf(str_date_obs, "%4d-%02d-%02dT%02d:%02d:%02d", yr_obs, mth_obs, (int)day, h_obs, m_obs, (int) s_obs);
	
	dat2YMD(jd_end, &yr_obs, &mth_obs, &day);
	
	sprintf(str_date_end, "%4d-%02d-%02dT%02d:%02d:%02d", yr_obs, mth_obs, (int) day, h_end, m_end, (int) s_end);
}

int UCAC2dataToFile(double ra_c, double de_c, double rho_x, double rho_y, double mag1, double mag2,	int shape, QString path, QString savePath, QString ufname, QString sepS, int pForm)
//all positional parameters must be in mas!
{
   QString debug_str;
   //calculation of positional bins
   double ra1,ra2,de1,de2,curr_ra1,curr_ra2,ra,de,mag;
   quint32 *p_32;
   qint32 *p_s32;
   quint16 *p_16;
   qint16 *p_s16;
   quint8  *p_8;
   qint8  *p_s8;
   if (de_c + rho_y > 324000000)
   {
     ra1 = 0; ra2 = 1296000000; de1 = de_c - rho_y; de2 = 324000000;
   }
   if (de_c + rho_y == 324000000)
   {
     ra1 = ra_c - 324000000;
     if (ra1<0) ra1 = ra1 + 1296000000;	 
	 ra2 = ra_c + 324000000;
     if (ra2>1296000000) ra2 = ra2 - 1296000000;	 
	 de1 = de_c - rho_y; de2 = 324000000;
   }
   if (de_c + rho_y < -324000000)
   {
     ra1 = 0; ra2 = 1296000000; de2 = de_c + rho_y; de1 = -324000000;
   }
   if (de_c + rho_y == -324000000)
   {
     ra1 = ra_c - 324000000;
     if (ra1<0) ra1 = ra1 + 1296000000;	 
	 ra2 = ra_c + 324000000;
     if (ra2>1296000000) ra2 = ra2 - 1296000000;	 
	 de2 = de_c + rho_y; de1 = -324000000;
   }
   if((de_c+rho_y<324000000)&&(de_c-rho_y>-324000000))
   {
       de1 = de_c - rho_y; de2 = de_c + rho_y;
	   double dra;
	   if(shape==0)
	   {
			dra = rad_to_mas(asin(sin(mas_to_rad(rho_x))/cos(mas_to_rad(de_c))));
		}
	   else
	   {
	       dra = rho_x;
		}
       ra1 = ra_c - dra;
       if (ra1 < 0) ra1 = ra1 + 1296000000;
       ra2 = ra_c + dra;
       if (ra2>1296000000) ra2 = ra2 - 1296000000;	   
   }
   //
   QFile outputfile;
   QTextStream out_stream;
   if (ufname=="-stdout")
   {
		outputfile.open(stdout, QIODevice::WriteOnly | QIODevice::Text);
   }
   else
   {
	   outputfile.setFileName(savePath+"/"+ufname);
	   outputfile.open(QIODevice::WriteOnly | QIODevice::Text);
   }
   out_stream.setDevice(&outputfile);
   QString S;
   //
   int bcount,k,i,j;
   if (ra2<ra1) bcount = 2; else bcount =1;
   //load index file
   quint32 u2index[288][240];
   QFile accfile(path + "/1cd/u2/u2index.da");
   if (accfile.open(QIODevice::ReadOnly))
   {
	  char *data = (char*)u2index;
	  accfile.read(data,288*240*4);
	  accfile.close();
   }
   else
   {
//	 QMessageBox::information(0,"debug", path + "/1cd/u2/u2index.da",QMessageBox::Ok,0,0);
	 
	 outputfile.close();
	 return 2;
   }
   QFile u2file;
   char *udata;
   quint32 No,N,Ns;
   QList<char*> st_list;
   QVector<quint32> ucac2_n;
   for (k=0;k<bcount;k++)
   {
       if (bcount==2)
	   {
	     if(k==0)
		 {
		    curr_ra1 = ra1; curr_ra2 = 1296000000;
		 }
		 if(k==1)
		 {
		    curr_ra1 = 0; curr_ra2 = ra2;
		 }		 
	   }
	   else
	   {
	       curr_ra1 = ra1; curr_ra2 = ra2;
	   }
	   //zone indexes determination
	   int zn1 = (int)((324000000 + de1)/1800000);
	   int zn2 = (int)((324000000 + de2)/1800000);
	   int jj1 = (int)((curr_ra1)/5400000);
	   int jj2 = (int)((curr_ra2)/5400000);
	   if (zn1>288)
        {
		  //QMessageBox::information(0,"debug","out of ucac2 zone",QMessageBox::Ok,0,0);
		  outputfile.close();
		  return 3;
		}
       if ((zn2>288)&&(zn1<=288)) zn2 = 288;		
	   //collecting of stars
	   for (i=zn1;i<=zn2;i++)
	   {
		  //file name determination
		  QString ufname = QString( "%1" ).arg( i+1,3,10,QLatin1Char( '0' ));
		  if (i<=105) ufname = path+"/1cd/u2/z" + ufname;
		  if ((i>105)&&(i<=181)) ufname = path+"/2cd/u2/z" + ufname;
		  if (i>181) ufname = path+"/3cd/u2/z" + ufname;
		  u2file.setFileName(ufname);
		  //set indexes and numbers
		  if(i==0)No =0; else No = u2index[i-1][239];
		  if (jj1==0)
		  {
		     if (i==0) N=0; else N = u2index[i-1][239];
		  }
		  else N = u2index[i][jj1-1];
		  Ns = u2index[i][jj2] - N;
		  if (u2file.open(QIODevice::ReadOnly))
		  {
			udata = new char[Ns*44];
			u2file.seek((N-No)*44);
			u2file.read(udata,Ns*44);
			u2file.close();
			//reading ucac2 data
			st_list.clear();
			ucac2_n.clear();
			for (j=0;j<Ns;j++)
			{
			    N++;
				p_32 = (quint32*)(udata+j*44);
				ra = (double)*p_32;
				p_s32 = (qint32*)(udata+j*44+4);
				de = (double)*p_s32;
				p_16 = (quint16*)(udata+j*44+8); mag = (double)*(p_16)/100;
				if (shape==0)
				{
					if((ang_b_dir(ra, de, ra_c, de_c)<=rho_y)&&(mag>=mag1)&&(mag<=mag2)&&(mag!=0))
					{
					   st_list << udata+j*44;
					   ucac2_n << N;
					}
				}
				if (shape==1)
				{
					if((delta_ra(ra,ra_c)<=rho_x)&&(de>de1)&&(de<=de2)&&(mag>=mag1)&&(mag<=mag2)&&(mag!=0))
					{
					   st_list << udata+j*44;
					   ucac2_n << N;
					}
				}
			}
			for (j=0;j<st_list.count();j++)
			{
			    S = "";
				p_32 = (quint32*)(st_list[j]);
				if (pForm==1)
				{
					S = S+mas_to_hms((double)*p_32, " ", 4)+sepS;
				}
				if (pForm==2)
				{
					S = S+QString("%1").arg((double)*p_32/3600000,12,'f',8,QLatin1Char( ' ' ))+sepS;
				}
				if (pForm==0) S = S+QString("%1").arg(*p_32,10,10,QLatin1Char( ' ' ))+sepS;
				p_s32 = (qint32*)(st_list[j]+4);
				if (pForm==1)
				{
					S = S+mas_to_damas((double)*p_s32, " ", 3)+sepS;
				}
				if (pForm==2)
				{
					S = S+QString("%1").arg((double)*p_s32/3600000,12,'f',8,QLatin1Char( ' ' ))+sepS;
				}
				if (pForm==0) S = S+QString("%1").arg(*p_s32,10,10,QLatin1Char( ' ' ))+sepS;
				p_16 = (quint16*)(st_list[j]+8);
				S = S+QString("%1").arg((double)*(p_16)/100,6,'f',3,QLatin1Char( ' ' ))+sepS;
				p_s8 = (qint8*)(st_list[j]+10);
				S = S+QString("%1").arg(*p_s8+127,3,10,QLatin1Char( ' ' ))+sepS;
				p_s8 = (qint8*)(st_list[j]+11);
				S = S+QString("%1").arg(*p_s8+127,3,10,QLatin1Char( ' ' ))+sepS;
				p_8 = (quint8*)(st_list[j]+12);
				S = S+QString("%1").arg(*p_8,3,10,QLatin1Char( ' ' ))+sepS;
				p_s8 = (qint8*)(st_list[j]+13);
				S = S+QString("%1").arg(*p_s8+127,3,10,QLatin1Char( ' ' ))+sepS;
				p_8 = (quint8*)(st_list[j]+14);
				S = S+QString("%1").arg(*p_8,3,10,QLatin1Char( ' ' ))+sepS;
				p_8 = (quint8*)(st_list[j]+15);
				S = S+QString("%1").arg(*p_8,3,10,QLatin1Char( ' ' ))+sepS;
				p_16 = (quint16*)(st_list[j]+16);
				S = S+QString("%1").arg((double)*(p_16)/1000+1975,8,'f',3,QLatin1Char( ' ' ))+sepS;
				p_16 = (quint16*)(st_list[j]+18);
				S = S+QString("%1").arg((double)*(p_16)/1000+1975,8,'f',3,QLatin1Char( ' ' ))+sepS;
				p_s32 = (qint32*)(st_list[j]+20);
				S = S+QString("%1").arg((double)*(p_s32)/10,6,'f',1,QLatin1Char( ' ' ))+sepS;
				p_s32 = (qint32*)(st_list[j]+24);
				S = S+QString("%1").arg((double)*(p_s32)/10,6,'f',1,QLatin1Char( ' ' ))+sepS;
				p_s8 = (qint8*)(st_list[j]+28);
				S = S+QString("%1").arg(((double)*p_s8+127)/10,5,'f',1,QLatin1Char( ' ' ))+sepS;
				p_s8 = (qint8*)(st_list[j]+29);
				S = S+QString("%1").arg(((double)*p_s8+127)/10,5,'f',1,QLatin1Char( ' ' ))+sepS;
				p_s8 = (qint8*)(st_list[j]+30);
				S = S+QString("%1").arg(((double)*p_s8+127)*0.05,5,'f',1,QLatin1Char( ' ' ))+sepS;
				p_s8 = (qint8*)(st_list[j]+31);
				S = S+QString("%1").arg(((double)*p_s8+127)*0.05,5,'f',1,QLatin1Char( ' ' ))+sepS;
				p_s32 = (qint32*)(st_list[j]+32);
				S = S+QString("%1").arg(*p_s32,10,10,QLatin1Char( ' ' ))+sepS;
				p_s16 = (qint16*)(st_list[j]+36);
				S = S+QString("%1").arg((double)*(p_s16)/1000,7,'f',3,QLatin1Char( ' ' ))+sepS;
				p_s16 = (qint16*)(st_list[j]+38);
				S = S+QString("%1").arg((double)*(p_s16)/1000,7,'f',3,QLatin1Char( ' ' ))+sepS;
				p_s16 = (qint16*)(st_list[j]+40);
				S = S+QString("%1").arg((double)*(p_s16)/1000,7,'f',3,QLatin1Char( ' ' ))+sepS;
				p_s8 = (qint8*)(st_list[j]+42); 
				S = S+QString("%1").arg(*p_s8+127,3,10,QLatin1Char( ' ' ))+sepS;
				p_s8 = (qint8*)(st_list[j]+43);
				S = S+QString("%1").arg(*p_s8+127,3,10,QLatin1Char( ' ' ))+sepS;
				S = S+QString("%1").arg(ucac2_n[j],10,10,QLatin1Char( ' ' ))+"\n";
				out_stream << S;
			}
			//
			delete udata;
		  }
		  else
		  {
			//QMessageBox::information(0,"debug",ufname + " has not been opened.",QMessageBox::Ok,0,0);
			outputfile.close();
			return 1;
		  }
	   }
	}
	outputfile.close();
	return 0;
};
*/
double delta_ra(double a1, double a2)
{
	return ang_b_dir(a1,0,a2,0);
}

double grad_to_mas(double angle)
{
        return angle*MAS_IN_GRAD;
}

double grad_to_rad(double angle)
{
        return angle*atan(1)*4/180.0;
}

void rot2D(double *r, double ang)
{
        double x, y;
        x = r[0];
        y = r[1];

        r[0] = cos(ang)*x + sin(ang)*y;
        r[1] = -sin(ang)*x + cos(ang)*y;
/*	r[0] = cos(ang)*y + sin(ang)*x;
        r[1] = sin(ang)*y - cos(ang)*x;*/
}





//Degree

void getDegToTang(double *ksi, double *eta, double ra, double de, double ra_c, double de_c)
{
//double rd_vector[2];
ra_c = grad_to_rad(ra_c);
de_c = grad_to_rad(de_c);
ra = grad_to_rad(ra);
de = grad_to_rad(de);
*ksi = cos(de)*sin(ra-ra_c)/(sin(de)*sin(de_c)+cos(de)*cos(de_c)*cos(ra - ra_c));
*eta = (sin(de)*cos(de_c)-cos(de)*sin(de_c)*cos(ra - ra_c))/(sin(de)*sin(de_c)+cos(de)*cos(de_c)*cos(ra - ra_c));
*ksi = rad2grad(*ksi);
*eta = rad2grad(*eta);
//return rd_vector;
};

void getTangToDeg(double *rd0, double *rd1, double ksi, double eta, double ra_c, double de_c)
{
        ra_c = grad_to_rad(ra_c);
        de_c = grad_to_rad(de_c);
        ksi = grad2rad(ksi);
        eta = grad2rad(eta);

        double secD = 1/cos(de_c);
        double commP = 1-eta*tan(de_c);
        double ra = atan2(ksi*secD, commP)+ra_c;
        double de = atan2((eta+tan(de_c))*cos(ra - ra_c),commP);
        *rd0 = rad2grad(ra);
        *rd1 = rad2grad(de);
};

void getPixPosToDegWCS(double *ra, double *de, double x, double y, double *WCSD)
{
   double ksi = (x - WCSD[0])*WCSD[8] + (y - WCSD[1])*WCSD[9];
   double eta = (x - WCSD[0])*WCSD[10] + (y - WCSD[1])*WCSD[11];
   getTangToDeg(ra, de, ksi, eta, WCSD[2], WCSD[3]);
}

void getDegToPixPosWCS(double *x, double *y, double ra, double de, double *WCSD)
{
        double* tPos;
        tPos = new double[2];
        getDegToTang(&tPos[0], &tPos[1], ra, de, WCSD[2], WCSD[3]);

        double t1 = tPos[0]; double t2 = tPos[1];
        double a1 = WCSD[8];double b1 = WCSD[9];
        double a2 = WCSD[10];double b2 = WCSD[11];
        double rPix[2];
        rPix[0] = (t2-t1*b2/b1)/(a2-a1*b2/b1);
        rPix[1] = (t1-a1*rPix[0])/b1;
        rPix[0] = rPix[0]+WCSD[0];
        rPix[1] = rPix[1]+WCSD[1];

        *x = rPix[0];
        *y = rPix[1];
};

double hms_to_deg(QString ra, QString spl_symb)
{
 ra = ra.trimmed();
 //QStringList ra_list = ra.split(spl_symb);
 QString nums;
// = ra_list.at(0);
 nums = ra.section(spl_symb, 0, 0);
 double h = nums.toDouble();
 //nums = ra_list.at(1);
 nums = ra.section(spl_symb, 1, 1);
 double m = nums.toDouble();
 //nums = ra_list.at(2);
 nums = ra.section(spl_symb, 2, 2);
 double s = nums.toDouble();
 return (h+m/60.0+s/3600.0)*15.0;
};

int hms_to_deg(double *raDeg, QString ra, QString spl_symb)
{
     ra = ra.trimmed();
     QStringList ra_list = ra.split(spl_symb);
     if(ra_list.size()!=3) return 1;
     QString nums;
    // = ra_list.at(0);
     nums = ra_list.at(0);
     double h = nums.toDouble();
     //nums = ra_list.at(1);
     nums = ra_list.at(1);
     double m = nums.toDouble();
     //nums = ra_list.at(2);
     nums = ra_list.at(2);
     double s = nums.toDouble();
     *raDeg = (h+m/60.0+s/3600.0)*15.0;
     return 0;
}

double damas_to_deg(QString de, QString spl_symb)
{
 de = de.trimmed();
 double sign;
 if (de[0]=='-') sign = -1; else sign = 1;
 //QStringList de_list = de.split(spl_symb);
 QString nums;// = de_list.at(0);
 nums = de.section(spl_symb, 0, 0);
 double deg = nums.toDouble();
 deg = fabs(deg);
 nums = de.section(spl_symb, 1, 1);//de_list.at(1);
 double arcmin = nums.toDouble();
 nums = de.section(spl_symb, 2, 2);//de_list.at(2);
 double arcsec = nums.toDouble();
 return sign*(deg+arcmin/60+arcsec/3600.0);
};

int damas_to_deg(double *deDeg, QString de, QString spl_symb)
{
     de = de.trimmed();
     double sign;
     if (de[0]=='-') sign = -1; else sign = 1;
     QStringList de_list = de.split(spl_symb);
     if(de_list.size()!=3) return 1;
     QString nums;// = de_list.at(0);
     nums = de_list.at(0);
     double deg = nums.toDouble();
     deg = fabs(deg);
     nums = de_list.at(1);
     double arcmin = nums.toDouble();
     nums = de_list.at(2);
     double arcsec = nums.toDouble();
     *deDeg = sign*(deg+arcmin/60+arcsec/3600.0);
     return 0;
};

QString deg_to_hms(double ra, QString spl_symb, int ndig)
{
 int h = (int)(ra/15);
 ra = (ra/15.0-h)*60;
 int m = (int)(ra);
 ra = (ra-m)*60;
 double s = ra;
 QString str = QString( "%1" ).arg( h,2,10,QLatin1Char( '0' ))+spl_symb;
   str = str + QString( "%1" ).arg( m,2,10,QLatin1Char( '0' ))+spl_symb;
   str = str + QString( "%1" ).arg( s,3+ndig,'f',ndig,QLatin1Char( '0' ));
  return str;
};

void deg_to_hms(QString *str, double ra, QString spl_symb, int ndig)
{
    str->clear();
 int h = (int)(ra/15);
 ra = (ra/15.0-h)*60;
 int m = (int)(ra);
 ra = (ra-m)*60;
 double s = ra;
 str->append(QString( "%1" ).arg( h,2,10,QLatin1Char( '0' ))+spl_symb);
   str->append(QString( "%1" ).arg( m,2,10,QLatin1Char( '0' ))+spl_symb);
   str->append(QString( "%1" ).arg( s,3+ndig,'f',ndig,QLatin1Char( '0' )));
  //return str;
};

QString deg_to_damas(double de, QString spl_symb, int ndig)
{
int sign = 0;
if (de<0) sign = 1;
de = fabs(de);
int d = (int)(de);
de = (de - d)*60;
int am = (int)(de);
de = (de - am)*60;
double as = de;
QString str = QString( "%1" ).arg( d,2,10,QLatin1Char( '0' ))+spl_symb;
  str = str + QString( "%1" ).arg( am,2,10,QLatin1Char( '0' ))+spl_symb;
  str = str + QString( "%1" ).arg( as,3+ndig,'f',ndig,QLatin1Char( '0' ));
  if (sign==1)
    str = "-"+str;
  else
    str = "+"+str;
  return str;
};

void deg_to_damas(QString *str, double de, QString spl_symb, int ndig)
{
    str->clear();
int sign = 0;
if (de<0) sign = 1;
de = fabs(de);
int d = (int)(de);
de = (de - d)*60;
int am = (int)(de);
de = (de - am)*60;
double as = de;
str->append(QString( "%1" ).arg( d,2,10,QLatin1Char( '0' ))+spl_symb);
  str->append(QString( "%1" ).arg( am,2,10,QLatin1Char( '0' ))+spl_symb);
  str->append(QString( "%1" ).arg( as,3+ndig,'f',ndig,QLatin1Char( '0' )));

  if(sign==1)str->prepend("-");
    //str = "-"+str;
  else str->prepend("+");
    //str = "+"+str;
  //return str;
};

double grad2rad(double grad)
{
    return(grad*PI/180.0);
}

double rad2grad(double rad)
{
    return(rad*180.0/PI);
}

double rad_to_mas(double angle)
{
        return angle*(180*3600000)/PI;
};

double rad2mas(double rad)
{
    return(rad*SECINRAD*1000.0);
}

double mas2rad(double mas)
{
    return(mas/1000.0/SECINRAD);
}


/*
void getPlateName(QString origName, QString *plName, int plNameType)
{
    QStringList fList;
    int sNum;
    plName->clear();

    qDebug() << QString("origName: %1\n").arg(origName);

    fList = origName.split("/");
    sNum = fList.size();
    qDebug() << QString("sNum: %1\n").arg(sNum);
    if(sNum==1)fList = origName.split("\\");
    sNum = fList.size();
    qDebug() << QString("sNum: %1\n").arg(sNum);

    switch(plNameType)
    {
    case 0:
       if(sNum>1)plName->insert(0, fList.at(sNum-2));
       else plName->insert(0, "");
       break;
    case 1:
       plName->insert(0, fList.at(sNum-1).section(".", 0, 0));
       break;
    }



}
*/



double getS(DATEOBS date_obs)
{
        double d_day = date_obs.day;
        double d_hour = date_obs.hour;
        double d_min = date_obs.min;
        double d_sec = date_obs.sec;
        d_day = d_hour*3600+d_min*60+d_sec;
        DATEOBS gm = date_obs;
        gm.hour = 0; gm.min = 0; gm.sec = 0;
        double JD = getJD(gm);
        double T=(JD-2451545)/36525;
        //double S0=6*3600+41*60+50.54841+8640184.812866*T+0.093104*T*T-0.0000062*T*T*T;
        double S0=6*3600+41*60+50.54841+236.555367908*(JD-2451545)+0.093104*T*T-0.0000062*T*T*T;
        S0=S0-floor(S0/86400)*86400;
        double S = S0+d_day*366.25/365.25+7278;
        if (S>86400){S=S-86400;}
        return S;
};

QString getStrFromS(double s, QString spl_symb, int ndig)
{
  int h = (int)(s/3600);
  s = s-h*3600;
  int m = (int)(s/60);
  s = s-m*60;
  QString str = QString( "%1" ).arg( h,2,10,QLatin1Char( '0' ))+spl_symb;
   str = str + QString( "%1" ).arg( m,2,10,QLatin1Char( '0' ))+spl_symb;
   if (ndig!=0) str = str + QString( "%1" ).arg( s,3+ndig,'f',ndig,QLatin1Char( '0' ));
       else     str = str + QString( "%1" ).arg( int(s),2,10,QLatin1Char( '0' ));
  return str;
}

QString getRaDecMag(QString str)
{
  bool fl = false;
  for(int i=0;i<str.length();i++)
  {
        if((str[i]==' ')||(str[i]=='+')||(str[i]=='-'))
        {
           if(fl)
           {
            if ((str[i]!='+')&&(str[i]!='-'))
                {
                        str[i] = '0';
                        fl = false;
                }
           }
           else
           {
            fl = true;
           }
        }
        else fl = false;
  }
  return str;
};

QString getStrFromDE(double de, QString spl_symb)
{
        int d = (int)(de/3600);
        de = de - d*3600;
        int am = (int)(de/60);
        de = de - am*60;
        int as = (int)de;
    QString str = QString( "%1" ).arg( d,2,10,QLatin1Char( '0' ))+spl_symb;
     str = str + QString( "%1" ).arg( am,2,10,QLatin1Char( '0' ))+spl_symb;
     str = str + QString( "%1" ).arg( as,2,10,QLatin1Char( '0' ));
    return str;
};



/*
#ifndef COMF_H


double rad_to_mas(double angle)
{
  return angle*(180*3600000)/acos(-1);
};

double grad2rad(double grad)
{
        return(grad*PI/180.0);
}

double rad2grad(double rad)
{
        return(rad*180.0/PI);
}






int streqv(char *str1, char *str2)
{
        int len1 = strlen(str1);
        int len2 = strlen(str2);
        if(len1!=len2) return 0;
        for(int i=0; i<len1; i++)
        {
                if(strcmp(&str1[i], &str2[i])!=NULL) return 0;
        }

        return 1;
}

int slovo_stopsim(char *str, char *slo, char* stopsim, int posb, int *pose, int nmax)
{
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

        *Jday = floor(365.25*y) + floor(30.6001*(m+1)) + day + 1720994.5 + B;

        return 0;
}

int dat2YMD(double Jday, int *year, int *mth, double *day)
{
        double x, Z, F, q, A, B, C, D, E;

        x = Jday + 0.5;
        Z = (int) x;//floor(x);
        F = x - Z;

        q = floor((Z - 1867216.25)/36524.25);
        A = Z + 1 + q - q/4;
        B = A + 1524;
        C = floor((B - 122.1)/365.25);
        D = floor(365.25*C);
        E = floor((B - D)/30.6001);
        *day = B - D - floor(30.6001*E) + F;

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

void dat2YMD_str(double JD, char *str)
{
        if(str==NULL) return;

        int y, m;
        double day;

        dat2YMD(JD, &y, &m, &day);

        sprintf(str, "%4d %02d %010.7f", y, m, day);
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

int translit(char *str_rus, char *str_tr)
{
        char *sim_tr = new char[2];

        int slen = strlen(str_rus);



        delete [] sim_tr;

        return 0;
}

int translit_dict(char *ru, char *tr)
{
        if(streqv(ru, "А")) strcpy(tr, "A");
        else if(streqv(ru, "Б")) strcpy(tr, "B");
        else if(streqv(ru, "В")) strcpy(tr, "V");
        else if(streqv(ru, "Г")) strcpy(tr, "G");
        else if(streqv(ru, "Д")) strcpy(tr, "D");
        else if(streqv(ru, "Е")) strcpy(tr, "E");
        else if(streqv(ru, "Ё")) strcpy(tr, "JO");
        else if(streqv(ru, "Ж")) strcpy(tr, "ZH");
        else if(streqv(ru, "З")) strcpy(tr, "Z");
        else if(streqv(ru, "И")) strcpy(tr, "I");
        else if(streqv(ru, "Й")) strcpy(tr, "J");
        else if(streqv(ru, "К")) strcpy(tr, "K");
        else if(streqv(ru, "Л")) strcpy(tr, "L");
        else if(streqv(ru, "М")) strcpy(tr, "M");
        else if(streqv(ru, "Н")) strcpy(tr, "N");
        else if(streqv(ru, "О")) strcpy(tr, "O");
        else if(streqv(ru, "П")) strcpy(tr, "P");
        else if(streqv(ru, "Р")) strcpy(tr, "R");
        else if(streqv(ru, "С")) strcpy(tr, "S");
        else if(streqv(ru, "Т")) strcpy(tr, "T");
        else if(streqv(ru, "У")) strcpy(tr, "U");
        else if(streqv(ru, "Ф")) strcpy(tr, "F");
        else if(streqv(ru, "Х")) strcpy(tr, "KH");
        else if(streqv(ru, "Ц")) strcpy(tr, "C");
        else if(streqv(ru, "Ч")) strcpy(tr, "CH");
        else if(streqv(ru, "Ш")) strcpy(tr, "SH");
        else if(streqv(ru, "Щ")) strcpy(tr, "SH");
        else if(streqv(ru, "Ь")) strcpy(tr, "");
        else if(streqv(ru, "Ы")) strcpy(tr, "Y");
        else if(streqv(ru, "Ъ")) strcpy(tr, "");
        else if(streqv(ru, "Э")) strcpy(tr, "E");
        else if(streqv(ru, "Ю")) strcpy(tr, "JU");
        else if(streqv(ru, "Я")) strcpy(tr, "YA");
        else return 1;

        return 0;
}


#endif
*/

