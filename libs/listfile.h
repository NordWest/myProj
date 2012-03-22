#ifndef LISTFILE_H
#define LISTFILE_H

#include <QtCore>

//template <class T>
class tRecord
{
public:
    tRecord(){};
//    ~tRecord(){};

    int fromString(QString tStr){};
    int toString(QString &tStr){};
};

/*
template <typename T>
class tRecordList
{
    QList <T*> recList;
};
*/

template <typename tRecord>
class listFile
{
    QList <tRecord*> recList;
    QString fileName;
 public:
    int init(QString fname)
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
    int s2rec(QString tStr);
};

#endif // LISTFILE_H
