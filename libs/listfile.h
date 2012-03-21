#ifndef LISTFILE_H
#define LISTFILE_H

#include <QtCore>

//template <class T>
class tRecord
{
    tRecord(){};
//    ~tRecord(){};

    int fromString(QString tStr){};
    int toString(QString &tStr){};
};

template <class tRecord>
class listFile
{
    QList <tRecord*> recList;
    QString fileName;
 public:
    int init(QString fname);

    int save();
    int saveAs(QString fname);

    //int getRec(tRecord *resRec, QString name);
    int getRec(tRecord *resRec, int pos);
    int getRec(tRecord *resRec, tRecord *iRec);
    int addRec(tRecord *nrec);
    int insertRec(tRecord *nrec, int pos);
    int updateRec(tRecord *nrec);
    int size();

    listFile();

    int rec2s(QString &tStr);
    int s2rec(QString tStr);
};

#endif // LISTFILE_H
