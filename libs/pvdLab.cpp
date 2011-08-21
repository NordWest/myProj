#include "stdafx.h"
#include "pvdLab.h"

pvdLab::pvdLab()
{
	this->ctype = -1;
	this->fn_odata = new char[255];
	this->fn_suff = new char[255];
	this->is_coords_loaded = 0;
	this->obsdata = new List4;
	this->orbs = new OrbCat;

}