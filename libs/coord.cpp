#include "stdafx.h"
#include "coord.h"

record4L::record4L()
{
	centre_name = new char[32];
	strcpy(centre_name, "");

	time_sys = -1;
	coord_sys = -1;
	coord_type = -1;
	pos = -1;
	dim = -1;

}

record4L::record4L(int dim)
{
	centre_name = new char[32];
	strcpy(centre_name, "");

	time_sys = -1;
	coord_sys = -1;
	coord_type = -1;

	pos = -1;

	x = new double[dim];
	dim = dim;
}

record4L::~record4L()
{
	delete [] centre_name;
}

record4L& record4L::operator=( record4L & source)
{
	int i, j;

	this->dim = source.dim;
	this->pos = source.pos;

	for(i=0; i<this->dim; i++) this->x[i] = source.x[i];

	strcpy(this->centre_name, source.centre_name);
	this->coord_sys = source.coord_sys;
	this->coord_type = source.coord_type;
	this->time_sys = source.time_sys;
	
	return *this;
}

List4::List4()
{
	this->dim = 4;
	this->num = 0;

	this->record = new record4L;
	this->record->pos = -1;
	this->record->x = new double[this->dim];
	this->record->dim = this->dim;
	this->root = new record4L;
	this->root->pos = -1;
	this->root->dim = this->dim;
	this->root->next = NULL;
}

List4::List4(int dim)
{
	this->record = new record4L;
	this->record->pos = -1;
	this->record->x = new double[dim];
	this->record->dim = dim;
	this->root = new record4L;
	this->root->pos = -1;
	this->root->next = NULL;
	this->root->dim = dim;

	this->dim = dim;
	this->num = 0;
}

List4::~List4()
{
	this->Free();

}

int List4::Add(double *x, int pos)
{
	record4L *next, *temp;
	next = this->root->next;
	temp = this->root;

	while(next!=NULL)
	{
		temp = next;
		next = next->next;

		if((pos>temp->pos)&&(pos<temp->pos)) break;
	}

	temp->next = new record4L;
	temp->next->pos = pos;
	temp->next->dim = dim;
	temp->next->next = next;
	temp->next->x = new double[this->dim];
	for(int i=0; i<this->dim; i++)
	{
		temp->next->x[i] = x[i];
	}

	strcpy(temp->next->centre_name, "");
	temp->next->coord_sys = -1;
	temp->next->coord_type = -1;
	temp->next->time_sys = -1;

	this->num++;
	
	return 0;
}

int List4::Add(double *x, int pos, char *oname, int time_sys, int coord_sys, int coord_type)
{
	record4L *next, *temp;
	next = this->root->next;
	temp = this->root;

	while(next!=NULL)
	{
		temp = next;
		next = next->next;

		if((pos>temp->pos)&&(pos<temp->pos)) break;
	}

	temp->next = new record4L;
	temp->next->pos = pos;
	temp->next->dim = dim;
	temp->next->next = next;
	temp->next->x = new double[this->dim];
	for(int i=0; i<this->dim; i++)
	{
		temp->next->x[i] = x[i];
	}

	strcpy(temp->next->centre_name, oname);
	temp->next->coord_sys = coord_sys;
	temp->next->coord_type = coord_type;
	temp->next->time_sys = time_sys;

	this->num++;
	
	return 0;
}



int List4::Get(double *x, int pos)
{
	record4L *next, *temp;
	next = this->root->next;
	temp = this->root;

	while((pos!=temp->pos)&&(next!=NULL))
	{
		temp = next;
		next = next->next;
	}

	if(pos!=temp->pos) return 1;

	for(int i=0; i<this->dim; i++)
	{
		x[i] = temp->x[i];
	}

	return 0;
}

int List4::Get(double *x, int pos, char *oname, int *time_sys, int *coord_sys, int *coord_type)
{
	record4L *next, *temp;
	next = this->root->next;
	temp = this->root;

	while((pos!=temp->pos)&&(next!=NULL))
	{
		temp = next;
		next = next->next;
	}

	if(pos!=temp->pos) return 1;

	for(int i=0; i<this->dim; i++)
	{
		x[i] = temp->x[i];
	}

	strcpy(oname, temp->centre_name);
	*time_sys = temp->time_sys;
	*coord_sys = temp->coord_sys;
	*coord_type = temp->coord_type;

	return 0;
}

int List4::Get(record4L *rec, int pos)
{
	if(rec==NULL) return 1;


	record4L *next, *temp;
	next = this->root->next;
	temp = this->root;

	while((pos!=temp->pos)&&(next!=NULL))
	{
		temp = next;
		next = next->next;
	}

	if(pos!=temp->pos) return 1;

	for(int i=0; i<this->dim; i++)
	{
		rec->x[i] = temp->x[i];
	}

	strcpy(rec->centre_name, temp->centre_name);
	rec->time_sys = temp->time_sys;
	rec->coord_sys = temp->coord_sys;
	rec->coord_type = temp->coord_type;

	return 0;
}

int List4::Update(double *x, int pos)
{
	record4L *next, *temp;
	next = this->root->next;
	temp = this->root;

	while((pos!=temp->pos)&&(next!=NULL))
	{
		temp = next;
		next = next->next;
	}

	if(pos!=temp->pos) return 1;

	for(int i=0; i<this->dim; i++) temp->x[i] = x[i];

	return 0;
}

int List4::Update(double *x, int pos, char *oname, int time_sys, int coord_sys, int coord_type)
{
	record4L *next, *temp;
	next = this->root->next;
	temp = this->root;

	while((pos!=temp->pos)&&(next!=NULL))
	{
		temp = next;
		next = next->next;
	}

	if(pos!=temp->pos) return 1;

	for(int i=0; i<this->dim; i++) temp->x[i] = x[i];

	strcpy(temp->centre_name, oname);
	temp->time_sys = time_sys;
	temp->coord_sys = coord_sys;
	temp->coord_type = coord_type;

	return 0;
}

int List4::Update(record4L *rec, int pos)
{
	if(rec==NULL) return 1;


	record4L *next, *temp;
	next = this->root->next;
	temp = this->root;

	while((pos!=temp->pos)&&(next!=NULL))
	{
		temp = next;
		next = next->next;
	}

	if(pos!=temp->pos) return 1;

	for(int i=0; i<this->dim; i++)
	{
		temp->x[i] = rec->x[i];
	}

	strcpy(temp->centre_name, rec->centre_name);
	temp->time_sys = rec->time_sys;
	temp->coord_sys = rec->coord_sys;
	temp->coord_type = rec->coord_type;

	return 0;
}

int List4::Del(int pos)
{
	if(pos>this->num-1) return 1;

	record4L *next, *temp;
	next = this->root->next;
	temp = this->root;



	while(next!=NULL)
	{
		if(pos==next->pos) break;
		temp = next;
		next = next->next;
		
	}

	if(pos!=next->pos) return 1;

	delete [] temp->next->x;
	delete [] temp->next->centre_name;

	temp->next = next->next;

	this->num--;
	return 0;
}

int List4::Push(double *x, char *oname, int time_sys, int coord_sys, int coord_type)
{
	return(this->Add(x, this->num, oname, time_sys, coord_sys, coord_type));
}

int List4::GetNum()
{
	return this->num;
}

int List4::GetDim()
{
	return this->dim;
}

int List4::Free()
{
	int i;
	for(i=num-1; i>=0; i--)
	{
		if(this->Del(i)) return i+1;
	}

	return 0;
}