#include <QtCore>
#include <QCoreApplication>

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


int main(int argc, char *argv[])
{
    QTextStream stream(stdout);
    //stream << "ok"<<endl;
    QString sRA(argv[1]);
    QString sDE(argv[2]);
    QString RHO(argv[3]);
    QString ucac4folder(argv[4]);
    //qDebug()<<sRA<<sDE<<RHO<<ucac4folder;

    ///////////////////////////////
    double A = HMStoRA(sRA,":");//return RA of center in radians
    double D = DAMAStoDE(sDE,":");//return Dec of center in radians
    double ra, dec,rhox,rhoy,dmas;
    //type of work. RHO = "r=10", or RHO = "b=10x20", or RHO = "g=10x20". All sizes in armins.
    QString stw = RHO.section('=',0,0);
    QString rxy = RHO.section('=',1,1);
    //qDebug()<<stw<<rxy;

    int tw;//flag for type of selection
    //within radius
    if(stw=="r") {rhox = rxy.section('x',0,0).toDouble()*60000;rhoy=rhox;tw=0;}
    //within box
    if(stw=="b") {rhox = rxy.section('x',0,0).toDouble()*30000;rhoy = rxy.section('x',1,1).toDouble()*30000;tw=1;}
    //within geodetic box
    if(stw=="g") {rhox = rxy.section('x',0,0).toDouble()*30000;rhoy = rxy.section('x',1,1).toDouble()*30000;tw=2;}
    //////////////////
    //determination of limits of coordinates

    ra = A*648000000/M_PI;
    dec = D*648000000/M_PI;
    double decmin = dec-rhoy;
    if(decmin<-324000000) decmin = -324000000;
    double decmax = dec+rhoy;
    if(decmax>324000000) decmax = 324000000;
    //////////////////
    double dra;
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
    ///////////////////////////////
    //return 0;
    //loading u4index.unf

    QFile unffile(ucac4folder + "/u4index.unf");
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
    double a,d;//current equatorial in radians
    double ksi,eta;//tangential coordinates
    QString u3str;
    quint64 ns;

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
        /*stream <<
        QString("%1").arg(RAMIN,10,10,QLatin1Char( ' ' ))                      +"|"+
        QString("%1").arg(RAMAX,10,10,QLatin1Char( ' ' ))                      +"|"+
        QString("%1").arg(DECMIN,10,10,QLatin1Char( ' ' ))                      +"|"+
        QString("%1").arg(DECMAX,10,10,QLatin1Char( ' ' ))<<endl;*/
        for(int z=DECMIN;z<=DECMAX;z++)
        {
            u4fname = ucac4folder + "/z"+QString( "%1" ).arg(z+1,3,10,QLatin1Char( '0' ));
            u4file.setFileName(u4fname);
            //////////////////////////
            if (!u4file.open(QIODevice::ReadOnly)){stream << u4fname <<endl; return 1;}
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
                //spd = spd-324000000;
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
                    u3str = RAtoHMS(a,":",4)                                                    +"|"+  //0 ra
                            DEtoDAMAS(d,":",3)                                                  +"|"+  //1 dec
                            QString("%1").arg((double)magm/1000.0,6,'f',3,QLatin1Char( ' ' ))   +"|"+  //2
                            QString("%1").arg((double)maga/1000.0,6,'f',3,QLatin1Char( ' ' ))   +"|"+  //3
                            QString("%1").arg((double)sigmag/100.0,6,'f',2,QLatin1Char( ' ' ))   +"|"+  //4
                            QString("%1").arg(objt,3,10,QLatin1Char( ' ' ))                      +"|"+  //5
                            QString("%1").arg(cdf,3,10,QLatin1Char( ' ' ))                      +"|"+  //6
                            QString("%1").arg(128.0+sigra,4,'f',0,QLatin1Char( ' ' ))           +"|"+  //7
                            QString("%1").arg(128.0+sigdc,4,'f',0,QLatin1Char( ' ' ))          +"|"+  //8
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
                            QString("%1").arg(rnz,10,10,QLatin1Char( ' ' ))                        //44
                            ;
                    stream << u3str << endl;

                }
            }
            //stream << '\n';
        }//end of z
    }//end of q

    ////////////////////////////////////////////////
    return 0;
}

