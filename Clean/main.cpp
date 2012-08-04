#include <QtCore/QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QList>
#include <QDebug>
#include <QSettings>

#include <time.h>

#include <complex>


#include "./../libs/uneven.h"
#include "./../libs/mb.h"


#define PI atan(1)*4

#define STRLEN 1024


//QTextStream logStm;

QTextStream resStm;
static QTextStream cout(stdout);

using namespace std;
/*
struct harmParam
{
    double A, P, Fi;
};

double detHarm(double t, QList <harmParam*> resList)
{
    double r0;
    int szh = resList.size();

    r0 = 0.0;

    for(int i=0; i<szh; i++) r0 += resList[i]->A*cos(2.0*PI*t/resList[i]->P + resList[i]->Fi);

    return(r0);
}
*/


#define A0 0.3
#define A1 1.3

double tFunc(double ai)
{
    double r0, r1;

    double P0 = 1200;
    //double A0 = 0.3;
    double Fi0 = -0.3;
    double P1 = 950;
    //double A1 = 1.3;
    double Fi1 = 0.766;

    r0 = A0*cos(2.0*PI*ai/P0 + Fi0);
    r1 = A1*cos(2.0*PI*ai/P1 + Fi1);
    return(r0+r1);
    //return(r0);
    //return 0.0;
}




int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    setlocale(LC_NUMERIC, "C");


    int i;





    QFile rFile("res.txt");
    rFile.open(QIODevice::Truncate | QIODevice::WriteOnly);
    resStm.setDevice(&rFile);
/*
    QFile logFile("log.txt");
    logFile.open(QIODevice::Truncate | QIODevice::WriteOnly);
    logStm.setDevice(&logFile);
*/
    QSettings *sett = new QSettings("./clean.ini", QSettings::IniFormat);

    cleanParam cp;

    QString colSep = sett->value("general/colSep", "|").toString();
    int cnt = sett->value("general/cnt", 0).toInt();
    int cnf = sett->value("general/cnf", 1).toInt();
    int redDeg = sett->value("general/redDeg", 10).toInt();

    cp.g = sett->value("cleanParam/g", 0.95).toDouble();
    cp.q = sett->value("cleanParam/q", 0.1).toDouble();
    cp.xi = sett->value("cleanParam/Xi", 4).toDouble();



    double *fData, *tData, *rData;
    double *rfData, *rtData;
    int tlen, flen;




    QVector <double> tVect;
    QVector <double> fVect;
    QString tStr;
    bool isOk;
    i=0;
    double t0, t1, tk, fk, tmin, tstep;
    t0 = 10000;
    t1 = 14000;
    tmin = t1;



    tlen = 3000;
    tstep = (t1-t0)/tlen;
    double snPar = 0.9;
    double rAmp=sqrt((A0*A0 + A1*A1)/(2.0*snPar));

