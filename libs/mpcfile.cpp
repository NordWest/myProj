#include "mpcfile.h"

void mpcRec::getMpNumber(QString &mpNumber)
{
    mpNumber.clear();
    mpNumber.insert(0, dataStr.mid(0, 5));
}

int mpcRec::mpNumber()
{
    bool isOk;
    int num = dataStr.mid(0, 5).toInt(&isOk);
    if(isOk) return num;
    return -1;
}

void mpcRec::getProvDest(QString &provDest);
void mpcRec::getDiscAst(QString &discAst);
int mpcRec::discAstKey();
void mpcRec::getNote1(QString &note1);
void mpcRec::getNote2(QString &note2);
double mpcRec::mjd();
double mpcRec::ra();
double mpcRec::dec();
double mpcRec::magn();
void mpcRec::getObsCode(QString &note2);

void mpcRec::setStr(QString dStr);

mpcFile::mpcFile()
{
}
