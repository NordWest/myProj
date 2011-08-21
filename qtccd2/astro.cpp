#include <QtCore>
#include <QtGui>
#include "astro.h"

double mas_to_rad(double angle)
{
  return acos(-1)*angle/(180*3600000);
};

double rad_to_mas(double angle)
{
  return angle*(180*3600000)/acos(-1);
};

double ang_b_dir(double a1, double d1, double a2, double d2)
{
  vector<double> r1 (3);
        r1(0) = cos(mas_to_rad(a1))*cos(mas_to_rad(d1)); 
		r1(1) = sin(mas_to_rad(a1))*cos(mas_to_rad(d1));
		r1(2) = sin(mas_to_rad(d1));
  vector<double> r2 (3);
        r2(0) = cos(mas_to_rad(a2))*cos(mas_to_rad(d2)); 
		r2(1) = sin(mas_to_rad(a2))*cos(mas_to_rad(d2));
		r2(2) = sin(mas_to_rad(d2));
    return rad_to_mas(acos(inner_prod(r1,r2)));
};

double hms_to_mas(QString ra, QString spl_symb)
{
 ra = ra.trimmed(); 
 QStringList ra_list = ra.split(spl_symb);
 QString nums = ra_list.at(0);
 double h = nums.toDouble();
 nums = ra_list.at(1);
 double m = nums.toDouble();
 nums = ra_list.at(2);
 double s = nums.toDouble();
 return (h*3600+m*60+s)*15000;
};

