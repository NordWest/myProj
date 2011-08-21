
class cmnk  
{
public:
	cmnk();
	virtual ~cmnk();
	void initmnk(int km, int nvar);
	void setel(int km, int nvar,long double el);
	void  slvls2(int s);
	long double * sgmnk(int km, int sn);
	long double* ptlst(int km, int sn);

	long double ** a;
	long double ** c;
	long double * xf;
	long double * sgm;
	int mkm,mnvar;



};


