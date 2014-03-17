#include <QCoreApplication>

#include "./../../libs/redStat.h"

struct eqObjRec
{
    QString objName;
    QList <ocRec*> eq_list;
    void sortByTime()
    {
        sortOClist(eq_list);
    }
    int size()
    {
        return eq_list.size();
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
                eqoTemp->eq_list << oc_rec;
                break;
            }
        }
        if(nObj)
        {
            eqoTemp = new eqObjRec;
            eqoTemp->objName = oc_rec->name;
            eqoTemp->eq_list << oc_rec;
            eqrList << eqoTemp;
        }
    };
};

struct eqObsRec
{
    QString obsCode;
    eqObjList objList;
};

struct eqObsList
{
    QList <eqObsRec*> eqoList;
    void addEQ(ocRec *oc_rec)
    {
        int i, sz, nObs;
        eqObsRec *eqoTemp;
        sz = eqoList.size();
        nObs=1;
        for(i=0;i<sz;i++)
        {
            eqoTemp = eqoList.at(i);
            if(QString().compare(oc_rec->obsCode, eqoTemp->obsCode)==0)
            {
                nObs=0;
                eqoTemp->objList.addEQ(oc_rec);
                break;
            }
        }
        if(nObs)
        {
            eqoTemp = new eqObsRec;
            eqoTemp->obsCode = oc_rec->name;
            eqoTemp->objList.addEQ(oc_rec);
            eqoList << eqoTemp;
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

    eqObsList obs_list;

    sz =eq_ini.size();
    qDebug() << QString("eq num: %1\n").arg(sz);
    for(i=0;i<sz;i++)
    {
        obs_list.addEQ(eq_ini.at(i));
    }

    sz = obs_list.eqoList.size();
    qDebug() << QString("Observatories num: %1\n").arg(sz);
    
    return 0;//a.exec();
}
