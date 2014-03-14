#include <QCoreApplication>

#include "./../../libs/redStat.h"

struct eqObjRec
{
    QString objName;
    eqFile* eq_list;
    void sortByTime()
    {
        eq_list->sortOClist();
    }
    int size()
    {
        return eq_list->ocList.size();
    }

};

struct eqObjList
{
    QList <eqObjRec*> eqrList;
    void addEQ(ocRec *oc_rec)
    {
        int i, sz, nObj;
        eqObjRec *eqoTemp;
        sz = eqrList.size();
        nObj=1;
        for(i=0;i<sz;i++)
        {
            eqoTemp = eqrList.at(i);
            if(QString().compare(oc_rec->name, eqoTemp->objName)==0)
            {
                nObj=0;
                eqoTemp->eq_list->ocList << oc_rec;
                break;
            }
        }
        if(nObj)
        {
            eqoTemp = new eqObjRec;
            eqoTemp->objName = oc_rec->name;
            eqoTemp->eq_list->ocList << oc_rec;
            eqrList << eqoTemp;
        }
    };
};


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    setlocale(LC_NUMERIC, "C");

    eqFile eq_ini;
    eq_ini.init(argv[1]);

    QString resFolder = "./eqStat";
    QDir resDir(resFolder);

    int sz, i, j, szj;

    sz =eq_ini.size();
    for(i=0;i<sz;i++)
    {

    }
    
    return a.exec();
}
