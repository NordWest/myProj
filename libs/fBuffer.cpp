//#include "stdafx.h"

#include "fBuffer.h"

bufStrRec::bufStrRec(int strlen)
{
	this->str = new char[STRLEN];
	this->next = NULL;
}

bufStrRec::~bufStrRec()
{
	delete [] str;
}

bufStr::bufStr(int strlen)
{
	this->record = NULL;
	this->record = new bufStrRec(STRLEN);
	this->strlen = strlen;
	this->len = 0;
}

bufStr::~bufStr()
{
	this->empty();
	len = 0;
}

int bufStr::getrec(char *gstr, int pos)
{
	bufStrRec *tmp;

	if(pos<0) return 1;
	
	tmp = this->record;
	int i = -1;
	int fn = 0;

	while((tmp!=NULL)&&i<len)
	{
		i++;
		if(tmp->num==pos)
		{
			fn = 1;
			break;
		}
		tmp = tmp->next;
	}

	if(!fn) return 1;

	strcpy(gstr, tmp->str);
//	record->num = pos;

	return 0;
}

int bufStr::remrec(int pos)
{
	bufStrRec *tcur, *tprev;

	if(pos<0||pos>=this->len) return 1;
	
	tcur = this->record;
	int i = 0;
	int fn = 0;
/*
	tnew = new bufStrRec(1024);

	strcpy(tnew->str, str);
	strcpy(&tnew->str[strlen], "\0");
	tnew->num = pos;
*/
	if(pos==0)
	{
		this->record = tcur->next;
//		this->record->next = tmp;
	}
	else
	{
		while((tcur->next!=NULL)&&i<len)
		{
			
			if(i==pos-1)
			{
				fn = 1;
				tcur->next = tcur->next->next;
//				tmp->next = tnew;
				break;
			}
			tcur = tcur->next;
			i++;
		}
		if(!fn) return 1;
	}
	this->arrange();

	this->len--;

	return 0;

}

int bufStr::setrec(char *str, int pos)
{
	bufStrRec *tmp, *tnew;

	if(pos<0) return 1;
	
	tmp = this->record;
	int i = -1;
	int fn = 0;

	tnew = new bufStrRec(this->strlen);

	strncpy(tnew->str, str, strlen);
	strcpy(&tnew->str[strlen], "\0");
	tnew->num = pos;

	if((tmp==NULL)||(pos==0))
	{
		this->record = tnew;
		this->record->next = tmp;
	}
	else
	{
		while((tmp!=NULL)&&i<len)
		{
			i++;
			if(i==pos-1)
			{
				fn = 1;
				tnew->next = tmp->next;
				tmp->next = tnew;
				break;
			}
			tmp = tmp->next;
			
		}
		if(!fn) return 1;
	}
	this->arrange();

	this->len++;

	return 0;
}

void bufStr::arrange()
{
	int i = 0;
	bufStrRec *tmp;

	tmp = this->record;

	while(tmp!=NULL)
	{
		tmp->num = i;
		tmp = tmp->next;
		i++;
	}
}

int bufStr::empty()
{
	
	bufStrRec *tmp, *next;
	
	tmp = this->record;

	int i = 0;
	if(tmp!=0)
	{
	while(tmp->next!=NULL)
	{
		i++;
		next = tmp->next;
		delete tmp;
//		len--;
		tmp = next;
	}
	}
	len = 0;
	record = NULL;

	return 0;
}

