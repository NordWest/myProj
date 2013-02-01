#include <QtCore>
#include <QCoreApplication>
#include <math.h>

#include <mgl2/mgl.h>
//#include <mgl/mgl_zb.h>

void loadDataFile(QString filename, QStringList& outList)
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QTextStream fstream(&file);
    while(!fstream.atEnd()) outList << fstream.readLine();
    file.close();
}

int main(int argc, char *argv[])
{//aitoffdens pixdata12.50.dat 32 0 1 2 0 wk sp12.50.eps - example of the command line
    //QCoreApplication a(argc, argv);
    QTextStream stream(stdout);
    QString fname(argv[1]);
    QString epsfname(argv[9]);
    int i;
    QStringList sl;
    loadDataFile(fname,sl);
    int npixX = QString(argv[2]).toInt();//number of pixels along vertical side
    int npixY = QString(argv[3]).toInt();//number of pixels along vertical side
    int cl = QString(argv[4]).toInt();//longitude
    int cb = QString(argv[5]).toInt();//latitude
    int cm = QString(argv[6]).toInt();//mag
    int fl = QString(argv[7]).toInt();//flag for separator of columns
    QString clims(argv[10]);
    /////////////////////////////

    qDebug() << QString("%1 -> %2\n").arg(fname).arg(epsfname);

    QVector<double>X,Y,M;
    double L,B,MAG;
    QString line;
    int lNum = sl.count();
    //qDebug() << QString("line num: %1\n").arg(lNum);
    if((npixX*npixY)!=lNum)
    {
        qDebug() << QString("Wrong dim\n");
        return 1;
    }
    for(i=0;i<lNum;i++)
    {
        line = sl[i].simplified();
        /*if(fl==0)
        {
            L = M_PI*line.section('|',cl,cl).toDouble()/180-M_PI;
            B = -M_PI*line.section('|',cb,cb).toDouble()/180;
            MAG = line.section('|',cm,cm).toDouble();
        }
        else
        {
            L = M_PI*line.section(' ',cl,cl).toDouble()/180-M_PI;
            B = -M_PI*line.section(' ',cb,cb).toDouble()/180;
            MAG = line.section(' ',cm,cm).toDouble();
        }*/

        if(fl==0)
        {
            L = line.section('|',cl,cl).toDouble();
            B = line.section('|',cb,cb).toDouble();
            MAG = line.section('|',cm,cm).toDouble();
        }
        else
        {
            L = line.section(' ',cl,cl).toDouble();
            B = line.section(' ',cb,cb).toDouble();
            MAG = line.section(' ',cm,cm).toDouble();
        }

        X<<L;
        Y<< B;
        M<<MAG;
    }
    //////////////////////////////
    double maxMag = M[0];
    double minMag = M[0];
    qDebug() << QString("M.num: %1\n").arg(M.count());

    if(clims.contains("yes"))
    {
        minMag=clims.section('|',1,1).toDouble();
        maxMag=clims.section('|',2,2).toDouble();
    }
    else
    {
        for(int i=0;i<M.count();i++)
        {
            if(M[i]>maxMag)maxMag=M[i];
            if(M[i]<minMag)minMag=M[i];
        }
    }
    //////////////////////////////
    double cx,cy,psi,d,ed;
    int x, y;
    mglData Z(npixX+1,npixY+1);
    //Z.SetVal(M.at(i), f(i/npixX);
    //Z.Set(M.toStdVector());
    for(i=0; i<M.size(); i++)
    {
        y = floor(i/npixX);
        x = i-y*npixX;
        Z.SetVal(M.at(i), x, y);
    }
    /*int minIndex=0;
    double mu,nu;
    for(int y=-npix;y<=npix;y++)
    {
        for(int x=-2*npix;x<=2*npix;x++)
        {
            //////////
            cx = (double)x/npix;cy=(double)y/npix;
            if((x!=0)&&(y!=0))
            {
                d = sqrt(cx*cx+cy*cy);
                psi=atan2(cy/d,cx/d);
                mu =cx/sqrt(tan(psi)*tan(psi)+0.25)/fabs(cx);
                nu =tan(psi)*mu;
                ed = sqrt(mu*mu+nu*nu);
            }
            else {d=0;ed=1;}
            /*stream << QString("%1").arg(cx,10,'f',5,QLatin1Char( ' ' )) <<'|'
                    << QString("%1").arg(cy,10,'f',5,QLatin1Char( ' ' )) <<'|'
                    << QString("%1").arg(psi*180/M_PI,10,'f',5,QLatin1Char( ' ' )) <<'|'
                    << QString("%1").arg(2*cos(psi),10,'f',5,QLatin1Char( ' ' )) <<'|'
                    << QString("%1").arg(sin(psi),10,'f',5,QLatin1Char( ' ' )) <<'|'
                    << endl;/
            if(d<ed)
            {
                ed = sqrt((cx-X[0])*(cx-X[0])+(cy-Y[0])*(cy-Y[0]));
                for(int i=0;i<X.count();i++)
                {
                    d = sqrt((cx-X[i])*(cx-X[i])+(cy-Y[i])*(cy-Y[i]));
                    if(d<ed){ed=d;minIndex=i;}
                    //stream << QString("%1").arg(M[i],10,'f',5,QLatin1Char( ' ' )) <<endl;
                }
                if(ed>2.0*M_PI/npix)Z.a[(y+npix)*(4*npix+1)+(x+2*npix)]=0;
                else
                {
                    //if(M[minIndex]!=0)
                     Z.a[(y+npix)*(4*npix+1)+(x+2*npix)]=M[minIndex];//(M[minIndex]-0.5*(minMag+maxMag))/(maxMag-minMag)/0.5;
                    //else
                    //    Z.a[(y+npix)*(4*npix+1)+(x+2*npix)]=minMag;
                    //stream << QString("%1").arg(M[minIndex],10,'f',5,QLatin1Char( ' ' )) <<'|';
                }
             }
             else Z.a[(y+npix)*(4*npix+1)+(x+2*npix)]=0;
            //stream << QString("%1").arg(-1,10,'f',5,QLatin1Char( ' ' )) <<'|';//OM[(y+npix)*4*npix+(x+2*npix)]=-1;
    //////////////
        }
        //stream << endl;
    }
    //////////////////////////////*/
    mglData xs(2),ys(2),zs(2),cs(2);
    xs.a[0]=-2;
    xs.a[1]=2;
    ys.a[0]=-1;
    ys.a[1]=1;
    //
    //qDebug() << QString("Range: %1 : %2\n").arg(minMag).arg(maxMag);

    if(clims.contains("yes"))
    {
        zs.a[0]=clims.section('|',1,1).toDouble();
        zs.a[1]=clims.section('|',2,2).toDouble();
        cs.a[0]=clims.section('|',1,1).toDouble();
        cs.a[1]=clims.section('|',2,2).toDouble();
    }
    else
    {
        zs.a[0]=minMag;//slimits.section('|',7,7).toDouble();
        zs.a[1]=maxMag;//slimits.section('|',8,8).toDouble();
        cs.a[0]=minMag;//slimits.section('|',7,7).toDouble();
        cs.a[1]=maxMag;//slimits.section('|',8,8).toDouble();
    }
    //
    mglGraph *gr = new mglGraph;
    gr->SetRanges(xs,ys,zs);
    gr->SetRange('c',cs);
    //gr->YRange(ys);
    //gr->CRange(zs);
    //gr->Box();
    //gr->Axis("xyz");
    gr->Dens(Z,argv[8]);
    //gr->Surf(Z,"wk");
    mglData ksi(101),eta(101);
    psi=0;
    //for(int i=0;i<=100;i++){ksi.a[i]=2*cos(psi);eta.a[i]=sin(psi);psi+=2*M_PI/100;}

    qDebug() << QString("colorBar: %1\n").arg(QString(argv[8]));
    gr->Colorbar(argv[8]);
    //gr->Colorbar("_wk");
    //gr->Plot(ksi,eta,"k.");
    ///////////////
    /*double lon=-M_PI;
    double lat =0;
    QVector<double> p,q;
    for(int i=0;i<=500;i++)
    {
        p<<-2*cos(lat)*sin(lon/2)/sqrt(1+cos(lat)*cos(lon/2));
        q<<-sin(lat)/sqrt(1+cos(lat)*cos(lon/2));
        //
        p<<-2*cos(lat+M_PI/4)*sin(lon/2)/sqrt(1+cos(lat+M_PI/4)*cos(lon/2));
        q<<-sin(lat+M_PI/4)/sqrt(1+cos(lat+M_PI/4)*cos(lon/2));
        //
        p<<-2*cos(lat-M_PI/4)*sin(lon/2)/sqrt(1+cos(lat-M_PI/4)*cos(lon/2));
        q<<-sin(lat-M_PI/4)/sqrt(1+cos(lat-M_PI/4)*cos(lon/2));
        //
        lon+=2*M_PI/500;
    }
    lon = 0;
    lat =-M_PI/2;
    for(int i=0;i<=500;i++)
    {
        p<<-2*cos(lat)*sin(lon/2)/sqrt(1+cos(lat)*cos(lon/2));
        q<<-sin(lat)/sqrt(1+cos(lat)*cos(lon/2));
        //
        p<<-2*cos(lat)*sin((lon+M_PI/2)/2)/sqrt(1+cos(lat)*cos((lon+M_PI/2)/2));
        q<<-sin(lat)/sqrt(1+cos(lat)*cos((lon+M_PI/2)/2));
        //
        p<<-2*cos(lat)*sin((lon-M_PI/2)/2)/sqrt(1+cos(lat)*cos((lon-M_PI/2)/2));
        q<<-sin(lat)/sqrt(1+cos(lat)*cos((lon-M_PI/2)/2));
        //
        lat+=M_PI/500;
    }
    mglData P,Q;
    P.Set(p.data(),p.count());
    Q.Set(q.data(),q.count());
    gr->Plot(P,Q," h.");*/
    //////////////
    double Ksi, Eta;
    ///////////////
    /*
    for (int i=0;i<=4;i++)
    {
        L = M_PI*i*90/180-M_PI;
        B = 0;
        Ksi=-2*cos(B)*sin(L/2)/sqrt(1+cos(B)*cos(L/2));
        Eta= -sin(B)/sqrt(1+cos(B)*cos(L/2));
        if(i*90!=180)
        gr->Puts(mglPoint(Ksi,Eta-0.2),
                 qPrintable(QString("%1").arg(i*90,2,10,QLatin1Char( ' ' ))));
        L = -M_PI;
        B = -M_PI*(-90+i*45)/180;
        Ksi=-2*cos(B)*sin(L/2)/sqrt(1+cos(B)*cos(L/2));
        Eta= -sin(B)/sqrt(1+cos(B)*cos(L/2));
        if(B!=0)
        {
            if(i*45-90>0)
                gr->Puts(mglPoint(Ksi+0.2,Eta),
                 qPrintable(QString("%1").arg(-90+i*45,2,10,QLatin1Char( ' ' ))));
            else
                gr->Puts(mglPoint(Ksi+0.2,Eta-0.2),
                 qPrintable(QString("%1").arg(-90+i*45,2,10,QLatin1Char( ' ' ))));
        }
    }*/
    ////////////////
    ///////////////
    /*mglData Mu,Nu;
    Mu.Set(X.data(),X.count());
    Nu.Set(Y.data(),Y.count());
    gr->Plot(Mu,Nu," k.");*/
    ///////////////
    gr->WriteEPS(qPrintable(epsfname),qPrintable(epsfname));
    //gr->WriteJPEG(qPrintable(epsfname+".jpg"),qPrintable(epsfname));
    delete gr;
    //////////////////////////////
    return 0;//a.exec();
}
