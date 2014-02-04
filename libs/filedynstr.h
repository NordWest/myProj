#ifndef FILEDYNSTR_H
#define FILEDYNSTR_H

#include <QString>
#include <QStringList>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "comfunc.h"

class FileDynStr
{
public:
    char *fn, *fn_tmp;

//	bufStr *buffer;

    int nstr;
    int sizestr;

    char *str;

    FileDynStr();
    FileDynStr(int sizestr);
    FileDynStr(char *fn, int sizestr);
    ~FileDynStr();
    int init(char* fn, int sizestr);

    int AddStr(char *nstr, int num);
    int DelStr(int num);
    int DelStr(char *pstr);
    int ReadStr(int num);
    int Exchange(int i, int j);
    int Update(char *nstr, int num);
    int Free();
};

class FileDynStrBin
{
public:
        QString fileName;

//	bufStr *buffer;

        //int nstr;
        //int sizestr;
        char *str;

        QStringList strList;

    FileDynStrBin();
    FileDynStrBin(int sizestr);
        FileDynStrBin(QString fn);
        FileDynStrBin(char *fn);
    ~FileDynStrBin();
        int init(QString fn);
        int init(char *fn);

    int AddStr(char *nstr, int num);
    int DelStr(int num);
    int DelStr(char *pstr);
    int ReadStr(int num);
    int Exchange(int i, int j);
    int Update(char *nstr, int num);
    int Free();
        int nstr();

        int saveAs(QString fn);
        int save();
};

#endif // FILEDYNSTR_H
