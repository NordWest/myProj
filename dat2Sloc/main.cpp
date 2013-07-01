#include <QCoreApplication>

#include "./../libs/comfunc.h"
#include "./../libs/observatory.h"
#include <astro.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    setlocale(LC_NUMERIC, "C");

    if(argc<5) return 1;

    int yr, mth;
    double day, jDay, Sloc;

    yr = atoi(argv[1]);
    mth = atoi(argv[2]);
    day = atof(argv[3]);
    QString obsCode = QString(argv[4]);

   observatory obs;
   obs.init("./Obs.txt", OBS_SIZE);
   obs.getobsynumO(obsCode.toAscii().data());

   dat2JD(&jDay, yr, mth, day);

   //jDay -= dUT1();

   UTC2s(jDay, obs.record->Long, &Sloc);

   Sloc = Sloc/2.0/PI;

   int h, m;
   double sec;

   Sloc *= 24;
   h = floor(Sloc);
   Sloc = (Sloc-h)*60;
   m = floor(Sloc);
   sec = (Sloc-m)*60;

   printf("Sloc: %02d %02d %05.2f\n", h, m, sec);


    
    return 0;//a.exec();
}
