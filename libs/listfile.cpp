#include "listfile.h"

template <class tRecord>
listFile::listFile()
{

}

//template <class tRecord>
int listFile::init(QString fname)
{
    fileName = fname;
    QFile iniFile(fileName);
    iniFile.open(QIODevice::ReadOnly);
    QTextStream iniStm(&iniFile);

    QString tStr;

    recList.clear();

    while(!iniStm.atEnd())
    {
        tStr = iniStm.readLine();
        s2rec(tStr);//) continue;
        //recList << tRec;
        //tRec = new tRecord;
    }

    iniFile.close();


    return 0;
}
/*
int listFile::rec2s(tRecord *tRec, QString &tStr)
{
    tRecord *tRec;
    tRec = new tRecord;
    if(tRec->toString(tStr)) return 1;
    recList << tRec;
}
*/
template <class tRecord>
int listFile<tRecord>::s2rec(QString tStr)
{
    tRecord *tRec;
    tRec = new tRecord;
    if(tRec->toString(tStr)) return 1;
    recList << tRec;
    return 0;
}

template <class tRecord>
int listFile<tRecord>::addRec(tRecord *nrec)
{
    tRecord *tRec = new tRecord;
    QString tStr;
    nrec->toString(tStr);
    tRec->fromString(tStr);
    recList.append(tRec);
    return 0;
}

template <class tRecord>
int listFile<tRecord>::insertRec(tRecord *nrec, int pos)
{
    tRecord *tRec = new tRecord;
    QString tStr;
    nrec->toString(tStr);
    tRec->fromString(tStr);
    recList.insert(pos, tRec);
    return 0;
}

template <class tRecord>
int listFile<tRecord>::getRec(tRecord *resRec, int pos)
{
    int sz = recList.size();
    if(pos>=sz||pos<0) return 1;
    resRec = recList.at(pos);

    return 0;
}

template <class tRecord>
int listFile<tRecord>::getRec(tRecord *resRec, tRecord *iRec)
{
    int i;
    int sz = recList.size();
    tRecord *tRec;

    for(i=0;i<sz;i++)
    {
        getRec(tRec, i);
        if(tRec==iRec)
        {
            resRec = tRec;
            break;
        }
    }

    return 0;
}

template <class tRecord>
int listFile<tRecord>::updateRec(tRecord *nrec)
{
    tRecord *tRec;

    if(getRec(tRec, nrec)) return 1;

    tRec = nrec;
    return 0;
}

template <class tRecord>
int listFile<tRecord>::size()
{
    return recList.size();
}

template <class tRecord>
int listFile<tRecord>::save()
{
    int i, sz;
    QFile iniFile(fileName);
    iniFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream iniStm(&iniFile);
    QString tStr;

    sz = recList.size();
    for(i=0; i<sz; i++)
    {
        recList.at(i)->toString(tStr);
        iniStm << tStr << "\n";
    }
    return 0;
}

template <class tRecord>
int listFile<tRecord>::saveAs(QString fname)
{
    fileName = fname;
    return(save());
}

