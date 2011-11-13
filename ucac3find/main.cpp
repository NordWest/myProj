#include <QtCore>
#include <QCoreApplication>
#include "./../astro/astro.h"

int main(int argc, char *argv[])
{//ucac3find hh:mm:ss +dd:am:as r=10 ucac3path
    //ucac3find hh:mm:ss +dd:am:as b=10x20 ucac3path
    //ucac3find hh:mm:ss +dd:am:as g=10x20 ucac3path
    QTextStream stream(stdout);
    //stream << "begin\n";
    double ra, dec,rhox,rhoy;
    QString RA = QString(argv[1]);
    QString DEC = QString(argv[2]);
    ra =  hms_to_mas(RA,":");
    dec = damas_to_mas(DEC,":");
    QString RHO = QString(argv[3]);
    //type of work
    QString stw = RHO.section('=',0,0);
    QString rxy = RHO.section('=',1,1);
    int tw;
    if(stw=="r") {rhox = rxy.toDouble()*60000;rhoy=rhox;tw=0;}//within radius
    if(stw=="b") {rhox = rxy.section('x',0,0).toDouble()*60000;rhoy = rxy.section('x',1,1).toDouble()*60000;tw=1;}//within box
    if(stw=="g") {rhox = rxy.section('x',0,0).toDouble()*60000;rhoy = rxy.section('x',1,1).toDouble()*60000;tw=2;}//within geodetic box
    //

    //stream << QString("ra: %1\tde: %2\n").arg(mas_to_hms(ra, ":", 4)).arg(mas_to_damas(dec, ":", 3));
   // stream << QString("%1").arg(tw,3,10,QLatin1Char( ' ' )) <<'\n'
  //  << QString("%1").arg(rhoy/60000,5,'f',2,QLatin1Char( ' ' )) <<'\n';
    QString ucac3path = QString(argv[4]);
    ///////////////////
    double decmin = dec-rhoy;
    if(decmin<-324000000) decmin = -324000000;
    double decmax = dec+rhoy;
    if(decmax>324000000) decmax = 324000000;

    //stream << QString("decmin: %1\tdecmax: %2\n").arg(mas_to_damas(decmin, ":", 3)).arg(mas_to_damas(decmax, ":", 3));
    //////////////////
    double dra;
    if(tw==2)
    {
        dra = rhox;
    }
    else
    {
        if((decmax<324000000)&&(decmin>-324000000))
            dra = rad_to_mas(asin(sin(mas_to_rad(rhox))/cos(mas_to_rad(dec))));
        else
            dra = 0;
    }
    //qDebug() << QString("dra= %1\n").arg(dra);
    double ramin = ra - dra;
    if(ramin<0) ramin = ramin+ 1296000000;
    double ramax = ra + dra;
    if(ramax>=1296000000) ramax = ramax - 1296000000;
    if(dra==0){ramin=0;ramax=1295999999;}
    //stream << QString("ramin: %1\tramax: %2\n").arg(mas_to_hms(ramin, ":", 3)).arg(mas_to_hms(ramax, ":", 3));
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
      stream <<"can't find u3index.unf \n";
      return 1;
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
    double tp[2];
    double val, dval;
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

        DECMIN= 180+(int)floor(decmin/1800000.0);
        DECMAX =180+(int)floor(decmax/1800000.0);
        //DECMIN= 180+(int)(decmin/1800000);
        //DECMAX =180+(int)(decmax/1800000);
        //stream << QString("DECMIN = %1\tDECMAX = %2\n").arg(DECMIN).arg(DECMAX);
        if(DECMAX>359) DECMAX--;
        for(int z=DECMIN;z<=DECMAX;z++)
        {
            u3fname = ucac3path + "/z"+QString( "%1" ).arg(z+1,3,10,QLatin1Char( '0' ));
            u3file.setFileName(u3fname);
            //stream << u3fname <<'\n';
            if (!u3file.open(QIODevice::ReadOnly)){stream << u3fname <<'\n'; return 1;}
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
                //stream << QString("ra: %1\tde: %2\n").arg(mas_to_hms(Ra, ":", 4)).arg(mas_to_damas(spd, ":", 3));
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
                    getDegToTang(&tp[0], &tp[1], mas_to_grad((double) Ra), mas_to_grad((double)spd), mas_to_grad(ra), mas_to_grad(dec));
                    //stream << QString("ra: %1\tde: %2\n").arg(mas_to_hms(Ra, ":", 4)).arg(mas_to_damas(spd, ":", 3));
                    //stream << QString("rac: %1\tdec: %2\n").arg(mas_to_hms(ra, ":", 4)).arg(mas_to_damas(dec, ":", 3));
                    //stream << QString("dr= %1\n").arg(grad_to_mas(sqrt(tp[0]*tp[0]+tp[1]*tp[1]))/60000.0);
                    outs = 0;
                    if((tw==0)&&(grad_to_mas(sqrt(tp[0]*tp[0]+tp[1]*tp[1]))<=rhox)) outs=1;
                    if((tw==1)&&(grad_to_mas(fabs(tp[0]))<=rhox)&&(grad_to_mas(fabs(tp[1]))<=rhoy)) outs=1;
                    //if(sqrt(tp[0]*tp[0]+tp[1]*tp[1])/60<=rhox/60000) stream << QString("%1").arg(sqrt(tp[0]*tp[0]+tp[1]*tp[1])/60,10,'f',2,QLatin1Char( ' ' )) <<'|'<<
                    //       QString("%1").arg(rhox/60000,10,'f',2,QLatin1Char( ' ' )) <<'|'<< QString("%1").arg(tw,3,10,QLatin1Char( ' ' ))<<'\n';
                }
                if(outs)
                {

                    stream << mas_to_hms(Ra, ":", 4)<<'|'                                                 //0
                            <<mas_to_damas(spd, ":", 3)<<'|'                                              //1
                            << QString("%1").arg((double)im1/1000,6,'f',3,QLatin1Char( ' ' )) <<'|'       //2
                            << QString("%1").arg((double)im2/1000,6,'f',3,QLatin1Char( ' ' )) <<'|'       //3
                            << QString("%1").arg((double)sigmag/1000,6,'f',3,QLatin1Char( ' ' )) <<'|'    //4
                            << QString("%1").arg(objt,3,10,QLatin1Char( ' ' )) <<'|'                      //5
                            << QString("%1").arg(dsf,3,10,QLatin1Char( ' ' )) <<'|'                       //6
                            << QString("%1").arg(sigra,3,10,QLatin1Char( ' ' )) <<'|'                     //7
                            << QString("%1").arg(sigdc,3,10,QLatin1Char( ' ' )) <<'|'                     //8
                            << QString("%1").arg(na1,3,10,QLatin1Char( ' ' )) <<'|'                       //9
                            << QString("%1").arg(nu1,3,10,QLatin1Char( ' ' )) <<'|'                      //10
                            << QString("%1").arg(us1,3,10,QLatin1Char( ' ' )) <<'|'                      //11
                            << QString("%1").arg(cn1,3,10,QLatin1Char( ' ' )) <<'|'                      //12
                            << QString("%1").arg(1900+(double)cepra/100,7,'f',2,QLatin1Char( ' ' )) <<'|'//13
                            << QString("%1").arg(1900+(double)cepdc/100,7,'f',2,QLatin1Char( ' ' )) <<'|'//14
                            << QString("%1").arg((double)pmrac/10,7,'f',1,QLatin1Char( ' ' )) <<'|'      //15
                            << QString("%1").arg((double)pmdc/10,7,'f',1,QLatin1Char( ' ' )) <<'|'       //16
                            << QString("%1").arg((double)sigpmr/10,5,'f',1,QLatin1Char( ' ' )) <<'|'     //17
                            << QString("%1").arg((double)sigpmd/10,5,'f',1,QLatin1Char( ' ' )) <<'|'     //18
                            << QString("%1").arg(id2m,10,10,QLatin1Char( ' ' )) <<'|'                    //19
                            << QString("%1").arg((double)jmag/1000,6,'f',3,QLatin1Char( ' ' )) <<'|'     //20
                            << QString("%1").arg((double)hmag/1000,6,'f',3,QLatin1Char( ' ' )) <<'|'     //21
                            << QString("%1").arg((double)kmag/1000,6,'f',3,QLatin1Char( ' ' )) <<'|';    //22
                    for(int k=0;k<3;k++) stream << QString("%1").arg(icqflg[k],2,10,QLatin1Char( '0' )); //23
                    stream <<'|'
                           << QString("%1").arg((double)e2mpho[0]/100,5,'f',2,QLatin1Char( ' ' )) <<'|'  //24
                           << QString("%1").arg((double)e2mpho[1]/100,5,'f',2,QLatin1Char( ' ' )) <<'|'  //25
                           << QString("%1").arg((double)e2mpho[2]/100,5,'f',2,QLatin1Char( ' ' )) <<'|'  //26
                           << QString("%1").arg((double)smB/1000,6,'f',3,QLatin1Char( ' ' )) <<'|'       //27
                           << QString("%1").arg((double)smR2/1000,6,'f',3,QLatin1Char( ' ' )) <<'|'      //28
                           << QString("%1").arg((double)smI/1000,6,'f',3,QLatin1Char( ' ' )) <<'|'       //29
                           << QString("%1").arg(clbl,3,10,QLatin1Char( ' ' )) <<'|'                      //30
                           << QString("%1").arg(qfB,3,10,QLatin1Char( ' ' )) <<'|'                       //31
                           << QString("%1").arg(qfR2,3,10,QLatin1Char( ' ' )) <<'|'                      //32
                           << QString("%1").arg(qfI,3,10,QLatin1Char( ' ' )) <<'|';                      //33
                    for(int k=0;k<10;k++) stream << QString("%1").arg(catflg[k],1,10,QLatin1Char( ' ' ));//34
                    stream <<'|'
                           << QString("%1").arg(c1,3,10,QLatin1Char( ' ' )) <<'|'                        //35
                           << QString("%1").arg(leda,3,10,QLatin1Char( ' ' )) <<'|'                      //36
                           << QString("%1").arg(x2m,3,10,QLatin1Char( ' ' )) <<'|'                       //37
                           << QString("%1").arg(rn,9,10,QLatin1Char( ' ' )) <<'|'                        //38
                            <<'\n';
                }
            }
            //stream << '\n';
        }
    }
    return 0;
}
/*
  num item   fmt unit        explanation                            remark
------------------------------------------------------------------------
 1  ra     I*4 mas         right ascension at  epoch J2000.0 (ICRS)  (1)
 2  spd    I*4 mas         south pole distance epoch J2000.0 (ICRS)  (1)
 3  im1    I*2 millimag    UCAC fit model magnitude                  (2)
 4  im2    I*2 millimag    UCAC aperture  magnitude                  (2)
 5  sigmag I*2 millimag    UCAC error on magnitude (larger of sc.mod)(3)
 6  objt   I*1             object type                               (4)
 7  dsf    I*1             double star flag                          (5)
         16
 8  sigra  I*2 mas         s.e. at central epoch in RA (*cos Dec)
 9  sigdc  I*2 mas         s.e. at central epoch in Dec
10  na1    I*1             total # of CCD images of this star
11  nu1    I*1             # of CCD images used for this star        (6)
12  us1    I*1             # catalogs (epochs) used for proper motions
13  cn1    I*1             total numb. catalogs (epochs) initial match
          8
14  cepra  I*2 0.01 yr     central epoch for mean RA, minus 1900
15  cepdc  I*2 0.01 yr     central epoch for mean Dec,minus 1900
16  pmrac  I*4 0.1 mas/yr  proper motion in RA*cos(Dec)
17  pmdc   I*4 0.1 mas/yr  proper motion in Dec
18  sigpmr I*2 0.1 mas/yr  s.e. of pmRA * cos Dec
19  sigpmd I*2 0.1 mas/yr  s.e. of pmDec
         16
20  id2m   I*4             2MASS pts_key star identifier
21  jmag   I*2 millimag    2MASS J  magnitude
22  hmag   I*2 millimag    2MASS H  magnitude
23  kmag   I*2 millimag    2MASS K_s magnitude
24  icqflg I*1 * 3         2MASS cc_flg*10 + phot.qual.flag          (7)
25  e2mpho I*1 * 3         2MASS error photom. (1/100 mag)           (8)
         16
26  smB    I*2 millimag    SuperCosmos Bmag
27  smR2   I*2 millimag    SC R2mag                                  (9)
28  smI    I*2 millimag    SC Imag
29  clbl   I*1             SC star/galaxy classif./quality flag     (10)
30  qfB    I*1             SC quality flag Bmag                     (11)
31  qfR2   I*1             SC quality flag R2mag                    (11)
32  qfI    I*1             SC quality flag Imag                     (11)
         10
33  catflg I*1 * 10        mmf flag for 10 major catalogs matched   (12)
34  g1     I*1             Yale SPM object type (g-flag)            (13)
35  c1     I*1             Yale SPM input cat.  (c-flag)            (14)
36  leda   I*1             LEDA galaxy match flag                   (15)
37  x2m    I*1             2MASS extend.source flag                 (16)
38  rn     I*4             MPOS star number; identifies HPM stars   (17)
  */
