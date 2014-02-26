#include <QtCore>
#include <QCoreApplication>
#include <math.h>

#include <mgl2/mgl.h>
//#include <mgl/mgl.h>

void loadDataFile(QString filename, QStringList& outList)
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QTextStream fstream(&file);
    while(!fstream.atEnd()) outList << fstream.readLine();
    file.close();
}

int main(int argc, char *argv[])
{
    QString fname(argv[1]);
    QString epsfname(argv[6]);
    QStringList sl;
    loadDataFile(fname,sl);
    int cl = QString(argv[2]).toInt();//longitude
    int cb = QString(argv[3]).toInt();//latitude
    int fl = QString(argv[4]).toInt();//flag for separator of columns
    QString style(argv[5]);
    /////////////////////////////
    QVector<double>X,Y;
    QVector<double>X1,Y1;
    double L,B;
    double Ksi,Eta;
    QString line;
    for(int i=0;i<sl.count();i++)
    {
        line = sl[i].simplified();
        /*
        if(fl==0)
        {
            L = M_PI*line.section('|',cl,cl).toDouble()/180-M_PI;
            B = -M_PI*line.section('|',cb,cb).toDouble()/180;
        }
        else
        {
            L = M_PI*line.section(' ',cl,cl).toDouble()/180-M_PI;
            B = -M_PI*line.section(' ',cb,cb).toDouble()/180;
        }
        */
        if(fl==0)
        {
            L = line.section('|',cl,cl).toDouble()-M_PI;
            B = -line.section('|',cb,cb).toDouble();
        }
        else
        {
            L = line.section(' ',cl,cl).toDouble()-M_PI;
            B = -line.section(' ',cb,cb).toDouble();
        }
        X<<-2*cos(B)*sin(L/2)/sqrt(1+cos(B)*cos(L/2));
        Y<< -sin(B)/sqrt(1+cos(B)*cos(L/2));
    }

    qDebug() << QString("pNum: %1\n").arg(X.size());

    //////////////////////////////
    mglData xs(2),ys(2);
    xs.a[0]=-2;
    xs.a[1]=2;
    ys.a[0]=-1;
    ys.a[1]=1;
    //
    mglGraph *gr = new mglGraph;
    //gr->XRange(xs);
    //gr->YRange(ys);
    gr->SetRanges(xs,ys);
    mglData ksi(201),eta(201);
    double psi=0;
    for(int i=0;i<=200;i++){ksi.a[i]=2*cos(psi);eta.a[i]=sin(psi);psi+=2*M_PI/200;}
    gr->Plot(ksi,eta,"h.");
    ///////////////
    double lon=-M_PI;
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
    gr->Plot(P,Q," h.");
    //////////////
    mglData Mu,Nu;
    Mu.Set(X.data(),X.count());
    Nu.Set(Y.data(),Y.count());
    gr->Plot(Mu,Nu,qPrintable(style));
    ///////////////
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
    }
    ////////////////
    gr->WriteEPS(qPrintable(epsfname),qPrintable(epsfname));
    //gr->WriteGIF(qPrintable(epsfname),qPrintable(epsfname));
    delete gr;
    //////////////////////////////
    return 0;
}
