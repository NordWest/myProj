#ifndef LISTFILE_H
#define LISTFILE_H

#include <QtCore>

//template <class T>
class tRecord
{
public:
//    tRecord(){};
//    ~tRecord(){};

    int fromString(QString tStr){return 0;};
    void toString(QString &tStr){};
    tRecord& operator=(const tRecord &rhs);
};

/*
template <typename T>
class tRecordList
{
    QList <T*> recList;
};
*/

template <class tRecord>
class listFile
{
public:
    QList <tRecord*> recList;
    QString fileName;

/*    int s2rec(QString tStr){
        tRecord *tRec;
        tRec = new tRecord;
        tRec->toString(tStr);
        recList << tRec;
        return 0;
    };*/
    int init(QString fname)
    {
        fileName = fname;
        QFile iniFile(fileName);
        if(!iniFile.open(QIODevice::ReadOnly))
        {
            qDebug() << "init ini.lst error\n";
            return 1;
        }
        QTextStream iniStm(&iniFile);

        QString tStr;

        recList.clear();
        tRecord* tRec;
        tRec = new tRecord;

        while(!iniStm.atEnd())
        {
            tStr = iniStm.readLine();

            if(tRec->fromString(tStr)) continue;
            recList << tRec;
            tRec = new tRecord;
        }

        iniFile.close();

        delete tRec;

        return 0;
    };

    int size()
    {
        return(recList.size());
    }

    tRecord* at(int i)
    {
        return(recList.at(i));
    }

    int save()
    {
        int i, sz;
        QString tstr;
        sz = recList.size();

        QFile resFile(fileName);
        if(!resFile.open(QFile::WriteOnly | QFile::Truncate)) return 1;
        QTextStream resStm(&resFile);

        for(i=0; i<sz; i++)
        {
            recList.at(i)->toString(tstr);
            resStm << tstr << "\n";
        }

        resFile.close();

        return 0;
    }

    int saveAs(QString tfName)
    {
        fileName = tfName;
        return(save());
    }

    int getRec(tRecord* tRec, int pos)
    {
        if(pos<0||pos>=recList.size()) return 1;

        tRec = recList.at(pos);

        return 0;
    }

    void append(tRecord tRec)
    {
        tRecord *tlnRec = new tRecord;
        *tlnRec = tRec;
        recList << tlnRec;
    }

    int updateRec(tRecord* tRec, int pos)
    {
        if(pos<0||pos>=recList.size()) return 1;

        recList.at(pos) = *tRec;

        return 0;
    }

/*
    int save();
    int saveAs(QString fname);

    //int getRec(tRecord *resRec, QString name);
    int getRec(tRecord *resRec, int pos);
    int getRec(tRecord *resRec, tRecord *iRec);
    int addRec(tRecord *nrec);
    int insertRec(tRecord *nrec, int pos);
    int updateRec(tRecord *nrec);
    int size();

    //listFile();

    int rec2s(QString &tStr);
    */
};

/*template <typename tRecord>
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
};

//template <class tRecord>
//int listFile<tRecord>::s2rec(QString tStr)
int listFile::s2rec(QString tStr)
{
    tRecord *tRec;
    tRec = new tRecord;
    if(tRec->toString(tStr)) return 1;
    recList << tRec;
    return 0;
}
*/
#endif // LISTFILE_H
