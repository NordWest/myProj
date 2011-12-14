#include <QtCore>
#include <QCoreApplication>
#include "./../astro/astro.h"
#include "./../mb/mb.h"
#include "./../libs/fitsio.h"

unsigned short freqpix(unsigned short *fdata, LONGLONG nelements)
//determination of frequest pixel value
{
    unsigned short maxv = fdata[0];
    unsigned short minv = fdata[0];
    for (quint32 k=0;k<nelements;k++)
    {
        if (fdata[k]>maxv)maxv=fdata[k];
        if (fdata[k]<minv)minv=fdata[k];
    }
    int hsize = maxv-minv+1;
    long *pf = new long[hsize];
    for (int i=0;i<hsize;i++) pf[i]=0;
    for (quint32 k=0;k<nelements;k++) pf[fdata[k]-minv]++;
    int fpn=0;
    for (int i=0;i<hsize;i++) if (pf[i]>pf[fpn])fpn=i;
    fpn = fpn + minv;
    delete[] pf;
    return fpn;
}

void getbox(const double* fd, long *nx, double *const b, int X,int Y,int bsize)
{
    int k=0;
    for(int y=Y-bsize;y<=Y+bsize;y++)
        for(int x=X-bsize;x<=X+bsize;x++)
            {
                if((x>=0)&&(x<nx[0])&&(y>=0)&&(y<nx[1]))
                    b[k]=fd[x+y*nx[0]];
                else
                    b[k]= -10E-9;
                k++;
            }
}

void approxLine(double* ld, double*const lz, double& uwe, int fo, int length, double flag)
{
    double C[length*fo];
    double W[length];
    double D[fo*fo];
    int rn;
    int ce[length];
    for(int x=0;x<length;x++)
    {
        W[x]=1;
        for(int k=0;k<fo;k++) C[k+x*fo] = pow(x,k);
    }
    iLSM(fo,length,10,ce,lz,C,ld,uwe,D,flag,rn,W);
}

double surface(double* b, int bsize)
{
    int nel = (2*bsize+1)*(2*bsize+1);
    int xsize = 2*bsize+1;
    int N=0;for (int i=0;i<nel;i++) if(b[i]!=-10E-9)N++;
    double *J = new double[N];
    double *C = new double[N*6];
    double *W = new double[N];
    double *Z = new double[6];
    int k=0;
    for(int y=0;y<xsize;y++)
        for(int x=0;x<xsize;x++)
        {
            if(b[x+y*xsize]!=-10E-9)
            {
                J[k]=b[x+y*xsize];W[k]=1;
                C[0+k*6]=1;C[1+k*6]=x;C[2+k*6]=y;C[3+k*6]=x*x;C[4+k*6]=x*y;C[5+k*6]=y*y;
                k++;
            }
        }
    slsm(6,N,Z,C,J,W);
    double res = Z[0]+Z[1]*bsize+Z[2]*bsize+Z[3]*bsize*bsize+Z[4]*bsize*bsize+Z[5]*bsize*bsize;
    delete[]J;delete[]C;delete[]W;delete[]Z;
    return res;
}


