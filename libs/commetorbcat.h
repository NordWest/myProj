#ifndef COMETORBCAT_H
#define COMETORBCAT_H

#include "comfunc.h"
#include "filedynstr.h"

#define COMET_E5_CAT 195

class come5_rec
{
public:
	int number;		//nomer
	int epoha;		//эпоха [год]
	double eJD;		//Юлианские дни с эпохи
	double M0;		//эпоха перицентра, TT
	double w;		//долгота перицентра
	double Node;	//долгота восх узла
	double ec;		//эксцентриситет
	double inc;		//наклон орбиты
	double q;		//перигелийное растояние
	float H;		//абсолютная зв. величина
	float G;		//X
	char *perturb;	//от чего считалась пертурбация 1000000000000000
	int opp;		//число оппозиций
	int obs;		//число наблюдений
	int obsY;		//год наблюдений
	int regY;		//год внесения в каталог
	char *un;		//neznaiu
	char *name;		//имя объекта + Автор
	char *author;	//Автор
	char *makeD;	//Дата составления YYMMDD
	float diam;		//диаметр [km]

	char *tail;

	come5_rec();
	~come5_rec();

	void copyTo(come5_rec *nrec);

	void set_number(int numb);
	void set_epoha(int epo);
	void set_eJD(double eJD);
	void set_M0(double m0);
	void set_w(double w);
	void set_Node(double node);
	void set_ec(double ec);
	void set_inc(double inc);
	void set_q(double q);
	void set_H(float H);
	void set_G(float G);
	void set_pr(char *pr);
	void set_opp(int opp);
	void set_obs(int obs);
	void set_obsY(int obsY);
	void set_regY(int regY);
	void set_un(char *un);
	void set_name(char *name);
	void set_author(char *author);
	void set_makeD(char *makeD);
	void set_diam(float diam);

	void set_default();

};

class CommetOrbCat : public FileDynStrBin
{
public:
	come5_rec *record;

	void s2rec(char *str_in);
	void rec2s(char *str_out);

	int GetRec(int pos);
	int GetRecName(char *name);
//	int GetRecNum(int num);
	int AddRec(int pos);
	int AddRec();

	CommetOrbCat();
	CommetOrbCat(char *fn, int size);
	~CommetOrbCat();

	int init(char *fname);

//	int init(char *fname);
};

#endif
