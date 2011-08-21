#include "orbit.h"

struct orbits
{
	orbit *orb;
	orbits *next;
};

class catalog
{
public:
	orbits *obs;
	int recn;

	catalog();
	catalog(char *fname);
	~catalog();

	int initFile(char *fname);
};