#include <QtCore/QCoreApplication>

#include "./../libs/redStat.h"

struct errBpln
{
    QString plName;
//    QString versName;
    QList <errBudgetRec*> errList;
};

struct errVers
{
    QString versName;
    errBudgetRec* err;
};

class errVersPl
{
public:
    QString plName;
    errBudgetRec* vers21;
    errBudgetRec* vers22;
    errBudgetRec* vers23;
    errBudgetRec* vers32;
    errBudgetRec* vers33;
    errVersPl();
    ~errVersPl();
};

errVersPl::errVersPl()
{
    vers21 = NULL;
    vers22 = NULL;
    vers23 = NULL;
    vers32 = NULL;
    vers33 = NULL;
}

errVersPl::~errVersPl()
{
    delete vers21;
    delete vers22;
    delete vers23;
    delete vers32;
    delete vers33;
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int resType = atoi(argv[3]);
    int i, j, k, sz0, sz1, sz2;
    errBudgetFile ebF;
    ebF.init(argv[1]);
    sz0 = ebF.errList.size();
    QList <errBpln*> errPlist;
    errBpln* errpl;
    int isn;
    QString plName, versName;
    for(i=0; i<sz0; i++)
    {
        switch(resType)
        {
        case 0:
            {
                plName = ebF.errList.at(i)->originName.section("\\", -2, -2);
            }
            break;
        case 1:
            {
                plName = ebF.errList.at(i)->originName.left(ebF.errList.at(i)->originName.lastIndexOf("\\"));
            }
            break;
        }
        versName = ebF.errList.at(i)->originName.section(".", -1, -1);
        if(versName=="aver") versName = ebF.errList.at(i)->originName.section(".", -2, -2);
        qDebug() << QString("plName: %1\tversName: %2\n").arg(plName).arg(versName);
        //qDebug() << QString("plName: %1\n");
        sz1 = errPlist.size();
        isn = 1;
        for(j=0; j<sz1; j++)
        {
            if(errPlist.at(j)->plName.indexOf(plName)==0)
            {
       //         errPlist.at(j)->versName = versName;
                errPlist.at(j)->errList << ebF.errList.at(i);
                isn = 0;
                break;
            }
        }
        if(isn)
        {
            errpl = new errBpln;
            errpl->plName = plName;
      //      errpl->versName = versName;
            errpl->errList << ebF.errList.at(i);
            errPlist << errpl;
        }
    }

    QFile outFile(argv[2]);
    outFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    QTextStream outf(&outFile);
    QString ostr, origName;
    errBudgetRec* errC;


    QList <errVersPl*> eList;
    errVersPl *errvpl;
    errVers *errv;


    switch(resType)
    {
    case 0:
        {
            sz0 = errPlist.size();
            for(i=0; i<sz0; i++)
            {
                sz1 = errPlist.at(i)->errList.size();
                plName = errPlist.at(i)->plName;
                for(j=0; j<sz1; j++)
                {
                    errC = errPlist.at(i)->errList.at(j);
                    ostr.clear();
                    ostr = QString("%1|%2|%3|%4|%5|%6\n").arg(plName).arg(errC->originName).arg(errC->xParams.N).arg(errC->xParams.UWE).arg(errC->yParams.N).arg(errC->yParams.UWE);
                    outf << ostr;
                }
            }
        }
        break;
    case 1:
        {

            sz0 = errPlist.size();
            qDebug() << QString("sz0= %1\n").arg(sz0);
            for(i=0; i<sz0; i++)
            {
                plName = errPlist.at(i)->plName;

                sz1 = eList.size();
                isn = 1;
                qDebug() << QString("sz1= %1\n").arg(sz1);
                //versName = errPlist.at(i)->versName;
                for(j=0; j<sz1; j++)
                {
                    //versName = errPlist.at(i)->versName;
                    if(eList.at(j)->plName.indexOf(plName)==0)
                    {
                        sz2 = errPlist.at(i)->errList.size();
                        qDebug() << QString("sz2= %1\n").arg(sz2);
                        for(k=0;k<sz2; k++)
                        {
                            versName = errPlist.at(i)->errList.at(k)->originName.section(".", -1, -1);
                            if(versName=="aver") versName = errPlist.at(i)->errList.at(k)->originName.section(".", -2, -2);

                            if(versName=="v21")
                            {
                                eList.at(j)->vers21 = new errBudgetRec;
                                eList.at(j)->vers21 = errPlist.at(i)->errList.at(k);
                                continue;
                            }
                            if(versName=="v22")
                            {
                                eList.at(j)->vers22 = new errBudgetRec;
                                eList.at(j)->vers22 = errPlist.at(i)->errList.at(k);
                                continue;
                            }
                            if(versName=="v23")
                            {
                                eList.at(j)->vers23 = new errBudgetRec;
                                eList.at(j)->vers23 = errPlist.at(i)->errList.at(k);
                                continue;
                            }
                            if(versName=="v32")
                            {
                                eList.at(j)->vers32 = new errBudgetRec;
                                eList.at(j)->vers32 = errPlist.at(i)->errList.at(k);
                                continue;
                            }
                            if(versName=="v33")
                            {
                                eList.at(j)->vers33 = new errBudgetRec;
                                eList.at(j)->vers33 = errPlist.at(i)->errList.at(k);
                                continue;
                            }

                        }
                        isn = 0;
                        break;
                    }
                }
                if(isn)
                {
                    errvpl = new errVersPl;
                    errvpl->plName = plName;
                    sz2 = errPlist.at(i)->errList.size();
                    qDebug() << QString("sz2= %1\n").arg(sz2);
                    for(k=0;k<sz2; k++)
                    {
                        versName = errPlist.at(i)->errList.at(k)->originName.section(".", -1, -1);
                        if(versName=="aver") versName = errPlist.at(i)->errList.at(k)->originName.section(".", -2, -2);

                        if(versName=="v21")
                        {
                            errvpl->vers21 = new errBudgetRec;
                            errvpl->vers21 = errPlist.at(i)->errList.at(k);
                            continue;
                        }
                        if(versName=="v22")
                        {
                            errvpl->vers22 = new errBudgetRec;
                            errvpl->vers22 = errPlist.at(i)->errList.at(k);
                            continue;
                        }
                        if(versName=="v23")
                        {
                            errvpl->vers23 = new errBudgetRec;
                            errvpl->vers23 = errPlist.at(i)->errList.at(k);
                            continue;
                        }
                        if(versName=="v32")
                        {
                            errvpl->vers32 = new errBudgetRec;
                            errvpl->vers32 = errPlist.at(i)->errList.at(k);
                            continue;
                        }
                        if(versName=="v33")
                        {
                            errvpl->vers33 = new errBudgetRec;
                            errvpl->vers33 = errPlist.at(i)->errList.at(k);
                            continue;
                        }

                    }
                    eList << errvpl;
                }
            }

            sz0 = eList.size();
            for(i=0; i<sz0; i++)
            {

                ostr.clear();
                ostr = QString("%1|").arg(eList.at(i)->plName);
                if(eList.at(i)->vers21!=NULL) ostr.append(QString("%1|%2|%3|%4|").arg(eList.at(i)->vers21->xParams.N).arg(eList.at(i)->vers21->xParams.UWE).arg(eList.at(i)->vers21->yParams.N).arg(eList.at(i)->vers21->yParams.UWE));
                else ostr.append(QString("-|-|-|-|"));
                if(eList.at(i)->vers22!=NULL) ostr.append(QString("%1|%2|%3|%4|").arg(eList.at(i)->vers22->xParams.N).arg(eList.at(i)->vers22->xParams.UWE).arg(eList.at(i)->vers22->yParams.N).arg(eList.at(i)->vers22->yParams.UWE));
                else ostr.append(QString("-|-|-|-|"));
                if(eList.at(i)->vers23!=NULL) ostr.append(QString("%1|%2|%3|%4|").arg(eList.at(i)->vers23->xParams.N).arg(eList.at(i)->vers23->xParams.UWE).arg(eList.at(i)->vers23->yParams.N).arg(eList.at(i)->vers23->yParams.UWE));
                else ostr.append(QString("-|-|-|-|"));
                if(eList.at(i)->vers32!=NULL) ostr.append(QString("%1|%2|%3|%4|").arg(eList.at(i)->vers32->xParams.N).arg(eList.at(i)->vers32->xParams.UWE).arg(eList.at(i)->vers32->yParams.N).arg(eList.at(i)->vers32->yParams.UWE));
                else ostr.append(QString("-|-|-|-|"));
                if(eList.at(i)->vers33!=NULL) ostr.append(QString("%1|%2|%3|%4\n").arg(eList.at(i)->vers33->xParams.N).arg(eList.at(i)->vers33->xParams.UWE).arg(eList.at(i)->vers33->yParams.N).arg(eList.at(i)->vers33->yParams.UWE));
                else ostr.append(QString("-|-|-|-\n"));
                outf << ostr;

            }

        }
        break;
    }

    outFile.close();

    return 0;// a.exec();
}
