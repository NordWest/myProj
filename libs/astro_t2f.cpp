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

double getAngleDeg(double cos_x, double sin_x)
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

//My

	
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
*/
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
				status = 0;*/
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