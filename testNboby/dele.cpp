//#include "stdafx.h"
#include "dele.h"

int planet_num(char *pname)
{
	if(strstr(pname, "Mercury")) return MERCURY_NUM;
	if(strstr(pname, "Venus")) return VENUS_NUM;
	if(strstr(pname, "Earth")) return EARTH_NUM;
	if(strstr(pname, "Mars")) return MARS_NUM;
	if(strstr(pname, "Jupiter")) return JUPITER_NUM;
	if(strstr(pname, "Saturn")) return SATURN_NUM;
	if(strstr(pname, "Uranus")) return URANUS_NUM;
	if(strstr(pname, "Neptune")) return NEPTUNE_NUM;
	if(strstr(pname, "Pluto")) return PLUTO_NUM;
	if(strstr(pname, "Moon")) return MOON_NUM;
	if(strstr(pname, "Sun")) return SUN_NUM;
	if(strstr(pname, "Nutations")) return NUTATIONS_NUM;
	if(strstr(pname, "Librations")) return LIBRATIONS_NUM;
	if(strstr(pname, "Geocentr")) return GEOCENTR_NUM;

	return -1;
}

double det_planet_H(int pl_num)
{
	switch(pl_num)
	{
	case MERCURY_NUM:
		return 0.162;
		break;
	case VENUS_NUM:
		return -1.472;
		break;
	case EARTH_NUM:
		return 3.000;
		break;
	case MARS_NUM:
		return -3.501;
		break;
	case JUPITER_NUM:
		return -5.970;
		break;
	case SATURN_NUM:
		return -5.306;
		break;
	case URANUS_NUM:
		return -0.621;
		break;
	case NEPTUNE_NUM:
		return 0.209;
		break;
	case PLUTO_NUM:
		return 7.118;
		break;
	default:
		return 3.0;
		break;
	}
}
/*

//1010
group1010::group1010()
{
	eph_name = new char[FNLEN];
	eph_begin = new char[FNLEN];
	eph_end = new char[FNLEN];
}

group1010::~group1010()
{
	delete [] eph_name;
	delete [] eph_begin;
	delete [] eph_end;
}

///////////////////////////////////////////

rec1040::rec1040()
{
	this->name = new char[FNLEN];
}

rec1040::~rec1040()
{
	delete [] this->name;
	this->name = NULL;
}

Mrec1040::Mrec1040()
{
	this->top = new rec1040;
	this->next = NULL;
}

Mrec1040::~Mrec1040()
{
	delete(this->top);
	this->top = NULL;
	this->next = NULL;
}

group1040::group1040()
{
	root = new Mrec1040;
	num = 0;
}

group1040::~group1040()
{
	delete(root);
	root = NULL;
}

int group1040::addElem(rec1040 *npar)
{
	Mrec1040 *troot, *tnew;

	troot = root;

	while(root->next!=NULL)
	{
		root = root->next;
	}

	tnew = new Mrec1040;
	tnew->next = NULL;
	tnew->top->value = npar->value;
	strncpy(tnew->top->name, npar->name, 32);
	
	root->next = tnew;

	root = troot;

	this->num++;
	
	return 0;
}

int group1040::remElem(int num)
{
	if(num>=this->num) return 1;

	Mrec1040 *troot, *tnext;
	troot = root;
	tnext=NULL;

	int i = 0;
	while((i<num)&&root!=NULL)
	{
		root = root->next;
		i++;
	}

	if(root==NULL) return 1;

	if(root->next!=NULL) tnext=root->next->next;

	delete(root->next);

	root->next = tnext;
	root = troot;

	this->num--;



	return 0;
}

int group1040::getElem(rec1040 *nrec, int pos)
{
	if(pos>this->num-1) return 1;
	if(pos<0) return 1;
	if(nrec==NULL)return 1;
	if(nrec->name==NULL) return 1;

	Mrec1040 *troot, *tnext;
	troot = root;
	tnext=NULL;

	int i = 0;
	while((i<=pos)&&troot->next!=NULL)
	{
		troot = troot->next;
		i++;
	}

	nrec->value = troot->top->value;
	strcpy(nrec->name, troot->top->name);

	return 0;

}

int group1040::getElemByName(rec1040 *nrec, char *name)
{
	if(nrec==NULL)return -1;
	if(nrec->name==NULL) return -1;
	if(name==NULL)return -1;

	Mrec1040 *troot, *tnext;
	troot = root;
	tnext=NULL;

	int pos = -1;

	while((pos<num)&&strstr(troot->top->name, name)==NULL)
	{
		troot = troot->next;
		pos++;
	}

	if(pos>this->num-1) return -1;

	nrec->value = troot->top->value;
	strcpy(nrec->name, troot->top->name);

	return pos;
}

int group1040::clear()
{
	int nnn;

	for(nnn=this->num-1;nnn>=0;nnn--)
	{
		if(this->remElem(nnn)) return 1;
	}

	return 0;
}


group1050::group1050()
{
	this->coef = new mymatrix(3, 39);
}
group1050::~group1050()
{
	delete(coef);
}


group1070::group1070()
{
//	this->fbin = new FileDynStrBin();
	this->fname = new char[FNLEN];

}

group1070::group1070(char *fnbin)
{
//	this->fbin = new FileDynStrBin();
	this->fname = new char[FNLEN];


}

group1070::~group1070()
{
	delete [] this->fname;
//	delete(this->fbin);
}

int group1070::convert2bin(char *asc_name, char *bin_name)
{
	FileDynStrBin *fasc;
	fasc = new FileDynStrBin(asc_name, 81);
//	fasc->init(asc_name, 80);

	FILE *FASC, *FBIN;

	if(asc_name==NULL) return 1;

	if(bin_name==NULL) return 2;

	strncpy(bin_name, asc_name, FNLEN);
	strcat(bin_name, ".bin\0");
/*
	FASC = fopen(asc_name, "r");
        if(FASC==NULL) return 1;
/
	FBIN = fopen(bin_name, "wb+");
	if(FBIN==NULL) return 3;

	char *strmove;
	strmove = new char[1000];
	int i0, i1;
	double x[3];
	int i;

	


	i=0;
	int cgran, gpos;
	int k;

	

	gpos = 0;
	cgran = 0;

	while(!fasc->ReadStr(i))
	{
		if(cgran==0)
		{
			sscanf(fasc->str, "%d %d", &i0, &i1);
			fwrite(&i0, sizeof(int), 1, FBIN);
			fwrite(&i1, sizeof(int), 1, FBIN);
			cgran = i1;

		}
		else
		{

			sscanf(fasc->str, "%lf %lf %lf", &x[0], &x[1], &x[2]);

			for(k = 0; k<3; k++)
			{
				fwrite(&x[k], sizeof(double), 1, FBIN);
				gpos++;
				if(gpos==cgran)
				{
					gpos=0;
					cgran = 0;
					break;
				}
			}
		}
//		fwrite(&x1, sizeof(double), 1, FBIN);
//		fwrite(&x2, sizeof(double), 1, FBIN);

		i++;
	}


//	fclose(FASC);
	fclose(FBIN);
	return 0;
}
/*
int group1070::get(int nrecord, int nplan, int ngran)
{
	FILE *fbin;
	fbin = fopen(this->fname, "rb");
	fseek(fbin, (sizeof(int)*2 + sizeof(double)*1080)*(nrecord-1) + 
	return 0;
}
*/