double damas_to_mas(QString de, QString spl_symb)
{
 de = de.trimmed();
 double sign;
 if (de[0]=='-') sign = -1; else sign = 1; 
 QStringList de_list = de.split(spl_symb);
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


double getJD(DATEOBS date_obs)
{
double d_day = date_obs.day;
double d_hour = date_obs.hour;
double d_min = date_obs.min;
double d_sec = date_obs.sec;
d_day = d_day + (d_hour+(d_min+d_sec/60)/60)/24;
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

double getYearFromMJD(double mjd)
{
   return 1858.87885010267 + mjd/365.25;
};

DATEOBS getDATEOBSfromMJD(double mjd)
{
   DATEOBS date_obs;
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
   date_obs.year = Year;
   date_obs.month = Month;
   date_obs.day = Day;
   date_obs.pday = Day+parth_of_day;
   date_obs.hour = (int)floor(parth_of_day*24);
   date_obs.min = (int)floor(parth_of_day*1440) - date_obs.hour*60;
   date_obs.sec = parth_of_day*86400 - date_obs.hour*3600 - date_obs.min*60;
   return date_obs;
};

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


QString getStrFromDATEOBS(DATEOBS date_obs, QString spl_symb, int format, int ndig)
{
  QString str;
  if (format == 0)
   {
	  str = QString( "%1" ).arg( date_obs.year,4,10,QLatin1Char( '0' ))+spl_symb;
	  str = str + QString( "%1" ).arg( date_obs.month,2,10,QLatin1Char( '0' ))+spl_symb;
	  str = str + QString( "%1" ).arg( date_obs.pday,3+ndig,'f',ndig,QLatin1Char( '0' )); 
    }
	else
	{
	  str = QString( "%1" ).arg( date_obs.year,4,10,QLatin1Char( '0' ))+spl_symb;
	  str = str + QString( "%1" ).arg( date_obs.month,2,10,QLatin1Char( '0' ))+spl_symb;
	  str = str + QString( "%1" ).arg( date_obs.day,2,10,QLatin1Char( '0' ))+" ";
	  str = str + QString( "%1" ).arg( date_obs.hour,2,10,QLatin1Char( '0' ))+spl_symb;
	  str = str + QString( "%1" ).arg( date_obs.min,2,10,QLatin1Char( '0' ))+spl_symb;
	  str = str + QString( "%1" ).arg( date_obs.sec,3+ndig,'f',ndig,QLatin1Char( '0' ));
	}
  return str;
};

double getAngleDeg(double cos_x, QString spl_symb, double sin_x)
{
  double PI = atan(1)*4;
  double ang = 180*atan2(cos_x, sin_x)/PI;
  if (ang<0)ang = 360 + ang;
  return ang;
};

matrix<double> getUCAC2data(double ra_c, double de_c, double rho, int epobs, double mjd, QString path, double mag1, double mag2)
//all positional parameters must be in mas!
{
   matrix<double> u2;
   QString debug_str;
   //calculation of positional bins
   double ra1,ra2,de1,de2,curr_ra1,curr_ra2,ra,de,mag;
   quint32 *p_32;
   qint32 *p_s32;
   quint16 *p_16;
   qint16 *p_s16;
   quint8  *p_8;
   qint8  *p_s8;
   if (de_c + rho > 324000000)
   {
     ra1 = 0; ra2 = 1296000000; de1 = de_c - rho; de2 = 324000000;
   }
   if (de_c + rho == 324000000)
   {
     ra1 = ra_c - 324000000;
     if (ra1<0) ra1 = ra1 + 1296000000;	 
	 ra2 = ra_c + 324000000;
     if (ra2>1296000000) ra2 = ra2 - 1296000000;	 
	 de1 = de_c - rho; de2 = 324000000;
   }
   if (de_c + rho < -324000000)
   {
     ra1 = 0; ra2 = 1296000000; de2 = de_c + rho; de1 = -324000000;
   }
   if (de_c + rho == -324000000)
   {
     ra1 = ra_c - 324000000;
     if (ra1<0) ra1 = ra1 + 1296000000;	 
	 ra2 = ra_c + 324000000;
     if (ra2>1296000000) ra2 = ra2 - 1296000000;	 
	 de2 = de_c + rho; de1 = -324000000;
   }
   if((de_c+rho<324000000)&&(de_c-rho>-324000000))
   {
       de1 = de_c - rho; de2 = de_c + rho;
       double dra = rad_to_mas(asin(sin(mas_to_rad(rho))/cos(mas_to_rad(de_c))));
       ra1 = ra_c - dra;
       if (ra1 < 0) ra1 = ra1 + 1296000000;
       ra2 = ra_c + dra;
       if (ra2>1296000000) ra2 = ra2 - 1296000000;	   
   }
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
	 QMessageBox::information(0,"debug","u2index.da has not been opened.",QMessageBox::Ok,0,0);
	 return u2;
   }
   QFile u2file;
   char *udata;
   quint32 No,N,Ns;
   QList<char*> st_list;
   QVector<quint32> ucac2_n;
   int startIdx;
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
		  QMessageBox::information(0,"debug","out of ucac2 zone",QMessageBox::Ok,0,0);
		  return u2;
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
				if((ang_b_dir(ra, de, ra_c, de_c)<=rho)&&(mag>=mag1)&&(mag<=mag2))
				{
				   st_list << udata+j*44;
				   ucac2_n << N;
				}
			}
			startIdx = u2.size1();
			u2.resize(startIdx+st_list.count(),24);
			for (j=0;j<st_list.count();j++)
			{
				p_32 = (quint32*)(st_list[j]); u2(startIdx+j,0) = (double)*p_32;
				p_s32 = (qint32*)(st_list[j]+4); u2(startIdx+j,1) = (double)*p_s32;
				p_16 = (quint16*)(st_list[j]+8); u2(startIdx+j,2) = (double)*(p_16)/100;
				p_s8 = (qint8*)(st_list[j]+10); u2(startIdx+j,3) = (double)*(p_s8)+127;
				p_s8 = (qint8*)(st_list[j]+11); u2(startIdx+j,4) = (double)*(p_s8)+127;
				p_8 = (quint8*)(st_list[j]+12); u2(startIdx+j,5) = (double)*p_8;
				p_s8 = (qint8*)(st_list[j]+13); u2(startIdx+j,6) = (double)*(p_s8)+127;
				p_8 = (quint8*)(st_list[j]+14); u2(startIdx+j,7) = (double)*p_8;
				p_8 = (quint8*)(st_list[j]+15); u2(startIdx+j,8) = (double)*p_8;
				p_16 = (quint16*)(st_list[j]+16); u2(startIdx+j,9) = (double)*p_16/1000 + 1975;
				p_16 = (quint16*)(st_list[j]+18); u2(startIdx+j,10) = (double)*p_16/1000 + 1975;
				p_s32 = (qint32*)(st_list[j]+20); u2(startIdx+j,11) = (double)*p_s32/10;
				p_s32 = (qint32*)(st_list[j]+24); u2(startIdx+j,12) = (double)*p_s32/10;
				p_s8 = (qint8*)(st_list[j]+28); u2(startIdx+j,13) = ((double)*p_s8+127)/10;
				p_s8 = (qint8*)(st_list[j]+29); u2(startIdx+j,14) = ((double)*p_s8+127)/10;
				p_s8 = (qint8*)(st_list[j]+30); u2(startIdx+j,15) = ((double)*p_s8+127)*0.05;
				p_s8 = (qint8*)(st_list[j]+31); u2(startIdx+j,16) = ((double)*p_s8+127)*0.05;
				p_s32 = (qint32*)(st_list[j]+32); u2(startIdx+j,17) = (double)*p_s32;
				p_s16 = (qint16*)(st_list[j]+36); u2(startIdx+j,18) = (double)*p_s16/1000;
				p_s16 = (qint16*)(st_list[j]+38); u2(startIdx+j,19) = (double)*p_s16/1000;
				p_s16 = (qint16*)(st_list[j]+40); u2(startIdx+j,20) = (double)*p_s16/1000;
				p_s8 = (qint8*)(st_list[j]+42); u2(startIdx+j,21) = (double)*(p_s8)+127;
				p_s8 = (qint8*)(st_list[j]+43); u2(startIdx+j,22) = (double)*(p_s8)+127;
				u2(startIdx+j,23) = (double)ucac2_n[j];
				if (epobs)
				{
				 u2(startIdx+j,0) = u2(startIdx+j,0) + u2(startIdx+j,11)*(getYearFromMJD(mjd) - 2000);
				 u2(startIdx+j,1) = u2(startIdx+j,1) + u2(startIdx+j,12)*(getYearFromMJD(mjd) - 2000);
				}
			}
			//
			delete udata;
		  }
		  else
		  {
			QMessageBox::information(0,"debug",ufname + " has not been opened.",QMessageBox::Ok,0,0);
			return u2;
		  }
	   }
	}
	return u2;
};


