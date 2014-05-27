#include <QCoreApplication>
#include <QtSql>

#include "./../../libs/mpccat.h"
#include "./../../libs/comfunc.h"
#include <astro.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int i, sz;
    if(argc<2) exit(1);

    setlocale(LC_NUMERIC, "C");

    mpcrec mrec;
    mpccatL mCatl;
    QString tStr;
    int tpos;

    mCatl.init(argv[1]);

    qDebug() << QString("%1: %2 lines\n").arg(argv[1]).arg(mCatl.nline);
/*
    QFile catFile(argv[1]);
    catFile.open(QFile::ReadOnly);
    QTextStream catStm(&catFile);
  */
    srand(time(NULL));

    for(i=0;i<1000;i++)
    {
        tpos = rand()%mCatl.nline;
        if(mCatl.GetRec(tpos))
        {
            qDebug() << "error get line\n";
            continue;
        }

        qDebug() << QString("%4: Des'n: %1\tnum: %2\tname: %3\n").arg(mCatl.record->getProvnum()).arg(mCatl.record->getNum()).arg(mCatl.record->name).arg(tpos);

    }

/*
    do
    {
        tStr = catStm.readLine();
        qDebug() << tStr << "\n";

    }while(mrec.fromString(tStr.toAscii().data()));

    int nline = 0;
    while(!catStm.atEnd()&&nline<10)
    {
        qDebug() << QString("Des'n: %1\tnum: %2\tname: %3\n").arg(mrec.getProvnum()).arg(mrec.getNum()).arg(mrec.name);
        tStr = catStm.readLine();
        mrec.fromString(tStr.toAscii().data());
        nline++;
    }
*/

    
    return 0;//a.exec();
}