///////////////////////////////////////////

dele::dele()
{
    setlocale(LC_NUMERIC, "C");
//	this->fname_h = new char[FNLEN];
//	strcpy(this->fname_h, "");

	this->fname_bin = new char[FNLEN];
	strcpy(this->fname_bin, "");

	g1010 = new group1010;
	g1030 = new group1030;
	g1040 = new group1040;
	g1041 = new group1040;
	g1050 = new group1050;
	g1070 = new group1070;

//	this->init_header(this->fname_h);
}

dele::dele(const char *fname)
{
    dele();
        //this->fname_h = new char[FNLEN];
        //strncpy(this->fname_h, fname, FNLEN);

	this->fname_bin = new char[FNLEN];
        strncpy(this->fname_bin, fname, FNLEN);
        //strcpy(this->fname_bin, "ascp2000.405\0");

	g1010 = new group1010;
	g1030 = new group1030;
	g1040 = new group1040;
	g1041 = new group1040;
	g1050 = new group1050;
	g1070 = new group1070;

        this->init_jpl_bin(this->fname_bin);
}

dele::dele(const char *fname_h, const char *fname_bin)
{
	this->fname_h = new char[FNLEN];
	strcpy(this->fname_h, fname_h);

	this->fname_bin = new char[FNLEN];
	strcpy(this->fname_bin, fname_bin);

	g1010 = new group1010;
	g1030 = new group1030;
	g1040 = new group1040;
	g1041 = new group1040;
	g1050 = new group1050;
	g1070 = new group1070;

	this->init_header(this->fname_h);
        this->init_jpl_bin(this->fname_bin);
}


