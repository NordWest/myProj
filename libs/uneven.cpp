#include "uneven.h"

double detHarm(double t, QList <harmParam*> resList)
{
    double r0;
    int szh = resList.size();

    r0 = 0.0;

    for(int i=0; i<szh; i++) r0 += resList[i]->A*cos(2.0*PI*t/resList[i]->P + resList[i]->Fi);

    return(r0);
}

double mod(complex<double> val)
{
    return(sqrt(val.real()*val.real() + val.imag()*val.imag()));
}


void sortTvect(double* tVect, double* fVect, int szi)
{
    int i, j;



    double valMin;
    int posMin;

    for(i=0; i<szi; i++)
    {
        posMin = -1;
        valMin = tVect[i];
        for(j=i+1; j<szi; j++)
        {
            if(tVect[j]<valMin)
            {
                valMin = tVect[j];
                posMin = j;
            }
        }
        if(posMin!=-1)
        {
            tVect[posMin] = tVect[i];
            tVect[i] = valMin;
            valMin = fVect[posMin];
            fVect[posMin] = fVect[i];
            fVect[i] = valMin;
        }
    }
}

int makePoly(double *f_time, double *F, int N, double* resCoef, int polDeg, int isLog)
{
    int i, j;
    double *xj = new double[polDeg];
    double *bj = new double[N];

    double *aij = new double[N*polDeg];

    for(i=0; i<N; i++)
    {
            //aij.set(i, 0, f_time[i]);
            //aij.set(i, 1, 1.0);
        for(j=0; j<polDeg; j++) aij[i*polDeg+j] = pow(f_time[i], j);
            //aij[i*w+0] = f_time[i];
            //aij[i*w+1] = 1.0;

        bj[i] = F[i];
    }

    slsm(polDeg, N, xj, aij, bj);

    for(j=0; j<polDeg; j++) resCoef[j] = xj[j];

    return 0;
}

double detPoly(double x, double* coefV, int polDeg)
{
    int j;
    double res = 0.0;
    for(j=0; j<polDeg; j++) res += coefV[j]*pow(x, j);
    return res;
}

