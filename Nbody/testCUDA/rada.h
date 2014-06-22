struct ever_params
{
    int NV, NCLASS, NOR, NI, LL;
    double XL, col, vout;
    char jkeys[10];
    int ppn;
};

struct nbobjStruct
{
    double mass;
    int planet_num;
};

class Everhardt
{
 public:
	int NOR;
	int NCLASS; // 1 - for )st order eq; 2 - for 2nd order
		    //			    -2 - no first derivateves
	int NV;     // number of dependent variables
	int LL;
	float XL;

	Everhardt(int, int nclass=2, int nor=17, int ni=2, int ll=9, float xl=1.0e-9);
	~Everhardt(void);
	void rada27(double *, double *, double, double);
//	contra(double);
	void force(double *, double *, double, double *);

protected:
	int N_CLASS;
	int NI;
	int NW[14];
	int MC[12];
	double *BT;
	double *BE;
	double TDIF;
	double DIR;
	double PW;
	double TP;
	double SR;
	double W1;
	double W[13];
	double U[13];
	double C[78];
	double D[78];
	double R[78];
	double XI[78];
	double H[14];
	double *F1;
	double *FJ;
	double *Y;
	double *Z;
	double *B;
/*	double *BE;
	double *BT;*/

	int NF;
	int KD;
	int KF;
	int KE;
	int NPQ;
	int NSF;
	int NES;
	int NCL;
	int NPER;

	int NCOUNT;
	int NL;
	double TM;
	int NS;
        double SM;
	double SS;
	int rada_maker(double *, double *, double);
};
