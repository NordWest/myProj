#include <QCoreApplication>

#include "./../../libs/skyarealf.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int i, pMax, pCur, nMax;

    pMax = 100000;
    nMax = 300;

    QStringList namesList;
    QVector <int> pList;

    QFile resFile("./randMCAT.txt");
    resFile.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream resStm(&resFile);

    mpccatL mCat;
    QString name;

    mCat.init("./../../data/cats/MPCCAT/MPCORB.DAT");

    srand(time(NULL));

    for(i=0; i<nMax; i++)
    {
        do
        {
            pCur = rand() % pMax;
        }while(pList.contains(pCur));
        pList << pCur;

        mCat.GetRec(pCur+1);

        resStm << QString("%1\n").arg(QString(mCat.record->name).simplified());

    }


    resFile.close();

    
    return 0;//a.exec();
}
