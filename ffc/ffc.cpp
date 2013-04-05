#include "ffc.h"

#include "math.h"


double getMJD(double yr, double mm, double dd, double hr, double min, double sec)
{
    double d_day = dd + (hr+(min+sec/60)/60)/24;
    int y;
    int m;
    if (mm<3)
    {
             y =yr-1;
             m =mm+12;
    }
    else
    {
             y =yr;
             m =mm;
    }
    int A = y/100;
    double B = 2-A+floor(A/4);
    double jd =floor(365.25*y)+floor(30.6001*(m+1))+d_day+1720994.5+B;
    return jd-2400000.5;
}

double getMJDfromStr(QString str)
{
    double yr,mm,dd,hr,min,sec;
    yr = str.section('T',0,0).section('-',0,0).toDouble();
    mm = str.section('T',0,0).section('-',1,1).toDouble();
    dd = str.section('T',0,0).section('-',2,2).toDouble();
    hr = str.section('T',1,1).section(':',0,0).toDouble();
    min = str.section('T',1,1).section(':',1,1).toDouble();
    sec = str.section('T',1,1).section(':',2,2).toDouble();
    return getMJD(yr,mm,dd,hr,min,sec);
}

QString DATEfromMJD(double mjd, QString spl_symb, int format, int ndig)
{
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
   int year = Year;
   int month = Month;
   int day = Day;
   double pday = Day+parth_of_day;
   int hour = (int)floor(parth_of_day*24);
   int min = (int)floor(parth_of_day*1440) - hour*60;
   double sec = parth_of_day*86400 - hour*3600 - min*60;
   QString str;
   switch(format)
   {
     case 0:
     {
       str = QString( "%1" ).arg( year,4,10,QLatin1Char( '0' ))+spl_symb;
       str = str + QString( "%1" ).arg( month,2,10,QLatin1Char( '0' ))+spl_symb;
       str = str + QString( "%1" ).arg( pday,3+ndig,'f',ndig,QLatin1Char( '0' ));
       break;
     }
     case 1:
     {
       str = QString( "%1" ).arg( year,4,10,QLatin1Char( '0' ))+spl_symb;
       str = str + QString( "%1" ).arg( month,2,10,QLatin1Char( '0' ))+spl_symb;
       str = str + QString( "%1" ).arg( day,2,10,QLatin1Char( '0' ))+"-";
       str = str + QString( "%1" ).arg( hour,2,10,QLatin1Char( '0' ))+spl_symb;
       str = str + QString( "%1" ).arg( min,2,10,QLatin1Char( '0' ))+spl_symb;
       if(ndig>0)
           str = str + QString( "%1" ).arg( sec,3+ndig,'f',ndig,QLatin1Char( '0' ));
       else
           str = str + QString( "%1" ).arg( sec,2,'f',0,QLatin1Char( '0' ));
       break;
     }
     case 2:
     {
       str = QString( "%1" ).arg( year,4,10,QLatin1Char( '0' ))+"-";
       str = str + QString( "%1" ).arg( month,2,10,QLatin1Char( '0' ))+"-";
       str = str + QString( "%1" ).arg( day,2,10,QLatin1Char( '0' ))+"T";
       str = str + QString( "%1" ).arg( hour,2,10,QLatin1Char( '0' ))+":";
       str = str + QString( "%1" ).arg( min,2,10,QLatin1Char( '0' ))+":";
       if(ndig>0)
           str = str + QString( "%1" ).arg( sec,3+ndig,'f',ndig,QLatin1Char( '0' ));
       else
           str = str + QString( "%1" ).arg( sec,2,'f',0,QLatin1Char( '0' ));
       break;
     }
   }
   return str;
}


QString RAtoHMS(double ra, QString spl, int ndig)
{
    double radPerHr = M_PI/12;
    double radPerMin = M_PI/(12*60);
    double radPerSec = M_PI/(12*3600);
     int h = (int)floor(ra/radPerHr);
     ra = ra-h*radPerHr;
     int m = (int)floor(ra/radPerMin);
     ra = ra-m*radPerMin;
     double s = ra/radPerSec;
     QString str = QString( "%1" ).arg( h,2,10,QLatin1Char( '0' ))+spl;
       str = str + QString( "%1" ).arg( m,2,10,QLatin1Char( '0' ))+spl;
       str = str + QString( "%1" ).arg( s,3+ndig,'f',ndig,QLatin1Char( '0' ));
      return str;
}

QString DEtoDAMAS(double de, QString spl, int ndig)
{
    double radPerDeg = M_PI/180;
    double radPerArcMin = M_PI/(180*60);
    double radPerArcSec = M_PI/(180*3600);
    int sign = 0;
    if (de<0) sign = 1;
    de = fabs(de);
    int d = (int)floor(de/radPerDeg);
    de = de - d*radPerDeg;
    int am = (int)floor(de/radPerArcMin);
    de = de - am*radPerArcMin;
    double as = de/radPerArcSec;
    QString str = QString( "%1" ).arg( d,2,10,QLatin1Char( '0' ))+spl;
      str = str + QString( "%1" ).arg( am,2,10,QLatin1Char( '0' ))+spl;
      str = str + QString( "%1" ).arg( as,3+ndig,'f',ndig,QLatin1Char( '0' ));
      if (sign==1)
        str = "-"+str;
      else
        str = "+"+str;
      return str;
}

double HMStoRA(QString strRA, QString sep)
{
    double ra = strRA.section(sep,0,0).toDouble()+
                strRA.section(sep,1,1).toDouble()/60+
                strRA.section(sep,2,2).toDouble()/3600;
    return ra*M_PI/12;
}

double DAMAStoDE(QString strDE, QString sep)
{
    double sign;
    if(strDE.contains('-') )sign=-1;else sign=1;
    double de = fabs(strDE.section(sep,0,0).toDouble())+
                strDE.section(sep,1,1).toDouble()/60+
                strDE.section(sep,2,2).toDouble()/3600;
    return sign*de*M_PI/180;
}



void tangFromRADE(double ra, double de, double RA, double DE, double& ksi, double& eta)
{
    ksi=cos(de)*sin(ra-RA)/(sin(de)*sin(DE)+cos(de)*cos(DE)*cos(ra-RA));
    eta=(sin(de)*cos(DE)-cos(de)*sin(DE)*cos(ra-RA))/(sin(de)*sin(DE)+cos(de)*cos(DE)*cos(ra-RA));
}

void RADEFromTang(double ksi, double eta, double RA, double DE, double& ra, double& de)
{
    double secRho = sqrt(1+ksi*ksi+eta*eta);
    double t11 = -sin(RA);
    double t12 = cos(RA);
    double t13 = 0;
    double t21 = -cos(RA)*sin(DE);
    double t22 = -sin(RA)*sin(DE);
    double t23 = cos(DE);
    double t31 = cos(RA)*cos(DE);
    double t32 = sin(RA)*cos(DE);
    double t33 = sin(DE);
    double x = (ksi*t11+eta*t21+t31)/secRho;
    double y = (ksi*t12+eta*t22+t32)/secRho;
    double z = (ksi*t13+eta*t23+t33)/secRho;
    ra = atan2(y,x);
    de = atan2(z,sqrt(x*x+y*y));
    if(ra<0)ra+=2*M_PI;
}

double ptp_anvle(double ra1, double de1, double ra2, double de2)
{
    double x1 = cos(de1)*cos(ra1);
    double y1 = cos(de1)*sin(ra1);
    double z1 = sin(de1);
    double x2 = cos(de2)*cos(ra2);
    double y2 = cos(de2)*sin(ra2);
    double z2 = sin(de2);
    return acos(x1*x2+y1*y2+z1*z2);
}

/*void setWCS(double *WCSdata, double *Ax, double *Ay)
{
        WCSdata[12]=1;
        WCSdata[0]=(Ay[1]*Ax[2]/Ax[1]-Ay[2])/(Ay[0]-Ay[1]*Ax[0]/Ax[1]);
        WCSdata[1]=-(Ax[2]+Ax[0]*WCSdata[0])/Ax[1];
        //WCSdata[2]=ra;
        //WCSdata[3]=dec;
        WCSdata[4]=sqrt(Ax[0]*Ax[0]+Ay[0]*Ay[0]);
        WCSdata[5]=sqrt(Ax[1]*Ax[1]+Ay[1]*Ay[1]);
        double rho1 =180*atan2(-Ay[0]/WCSdata[4],Ax[0]/WCSdata[4])/M_PI;
        double rho2 = 180*atan2(Ax[1]/WCSdata[5],Ay[1]/WCSdata[5])/M_PI;
        if (rho1<0) rho1 = 360+rho1;
        if (rho2<0) rho2 = 360+rho2;
        WCSdata[6]=rho1;
        WCSdata[7]=rho2;
        WCSdata[8]=Ax[0];
        WCSdata[9]=Ax[1];
        WCSdata[10]=Ay[0];
        WCSdata[11]=Ay[1];
}*/

void triangSRA(double X0, double Y0, double X1, double Y1,double X2, double Y2, double& angle, double& ratio)
{
    double r1 = sqrt((X1-X0)*(X1-X0)+(Y1-Y0)*(Y1-Y0));
    double r2 = sqrt((X2-X0)*(X2-X0)+(Y2-Y0)*(Y2-Y0));
    double V1x = (X1-X0)/r1;
    double V1y = (Y1-Y0)/r1;
    double V2x = (X2-X0)/r2;
    double V2y = (Y2-Y0)/r2;
    angle = acos(V1x*V2x+V1y*V2y)*180/M_PI;
    if(r1<r2) ratio = r2/r1; else ratio = -r1/r2;
}

double sortVector(QVector<double> d, QVector<int>& n)
{
    n.clear();
    //QVector<double> *sortD = new QVector<double>;
    //sortD->append(d[0]);
    n<<0;
    for(int i=1;i<d.count();i++)
    {
        if(d[i]<=d[n[0]])
            {n.prepend(i);continue;}
        if(d[i]>d[n[n.count()-1]])
            {n.append(i);continue;}
        for(int j=0;j<n.count()-1;j++)
            if((d[i]>=d[n[j]])&&(d[i]<d[n[j+1]]))
                //{n.insert(j,i);break;}//
                {n.insert(j+1,i);break;}
    }
}

void convertEqtoGal(double ra, double dec, double& l, double& b)//ra,dec in redians
{
  long double DEC = dec;//*M_PI/180;
  long double RA = ra;//*M_PI/180;
  long double eqX = cos(DEC)*cos(RA);
  long double eqY = cos(DEC)*sin(RA);
  long double eqZ = sin(DEC);
  //////////////////////////////////////
  long double Ag[3][3] = {-0.0548755601367195, -0.8734370902532698, -0.4838350155472244,
                      0.4941094280132430, -0.4448296298016944, 0.7469822445004389,
                     -0.8676661489582886, -0.1980763737056720, 0.4559837761713720};
  long double gX = Ag[0][0]*eqX+Ag[0][1]*eqY+Ag[0][2]*eqZ;
  long double gY = Ag[1][0]*eqX+Ag[1][1]*eqY+Ag[1][2]*eqZ;
  long double gZ = Ag[2][0]*eqX+Ag[2][1]*eqY+Ag[2][2]*eqZ;
  ///////////////////////////////////////////
  b = atan(gZ/(sqrt(gX*gX+gY*gY)));// l,b in radians
  l = atan2(gY,gX);
  if(l<0) l+=2*M_PI;
}

void convertPM_EqtoGal(double DEC, double MUra, double MUdec, double l, double b, double& MUl, double& MUb)
{
    long double L = 32.93192*M_PI/180;
    long double i = 62.87175*M_PI/180;
    long double lmL = l-L;
    //////////////////////////////////
    double sin_phi = sin(i)*cos(lmL)/cos(DEC);
    double cos_phi = (cos(b)*cos(i)-sin(b)*sin(i)*sin(lmL))/cos(DEC);
    MUl = MUra*cos_phi+MUdec*sin_phi;
    MUb = -MUra*sin_phi+MUdec*cos_phi;
}



