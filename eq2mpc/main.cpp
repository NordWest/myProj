#include <QtCore/QCoreApplication>
#include "./../libs/redStat.h"
#include "./../libs/mpcs.h"
#include "./../astro/astro.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    eqFile eqFile;
    eqFile.init(argv[1]);

    ocRec *ocRecord;
    mpc *mpcRecord;
    mpccat mpoCat;

    mpcs mpFile;
    mpFile.init(argv[2]);

    if(mpoCat.init(mpcOrbCat.toAscii().data())) qDebug() << QString("mpoCat init error!\n");

    objName = objStat.objList.at(i)->objName.simplified();
    catName = objStat.objList.at(i)->catName.simplified();
    catMagName = objStat.objList.at(i)->catMagName.simplified();



    int i, szi;
    szi = eqFile.ocList.size();

    for(i=0;i<szi;i++)
    {
        ocRecord = eqFile.ocList.at(i);
        ocRecord->rec2MPC();
        mpcRecord = new mpc;
        mpcRecord->d = ocRecord->de;
        mpcRecord->eJD = mjd2jd(ocRecord->MJday);
        mpcRecord->magn = ocRecord->mag0;
        mpcRecord->num = ocRecord->de;
        mpcRecord->d = ocRecord->de;
        mpcRecord->d = ocRecord->de;
    }




    return 0;//a.exec();
}
