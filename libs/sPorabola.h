#include "..\classes\rec.h"
#include "..\classes\ast.h"
#include "..\classes\orbit.h"
#include "comfunc.h"

#define N 100

struct eEq1Const
{
	double C1, C2 ,S12, S22, D1, D2, E, G;
};

struct AppConst
{
	double l1, m1, v1, t1;
	double l, m, v, t;
	double l2, m2, v2, t2;
	double t00;
};

struct ComonVar
{
	double n1, n2;
	double K, L1, L2, L3;
	double M, m;
	double tau, tau1, tau2;

	double mu1, mu, mu2;

	double ro1[N], ro2[N], r1[N], r2[N], s2[N], f[N][N];
	int last;
	double psnaccR, psnaccD;
	double psaccR, psaccD;
	double extaccR, extaccD;

	double x, y, z;
	double x1, y1, z1;
	double x2, y2, z2;

	double mx, my, mz, nx, ny, nz;

	double initpo1, initpo2;
};

class sPorabola
{
public:
	FILE *FLOG;

	char *fini;

	rec *sunpos;
	ast *apppos;
	orbit *objorb;

	eEq1Const *eeq1cnt;
	AppConst *appcnt;
	ComonVar *comvar;

	sPorabola();

	int compute(char *fast, char *frec, char *fini, char *forb, int iFlog, int MClog, int FAlog, int SAlog, int TAlog, int PSNlog, int COlog);

	int initFiles(char *fast, char *frec, int log);

//private:
	int makeConst(int log);
	int indexMm();
	int initC();

	int ferstApp(int log);
	int secondApp(int log);
	int thirdApp(int log);

	int comComp(double p1, double p2, int log);
	int comComp1(double p1, double p2, int log);
	int comComp2(int n1, int n2, int log);


	int eilerEq1(double *f, double *r1, double *r2, double *s2, double po1, double *po2, int log);

	double detpo2(double po1);
	double detO0(double c);

	int PSN(int log);

	int countOrb(int log);
	int countOrb1(int log);
	int countOrbBan(int log);

//	int detS(double *s, double B);

	int orbWrite(char *forb);

	int detR(double *r, double r1, double r2);

	int fulInfo();

	int int2pert(int *ip, char *pert);

//	int rdsys(double *aV, double *dV, double P1, double P2, double P3, int log);

	int soPost();
	int proofSM();
	int printOC(double aV, double dV, double a, double d, double t);

};