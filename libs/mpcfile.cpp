#include "mpcfile.h"


void mpcRec::getMpNumber(QString &mpNumber)
{
    mpNumber.clear();
    mpNumber.insert(0, dataStr.mid(0, 5));
}

/*
QString mpcRec::getMpNumber()
{
    return(QString(dataStr.mid(0, 5)));
}
*/
int mpcRec::mpNumber()
{
    bool isOk;
    int num = dataStr.mid(0, 5).toInt(&isOk);
    if(isOk) return num;
    return -1;
}

void mpcRec::getProvDest(QString &provDest)
{
    provDest.clear();
    provDest.insert(0, dataStr.mid(5, 7));
}

void mpcRec::getDiscAst(QString &discAst)
{
    discAst.clear();
    discAst.insert(0, dataStr.mid(12, 1));
}

int mpcRec::discAstKey()
{
    QString tStr;
    getDiscAst(tStr);
    if(QString().compare(tStr, "*")==0) return 1;
    return 0;
}

void mpcRec::getNote1(QString &note1)
{
    note1.clear();
    note1.insert(0, dataStr.mid(13, 1));
}

void mpcRec::getNote2(QString &note2)
{
    note2.clear();
    note2.insert(0, dataStr.mid(14, 1));
}

double mpcRec::mjd()
{
    QString tStr;
    tStr = dataStr.mid(15, 17);

    return(getMJDfromYMD(tStr));
}

double mpcRec::ra()
{
    QString tStr;
    tStr = dataStr.mid(32, 12);
    return(mas_to_grad(hms_to_mas(dataStr.mid(32, 12), " ")));
}

double mpcRec::dec()
{
    return(mas_to_grad(damas_to_mas(dataStr.mid(44, 12), " ")));
}

double mpcRec::magn()
{
    QString tStr;
    tStr = dataStr.mid(65, 5);
    qDebug() << QString("magn: %1\n").arg(tStr);
    return(tStr.toDouble());
}

void mpcRec::getObsCode(QString &obsCode)
{
    obsCode.clear();
    obsCode.insert(0, dataStr.mid(77, 3));
}

void mpcRec::getBand(QString &bandStr)
{
    bandStr.clear();
    bandStr.insert(0, dataStr.mid(70, 1));
}

int mpcRec::fromStr(QString dStr)
{
    if(dStr.size()<80) return 1;
    dataStr.clear();
    dataStr.insert(0, dStr);
    return 0;
}

mpcRec::mpcRec()
{
    dataStr.clear();
}

mpcRec::mpcRec(QString nStr)
{
    dataStr.clear();
    dataStr.insert(0, nStr);
}


///////////////////////////////////////////////////////////


mpcFile::mpcFile()
{
    recList.clear();
}

int mpcFile::init(QString fName)
{
    QFile iniFile(fName);
    if(!iniFile.open(QIODevice::ReadOnly)) return 1;
    QTextStream iniStm(&iniFile);
    mpcRec *mpR;

    while(!iniStm.atEnd())
    {
        mpR = new mpcRec;
        if(mpR->fromStr(iniStm.readLine()))
        {
            delete mpR;
            continue;
        }
        recList << mpR;
    }
}

mpcRec* mpcFile::at(int i)
{
    if(i<0||i>recList.size()) return NULL;
    return recList.at(i);
}

int mpcFile::size()
{
    return(recList.size());
}

