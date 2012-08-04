#include "catalog.h"
#include "stdio.h"
//#include "orbit.h"

//#include "..\DAster\stdafx.h"
//#include "..\DAster\DAster.h"


catalog::catalog()
{
	this->obs = NULL;
	this->recn = 0;
}

catalog::catalog(char *fname)
{
	this->obs = NULL;
	this->recn = 0;
}

catalog::~catalog(char *fname)
{
	this->obs = NULL;
	this->recn = 0;
}