int main(int argc, char *argv[])//twdb iniFile resFolder
{
    //QTextStream stream(stdout);
    QCoreApplication a(argc, argv);
    ///////////1. Reading fits file //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QString ifname = QString(argv[1]);
    QString lockFileName = QString("%1.lock").arg(ifname);

    QString filePath;
    QFileInfo fi(ifname);
    /*if(ifname.lastIndexOf("/")==-1) filePath = ifname.left(ifname.lastIndexOf("\\")+1);
    else filePath = ifname.left(ifname.lastIndexOf("/")+1);
    if(filePath=="") filePath = QString("./");*/
    filePath = fi.absolutePath();
    qDebug() << QString("fileName: %1\n").arg(ifname);
    qDebug() << QString("filePath: %1\n").arg(filePath);

    QString resPath = QString(argv[2]);
    QDir().mkpath(resPath);
    QDir resDir(resPath);

    QString ofname = resPath+'/'+fi.completeBaseName()+".fit";//name of resulted file is "old file name"+"_f.fit"
    if(resDir.exists(ofname))
    {
        qDebug() << QString("res file %1 exists\n").arg(ofname);
        return 0;
    }

    ofname.prepend("!");


    QDir wDir(filePath);
    if(wDir.exists(lockFileName))
    {
        qDebug() << QString("file %1 locked\n").arg(ifname);

        //qInstallMsgHandler(0);
        return 0;
    }

    QFile lockFile(lockFileName);
    lockFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    lockFile.close();


    fitsfile *fptr;
    int status = 0;
    fits_open_file(&fptr, argv[1] , READONLY,&status);
    //BEGIN open fits file
    long naxes[2];
    long felem = 1;
    fits_get_img_size(fptr, 2, naxes, &status);
    LONGLONG nelements = naxes[0]*naxes[1];
    int anynul = 0;
    unsigned short *fd = new unsigned short[nelements];
    fits_read_img(fptr, TUSHORT, felem, nelements, NULL, fd, &anynul, &status);
    //END read fits
    unsigned short fpix = freqpix(fd,nelements);
    //BEGIN filtering
    double *fD = new double[nelements];
    int fo=4;
    double *lineZ = new double[fo];
    double *lineD = new double[naxes[0]];
    double *lineC = new double[naxes[0]*fo];
    double *allZ = new double[naxes[1]*fo];
    double uwe;
    double esignal;
    for(int y=0;y<naxes[1];y++)
    {
        for(int x=0;x<naxes[0];x++){lineD[x]=fd[x+y*naxes[0]];for(int k=0;k<fo;k++) lineC[k+x*fo]=pow(x,k);}
        approxLine(lineD,lineZ,uwe,fo,naxes[0],3);
        for(int k=0;k<fo;k++) allZ[k+y*fo] = lineZ[k];
        for(int x=0;x<naxes[0];x++)
        {
            esignal=0;for(int k=0;k<fo;k++) esignal+=lineZ[k]*pow(x,k);
            esignal = (double)fd[x+y*naxes[0]] - esignal;
            if(esignal<sqrt(uwe)) fD[x+y*naxes[0]] = esignal; else fD[x+y*naxes[0]] = 0;
        }
    }
    delete[]lineZ;delete[]lineD;delete[]lineC;
    //return 0;
    int msize = QString(argv[3]).toInt();
    int fl = QString(argv[4]).toInt();
    double *bx = new double[(2*msize+1)*(2*msize+1)];
    //for(int i=0;i<(2*msize+1)*(2*msize+1);i++)bx[i]=i;
    double me;
    for(int y=0;y<naxes[1];y++)
        for(int x=0;x<naxes[0];x++)
        {
            getbox(fD,naxes,bx,x,y,msize);
            me = surface(bx,msize);
            esignal=0;for(int k=0;k<fo;k++) esignal+=allZ[k+y*fo]*pow(x,k);
            if(fl==0)fd[x+y*naxes[0]]=(unsigned short)(fd[x+y*naxes[0]]*fpix/(esignal+me));
            if(fl==1)fd[x+y*naxes[0]]=fd[x+y*naxes[0]]-(unsigned short)(esignal+me)+fpix;
            if(fl==2)fd[x+y*naxes[0]]=(unsigned short)(esignal+me)+fpix;
        }
    delete[] bx;
    delete[] allZ;
    //return 0;
    //BEGIN Writing fits file
    fitsfile *nfptr;
    //QFileInfo fi(ifname);
    //QString ofname = '!'+QString(argv[2])+'/'+fi.completeBaseName()+".fit";//name of resulted file is "old file name"+"_f.fit"
    fits_create_file(&nfptr, qPrintable(ofname), &status);
    fits_open_file(&fptr, argv[1] , READONLY, &status);
    fits_copy_hdu(fptr,nfptr,0,&status);
    fits_close_file(fptr, &status);
    LONGLONG fpixel = 1;
    //fits_write_img(nfptr, TUSHORT, fpixel, nelements, outfd, &status);
    fits_write_img(nfptr, TUSHORT, fpixel, nelements, fd, &status);
    delete[] fd;
    //delete[] outfd;
    delete[] fD;
    fits_close_file(nfptr, &status);
    //END writing fits file

    QDir(filePath).remove(lockFileName);


    return 0;
}