int bufStr::exchange(int i, int j)
{
	bufStrRec *tmp, *pre, *tmp0, *pre0, *tmp1, *pre1;

	if((i<0)||(j<0)) return 1;
	
	pre = NULL;
	tmp = this->record;
	int k = -1;
	int fn0 = 0;
	int fn1 = 0;
	int scss = 0;

	while((tmp!=NULL)&&k<len)
	{
		k++;
		if(tmp->num==i)
		{
			fn0 = 1;
			tmp0 = tmp;
			pre0 = pre;
		}
		if(tmp->num==j)
		{
			fn1 = 1;
			tmp1 = tmp;
			pre1 = pre;
		}

		if(fn0&&fn1)
		{
			scss = 1;
			break;
		}
		pre = tmp;
		tmp = tmp->next;
	}

	if(!scss) return 1;

	switch(i-j)
	{
	case -1:
		if(pre0!=NULL) pre0->next = tmp1;
		else this->record = tmp1;
		tmp = tmp1->next;
		tmp1->next = tmp0;
		tmp0->next = tmp;
		break;
	case 1:
		if(pre1!=NULL) pre1->next = tmp0;
		else this->record = tmp0;
		tmp = tmp0->next;
		tmp0->next = tmp1;
		tmp1->next = tmp;
		break;
	default:
		tmp = tmp1->next;
		pre = pre1;
		
		if(pre0!=NULL) pre0->next = tmp1;
		else this->record = tmp1;
		tmp1->next = tmp0->next;

		if(pre1!=NULL) pre1->next = tmp0;
		else this->record = tmp0;
		tmp0->next = tmp;
		break;
	}

	tmp1->num = i;
	tmp0->num = j;

	return 0;
}

int bufStr::update(char *gstr, int pos)
{
	bufStrRec *tmp;

	if(pos<0) return 1;
	
	tmp = this->record;
	int i = -1;
	int fn = 0;

	while((tmp!=NULL)&&i<len)
	{
		i++;
		if(tmp->num==pos)
		{
			fn = 1;

			break;
		}
		tmp = tmp->next;
	}

	if(!fn) return 1;

	strcpy(tmp->str, gstr);
//	record->num = pos;

	return 0;

}

////////////////////	fBuffer	/////////////////////////////

fBuffer::fBuffer()
{
	this->str = new char[STRLEN];
	strcpy(str, "\0");
	this->fn = new char[255];
	this->fn_tmp = new char[255];
	this->buffer = NULL;
	this->sizestr = 0;
	this->nstr = 0;
	is_buff = 0;
}

fBuffer::fBuffer(int sizestr)
{
	this->str = new char[STRLEN];
	strcpy(str, "\0");
	this->fn = new char[255];
	this->fn_tmp = new char[255];
	this->buffer = new bufStr(sizestr);
	this->sizestr = sizestr;
	this->nstr = 0;
	is_buff = 0;
}

fBuffer::fBuffer(char *fn, int sizestr)
{
	this->str = new char[STRLEN];
	strcpy(str, "\0");
	this->fn = new char[255];
	this->fn_tmp = new char[255];
	this->sizestr = sizestr;
	strncpy(this->fn, fn, 255);
	strncpy(this->fn_tmp, fn, 255);
	strcat(this->fn_tmp, ".tmp\n");
	this->buffer = new bufStr(sizestr);
	this->nstr = 0;
	is_buff = 0;
	is_edited = 0;
	while(!this->ReadStr(this->nstr)) this->nstr++;
}

fBuffer::~fBuffer()
{
	delete [] this->str;
	this->str = NULL;
	delete [] this->fn;
	this->fn = NULL;
	delete [] this->fn_tmp;
	this->fn_tmp = NULL;
	delete(this->buffer);
	this->nstr = -1;
}

int fBuffer::ReadStr(int num)
{
//	if(num>this->nstr-1) return 1;
	int i = -1;

	FILE *FNN;

	if(!this->buffer->getrec(this->str, num)) return 0;

	FNN = fopen(this->fn, "rb");
	if(FNN==NULL) FNN = fopen(this->fn, "wb+");

	if(FNN==NULL) return 1;

	int len;
	if(fseek(FNN, (this->sizestr+1)*num, SEEK_SET))
	{
		fclose(FNN);
		return 1;
	}

	strcpy(str, "\0");
	if(!fread(this->str, this->sizestr+1, 1, FNN))
	{
		fclose(FNN);
		return 2;
	}
	strcpy(&str[this->sizestr+1], "\0");

	fclose(FNN);

	this->buffer->setrec(this->str, num);

//	delete(FNN);
	return 0;
}

