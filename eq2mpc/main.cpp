#include <QtCore/QCoreApplication>
#include "./../libs/redStat.h"
#include "./../libs/mpcs.h"
#include "./../libs/astro.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    eqFile eqFile;
    eqFile.init(argv[1]);

    ocRec *ocRecord;

    mpcs mpFile;
    mpFile.init(argv[2]);

    mpc *mpcRecord;

    int i, szi;
    szi = eqFile.ocList.size();

    for(i=0;i<szi;i++)
    {
        ocRecord = eqFile.ocList.at(i);
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