dele::~dele()
{
        delete(g1010);
	delete(g1030);
	delete(g1040);
	delete(g1041);
	delete(g1050);
        delete(g1070);
}

int dele::init_header(const char *fname_h)
{
	strcpy(this->fname_h, fname_h);
	int i;
	char *sepstr;
	sepstr = new char[STRLEN];
	char *sepstr1;
	sepstr1 = new char[STRLEN];
	int d1, d2;
	FileDynStr *fil;

	int len, pos0, pos1;
	rec1040 *r1040;
	r1040 = new rec1040;




	fil = new FileDynStr(this->fname_h, 1024);

	if(fil->nstr==0) return 1;

	i=0;

	while(!fil->ReadStr(i))
	{
		strncpy(sepstr, fil->str, 5);
		strcpy(&sepstr[5], "\0");
		if(strstr(sepstr, "KSIZE"))
		{
			sscanf(fil->str, "%s %d %s %d", sepstr, &d1, sepstr1, &d2);
			this->KSize = d1;
			this->Ncoeff = d2;
			i++;
		}
		else if(strstr(sepstr, "GROUP"))
		{
			strncpy(sepstr, fil->str, 12);
			strcpy(&sepstr[12], "\0");

			i+=2;

			if(strstr(sepstr, "GROUP   1010"))
			{
				if(fil->ReadStr(i)) return 2;
				strcpy(this->g1010->eph_name, fil->str);

				i++;

				if(fil->ReadStr(i)) return 2;
				strcpy(this->g1010->eph_begin, fil->str);

				i++;

				if(fil->ReadStr(i)) return 2;
				strcpy(this->g1010->eph_end, fil->str);
				
			}

			if(strstr(sepstr, "GROUP   1030"))
			{
				if(fil->ReadStr(i)) return 2;
                                //sscanf(fil->str, "%f %f %f", &g1030->eph_begin, &g1030->eph_end, &g1030->eph_step);
                                pos0 = 0;
                                slovo(fil->str, sepstr1, pos0, &pos1);
                                pos0 = pos1;
                                g1030->eph_begin = atof(sepstr1);
                                slovo(fil->str, sepstr1, pos0, &pos1);
                                pos0 = pos1;
                                g1030->eph_end = atof(sepstr1);
                                slovo(fil->str, sepstr1, pos0, &pos1);
                                pos0 = pos1;
                                g1030->eph_step = atof(sepstr1);


			}

			if(strstr(sepstr, "GROUP   1040"))
			{
				i++;

//				if(fil->ReadStr(i)) return 2;

				
				while(!fil->ReadStr(i))
				{
					len = strlen(fil->str);
					if(len<3) break;

					pos0 = 0;
//					pos1 = 4;

					while(!slovo(fil->str, sepstr1, pos0, &pos1))
					{
						strcpy(r1040->name, sepstr1);
						this->g1040->addElem(r1040);
						
						pos0 = pos1+1;
					}
					i++;
				};


				i--;


			}

			if(strstr(sepstr, "GROUP   1041"))
			{
				i++;

//				if(fil->ReadStr(i)) return 2;

				
				while(!fil->ReadStr(i))
				{
					len = strlen(fil->str);
					if(len<3) break;

					pos0 = 0;
//					pos1 = 4;

					while(!slovo(fil->str, sepstr1, pos0, &pos1))
					{
						r1040->value = atof(sepstr1);
						this->g1041->addElem(r1040);
						
						pos0 = pos1+1;
					}
					i++;
				};

				i--;


			}
			if(strstr(sepstr, "GROUP   1050"))
			{
//				i++;

//				if(fil->ReadStr(i)) return 2;
				int k, l;

				k = 0;


				
				while(!fil->ReadStr(i))
				{
					len = strlen(fil->str);
					if(len<3) break;

					pos0 = 0;
//					pos1 = 4;

					l = 0;

					while(!slovo(fil->str, sepstr1, pos0, &pos1))
					{
						this->g1050->coef->set(k, l, atof(sepstr1));
						
						pos0 = pos1+1;
						l++;
					}
					k++;
					i++;
				};

				i--;

			}

			if(strstr(sepstr, "GROUP   1070"))
			{
			}
			
			i++;


		}
		else return 1;
//		strncpy(sepstr, fil->str, 5);
//		strcpy(&sepstr[5], "\0");



		i++;
	}

//	delete [] sepstr;
//	delete [] sepstr1;
//	delete rec1040;

	return 0;
}

