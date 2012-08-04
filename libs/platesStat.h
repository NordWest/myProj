#ifndef PLATESSTAT_H
#define PLATESSTAT_H

#include <QtCore>
#include "astro.h"

extern QTextStream out_stream;

struct plateRec;

void desc2NumName(QString desc, int *num, QString *name);
int sortPl(QList<plateRec*> *plL);


struct plateRec
{
public:
        QString name, desc;
        double jD;
        int num;
        QString astName;

        void rec2s(QString *rstr);
        void s2rec(QString rstr);
        void copyTo(plateRec *plr);
};

struct statBox
{
    int num;
    int name;
};

struct statRec
{
        public:
        double ti;
        QList <statBox*> boxes;
        statRec();
        ~statRec();
        void addNum(int name);
        int getNum(int name);
        void sortName();
};

class plateList
{
public:
    QList <plateRec*> plates;

    void initPlatesFromFile(QString fileName);
    void remFilePlates(QString remFileName);
    void task0(int nodeNum);
    void findNodesList(QList <plateRec*> selList, double dT);


};

#endif // PLATESSTAT_H