/////////////////////////////////////////////ucac3find
QString ucac3find(double ra, double dec, double rhox, double rhoy, int tw,  QString ucac3path)
{//[ra]=rad
 //[dec]=rad
 //[rhox],[rhoy]=arcmin
    QString outstr="";
    double A,D;
    A=ra;
    D=dec;
    ra=ra*648000000/M_PI;
    dec=dec*648000000/M_PI;
    if(tw==0) {rhox = rhox*60000;rhoy=rhox;}//within radius
    if(tw==1) {rhox = rhox*30000;rhoy = rhoy*30000;}//within box
    if(tw==2) {rhox = rhox*30000;rhoy = rhoy*30000;}//within geodetic box
    ///////////////////
    double decmin = dec-rhoy;
    if(decmin<-324000000) decmin = -324000000;
    double decmax = dec+rhoy;
    if(decmax>324000000) decmax = 324000000;
    //////////////////
    double a,d,dra;
    if(tw==2)
    {
        dra = rhox;
    }
    else
    {
        if((decmax<324000000)&&(decmin>-324000000))
            dra = 648000000*asin(sin(rhox*M_PI/648000000)/cos(dec*M_PI/648000000))/M_PI;
        else
            dra = 0;
    }
    double ramin = ra - dra;
    if(ramin<0) ramin = ramin+ 1296000000;
    double ramax = ra + dra;
    if(ramax>=1296000000) ramax = ramax - 1296000000;
    if(dra==0){ramin=0;ramax=1295999999;}
    //loading u3index.unf
    quint32 n0[240][360];
    quint32 nn[240][360];
    QFile unffile(ucac3path + "/u3index.unf");
    if (unffile.open(QIODevice::ReadOnly))
    {
      char *data = (char*)n0;
      unffile.read(data,360*240*4);
      data = (char*)nn;
      unffile.read(data,360*240*4);
      unffile.close();
    }
    else
    {
      return "can't find u3index.unf \n";
    }
    // end for u3index.unf
    //load data
    QString u3fname;
    QFile u3file;
    quint64 ns;
    //////////ucac3 data
    qint32 Ra,spd, pmrac,pmdc, id2m, rn;
    qint16 im1,im2,sigmag, sigra,sigdc, cepra,cepdc,sigpmr,sigpmd
              ,jmag,hmag,kmag, smB,smR2,smI;
    qint8 objt,dsf, na1,nu1,us1,cn1, clbl,qfB,qfR2,qfI;
    qint8 icqflg[3], e2mpho[3], catflg[10], g1,c1,leda,x2m;
    int fcount = (ramax<ramin);
    int RAMIN,RAMAX,DECMIN,DECMAX;
    int outs;
    for(int q=0;q<=fcount;q++)
    {
        if(fcount)
        {
            if(q){RAMIN=0;RAMAX = (int)(ramax/5400000);}else{RAMIN = (int)(ramin/5400000);RAMAX=239;}
        }
        else
        {
            RAMIN = (int)(ramin/5400000);
            RAMAX = (int)(ramax/5400000);
        }
        //DECMIN= 180+(int)(decmin/1800000);
        //DECMAX =180+(int)(decmax/1800000);
        DECMIN= 180+(int)floor(decmin/1800000.0);
        DECMAX =180+(int)floor(decmax/1800000.0);
        if(DECMAX>359) DECMAX--;
        for(int z=DECMIN;z<=DECMAX;z++)
        {
            u3fname = ucac3path + "/z"+QString( "%1" ).arg(z+1,3,10,QLatin1Char( '0' ));
            u3file.setFileName(u3fname);
            //stream << u3fname <<'\n';
            if (!u3file.open(QIODevice::ReadOnly)){return u3fname;}
            u3file.seek(n0[RAMIN][z]*84);
            ns = n0[RAMAX][z]+nn[RAMAX][z] - n0[RAMIN][z];
            QByteArray u3ba = u3file.read(ns*84);
            u3file.close();
            QDataStream u3stream(u3ba);
            u3stream.setByteOrder(QDataStream::LittleEndian);
            for(int st=0;st<ns;st++)
            {
                u3stream >> Ra >> spd >>
                    im1>>im2>>sigmag>> objt>>dsf>>
                    sigra>>sigdc>> na1>>nu1>>us1>>cn1>>
                    cepra>>cepdc>> pmrac>>pmdc>> sigpmr>>sigpmd>>
                    id2m>> jmag>>hmag>>kmag>> icqflg[0]>> icqflg[1]>> icqflg[2]>> e2mpho[0]>>e2mpho[1]>>e2mpho[2]>>
                    smB>>smR2>>smI>> clbl>>qfB>>qfR2>>qfI>>
                    catflg[0] >> catflg[1] >> catflg[2]>> catflg[3]>> catflg[4]>> catflg[5]>>
                    catflg[6]>> catflg[7]>> catflg[8]>> catflg[9]>> g1>>c1>> leda>> x2m>> rn;
                spd = spd-324000000;
                if(tw==2)
                {
                    if(fcount)
                    {
                        if(q)
                        {
                            if((Ra>=0)&&(Ra<=ramax)&&(spd>=decmin)&&(spd<=decmax)) outs=1;else outs=0;
                        }
                        else
                        {
                            if((Ra>=ramin)&&(Ra<= 1296000000)&&(spd>=decmin)&&(spd<=decmax)) outs=1;else outs=0;
                        }
                    }
                    else if((Ra>=ramin)&&(Ra<=ramax)&&(spd>=decmin)&&(spd<=decmax)) outs=1;else outs=0;
                }
                else
                {
                    //getRaDeToTang((double)Ra,(double)spd,ra,dec,tp);
                    double ksi,eta;
                    a = ((double)Ra)*M_PI/648000000;
                    d = ((double)spd)*M_PI/648000000;
                    tangFromRADE(a,d,A,D,ksi,eta);
                    ksi = ksi*648000000/M_PI;
                    eta = eta*648000000/M_PI;
                    outs = 0;
                    if((tw==0)&&(sqrt(ksi*ksi+eta*eta)<=rhox)) outs=1;
                    if((tw==1)&&(fabs(ksi)<=rhox)&&(fabs(eta)<=rhoy)) outs=1;
                    //if(sqrt(tp[0]*tp[0]+tp[1]*tp[1])/60<=rhox/60000) stream << QString("%1").arg(sqrt(tp[0]*tp[0]+tp[1]*tp[1])/60,10,'f',2,QLatin1Char( ' ' )) <<'|'<<
                    //       QString("%1").arg(rhox/60000,10,'f',2,QLatin1Char( ' ' )) <<'|'<< QString("%1").arg(tw,3,10,QLatin1Char( ' ' ))<<'\n';
                }
                if(outs)
                {
                    a = ((double)Ra)*M_PI/648000000;
                    d = ((double)spd)*M_PI/648000000;
                    outstr+=  RAtoHMS(a,":",4)+'|'                                                 //0
                            +DEtoDAMAS(d,":",3)+'|'                                              //1
                            +QString("%1").arg((double)im1/1000,6,'f',3,QLatin1Char( ' ' )) +'|'       //2
                            +QString("%1").arg((double)im2/1000,6,'f',3,QLatin1Char( ' ' )) +'|'       //3
                            +QString("%1").arg((double)sigmag/1000,6,'f',3,QLatin1Char( ' ' )) +'|'    //4
                            +QString("%1").arg(objt,3,10,QLatin1Char( ' ' )) +'|'                      //5
                            +QString("%1").arg(dsf,3,10,QLatin1Char( ' ' )) +'|'                       //6
                            +QString("%1").arg(sigra,3,10,QLatin1Char( ' ' )) +'|'                     //7
                            +QString("%1").arg(sigdc,3,10,QLatin1Char( ' ' )) +'|'                     //8
                            +QString("%1").arg(na1,3,10,QLatin1Char( ' ' )) +'|'                       //9
                            +QString("%1").arg(nu1,3,10,QLatin1Char( ' ' )) +'|'                      //10
                            +QString("%1").arg(us1,3,10,QLatin1Char( ' ' )) +'|'                      //11
                            +QString("%1").arg(cn1,3,10,QLatin1Char( ' ' )) +'|'                      //12
                            +QString("%1").arg(1900+(double)cepra/100,7,'f',2,QLatin1Char( ' ' )) +'|'//13
                            +QString("%1").arg(1900+(double)cepdc/100,7,'f',2,QLatin1Char( ' ' )) +'|'//14
                            +QString("%1").arg((double)pmrac/10,7,'f',1,QLatin1Char( ' ' )) +'|'      //15
                            +QString("%1").arg((double)pmdc/10,7,'f',1,QLatin1Char( ' ' )) +'|'       //16
                            +QString("%1").arg((double)sigpmr/10,5,'f',1,QLatin1Char( ' ' )) +'|'     //17
                            +QString("%1").arg((double)sigpmd/10,5,'f',1,QLatin1Char( ' ' )) +'|'     //18
                            +QString("%1").arg(id2m,10,10,QLatin1Char( ' ' )) +'|'                    //19
                            +QString("%1").arg((double)jmag/1000,6,'f',3,QLatin1Char( ' ' )) +'|'     //20
                            +QString("%1").arg((double)hmag/1000,6,'f',3,QLatin1Char( ' ' )) +'|'     //21
                            +QString("%1").arg((double)kmag/1000,6,'f',3,QLatin1Char( ' ' )) +'|';    //22
                    for(int k=0;k<3;k++) outstr+= QString("%1").arg(icqflg[k],2,10,QLatin1Char( '0' )); //23
                    outstr+= '|'
                           + QString("%1").arg((double)e2mpho[0]/100,5,'f',2,QLatin1Char( ' ' )) +'|'  //24
                           + QString("%1").arg((double)e2mpho[1]/100,5,'f',2,QLatin1Char( ' ' )) +'|'  //25
                           + QString("%1").arg((double)e2mpho[2]/100,5,'f',2,QLatin1Char( ' ' )) +'|'  //26
                           + QString("%1").arg((double)smB/1000,6,'f',3,QLatin1Char( ' ' )) +'|'       //27
                           + QString("%1").arg((double)smR2/1000,6,'f',3,QLatin1Char( ' ' )) +'|'      //28
                           + QString("%1").arg((double)smI/1000,6,'f',3,QLatin1Char( ' ' )) +'|'       //29
                           + QString("%1").arg(clbl,3,10,QLatin1Char( ' ' )) +'|'                      //30
                           + QString("%1").arg(qfB,3,10,QLatin1Char( ' ' )) +'|'                       //31
                           + QString("%1").arg(qfR2,3,10,QLatin1Char( ' ' )) +'|'                      //32
                           + QString("%1").arg(qfI,3,10,QLatin1Char( ' ' )) +'|';                      //33
                    for(int k=0;k<10;k++) outstr+= QString("%1").arg(catflg[k],1,10,QLatin1Char( ' ' ));//34
                    outstr+='|'
                           + QString("%1").arg(c1,3,10,QLatin1Char( ' ' )) +'|'                        //35
                           + QString("%1").arg(leda,3,10,QLatin1Char( ' ' )) +'|'                      //36
                           + QString("%1").arg(x2m,3,10,QLatin1Char( ' ' )) +'|'                       //37
                           + QString("%1").arg(rn,9,10,QLatin1Char( ' ' )) +'|'                        //38
                            +'\n';
                }
            }
            //stream << '\n';
        }
    }
    return outstr;
}