int dele::init_jpl(const char *jpl_name)
{
	strcpy(this->fname_bin, jpl_name);
	this->g1070 = new group1070;
	strcpy(this->g1070->fname, this->fname_bin);

	return 0;
}

int dele::init_jpl_bin(const char *jpl_name)
{
        strcpy(this->fname_bin, jpl_name);
        //this->g1070 = new group1070;
        //strcpy(this->g1070->fname, this->fname_bin);

        return 0;
}

int dele::detR(double *x, double *y, double *z, double Time, char *planet, int proizv, int centr, int sk)
{
	int nplanet = planet_num(planet);
	if(nplanet<0) return 1;
/*
        double *Position = new double[3];

        Initialize_Ephemeris(fname_bin);

        stateData State;

        if(proizv)
        {
            Interpolate_State( Time , nplanet , &State );
            *x = State.Position[0];
            *y = State.Position[1];
            *z = State.Position[2];
        }
        else
        {
            Interpolate_Position( Time , nplanet , Position );
            *x = Position[0];
            *y = Position[1];
            *z = Position[2];
        }
  */

	return(detR(x, y, z, Time, nplanet, proizv, centr, sk));
}

int dele::detRtt(double *x, double *y, double *z, double Time, int nplanet, int centr, int sk)
{
	List4 *cs;
	cs = new List4(7);
	double xk[7], xk1[7];
//	xk = new double[11][6];
        //rec1040 *rc;
        //rc = new rec1040;
	int npl;

	int i;
	for(i=0; i<11; i++)
	{
		detR(&xk[0], &xk[1], &xk[2], Time, i, 0, centr, sk);
		detR(&xk[3], &xk[4], &xk[5], Time, i, 1, centr, sk);
                //this->g1041->getElem(rc, 8+i);
                xk[6] = H2.data.constValue[8+i];//rc->value;
		cs->Add(xk, i);
	}

	detR(&xk[0], &xk[1], &xk[2], Time, nplanet, 0, centr, sk);
	detR(&xk[3], &xk[4], &xk[5], Time, nplanet, 1, centr, sk);

	npl = nplanet;
	if(nplanet==13) npl = 2;
        //this->g1041->getElem(rc, 8+npl);
        xk[6] = H2.data.constValue[8+npl];//rc->value;
	cs->Add(xk, 11);

	cs->Get(xk, 9);
	cs->Get(xk1, 11);

	double rmod1;

	rmod1 = sqrt(xk1[0]*xk1[0] + xk1[1]*xk1[1] + xk1[2]*xk1[2]);
/*
	*x = -(xk[6] + xk1[6])*xk1[0]/pow(fabs(xk1[0]), 3.0);
	*y = -(xk[6] + xk1[6])*xk1[1]/pow(fabs(xk1[1]), 3.0);
	*z = -(xk[6] + xk1[6])*xk1[2]/pow(fabs(xk1[2]), 3.0);
*/
//	rmod1 = pow(rmod1, 3.0);
	*x = -(xk[6] + xk1[6])*xk1[0]/pow(rmod1, 3.0);
	*y = -(xk[6] + xk1[6])*xk1[1]/pow(rmod1, 3.0);
	*z = -(xk[6] + xk1[6])*xk1[2]/pow(rmod1, 3.0);

	double A[3];
	double c;
        //this->g1041->getElem(rc, 6);
        c = H2.data.constValue[6];//rc->value;

	double vmod1 = sqrt(xk1[3]*xk1[3] + xk1[4]*xk1[4] + xk1[5]*xk1[5]);
	

	for(i=0; i<3; i++) A[i] = xk[6]*((4.0*xk[6]/rmod1 - pow(vmod1, 2.0))*xk1[i+3] + 4.0*(xk1[i]*xk1[i+3])*xk1[i+3])/(c*c*pow(rmod1, 3.0));

	*x+=A[0];
	*y+=A[1];
	*z+=A[2];

	int k;

	double rmodij, rmodj;


	for(i=0; i<3; i++)
	{
		
		A[i] = 0.0;


		for(k=0; k<9; k++)
		{
			if(k==npl) continue;
			cs->Get(xk, k);
			rmodij = sqrt((xk[0]-xk1[0])*(xk[0]-xk1[0]) + (xk[1]-xk1[1])*(xk[1]-xk1[1]) + (xk[2]-xk1[2])*(xk[2]-xk1[2]));
			rmodj = sqrt(xk[0]*xk[0] + xk[1]*xk[1] + xk[2]*xk[2]);
			A[i] += xk[6]*((xk[i]-xk1[i])/pow(rmodij, 3.0)-(xk[i])/pow(rmodj, 3.0));
		}
	}

	*x+=A[0];
	*y+=A[1];
	*z+=A[2];


	return 0;
}


