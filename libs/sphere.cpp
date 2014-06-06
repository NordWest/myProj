#include "sphere.h"


int lsmCount(double *ra, double *dec, double *dRa, double *dDe, int pointNum, double *Eps, double *sgEps)
{
    int i;

    double *A = new double[3*pointNum*2];
    double *r = new double[pointNum*2];
    double *W = new double[pointNum*2];
    double *Z = new double[3];

    double *Ara = new double[3*pointNum];
    double *rRa = new double[pointNum];
    double *Wra = new double[pointNum];
    double *Zra = new double[3];

    double *Ade = new double[2*pointNum];
    double *rDe = new double[pointNum];
    double *Wde = new double[pointNum];
    double *Zde = new double[2];

    double Dx[3][3];
    double Dra[3][3];
    double Dde[2][2];
    double uwe, uweRa, uweDe;

    for(i=0; i<pointNum; i++)
    {

        rRa[i] = r[i] = dRa[i];
        rDe[i] = r[pointNum+i] = dDe[i];

        Ara[i*3] = A[i*3] = -(sin(dec[i])*cos(ra[i]));
        Ara[i*3+1] = A[i*3+1] = -(sin(dec[i])*sin(ra[i]));
        Ara[i*3+2] = A[i*3+2] = cos(dec[i]);

        Ade[i*2] = A[pointNum+i*3] = sin(ra[i]);
        Ade[i*2+1] = A[pointNum+i*3+1] = -cos(ra[i]);
        A[pointNum+i*3+2] = 0.0;

        Wra[i] = Wde[i] = W[i] = 1.0;
        W[pointNum+i] = 1.0;
    }


////////////////////////////////////////////////////////////////////

    lsm(3, pointNum*2, Z, A, r, uwe, &Dx[0][0], W);

    qDebug() << QString("Z: %1 +- %2\t%3 +- %4\t%5 +- %6\n").arg(rad2mas(Z[0]),12, 'f', 8).arg(rad2mas(sqrt(Dx[0][0])),12, 'f', 8).arg(rad2mas(Z[1]),12, 'f', 8).arg(rad2mas(sqrt(Dx[1][1])),12, 'f', 8).arg(rad2mas(Z[2]),12, 'f', 8).arg(rad2mas(sqrt(Dx[2][2])),12, 'f', 8);
    qDebug() << QString("uwe: %1\n").arg(rad2mas(sqrt(uwe)),12, 'f', 8);

    lsm(3, pointNum, Zra, Ara, rRa, uweRa, &Dra[0][0], Wra);
    qDebug() << QString("Zra: %1 +- %2\t%3 +- %4\t%5 +- %6\n").arg(rad2mas(Zra[0]),12, 'f', 8).arg(rad2mas(sqrt(Dra[0][0])),12, 'f', 8).arg(rad2mas(Zra[1]),12, 'f', 8).arg(rad2mas(sqrt(Dra[1][1])),12, 'f', 8).arg(rad2mas(Zra[2]),12, 'f', 8).arg(rad2mas(sqrt(Dra[2][2])),12, 'f', 8);
    qDebug() << QString("uweRa: %1\n").arg(rad2mas(sqrt(uweRa)),12, 'f', 8);

    lsm(2, pointNum, Zde, Ade, rDe, uweDe, &Dde[0][0], Wde);
    qDebug() << QString("Zde: %1 +- %2\t%3 +- %4\n").arg(rad2mas(Zde[0]),12, 'f', 8).arg(rad2mas(sqrt(Dde[0][0])),12, 'f', 8).arg(rad2mas(Zde[1]),12, 'f', 8).arg(rad2mas(sqrt(Dde[1][1])),12, 'f', 8);
    qDebug() << QString("uweDe: %1\n").arg(rad2mas(sqrt(uweDe)),12, 'f', 8);

    Eps[0] = Zde[0];
    Eps[1] = Zde[1];
    Eps[2] = Zra[2];

    sgEps[0] = sqrt(Dde[0][0]);
    sgEps[1] = sqrt(Dde[1][1]);
    sgEps[2] = sqrt(Dra[2][2]);

    return 0;
}