matrix<double> getUSNOBdata(double ra_c, double de_c, double rho, int epobs, double mjd, QString path, double mag1, double mag2)
{
   matrix<double> usnob;
   QString debug_str;
   //calculation of positional bins
   double ra1,ra2,de1,de2,ra,de,mag;
   /////////////////////////////////////////////
   if (de_c + rho > 324000000)
   {
     ra1 = 0; ra2 = 1296000000; de1 = de_c - rho; de2 = 324000000;
   }
   if (de_c + rho == 324000000)
   {
     ra1 = ra_c - 324000000;
     if (ra1<0) ra1 = ra1 + 1296000000;	 
	 ra2 = ra_c + 324000000;
     if (ra2>1296000000) ra2 = ra2 - 1296000000;	 
	 de1 = de_c - rho; de2 = 324000000;
   }
   if (de_c + rho < -324000000)
   {
     ra1 = 0; ra2 = 1296000000; de2 = de_c + rho; de1 = -324000000;
   }
   if (de_c + rho == -324000000)
   {
     ra1 = ra_c - 324000000;
     if (ra1<0) ra1 = ra1 + 1296000000;	 
	 ra2 = ra_c + 324000000;
     if (ra2>1296000000) ra2 = ra2 - 1296000000;	 
	 de2 = de_c + rho; de1 = -324000000;
   }
   if((de_c+rho<324000000)&&(de_c-rho>-324000000))
   {
       de1 = de_c - rho; de2 = de_c + rho;
       double dra = rad_to_mas(asin(sin(mas_to_rad(rho))/cos(mas_to_rad(de_c))));
       ra1 = ra_c - dra;
       if (ra1 < 0) ra1 = ra1 + 1296000000;
       ra2 = ra_c + dra;
       if (ra2>1296000000) ra2 = ra2 - 1296000000;	   
   }
   ///////////////////////
   int spd1 = (int)floor(900+(de1)/360000);//south polar distance in 0.1 deg
   int spd2 = (int)floor(900+(de2)/360000);
   if (spd1<0)spd1=0;
   if (spd1>=1800)spd2=1799;
   ///////////////////////////////
   int k = 1;
   int zra1 = (int)floor((ra1)/(3600000*15*0.25));
   int zra2 = (int)floor((ra2)/(3600000*15*0.25));
   int zone_ra1[2];
   int zone_ra2[2];
   zone_ra1[0] = zra1;
   zone_ra2[0] = zra2;
   if (zra1<0)
   {
     k =2;
     zone_ra1[0] = (int)floor((ra1+360*3600000)/(3600000*15*0.25));
     zone_ra2[0] = 95;
     zone_ra1[1] = 0;
     zone_ra2[1] = zra2;	 
   }//end if
   if (zra2>95)
   {
     k =2;
     zone_ra1[0] = zra1;
     zone_ra2[0] = 95;
     zone_ra1[1] = 0;
     zone_ra2[1] = (int)floor((ra2-360*3600000)/(3600000*15*0.25));	 
   }//end if
   ///////////////////////////////
   QVector<int> st_list;
   bool ok;//needed for converting
   QStringList accList;//the accelerators list
   QString SPDdir;// the directory according to South Polar Distance
   QString nums;//variable for converting
   QString fname;//current usnob file name
   QString accline;// current line of accelerators list
   QFile accfile, catfile;
   QTextStream in_acc, out_stream;
   quint64 start_acc, final_acc, bsize, star_index, buffsize;
   quint32 *par4b;
   double p4b,a,d;
   double mu_ra,mu_de,sra,sde,smu_ra,smu_de,bmag1,bmag2,rmag1,rmag2,sgf_b1,sgf_b2,sgf_r1,sgf_r2,epoch,ys4,Nmag,sgf_N;
   //QMessageBox::information(0,"debug","ok2",QMessageBox::Ok,0,0);
   int startIndex = 0;
   ///
   for (int i=spd1;i<=spd2;i++)
   {
       // construction of the file name
       nums = QString( "%1" ).arg( i, 4, 10,QLatin1Char( '0' ) );//converting to "001" from 1
	   fname = path+"/";
	   SPDdir=nums.mid(0,3);
	   fname+=SPDdir;
	   fname+="/b";
	   fname+=nums;
	   //load file into the list
	   accfile.setFileName(fname+".acc");
       if (accfile.open(QIODevice::ReadOnly | QIODevice::Text))
	   {
	       in_acc.setDevice(&accfile);
	       while (!in_acc.atEnd()) {accList << in_acc.readLine();}
	       accfile.close();
		  for (int g=0;g<k;g++)
          {	  
	        accline = accList.at(zone_ra1[g]);
	        nums = accline.mid(5,12);
	        start_acc = nums.toULongLong(&ok,10);
	        accline = accList.at(zone_ra2[g]);
	        nums = accline.mid(5,12);
	        final_acc = nums.toULongLong(&ok,10);
		    nums = accline.mid(17,12);
	        final_acc = final_acc+nums.toULongLong(&ok,10);
		    //load data from catalog file
		    catfile.setFileName(fname+".cat");
		    if (catfile.open(QIODevice::ReadOnly))
	          {
				catfile.seek((start_acc-1)*80);
			    bsize = final_acc-start_acc;
				buffsize = bsize*80;
			    char* catdata = new char[buffsize];
			    catfile.read(catdata,bsize*80);
				catfile.close();
			    par4b = (quint32*)catdata;
				//startIndex = usnob.size1();
				//usnob.resize(startIndex+bsize,20);
				st_list.clear();
			    for (quint64 j=0;j<bsize;j++)
                  {
				    star_index=j*20;
                    //ra = (double)par4b[star_index]/360000;
					ra = (double)par4b[star_index]*10;
                    //de = (double)par4b[star_index+1]/360000-90;
					de = (double)par4b[star_index+1]*10-90*3600000;
					p4b = (double)par4b[star_index+5];
                    mag = (p4b/10000-floor(p4b/10000))*100;
					if ((ang_b_dir(ra_c, de_c, ra, de)<=rho)&&(mag>=mag1)&&(mag<=mag2))
					        st_list << star_index;
				  }//end of star processing
				startIndex = usnob.size1();
				usnob.resize(startIndex+st_list.count(),20);
				for (int i=0;i<st_list.count();i++)
				{
				    {
					   star_index = st_list[i];
					   ra = (double)par4b[star_index]*10;
					   de = (double)par4b[star_index+1]*10-90*3600000;
					   usnob(startIndex+i,0) = ra, usnob(startIndex+i,1) = de;
					   //QMessageBox::information(0,"debug",mas_to_hms(ra, ":", 2),QMessageBox::Ok,0,0);
					   //QMessageBox::information(0,"debug","ok2",QMessageBox::Ok,0,0);
                       p4b = (double)par4b[star_index+2];
					   mu_ra = (p4b/10000-floor(p4b/10000))*20000-10000;usnob(startIndex+i,2) = mu_ra;
					   mu_de = floor(p4b/10000);
					   mu_de = (mu_de/10000-floor(mu_de/10000))*20000-10000;usnob(startIndex+i,3) = mu_de;
					   p4b = (double)par4b[star_index+3];
					   smu_ra = (p4b/1000-floor(p4b/1000))*1000;usnob(startIndex+i,4) = smu_ra;
					   smu_de = floor(p4b/1000);
					   smu_de = (smu_de/1000-floor(smu_de/1000))*1000;usnob(startIndex+i,5) = smu_de;
					   p4b = (double)par4b[star_index+4];
					   sra = (p4b/1000-floor(p4b/1000))*1000;usnob(startIndex+i,6) = sra;
					   sde = floor(p4b/1000);
					   sde = (sde/1000-floor(sde/1000))*1000;usnob(startIndex+i,7) = sde;
					   epoch = floor(p4b/1000000);
                       epoch = (epoch/1000-floor(epoch/1000))*100+1950;usnob(startIndex+i,8) = epoch;
                       ys4 = floor(p4b/1000000000);usnob(startIndex+i,9) = ys4;
                       p4b = (double)par4b[star_index+5];
                       bmag1 = (p4b/10000-floor(p4b/10000))*100;usnob(startIndex+i,10) = bmag1;
                       sgf_b1 = floor(p4b/100000000);usnob(startIndex+i,11) = sgf_b1;
                       p4b = (double)par4b[star_index+6];
                       rmag1 = (p4b/10000-floor(p4b/10000))*100;usnob(startIndex+i,12) = rmag1;
                       sgf_r1 = floor(p4b/100000000);usnob(startIndex+i,13) = sgf_r1;
                       p4b = (double)par4b[star_index+7];
                       bmag2 = (p4b/10000-floor(p4b/10000))*100;usnob(startIndex+i,14) = bmag2;
                       sgf_b2 = floor(p4b/100000000);usnob(startIndex+i,15) = sgf_b2;
                       p4b = (double)par4b[star_index+8];
                       rmag2 = (p4b/10000-floor(p4b/10000))*100;usnob(startIndex+i,16) = rmag2;
                       sgf_r2 = floor(p4b/100000000);usnob(startIndex+i,17) = sgf_r2;
                       p4b = (double)par4b[star_index+9];
                       Nmag = (p4b/10000-floor(p4b/10000))*100;usnob(startIndex+i,18) = Nmag;
                       sgf_N = floor(p4b/100000000);usnob(startIndex+i,19) = sgf_N;					
					   ///////////////////////////////////////
					   if (epobs)
						{
						 usnob(startIndex+i,0) = ra + mu_de*(getYearFromMJD(mjd) - 2000)/cos(mas_to_rad(de));
						 usnob(startIndex+i,1) = de + mu_ra*(getYearFromMJD(mjd) - 2000);
						}
                    }//end of selecting of star
				}
			    delete catdata;
			    }//end load catalog file
			}
			//QMessageBox::information(0,"debug","ok",QMessageBox::Ok,0,0);
			accList.clear();
        }//end of reading data		  
   }//end for i
   return usnob;
   ///////////////////////////////
};