int dele::detR(double *x, double *y, double *z, double Time, int nplanet, int proizv, int centr, int sk)
{
    double xt, yt, zt, rznorm, xtnorm;
    double Em;
    int npl = 0;
    if(nplanet==GEOCENTR_NUM)
    {
            npl = 1;
            nplanet = EARTH_NUM;
    }

    double *Position = new double[3];

    Initialize_Ephemeris(fname_bin);

    stateData State;

    if(proizv)
    {
        Interpolate_State( Time , nplanet , &State );
        *x = State.Position[0];
        *y = State.Position[1];
        *z = State.Position[2];
    }
    else
    {
        Interpolate_Position( Time , nplanet , Position );
        *x = Position[0];
        *y = Position[1];
        *z = Position[2];
    }

    if(npl)
    {
        xt = yt = zt = 0.0;
        Em = H1.data.EMRAT;
        //this->g1041->getElemByName(&unitRec, "EMRAT");
        //unitRec.value;
        //Em = 81.300559999999983223729606289923;
        Em = Em + 1.0;

        this->detR(&xt, &yt, &zt, Time, MOON_NUM, proizv, 0, 0);

//		printf("\nEarth-Moon:\t%17.15e %17.15e %17.15e\n", xt/AUKM, yt/AUKM, zt/AUKM);

        xtnorm = sqrt(xt*xt + yt*yt + zt*zt);
        rznorm = xtnorm/Em;

        xt = -xt/xtnorm*rznorm;
        yt = -yt/xtnorm*rznorm;
        zt = -zt/xtnorm*rznorm;

//		printf("%17.15e %17.15e %17.15e\n", xt/AUKM, yt/AUKM, zt/AUKM);

        *x += xt;
        *y += yt;
        *z += zt;
    }

    if(centr)
    {
            this->detR(&xt, &yt, &zt, Time, SUN_NUM, proizv, 0, 0);

            printf("sun_pos: %f %f %f\n", xt, yt, zt);

            *x -= xt;
            *y -= yt;
            *z -= zt;
    }

    if(sk)
    {
        double *vect = new double[3];

        //H1.data.
        vect[0] = *x;
        vect[1] = *y;
        vect[2] = *z;
            /*Vt->set(0, *x);
            Vt->set(1, *y);
            Vt->set(2, *z);*/

            RotX(vect, -EKV);


    }

    return 0;
}


