#ifndef FBUFFER_H
#define FBUFFER_H

#include "string.h"
#include "stdlib.h"
#include "math.h"
#include "comfunc.h"
//#define STRLENBUFF 1024

class bufStrRec
{
public:
	char *str;
	int num;
	bufStrRec *next;

	bufStrRec(int strlen);
	~bufStrRec();
};

class bufStr
{
public:
	bufStrRec *record;

	int len;
	int strlen;

	bufStr(int strlen);
	~bufStr();

	void arrange();

	int getrec(char *gstr, int pos);
	int remrec(int pos);
	int setrec(char *str, int pos);
	int empty();
	int exchange(int i, int j);
	int update(char *gstr, int pos);
};

class fBuffer
{
public:
	char *fn, *fn_tmp;

	bufStr *buffer;

	int nstr;
	int sizestr;

	int is_buff;
	int is_edited;

	char *str;

	fBuffer();
	fBuffer(int sizestr);
	fBuffer(char *fn, int sizestr);
	~fBuffer();
	int init(char* fn, int sizestr);

	int AddStr(char *nstr, int num);
	int DelStr(int num);
//	int DelStr(char *pstr);
	int ReadStr(int num);
	int Exchange(int i, int j);
	int Update(char *nstr, int num);
	int Free();

	int Save();
	int SaveAs(char *fnname);
};

#endif