int lsmCountVel(double *dTime, double *ra, double *dec, double *dRa, double *dDe, int pointNum, double *Eps, double *sgEps)
{
    int i;

    double *A = new double[6*pointNum*2];
    double *r = new double[pointNum*2];
    double *W = new double[pointNum*2];
    double *Z = new double[6];

    double *Ara = new double[6*pointNum];
    double *rRa = new double[pointNum];
    double *Wra = new double[pointNum];
    double *Zra = new double[6];

    double *Ade = new double[4*pointNum];
    double *rDe = new double[pointNum];
    double *Wde = new double[pointNum];
    double *Zde = new double[4];

    double Dx[6][6];
    double Dra[6][6];
    double Dde[4][4];
    double uwe, uweRa, uweDe;

    for(i=0; i<pointNum; i++)
    {

//        obj = new double[2];
//        objR = new double[2];
        rRa[i] = r[i] = dRa[i];
        rDe[i] = r[pointNum+i] = dDe[i];

        Ara[i*6] = A[i*6] = -(sin(dec[i])*cos(ra[i]));
        Ara[i*6+1] = A[i*6+1] = -(sin(dec[i])*sin(ra[i]));
        Ara[i*6+2] = A[i*6+2] = cos(dec[i]);
        Ara[i*6+3] = A[i*6+3] = -(sin(dec[i])*cos(ra[i]))*dTime[i];
        Ara[i*6+4] = A[i*6+4] = -(sin(dec[i])*sin(ra[i]))*dTime[i];
        Ara[i*6+5] = A[i*6+5] = cos(dec[i])*dTime[i];

        Ade[i*4] = A[pointNum+i*6] = sin(ra[i]);
        Ade[i*4+1] = A[pointNum+i*6+1] = -cos(ra[i]);
        A[pointNum+i*6+2] = 0.0;
        Ade[i*4+2] = A[pointNum+i*6+3] = sin(ra[i])*dTime[i];
        Ade[i*4+3] = A[pointNum+i*6+4] = -cos(ra[i])*dTime[i];
        A[pointNum+i*3+5] = 0.0;

        Wra[i] = Wde[i] = W[i] = 1.0;
        W[pointNum+i] = 1.0;

        //qDebug() << QString("dTime = %1\n").arg(dTime[i],12, 'f', 8);

        //qDebug() << QString("%1\t%2\t%3\t %4\t%5\t%6 = %7\n").arg((A[i*6]),12, 'f', 8).arg((A[i*6+1]),12, 'f', 8).arg((A[i*6+2]),12, 'f', 8).arg((A[i*6+3]),12, 'f', 8).arg((A[i*6+4]),12, 'f', 8).arg((A[i*6+5]),12, 'f', 8).arg((r[i]),12, 'f', 8);
        //qDebug() << QString("%1\t%2\t%3\t %4\t%5\t%6 = %7\n").arg(rad2mas(A[pointNum+i*6]),12, 'f', 8).arg(rad2mas(A[pointNum+i*6+1]),12, 'f', 8).arg(rad2mas(A[pointNum+i*6+2]),12, 'f', 8).arg(rad2mas(A[pointNum+i*6+3]),12, 'f', 8).arg(rad2mas(A[pointNum+i*6+4]),12, 'f', 8).arg(rad2mas(A[pointNum+i*6+5]),12, 'f', 8).arg(rad2mas(r[pointNum+i]),12, 'f', 8);
    }


////////////////////////////////////////////////////////////////////
//    qDebug() << QString("Eps: %1\t%2\t%3\n").arg(rad2mas(Eps[0]),12, 'f', 8).arg(rad2mas(Eps[1]),12, 'f', 8).arg(rad2mas(Eps[2]),12, 'f', 8);

    lsm(6, pointNum*2, Z, A, r, uwe, &Dx[0][0], W);

//    slsm(3, pointNum, Z, A, r, W);
//    iLSM(3, pointNum, 0.6, &exclind[0], Z, A, r, uwe, &Dx[0][0], -1, rn, W);
    qDebug() << QString("Z: %1 +- %2\t%3 +- %4\t%5 +- %6\n").arg(rad2mas(Z[0]),12, 'f', 8).arg(rad2mas(sqrt(Dx[0][0])),12, 'f', 8).arg(rad2mas(Z[1]),12, 'f', 8).arg(rad2mas(sqrt(Dx[1][1])),12, 'f', 8).arg(rad2mas(Z[2]),12, 'f', 8).arg(rad2mas(sqrt(Dx[2][2])),12, 'f', 8);
    qDebug() << QString("Zvel: %1 +- %2\t%3 +- %4\t%5 +- %6\n").arg(rad2mas(Z[3]),12, 'f', 8).arg(rad2mas(sqrt(Dx[3][3])),12, 'f', 8).arg(rad2mas(Z[4]),12, 'f', 8).arg(rad2mas(sqrt(Dx[4][4])),12, 'f', 8).arg(rad2mas(Z[5]),12, 'f', 8).arg(rad2mas(sqrt(Dx[5][5])),12, 'f', 8);
    qDebug() << QString("uwe: %1\n").arg(rad2mas(sqrt(uwe)),12, 'f', 8);
//    slsm(2, pointNum, Zd, Ad, rd, Wd);
  //  qDebug() << "Zd: " << Zd[0] << "\t" << Zd[1] << "\n";
//    qDebug() << QString("Zd: %1\t%2\n").arg(Zd[0],12, 'f', 8).arg(Zd[1],12, 'f', 8);

    lsm(6, pointNum, Zra, Ara, rRa, uweRa, &Dra[0][0], Wra);
    qDebug() << QString("Zra: %1 +- %2\t%3 +- %4\t%5 +- %6\n").arg(rad2mas(Zra[0]),12, 'f', 8).arg(rad2mas(sqrt(Dra[0][0])),12, 'f', 8).arg(rad2mas(Zra[1]),12, 'f', 8).arg(rad2mas(sqrt(Dra[1][1])),12, 'f', 8).arg(rad2mas(Zra[2]),12, 'f', 8).arg(rad2mas(sqrt(Dra[2][2])),12, 'f', 8);
    qDebug() << QString("ZraVel: %1 +- %2\t%3 +- %4\t%5 +- %6\n").arg(rad2mas(Zra[3]),12, 'f', 8).arg(rad2mas(sqrt(Dra[3][3])),12, 'f', 8).arg(rad2mas(Zra[4]),12, 'f', 8).arg(rad2mas(sqrt(Dra[4][4])),12, 'f', 8).arg(rad2mas(Zra[5]),12, 'f', 8).arg(rad2mas(sqrt(Dra[5][5])),12, 'f', 8);
    qDebug() << QString("uweRa: %1\n").arg(rad2mas(sqrt(uweRa)),12, 'f', 8);

    lsm(4, pointNum, Zde, Ade, rDe, uweDe, &Dde[0][0], Wde);
    qDebug() << QString("Zde: %1 +- %2\t%3 +- %4\n").arg(rad2mas(Zde[0]),12, 'f', 8).arg(rad2mas(sqrt(Dde[0][0])),12, 'f', 8).arg(rad2mas(Zde[1]),12, 'f', 8).arg(rad2mas(sqrt(Dde[1][1])),12, 'f', 8);
    qDebug() << QString("Zde: %1 +- %2\t%3 +- %4\n").arg(rad2mas(Zde[2]),12, 'f', 8).arg(rad2mas(sqrt(Dde[2][2])),12, 'f', 8).arg(rad2mas(Zde[3]),12, 'f', 8).arg(rad2mas(sqrt(Dde[3][3])),12, 'f', 8);
    qDebug() << QString("uweDe: %1\n").arg(rad2mas(sqrt(uweDe)),12, 'f', 8);

/*
    Eps[0] = Zde[0];
    Eps[1] = Zde[1];
    Eps[2] = Zra[2];

    sgEps[0] = sqrt(Dde[0][0]);
    sgEps[1] = sqrt(Dde[1][1]);
    sgEps[2] = sqrt(Dra[2][2]);
*/
    Eps[0] = Z[0];
    Eps[1] = Z[1];
    Eps[2] = Z[2];
    Eps[3] = Z[3];
    Eps[4] = Z[4];
    Eps[5] = Z[5];

    sgEps[0] = sqrt(Dde[0][0]);
    sgEps[1] = sqrt(Dde[1][1]);
    sgEps[2] = sqrt(Dra[2][2]);
    sgEps[3] = sqrt(Dra[3][3]);
    sgEps[4] = sqrt(Dra[4][4]);
    sgEps[5] = sqrt(Dra[5][5]);

    return 0;
}

