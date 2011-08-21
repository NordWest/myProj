#include <QtCore>
#include <QCoreApplication>
#include "astro.h"
#include "mb.h"
#include "fitsio.h"

void getAperture(unsigned short *fd, double *const ap, int cx, int cy, int rho, long *naxes)
{
        int ulX = cx-rho;
        int ulY = cy-rho;
        double r;
        for(int y = 0; y<2*rho;y++)
                for(int x = 0; x<2*rho;x++)
                {
                        r = sqrt((x-rho)*(x-rho)+(y-rho)*(y-rho));
                        if((ulX+x>=0)&&(ulX+x<naxes[0])&&(ulY+y>=0)&&(ulY+y<naxes[1])&&(r<=rho))
                                ap[y*2*rho+x] = (double)fd[(ulY+y)*naxes[0]+(ulX+x)];
                        else ap[y*2*rho+x] = -10E-9;
                }
}

bool findStars(unsigned short *fd, long *naxes, QVector<double>& cmX, QVector<double>&cmY, QVector<double>& flux, int rho)
{
    cmX.clear();cmY.clear();flux.clear();
    int bsize = 40;
    int n = naxes[0]/bsize;
    int m = naxes[1]/bsize;
    int flag;
    int N=8;
    int XM[N];
    int YM[N];
    int xm,ym;
    double xmean,ymean,ex,ey,D,r;
    //
    for(int i=0;i<m;i++)
        for(int j=0;j<n;j++)
        {
           for(int k=0;k<N;k++)
            {
               ym = i*bsize;xm =j*bsize;
               for(int y=i*bsize;y<(i+1)*bsize;y++)
                   for(int x=j*bsize;x<(j+1)*bsize;x++)
                       if(fd[x+y*naxes[0]]>fd[xm+ym*naxes[0]])
                        {
                            flag=0; for(int q=0;q<k;q++) if((XM[q]==x)&&(YM[q]==y)) flag=1;
                            if(flag==0){xm=x;ym=y;}
                        }
               XM[k] = xm; YM[k] = ym;
               //cmX<<xm;cmY<<ym;
            }
            xmean = 0; ymean = 0;for(int k=0;k<N;k++){xmean+=XM[k]; ymean+=YM[k];}
            xmean = xmean/N; ymean = ymean/N;
            ex = 0; ey = 0;for(int k=0;k<N;k++){ex+=(XM[k]-xmean)*(XM[k]-xmean); ey+=(YM[k]-ymean)*(YM[k]-ymean);}
            D = sqrt(ex/(N-1)+ey/(N-1));
            if(D<2)
            {
                flag=0;
                for(int k=0;k<cmX.count();k++)
                {
                    r=sqrt((xmean-cmX[k])*(xmean-cmX[k])+(ymean-cmY[k])*(ymean-cmY[k]));
                    if(r<bsize/4){cmX[k]=(cmX[k]+xmean)/2; cmY[k]=(cmY[k]+ymean)/2; flag=1; break;}
                }
                if(flag==0){cmX<<xmean;cmY<<ymean;}
            }
            //for(int k=0;){cmX<<xm;cmY<<ym;}
        }
    //begin estimate flux
    int s;
    int nel = 4*rho*rho;
    double* fda = new double[nel];
    double flu;
    double cmass[2];
    for(int i=0;i<cmX.count();i++)
    {
        getAperture(fd,fda, (int)cmX[i], (int)cmY[i], rho, naxes);
        centerofmass(cmass,fda,rho);
        cmX[i] = (int)cmX[i]+cmass[0]-rho;cmY[i] = (int)cmY[i]+cmass[1]-rho;
        //
        flu = 0;
        s=0;for(int j=0;j<nel;j++) if(fda[j]!=-10E-9){flu+=fda[j];s++;}
        flux << flu/s;
    }
    delete[] fda;
    //end estimate flux*/
    m=0;
    while(m<cmX.count())
    {
       n=0;
        while(n<cmX.count())
        {
            r=sqrt((cmX[m]-cmX[n])*(cmX[m]-cmX[n])+(cmY[m]-cmY[n])*(cmY[m]-cmY[n]));
            if((m!=n)&&(r<bsize/4)){cmX[m]=(cmX[m]+cmX[n])/2; cmY[m]=(cmY[n]+cmY[m])/2;cmX.remove(n);cmY.remove(n);flux.remove(n);}
            else {n++;}
        }
        m++;
    }
    return true;
}

