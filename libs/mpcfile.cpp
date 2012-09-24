#include "mpcfile.h"


QString trCatFlag(QString catFlag)
{
    if(QString().compare(catFlag, "a", Qt::CaseInsensitive)==0||QString().compare(catFlag, "b", Qt::CaseInsensitive)==0) return QString("USNO-A1.0");
    if(QString().compare(catFlag, "c", Qt::CaseInsensitive)==0||QString().compare(catFlag, "d", Qt::CaseInsensitive)==0) return QString("USNO-A2.0");
    if(QString().compare(catFlag, "e", Qt::CaseInsensitive)==0) return QString("UCAC1");
    if(QString().compare(catFlag, "f", Qt::CaseInsensitive)==0) return QString("Tycho-1");
    if(QString().compare(catFlag, "g", Qt::CaseInsensitive)==0) return QString("Tycho-2");
    if(QString().compare(catFlag, "h", Qt::CaseInsensitive)==0) return QString("GSC-1.0");
    if(QString().compare(catFlag, "i", Qt::CaseInsensitive)==0) return QString("GSC-1.1");
    if(QString().compare(catFlag, "j", Qt::CaseInsensitive)==0) return QString("GSC-1.2");
    if(QString().compare(catFlag, "k", Qt::CaseInsensitive)==0) return QString("GSC-2.2");
    if(QString().compare(catFlag, "l", Qt::CaseInsensitive)==0) return QString("ACT");
    if(QString().compare(catFlag, "m", Qt::CaseInsensitive)==0) return QString("GSC-ACT");
    if(QString().compare(catFlag, "n", Qt::CaseInsensitive)==0) return QString("TRC");
    if(QString().compare(catFlag, "o", Qt::CaseInsensitive)==0) return QString("USNO-B1.0");
    if(QString().compare(catFlag, "p", Qt::CaseInsensitive)==0) return QString("PPM");
    if(QString().compare(catFlag, "q", Qt::CaseInsensitive)==0) return QString("UCAC2-beta");
    if(QString().compare(catFlag, "r", Qt::CaseInsensitive)==0) return QString("UCAC2");
    if(QString().compare(catFlag, "s", Qt::CaseInsensitive)==0) return QString("USNO-B2.0");
    if(QString().compare(catFlag, "t", Qt::CaseInsensitive)==0) return QString("UCAC3-beta");
    if(QString().compare(catFlag, "u", Qt::CaseInsensitive)==0) return QString("UCAC3");
    if(QString().compare(catFlag, "v", Qt::CaseInsensitive)==0) return QString("NOMAD");
    if(QString().compare(catFlag, "w", Qt::CaseInsensitive)==0) return QString("CMC");
    if(QString().compare(catFlag, "x", Qt::CaseInsensitive)==0) return QString("Hip-2");
    if(QString().compare(catFlag, "z", Qt::CaseInsensitive)==0) return QString("GSC");
    return QString("Unspecified");
}

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
    QString upStr;


    unpackString(&upStr, dataStr.mid(0, 5));

    int num = upStr.toInt(&isOk);

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
    double jday;
    QString tStr;
    tStr = dataStr.mid(15, 17);
    dat2JD(&jday, tStr.section(" ", 0, 0, QString::SectionSkipEmpty).toInt(), tStr.section(" ", 1, 1, QString::SectionSkipEmpty).toInt(), tStr.section(" ", 2, 2, QString::SectionSkipEmpty).toDouble());
    double res = jd2mjd(jday);
    return(res);
}

double mpcRec::ra()
{
    QString tStr;
    tStr = dataStr.mid(32, 12);
    return(hms2grad(tStr.mid(0, 2).toInt(), tStr.mid(3, 2).toInt(), tStr.mid(6, 6).toDouble()));
}

double mpcRec::dec()
{
    QString tStr;
    tStr = dataStr.mid(44, 12);
    return(gms2grad(QString().compare(tStr.mid(0,1), "+")==0, tStr.mid(1,2).toInt(), tStr.mid(4,2).toInt(), tStr.mid(7,5).toDouble()));
}

double mpcRec::magn()
{
    QString tStr;
    tStr = dataStr.mid(65, 5);
    //qDebug() << QString("magn: %1\n").arg(tStr);
    return(tStr.toDouble());
}

void mpcRec::getObsCode(QString &obsCode)
{
    obsCode.clear();
    obsCode.insert(0, dataStr.mid(77, 3));
}

int  mpcRec::getObsNum()
{
    QString obsCode;
    getObsCode(obsCode);

    bool isOk;
    QString upStr;


    unpackString(&upStr, obsCode);

    int num = upStr.toInt(&isOk);

    if(isOk) return num;
    return -1;
}

void mpcRec::getBand(QString &bandStr)
{
    bandStr.clear();
    bandStr.insert(0, dataStr.mid(70, 1));
}

void mpcRec::getCatFlag(QString &catFlag)
{
    catFlag.clear();
    catFlag.insert(0, dataStr.mid(71, 1));
}

void mpcRec::getCatName(QString &catName)
{
    QString catFlag;

    getCatFlag(catFlag);
    catName = trCatFlag(catFlag);

}
/*
void mpcRec::getCatFamName(QString &catName)
{
    QString catFlag;

    getCatFlag(catFlag);
    catName = trCatFlag(catFlag);

}
*/
int mpcRec::fromStr(QString dStr)
{
    if(dStr.size()<80) return 1;
    dataStr.clear();
    dataStr.insert(0, dStr);
    return 0;
}

int mpcRec::fromRec(mpcRec& source)
{
    dataStr.clear();
    dataStr.insert(0, source.toStr());
}

QString mpcRec::toStr()
{
    return QString(dataStr);
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

void mpcRec::setTimePos(double mjdt, double radeg, double deDeg)
{
    QString strT(dataStr);
    dataStr.clear();
    dataStr = QString("%1%2%3%4%5").arg(strT.mid(0));
}

void mpcRec::setPos(double radeg, double deDeg)
{
    QString strT(dataStr);
    dataStr.clear();
    dataStr = QString("%1%2%3%4%5").arg(strT.mid(0));
}

void mpcRec::setMagn(double magn)
{

}

void mpcRec::setName(QString name)
{

}

void mpcRec::setProvDest(QString pName)
{

}


///////////////////////////////////////////////////////////


mpcFile::mpcFile()
{
    recList.clear();
}

int mpcFile::init(QString fName)
{
    fileName = QString(fName);
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
    return 0;
}

int mpcFile::saveAs(QString fName)
{
    fileName = QString(fName);
    return(save());
}

int mpcFile::save()
{
    QFile outFile(fileName);
    if(!outFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) return 1;
    QTextStream outStm(&outFile);
    //mpcRec *mpR;

    int szi, i;

    szi = recList.size();

    for(i=0; i<szi; i++)
    {
        outStm << recList.at(i)->toStr() << "\n";
    }

    outFile.close();
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

int mpcFile::removeAt(int pos)
{
    if(pos<0||pos>=size()) return 1;
    recList.removeAt(pos);
    return 0;
}

void mpcFile::addRec(mpcRec& nRec)
{
    mpcRec *sRec = new mpcRec(nRec.toStr());
    recList << sRec;
}