int vsfCount_lsm(double *ra, double *dec, double *dRa, double *dDe, int pointNum, int coefNum, double *sCoef, double *tCoef, double *sCoefSg, double *tCoefSg, double &xi)
{
    int i, j, k;

    double *A = new double[coefNum*pointNum*4];
    double *LD = new double[pointNum*2];
    double *W = new double[pointNum*2];
    double *Z = new double[coefNum*4];
/*
    double *Ara = new double[coefNum*pointNum*2];
    double *rRa = new double[pointNum];
    double *Wra = new double[pointNum];
    double *Zra = new double[coefNum*2];

    double *Ade = new double[2*pointNum*coefNum];
    double *rDe = new double[pointNum];
    double *Wde = new double[pointNum];
    double *Zde = new double[coefNum*2];
*/
    double* D = new double[4*pointNum*pointNum];
    //double Dra[coefNum*2][coefNum*2];
    //double Dde[coefNum*2][coefNum*2];
    double uwe;

    for(i=0; i<pointNum; i++)
    {
        LD[i] = dRa[i];
        LD[i+pointNum] = dDe[i];
        //rRa[i] = dRa[i];
        //rDe[i] = dDe[i];

        for(j=0; j<coefNum; j++)
        {
            k= (i*2*coefNum);
            A[k+j] =  TLJ(j+1, ra[i], dec[i]);//SLJ(j+1, ra[i], dec[i]);//sCoef[j]*SLJ(j+1, ra[i], dec[i]) + tCoef[j]*TLJ(j+1, ra[i], dec[i]);
            A[k+coefNum+j] = SLJ(j+1, ra[i], dec[i]);
            A[coefNum*pointNum*2+k+j] =   TBJ(j+1, ra[i], dec[i]);
            A[coefNum*pointNum*2+k+j+coefNum] = SBJ(j+1, ra[i], dec[i]);
            /*if(j==0)
            {
                qDebug() << QString("Ara: %1\t%2\n").arg(Ara[k+j*2],12, 'f', 8).arg(Ara[k+j*2+1],12, 'f', 8);
                qDebug() << QString("Ade: %1\t%2\n").arg(Ade[k+j*2],12, 'f', 8).arg(Ade[k+j*2+1],12, 'f', 8);
            }*/
        }
        W[i] = W[i+pointNum]  = 1.0;
    }


////////////////////////////////////////////////////////////////////
//    qDebug() << QString("Eps: %1\t%2\t%3\n").arg(rad2mas(Eps[0]),12, 'f', 8).arg(rad2mas(Eps[1]),12, 'f', 8).arg(rad2mas(Eps[2]),12, 'f', 8);


    //lsm(coefNum*2, pointNum, Zra, Ara, rRa, uweRa, &Dra[0][0], Wra);
    //qDebug() << QString("Zra: %1 +- %2\t%3 +- %4\t%5 +- %6\n").arg(rad2mas(Zra[0]),12, 'f', 8).arg(rad2mas(sqrt(Dra[0][0])),12, 'f', 8).arg(rad2mas(Zra[1]),12, 'f', 8).arg(rad2mas(sqrt(Dra[1][1])),12, 'f', 8).arg(rad2mas(Zra[2]),12, 'f', 8).arg(rad2mas(sqrt(Dra[2][2])),12, 'f', 8);
    //qDebug() << QString("uweRa: %1\n").arg(rad2mas(sqrt(uweRa)),12, 'f', 8);

    //lsm(coefNum*2, pointNum, Zde, Ade, rDe, uweDe, &Dde[0][0], Wde);
    //qDebug() << QString("Zde: %1 +- %2\t%3 +- %4\n").arg(rad2mas(Zde[0]),12, 'f', 8).arg(rad2mas(sqrt(Dde[0][0])),12, 'f', 8).arg(rad2mas(Zde[1]),12, 'f', 8).arg(rad2mas(sqrt(Dde[1][1])),12, 'f', 8);
    //qDebug() << QString("uweDe: %1\n").arg(rad2mas(sqrt(uweDe)),12, 'f', 8);
    lsm(coefNum*2, pointNum*2, Z, A, LD, uwe, D, W);

    for(i=0;i<coefNum;i++)
    {
        tCoef[i] = Z[i];
        tCoefSg[i] = sqrt(D[i*coefNum+i]);

        sCoef[i] = Z[i+coefNum];
        sCoefSg[i] = sqrt(D[(i+coefNum)*coefNum+(i+coefNum)]);

        qDebug() << QString("coef[%1]: %2\t%3\n").arg(indexesStr(i)).arg(rad2mas(tCoef[i])).arg(rad2mas(sCoef[i]));
        //qDebug() << QString("coefDE[%1]: %2\t%3\n").arg(indexesStr(i)).arg(rad2mas(Zde[i*2])).arg(rad2mas(Zde[i*2+1]));
    }
    xi=sqrt(uwe);
    return 0;
}

