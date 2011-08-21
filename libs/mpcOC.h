#include "fbStream.h"
#include "comfunc.h"
#include "DynArr.h"

#define mpcOC_LEN 103
#define mpcOC_NAME_KPOS 0

struct recOC
{
	double mJD, ra, de, phase, ocRa, ocDe, ocMuRa, ocMuDe;
	
};

class mpcOC : public fbStreamLine
{
public:
	recOC *record;
	char *sep;

	void s2rec(char *str_in);
	void rec2s(char *str_out);

	int GetRec(int pos);
//	int GetRecName(char *name);
//	int GetRecNum(char *num, int keypos);
	int AddRec(int pos);
	int AddRec();

	mpcOC();
	mpcOC(char *fn);
	~mpcOC();

	int init(char *fname);
};