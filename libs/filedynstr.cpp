#include "filedynstr.h"

////////////////////	FileDynStr	/////////////////////////////

FileDynStr::FileDynStr()
{
    this->str = new char[1024];
    this->fn = new char[255];
    this->fn_tmp = new char[255];
//	this->buffer = NULL;
    this->sizestr = 0;
    this->nstr = 0;
}

FileDynStr::FileDynStr(int sizestr)
{
    this->str = new char[sizestr];
    this->fn = new char[255];
    this->fn_tmp = new char[255];
//	this->buffer = new bufStr(sizestr);
    this->sizestr = sizestr;
    this->nstr = 0;
}

FileDynStr::FileDynStr(char *fn, int sizestr)
{
    this->str = new char[sizestr];
    this->fn = new char[255];
    this->fn_tmp = new char[255];
    this->sizestr = sizestr;
    strcpy(this->fn, fn);
    strcpy(this->fn_tmp, fn);
    strcat(this->fn_tmp, ".tmp");
//	this->buffer = new bufStr(sizestr);
    this->nstr = 0;
    while(!this->ReadStr(this->nstr)) this->nstr++;
}

FileDynStr::~FileDynStr()
{
    delete [] this->str;
    this->str = NULL;
    delete [] this->fn;
    this->fn = NULL;
    delete [] this->fn_tmp;
    this->fn_tmp = NULL;
//	delete(this->buffer);
    this->nstr = -1;
}

int FileDynStr::ReadStr(int num)
{
//	if(num>this->nstr-1) return 1;
    int i = -1;

    FILE *FNN;

//	if(!this->buffer->getrec(this->str, num)) return 0;

    FNN = fopen(this->fn, "rb");
    if(FNN==NULL) FNN = fopen(this->fn, "wb+");

    if(FNN==NULL) return 1;

    int len;
    for(int lp=0; lp<num; lp++)
    {
        if(fgets(this->str, this->sizestr+1, FNN)==NULL)
        {
            fclose(FNN);
            return 1;
        }
    }

    strcpy(str, "");
//	fseek(FNN, 0, SEEK_SET);
    if(fgets(this->str, this->sizestr+1, FNN)==NULL)
    {
        ferror(FNN);
        fclose(FNN);
        return 2;
    }
    int sstr = strlen(str);
    if(sstr<2) return 1;
    strcpy(&str[sstr-1], "\0");

    fclose(FNN);

//	this->buffer->setrec(this->str, num);

//	delete(FNN);
    return 0;
}

int FileDynStr::init(char* fn, int sizestr)
{
//	this->buffer = new bufStr(sizestr);
    this->sizestr = sizestr;
    strcpy(this->fn, fn);
    strcpy(this->fn_tmp, fn);
    strcat(this->fn_tmp, ".tmp");

        FILE *FNN = NULL;

    FNN = fopen(this->fn, "r");
        if(FNN==NULL) return 1;
    while(fgets(this->str, this->sizestr, FNN)!=NULL)this->nstr++;
    //this->sizestr = strlen(this->str);

    fclose(FNN);
/*

    FNN = fopen(this->fn, "rb");
    if(FNN==NULL) FNN = fopen(this->fn, "wb+");

    if(FNN==NULL) return 1;

//	fseek(FNN, 0, SEEK_SET);
    this->nstr = 0;
//	fgets(this->str, 1000, FNN);
//	this->sizestr = strlen(this->str);
    while(fread(this->str, this->sizestr+1, 1, FNN)) this->nstr++;

    fclose(FNN);
*/
    return 0;
}