int vsfCount(double *ra, double *dec, double *dRa, double *dDe, int pointNum, double *sCoef, double *tCoef, int coefNum, double &sigmaVal)
{
    int i, j;

    for(j=0; j<coefNum; j++)
    {
    tCoef[j] = 0;
    sCoef[j] = 0;
    }

    for(i=0; i<pointNum; i++)
    {
        for(j=0; j<coefNum; j++)
        {
            //indexes(j, n, k, p);
            /*
            Eps[0] += (dRa[i]*TL(1, 1, 1, ra[i], dec[i])+dDe[i]*TB(1, 1, 1,ra[i], dec[i]));//*cos(dec[i]);
            Eps[1] += (dRa[i]*TL(1, 1, 0, ra[i], dec[i])+dDe[i]*TB(1, 1, 0,ra[i], dec[i]));//*cos(dec[i]);
            Eps[2] += (dRa[i]*TL(1, 0, 1, ra[i], dec[i])+dDe[i]*TB(1, 0, 1,ra[i], dec[i]));//*cos(dec[i]);*/
            tCoef[j] += (dRa[i]*TLJ(j+1, ra[i], dec[i])+dDe[i]*TBJ(j+1,ra[i], dec[i]))*4*PI/pointNum;
            sCoef[j] += (dRa[i]*SLJ(j+1, ra[i], dec[i])+dDe[i]*SBJ(j+1,ra[i], dec[i]))*4*PI/pointNum;
        }
    }

    double *vsfRA, *vsfDE;
    vsfRA = new double[pointNum];
    vsfDE = new double[pointNum];
    double dra1, dde1;

    for(i=0; i<pointNum; i++)
    {
        dra1 = 0;
        dde1 = 0;
        for(j=0; j<coefNum; j++)
        {
            dra1 += sCoef[j]*SLJ(j+1, ra[i], dec[i]) + tCoef[j]*TLJ(j+1, ra[i], dec[i]);
            dde1 += sCoef[j]*SBJ(j+1, ra[i], dec[i]) + tCoef[j]*TBJ(j+1, ra[i], dec[i]);
        }
        vsfRA[i] = dRa[i] - dra1;
        vsfDE[i] = (dDe[i] - dde1);
    }

    double meanRa, meanDe, rmsOneRa, rmsOneDe, rmsMeanRa, rmsMeanDe;

    meanRa = meanDe = rmsOneRa = rmsOneDe = rmsMeanRa = rmsMeanDe = 0.0;

    for(i=0; i<pointNum; i++)
    {
        meanRa += vsfRA[i];
        meanDe += vsfDE[i];
    }

    meanRa /= pointNum;
    meanDe /= pointNum;

    for(i=0; i<pointNum; i++)
    {
        rmsOneRa += pow(vsfRA[i]-meanRa, 2.0);
        rmsOneDe += pow(vsfDE[i]-meanDe, 2.0);
    }
    rmsOneRa = sqrt(rmsOneRa/(pointNum-1));
    rmsOneDe = sqrt(rmsOneDe/(pointNum-1));

    rmsMeanRa = rmsOneRa/sqrt(pointNum);
    rmsMeanDe = rmsOneDe/sqrt(pointNum);

    double sigmaVal0 = rmsMeanRa/2.0*coefNum;
    double sigmaVal1 = rmsMeanDe/2.0*coefNum;


    sigmaVal = sqrt((sigmaVal0*sigmaVal0 + sigmaVal1*sigmaVal1))/2.0;

    qDebug() << QString("ra: %1\t%2\t%3\n").arg(rad2mas(meanRa)).arg(rad2mas(rmsOneRa), 10, 'e').arg(rad2mas(rmsMeanRa), 10, 'e');
    qDebug() << QString("de: %1\t%2\t%3\n").arg(rad2mas(meanDe)).arg(rad2mas(rmsOneDe), 10, 'e').arg(rad2mas(rmsMeanDe), 10, 'e');

    qDebug() << QString("sigma: %1\t%2\t:%3\n").arg(rad2mas(sigmaVal0)).arg(rad2mas(sigmaVal1)).arg(rad2mas(sigmaVal));
}