int fBuffer::init(char* fn, int sizestr)
{
	is_edited = 0;
	if(buffer!=NULL) delete buffer;
	this->buffer = new bufStr(sizestr);
	this->sizestr = sizestr;
	
	strncpy(this->fn, fn, 255);
	strncpy(this->fn_tmp, fn, 255);
	strcat(this->fn_tmp, ".tmp\0");

	FILE *FNN;

	if(sizestr==0)
	{
		FNN = fopen(this->fn, "r");

		fgets(this->str, STRLEN, FNN);
		this->sizestr = strlen(this->str);
		
		fclose(FNN);
		return 0;
	}

	FNN = fopen(this->fn, "rb");
	if(FNN==NULL) FNN = fopen(this->fn, "wb+");

	if(FNN==NULL) return 1;

//	fseek(FNN, 0, SEEK_SET);
	this->nstr = 0;
//	fgets(this->str, 1000, FNN);
//	this->sizestr = strlen(this->str);
	while(fread(this->str, this->sizestr+1, 1, FNN))
	{
		strcpy(&str[this->sizestr+1], "\0");
		this->buffer->setrec(this->str, nstr);
		
		this->nstr++;
	}

	this->is_edited = 0;

	fclose(FNN);

	return 0;
}

int fBuffer::AddStr(char *nnnstr, int num)
{
	if((num>this->nstr)||(num<0)) return 1;

	int i = 0;
/*
	FILE *FN, *FNTMP;

	FN = NULL;
	FNTMP = NULL;

	if(num==this->nstr)
	{
		FN = fopen(this->fn, "ab");
		fseek(FN, 0, SEEK_END);
		sprintf(this->str, "%s\n", nnnstr);
		fwrite(this->str, this->sizestr+1, 1, FN);
	}
	else
	{
		FN = fopen(this->fn, "rb");
		if(FN==NULL) FN = fopen(this->fn, "wb+");

		FNTMP = fopen(this->fn_tmp, "wb+");

		for(i=0; i<this->nstr; i++)
		{
			if(!fread(this->str, this->sizestr+1, 1, FN)) break;
			if(!fwrite(this->str, this->sizestr+1, 1, FNTMP)) break;
		}

		FN = freopen(this->fn, "wb+", FN);
		FNTMP = freopen(this->fn_tmp, "rb", FNTMP);

		i=0;

		while(i<this->nstr+1)
		{
			strcpy(this->str, "");
			if(i!=num) fread(this->str, this->sizestr+1, 1, FNTMP);
			else sprintf(this->str, "%s\n", nnnstr);
//			else sprintf(this->str, "%s", nnnstr);

//			strcpy(&this->str[this->sizestr], "\n");

			if(!fwrite(this->str, this->sizestr+1, 1, FN)) break;
			i++;
		}          

	}
*/
	if(this->buffer->setrec(nnnstr, num)) return 1;
	this->is_edited = 1;
	this->nstr++;

/*
	if(FN!=NULL)
	{
		fclose(FN);
		FN = NULL;
	}
	if(FNTMP!=NULL)
	{
		fclose(FNTMP);
		FNTMP = NULL;
	}
*/
	return 0;
}

int fBuffer::DelStr(int num)
{
	if(this->buffer->remrec(num)) return 1;
//	this->buffer->arrange();

	this->nstr--;
	this->is_edited = 1;
	
	return 0;
}

int fBuffer::Free()
{

/*
	FILE *FN;
	FN = fopen(this->fn, "wb+");
	fclose(FN);

	FN = fopen(this->fn_tmp, "wb+");
	fclose(FN);
*/
	this->buffer->empty();

	this->nstr = this->buffer->len;
	this->is_edited = 1;

	return 0;
}