int FileDynStr::AddStr(char *nnnstr, int num)
{
    if((num>this->nstr)||(num<0)) return 1;

    int i = 0;

    FILE *FN, *FNTMP;

    FN = NULL;
    FNTMP = NULL;

    if(num==this->nstr)
    {
        FN = fopen(this->fn, "a");
        fseek(FN, 0, SEEK_END);
        sprintf(this->str, "%s", nnnstr);
        fputs(this->str, FN);
    }
    else
    {
        FN = fopen(this->fn, "r");
        if(FN==NULL) FN = fopen(this->fn, "w+");

        FNTMP = fopen(this->fn_tmp, "w+");

        for(i=0; i<this->nstr; i++)
        {
            if(fgets(this->str, this->sizestr+1, FN)==NULL) break;
            if(!fputs(this->str, FNTMP)) break;
        }

        FN = freopen(this->fn, "w+", FN);
        FNTMP = freopen(this->fn_tmp, "r", FNTMP);

        i=0;

        while(i<this->nstr+1)
        {
            strcpy(this->str, "");
            if(i!=num) fputs(this->str, FNTMP);
            else sprintf(this->str, "%s", nnnstr);
//			else sprintf(this->str, "%s", nnnstr);

//			strcpy(&this->str[this->sizestr], "\n");

            if(!fputs(this->str, FN)) break;
            i++;
        }

    }
    this->nstr++;

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

    return 0;
}

int FileDynStr::DelStr(int num)
{
    int i = 0;
    FILE *FN, *FNTMP;

    if((num>this->nstr-1)||(num<0)) return 1;

    FN = fopen(this->fn, "rb");
    if(FN==NULL) FN = fopen(this->fn, "wb+");

    FNTMP = fopen(this->fn_tmp, "wb+");

    for(i=0; i<this->nstr; i++)
    {
        if(!fread(this->str, this->sizestr+1, 1, FN)) return 1;
        if(i!=num) fwrite(this->str, this->sizestr+1, 1, FNTMP);
    }

    FN = freopen(this->fn, "wb+", FN);
    FNTMP = freopen(this->fn_tmp, "rb", FNTMP);

    while(fread(this->str, this->sizestr+1, 1, FNTMP)) fwrite(this->str, this->sizestr+1, 1, FN);

    this->nstr--;

    fclose(FN);
    fclose(FNTMP);
    return 0;
}

int FileDynStr::Free()
{
    FILE *FN;
    FN = fopen(this->fn, "wb+");
    fclose(FN);

    FN = fopen(this->fn_tmp, "wb+");
    fclose(FN);

    this->nstr = 0;

    return 0;
}

int FileDynStr::DelStr(char *delstr)
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

int FileDynStr::Exchange(int k, int l)
{
    if((k>=this->nstr)||(l>=this->nstr)) return 1;

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
    return 0;
}

int FileDynStr::Update(char *nnstr, int num)
{
    if(num>this->nstr) return 1;

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
    return 0;
}


///////////////////////////////////////////////////////////////

////////////////////	FileDynStrBin	/////////////////////////////

FileDynStrBin::FileDynStrBin()
{
    this->str = new char[1024];
/*
    this->fn = new char[255];
    this->fn_tmp = new char[255];
//	this->buffer = NULL;
    this->sizestr = 0;
        this->nstr = 0;*/
}

FileDynStrBin::FileDynStrBin(int sizestr)
{
    this->str = new char[sizestr];
    /*
    this->str = new char[sizestr];
    this->fn = new char[255];
    this->fn_tmp = new char[255];
//	this->buffer = new bufStr(sizestr);
    this->sizestr = sizestr;
    this->nstr = 0;
        */
}

FileDynStrBin::FileDynStrBin(QString fn)
{
    this->str = new char[1024];
    fileName = QString(fn);
    init(fileName);
    /*
    this->str = new char[sizestr];
    this->fn = new char[255];
    this->fn_tmp = new char[255];
    this->sizestr = sizestr;
    strcpy(this->fn, fn);
    strcpy(this->fn_tmp, fn);
    strcat(this->fn_tmp, ".tmp");
//	this->buffer = new bufStr(sizestr);
    this->nstr = 0;
        while(!this->ReadStr(this->nstr)) this->nstr++;*/
}

FileDynStrBin::FileDynStrBin(char *fn)
{
    this->str = new char[1024];
    fileName = QString(fn);
    init(fileName);
}

FileDynStrBin::~FileDynStrBin()
{
    strList.clear();
    delete [] this->str;
    this->str = NULL;
    /*

    delete [] this->fn;
    this->fn = NULL;
    delete [] this->fn_tmp;
    this->fn_tmp = NULL;
//	delete(this->buffer);
        this->nstr = -1;*/
}

int FileDynStrBin::nstr()
{
    return(strList.size());
}

