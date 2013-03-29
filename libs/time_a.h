#ifndef COMF_H
#include "comfunc.h"
#endif


#ifndef TIMEA_H


class time_a
{

    double value;
public:

	/*
		type = 0 - UTC - Всемирное координированное время;
		type = 1 - UT = UT1 - Всемирное время от полуночи в средних солнечных сутках;
		type = 2 - UT0 - Всемирное время, не исправленное за движение полюса;
		type = 3 - TDB - динамическое барицентрическое время;
		type = 4 - TDT  - земное динамическое время.
		
	*/

	time_a();
	~time_a();

    double TDB();
    //double UT();
    double UTC();
    double TDT();

    void setTDB(double jdTime);
    //void setUT(double jdTime);
    void setUTC(double jdTime);
    void setTDT(double jdTime);
};

#define TIMEA_H
#endif