matrix<double> getTangPos(matrix<double> rc, double ra_c, double de_c, int Nra, int Nde, int Nmag)
{
ra_c = mas_to_rad(ra_c);
de_c = mas_to_rad(de_c);
matrix<double> tp;
tp.resize(rc.size1(),3);
double ksi,eta,ra,de;
for (int i=0;i<tp.size1();i++)
{
  ra = rc(i,Nra);
  de = rc(i,Nde); 
  ra = mas_to_rad(ra);
  de = mas_to_rad(de);
  ksi = cos(de)*sin(ra-ra_c)/(sin(de)*sin(de_c)+cos(de)*cos(de_c)*cos(ra - ra_c));
  eta = (sin(de)*cos(de_c)-cos(de)*sin(de_c)*cos(ra - ra_c))/(sin(de)*sin(de_c)+cos(de)*cos(de_c)*cos(ra - ra_c));
  tp(i,0) = rad_to_mas(ksi)/1000;
  tp(i,1) = rad_to_mas(eta)/1000;
  tp(i,2) = rc(i,Nmag);
}
return tp;
};

vector<double> getTangToRaDe(double ksi, double eta, double ra_c, double de_c)
{
    vector<double> rd_vector(2);
	ra_c = mas_to_rad(ra_c);
	de_c = mas_to_rad(de_c);
	ksi = mas_to_rad(ksi*1000);
    eta = mas_to_rad(eta*1000);
	double secD = 1/cos(de_c);
	double commP = 1-eta*tan(de_c);
	double ra = atan((ksi*secD)/(commP))+ra_c;
	double de = atan((eta+tan(de_c))*cos(ra - ra_c)/commP);
	rd_vector(0) = rad_to_mas(ra);
	rd_vector(1) = rad_to_mas(de);
	return rd_vector;
};

vector<double> getRaDeToTang(double ra, double de, double ra_c, double de_c)
{
vector<double> rd_vector(2);
ra_c = mas_to_rad(ra_c);
de_c = mas_to_rad(de_c);
ra = mas_to_rad(ra);
de = mas_to_rad(de);
double ksi = cos(de)*sin(ra-ra_c)/(sin(de)*sin(de_c)+cos(de)*cos(de_c)*cos(ra - ra_c));
double eta = (sin(de)*cos(de_c)-cos(de)*sin(de_c)*cos(ra - ra_c))/(sin(de)*sin(de_c)+cos(de)*cos(de_c)*cos(ra - ra_c));
rd_vector(0) = rad_to_mas(ksi)/1000;
rd_vector(1) = rad_to_mas(eta)/1000;
return rd_vector;
};

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