int FileDynStrBin::ReadStr(int num)
{
    strcpy(str, strList.at(num).toAscii().data());

   /*
//	if(num>this->nstr-1) return 1;
    int i = -1;

    FILE *FNN;

//	if(!this->buffer->getrec(this->str, num)) return 0;

    FNN = fopen(this->fn, "rb");
    if(FNN==NULL) FNN = fopen(this->fn, "wb+");

    if(FNN==NULL) return 1;

    int len;
    if(fseek(FNN, (this->sizestr+1)*num, SEEK_SET))
    {
        fclose(FNN);
        return 1;
    }

    strcpy(str, "");
    if(!fread(this->str, this->sizestr+1, 1, FNN))
    {
        fclose(FNN);
        return 2;
    }
    strcpy(&str[this->sizestr], "\0");

    fclose(FNN);

//	this->buffer->setrec(this->str, num);

//	delete(FNN);*/
    return 0;
}

int FileDynStrBin::init(QString fn)
{
//	this->buffer = new bufStr(sizestr);
    QFile inFile(fn);
    if(!inFile.open(QIODevice::ReadOnly))
    {
        qDebug() << QString("File %1 not open\n").arg(fn);
        return 1;
    }
    QTextStream inStm(&inFile);

    strList.clear();

    while(!inStm.atEnd())
    {
        strList << inStm.readLine();
    }

    inFile.close();

    return 0;
}

int FileDynStrBin::init(char *fn)
{
    fileName = QString(fn);
    init(fileName);
}

int FileDynStrBin::AddStr(char *nnnstr, int num)
{
    if((num>nstr())||(num<0)) return 1;

        strList.insert(num, QString(nnnstr));
        /*

    int i = 0;

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
    this->nstr++;

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

int FileDynStrBin::DelStr(int num)
{
    if(num<0||num>nstr()) return 1;
    strList.removeAt(num);

    /*
    int i = 0;
    FILE *FN, *FNTMP;

    if((num>this->nstr-1)||(num<0)) return 1;

    FN = fopen(this->fn, "rb");
    if(FN==NULL) FN = fopen(this->fn, "wb+");

    FNTMP = fopen(this->fn_tmp, "wb+");

    for(i=0; i<this->nstr; i++)
    {
        if(!fread(this->str, this->sizestr+1, 1, FN)) return 1;
                if(i!=num) fwrite(this->str, this->sizestr+1, 1, FNTMP);
    }

    FN = freopen(this->fn, "wb+", FN);
    FNTMP = freopen(this->fn_tmp, "rb", FNTMP);

    while(fread(this->str, this->sizestr+1, 1, FNTMP)) fwrite(this->str, this->sizestr+1, 1, FN);

    this->nstr--;

    fclose(FN);
        fclose(FNTMP);*/
    return 0;
}

int FileDynStrBin::Free()
{
    strList.clear();

    /*
    FILE *FN;
    FN = fopen(this->fn, "wb+");
    fclose(FN);

    FN = fopen(this->fn_tmp, "wb+");
    fclose(FN);

    this->nstr = 0;
    */
    return 0;
}


int FileDynStrBin::DelStr(char *delstr)
{
    int i;
    int ns = nstr();

    QString tStr(delstr);

    for(i=ns-1; i>=0; i--)
    {
        if(QString().compare(strList.at(i), tStr)==0)
        {
            strList.removeAt(i);
            return 0;
        }
    }

    return 1;
/*
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
        */
}

int FileDynStrBin::Exchange(int k, int l)
{
    if((k>=nstr())||(l>=nstr())) return 1;

//	this->buffer->remrec(k);
//	this->buffer->remrec(l);

    strList.swap(k, l);

    /*
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
        FNTMP = NULL;*/


    return 0;
}

int FileDynStrBin::Update(char *nnstr, int num)
{
    if(num>nstr()) return 1;

    strList.replace(num, QString(nnstr));
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
        fclose(FNTMP);*/
    return 0;
}

int FileDynStrBin::saveAs(QString fn)
{
    fileName = QString(fn);
    return(save());
}

int FileDynStrBin::save()
{
    QFile oFile(fileName);
    if(!oFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qDebug() << QString("File %1 not open\n").arg(fileName);
        return 1;
    }
    QTextStream oStm(&oFile);

    oStm << strList.join("\n");

    return 0;
}



///////////////////////////////////////////////////////////////