/*
col byte item   fmt unit       explanation                            notes
---------------------------------------------------------------------------
 1  1- 3 ra     I*4 mas        right ascension at  epoch J2000.0 (ICRS) (1)
 2  5- 8 spd    I*4 mas        south pole distance epoch J2000.0 (ICRS) (1)
 3  9-10 magm   I*2 millimag   UCAC fit model magnitude                 (2)
 4 11-12 maga   I*2 millimag   UCAC aperture  magnitude                 (2)
 5 13    sigmag I*1 1/100 mag  error of UCAC magnitude                  (3)
 6 14    objt   I*1            object type                              (4)
 7 15    cdf    I*1            combined double star flag                (5)
         15 bytes
 8 16    sigra  I*1 mas        s.e. at central epoch in RA (*cos Dec)   (6)
 9 17    sigdc  I*1 mas        s.e. at central epoch in Dec             (6)
10 18    na1    I*1            total # of CCD images of this star
11 19    nu1    I*1            # of CCD images used for this star       (7)
12 20    cu1    I*1            # catalogs (epochs) used for proper motions
          5 bytes
13 21-22 cepra  I*2 0.01 yr    central epoch for mean RA, minus 1900
14 23-24 cepdc  I*2 0.01 yr    central epoch for mean Dec,minus 1900
15 25-26 pmrac  I*2 0.1 mas/yr proper motion in RA*cos(Dec)             (8)
16 27-28 pmdc   I*2 0.1 mas/yr proper motion in Dec
17 29    sigpmr I*1 0.1 mas/yr s.e. of pmRA * cos Dec                   (9)
18 30    sigpmd I*1 0.1 mas/yr s.e. of pmDec                            (9)
         10 bytes
19 31-34 pts_key I*4           2MASS unique star identifier            (10)
20 35-36 j_m    I*2 millimag   2MASS J  magnitude
21 37-38 h_m    I*2 millimag   2MASS H  magnitude
22 39-40 k_m    I*2 millimag   2MASS K_s magnitude
23 41    icqflg I*1            2MASS cc_flg*10 + ph_qual flag for J    (11)
24 42     (2)   I*1            2MASS cc_flg*10 + ph_qual flag for H    (11)
25 43     (3)   I*1            2MASS cc_flg*10 + ph_qual flag for K_s  (11)
26 44    e2mpho I*1 1/100 mag  error 2MASS J   magnitude               (12)
27 45     (2)   I*1 1/100 mag  error 2MASS H   magnitude               (12)
28 46     (3)   I*1 1/100 mag  error 2MASS K_s magnitude               (12)
         16 bytes
29 47-48 apasm  I*2 millimag   B magnitude from APASS                  (13)
30 49-50  (2)   I*2 millimag   V magnitude from APASS                  (13)
31 51-52  (3)   I*2 millimag   g magnitude from APASS                  (13)
32 53-54  (4)   I*2 millimag   r magnitude from APASS                  (13)
33 55-56  (5)   I*2 millimag   i magnitude from APASS                  (13)
34 57    apase  I*1 1/100 mag  error of B magnitude from APASS         (14)
35 58     (2)   I*1 1/100 mag  error of V magnitude from APASS         (14)
36 59     (3)   I*1 1/100 mag  error of g magnitude from APASS         (14)
37 60     (4)   I*1 1/100 mag  error of r magnitude from APASS         (14)
38 61     (5)   I*1 1/100 mag  error of i magnitude from APASS         (14)
39 62    gcflg  I*1            Yale SPM g-flag*10  c-flag              (15)
         16 bytes
40 63-66 icf(1) I*4            FK6-Hipparcos-Tycho source flag         (16)
41       icf(2) ..             AC2000       catalog match flag         (17)
42       icf(3) ..             AGK2 Bonn    catalog match flag         (17)
43       icf(4) ..             AKG2 Hamburg catalog match flag         (17)
44       icf(5) ..             Zone Astrog. catalog match flag         (17)
45       icf(6) ..             Black Birch  catalog match flag         (17)
46       icf(7) ..             Lick Astrog. catalog match flag         (17)
47       icf(8) ..             NPM  Lick    catalog match flag         (17)
48       icf(9) ..             SPM  YSJ1    catalog match flag         (17)
          4 bytes
49 67    leda   I*1            LEDA galaxy match flag                  (18)
50 68    x2m    I*1            2MASS extend.source flag                (19)
51 69-72 rnm    I*4            unique star identification number       (20)
52 73-74 zn2    I*2            zone number of UCAC2 (0 = no match)     (21)
53 75-78 rn2    I*4            running record number along UCAC2 zone  (21)
         12 bytes
---------------------------------------------------------------------------
         78 = total number of bytes per star record

*/