int main(int argc, char *argv[])//matchstars.exe file.fit E:/tmp E:/cats/ucac3 skipWCS maxMag
{
    QTextStream stream(stdout);
    QCoreApplication app(argc, argv);
    ///////////1. Reading fits file //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QString ifname = QString(argv[1]);
    fitsfile *fptr;
    int status = 0;
    int skipWCS = QString(argv[4]).toInt();
    double maxMag = QString(argv[5]).toDouble();
    char strkey[FLEN_CARD];
    fits_open_file(&fptr, argv[1] , READONLY,&status);
    fits_read_key(fptr, TSTRING, "CTYPE1", strkey, NULL, &status);
    if((status==0)&&(skipWCS==1))
    {
        fitsfile *outfptr;
        QFileInfo finfo(ifname);
        QString outfname = '!'+QString(argv[2])+'/'+finfo.completeBaseName()+"_wcs.fit";//name of resulted file is "old file name"+"_c.fit"
        fits_create_file(&outfptr, qPrintable(outfname), &status);
        fits_copy_hdu(fptr,outfptr,0,&status);
        fits_close_file(fptr, &status);
        fits_close_file(outfptr, &status);
        stream << "2 |" << ifname << " | 0 | wcs is exist\n";
        return 0;
    }
    status = 0;
    //BEGIN open fits file
    long naxes[2];
    long felem = 1;
    fits_get_img_size(fptr, 2, naxes, &status);
    LONGLONG nelements = naxes[0]*naxes[1];
    int anynul = 0;
    unsigned short *fd = new unsigned short[nelements];
    fits_read_img(fptr, TUSHORT, felem, nelements, NULL, fd, &anynul, &status);
    //
    //END read fits
    double WCSdata[13];
    //RA and Dec of the center of FOV
    status=0;
    fits_read_key(fptr, TSTRING, "RA", strkey, NULL, &status);status = 0;// read "RA" key
    WCSdata[2] = hms_to_mas(QString(strkey),":")/3600000;//RA in deg
    QString RA(strkey);
    fits_read_key(fptr, TSTRING, "DEC", strkey, NULL, &status);status = 0;// read "DEC" key
    WCSdata[3] = damas_to_mas(QString(strkey),":")/3600000;//DEC in deg
    QString DEC(strkey);
    fits_close_file(fptr, &status);
    ///////////2. loading UCAC3 data
    QProcess ucac3findProcess;
    ucac3findProcess.setWorkingDirectory(QCoreApplication::applicationDirPath());
    ucac3findProcess.setProcessChannelMode(QProcess::MergedChannels);
    ucac3findProcess.setReadChannel(QProcess::StandardOutput);
    ucac3findProcess.start(QCoreApplication::applicationDirPath()+"/ucac3find",
                QStringList::QStringList()<<RA<<DEC<<"b=25x25"<<QString(argv[3]));
    ucac3findProcess.waitForFinished(-1);
    QTextStream ucac3Stream(ucac3findProcess.readAllStandardOutput());
    QVector<double> KSI,ETA,MAG;//arrays of tangential positions of the reference stars
    double ra,dec,mag;
    double tang[2];
    QString u3Line;
    while(!ucac3Stream.atEnd())
    {
        //stream <<ucac3Stream.readLine() <<'\n';
        u3Line = ucac3Stream.readLine();
        ra = hms_to_mas(u3Line.section('|',0,0),":");
        dec = damas_to_mas(u3Line.section('|',1,1),":");
        mag = u3Line.section('|',2,2).toDouble();
        getRaDeToTang(ra,dec,WCSdata[2]*3600000,WCSdata[3]*3600000,tang);
        if((mag>7)&&(mag<maxMag)){KSI << tang[0];ETA << tang[1];MAG<<mag;}//select stars
        /*if((mag>0)&&(mag<maxMag))
        {
            stream << QString("%1").arg( tang[0],10,'f',3,QLatin1Char( ' ' )) <<'|'
                    << QString("%1").arg( tang[1],10,'f',3,QLatin1Char( ' ' )) <<'|'
                    << QString("%1").arg( mag,10,'f',3,QLatin1Char( ' ' ))
                    <<'\n';
            //cmX << tang[0]+500;cmY <<-tang[1]+500;
        }*/
    }
    if(ETA.count()==0){ stream << "0 |" << ifname << " | -3 | there are no stars in catalogue\n";return 1;}
    ///3. finding stars
    QVector<double> cmX,cmY,flux;
    //stream << QString( "%1" ).arg(maxMag,6,'f',1,QLatin1Char( ' ' ))<<QString( "%1" ).arg(KSI.count(),6,'f',1,QLatin1Char( ' ' ))<<'\n';
    //return 0;
    if(!findStars(fd,naxes,cmX,cmY,flux,12)){ stream << "0 |" << ifname << " | -1 |stellar images not found\n";return 1;}
    //stream << QString( "%1" ).arg(cmX.count(),6,'f',1,QLatin1Char( ' ' ))<<QString( "%1" ).arg(flux.count(),6,'f',1,QLatin1Char( ' ' ))<<'\n';

    /*for(int i=0;i<cmX.count();i++)
        stream << QString("%1").arg( cmX[i],10,'f',3,QLatin1Char( ' ' )) <<'|'
                    << QString("%1").arg( cmY[i],10,'f',3,QLatin1Char( ' ' )) <<'|'
                    << QString("%1").arg( flux[i],10,'f',3,QLatin1Char( ' ' ))
                    <<'\n';*/
    //stream <<QString( "%1" ).arg(cmX.count(),6,'f',1,QLatin1Char( ' ' ))<<'\n';
    //return 0;
    //begin sort arrays (by flux and MAG)
    QVector<int> snPix,snTang;
    for(int i=0;i<flux.count();i++)snPix<<-1;
    for(int i=0;i<MAG.count();i++)snTang<<-1;
    int currF;
    for(int i=0;i<snPix.count();i++)
    {
        for(int j=0;j<flux.count();j++)
        {
            currF=0;
            for(int k=0;k<snPix.count();k++) if(j==snPix[k]){currF=1;break;}
            if(!currF)
            {
                if(snPix[i]==-1)
                    snPix[i]=j;
                else
                    if(flux[j]>flux[snPix[i]])snPix[i]=j;
            }
        }
    }
    //
    for(int i=0;i<snTang.count();i++)
    {
        for(int j=0;j<MAG.count();j++)
        {
            currF=0;
            for(int k=0;k<snTang.count();k++) if(j==snTang[k]){currF=1;break;}
            if(!currF)
            {
                if(snTang[i]==-1)
                    snTang[i]=j;
                else
                    if(MAG[j]<MAG[snTang[i]])snTang[i]=j;
            }
        }
    }
     //stream <<QString( "%1" ).arg(KSI.count(),6,'f',1,QLatin1Char( ' ' ))<<'\n';
    double X[cmX.count()];
    double Y[cmX.count()];
    double ksi[KSI.count()];
    double eta[KSI.count()];
    double alpha[KSI.count()];
    double delta[KSI.count()];
    int pixN[cmX.count()];
    int tangN[cmX.count()];
    //stream << QString( "%1" ).arg(ksi.count(),6,'f',1,QLatin1Char( ' ' ))<<QString( "%1" ).arg(flux.count(),6,'f',1,QLatin1Char( ' ' ))<<'\n';
    //return 0;
    for(int i=0;i<snPix.count();i++){X[i]=cmX[snPix[i]];Y[i]=cmY[snPix[i]];}//pixel positions sorted by flux
    //return 0;
    for(int i=0;i<snTang.count();i++){ksi[i]=KSI[snTang[i]];eta[i]=ETA[snTang[i]];}//alpha[i]=ALPHA[snTang[i]];delta[i]=DELTA[snTang[i]];}//tangential positins sorted by MAG
    //end sort arrays
    //return 0;
    //4. matching stars
    //stream <<QString( "%1" ).arg(KSI.count(),6,'f',1,QLatin1Char( ' ' ))<<QString( "%1" ).arg(flux.count(),6,'f',1,QLatin1Char( ' ' ))<<'\n';
    //return 0;
    int k,ns;
    double uwex,uwey;
    int rnx,rny;
    bool isMatched=false;
    for(int i=0;i<3;i++)
    {
        if(i==0){isMatched = matchstars(tangN,ksi,eta,pixN,X,Y, KSI.count(), cmX.count(), 2, 0.01, 0.005,6); if(isMatched)break;}
        if(i==1){isMatched = matchstars(tangN,ksi,eta,pixN,X,Y, KSI.count(), cmX.count(), 2, 0.02, 0.010,5); if(isMatched)break;}
        if(i==2){isMatched = matchstars(tangN,ksi,eta,pixN,X,Y, KSI.count(), cmX.count(), 2, 0.03, 0.015,5); if(isMatched)break;}
    }
    if(isMatched)
    {
        ns=0;
        for(int i=0;i<cmX.count();i++)if(tangN[i]>0)ns++;
        if(ns<4) {stream << "0 |" << ifname << " | -4 | insufficient number of stars for matching\n";return 1;}
         stream << "1 |" << ifname << " | "+QString( "%1" ).arg(ns,6,'f',1,QLatin1Char( ' ' ))<<" | matching is successful\n";
        //return 0;
        double *ksip = new double[ns];
        double *etap = new double[ns];
        double *Cp = new double[ns*3];
        double *W = new double[ns];
        int *exclindx = new int[ns];
        int *exclindy = new int[ns];
        double *zx = new double[3];
        double *zy = new double[3];
        double *Dx = new double[9];
        double *Dy = new double[9];
        //
        /*QFile resFile(QCoreApplication::applicationDirPath()+"/out/stars.txt");
        resFile.open(QIODevice::WriteOnly| QIODevice::Text);
        QTextStream resStream;
        resStream.setDevice(&resFile);
        //*/
        k=0;
        for(int i=0;i<cmX.count();i++)
        {
            if(tangN[i]>0)
            {
                ksip[k] = ksi[tangN[i]]/3600;etap[k] = eta[tangN[i]]/3600;
                Cp[k*3+0]=X[pixN[i]];Cp[k*3+1]=Y[pixN[i]];Cp[k*3+2]=1;
                W[k]=1;
                k++;
                /*resStream << QString( "%1" ).arg(ksi[tangN[i]],8,'f',3,QLatin1Char( ' ' ))<<' '
                        << QString( "%1" ).arg(eta[tangN[i]],8,'f',3,QLatin1Char( ' ' ))<<' '
                        << QString( "%1" ).arg(X[pixN[i]],8,'f',3,QLatin1Char( ' ' ))<<' '
                        << QString( "%1" ).arg(Y[pixN[i]],8,'f',3,QLatin1Char( ' ' ))<<' '
                        <<'\n';*/
            }
        }
        //stream << QString( "%1" ).arg(k,6,'f',1,QLatin1Char( ' ' ))<<'\n';
        //return 0;
        //resFile.close();
        iLSM(3,ns,1,exclindx,zx,Cp,ksip,uwex,Dx,2,rnx,W);
        iLSM(3,ns,1,exclindy,zy,Cp,etap,uwey,Dy,2,rny,W);
        //lsm(3,ns,zx,Cp,ksip,uwex,Dx,W);
        //lsm(3,ns,zy,Cp,etap,uwey,Dy,W);
        getWCS(WCSdata,zx,zy);
        delete[]ksip;delete[]etap;delete[]Cp;delete[]W;delete[]exclindx;delete[]exclindy;delete[]zx;delete[]zy;delete[]Dx;delete[]Dy;
        //bool matchstars(int *tangNum, double *ksi, double *eta, int *pixNum, double *X, double *Y, int Nstang, int Nspix, double err, double eAng, double eRatio)
        //BEGIN Writing fits file
        fitsfile *nfptr;
        QFileInfo fi(ifname);
        QString ofname = '!'+QString(argv[2])+'/'+fi.completeBaseName()+"_wcs.fit";//name of resulted file is "old file name"+"_c.fit"
        fits_create_file(&nfptr, qPrintable(ofname), &status);
        fits_open_file(&fptr, argv[1] , READONLY, &status);
        fits_copy_hdu(fptr,nfptr,0,&status);
        fits_close_file(fptr, &status);
        //
        char key1[]="RA---TAN";fits_update_key(nfptr, TSTRING, "CTYPE1",  key1, " ", &status);
        char key2[]="DEC--TAN";fits_update_key(nfptr, TSTRING, "CTYPE2",  key2, " ", &status);
        QString str;
        str = QString( "%1" ).arg(WCSdata[0],24,'f',18,QLatin1Char( ' ' ));
        fits_update_key(nfptr, TSTRING, "CRPIX1", str.toAscii().data(), "ref pix 1", &status);
        str = QString( "%1" ).arg(WCSdata[1],24,'f',18,QLatin1Char( ' ' ));
        fits_update_key(nfptr, TSTRING, "CRPIX2", str.toAscii().data(), "ref pix 2", &status);
        str = QString( "%1" ).arg(WCSdata[2],24,'f',18,QLatin1Char( ' ' ));
        fits_update_key(nfptr, TSTRING, "CRVAL1", str.toAscii().data(), "RA at ref pix", &status);
        str = QString( "%1" ).arg(WCSdata[3],24,'f',18,QLatin1Char( ' ' ));
        fits_update_key(nfptr, TSTRING, "CRVAL2", str.toAscii().data(), "DEC at ref pix", &status);
        str = QString( "%1" ).arg(WCSdata[4],24,'f',18,QLatin1Char( ' ' ));
        fits_update_key(nfptr, TSTRING, "CDELT1", str.toAscii().data(), "scale1 [deg/pix]", &status);
        str = QString( "%1" ).arg(WCSdata[5],24,'f',18,QLatin1Char( ' ' ));
        fits_update_key(nfptr, TSTRING, "CDELT2", str.toAscii().data(), "scale2 [deg/pix]", &status);
        str = QString( "%1" ).arg(WCSdata[6],24,'f',18,QLatin1Char( ' ' ));
        fits_update_key(nfptr, TSTRING, "CROTA1", str.toAscii().data(), "rotation1 [deg]", &status);
        str = QString( "%1" ).arg(WCSdata[7],24,'f',18,QLatin1Char( ' ' ));
        fits_update_key(nfptr, TSTRING, "CROTA2", str.toAscii().data(), "rotation2 [deg]", &status);
        str = QString( "%1" ).arg(WCSdata[8],24,'f',18,QLatin1Char( ' ' ));
        fits_update_key(nfptr, TSTRING, "CD1_1", str.toAscii().data(), "WCS CD matrix", &status);
        str = QString( "%1" ).arg(WCSdata[9],24,'f',18,QLatin1Char( ' ' ));
        fits_update_key(nfptr, TSTRING, "CD1_2", str.toAscii().data(), "WCS CD matrix", &status);
        str = QString( "%1" ).arg(WCSdata[10],24,'f',18,QLatin1Char( ' ' ));
        fits_update_key(nfptr, TSTRING, "CD2_1", str.toAscii().data(), "WCS CD matrix", &status);
        str = QString( "%1" ).arg(WCSdata[11],24,'f',18,QLatin1Char( ' ' ));
        fits_update_key(nfptr, TSTRING, "CD2_2", str.toAscii().data(), "WCS CD matrix", &status);
        char key3[]="UCAC3";fits_update_key(nfptr, TSTRING, "WCSRFCAT", key3, "wcs catalog", &status);
        char key4[]="deg";fits_update_key(nfptr, TSTRING, "CUNIT1", key4, "unit 1", &status);
        fits_update_key(nfptr, TSTRING, "CUNIT2", key4, "unit 2", &status);
        double Sx = 3600000*sqrt(uwex);str = QString( "%1" ).arg(Sx,24,'f',18,QLatin1Char( ' ' ));
        fits_update_key(nfptr, TSTRING, "WCSUWE1", str.toAscii().data(),  "unit weight error in mas", &status);
        double Sy = 3600000*sqrt(uwey);str = QString( "%1" ).arg(Sy,24,'f',18,QLatin1Char( ' ' ));
        fits_update_key(nfptr, TSTRING, "WCSUWE2", str.toAscii().data(), "unit weight error in mas", &status);
        //
        LONGLONG fpixel = 1;
        //fits_write_img(nfptr, TUSHORT, fpixel, nelements, fd, &status);
        //delete[] fd;
        fits_close_file(nfptr, &status);
        return 0;
        //END writing fits file
    }
    stream << "0 |" << ifname << " | -2 | frame was not matched\n";
    return 1;
}