if(argc<2)
{
    srand ( time(NULL) );
    for(i=0; i<tlen; i++)
    {

        tk = (rand()/(RAND_MAX*1.0))*(t1-t0)+t0;
        //tk = t0 + i*tstep;

        fk = tFunc(tk);//+rAmp*(rand()/(RAND_MAX*1.0)-0.5);
        //cout << QString("tk= %1\tfk= %2\n").arg(tk).arg(fk);
        if(tk<tmin) tmin = tk;
        tVect << tk;
        fVect << fk;
    }
}
else
{





    if(argc<2) return 1;
    QString fName = QString(argv[1]);
    QFile iFile(fName);
    iFile.open(QIODevice::ReadOnly);
    QTextStream iStm(&iFile);
    while(!iStm.atEnd())
    {
        tStr = iStm.readLine();
        tk = tStr.section(colSep, cnt, cnt, QString::SectionSkipEmpty).toDouble(&isOk);
        if((tk<t1)&&(tk>t0))
        {
            if(tk<tmin) tmin = tk;
            tVect << tk;
            if(!isOk) resStm << QString("WARN tVect[%1] toDouble problem\n").arg(i);
            fVect << tStr.section(colSep, cnf, cnf, QString::SectionSkipEmpty).toDouble(&isOk);
            if(!isOk) resStm << QString("WARN fVect[%1] toDouble problem\n").arg(i);

            i++;
        }
    }
    iFile.close();


}












    tlen = tVect.size();
    flen = fVect.size();
    if(tlen!=flen) return 2;


    fData = new double[flen];
    tData = new double[tlen];
    rData = new double[tlen];

    for(i=0; i<tlen; i++)
    {
        tData[i] = tVect.at(i);//-tmin;
        fData[i] = fVect.at(i);// - 0.0402*sin(tData[i]/47.1);
    }

    sortTvect(tData, fData, tlen);

    int len1, p, k;
    double s0, s1;

    if(redDeg>1)
    {
        div_t divresult;
        divresult = div(tlen, redDeg);
        len1 = divresult.quot;
        if(divresult.rem>0) len1++;

        p=0;
        k=0;
        s1 = s0 = 0;

        for(i=0; i<tlen; i++)
        {
            s0 += tData[i];
            s1 += fData[i];
            p++;
            if((p==redDeg)||(i==(tlen-1)))
            {
                tData[k] = s0/(1.0*p);
                fData[k] = s1/(1.0*p);
                k++;
                p=0;
                s0=0;
                s1=0;
            }

        }
        tlen = k-1;
    }

    t0 = tData[0];

    //for(i=0; i<tlen; i++) tData[i] = tData[i] - t0;

    QList <harmParam*> resList;

    makeClean(tData, fData, tlen, cp, resList, 1);

    rFile.close();


    rFile.setFileName("resData.txt");
    rFile.open(QIODevice::Truncate | QIODevice::WriteOnly);
    resStm.setDevice(&rFile);

    double meanR, rmsR, rmsM;

    rmsM = rmsR = meanR = 0.0;

    for(i=0; i<tlen; i++)
    {
        rData[i] = detHarm(tData[i], resList);
        meanR += rData[i]*rData[i];
        rmsR += fData[i]*fData[i];
        resStm << QString("%1 %2 %3 %4\n").arg(tData[i]).arg(fData[i]).arg(fData[i]-rData[i]).arg(rData[i]);
    }

    meanR /= tlen;
    rmsR /= tlen;

    qDebug() << QString("sgR: %1 - %2\n").arg(rmsR).arg(meanR);

    rFile.close();
   /*
    QFile rFile1("fisub.txt");
    rFile1.open(QIODevice::Truncate | QIODevice::WriteOnly);
    resStm1.setDevice(&rFile1);

    double fsub;

    for(i=0; i<tlen; i++)
    {
        fsub = fData[i] - 0.2936*cos(2.0*PI*(tData[i]/))
    }
*/
    return 0;
//    return a.exec();
}





