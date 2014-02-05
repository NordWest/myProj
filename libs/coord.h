//#pragma once
//	class Ccoord
#ifndef COORD_H
#define COORD_H

#include <string.h>

class record4L
{
public:
	double *x;
	int pos;
	int dim;

	char *centre_name;	//имя сентра			
	int time_sys;		//система времени:	-1 = неопределено	
	/*
		type = 0 - UTC - Всемирное координированное время;
		type = 1 - UT = UT1 - Всемирное время от полуночи в средних солнечных сутках;
		type = 2 - UT0 - Всемирное время, не исправленное за движение полюса;
		type = 3 - TDB - динамическое барицентрическое время;
		type = 4 - TDT  - земное динамическое время.
		
	*/
	int coord_sys;		//система координат:	1
	int coord_type;		//тип координат:		-1 = неопределено, 0 = сферическая, 1 = прямоугольная

	record4L *next;

	record4L();
	record4L(int dim);
	~record4L();

	record4L& operator=( record4L & source);  

};

class List4
{
	record4L *record;
	record4L *root;

//	List4 *nextL;

	int num;
	int dim;

public:
	List4();
	List4(int dim);
	~List4();

	int Add(double *x, int pos);
	int Add(double *x, int pos, char *oname, int time_sys, int coord_sys, int coord_type);
	int Push(double *x, char *oname = "", int time_sys = -1, int coord_sys = -1, int coord_type = -1);

	int Get(double *x, int pos);
	int Get(double *x, int pos, char *oname, int *time_sys, int *coord_sys, int *coord_type);
	int Get(record4L *rec, int pos);

	int Update(double *x, int pos);
	int Update(double *x, int pos, char *oname, int time_sys, int coord_sys, int coord_type);
	int Update(record4L *rec, int pos);

	int Del(int pos);

	int GetNum();
	int GetDim();

	int Free();

};
/*
class ComCoords
{
	List4 *clist;


	ComCoords();
	~ComCoords();
};
*/

#endif