using namespace std;
int makeClean(double *f_time, double *F, int N, cleanParam cp, QList <harmParam*> &resList, int isLog)
{
        FILE *FI, *FC, *FT, *FNT, *DF, *DFO, *CF, *BF, *WF, *SF, *XF, *FRF, *FNRF;
        char *str, *slo;
        str = new char[STRLEN];
        slo = new char[32];
        char *s;
        s = new char[100];
        double tk, Dk, fk, res;
        int i;
        complex<double> *D, *Dt, *W, *C;
        complex<double> *fini;

        double *y;
        int j, p, k;
        double re, im, pok;
        complex<double> ex, exn;
        double pokn;

        harmParam* resHarm;

        QTextStream resStm;
        int Y, M;
        double Day, Jday;
        QFile rFile;
        fini = new complex<double>[N];
        double sgR;
        QTextStream cout(stdout);


        for(i=0; i<N; i++) fini[i] = complex<double>(F[i], 0.0);



        if(isLog)
        {
            //cout.setDevice(&stdout);
            rFile.setFileName("makeClean.log");
            rFile.open(QIODevice::Truncate | QIODevice::WriteOnly);
            resStm.setDevice(&rFile);
            sprintf(str, "fi.dat");
            FI = fopen(str, "w");
            sprintf(str, "fc.dat");
            FC = fopen(str, "w");
            //sprintf(str, "ft.dat");
            //FT = fopen(str, "w");
            //sprintf(str, "fnt.dat");
            //FNT = fopen(str, "w");
            sprintf(str, "d.dat");
            DF = fopen(str, "w");
            sprintf(str, "dfo.dat");
            DFO = fopen(str, "w");
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
        }

        sgR = 0.0;

        for(i=0; i<N; i++)
        {
               //trend = xj[0]*f_time[i] + xj[1];
                //fprintf(FT, "%17.15e %9.6f 1\n", F[i], f_time[i]);
                //fprintf(FT, "%17.15e %9.6f 2\n", trend, f_time[i]);
                //if(isLog) fprintf(FT, "%09.6f %17.15e %17.15e\n", f_time[i], F[i], trend);
                //F[i] = F[i] - trend;
                //fini[i] = complex<double>(F[i], 0.0);

                sgR += F[i]*F[i];
                //fprintf(FNT, "%17.15e %9.6f 1\n", F[i], f_time[i]);
                if(isLog) fprintf(FI, "%09.6f %17.15e\n", f_time[i], F[i]);
        }

        sgR /= N-1;

        if(isLog) cout << QString("sgR= %1\n").arg(sgR);
        if(isLog) resStm << QString("sgR= %1\n").arg(sgR);


        if(isLog) fclose(FI);
        //if(isLog) fclose(FNT);

        //nfile++;

//centrovka//////////////////////////////////////////////
/*        double MNF, MXF, Dcentr;
        MNF = F[0];
        MXF = F[0];
        for(i=0; i<N; i++)
        {
                MNF += F[i];
                if(F[i]>MXF) MXF = F[i];
                //fprintf(FI, "%17.15e %9.6f 1\n", F[i], f_time[i]);
                if(isLog) fprintf(FI, "%9.6f %17.15e\n", f_time[i], F[i]);
        }
        MNF /= N;
//	Dcentr = (fabs(MXF - MNF))/2.0 + MNF;
        Dcentr = MNF;

        //this->m_MXD.Format("MXD =\t\t%f", MXF);
        //this->m_MND.Format("MND =\t\t%f", MNF);
        //this->m_DC.Format("DC =\t\t%f", Dcentr);
        //UpdateData(0);
        //Sleep(1000);

        if(isLog) cout << "MXD " << MXF << " MND " << MNF << " Dc " << Dcentr << endl;
        for(i=0; i<N; i++)
        {
                F[i] = F[i] - Dcentr;
                //fprintf(FC, "%f %9.6f 1\n", F[i], f_time[i]);
                if(isLog) fprintf(FC, "%9.6f %17.15e\n", f_time[i], F[i]);
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

        //complex<double> *fini;
        //fini = new complex<double>[N];
        //double sgR;

        sgR = 0.0;

        for(i=0; i<N; i++)
        {
                trend = xj[0]*f_time[i] + xj[1];
                //fprintf(FT, "%17.15e %9.6f 1\n", F[i], f_time[i]);
                //fprintf(FT, "%17.15e %9.6f 2\n", trend, f_time[i]);
                if(isLog) fprintf(FT, "%09.6f %17.15e %17.15e\n", f_time[i], F[i], trend);
                F[i] = F[i] - trend;
                fini[i] = complex<double>(F[i], 0.0);

                sgR += F[i]*F[i];
                //fprintf(FNT, "%17.15e %9.6f 1\n", F[i], f_time[i]);
                if(isLog) fprintf(FNT, "%09.6f %17.15e\n", f_time[i], F[i]);
        }

        sgR /= N-1;

        if(isLog) cout << QString("sgR= %1\n").arg(sgR);
        if(isLog) resStm << QString("sgR= %1\n").arg(sgR);


        if(isLog) fclose(FT);
        if(isLog) fclose(FNT);
*/
//	control params	//

        double g, q, Xq, Xi;

        g = cp.g;
        q = cp.q;
        Xq = -log(1.0 - pow(1.0 - q, 2.0/(N-2.0)));
        Xi = cp.xi;

        //this->m_Xq.Format("Xq =\t\t%f", Xq);
        if(isLog) resStm << QString("Xq= %1\n").arg(Xq);
        if(isLog) cout << "Xq= " << Xq << endl;
        //UpdateData(0);
        //Sleep(1000);Re
//////////////////////////

        double Dmin, ymax;
        int m;

        Dk = fabs(f_time[N-1] - f_time[0]);

        Dmin = Dk/((double)N);

        //this->m_Dmin.Format("Dmin =\t\t%f", Dmin);
        if(isLog) resStm << QString("Dmin= %1\n").arg(Dmin);
        //UpdateData(0);
        //Sleep(1000);
        if(isLog) cout << "Dmin= "<< Dmin << endl;


        ymax = 1.0/(2.0*Dmin);

        //this->m_ymax.Format("ymax =\t\t%f", ymax);
        if(isLog) resStm << QString("ymax= %1\n").arg(ymax);
        if(isLog) cout << QString("ymax= %1\n").arg(ymax);
        //UpdateData(0);
//	cout << "ymax= " << ymax << endl;
        if(isLog) resStm << QString("t[N-1]= %1\tt[0]= %2\n").arg(f_time[N-1]).arg(f_time[0]);
        if(isLog) cout << QString("t[N-1]= %1\tt[0]= %2\n").arg(f_time[N-1]).arg(f_time[0]);

        m = floor(Xi*ymax*(f_time[N-1] - f_time[0]));

        if(isLog) resStm << QString("m= %1\n").arg(m);
        if(isLog) cout << QString("m= %1\n").arg(m);
        //this->m_m.Format("m =\t\t%d", m);
        if(m<=0)
        {
                //AfxMessageBox("Ошибка! Проверьте ряд!");
                if(isLog) resStm << "\nError! Test row!\n";
                return 1;
        }
        if(isLog) cout << "m= " << m << endl;

        //UpdateData(0);
        //Sleep(1000);
/////////////////////////////////////

        int m2 = 2*m+1;
        int m4 = 4*m+1;

        //cout << QString("m2= %1\tm4= %2\n").arg(m2).arg(m4);

        D = new complex<double>[m2];
        Dt = new complex<double>[m2];
        C = new complex<double>[m2];
        W = new complex<double>[m4];
        y = new double[m4];

//	yk	///////////////////////////////////////
        if(isLog) resStm << QString("yk\n");
        if(isLog) cout << QString("yk\n");
        for(j=-2*m, p=0; j<=2*m; j++, p++)
        {
                y[p] = ((double)j/(double)m)*ymax;
        }


//	D	//////////////////////////////////////////
        if(isLog) resStm << QString("D\n");
        if(isLog) cout << QString("D\n\n");
        for(j = -m, p=0; j<=m; j++, p++)
        {
           if(isLog) cout << QString("\rp= %1\%").arg((p*1.0/(2.0*m+1.0))*100.0);
                re = im = 0.0;
                D[p] = complex<double>(0,0);
                //if(isLog) resStm << QString("D[p]0= %1\t%2\n").arg(D[p].real()).arg(D[p].imag());
                for(k=0; k<N ; k++)
                {
                        pok = -2.0*PI*y[p+m]*f_time[k];
                        ex = complex<double>(cos(pok), sin(pok));
                        //D[p] += complex<double>(F[k]*cos(pok), F[k]*sin(pok));
                        D[p] += fini[k]*ex;
                        //D[p].imag() += complex<double>(fini[k]*ex).imag();
                        //re += F[k]*cos(pok);
                        //im += F[k]*sin(pok);
                        //if(isLog) resStm << QString("D[p]= %1\t%2\nex= %3\t%4\nfini: %5\t%6\n").arg(D[p].real()).arg(D[p].imag()).arg(ex.real()).arg(ex.imag()).arg(fini[k].real()).arg(fini[k].imag());
                }
                //D[p].setRe(re/((double)N));
                //D[p].setIm(im/((double)N));
                //re *= 1.0/((double)N);
                //im *= 1.0/((double)N);

                //D[p] *= 1.0/(2.0*N);
                D[p] *= 1.0/(1.0*N);


                if(isLog) resStm << QString("D[p]= %1\t%2\nre= %3\tim=%4\n").arg(D[p].real()).arg(D[p].imag()).arg(re).arg(im);

                C[p] = complex<double>(0,0);
                //C[p].setRe(0.0);
                //C[p].setIm(0.0);

        }

        //cout << "\n";


//	W	////////////////////////////////////////////
        if(isLog) resStm << QString("W\n");
        if(isLog) cout << QString("W\n\n");
        //p=0;
        for(p = 0; p<=4*m; p++)
        {

            if(isLog) cout << QString("\rp= %1\%\tp= %2|").arg((p*1.0/(4.0*m+1.0))*100).arg(p);

                re = im = 0.0;
                for(k=0; k<N ; k++)
                {
                        pok = -2.0*PI*y[p]*f_time[k];
                        W[p] += complex<double>(cos(pok), sin(pok));
                        re += cos(pok);
                        im += sin(pok);
                }
                //W[p].setRe(re/((double)N));
                //W[p].setIm(im/((double)N));
                W[p] *= 1.0/((double)N);

                re *= 1.0/((double)N);
                im *= 1.0/((double)N);

                //qDebug() << QString("W[p]= %1\t%2\nre= %3\tim=%4\n").arg(W[p].real()).arg(W[p].imag()).arg(re).arg(im);

                //fprintf(WF, "%17.15e %e 1\n", W[p].mod(), y[p]);
                if(isLog) fprintf(WF, "%e %17.15e\n", y[p], mod(W[p]));

                //p++;
        }
        if(isLog) cout << "\n\n";
/*
        cout << QString("Writing W\n");

        for(i=0; i<=4*m; i++)
        {
            cout << QString("p= %1\%\n").arg((i*1.0/(4.0*m+1.0))*100);
                fprintf(WF, "%17.15e %e 1\n", W[i].mod(), y[i]);
        }
*/
        if(isLog) fclose(WF);

//	porog	/////////////////////////////////////

        double Dq, Dsr;

        Dsr = 0.0;
        if(isLog) resStm << QString("porog\n");
        if(isLog) cout << QString("porog\n");
        for(i=0; i<=m; i++)
        {
                //Dsr += D[i].real()*D[i].real() + D[i].imag()*D[i].real();
                Dsr += pow(mod(D[i]), 2.0);
        }

        Dsr /= (m+1.0);
        Dq = Dsr*Xq;

        //this->m_Dq.Format("Dq =\t\t%f", Dq);
        if(isLog) resStm << QString("Dq= %1\n").arg(Dq);
        if(isLog) cout << QString("Dq= %1\n").arg(Dq);
        //UpdateData(0);
        //Sleep(1000);
//	cout << "Dq= " << Dq << endl;

        for(i=0; i<=2*m; i++)
        {
                //fprintf(DF, "%17.15e %e 1\n", D[i].mod(), y[i+m]);
                //fprintf(DF, "%17.15e %e 2\n", sqrt(Dq), y[i+m]);
            if(isLog) fprintf(DF, "%e %17.15e %17.15e\n", y[i+m], mod(D[i]), sqrt(Dq));
        }

        if(isLog) fclose(DF);


/////////////////////

        complex<double> *fiobr;
        fiobr = new complex<double>[N];


        for(i=0; i<N; i++)
        {
            if(isLog) cout << QString("i= %1\%\r").arg((i*1.0/(1.0*N))*100);
                        //f_ravn[i].setIm(0.0);
                        //f_ravn[i].setRe(0.0);
                        fiobr[i]= complex<double>(0.0, 0.0);
                        //f_ravn[i] = 0.0;

                        p=0;
                        for(j=m; j<3*m; j++)
                        {

                                pokn = 2.0*PI*y[j]*f_time[i];

                                exn= complex<double>(cos(pokn), sin(pokn));

                                fiobr[i] += D[p]*exn;

                                p++;
                        }

                        fiobr[i] *= N/(1.0*m);

                        if(isLog) fprintf(DFO, "%09.6f %17.15e\n", f_time[i], fiobr[i].real());
        }

       if(isLog) cout << "\n";


////////////////////

//	iteracii	////////////////////////////////////////////

        int iter, Jey_m, Jey_2m, Jey2;
        double Dmax, d, zn;
        double fi, A, P;
        complex<double> a;
        double aRe, aIm;
        double dRe, dIm;
        double cRe, cIm;

        iter=0;

        do
        {

                Dmax = pow(mod(D[m+1]), 2.0);//.mod()*D[m+1].mod();
                Jey_m = m+1;
                //Dmax = D[m].mod()*D[m].mod();
                //Jey = m;

                for(i=m+2; i<=2*m; i++)
                //for(i=m+1; i<=2*m; i++)
                {
                        d = mod(D[i])*mod(D[i]);
                        if(Dmax<d)
                        {
                                Dmax = d;
                                Jey_m = i;
                        }
                }

                Jey_2m = Jey_m + m;

                if(isLog)
                {
                    cout << QString("Dq = %1\n").arg(Dq);
                    resStm << QString("Dq = %1\n").arg(Dq);
                }

                //this->m_Dmax.Format("Dmax =\t\t%f", Dmax);
                if(isLog) cout << QString("Dmax = %1\n").arg(Dmax);
                if(isLog) resStm << QString("Dmax = %1\n").arg(Dmax);
                //this->m_Jey.Format("J =\t\t%d", Jey);
                if(isLog) cout << QString("Jey = %1\n").arg(Jey_m-m);
                if(isLog) resStm << QString("Jey = %1\n").arg(Jey_m-m);
                P = 1.0/(y[Jey_2m]);
                //this->m_Period.Format("Period =\t%d", 1.0/y[Jey]);
                if(isLog) cout << QString("Period = %1\n").arg(P);
                if(isLog) resStm << QString("Period = %1\n").arg(P);
                //UpdateData(0);
                //Sleep(1000);
//if(isLog)		cout << "Dmax= " << Dmax << " J= " << Jey << endl;
if(isLog)		cout << "Period: " << P << " day" << endl;

                if(Dmax<Dq) break;

                Jey2 = 2*Jey_2m-2*m;


                zn = 1.0 - mod(W[Jey2])*mod(W[Jey2]);

                //cout << QString("W^2= %1 = %2 = %3\n").arg(abs(W[Jey2])).arg(norm(W[Jey2])*norm(W[Jey2])).arg(W[Jey2].real()*W[Jey2].real() + W[Jey2].imag()*W[Jey2].imag());

                //this->m_W2J.Format("W[2J] =\t%f", W[2*Jey].mod());
                //UpdateData(0);
                //Sleep(1000);
//		cout << "W[2J]= " << W[2*Jey].mod() << endl;


                //a.setRe((D[Jey].Re() - (D[Jey].Re()*W[2*Jey].Re() + D[Jey].real()*W[2*Jey].real()))/zn);
                //a.setIm((D[Jey].real() - (D[Jey].Re()*W[2*Jey].real() - D[Jey].real()*W[2*Jey].Re()))/zn);

                //aRe = ((D[Jey_m].real() - (D[Jey_m].real()*W[Jey2].real() + D[Jey_m].imag()*W[Jey2].imag()))/zn);
                //aIm = ((D[Jey_m].imag() - (D[Jey_m].real()*W[Jey2].imag() - D[Jey_m].imag()*W[Jey2].real()))/zn);
                a = complex<double>((D[Jey_m] - conj(D[Jey_m])*W[Jey2]));
                a /= zn;

               if(isLog)cout << QString("a= %1\t%2\n").arg(aRe).arg(aIm);

                //a = complex<double>(aRe, aIm);
                if(isLog)cout << QString("a= %1\t%2\n").arg(a.real()).arg(a.imag());

                fi = atan2(a.imag(), a.real());
                A = 2.0*mod(a);

                if(isLog)cout << QString("A= %1\tfi= %2\n").arg(A).arg(fi);
                if(isLog) resStm << QString("A= %1\tfi= %2\n").arg(A).arg(fi);


                resHarm = new harmParam;
                resHarm->A = A;
                resHarm->Fi = fi;
                resHarm->P = P;
                resList << resHarm;

                //this->m_a.Format("a =\t\t%f + i%f", a.real(), a.real());
                //this->m_zn.Format("zn =\t\t%f", zn);
                //UpdateData(0);
                //cout << QString("a.re= %1\ta.im= %2\ta.mod= %3\n").arg(a.real()).arg(a.real()).arg(mod(a));
                //cout << "a= " << a.real() << " i " << a.real() << " ; zn= " << zn << endl;

                for(j=-m, p=0; j<=m; j++, p++)
                {
                        dRe = D[p].real() - g*(a.real()*W[j-Jey_m+3*m].real() - a.imag()*W[j-Jey_m+3*m].imag() + a.real()*W[j+Jey_m+m].real() - a.imag()*W[j+Jey_m+m].imag());
                        dIm = D[p].imag() - g*(a.imag()*W[j-Jey_m+3*m].real() + a.real()*W[j-Jey_m+3*m].imag() + a.real()*W[j+Jey_m+m].imag() - a.imag()*W[j+Jey_m+m].real());
                    //D[p].setRe(D[p].real() - g*(a.real()*W[j-Jey_2m].real() - a.real()*W[j-Jey_2m].real() + a.real()*W[j+Jey_2m].real() - a.real()*W[j+Jey_2m].real()));
                    //D[p].setIm(D[p].real() - g*(a.real()*W[j-Jey_2m].real() + a.real()*W[j-Jey_2m].real() + a.real()*W[j+Jey_2m].real() - a.real()*W[j+Jey_2m].real()));
                        //D[p] = complex<double>(dRe, dIm);
                        D[p] = D[p] - (a*W[j-Jey_m+3*m]+conj(a)*W[j+Jey_m+m])*g;
                }

                //cRe = C[Jey_m].real() + g*a.real();
                //cIm = C[Jey_m].real() + g*a.real();
                //C[Jey_m] = complex<double>(cRe, cIm);
                //C[Jey_m].setRe(C[Jey_m].real() + g*a.real());
                //C[Jey_m].setIm(C[Jey_m].real() + g*a.real());
                C[Jey_m] = C[Jey_m] + a*g;

                //cRe = C[m2-Jey_m].real() + g*a.real();
                //cIm = C[m2-Jey_m].real() + g*a.real();
                //C[m2-Jey_m].setRe(C[m2-Jey_m].real() + g*a.real());
                //C[m2-Jey_m].setIm(C[m2-Jey_m].real() + g*a.real());
                //C[m2-Jey_m] = complex<double>(cRe, cIm);
                C[m2-Jey_m] = C[m2-Jey_m] + conj(a)*g;

                iter++;
//		getch();

        }while(1);

//	postanaliz	//////////////////////////////////

        if(isLog)cout << "postanaliz\n";
        if(isLog) resStm << "postanaliz\n";

        //this->m_iter.Format("iter =\t\t%d", iter);
        //UpdateData(0);
        //Sleep(1000);
//	cout << "iter= " << iter << endl;
        if(iter==0)
        {
                //if(nfile==1) resStm << QString("В данном ряду с вероятностью %1 нет гармонических элементов").arg(1.0-q);
                //if(nfile==2) resStm << QString("В данном ряду с вероятностью %1 нет гармонических элементов").arg(1.0-q);
                if(isLog) resStm << QString("There is no harmonic elements in this row with probability of %1\n").arg(1.0-q);
                //cout << QString("There is no harmonic elements in this row with probability of %1\n").arg(1.0-q);
//		cout << "V dannom ryadu s veroyatnost\'y " << 1.0-q << " net harmonic heskih elementov" << endl;
            //    UpdateData(0);
              //  Sleep(1000);
                return 0;
        }

        complex<double> *B, *S;

        B = new complex<double>[4*m+1];
        S = new complex<double>[2*m+1];

//	B	////////////////////////////////////////////
        double *t_ravn;
        t_ravn = new double[N];

        tk = (f_time[N-1] - f_time[0])/(N-1.0);

        if(isLog) cout << "B\n";

        for(j = -2*m, p=0; j<=2*m; j++, p++)
        {
            if(isLog) cout << QString("p= %1\%\r").arg((p*1.0/(4.0*m+1.0))*100);
                //re = im = 0.0;
                B[p] = complex<double>(0.0, 0.0);
                for(k=0; k<N ; k++)
                {
                        t_ravn[k] = tk*k;
                        pok = -2.0*PI*y[p]*(t_ravn[k]);
                        B[p] += complex<double>(cos(pok), sin(pok));
                        //re += cos(pok);
                        //im += sin(pok);
                }
                //B[p].setRe(re/((double)N));
                //B[p].setIm(im/((double)N));
                B[p] *= 1.0/N;


        }

        if(isLog) cout << "\n";

        for(i=0; i<=4*m; i++)
        {
                //fprintf(BF, "%17.15e %9.6f 1\n", B[i].mod(), (tk*(i)));
                //fprintf(BF, += C[l]*B[j-k+2*m];"%09.6f %17.15e\n",  (tk*i), B[i].mod());
            //fprintf(BF, "%09.6e %17.15e\n",  1.0/(tk*(i-2*m)), B[i].mod());
            if(isLog) fprintf(BF, "%09.6e %17.15e\n",  y[i], mod(B[i]));

        }

//	S	/////////////////////////////////////////////
        int l;
        //kompl tI;

       if(isLog) cout << "S\n";

        for(j = -m, p=0; j<=m; j++, p++)
        {
            if(isLog) cout << QString("p= %1\%\r").arg((p*1.0/(2.0*m+1.0))*100);
                //re = im = 0.0;
                //S[p].setRe(0.0);
                //S[p].setIm(0.0);
                S[p] = complex<double>(0.0, 0.0);
                for(k=-m, l=0; k<=m; k++, l++)
                {
                        //re += (C[l].real()*B[j-k+2*m].real() - C[l].real()*B[j-k+2*m].real());
                        //im += (C[l].real()*B[j-k+2*m].real() + C[l].real()*B[j-k+2*m].real());
                    //tI = C[l]*B[j-k+2*m];


                   // S[p].setRe(S[p].Re + tI.real());
                   // S[p].setRe(S[p].Im + tI.real());

                    S[p] += C[l]*B[j-k+2*m];
                }

                //S[p].setRe(re);
                //S[p].setIm(im);
        }

        if(isLog) cout << "\n";

        for(i=0; i<=2*m; i++)
        {
                //fprintf(SF, "%17.15e %e 1\n", S[i].mod(), 1.0/(tk*(i+m)));
            //fprintf(SF, "%e %17.15e\n", 1.0/(tk*(i-m)), S[i].mod());
            if(isLog) fprintf(SF, "%e %17.15e\n", y[i+m], mod(S[i]));
            if(isLog) fprintf(CF, "%e %17.15e\n", y[i+m], mod(C[i]));
        }

//	XiS	/////////////////////////////////////////////
        if(isLog) cout << "XiS\n";
        complex<double> *XiS, eDeg;

        XiS = new complex<double>[N];

        for(i=0; i<N; i++)
        {
            if(isLog) cout << QString("i= %1\%\r").arg((i*1.0/(1.0*N))*100);
                        //re = im = 0.0;
                        XiS[i]= complex<double>(0.0, 0.0);

                        for(j=-m, p=0; j<=m; j++, p++)
                        {
                                pok = 2.0*PI*y[p+m]*t_ravn[i];

                                eDeg= complex<double>(cos(pok), sin(pok));

                                //re += S[p].mod()*S[p].mod()*cos(pok);
//				re += S[p].mod()*S[p].mod()*exp(pok);
                                //im += S[p].mod()*S[p].mod()*sin(pok);
                                XiS[i] += eDeg*mod(S[p])*mod(S[p]);
                        }

                        //XiS[i].setRe(re*(N/(1.0*m)));
                        //XiS[i].setIm(im*(N/(1.0*m)));

                        XiS[i] *= N/(1.0*m);
        }

        if(isLog) cout << "\n";

        for(i=0; i<N; i++)
        {
                //fprintf(XF, "%17.15e %9.6f 1\n", XiS[i].real(), t_ravn[i]);
            if(isLog) fprintf(XF, "%09.6f %17.15e\n", t_ravn[i], XiS[i].real());
        }

//	f_ravn	//////////////////////////////////////////
        if(isLog) cout << "f_ravn\n";

        complex<double> *f_ravn, *f_nravn;
        f_ravn = new complex<double>[N];
        f_nravn = new complex<double>[N];




/*
        double *f_ravn, *f_nravn;
        f_ravn = new double[N];
        f_nravn = new double[N];
*/

        double cpok;
        double spok;

        for(i=0; i<N; i++)
        {
            //cout << QString("i= %1\%\r").arg((i*1.0/(1.0*N))*100);
                        //f_ravn[i].setIm(0.0);
                        //f_ravn[i].setRe(0.0);
                        f_ravn[i]= complex<double>(0.0, 0.0);
                        //f_ravn[i] = 0.0;

                        //f_nravn[i].setIm(0.0);
                        //f_nravn[i].setRe(0.0);
                        f_nravn[i]= complex<double>(0.0, 0.0);
                        //f_nravn[i] = 0.0;

                        p=0;
                        for(j=m; j<3*m; j++)
                        {

                                pok = 2.0*PI*y[j]*t_ravn[i];
                                pokn = 2.0*PI*y[j]*f_time[i];

                                cpok = cos(pok);
                                spok= sin(pok);

                                //qDebug() << QString("pok: %1\tcpok: %2\tspok: %3\n").arg(pok).arg(cpok).arg(spok);

                                //ex.setRe(cpok);
                                //ex.setIm(spok);
                                ex= complex<double>(cpok, spok);

                                //exn.setRe(cos(pokn));
                                //exn.setIm(sin(pokn));
                                exn= complex<double>(cos(pokn), sin(pokn));

                                //tkom.setRe(S[p].real()*ex.real() - S[p].real()*ex.real());
                                //tkom.setIm(S[p].real()*ex.real() + S[p].real()*ex.real());
                                //tkom.setRe(S[p].real()*ex.real());
                                //tkom.setIm(S[p].real()*ex.real());
                                //f_ravn[i] += tkom.mod();
                                //f_ravn[i].addRe(tkom.real());
                                //f_ravn[i].addIm(tkom.real());
                                //f_ravn[i] += (S[p]*ex).mod();
                                //tkom = S[p]*exn;
                                f_ravn[i] += S[p]*ex;

                                //tkom.setRe(S[p].real()*exn.real() - S[p].real()*exn.real());
                                //tkom.setIm(S[p].real()*exn.real() + S[p].real()*exn.real());
                                //f_nravn[i].addRe(tkom.real());
                                //f_nravn[i].addIm(tkom.real());
                                //f_nravn[i] += tkom.real();
                                f_nravn[i] += D[p]*exn;

/*
                                f_ravn[i].setRe(f_ravn[i].real() + S[p].real()*ex.real() - S[p].real()*ex.real());
                                f_ravn[i].setIm(f_ravn[i].real() + S[p].real()*ex.real() + S[p].real()*ex.real());

                                f_nravn[i].setRe(f_nravn[i].real() + S[p].real()*exn.real() - S[p].real()*exn.real());
                                f_nravn[i].setIm(f_nravn[i].real() + S[p].real()*exn.real() + S[p].real()*exn.real());
                                */

                                p++;
                        }

                        f_ravn[i] *= N/(1.0*m);
                        //f_ravn[i].setRe(f_ravn[i].real() * N/(1.0*m));
                        //f_ravn[i].setIm(f_ravn[i].real() * N/(1.0*m));
                        f_nravn[i] *= N/(1.0*m);

                        /*

                        f_ravn[i].setRe(f_ravn[i].real()*(N/(1.0*m)));
                        f_ravn[i].setIm(f_ravn[i].real()*(N/(1.0*m)));

                        f_nravn[i].setRe(f_nravn[i].real()*(N/(1.0*m)));
                        f_nravn[i].setIm(f_nravn[i].real()*(N/(1.0*m)));*/
        }

        if(isLog) cout << "\n";

        sgR = 0.0;

        for(i=0; i<N; i++)
        {
                //fprintf(FRF, "%17.15e %9.6f 1\n", f_ravn[i].mod(), t_ravn[i]);
            if(isLog)fprintf(FRF, "%09.6f %17.15e %17.15e %17.15e\n", t_ravn[i], f_ravn[i].real(), f_ravn[i].imag(), mod(f_ravn[i]));

            sgR += f_nravn[i].real()*f_nravn[i].real();
            if(isLog) fprintf(FNRF, "%09.6f %17.15e\n", f_time[i], f_nravn[i].real());
        }

        sgR /= N-1;

        if(isLog) cout << QString("res sgR= %1\n").arg(sgR);
        if(isLog) resStm << QString("res sgR= %1\n").arg(sgR);

//	this->m_itog.Format("Подсчет завершен");
        //if(nfile==1) this->m_itog.Format("Подсчет завершен");
        //if(nfile==2) this->sItogo2.Format("Подсчет завершен");
        if(isLog) cout << QString("Count conclude\n");
        if(isLog) resStm << QString("Count conclude\n");

        //UpdateData(0);


        if(isLog)
        {
            rFile.close();


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
        }

        return 0;
}
