#include "stdafx.h"
#include "time_a.h"


time_a::time_a()
{
}

time_a::~time_a()
{
}

int time_a::conv2TDB()
{
	return 0;
}

int time_a::conv2UT()
{
	return 0;
}

int time_a::conv2UTC()
{
	if(this->type==1) this->value = this->value - (32.184+TAIDUTC)/86400.0;
	else return 1;

	return 0;
}

int time_a::conv2TDT()
{
	if(this->type==0) this->value = this->value + (32.184+TAIDUTC)/86400.0;
	else return 1;


	this->type = 1;
	return 0;
}