void ormlsm(double* L, double* B, double* MUL, double* MUB, int N, double* T, double* S, double* eT, double* eS, int ORD, double& xi)
{
    double* LD = new double[2*N];
    double* W = new double[2*N];
    double* C = new double[4*N*ORD];
    for(int i=0;i<N;i++) {LD[i]=MUL[i];W[i]=1;LD[i+N]=MUB[i];W[i+N]=1;}
    for(int i=0;i<N;i++)
    {
        for(int j=0;j<ORD;j++)
        {
            C[i*2*ORD+j]=TLJ(j+1,L[i],B[i]);C[i*2*ORD+j+ORD]=SLJ(j+1,L[i],B[i]);
            C[(i+N)*2*ORD+j]=TBJ(j+1,L[i],B[i]);C[(i+N)*2*ORD+j+ORD]=SBJ(j+1,L[i],B[i]);
        }
        //om(L[i],B[i],OML,OMB);
        //for(int j=0;j<11;j++){C[i*11+j]=OML[j];C[(i+N)*11+j]=OMB[j];}
    }
    double* Z = new double[2*ORD];
    double* D = new double[4*ORD*ORD];
    double uwe;
    lsm(2*ORD,2*N,Z,C,LD,uwe,D,W);
    for(int i=0;i<ORD;i++)
    {
        T[i]=Z[i];eT[i]=sqrt(D[i*ORD+i]);
        S[i]=Z[i+ORD];eS[i]=sqrt(D[(i+ORD)*ORD+(i+ORD)]);
    }
    xi=sqrt(uwe);
    delete[] LD;
    delete[] W;
    delete[] C;
    delete[] Z;
    delete[] D;
}
