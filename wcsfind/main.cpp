#include <QtCore>
#include <QCoreApplication>
#include <ffc.h>

void Photocenters(fitsData* fd, QVector<int>X, QVector<int>Y,
                  QVector<double>& x, QVector<double>& y,
                  QVector<double>& mag, int rho1, int rho2)
{
    double *ap = new double[4*rho1*rho1];
    double *ring = new double[4*rho2*rho2];
    x.clear();y.clear();mag.clear();
    double cx,cy,cJ,mJ,Mag;
    int aN,rN;
    //qDebug() << QString("%1").arg(rho1,10,'f',6,QLatin1Char( ' ' ));
    for(int i=0;i<X.count();i++)
    {
        fd->getAperture(ap, X[i], Y[i], rho1);
        fd->getRing(ring, X[i], Y[i], rho1, rho2);
        cms(ap, rho1, cx, cy, cJ, aN);
        ringSumm(ring, rho2, mJ, rN);
        //cJ=cJ/aN;
        mJ=mJ*aN/rN;
        Mag = 16.5-2.5*log10((cJ-mJ)/aN);
        if(!isnan(Mag))
        {
            mag<<Mag;
            x<<X[i]-rho1+cx; y<<Y[i]-rho1+cy;
            /*qDebug() << QString("%1").arg(X[i]-rho1+cx,14,'f',6,QLatin1Char( ' ' ))+" | "
            +QString("%1").arg(Y[i]-rho1+cy,14,'f',6,QLatin1Char( ' ' ))+" | "
            +QString("%1").arg(Mag,14,'f',6,QLatin1Char( ' ' ));*/
        }
    }
    delete[] ap;
    delete[] ring;
}

int main(int argc, char *argv[])
{
    QString fname(argv[1]);
    QString catfolder(argv[2]);
    QString savefolder(argv[3]);
    QString sflag(argv[4]);
    int flag = sflag.toInt();//0: use ucac3 to determine WCS
    int saveflag = QString(argv[5]).toInt();//
    int Nmin = QString(argv[6]).toInt();//Nmin - minimal number of stars that must to be matched
    int Nmax = QString(argv[7]).toInt();//Nmax - maximal number of stars to analysis
    double prec = QString(argv[8]).toDouble();//presision in arcsec
    QString newfname(argv[9]);
    /////////////////////////////
    fitsData *fd = new fitsData(fname);
    wcsData  *wd = new wcsData(fname);
    //////////////////////
    //QTextStream stream(stdout);
    //stream<<fd->filename()<<endl;
    QVector<int>X,Y;
    findStars(fd,X,Y);
    QVector<double> cmX,cmY,Flux;
    Photocenters(fd,X,Y,cmX,cmY,Flux,5,8);
    ////////////////
    QVector<int>NC;
    sortVector(Flux,NC);
    QVector<double>MX,MY;
    for(int i=0;i<NC.count();i++)
    {
        MX<<cmX[NC[i]];MY<<cmY[NC[i]];
    }
    ///////////////////////////
    QVector<double>x,y,mag;
    if(flag==0)
    {
        QString ucac3str = ucac3find(M_PI*wd->refpixRA()/180,M_PI*wd->refpixDE()/180,25,25,1,catfolder);
        tangFromUCAC3(ucac3str,x,y,mag,
                      M_PI*wd->refpixRA()/180,
                      M_PI*wd->refpixDE()/180,
                      wd->year(),8,17);
    }
    else
    {
        delete fd; delete wd;return 1;
    }
    ///////////////////////////
    QVector<int>nc;
    sortVector(mag,nc);
    QVector<double>mx,my;
    for(int i=0;i<nc.count();i++)
    {
        mx<<x[nc[i]];my<<y[nc[i]];
    }
    ///////////////////////////
    double* Ax=new double[3];
    double* Ay=new double[3];
    if(matchStars(MX,MY,mx,my,Ax,Ay,prec/3600.0,Nmin,Nmax)==0)
    {
        wd->setWCS(Ax,Ay);
        wd->saveWCS(savefolder,saveflag,newfname);
    }
    delete[] Ax;delete[] Ay;
    //////////////////////
    delete fd; delete wd;
    return 0;
}