void tangFromUCAC3(QString u3str, QVector<double>& ksi, QVector<double>& eta,
                   QVector<double>& mag, double RA, double DE, double year, double mag1, double mag2)
{
    QTextStream stream(&u3str);
    QString str;
    double ra,de,pmRA,pmDE,m,KSI,ETA;
    while(!stream.atEnd())
    {
        str = stream.readLine();
        ra = HMStoRA(str.section('|',0,0),":");
        de = DAMAStoDE(str.section('|',1,1),":");
        pmRA = str.section('|',15,15).toDouble()*M_PI/648000000/cos(de);
        pmDE = str.section('|',16,16).toDouble()*M_PI/648000000;
        ra = ra+pmRA*(year-2000);
        de = de+pmDE*(year-2000);
        m = str.section('|',2,2).toDouble();
        tangFromRADE(ra,de,RA,DE,KSI,ETA);
        if((m>=mag1)&&(m<=mag2))
        {
            ksi << 180*KSI/M_PI; eta << 180*ETA/M_PI;
            mag << m;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////ucac4find
QString ucac4find(double ra, double dec, double rhox, double rhoy, int tw,  QString ucac4path)
{//[ra]=rad
 //[dec]=rad
 //[rhox],[rhoy]=arcmin

    double A,D;
    A=ra;
    D=dec;
    ra=ra*648000000/M_PI;
    dec=dec*648000000/M_PI;
    if(tw==0) {rhox = rhox*60000;rhoy=rhox;}//within radius
    if(tw==1) {rhox = rhox*30000;rhoy = rhoy*30000;}//within box
    if(tw==2) {rhox = rhox*30000;rhoy = rhoy*30000;}//within geodetic box
    ///////////////////
    double decmin = dec-rhoy;
    if(decmin<-324000000) decmin = -324000000;
    double decmax = dec+rhoy;
    if(decmax>324000000) decmax = 324000000;
    //////////////////
    double a,d,dra;
    if(tw==2)
    {
        dra = rhox;
    }
    else
    {
        if((decmax<324000000)&&(decmin>-324000000))
            dra = 648000000*asin(sin(rhox*M_PI/648000000)/cos(D))/M_PI;
        else
            dra = 0;
    }
    double ramin = ra - dra;
    if(ramin<0) ramin = ramin+ 1296000000;
    double ramax = ra + dra;
    if(ramax>=1296000000) ramax = ramax - 1296000000;
    if(dra==0){ramin=0;ramax=1295999999;}
    //loading u4index.unf

        QFile unffile(ucac4path + "/u4index.unf");
        unffile.open(QIODevice::ReadOnly);
        quint32 *data = new quint32[1440*900*8];
        unffile.read((char*)data,1440*900*8);
        unffile.close();

        ////////////////////////////////////////////////
        int fcount = (ramax<ramin);//number of finding loops (fcount=1 if, for example ramax = 23h and ramin = 1h)
        int RAMIN,RAMAX,DECMIN,DECMAX;//indexes of data blocks
        int outs;
        QString u4fname;
        QFile u4file;
        ////////////////////////////////
        //ucac4 fields
        quint32 Ra, spd, pts_key,rnm, mcf, rnz;
        quint16 magm,maga, cepra,cepdc, j_m, h_m, k_m,   zn2;
        quint16 apasm[5];
        qint16 pmra2,pmdc2;
        qint8  sigra,sigdc, sigpmr,sigpmd;
        quint8 sigmag, objt,cdf, na1,nu1,cu1,  gcflg;
        qint8 apase[5];
        quint8 icqflg[3], q2mflg[3], leda,x2m;

        ////////////////
        double Sigpmr,Sigpmd;
        //double a,d;//current equatorial in radians
        double ksi,eta;//tangential coordinates
        QString u4str;
        quint64 ns;
        double dmas;
        ///////////////////////////////
        for(int q=0;q<=fcount;q++)
        {
            //determination of indexes
            if(fcount)
            {
                if(q){RAMIN=0;RAMAX = (int)(ramax/900000.0);}else{RAMIN = (int)(ramin/900000.0);RAMAX=1439;}
            }
            else
            {
                RAMIN = (int)(ramin/900000.0);
                RAMAX = (int)(ramax/900000.0);
            }
            DECMIN= 450+(int)floor(decmin/720000.0);
            DECMAX =450+(int)floor(decmax/720000.0);
            if(DECMAX>899) DECMAX--;
            //main loop

            for(int z=DECMIN;z<=DECMAX;z++)
            {
                u4fname = ucac4path + "/z"+QString( "%1" ).arg(z+1,3,10,QLatin1Char( '0' ));
                u4file.setFileName(u4fname);
                //////////////////////////
                if (!u4file.open(QIODevice::ReadOnly)){return  u4fname; }
                u4file.seek(data[RAMIN*900+z]*78);
                ns = data[RAMAX*900+z]+data[1440*900+RAMAX*900+z] - data[RAMIN*900+z];
                QByteArray u4ba = u4file.read(ns*78);
                u4file.close();
                QDataStream u4stream(u4ba);
                u4stream.setByteOrder(QDataStream::LittleEndian);
                for(int st=0;st<ns;st++)
                {
                    u4stream >> Ra >> spd >> magm >>maga >> sigmag >> objt >> cdf
                                >> sigra >> sigdc >> na1 >> nu1 >> cu1
                                >> cepra >> cepdc >> pmra2 >> pmdc2 >> sigpmr >> sigpmd
                                >> pts_key >> j_m >> h_m >> k_m
                                >> icqflg[0] >> icqflg[1] >> icqflg[2]
                                >> q2mflg[0] >> q2mflg[1] >> q2mflg[2]
                                >> apasm[0] >> apasm[1] >> apasm[2] >> apasm[3] >> apasm[4]
                                >> apase[0] >> apase[1] >> apase[2] >> apase[3] >> apase[4]
                                >>  gcflg
                                >> mcf >> leda >> x2m >> rnm
                                >> zn2 >> rnz;
                    //spd = spd-324000000;
                    a = Ra*M_PI/648000000.0;
                    d = spd*M_PI/648000000.0;
                    d-=M_PI/2;
                    dmas=648000000.0*d/M_PI;
                    if(tw==2)
                    {
                        if(fcount)
                        {
                            if(q)
                            {
                                if((Ra>=0)&&(Ra<=ramax)&&(dmas>=decmin)&&(dmas<=decmax)) outs=1;else outs=0;
                            }
                            else
                            {
                                if((Ra>=ramin)&&(Ra<= 1296000000)&&(dmas>=decmin)&&(dmas<=decmax)) outs=1;else outs=0;
                            }
                        }
                        else if((Ra>=ramin)&&(Ra<=ramax)&&(dmas>=decmin)&&(dmas<=decmax)) outs=1;else outs=0;
                    }
                    else
                    {
                        tangFromRADE(a,d,A,D,ksi,eta);
                        ksi = ksi*648000000/M_PI;
                        eta = eta*648000000/M_PI;
                        outs = 0;
                        if((tw==0)&&(sqrt(ksi*ksi+eta*eta)<=rhox)) outs=1;
                        if((tw==1)&&(fabs(ksi)<=rhox)&&(fabs(eta)<=rhoy)) outs=1;
                    }
                    //outs=1;
                    if(outs)
                    {
                        //////////////////////////
                        Sigpmr = (128.0+sigpmr)/10;
                        if(Sigpmr==25.1)Sigpmr=27.5;
                        if(Sigpmr==25.2)Sigpmr=32.5;
                        if(Sigpmr==25.3)Sigpmr=37.5;
                        if(Sigpmr==25.4)Sigpmr=45.0;
                        Sigpmd = (128.0+sigpmd)/10;
                        if(Sigpmd==25.1)Sigpmd=27.5;
                        if(Sigpmd==25.2)Sigpmd=32.5;
                        if(Sigpmd==25.3)Sigpmd=37.5;
                        if(Sigpmd==25.4)Sigpmd=45.0;
                        ////////////////////////////
                        u4str+= RAtoHMS(a,":",4)                                                    +"|"+  //0 ra
                                DEtoDAMAS(d,":",3)                                                  +"|"+  //1 dec
                                QString("%1").arg((double)magm/1000.0,6,'f',3,QLatin1Char( ' ' ))   +"|"+  //2
                                QString("%1").arg((double)maga/1000.0,6,'f',3,QLatin1Char( ' ' ))   +"|"+  //3
                                QString("%1").arg((double)sigmag/100.0,6,'f',2,QLatin1Char( ' ' ))   +"|"+  //4
                                QString("%1").arg(objt,3,10,QLatin1Char( ' ' ))                      +"|"+  //5
                                QString("%1").arg(cdf,3,10,QLatin1Char( ' ' ))                      +"|"+  //6
                                QString("%1").arg(128.0+sigra,4,'f',0,QLatin1Char( ' ' ))                      +"|"+  //7
                                QString("%1").arg(128.0+sigdc,4,'f',0,QLatin1Char( ' ' ))                      +"|"+  //8
                                QString("%1").arg(na1,3,10,QLatin1Char( ' ' ))                      +"|"+  //9
                                QString("%1").arg(nu1,3,10,QLatin1Char( ' ' ))                      +"|"+  //10
                                QString("%1").arg(cu1,3,10,QLatin1Char( ' ' ))                      +"|"+  //11
                                QString("%1").arg(1900+(double)cepra/100.0,7,'f',2,QLatin1Char( ' ' ))   +"|"+  //12
                                QString("%1").arg(1900+(double)cepdc/100.0,7,'f',2,QLatin1Char( ' ' ))   +"|"+  //13
                                QString("%1").arg((double)pmra2/10.0,8,'f',1,QLatin1Char( ' ' ))   +"|"+  //14
                                QString("%1").arg((double)pmdc2/10.0,8,'f',1,QLatin1Char( ' ' ))   +"|"+  //15
                                QString("%1").arg(Sigpmr,6,'f',1,QLatin1Char( ' ' ))   +"|"+  //16
                                QString("%1").arg(Sigpmd,6,'f',1,QLatin1Char( ' ' ))   +"|"+  //17
                                QString("%1").arg(pts_key,10,10,QLatin1Char( ' ' ))                  +"|"+  //18
                                QString("%1").arg((double)j_m/1000.0,6,'f',3,QLatin1Char( ' ' ))   +"|"+  //19
                                QString("%1").arg((double)h_m/1000.0,6,'f',3,QLatin1Char( ' ' ))   +"|"+  //20
                                QString("%1").arg((double)k_m/1000.0,6,'f',3,QLatin1Char( ' ' ))   +"|"+  //21
                                QString("%1").arg(icqflg[0],3,10,QLatin1Char( ' ' ))                      +"|"+  //22
                                QString("%1").arg(icqflg[1],3,10,QLatin1Char( ' ' ))                      +"|"+  //23
                                QString("%1").arg(icqflg[2],3,10,QLatin1Char( ' ' ))                      +"|"+  //24
                                QString("%1").arg((double)q2mflg[0]/100.0,6,'f',2,QLatin1Char( ' ' ))   +"|"+  //25
                                QString("%1").arg((double)q2mflg[1]/100.0,6,'f',2,QLatin1Char( ' ' ))   +"|"+  //26
                                QString("%1").arg((double)q2mflg[2]/100.0,6,'f',2,QLatin1Char( ' ' ))   +"|"+  //27
                                ////////////////////////////////////////////////////////////////////////////////////
                                QString("%1").arg((double)apasm[0]/1000.0,6,'f',3,QLatin1Char( ' ' ))   +"|"+  //28
                                QString("%1").arg((double)apasm[1]/1000.0,6,'f',3,QLatin1Char( ' ' ))   +"|"+  //29
                                QString("%1").arg((double)apasm[2]/1000.0,6,'f',3,QLatin1Char( ' ' ))   +"|"+  //30
                                QString("%1").arg((double)apasm[3]/1000.0,6,'f',3,QLatin1Char( ' ' ))   +"|"+  //31
                                QString("%1").arg((double)apasm[4]/1000.0,6,'f',3,QLatin1Char( ' ' ))   +"|"+  //32
                                ////////////////////////////////////////////////////////////////////////////////////
                                QString("%1").arg((double)apase[0]/100.0,6,'f',2,QLatin1Char( ' ' ))   +"|"+  //33
                                QString("%1").arg((double)apase[1]/100.0,6,'f',2,QLatin1Char( ' ' ))   +"|"+  //34
                                QString("%1").arg((double)apase[2]/100.0,6,'f',2,QLatin1Char( ' ' ))   +"|"+  //35
                                QString("%1").arg((double)apase[3]/100.0,6,'f',2,QLatin1Char( ' ' ))   +"|"+  //36
                                QString("%1").arg((double)apase[4]/100.0,6,'f',2,QLatin1Char( ' ' ))   +"|"+  //37
                                /////////////////////////////////////////////////////////////////////////////////////
                                QString("%1").arg(gcflg,3,10,QLatin1Char( ' ' ))                      +"|"+  //38
                                QString("%1").arg(mcf,10,10,QLatin1Char( ' ' ))                      +"|"+  //39
                                QString("%1").arg(leda,3,10,QLatin1Char( ' ' ))                      +"|"+  //40
                                QString("%1").arg(x2m,3,10,QLatin1Char( ' ' ))                      +"|"+  //41
                                /////////////////////////////////////////////////////////////////////////////////////
                                QString("%1").arg(rnm,10,10,QLatin1Char( ' ' ))                      +"|"+  //42
                                QString("%1").arg(zn2,10,10,QLatin1Char( ' ' ))                      +"|"+  //43
                                QString("%1").arg(rnz,10,10,QLatin1Char( ' ' ))                      +"\n" //44
                                ;
                        //stream << u4str << endl;

                    }
                }
                //stream << '\n';
            }//end of z
        }//end of q
        delete[]data;
    return u4str;
}

void tangFromUCAC4(QString u4str,
                   QVector<double>& ksi, QVector<double>& eta,
                   QList<double*>& mag, double RA, double DE,
                   double year, double mag1, double mag2, int nmag)
{
    QTextStream stream(&u4str);
    QString str;
    double ra,de,pmRA,pmDE,m,KSI,ETA;
    double *Mag;
    while(!stream.atEnd())
    {
        str = stream.readLine();
        ra = HMStoRA(str.section('|',0,0),":");
        de = DAMAStoDE(str.section('|',1,1),":");
        pmRA = str.section('|',14,14).toDouble()*M_PI/648000000/cos(de);
        pmDE = str.section('|',15,15).toDouble()*M_PI/648000000;
        ra = ra+pmRA*(year-2000);
        de = de+pmDE*(year-2000);
        //////////////////////////////
        Mag = new double[10];
        Mag[0] = str.section('|',2,2).toDouble();//magm
        Mag[1] = str.section('|',3,3).toDouble();//maga
        ////////////////////////////////////////////
        Mag[2] = str.section('|',19,19).toDouble();//J
        Mag[3] = str.section('|',20,20).toDouble();//H
        Mag[4] = str.section('|',20,20).toDouble();//Ks
        /////////////////////////////////////////////////
        Mag[5] = str.section('|',28,28).toDouble();//B
        Mag[6] = str.section('|',29,29).toDouble();//V
        Mag[7] = str.section('|',30,30).toDouble();//g
        Mag[8] = str.section('|',29,29).toDouble();//r
        Mag[9] = str.section('|',30,30).toDouble();//i
        //////////////////////////////
        if(nmag==0)m = Mag[0];
        if(nmag==1)m = Mag[1];
        if(nmag==2)m = Mag[2];
        if(nmag==3)m = Mag[3];
        if(nmag==4)m = Mag[4];
        if(nmag==5)m = Mag[5];
        if(nmag==6)m = Mag[6];
        if(nmag==7)m = Mag[7];
        if(nmag==8)m = Mag[8];
        if(nmag==9)m = Mag[9];
        //
        ////////////////////////////////////////////////
        tangFromRADE(ra,de,RA,DE,KSI,ETA);
        if((m>=mag1)&&(m<=mag2))
        {
            ksi << 180*KSI/M_PI; eta << 180*ETA/M_PI;
            mag << Mag;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////////

void cms(const double* fd, int rho, double& cx, double& cy, double& cJ, int& N)
{
  double sumX=0, sumY=0, sumJ=0, J;
  N=0;
  for(int y=0;y<2*rho;y++)
    for(int x=0;x<2*rho;x++)
      {
        J = fd[y*2*rho+x];
        if(J!=-10E-9)
        {
          sumX+=x*J;
          sumY+=y*J;
          sumJ+=J;
          N++;
        }
      }
  if(sumJ!=0){cx = sumX/sumJ;cy = sumY/sumJ;}else{cx = rho;cy = rho;}
  if(isnan(cx))cx = rho;
  if(isnan(cy))cy = rho;
  cJ = sumJ;
}

void ringSumm(const double* fd, int rho2, double& sJ, int& N)
{
  N = 0;
  double J;
  sJ=0;
  for(int y=0;y<2*rho2;y++)
    for(int x=0;x<2*rho2;x++)
      {J=fd[y*2*rho2+x];if(J!=-10E-9){sJ+=J;N++;}}
}

double mfv(QVector<double> S,double step)
{
    double minS=S[0];
    double maxS=S[0];
    for(int i;i<S.count();i++)
    {
        if(S[i]<minS)minS=S[i];
        if(S[i]>maxS)maxS=S[i];
    }
    //qDebug()<< QString("%1").arg(minS,10,'f',6,QLatin1Char( ' ' ));
    //qDebug()<< QString("%1").arg(maxS,10,'f',6,QLatin1Char( ' ' ));
    int N=(int)floor((maxS-minS)/step);
   // qDebug()<< QString("%1").arg(N,10,'f',6,QLatin1Char( ' ' ));
    int *H = new int[N];
    int k;
    for(int i=0;i<N;i++)H[i]=0;
    for(int i=0;i<S.count();i++)
    {
        k=(int)floor((S[i]-minS)/step);
        if((k>=0)&&(k<N))H[k]++;
    }
    k=0;
    //for(int i=0;i<N;i++)qDebug() << QString("%1").arg(H[i],10,'f',0,QLatin1Char( ' ' ));
    for(int i=0;i<N;i++)if(H[i]>H[k])k=i;
    delete[]H;
    return minS+(k+0.5)*step;
}

void prvalues(QVector<double> S, double step, QVector<double>& H)
{
    double minS=S[0];
    double maxS=S[0];
    for(int i;i<S.count();i++)
    {
        if(S[i]<minS)minS=S[i];
        if(S[i]>maxS)maxS=S[i];
    }
    ////////////////////////////
    int N=(int)floor((maxS-minS)/step);
    QVector<double> Hs,V;
    int k;
    for(int i=0;i<N;i++){Hs<<0;V<<minS+(i+0.5)*step;}
    for(int i=0;i<S.count();i++)
    {
        k=(int)floor((S[i]-minS)/step);
        if((k>=0)&&(k<N))Hs[k]+=1;
    }
    QVector<int> n;
    sortVector(Hs,n);
    H.clear();
    for(int i=0;i<n.count();i++) H << V[n[i]];

}


int comparePointsSets(QVector<double>x,QVector<double>y,
                      QVector<double>X,QVector<double>Y,
                      double *Ax, double *Ay,
                      double pr, int N)
{
    double xx,yy,r;
    int k=0;
    double *ksi = new double[N];
    double *eta = new double[N];
    double *C = new double[N*3];
    for(int i=0;i<X.count();i++)
    {
        xx = (Ax[0]*X[i]+Ax[1]*Y[i]+Ax[2]);
        yy = (Ay[0]*X[i]+Ay[1]*Y[i]+Ay[2]);
        for(int j=0;j<x.count();j++)
        {
            r=sqrt((x[j]-xx)*(x[j]-xx)+(y[j]-yy)*(y[j]-yy));
            //qDebug() << QString("%1").arg(k,10,'f',5,QLatin1Char( ' ' ));
            if(r<pr)
            {
                ksi[k]=x[j];eta[k]=y[j];
                C[k*3+0]=X[i];
                C[k*3+1]=Y[i];
                C[k*3+2]=1;
                //qDebug() << QString("%1").arg(k,10,'f',5,QLatin1Char( ' ' ));
                k++;
            }
            if(k>=N)
            {
                slsm(3,N,Ax,C,ksi);
                slsm(3,N,Ay,C,eta);
                delete[] ksi;delete[] eta;delete[] C;
                return 1;
            }
        }
    }//end for i
    delete[] ksi;delete[] eta;delete[] C;
    return 0;
}



//BEGIN wcsData class implementation

wcsData::wcsData(QString filename)
{
    int status=0;
    wcs_Type=0;
    isWcsExist=true;
    fileName = filename;
    QString str;
    fitsfile *fptr;
    fits_open_file(&fptr, qPrintable(filename), READONLY,&status);//open file
    if(status!=0){isWcsExist=false;return;}
    long naxes[2];//size of the image
    fits_get_img_size(fptr, 2, naxes, &status);
    if(status!=0){isWcsExist=false;return;}
    w = naxes[0];
    h = naxes[1];
    //////////////////////////////////
    char strkey[FLEN_CARD];
    fits_read_key(fptr, TSTRING, "ORIGIN", strkey, NULL, &status);
        if(status!=0){isWcsExist=false;wcs_Type=0;status=0;fits_close_file(fptr, &status);return;}
       // qDebug()<<"ok1";
    if(QString(strkey).contains("Pulkovo"))wcs_Type=1;
    if(QString(strkey).contains("SDSS"))wcs_Type=2;
    if(QString(strkey).contains("STScI/MAST"))wcs_Type=3;
    if(QString(strkey).contains("Harvard-Smithsonian"))wcs_Type=4;
    if(QString(strkey).contains("densmap"))wcs_Type=5;
    if(QString(strkey).contains("SAO"))wcs_Type=6;
    //qDebug("wcs type: %d", wcs_Type);
    //fits_read_key(fptr, TSTRING, "CTYPE1", strkey, NULL, &status);
    //    if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
    //if(!QString(strkey).contains("RA---TAN"))
    //    {isWcsExist=false;wcs_Type=0;status=0;fits_close_file(fptr, &status);return;}
    if(wcs_Type==1)//Normal Astrograph
    {

        //yr,mm,dd,hr,min,sec,mjd,exptime;
        fits_read_key(fptr, TSTRING, "DATE-OBS", strkey, NULL, &status);
            if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        str =QString(strkey);
        yr = str.section('T',0,0).section('-',0,0).toDouble();
        mm = str.section('T',0,0).section('-',1,1).toDouble();
        dd = str.section('T',0,0).section('-',2,2).toDouble();
        hr = str.section('T',1,1).section(':',0,0).toDouble();
        min = str.section('T',1,1).section(':',1,1).toDouble();
        sec = str.section('T',1,1).section(':',2,2).toDouble();
        fits_read_key(fptr, TDOUBLE, "EXPTIME", &exptime, NULL, &status);
            if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        mjd = getMJD(yr,mm,dd,hr,min,sec)+ exptime/(2*86400);
        Year = 1858.87885010267 + mjd/365.25;
        //
        fits_read_key(fptr, TSTRING, "RA", strkey, NULL, &status);
            if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        str =QString(strkey);
        crval1 = HMStoRA(str,":")*180/M_PI;
        //
        fits_read_key(fptr, TSTRING, "DEC", strkey, NULL, &status);
            if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        str =QString(strkey);
        crval2 = DAMAStoDE(str,":")*180/M_PI;
        //
        fits_read_key(fptr, TSTRING, "TARGET", strkey, NULL, &status);
            if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        Target =QString(strkey);
        //
        fits_read_key(fptr, TDOUBLE, "CRPIX1", &crpix1, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CRPIX2", &crpix2, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CRVAL1", &crval1, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CRVAL2", &crval2, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CDELT1", &cdelt1, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CDELT2", &cdelt2, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CROTA1", &crota1, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CROTA2", &crota2, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        /////////////
        fits_read_key(fptr, TDOUBLE, "CD1_1", &cd1_1, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CD1_2", &cd1_2, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CD2_1", &cd2_1, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CD2_2", &cd2_2, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        //
    }
    if(wcs_Type==2)//SDSS
    {
        fits_read_key(fptr, TSTRING, "CTYPE1", strkey, NULL, &status);
        if(QString(strkey).contains("GSS"))
        {
            fits_read_key(fptr, TDOUBLE, "PPO3", &crpix1, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
            fits_read_key(fptr, TDOUBLE, "PPO6", &crpix2, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
            crpix1 = -crpix1/1000;
            crpix2 = crpix2/1000;
        }
        else
        {
        ////////////
            fits_read_key(fptr, TDOUBLE, "CRPIX1", &crpix1, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
            fits_read_key(fptr, TDOUBLE, "CRPIX2", &crpix2, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        }
        fits_read_key(fptr, TDOUBLE, "CRVAL1", &crval1, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CRVAL2", &crval2, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        ////////
        //cdelt1=0;
        //cdelt2=0;
        //crota1=0;
        //crota2=0;
        /////////////
        fits_read_key(fptr, TDOUBLE, "CD1_1", &cd1_1, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CD1_2", &cd1_2, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CD2_1", &cd2_1, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CD2_2", &cd2_2, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        ////////////
        cdelt1=sqrt(cd1_1*cd1_1+cd2_1*cd2_1);
        cdelt2=sqrt(cd2_1*cd2_1+cd2_2*cd2_2);
        double rho1 =180*atan2(-cd2_1/cdelt1,cd1_1/cdelt1)/M_PI;
        double rho2 = 180*atan2(cd1_2/cdelt2,cd2_2/cdelt2)/M_PI;
        if (rho1<0) rho1 = 360+rho1;
        if (rho2<0) rho2 = 360+rho2;
        crota1=rho1;
        crota2=rho2;
        ////////////
        fits_read_key(fptr, TSTRING, "DATE-OBS", strkey, NULL, &status);
            if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        str =QString(strkey);
        yr = str.section('-',0,0).toDouble();
        mm = str.section('-',1,1).toDouble();
        dd = str.section('-',2,2).toDouble();
        fits_read_key(fptr, TSTRING, "TAIHMS", strkey, NULL, &status);
            if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        str =QString(strkey);
        hr = str.section(':',0,0).toDouble();
        min = str.section(':',1,1).toDouble();
        sec = str.section(':',2,2).toDouble();
        mjd = getMJD(yr,mm,dd,hr,min,sec);
    }
    if(wcs_Type==3)//DSS
    {
        fits_read_key(fptr, TDOUBLE, "CRPIX1", &crpix1, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CRPIX2", &crpix2, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CRVAL1", &crval1, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CRVAL2", &crval2, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        //cdelt1=0;
        //cdelt2=0;
        //crota1=0;
        //crota2=0;
        /////////////
        fits_read_key(fptr, TDOUBLE, "CD1_1", &cd1_1, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CD1_2", &cd1_2, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CD2_1", &cd2_1, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CD2_2", &cd2_2, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        ////////////
        cdelt1=sqrt(cd1_1*cd1_1+cd2_1*cd2_1);
        cdelt2=sqrt(cd2_1*cd2_1+cd2_2*cd2_2);
        double rho1 =180*atan2(-cd2_1/cdelt1,cd1_1/cdelt1)/M_PI;
        double rho2 = 180*atan2(cd1_2/cdelt2,cd2_2/cdelt2)/M_PI;
        if (rho1<0) rho1 = 360+rho1;
        if (rho2<0) rho2 = 360+rho2;
        crota1=rho1;
        crota2=rho2;
        ////////////
        //yr,mm,dd,hr,min,sec,mjd,exptime;
        fits_read_key(fptr, TSTRING, "DATE-OBS", strkey, NULL, &status);
            if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        str =QString(strkey);
        yr = str.section('T',0,0).section('-',0,0).toDouble();
        mm = str.section('T',0,0).section('-',1,1).toDouble();
        dd = str.section('T',0,0).section('-',2,2).toDouble();
        hr = str.section('T',1,1).section(':',0,0).toDouble();
        min = str.section('T',1,1).section(':',1,1).toDouble();
        sec = str.section('T',1,1).section(':',2,2).toDouble();
        fits_read_key(fptr, TDOUBLE, "EXPOSURE", &exptime, NULL, &status);
            if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        mjd = getMJD(yr,mm,dd,hr,min,sec)+ exptime/(2*86400);
    }
    if(wcs_Type==4)//MicroObservatory
    {
        //yr,mm,dd,hr,min,sec,mjd,exptime;
        fits_read_key(fptr, TSTRING, "DATE-OBS", strkey, NULL, &status);
            if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        str =QString(strkey);
        yr = str.section('T',0,0).section('-',0,0).toDouble();
        mm = str.section('T',0,0).section('-',1,1).toDouble();
        dd = str.section('T',0,0).section('-',2,2).toDouble();
        hr = str.section('T',1,1).section(':',0,0).toDouble();
        min = str.section('T',1,1).section(':',1,1).toDouble();
        sec = str.section('T',1,1).section(':',2,2).toDouble();
        fits_read_key(fptr, TDOUBLE, "EXPTIME", &exptime, NULL, &status);
            if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        mjd = getMJD(yr,mm,dd,hr,min,sec)+ exptime/(2*86400);
        Year = 1858.87885010267 + mjd/365.25;
        //
        fits_read_key(fptr, TSTRING, "RA", strkey, NULL, &status);
            if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        str =QString(strkey);
        crval1 = str.toDouble();
        //
        fits_read_key(fptr, TSTRING, "DEC", strkey, NULL, &status);
            if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        str =QString(strkey);
        crval2 = str.toDouble();
        //
        fits_read_key(fptr, TDOUBLE, "CRPIX1", &crpix1, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CRPIX2", &crpix2, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CRVAL1", &crval1, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CRVAL2", &crval2, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CDELT1", &cdelt1, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CDELT2", &cdelt2, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CROTA1", &crota1, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CROTA2", &crota2, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        /////////////
        fits_read_key(fptr, TDOUBLE, "CD1_1", &cd1_1, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CD1_2", &cd1_2, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CD2_1", &cd2_1, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CD2_2", &cd2_2, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        //
    }
    if(wcs_Type==5)
    {
        fits_read_key(fptr, TDOUBLE, "CRPIX1", &crpix1, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CRPIX2", &crpix2, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CRVAL1", &crval1, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CRVAL2", &crval2, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        cdelt1=0;
        cdelt2=0;
        crota1=0;
        crota2=0;
        /////////////
        fits_read_key(fptr, TDOUBLE, "CD1_1", &cd1_1, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CD1_2", &cd1_2, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CD2_1", &cd2_1, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CD2_2", &cd2_2, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        //yr,mm,dd,hr,min,sec,mjd,exptime;
        yr = 0;
        mm = 0;
        dd = 0;
        hr = 0;
        min = 0;
        sec = 0;
        exptime=0;
        mjd = 0;
    }
    if(wcs_Type==6)//Zeiss 20sm guide last observed FITS-image
    {
        qDebug()<<"sao cam";
        //yr,mm,dd,hr,min,sec,mjd,exptime;
        fits_read_key(fptr, TSTRING, "DATE-OBS", strkey, NULL, &status);
            if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        str =QString(strkey);
        qDebug()<<str;
        yr = str.section('T',0,0).section('-',0,0).toDouble();
        mm = str.section('T',0,0).section('-',1,1).toDouble();
        dd = str.section('T',0,0).section('-',2,2).toDouble();
        hr = str.section('T',1,1).section(':',0,0).toDouble();
        min = str.section('T',1,1).section(':',1,1).toDouble();
        sec = str.section('T',1,1).section(':',2,2).toDouble();
        fits_read_key(fptr, TDOUBLE, "EXPTIME", &exptime, NULL, &status);
            if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        mjd = getMJD(yr,mm,dd,hr,min,sec)+ exptime/(2*86400);
        Year = 1858.87885010267 + mjd/365.25;
        //
        fits_read_key(fptr, TSTRING, "RA", strkey, NULL, &status);
            if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        str =QString(strkey);
        crval1 = str.toDouble();
        //
        fits_read_key(fptr, TSTRING, "DEC", strkey, NULL, &status);
            if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        str =QString(strkey);
        crval2 = str.toDouble();
        //

        Target ="SAO RAS CAM";
        //
        fits_read_key(fptr, TDOUBLE, "CRPIX1", &crpix1, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CRPIX2", &crpix2, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CRVAL1", &crval1, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CRVAL2", &crval2, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        //fits_read_key(fptr, TDOUBLE, "CDELT1", &cdelt1, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        //fits_read_key(fptr, TDOUBLE, "CDELT2", &cdelt2, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        //fits_read_key(fptr, TDOUBLE, "CROTA1", &crota1, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        //fits_read_key(fptr, TDOUBLE, "CROTA2", &crota2, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        /////////////
        fits_read_key(fptr, TDOUBLE, "CD1_1", &cd1_1, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CD1_2", &cd1_2, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CD2_1", &cd2_1, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        fits_read_key(fptr, TDOUBLE, "CD2_2", &cd2_2, NULL, &status);if(status!=0){isWcsExist=false;status=0;fits_close_file(fptr, &status);return;}
        //
        qDebug()<<QString("%1").arg(mjd,12,'f',6,QLatin1Char( ' ' ));
        qDebug()<<QString("%1").arg(crval1,12,'f',6,QLatin1Char( ' ' ));
        qDebug()<<QString("%1").arg(crval2,12,'f',6,QLatin1Char( ' ' ));
    }
    //////////////////////////////////
    Year = 1858.87885010267 + mjd/365.25;
    fits_close_file(fptr, &status);
    if(status!=0){isWcsExist=false;return;}
}

double wcsData::refpixRA(){return crval1;}
double wcsData::refpixDE(){return crval2;}
double wcsData::refpixX(){return crpix1;}
double wcsData::refpixY(){return crpix2;}
double wcsData::scalex(){return cdelt1;}
double wcsData::scaley(){return cdelt2;}
double wcsData::year(){return Year;}
double wcsData::ExpTime(){return exptime;}
QString wcsData::target(){return Target;}
int wcsData::width(){return w;}
int wcsData::height(){return h;}

wcsData::~wcsData(){}

bool wcsData::isExist(){return isWcsExist;}
int wcsData::wcsType(){return wcs_Type;}
double wcsData::MJD(){return mjd;}

void  wcsData::pixFromRADE(double ra, double de, double& x, double& y)
{
    double ksi,eta;
    tangFromRADE(ra,de,crval1*M_PI/180,crval2*M_PI/180,ksi,eta);
    ksi=ksi*180/M_PI;
    eta=eta*180/M_PI;
    //
    //ksi=ksi*3600;
    //eta=eta*3600;
    //if(wcs_Type==1)
    //{
        x = (eta-ksi*cd2_2/cd1_2)/(cd2_1-cd1_1*cd2_2/cd1_2);
        y = (ksi-cd1_1*x)/cd1_2;
        x+=crpix1;
        y+=crpix2;
    //}
}

void  wcsData::RADEFromPix(int x, int y, double& ra, double& de)
{
    double ksi = cd1_1*(x-crpix1)+cd1_2*(y-crpix2);
    double eta = cd2_1*(x-crpix1)+cd2_2*(y-crpix2);
    RADEFromTang(ksi*M_PI/180,eta*M_PI/180,crval1*M_PI/180,crval2*M_PI/180,ra,de);
}

void wcsData::pixFromXY(double X, double Y, double& x, double& y)
{
    x = (Y-X*cd2_2/cd1_2)/(cd2_1-cd1_1*cd2_2/cd1_2);
    y = (X-cd1_1*x)/cd1_2;
    x+=crpix1;
    y+=crpix2;
}
void  wcsData::XYFromPix(int x, int y, double& X, double& Y)
{
    X = cd1_1*(x-crpix1)+cd1_2*(y-crpix2)+crval1;
    Y = cd2_1*(x-crpix1)+cd2_2*(y-crpix2)+crval2;
}

void wcsData::FOV(double& fovW, double& fovH)
{
    double raTL,deTL,raBR,deBR;
    fovW=0;fovH=0;
    if(isExist())
    {
        RADEFromPix(0,0,raTL,deTL);
        RADEFromPix(w,h,raBR,deBR);
        double psi = ptp_anvle(raTL,deTL,raBR,deBR);
        fovW=psi*10800/M_PI;
        fovH=fovW;
    }
}

void wcsData::setWCS(double *Ax, double *Ay)
{
        isWcsExist=true;
        wcs_Type=1;
        crpix1=(Ay[1]*Ax[2]/Ax[1]-Ay[2])/(Ay[0]-Ay[1]*Ax[0]/Ax[1]);
        crpix2=-(Ax[2]+Ax[0]*crpix1)/Ax[1];
        //crval1=ra;
        //crval12=dec;
        cdelt1=sqrt(Ax[0]*Ax[0]+Ay[0]*Ay[0]);
        cdelt2=sqrt(Ax[1]*Ax[1]+Ay[1]*Ay[1]);
        double rho1 =180*atan2(-Ay[0]/cdelt1,Ax[0]/cdelt1)/M_PI;
        double rho2 = 180*atan2(Ax[1]/cdelt2,Ay[1]/cdelt2)/M_PI;
        if (rho1<0) rho1 = 360+rho1;
        if (rho2<0) rho2 = 360+rho2;
        crota1=rho1;
        crota2=rho2;
        cd1_1=Ax[0];
        cd1_2=Ax[1];
        cd2_1=Ay[0];
        cd2_2=Ay[1];
}

void wcsData::saveWCS(QString fpath, int flag, QString newfname)
{
    QString fname;
    if(flag==0) fname = '!'+fileName;
    if(flag==1)
    {
        QFileInfo fi(fileName);
        fname = '!'+fi.canonicalPath()+'/'+fi.completeBaseName()+"_wcs.fit";
    }
    if(flag==2)
    {
        QFileInfo fi(fileName);
        fname = '!'+fpath+'/'+fi.completeBaseName()+".fit";
    }
    if(flag==3)
    {
        fname = '!'+fpath+'/'+newfname+".fit";
    }
    int status=0;
    fitsfile *fptr;
    fitsfile *nfptr;
    fits_create_file(&nfptr, qPrintable(fname), &status);
    fits_open_file(&fptr, qPrintable(fileName), READONLY,&status);//open file
    fits_copy_hdu(fptr,nfptr,0,&status);
    fits_close_file(fptr, &status);
    ///////////////////////////////////
    char key1[]="RA---TAN";fits_update_key(nfptr, TSTRING, "CTYPE1",  key1, " ", &status);
    char key2[]="DEC--TAN";fits_update_key(nfptr, TSTRING, "CTYPE2",  key2, " ", &status);
    QString str;
    str = QString( "%1" ).arg(crpix1,24,'f',18,QLatin1Char( ' ' ));
    fits_update_key(nfptr, TSTRING, "CRPIX1", str.toAscii().data(), "ref pix 1", &status);
    str = QString( "%1" ).arg(crpix2,24,'f',18,QLatin1Char( ' ' ));
    fits_update_key(nfptr, TSTRING, "CRPIX2", str.toAscii().data(), "ref pix 2", &status);
    str = QString( "%1" ).arg(crval1,24,'f',18,QLatin1Char( ' ' ));
    fits_update_key(nfptr, TSTRING, "CRVAL1", str.toAscii().data(), "RA at ref pix", &status);
    str = QString( "%1" ).arg(crval2,24,'f',18,QLatin1Char( ' ' ));
    fits_update_key(nfptr, TSTRING, "CRVAL2", str.toAscii().data(), "DEC at ref pix", &status);
    str = QString( "%1" ).arg(cdelt1,24,'f',18,QLatin1Char( ' ' ));
    fits_update_key(nfptr, TSTRING, "CDELT1", str.toAscii().data(), "scale1 [deg/pix]", &status);
    str = QString( "%1" ).arg(cdelt2,24,'f',18,QLatin1Char( ' ' ));
    fits_update_key(nfptr, TSTRING, "CDELT2", str.toAscii().data(), "scale2 [deg/pix]", &status);
    str = QString( "%1" ).arg(crota1,24,'f',18,QLatin1Char( ' ' ));
    fits_update_key(nfptr, TSTRING, "CROTA1", str.toAscii().data(), "rotation1 [deg]", &status);
    str = QString( "%1" ).arg(crota2,24,'f',18,QLatin1Char( ' ' ));
    fits_update_key(nfptr, TSTRING, "CROTA2", str.toAscii().data(), "rotation2 [deg]", &status);
    str = QString( "%1" ).arg(cd1_1,24,'f',18,QLatin1Char( ' ' ));
    fits_update_key(nfptr, TSTRING, "CD1_1", str.toAscii().data(), "WCS CD matrix", &status);
    str = QString( "%1" ).arg(cd1_2,24,'f',18,QLatin1Char( ' ' ));
    fits_update_key(nfptr, TSTRING, "CD1_2", str.toAscii().data(), "WCS CD matrix", &status);
    str = QString( "%1" ).arg(cd2_1,24,'f',18,QLatin1Char( ' ' ));
    fits_update_key(nfptr, TSTRING, "CD2_1", str.toAscii().data(), "WCS CD matrix", &status);
    str = QString( "%1" ).arg(cd2_2,24,'f',18,QLatin1Char( ' ' ));
    fits_update_key(nfptr, TSTRING, "CD2_2", str.toAscii().data(), "WCS CD matrix", &status);
    char key4[]="deg";fits_update_key(nfptr, TSTRING, "CUNIT1", key4, "unit 1", &status);
    fits_update_key(nfptr, TSTRING, "CUNIT2", key4, "unit 2", &status);
    fits_close_file(nfptr, &status);
}

QString wcsData::wcstostring()
{
    QString str="";
    str+="crpix1 = "+ QString("%1").arg(crpix1,20,'f',8,QLatin1Char( ' ' ))+"\n";
    str+="crpix2 = "+ QString("%1").arg(crpix2,20,'f',8,QLatin1Char( ' ' ))+"\n";
    str+="crval1 = "+ QString("%1").arg(crval1,20,'f',8,QLatin1Char( ' ' ))+"\n";
    str+="crval2 = "+ QString("%1").arg(crval2,20,'f',8,QLatin1Char( ' ' ))+"\n";
    str+="cdelt1 = "+ QString("%1").arg(cdelt1,20,'f',8,QLatin1Char( ' ' ))+"\n";
    str+="cdelt2 = "+ QString("%1").arg(cdelt2,20,'f',8,QLatin1Char( ' ' ))+"\n";
    str+="crota1 = "+ QString("%1").arg(crota1,20,'f',8,QLatin1Char( ' ' ))+"\n";
    str+="crota2 = "+ QString("%1").arg(crota2,20,'f',8,QLatin1Char( ' ' ))+"\n";
    //
    str+="cd1_1 = "+ QString("%1").arg(cd1_1,20,'f',8,QLatin1Char( ' ' ))+"\n";
    str+="cd1_2 = "+ QString("%1").arg(cd1_2,20,'f',8,QLatin1Char( ' ' ))+"\n";
    str+="cd2_1 = "+ QString("%1").arg(cd2_1,20,'f',8,QLatin1Char( ' ' ))+"\n";
    str+="cd2_2 = "+ QString("%1").arg(cd2_2,20,'f',8,QLatin1Char( ' ' ))+"\n";
    str+="\n";
    str+=DATEfromMJD(mjd,":",1,1)+"\n";
    str+="year = "+ QString("%1").arg(Year,20,'f',8,QLatin1Char( ' ' ))+"\n";
    return str;
}

//END wcsData class implementation


//BEGIN fitsData class implementation
//constructor

fitsData::fitsData(QString filename)
{
    fileName = filename;
    //////////////////
    errCode=0;
    fitsfile *fptr;//pointer of FITSfile
    int status=0;//status
    fits_open_file(&fptr, qPrintable(filename), READONLY,&status);//open file
    if(status!=0){errCode=status;return;}
    fits_get_img_equivtype(fptr, &bpix, &status);//determination of type of data (bpix)
    if (status!=0){errCode=status;return;}
    long naxes[2];//size of the image
    fits_get_img_size(fptr, 2, naxes, &status);//determination of size of the image
    if (status!=0){errCode=status;return;}
    w = naxes[0];h = naxes[1];
    //
    readHeader(fptr);
    ///////////////////////////////
    //BEGIN read image data
    LONGLONG nelements = naxes[0]*naxes[1];//calculation of the number of elements (pixels) of the image
    int anynul = 0;//service variable
    long felem = 1;//service variable
    switch(bpix)
    {
        case USHORT_IMG:
        {
            ushD = new unsigned short[nelements];
            fits_read_img(fptr, TUSHORT, felem, nelements, NULL, ushD ,&anynul, &status);
            break;
        }
        case SHORT_IMG:
        {
            shD = new short[nelements];
            fits_read_img(fptr, TSHORT, felem, nelements, NULL, shD ,&anynul, &status);
            break;
        }
        case LONG_IMG:
        {
            lD = new long[nelements];
            fits_read_img(fptr, TLONG, felem, nelements, NULL, lD ,&anynul, &status);
            break;
        }
        case FLOAT_IMG:
        {
            fD = new float[nelements];
            fits_read_img(fptr, TFLOAT, felem, nelements, NULL, fD, &anynul, &status);
            break;
        }
        case DOUBLE_IMG:
        {
            dD = new double[nelements];
            fits_read_img(fptr, TDOUBLE, felem, nelements, NULL, dD, &anynul, &status);
            break;
        }
    }
    if (status!=0){errCode=status;return;}
    //END read image data
    //////////////////////////////
    fits_close_file(fptr, &status);
    if (status!=0){errCode=status;return;}
    //
    Hist();
    //leftlim = 100;
    //rightlim = 600;
}

int fitsData::width(){return w;}

int fitsData::height(){return h;}

QString fitsData::filename(){return fileName;}

int fitsData::datatype(){return bpix;}

int fitsData::error(){return errCode;}

QString fitsData::header(){return Header;}//{return Header;}

QString fitsData::propertiesAsText(){return ptext;}

double fitsData::pix(int x, int y)
{
    if((x>=0)&&(y>=0)&&(x<w)&&(y<h))
    {
        switch(bpix)
        {
            case USHORT_IMG:{return (double)ushD[x+y*w];break;}
            case SHORT_IMG: {return (double)shD[x+y*w];break;}
            case LONG_IMG:  {return (double)lD[x+y*w];break;}
            case FLOAT_IMG: {return (double)fD[x+y*w];break;}
            case DOUBLE_IMG:{return (double)dD[x+y*w];break;}
        }
        return 0;
    }else return 0;
}

void fitsData::getAperture(double *ap, int cx, int cy, int rho)
{
    int ulX = cx-rho;
    int ulY = cy-rho;
    double r;
    for(int y = 0; y<2*rho;y++)
        for(int x = 0; x<2*rho;x++)
        {
            r = sqrt((x-rho)*(x-rho)+(y-rho)*(y-rho));
            if((ulX+x>=0)&&(ulX+x<w)&&(ulY+y>=0)&&(ulY+y<h)&&(r<=rho))
            {
                switch(bpix)
                {
                    case USHORT_IMG:{ap[y*2*rho+x] = (double)ushD[(ulY+y)*w+(ulX+x)];break;}
                    case SHORT_IMG: {ap[y*2*rho+x] = (double)shD[(ulY+y)*w+(ulX+x)];break;}
                    case LONG_IMG:  {ap[y*2*rho+x] = (double)lD[(ulY+y)*w+(ulX+x)];break;}
                    case FLOAT_IMG: {ap[y*2*rho+x] = (double)fD[(ulY+y)*w+(ulX+x)];break;}
                    case DOUBLE_IMG:{ap[y*2*rho+x] = (double)dD[(ulY+y)*w+(ulX+x)];break;}
                }//end switch

            }//end if
            else ap[y*2*rho+x] = -10E-9;
        }//end for
}


void fitsData::getRing(double *ap, int cx, int cy, int rho1, int rho2)
{
    int ulX = cx-rho2;
    int ulY = cy-rho2;
    double r;
    for(int y = 0; y<2*rho2;y++)
        for(int x = 0; x<2*rho2;x++)
        {
            r = sqrt((x-rho2)*(x-rho2)+(y-rho2)*(y-rho2));
            if((ulX+x>=0)&&(ulX+x<w)&&(ulY+y>=0)&&(ulY+y<h)&&(r>rho1)&&(r<=rho2))
            {
                switch(bpix)
                {
                    case USHORT_IMG:{ap[y*2*rho2+x] = (double)ushD[(ulY+y)*w+(ulX+x)];break;}
                    case SHORT_IMG: {ap[y*2*rho2+x] = (double)shD[(ulY+y)*w+(ulX+x)];break;}
                    case LONG_IMG:  {ap[y*2*rho2+x] = (double)lD[(ulY+y)*w+(ulX+x)];break;}
                    case FLOAT_IMG: {ap[y*2*rho2+x] = (double)fD[(ulY+y)*w+(ulX+x)];break;}
                    case DOUBLE_IMG:{ap[y*2*rho2+x] = (double)dD[(ulY+y)*w+(ulX+x)];break;}
                }//end switch

            }//end if
            else ap[y*2*rho2+x] = -10E-9;
        }//end for
}

void fitsData::getBox(double *ap, int cx, int cy, int bsz)
{
    int ulX = cx-bsz;
    int ulY = cy-bsz;
    int wb = 2*bsz+1;
    for(int y=0;y<wb;y++)
        for(int x=0;x<wb;x++)
            {
                if((ulX+x>=0)&&(ulX+x<w)&&(ulY+y>=0)&&(ulY+y<h))
                {
                    switch(bpix)
                    {
                        case USHORT_IMG:{ap[y*wb+x] = (double)ushD[(ulY+y)*w+(ulX+x)];break;}
                        case SHORT_IMG: {ap[y*wb+x] = (double)shD[(ulY+y)*w+(ulX+x)];break;}
                        case LONG_IMG:  {ap[y*wb+x] = (double)lD[(ulY+y)*w+(ulX+x)];break;}
                        case FLOAT_IMG: {ap[y*wb+x] = (double)fD[(ulY+y)*w+(ulX+x)];break;}
                        case DOUBLE_IMG:{ap[y*wb+x] = (double)dD[(ulY+y)*w+(ulX+x)];break;}
                    }//end switch
                }
                else
                    ap[y*wb+x] = -10E-9;
            }
}


double fitsData::leftlimit(){return leftlim;}
double fitsData::rightlimit(){return rightlim;}
int fitsData::leftlimitN(){return w1;}
int fitsData::rightlimitN(){return w2;}
double fitsData::mostprobpix(){return mpel;}

void fitsData::Hist()
{
    unsigned long nelements = w*h;//number of elements of the image
    nhist =600; // size of the histogramm
    int step;//step for selection of pixel to construct the histogramm
    int nselpix = 1024*1024;//size of the buffer of selected pixels to construct the histogramm
    if(nelements<nselpix)step=1;else//select all pixels for small images
    {
        step = nelements/nselpix;//select each of (nelements/nselpix)-th pixel
    }
    //select pixels and find maximum and minimum
    double* selpix = new double[nselpix];
    maxp = pix(0,0); minp = pix(0,0);
    double pD;
    int k=0;
    for(int y=0;y<h;y+=step)
        for(int x=0;x<w;x+=step)
        {
            pD = pix(x,y);
            if(k<nselpix){selpix[k]=pD;k++;}
            if(pD>maxp)maxp=pD;
            if(pD<minp)minp=pD;
    }
    if(maxp-minp==0)return;
    for(int i=0;i<nselpix;i++)selpix[i]=10000*(selpix[i]-minp)/(maxp-minp);
    //construct the histogramm
    hist = new unsigned long[nhist];
    for(int i=0;i<nhist;i++) hist[i]=0;//set all elements of histogramm to zero
    for(int i=0;i<nselpix;i++)
    {
        k = (int)floor(nhist*log(selpix[i])/log(10000));
        //if(selpix[i]>0)qDebug()<<QString("%1").arg(selpix[i],15,'f',6,QLatin1Char( ' ' ))<<endl;
        if((k<nhist)&&(k>=0))hist[k]++;
    }
    //qDebug() <<"ok";
    delete[] selpix;//free buffer of selected pixels
    //calculates the limits to screen image manipulations (contrast/brightness)
    k=0;
    for(int i=0;i<nhist;i++)if(hist[i]>hist[k])k=i;
    maxhistel=k;
    if(k-50>0)w1=k-50;else w1=0;
    if(k+50<nhist)w2=k+50;else w2=nhist;
    //w1=0;w2=nhist;
    leftlim = 1E-4*(maxp-minp)*exp(w1*log(1E4)/nhist)+minp;
    rightlim = 1E-4*(maxp-minp)*exp(w2*log(1E4)/nhist)+minp;
    mpel = 1E-4*(maxp-minp)*exp(k*log(1E4)/nhist)+minp;
    ptext = "min pix value = "
            +QString("%1").arg(minp,15,'f',6,QLatin1Char( ' ' ))+
            "\n"
            +"max pix value = "
            +QString("%1").arg(maxp,15,'f',6,QLatin1Char( ' ' ))+
            "\n"
            +"max of histogram pix value = "
            +QString("%1").arg(1E-4*(maxp-minp)*exp(k*log(1E4)/nhist)+minp,15,'f',6,QLatin1Char( ' ' ))+
            "\n"
            +"index of max of histogram pix = "
            +QString("%1").arg(k,15,'f',0,QLatin1Char( ' ' ))+
            "\n"
            +"pix step of histogram = "
            +QString("%1").arg(step,15,'f',0,QLatin1Char( ' ' ))+
            "\n"
            ;
    //leftlim = minp;
    //rightlim = maxp;
}

void fitsData::readHeader(fitsfile *fptr)
{
    errCode=0;
    int status=0;
    int nkeys;//variable to store number of line of the header
    fits_get_hdrspace(fptr, &nkeys, NULL, &status);
    if(status!=0){errCode=status;return;}
    char card[FLEN_CARD];//buffer to read keys from header
    Header="";//strings from header
    for (int k = 1; k <= nkeys; k++)//reading strings from header
    {
       fits_read_record(fptr, k, card, &status);// read separate record
       if(status!=0){errCode=status;return;}
       Header+= QString(card)+"\n";// add record
    }
}

//void fitsData::readWcs(fitsfile *fptr){}

QPixmap fitsData::histPixmap()
{
    QPixmap pixmap(nhist,200);
    pixmap.fill(Qt::black);
    QPainter painter(&pixmap);
    for(int i=0;i<nhist;i++)
    {
        if((i>w1)&&(i<w2))
        {
            painter.setPen(Qt::white);
            painter.drawLine(QPoint(i,200),
                         QPoint(i,200-180*(double)hist[i]/(double)hist[maxhistel]));
        }
        else
        {
            painter.setPen(Qt::gray);
            painter.drawLine(QPoint(i,200),
                         QPoint(i,200-180*(double)hist[i]/(double)hist[maxhistel]));
        }
    }
    return pixmap;
}
//
QPixmap fitsData::profilePixmap(int X1, int Y1, int X2, int Y2)
{
    int r = sqrt((X2-X1)*(X2-X1)+(Y2-Y1)*(Y2-Y1));
    double cosA=(double)(X2-X1)/r;
    double sinA=(double)(Y2-Y1)/r;
    double *pixels = new double[r];
    int x,y;
    for(int R=0;R<r;R++)
    {
        x=X1+R*cosA;
        y=Y1+R*sinA;
        pixels[R]=pix(x,y);
    }
    //
    double maxPr=pixels[0];
    double minPr=pixels[0];
    for(int i=0;i<r;i++)
    {
        if(pixels[i]>maxPr)maxPr=pixels[i];
        if(pixels[i]<minPr)minPr=pixels[i];
    }
    ////////////////////////
    QPixmap pixmap(600,200);
    pixmap.fill(Qt::black);
    QPainter painter(&pixmap);
    painter.setPen(Qt::white);
    for(int i=0;i<r;i++)
    {
        x = 600*i/r;
        y = 190 - 180*(pixels[i]-minPr)/(maxPr-minPr);
        painter.drawLine(QPoint(x,190),QPoint(x,y));
    }
    delete pixels;
    return pixmap;
}
//
void fitsData::setRightLimit(int W){w2=W;rightlim = 1E-4*(maxp-minp)*exp(w2*log(1E4)/nhist)+minp;}
void fitsData::setLeftLimit(int W){w1=W;leftlim = 1E-4*(maxp-minp)*exp(w1*log(1E4)/nhist)+minp;}
//destructor
fitsData::~fitsData()
{
    //QMessageBox::information(0,"fitsviewer","ok",QMessageBox::Ok,0,0);
    switch(bpix)
    {
        case USHORT_IMG:{delete ushD;break;}
        case SHORT_IMG: {delete shD;break;}
        case LONG_IMG:  {delete lD;break;}
        case FLOAT_IMG: {delete fD;break;}
        case DOUBLE_IMG:{delete dD;break;}
    }
    delete hist;
}

//END fitsData class implementation



void findStars(fitsData *fd, QVector<int>& cmX, QVector<int>&cmY)
{
    cmX.clear();cmY.clear();
    int bsize = 40;
    int n = fd->width()/bsize;
    int m = fd->height()/bsize;
    int flag;
    int N=8;
    int XM[N];
    int YM[N];
    int xm,ym;
    double xmean,ymean,ex,ey,D,r;
    for(int i=0;i<m;i++)
        for(int j=0;j<n;j++)//loop to view all boxes bsize x bsize
        {
           for(int k=0;k<N;k++)//form the list of positions of eight largest value
            {
               ym = i*bsize;xm =j*bsize;
               for(int y=i*bsize;y<(i+1)*bsize;y++)
                   for(int x=j*bsize;x<(j+1)*bsize;x++)
                       if(fd->pix(x,y)>fd->pix(xm,ym))
                        {
                            flag=0; for(int q=0;q<k;q++) if((XM[q]==x)&&(YM[q]==y)) flag=1;
                            if(flag==0){xm=x;ym=y;}
                        }
               XM[k] = xm; YM[k] = ym;
            }
           ///////////////////////////////////////////////////////////
           xmean = 0; ymean = 0;
           for(int k=0;k<N;k++){xmean+=XM[k]; ymean+=YM[k];}
           xmean = xmean/N; ymean = ymean/N;
           ex = 0; ey = 0;
           for(int k=0;k<N;k++)
            {
                ex+=(XM[k]-xmean)*(XM[k]-xmean);
                ey+=(YM[k]-ymean)*(YM[k]-ymean);
            }
           D = sqrt(ex/(N-1)+ey/(N-1));
           if(D<2)
           {
               flag=0;
               for(int k=0;k<cmX.count();k++)
               {
                   r=sqrt((xmean-cmX[k])*(xmean-cmX[k])+(ymean-cmY[k])*(ymean-cmY[k]));
                   if(r<bsize/4){cmX[k]=(cmX[k]+xmean)/2; cmY[k]=(cmY[k]+ymean)/2; flag=1; break;}
               }
               if(flag==0){cmX<<(int)floor(xmean);cmY<<(int)floor(ymean);}
           }
        }//end of loop to view all boxes bsize x bsize


}//end of function findStars


int matchStars(QVector<double> X, QVector<double> Y,
                QVector<double> x, QVector<double>y,
                double* Ax, double* Ay,
                double e, int Nmin, int Nmax)
{
    //form the list of number of points of triangulums constructed from x,y data
    int nx;
    if(x.count()<Nmax)
        nx=x.count();else nx=Nmax;
    int NX;
    if(X.count()<Nmax)
        NX=X.count();else NX=Nmax;

    QList<int*> triangle;
    double R,r;
    int* trig;
    for(int i=0;i<nx-2;i++)
        for(int j=i+1;j<nx-1;j++)
            for(int k=j+1;k<nx;k++)
            {
                trig = new int[3];trig[0]=i;trig[1]=j;trig[2]=k;triangle << trig;
                if(nx<=NX)
                {
                    trig = new int[3];trig[0]=i;trig[1]=k;trig[2]=j;triangle << trig;
                    //
                    trig = new int[3];trig[0]=j;trig[1]=i;trig[2]=k;triangle << trig;
                    trig = new int[3];trig[0]=j;trig[1]=k;trig[2]=i;triangle << trig;
                    //
                    trig = new int[3];trig[0]=k;trig[1]=i;trig[2]=j;triangle << trig;
                    trig = new int[3];trig[0]=k;trig[1]=j;trig[2]=i;triangle << trig;
                }//end if(nx<=NX)
            }//end for
    //

    //form the list of number of points of triangulums constructed from X,Y data
    QList<int*> TRIANGLE;
    for(int i=0;i<NX-2;i++)
        for(int j=i+1;j<NX-1;j++)
            for(int k=j+1;k<NX;k++)
            {
                trig = new int[3];trig[0]=i;trig[1]=j;trig[2]=k;TRIANGLE << trig;
                if(nx>NX)
                {
                    trig = new int[3];trig[0]=i;trig[1]=k;trig[2]=j;TRIANGLE << trig;
                    //
                    trig = new int[3];trig[0]=j;trig[1]=i;trig[2]=k;TRIANGLE << trig;
                    trig = new int[3];trig[0]=j;trig[1]=k;trig[2]=i;TRIANGLE << trig;
                    //
                    trig = new int[3];trig[0]=k;trig[1]=i;trig[2]=j;TRIANGLE << trig;
                    trig = new int[3];trig[0]=k;trig[1]=j;trig[2]=i;TRIANGLE << trig;
                }//end if
            }//end for
    /////////////////
    //form the list of SRA (Sizes Ratio and Angle)
    double *sra = new double[triangle.count()*2];
    double psi;
    for(int i=0;i<triangle.count();i++)
    {
        triangSRA(x[triangle[i][0]],y[triangle[i][0]],
                  x[triangle[i][1]],y[triangle[i][1]],
                  x[triangle[i][2]],y[triangle[i][2]],
                  psi,r);
        sra[i*2+0]=r;sra[i*2+1]=psi;
    }
    double *SRA = new double[TRIANGLE.count()*2];
    for(int i=0;i<TRIANGLE.count();i++)
    {
        triangSRA(X[TRIANGLE[i][0]],Y[TRIANGLE[i][0]],
                  X[TRIANGLE[i][1]],Y[TRIANGLE[i][1]],
                  X[TRIANGLE[i][2]],Y[TRIANGLE[i][2]],
                  psi,r);
        SRA[i*2+0]=r;SRA[i*2+1]=psi;
        //qDebug() << QString("%1").arg(r,10,'f',6,QLatin1Char( ' ' ))<<"|"
        //         << QString("%1").arg(psi,10,'f',6,QLatin1Char( ' ' ));
    }
    //search for similar triangulums
    //qDebug() << "triangles = "+QString("%1").arg(triangle.count(),10,'f',0,QLatin1Char( ' ' ));
    //qDebug() << "TRIANGLES = "+QString("%1").arg(TRIANGLE.count(),10,'f',0,QLatin1Char( ' ' ));
    double PSI;
    //int N = triangle.count()*TRIANGLE.count();
    //QVector<double>S;
    QVector<int>I,J;
    double d;
    //QFile file("log.dat");
    //file.open(QIODevice::WriteOnly|QIODevice::Text);
    //QTextStream stream(&file);

    for(int i=0;i<triangle.count();i++)
    {
        for(int j=0;j<TRIANGLE.count();j++)
        {
            r=sra[i*2+0];psi=sra[i*2+1];
            R=SRA[j*2+0];PSI=SRA[j*2+1];
            d=(R-r)*(R-r)+(PSI-psi)*(PSI-psi);
            if(d<0.001)
            {
                I<<i;
                J<<j;
            }//end if d<1
        }//end of for(int j=
    }//end for(int i=
    //file.close();
    //qDebug() <<"\n-----------------------------------\n";
    //for(int i=0;i<S.count();i++)
    //{
    //    qDebug() << QString("%1").arg(I[i],10,'f',0,QLatin1Char( ' ' )) +" | "
    //    +QString("%1").arg(J[i],10,'f',0,QLatin1Char( ' ' )) +" | "
    //    +QString("%1").arg(S[i],20,'f',8,QLatin1Char( ' ' ));
    //}
    ///////////////////////////////

    if(J.count()==0)
    {
        for(int i=0;i<triangle.count();i++)
        {
            trig = triangle[i];
            delete[] trig;
        }
        for(int i=0;i<TRIANGLE.count();i++)
        {
            trig = TRIANGLE[i];
            delete[] trig;
        }
        delete[] sra;delete[] SRA;
        return 1;
    }//end if S

    int P,Q;
    double Rho1,Rho2,scale1,scale2,kappa;
    double *ksi=new double[3];
    double *eta=new double[3];
    double *M=new double[9];
    //qDebug() <<"\n++++++++++++++++++++++++++++++++++++\n";
    for(int i=0;i<J.count();i++)
    {
        P=I[i];Q=J[i];
        for(int j=0;j<3;j++)
            {
                ksi[j]= x[triangle[P][j]];
                eta[j]= y[triangle[P][j]];
                M[j*3+0]=X[TRIANGLE[Q][j]];
                M[j*3+1]=Y[TRIANGLE[Q][j]];
                M[j*3+2]=1;
            }//for j
        ssleg(M,Ax,ksi,3);ssleg(M,Ay,eta,3);
        scale1=sqrt(Ax[0]*Ax[0]+Ay[0]*Ay[0]);
        scale2=sqrt(Ax[1]*Ax[1]+Ay[1]*Ay[1]);
        Rho1 =180*atan2(-Ay[0]/scale1,Ax[0]/scale1)/M_PI;
        Rho2 = 180*atan2(Ax[1]/scale2,Ay[1]/scale2)/M_PI;
        if (Rho1<0) Rho1 = 360+Rho1;
        if (Rho2<0) Rho2 = 360+Rho2;
        if(fabs(Rho1-Rho2)>85)
        {
            kappa = fabs(floor((Rho1-Rho2)/90) - (Rho1-Rho2)/90);
            if(kappa>0.9)kappa=1-kappa;
            if(kappa<0.001)
            {
                /*
            qDebug() << //QString("%1").arg(S[i],10,'f',5,QLatin1Char( ' ' )) +" | "
                        QString("%1").arg(kappa,10,'f',5,QLatin1Char( ' ' )) +" | "
                        +QString("%1").arg(Rho1-Rho2,20,'f',8,QLatin1Char( ' ' )) +" | "
                        //+QString("%1").arg(scale1/scale2,20,'f',8,QLatin1Char( ' ' ))+" | "
                         +QString("%1").arg(scale1,20,'f',8,QLatin1Char( ' ' ))+" | "
                         +QString("%1").arg(scale2,20,'f',8,QLatin1Char( ' ' ));//*/
            if(comparePointsSets(x,y,X,Y,Ax,Ay,e,Nmin)==1)//3.0/3600.0,8)==1)
            {
                delete[] ksi;delete[] eta; delete[] M;
                for(int i=0;i<triangle.count();i++)
                {
                    trig = triangle[i];
                    delete[] trig;
                }
                for(int i=0;i<TRIANGLE.count();i++)
                {
                    trig = TRIANGLE[i];
                    delete[] trig;
                }
                delete[] sra;delete[] SRA;
                return 0;
            }//if comparePointsSets
            }
        }//end if (fabs(Rho1-Rho2)>87)
    }//end for i
 //qDebug() <<"\n!!!\n";
    delete[] ksi;delete[] eta; delete[] M;
    for(int i=0;i<triangle.count();i++)
    {
        trig = triangle[i];
        delete[] trig;
    }
    for(int i=0;i<TRIANGLE.count();i++)
    {
        trig = TRIANGLE[i];
        delete[] trig;
    }
    delete[] sra;delete[] SRA;
    //qDebug() <<"\n!!!\n";
    return 1;
}//end of function


/*int matchStars(QVector<double> X, QVector<double> Y,
                QVector<double> x, QVector<double>y,
                double* Ax, double* Ay, double e)
{
    //form the list of number of points of triangulums constructed from x,y data
    int nx;
    if(x.count()<30)
        nx=x.count();else nx=30;
    int NX;
    if(X.count()<30)
        NX=X.count();else NX=30;
    QList<int*> triangle;
    double R,r;
    int* trig;
    for(int i=0;i<nx-2;i++)
        for(int j=i+1;j<nx-1;j++)
            for(int k=j+1;k<nx;k++)
            {
                trig = new int[3];trig[0]=i;trig[1]=j;trig[2]=k;triangle << trig;
                if(nx<=NX)
                {
                    trig = new int[3];trig[0]=i;trig[1]=k;trig[2]=j;triangle << trig;
                    //
                    trig = new int[3];trig[0]=j;trig[1]=i;trig[2]=k;triangle << trig;
                    trig = new int[3];trig[0]=j;trig[1]=k;trig[2]=i;triangle << trig;
                    //
                    trig = new int[3];trig[0]=k;trig[1]=i;trig[2]=j;triangle << trig;
                    trig = new int[3];trig[0]=k;trig[1]=j;trig[2]=i;triangle << trig;
                }//end if(nx<=NX)
            }//end for
    //
    //form the list of number of points of triangulums constructed from X,Y data
    QList<int*> TRIANGLE;
    for(int i=0;i<NX-2;i++)
        for(int j=i+1;j<NX-1;j++)
            for(int k=j+1;k<NX;k++)
            {
                trig = new int[3];trig[0]=i;trig[1]=j;trig[2]=k;TRIANGLE << trig;
                if(nx>NX)
                {
                    trig = new int[3];trig[0]=i;trig[1]=k;trig[2]=j;TRIANGLE << trig;
                    //
                    trig = new int[3];trig[0]=j;trig[1]=i;trig[2]=k;TRIANGLE << trig;
                    trig = new int[3];trig[0]=j;trig[1]=k;trig[2]=i;TRIANGLE << trig;
                    //
                    trig = new int[3];trig[0]=k;trig[1]=i;trig[2]=j;TRIANGLE << trig;
                    trig = new int[3];trig[0]=k;trig[1]=j;trig[2]=i;TRIANGLE << trig;
                }//end if
            }//end for
    /////////////////
    //form the list of SRA (Sizes Ratio and Angle)
    double *sra = new double[triangle.count()*2];
    double psi;
    for(int i=0;i<triangle.count();i++)
    {
        triangSRA(x[triangle[i][0]],y[triangle[i][0]],
                  x[triangle[i][1]],y[triangle[i][1]],
                  x[triangle[i][2]],y[triangle[i][2]],
                  psi,r);
        sra[i*2+0]=r;sra[i*2+1]=psi;
    }
    double *SRA = new double[TRIANGLE.count()*2];
    for(int i=0;i<TRIANGLE.count();i++)
    {
        triangSRA(X[TRIANGLE[i][0]],Y[TRIANGLE[i][0]],
                  X[TRIANGLE[i][1]],Y[TRIANGLE[i][1]],
                  X[TRIANGLE[i][2]],Y[TRIANGLE[i][2]],
                  psi,r);
        SRA[i*2+0]=r;SRA[i*2+1]=psi;
        //qDebug() << QString("%1").arg(r,10,'f',6,QLatin1Char( ' ' ))<<"|"
        //         << QString("%1").arg(psi,10,'f',6,QLatin1Char( ' ' ));
    }
    //search for similar triangulums
    //qDebug() << "triangles = "+QString("%1").arg(triangle.count(),10,'f',0,QLatin1Char( ' ' ));
    //qDebug() << "TRIANGLES = "+QString("%1").arg(TRIANGLE.count(),10,'f',0,QLatin1Char( ' ' ));
    double PSI;
    //int N = triangle.count()*TRIANGLE.count();
    QVector<double>S;
    QVector<int>I,J;
    double d,s1,s2;
    //QFile file("log.dat");
    //file.open(QIODevice::WriteOnly|QIODevice::Text);
    //QTextStream stream(&file);

    for(int i=0;i<triangle.count();i++)
    {
        for(int j=0;j<TRIANGLE.count();j++)
        {
            r=sra[i*2+0];psi=sra[i*2+1];
            R=SRA[j*2+0];PSI=SRA[j*2+1];
            d=(R-r)*(R-r)+(PSI-psi)*(PSI-psi);
            if(d<0.01)
            {
                I<<i;
                J<<j;
                ///////////
                r = (x[triangle[i][1]]-x[triangle[i][0]])*(x[triangle[i][1]]-x[triangle[i][0]])+
                    (y[triangle[i][1]]-y[triangle[i][0]])*(y[triangle[i][1]]-y[triangle[i][0]]);
                R = (x[triangle[i][2]]-x[triangle[i][0]])*(x[triangle[i][2]]-x[triangle[i][0]])+
                    (y[triangle[i][2]]-y[triangle[i][0]])*(y[triangle[i][2]]-y[triangle[i][0]]);
                s1 = r*R*sin(M_PI*psi/180);
                /////////////
                r = (X[TRIANGLE[j][1]]-X[TRIANGLE[j][0]])*(X[TRIANGLE[j][1]]-X[TRIANGLE[j][0]])+
                    (Y[TRIANGLE[j][1]]-Y[TRIANGLE[j][0]])*(Y[TRIANGLE[j][1]]-Y[TRIANGLE[j][0]]);
                R = (X[TRIANGLE[j][2]]-X[TRIANGLE[j][0]])*(X[TRIANGLE[j][2]]-X[TRIANGLE[j][0]])+
                    (Y[TRIANGLE[j][2]]-Y[TRIANGLE[j][0]])*(Y[TRIANGLE[j][2]]-Y[TRIANGLE[j][0]]);
                s2 = r*R*sin(M_PI*PSI/180);
                //////////////
                S<<s2/s1;
            }//end if d<1
        }//end of for(int j=
    }//end for(int i=
    //file.close();
    //qDebug() <<"\n++++++++++++++++++++++++++++++++++++\n";
    //for(int i=0;i<S.count();i++)
    //{
    //    qDebug() << QString("%1").arg(I[i],10,'f',0,QLatin1Char( ' ' )) +" | "
    //    +QString("%1").arg(J[i],10,'f',0,QLatin1Char( ' ' )) +" | "
    //    +QString("%1").arg(S[i],20,'f',8,QLatin1Char( ' ' ));
    //}
    ///////////////////////////////

    if(S.count()==0)
    {
        for(int i=0;i<triangle.count();i++)
        {
            trig = triangle[i];
            delete[] trig;
        }
        for(int i=0;i<TRIANGLE.count();i++)
        {
            trig = TRIANGLE[i];
            delete[] trig;
        }
        delete[] sra;delete[] SRA;
        return 1;
    }//end if S
    //double s=mfv(S,0.005);

    int P,Q;
    double Rho1,Rho2,scale1,scale2,kappa;
    double *ksi=new double[3];
    double *eta=new double[3];
    double *M=new double[9];
    qDebug() <<"\n++++++++++++++++++++++++++++++++++++\n";
    for(int i=0;i<S.count();i++)
    {
        P=I[i];Q=J[i];
        for(int j=0;j<3;j++)
            {
                ksi[j]= x[triangle[P][j]]/3600;
                eta[j]= y[triangle[P][j]]/3600;
                M[j*3+0]=X[TRIANGLE[Q][j]];
                M[j*3+1]=Y[TRIANGLE[Q][j]];
                M[j*3+2]=1;
            }//for j
        ssleg(M,Ax,ksi,3);ssleg(M,Ay,eta,3);
        scale1=sqrt(Ax[0]*Ax[0]+Ay[0]*Ay[0]);
        scale2=sqrt(Ax[1]*Ax[1]+Ay[1]*Ay[1]);
        Rho1 =180*atan2(-Ay[0]/scale1,Ax[0]/scale1)/M_PI;
        Rho2 = 180*atan2(Ax[1]/scale2,Ay[1]/scale2)/M_PI;
        if (Rho1<0) Rho1 = 360+Rho1;
        if (Rho2<0) Rho2 = 360+Rho2;
        if(fabs(Rho1-Rho2)>85)
        {
            kappa = fabs(floor((Rho1-Rho2)/90) - (Rho1-Rho2)/90);
            if(kappa>0.9)kappa=1-kappa;
            if(kappa<0.005)
                //*
            qDebug() << QString("%1").arg(S[i],10,'f',5,QLatin1Char( ' ' )) +" | "
                        +QString("%1").arg(kappa,10,'f',5,QLatin1Char( ' ' )) +" | "
                        +QString("%1").arg(Rho1-Rho2,20,'f',8,QLatin1Char( ' ' )) +" | "
                        //+QString("%1").arg(scale1/scale2,20,'f',8,QLatin1Char( ' ' ))+" | "
                         +QString("%1").arg(scale1,20,'f',8,QLatin1Char( ' ' ))+" | "
                         +QString("%1").arg(scale2,20,'f',8,QLatin1Char( ' ' ));//
            if(comparePointsSets(x,y,X,Y,Ax,Ay,2,8)==1)
            {
                delete[] ksi;delete[] eta; delete[] M;
                for(int i=0;i<triangle.count();i++)
                {
                    trig = triangle[i];
                    delete[] trig;
                }
                for(int i=0;i<TRIANGLE.count();i++)
                {
                    trig = TRIANGLE[i];
                    delete[] trig;
                }
                delete[] sra;delete[] SRA;
                return 0;
            }
        }//end if (fabs(Rho1-Rho2)>87)
    }//end for i

    delete[] ksi;delete[] eta; delete[] M;
    for(int i=0;i<triangle.count();i++)
    {
        trig = triangle[i];
        delete[] trig;
    }
    for(int i=0;i<TRIANGLE.count();i++)
    {
        trig = TRIANGLE[i];
        delete[] trig;
    }
    delete[] sra;delete[] SRA;
    return 1;
}//end of function
*/
