#include <QtCore/QCoreApplication>
#include "./../libs/observ.h"
#include "./../libs/skyarea.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    setlocale(LC_NUMERIC, "C");

    observ obsPos;

    obsPos.init("./Obs.txt", "./../../data/cats/bin1940_2020.lin.405");

    obsPos.set_obs_parpam(GEOCENTR_NUM, CENTER_BARY, SK_EKVATOR, "084");


    IList iList;
    OrbCat iCat;

    iList.init("ini.lst");
    iCat.init("ini.cat");

    int szi, i;
    szi = iList.nstr;

    for(i=0; i<nstr; i++)
    {

    }


    return 0;//a.exec();
}