/*
int makeClean(double *f_time, double *F, int N, cleanParam cp)
{
        FILE *FI, *FC, *FT, *FNT, *DF, *CF, *BF, *WF, *SF, *XF, *FRF, *FNRF;
        char *str, *slo;
        str = new char[STRLEN];
        slo = new char[32];
        char *s;
        s = new char[100];
        double tk, Dk, fk, res;
        int i;
        kompl *D, *Dt, *W, *C;
        double *y;
        int j, p, k;
        double re, im, pok;

        int Y, M;
        double Day, Jday;

        sprintf(str, "fi.dat");
        FI = fopen(str, "w");
        sprintf(str, "fc.dat");
        FC = fopen(str, "w");
        sprintf(str, "ft.dat");
        FT = fopen(str, "w");
        sprintf(str, "fnt.dat");
        FNT = fopen(str, "w");
        sprintf(str, "d.dat");
        DF = fopen(str, "w");
        sprintf(str, "c.dat");
        CF = fopen(str, "w");
        sprintf(str, "b.dat");
        BF = fopen(str, "w");
        sprintf(str, "w.dat");
        WF = fopen(str, "w");
        sprintf(str, "s.dat");
        SF = fopen(str, "w");
        sprintf(str, "xf.dat");
        XF = fopen(str, "w");
        sprintf(str, "fr.dat");
        FRF = fopen(str, "w");
        sprintf(str, "fnr.dat");
        FNRF = fopen(str, "w");

        //nfile++;

//centrovka//////////////////////////////////////////////
        double MNF, MXF, Dcentr;
        MNF = F[0];
        MXF = F[0];
        for(i=0; i<N; i++)
        {
                MNF += F[i];
                if(F[i]>MXF) MXF = F[i];
                //fprintf(FI, "%17.15e %9.6f 1\n", F[i], f_time[i]);
                fprintf(FI, "%9.6f %17.15e\n", f_time[i], F[i]);
        }
        MNF /= N;
//	Dcentr = (fabs(MXF - MNF))/2.0 + MNF;
        Dcentr = MNF;

        //this->m_MXD.Format("MXD =\t\t%f", MXF);
        //this->m_MND.Format("MND =\t\t%f", MNF);
        //this->m_DC.Format("DC =\t\t%f", Dcentr);
        //UpdateData(0);
        //Sleep(1000);

//	cout << "MXD " << MXF << " MND " << MNF << " Dc " << Dcentr << endl;
        for(i=0; i<N; i++)
        {
                F[i] = F[i] - Dcentr;
                //fprintf(FC, "%f %9.6f 1\n", F[i], f_time[i]);
                fprintf(FC, "%9.6f %17.15e\n", f_time[i], F[i]);
        }

//trend///////////////////////////////////////////////////////////

        //vector xj(2);
        int w = 2;
        double *xj = new double[w];
        //matrix aij(N, 2), ckj(2, 2), Dkj(2, 2);
        double *aij = new double[N*w];
        double *ckj = new double[w*w];
        double *Dkj = new double[w*w];

        //vector bj(2);
        double *bj = new double[w];
        double trend;
        for(i=0; i<N; i++)
        {
                //aij.set(i, 0, f_time[i]);
                //aij.set(i, 1, 1.0);
                aij[i*w+0] = f_time[i];
                aij[i*w+1] = 1.0;
        }

        for(k=0; k<2; k++)
        {
                for(j=0; j<2; j++)
                {
                        res = 0.0;
                        for(i=0; i<N; i++)
                        {
                                //res += aij.get(i, k)*aij.get(i, j);
                                res+= aij[i*w+k]*aij[i*w+j];
                        }
                        //ckj.set(k, j, res);
                        ckj[k*w+j] = res;
                }
        }

        for(j=0; j<2; j++)
        {
                res = 0.0;
                for(i=0; i<N; i++)
                {
                        //res += aij.get(i, j)*F[i];
                        res+= aij[i*w+j]*F[i];
                }
                //bj.set(j, res);
                bj[j] = res;
        }

        //Lsys(&ckj, &bj, &xj);
        slsm(w, 2, xj, ckj, bj);

        for(i=0; i<N; i++)
        {
                trend = xj[0]*f_time[i] + xj[1];
                //fprintf(FT, "%17.15e %9.6f 1\n", F[i], f_time[i]);
                //fprintf(FT, "%17.15e %9.6f 2\n", trend, f_time[i]);
                fprintf(FT, "%09.6f %17.15e %17.15e\n", f_time[i], F[i], trend);
                F[i] = F[i] - trend;
                //fprintf(FNT, "%17.15e %9.6f 1\n", F[i], f_time[i]);
                fprintf(FNT, "%09.6f %17.15e\n", f_time[i], F[i]);
        }

        fclose(FT);
        fclose(FNT);

//	control params	//

        double g, q, Xq, Xi;

        g = cp.g;
        q = cp.q;
        Xq = -log(1.0 - pow(1.0 - q, 2.0/(N-2.0)));
        Xi = cp.Xi;

        //this->m_Xq.Format("Xq =\t\t%f", Xq);
        resStm << QString("Xq= %1\n").arg(Xq);
//	cout << "Xq= " << Xq << endl;
        //UpdateData(0);
        //Sleep(1000);Re
//////////////////////////

        double Dmin, ymax;
        int m;

        Dk = fabs(f_time[N-1] - f_time[0]);

        Dmin = Dk/((double)N);

        //this->m_Dmin.Format("Dmin =\t\t%f", Dmin);
        resStm << QString("Dmin= %1\n").arg(Dmin);
        //UpdateData(0);
        //Sleep(1000);
//	cout << "Dmin= "<< Dmin << endl;


        ymax = 1.0/(2.0*Dmin);

        //this->m_ymax.Format("ymax =\t\t%f", ymax);
        resStm << QString("ymax= %1\n").arg(ymax);
        cout << QString("ymax= %1\n").arg(ymax);
        //UpdateData(0);
//	cout << "ymax= " << ymax << endl;
        resStm << QString("t[N-1]= %1\tt[0]= %2\n").arg(f_time[N-1]).arg(f_time[0]);
        cout << QString("t[N-1]= %1\tt[0]= %2\n").arg(f_time[N-1]).arg(f_time[0]);

        m = floor(Xi*ymax*(f_time[N-1] - f_time[0]));

        resStm << QString("m= %1\n").arg(m);
        cout << QString("m= %1\n").arg(m);
        //this->m_m.Format("m =\t\t%d", m);
        if(m<=0)
        {
                //AfxMessageBox("Ошибка! Проверьте ряд!");
                resStm << "\nError! Test row!\n";
                return 1;
        }
//	cout << "m= " << m << endl;

        //UpdateData(0);
        //Sleep(1000);
/////////////////////////////////////

        int m2 = 2*m+1;
        int m4 = 4*m+1;

        cout << QString("m2= %1\tm4= %2\n").arg(m2).arg(m4);

        D = new kompl[m2];
        Dt = new kompl[m2];
        C = new kompl[m2];
        W = new kompl[m4];
        y = new double[m4];

//	yk	///////////////////////////////////////
        resStm << QString("yk\n");
        cout << QString("yk\n");
        for(j=-2*m, p=0; j<=2*m; j++, p++)
        {
                y[p] = ((double)j/(double)m)*ymax;
        }


//	D	//////////////////////////////////////////
        resStm << QString("D\n");
        cout << QString("D\n\n");
        for(j = -m, p=0; j<=m; j++, p++)
        {
            cout << QString("\rp= %1\%").arg((p*1.0/(2.0*m+1.0))*100.0);
                re = im = 0.0;
                for(k=0; k<N ; k++)
                {
                        pok = -2.0*PI*y[p+m]*f_time[k];
                        re += F[k]*cos(pok);
                        im += F[k]*sin(pok);
                }
                D[p].setRe(re/((double)N));
                D[p].setIm(im/((double)N));
                C[p].setRe(0.0);
                C[p].setIm(0.0);

        }

        cout << "\n";


//	W	////////////////////////////////////////////
        resStm << QString("W\n");
        cout << QString("W\n\n");
        //p=0;
        for(p = 0; p<=4*m; p++)
        {

            cout << QString("\rp= %1\%\tp= %2|").arg((p*1.0/(4.0*m+1.0))*100).arg(p);

                re = im = 0.0;
                for(k=0; k<N ; k++)
                {
                        pok = -2.0*PI*y[p]*f_time[k];
                        re += cos(pok);
                        im += sin(pok);
                }
                W[p].setRe(re/((double)N));
                W[p].setIm(im/((double)N));

                //fprintf(WF, "%17.15e %e 1\n", W[p].mod(), y[p]);
                fprintf(WF, "%e %17.15e\n", y[p], W[p].mod());

                //p++;
        }
        cout << "\n\n";
/*
        cout << QString("Writing W\n");

        for(i=0; i<=4*m; i++)
        {
            cout << QString("p= %1\%\n").arg((i*1.0/(4.0*m+1.0))*100);
                fprintf(WF, "%17.15e %e 1\n", W[i].mod(), y[i]);
        }
/
        fclose(WF);

//	porog	/////////////////////////////////////

        double Dq, Dsr;

        Dsr = 0.0;
        resStm << QString("porog\n");
        cout << QString("porog\n");
        for(i=0; i<=m; i++)
        {
                Dsr += D[i].Re()*D[i].Re() + D[i].Im()*D[i].Im();
        }

        Dsr /= (m+1.0);
        Dq = Dsr*Xq;

        //this->m_Dq.Format("Dq =\t\t%f", Dq);
        resStm << QString("Dq= %1\n").arg(Dq);
        cout << QString("Dq= %1\n").arg(Dq);
        //UpdateData(0);
        //Sleep(1000);
//	cout << "Dq= " << Dq << endl;

        for(i=0; i<=2*m; i++)
        {
                //fprintf(DF, "%17.15e %e 1\n", D[i].mod(), y[i+m]);
                //fprintf(DF, "%17.15e %e 2\n", sqrt(Dq), y[i+m]);
            fprintf(DF, "%e %17.15e %17.15e\n", y[i+m], D[i].mod(), sqrt(Dq));
        }

        fclose(DF);

//	iteracii	////////////////////////////////////////////

        int iter, Jey_m, Jey_2m, Jey2;
        double Dmax, d, zn;
        double fi, A;
        kompl a;

        iter=0;

        do
        {
                Dmax = D[m+1].mod()*D[m+1].mod();
                Jey_m = m+1;
                //Dmax = D[m].mod()*D[m].mod();
                //Jey = m;

                for(i=m+2; i<=2*m; i++)
                //for(i=m+1; i<=2*m; i++)
                {
                        d = D[i].mod()*D[i].mod();
                        if(Dmax<d)
                        {
                                Dmax = d;
                                Jey_m = i;
                        }
                }

                Jey_2m = Jey_m + m;

                //this->m_Dmax.Format("Dmax =\t\t%f", Dmax);
                cout << QString("Dmax = %1\n").arg(Dmax);
                resStm << QString("Dmax = %1\n").arg(Dmax);
                //this->m_Jey.Format("J =\t\t%d", Jey);
                cout << QString("Jey = %1\n").arg(Jey_m-m);
                resStm << QString("Jey = %1\n").arg(Jey_m-m);
                //this->m_Period.Format("Period =\t%d", 1.0/y[Jey]);
                cout << QString("Period = %1\n").arg(1.0/(y[Jey_2m]));
                resStm << QString("Period = %1\n").arg(1.0/(y[Jey_2m]));
                //UpdateData(0);
                //Sleep(1000);
//		cout << "Dmax= " << Dmax << " J= " << Jey << endl;
//		cout << "Period: " << 1.0/y[Jey] << " day" << endl;

                if(Dmax<Dq) break;

                Jey2 = 2*Jey_2m-2*m;


                zn = 1.0 - pow(W[Jey2].mod(), 2.0);

                //this->m_W2J.Format("W[2J] =\t%f", W[2*Jey].mod());
                //UpdateData(0);
                //Sleep(1000);
//		cout << "W[2J]= " << W[2*Jey].mod() << endl;


                //a.setRe((D[Jey].Re() - (D[Jey].Re()*W[2*Jey].Re() + D[Jey].Im()*W[2*Jey].Im()))/zn);
                //a.setIm((D[Jey].Im() - (D[Jey].Re()*W[2*Jey].Im() - D[Jey].Im()*W[2*Jey].Re()))/zn);

                a.setRe((D[Jey_m].Re() - (D[Jey_m].Re()*W[Jey2].Re() + D[Jey_m].Im()*W[Jey2].Im()))/zn);
                a.setIm((D[Jey_m].Im() - (D[Jey_m].Re()*W[Jey2].Im() - D[Jey_m].Im()*W[Jey2].Re()))/zn);

                fi = atan2(a.Im(), a.Re());
                A = 2.0*a.mod();

                cout << QString("A= %1\tfi= %2\n").arg(A).arg(fi);
                resStm << QString("A= %1\tfi= %2\n").arg(A).arg(fi);



                //this->m_a.Format("a =\t\t%f + i%f", a.Re(), a.Im());
                //this->m_zn.Format("zn =\t\t%f", zn);
                //UpdateData(0);
                cout << QString("a.re= %1\ta.im= %2\ta.mod= %3\n").arg(a.Re()).arg(a.Im()).arg(a.mod());
                //cout << "a= " << a.Re() << " i " << a.Im() << " ; zn= " << zn << endl;

                for(j=-m, p=0; j<=m; j++, p++)
                {
                        D[p].setRe(D[p].Re() - g*(a.Re()*W[j-Jey_m+3*m].Re() - a.Im()*W[j-Jey_m+3*m].Im() + a.Re()*W[j+Jey_m+m].Re() - a.Im()*W[j+Jey_m+m].Im()));
                        D[p].setIm(D[p].Im() - g*(a.Im()*W[j-Jey_m+3*m].Re() + a.Re()*W[j-Jey_m+3*m].Im() + a.Re()*W[j+Jey_m+m].Im() - a.Im()*W[j+Jey_m+m].Re()));
                    //D[p].setRe(D[p].Re() - g*(a.Re()*W[j-Jey_2m].Re() - a.Im()*W[j-Jey_2m].Im() + a.Re()*W[j+Jey_2m].Re() - a.Im()*W[j+Jey_2m].Im()));
                    //D[p].setIm(D[p].Im() - g*(a.Im()*W[j-Jey_2m].Re() + a.Re()*W[j-Jey_2m].Im() + a.Re()*W[j+Jey_2m].Im() - a.Im()*W[j+Jey_2m].Re()));
                }

                C[Jey_m].setRe(C[Jey_m].Re() + g*a.Re());
                C[Jey_m].setIm(C[Jey_m].Im() + g*a.Im());
                C[m2-Jey_m].setRe(C[m2-Jey_m].Re() + g*a.Im());
                C[m2-Jey_m].setIm(C[m2-Jey_m].Im() + g*a.Re());

                iter++;
//		getch();

        }while(1);

//	postanaliz	//////////////////////////////////

        cout << "postanaliz\n";
        resStm << "postanaliz\n";

        //this->m_iter.Format("iter =\t\t%d", iter);
        //UpdateData(0);
        //Sleep(1000);
//	cout << "iter= " << iter << endl;
        if(iter==0)
        {
                //if(nfile==1) resStm << QString("В данном ряду с вероятностью %1 нет гармонических элементов").arg(1.0-q);
                //if(nfile==2) resStm << QString("В данном ряду с вероятностью %1 нет гармонических элементов").arg(1.0-q);
                resStm << QString("There is no harmonic elements in this row with probability of %1\n").arg(1.0-q);
                cout << QString("There is no harmonic elements in this row with probability of %1\n").arg(1.0-q);
//		cout << "V dannom ryadu s veroyatnost\'y " << 1.0-q << " net harmonic heskih elementov" << endl;
            //    UpdateData(0);
              //  Sleep(1000);
                return 0;
        }

        kompl *B, *S;

        B = new kompl[4*m+1];
        S = new kompl[2*m+1];

//	B	////////////////////////////////////////////
        double *t_ravn;
        t_ravn = new double[N];

        tk = (f_time[N-1] - f_time[0])/(N-1.0);

        cout << "B\n";

        for(j = -2*m, p=0; j<=2*m; j++, p++)
        {
            cout << QString("p= %1\%\r").arg((p*1.0/(4.0*m+1.0))*100);
                re = im = 0.0;
                for(k=0; k<N ; k++)
                {
                        t_ravn[k] = tk*k;
                        pok = -2.0*PI*y[p]*(t_ravn[k]);
                        re += cos(pok);
                        im += sin(pok);
                }
                B[p].setRe(re/((double)N));
                B[p].setIm(im/((double)N));
        }

        cout << "\n";

        for(i=0; i<=4*m; i++)
        {
                //fprintf(BF, "%17.15e %9.6f 1\n", B[i].mod(), (tk*(i)));
                //fprintf(BF, += C[l]*B[j-k+2*m];"%09.6f %17.15e\n",  (tk*i), B[i].mod());
            //fprintf(BF, "%09.6e %17.15e\n",  1.0/(tk*(i-2*m)), B[i].mod());
            fprintf(BF, "%09.6e %17.15e\n",  y[i], B[i].mod());

        }

//	S	/////////////////////////////////////////////
        int l;
        kompl tI;

        cout << "S\n";

        for(j = -m, p=0; j<=m; j++, p++)
        {
            cout << QString("p= %1\%\r").arg((p*1.0/(2.0*m+1.0))*100);
                re = im = 0.0;
                S[p].setRe(0.0);
                S[p].setIm(0.0);
                for(k=-m, l=0; k<=m; k++, l++)
                {
                        re += (C[l].Re()*B[j-k+2*m].Re() - C[l].Im()*B[j-k+2*m].Im());
                        im += (C[l].Re()*B[j-k+2*m].Im() + C[l].Im()*B[j-k+2*m].Re());
                    //tI = C[l]*B[j-k+2*m];

                   // S[p].setRe(S[p].Re + tI.Re());
                   // S[p].setRe(S[p].Im + tI.Im());

                    //S[p] += C[l]*B[j-k+2*m];
                }

                S[p].setRe(re);
                S[p].setIm(im);
        }

        cout << "\n";

        for(i=0; i<=2*m; i++)
        {
                //fprintf(SF, "%17.15e %e 1\n", S[i].mod(), 1.0/(tk*(i+m)));
            //fprintf(SF, "%e %17.15e\n", 1.0/(tk*(i-m)), S[i].mod());
            fprintf(SF, "%e %17.15e\n", y[i+m], S[i].mod());
        }

//	XiS	/////////////////////////////////////////////
        cout << "XiS\n";
        kompl *XiS, eDeg;

        XiS = new kompl[N];

        for(i=0; i<N; i++)
        {
            cout << QString("i= %1\%\r").arg((i*1.0/(1.0*N))*100);
                        re = im = 0.0;
                        XiS[i].setVal(0.0, 0.0);

                        for(j=-m, p=0; j<=m; j++, p++)
                        {
                                pok = 2.0*PI*y[p+m]*t_ravn[i];

                                eDeg.setVal(cos(pok), sin(pok));

                                re += S[p].mod()*S[p].mod()*cos(pok);
//				re += S[p].mod()*S[p].mod()*exp(pok);
                                im += S[p].mod()*S[p].mod()*sin(pok);
                                //XiS[i] += S[p].mod()*S[p].mod()*eDeg;
                        }
                        XiS[i].setRe(re*(N/(1.0*m)));
                        XiS[i].setIm(im*(N/(1.0*m)));

                        //XiS[i] *= N/(1.0*m);
        }

        cout << "\n";

        for(i=0; i<N; i++)
        {
                //fprintf(XF, "%17.15e %9.6f 1\n", XiS[i].Re(), t_ravn[i]);
            fprintf(XF, "%09.6f %17.15e\n", t_ravn[i], XiS[i].Re());
        }

//	f_ravn	//////////////////////////////////////////
        cout << "f_ravn\n";

        kompl *f_ravn, *f_nravn;
        f_ravn = new kompl[N];
        f_nravn = new kompl[N];


        kompl ex, exn;
        int pokn;

/*
        double *f_ravn, *f_nravn;
        f_ravn = new double[N];
        f_nravn = new double[N];
/
        kompl tkom;
        double cpok;
        double spok;

        for(i=0; i<N; i++)
        {
            cout << QString("i= %1\%\r").arg((i*1.0/(1.0*N))*100);
                        //f_ravn[i].setIm(0.0);
                        //f_ravn[i].setRe(0.0);
                        f_ravn[i].setVal(0.0, 0.0);
                        //f_ravn[i] = 0.0;

                        //f_nravn[i].setIm(0.0);
                        //f_nravn[i].setRe(0.0);
                        f_nravn[i].setVal(0.0, 0.0);
                        //f_nravn[i] = 0.0;

                        p=0;
                        for(j=-m; j<=m; j++)
                        {

                                pok = 2.0*PI*y[p+m]*t_ravn[i];
                                pokn = 2.0*PI*y[p+m]*f_time[i];

                                cpok = cos(pok);
                                spok= sin(pok);

                                //qDebug() << QString("pok: %1\tcpok: %2\tspok: %3\n").arg(pok).arg(cpok).arg(spok);

                                ex.setRe(cpok);
                                ex.setIm(spok);

                                exn.setRe(cos(pokn));
                                exn.setIm(sin(pokn));

                                tkom.setRe(S[p].Re()*ex.Re() - S[p].Im()*ex.Im());
                                tkom.setIm(S[p].Re()*ex.Im() + S[p].Im()*ex.Re());
                                //tkom.setRe(S[p].Re()*ex.Re());
                                //tkom.setIm(S[p].Re()*ex.Im());
                                //f_ravn[i] += tkom.mod();
                                f_ravn[i].addRe(tkom.Re());
                                f_ravn[i].addIm(tkom.Im());
                                //f_ravn[i] += (S[p]*ex).mod();
                                //tkom = S[p]*exn;
                                //f_nravn[i] += (S[p]*ex).mod();

                                tkom.setRe(S[p].Re()*exn.Re() - S[p].Im()*exn.Im());
                                tkom.setIm(S[p].Re()*exn.Im() + S[p].Im()*exn.Re());
                                f_nravn[i].addRe(tkom.Re());
                                f_nravn[i].addIm(tkom.Im());
                                //f_nravn[i] += tkom.Re();
/*
                                f_ravn[i].setRe(f_ravn[i].Re() + S[p].Re()*ex.Re() - S[p].Im()*ex.Im());
                                f_ravn[i].setIm(f_ravn[i].Im() + S[p].Re()*ex.Im() + S[p].Im()*ex.Re());

                                f_nravn[i].setRe(f_nravn[i].Re() + S[p].Re()*exn.Re() - S[p].Im()*exn.Im());
                                f_nravn[i].setIm(f_nravn[i].Im() + S[p].Re()*exn.Im() + S[p].Im()*exn.Re());
                                /

                                p++;
                        }

                        //f_ravn[i] *= N/(1.0*m);
                        f_ravn[i].setRe(f_ravn[i].Re() * N/(1.0*m));
                        f_ravn[i].setIm(f_ravn[i].Im() * N/(1.0*m));
                        f_nravn[i] *= N/(1.0*m);

                        /*

                        f_ravn[i].setRe(f_ravn[i].Re()*(N/(1.0*m)));
                        f_ravn[i].setIm(f_ravn[i].Im()*(N/(1.0*m)));

                        f_nravn[i].setRe(f_nravn[i].Re()*(N/(1.0*m)));
                        f_nravn[i].setIm(f_nravn[i].Im()*(N/(1.0*m)));/
        }

        cout << "\n";

        for(i=0; i<N; i++)
        {
                //fprintf(FRF, "%17.15e %9.6f 1\n", f_ravn[i].mod(), t_ravn[i]);
            fprintf(FRF, "%09.6f %17.15e\n", t_ravn[i], f_ravn[i].Re());

            fprintf(FNRF, "%09.6f %17.15e\n", f_time[i], f_nravn[i].mod());
        }

//	this->m_itog.Format("Подсчет завершен");
        //if(nfile==1) this->m_itog.Format("Подсчет завершен");
        //if(nfile==2) this->sItogo2.Format("Подсчет завершен");
        cout << QString("Count conclude\n");
        resStm << QString("Count conclude\n");

        //UpdateData(0);



        fclose(FI);
        fclose(FC);
        //fclose(FT);
        //fclose(FNT);
        //fclose(DF);
        fclose(CF);
        fclose(BF);
        //fclose(WF);
        fclose(SF);
        fclose(XF);
        fclose(FRF);
        fclose(FNRF);

        return 0;
}
*/