/*
int fBuffer::DelStr(char *delstr)
{
	int i = 0;
	int res = 1;

	char *tstr;
	tstr = new char[this->sizestr+1];

//	sprintf(tstr, "%s\n", delstr);

	FILE *FN, *FNTMP;

	FN = fopen(this->fn, "rb");
	if(FN==NULL) FN = fopen(this->fn, "wb+");

	FNTMP = fopen(this->fn_tmp, "wb+");

	for(i=0; i<this->nstr; i++)
	{
		if(!fread(this->str, this->sizestr+1, 1, FN)) return 1;
		strcpy(tstr, this->str);
		strcpy(&tstr[this->sizestr], "\0");
		if(!streqv(delstr, tstr)) fwrite(this->str, this->sizestr+1, 1, FNTMP);
		else 
		{
			res = 0;
//			this->buffer->remrec(i);
		}
	}

	FN = freopen(this->fn, "wb+", FN);
	FNTMP = freopen(this->fn_tmp, "rb", FNTMP);

	while(fread(this->str, this->sizestr+1, 1, FNTMP)) fwrite(this->str, this->sizestr+1, 1, FN);

	this->nstr -= 1 - res;

	delete [] tstr;
	fclose(FN);
	fclose(FNTMP);
	return res;
}
*/
int fBuffer::Exchange(int k, int l)
{
	if((k>=this->nstr)||(l>=this->nstr)) return 1;


	this->is_edited = !this->buffer->exchange(k, l);




/*

//	this->buffer->remrec(k);
//	this->buffer->remrec(l);

	char *k_str, *l_str;
	k_str = new char[this->sizestr+8];
	l_str = new char[this->sizestr+8];

	int i = 0;

	FILE *FN, *FNTMP;

	FN = fopen(this->fn, "rb");
	if(FN==NULL) FN = fopen(this->fn, "wb+");

	FNTMP = fopen(this->fn_tmp, "wb+");

	for(i=0; i<this->nstr; i++)
	{
		if(!fread(this->str, this->sizestr+1, 1, FN)) return 1;
		if(i==k) strcpy(k_str, this->str);
		else if(i==l) strcpy(l_str, this->str);
		else fwrite(this->str, this->sizestr+1, 1, FNTMP);
	}

	FN = freopen(this->fn, "wb+", FN);
	FNTMP = freopen(this->fn_tmp, "rb", FNTMP);

	i=0;

	while(i<this->nstr)
	{
		if(i==l) strcpy(this->str, k_str);
		else if(i==k) strcpy(this->str, l_str);
		else if(!fread(this->str, this->sizestr+1, 1, FNTMP)) break;
		fwrite(this->str, this->sizestr+1, 1, FN);
		i++;
	}

//	this->ReadStr(k);
//	this->ReadStr(l);
	
	fclose(FN);
	fclose(FNTMP);
	delete [] k_str;
	delete [] l_str;
	k_str = NULL;
	l_str = NULL;
	FN = NULL;
	FNTMP = NULL;
*/
	return 0;
}

int fBuffer::Update(char *nnstr, int num)
{
	if(num>this->nstr) return 1;

	this->is_edited = !this->buffer->update(nnstr, num);

	return 0;

/*

	int i = 0;

	FILE *FN, *FNTMP;

	FN = fopen(this->fn, "rb");
	if(FN==NULL) FN = fopen(this->fn, "wb+");

	FNTMP = fopen(this->fn_tmp, "wb+");

	for(i=0; i<this->nstr; i++)
	{
		if(!fread(this->str, this->sizestr+1, 1, FN)) return 1;
		fwrite(this->str, this->sizestr+1, 1, FNTMP);
	}

	FN = freopen(this->fn, "wb+", FN);
	FNTMP = freopen(this->fn_tmp, "rb", FNTMP);

	i=0;

	while(i<this->nstr)
	{
		if(i==num) 
		{
			if(!fread(this->str, this->sizestr+1, 1, FNTMP)) return 1;
			strcpy(this->str, nnstr);
			strcat(this->str, "\n");
		}
		else if(!fread(this->str, this->sizestr+1, 1, FNTMP)) return 1;
		fwrite(this->str, this->sizestr+1, 1, FN);
		i++;
	}
	
	fclose(FN);
	fclose(FNTMP);
*/
	return 0;
}

int fBuffer::Save()
{
	int i;
	if(this->is_edited)
	{
		FILE *FN = fopen(this->fn, "wb+");


		for(i=0; i<this->nstr; i++)
		{
			this->buffer->getrec(str, i);

			strcat(this->str, "\n");
			if(!fwrite(str, sizestr+1, 1, FN)) return 2;
			
		}

		this->is_edited = 0;
		fclose(FN);

	}

	return this->is_edited;
}

int fBuffer::SaveAs(char *fnname)
{
	FILE *FE = fopen(fnname, "w");
	fclose(FE);
	strcpy(this->fn, fnname);
	this->is_edited = 1;
	return(this->Save());
}

///////////////////////////////////////////////////////////////