/*
int dele::detR(double *x, double *y, double *z, double Time, int nplanet, int proizv, int centr, int sk)
{
	double *granul;
	double Em;
	double xt, yt, zt, rznorm, xtnorm;
	int vect_len;
	part_head ph;
	int npl = 0;

	myvector *Vt, *V;

	Vt = new myvector(3);
	V = new myvector(3);

	if(nplanet==GEOCENTR_NUM)
	{
		npl = 1;
		nplanet = EARTH_NUM;
	}

	vect_len = 3*this->g1050->coef->get(1, nplanet);
	granul = new double[vect_len];

	FILE *FB;

	FB = fopen(this->g1070->fname, "rb");
	if(FB==NULL) return 1;

	int shift_part, shift_pl, shift_gran, part_size, gran_size;
	double gran_step;

	part_size = 2*sizeof(int) + this->Ncoeff*sizeof(double);
	gran_size = 3.0*this->g1050->coef->get(1, nplanet)*sizeof(double);
	gran_step = ((int)(this->g1030->eph_step/this->g1050->coef->get(2, nplanet)));
	shift_part = (int)((Time - this->g1030->eph_begin)/this->g1030->eph_step);

	fseek(FB, part_size*shift_part, SEEK_SET);
        fread(&ph, sizeof(part_head), 1, FB);

	shift_pl = this->g1050->coef->get(0, nplanet) - 3;

	fseek(FB, shift_pl*sizeof(double), SEEK_CUR);

	shift_gran = (Time - ph.T0)/((int)gran_step);

	fseek(FB, shift_gran*gran_size, SEEK_CUR);
	fread(granul, gran_size, 1, FB);

	double T0 = ph.T0 + shift_gran*gran_step;
	double tm = 2.0*(Time - T0)/gran_step - 1.0;

	int gbeg = 0*this->g1050->coef->get(1, nplanet);
	if(!proizv) *x = Rx(tm, this->g1050->coef->get(1, nplanet), &granul[gbeg]);
	else
	{
		*x = Vx(tm, this->g1050->coef->get(1, nplanet), &granul[gbeg]);
		*x *= 2.0*this->g1050->coef->get(2, nplanet)/(this->g1030->eph_step);
	}

	gbeg = 1*this->g1050->coef->get(1, nplanet);
	if(!proizv) *y = Rx(tm, this->g1050->coef->get(1, nplanet), &granul[gbeg]);
	else
	{
		*y = Vx(tm, this->g1050->coef->get(1, nplanet), &granul[gbeg]);
		*y *= 2.0*this->g1050->coef->get(2, nplanet)/(this->g1030->eph_step);
	}

	gbeg = 2*this->g1050->coef->get(1, nplanet);
	if(!proizv) *z = Rx(tm, this->g1050->coef->get(1, nplanet), &granul[gbeg]);
	else
	{
		*z = Vx(tm, this->g1050->coef->get(1, nplanet), &granul[gbeg]);
		*z *= 2.0*this->g1050->coef->get(2, nplanet)/(this->g1030->eph_step);
	}

	fclose(FB);

	*x /= AUKM;
	*y /= AUKM;
	*z /= AUKM;

        rec1040 unitRec;


	if(npl)
	{

		
		xt = yt = zt = 0.0;

		

                this->g1041->getElemByName(&unitRec, "EMRAT");
                Em = unitRec.value;
                //Em = 81.300559999999983223729606289923;
		Em = Em + 1.0;

                this->detR(&xt, &yt, &zt, Time, MOON_NUM, proizv, 0, 0);

//		printf("\nEarth-Moon:\t%17.15e %17.15e %17.15e\n", xt/AUKM, yt/AUKM, zt/AUKM);
		
		xtnorm = sqrt(xt*xt + yt*yt + zt*zt);
		rznorm = xtnorm/Em;

		xt = -xt/xtnorm*rznorm;
		yt = -yt/xtnorm*rznorm;
		zt = -zt/xtnorm*rznorm;

//		printf("%17.15e %17.15e %17.15e\n", xt/AUKM, yt/AUKM, zt/AUKM);
		
		*x += xt;
		*y += yt;
		*z += zt;
	}

	if(centr)
	{
                this->detR(&xt, &yt, &zt, Time, SUN_NUM, proizv, 0, 0);

                printf("sun_pos: %f %f %f\n", xt, yt, zt);

		*x -= xt;
		*y -= yt;
		*z -= zt;
	}

	if(sk)
	{

		Vt->set(0, *x);
		Vt->set(1, *y);
		Vt->set(2, *z);
/*
		xt = *x;
		yt = *y;
		zt = *z;
/
		RotX(V, Vt, -EKV);
/*
		*x = xt;
		*y = cos(-EKV)*yt - sin(-EKV)*zt;
		*z = sin(-EKV)*yt + cos(-EKV)*zt;
/
		*x = V->get(0);
		*y = V->get(1);
		*z = V->get(2);

	}
	delete [] granul;
	delete V;
	delete Vt;

	return 0;
}
*/
