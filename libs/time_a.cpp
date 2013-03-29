#include "stdafx.h"
#include "time_a.h"


time_a::time_a()
{
    value = 0.0;
}

time_a::~time_a()
{
}


double time_a::TDB()
{
    return(value);
}

//double time_a::UT();
double time_a::UTC()
{
    double jdUTC;
    TDB2UTC(value, &jdUTC);
    return(jdUTC);
}

double time_a::TDT()
{
    return(TDB2TDT(value));
}

void time_a::setTDB(double jdTime)
{
    value =jdTime;
}

//void time_a::setUT(double jdTime);
void time_a::setUTC(double jdTime)
{
    UTC2TDB(jdTime, &value);
}

void time_a::setTDT(double jdTime)
{
    value = TDT2TDB(jdTime);
}
