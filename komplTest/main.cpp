#include <QtCore/QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QString>

#include "./../libs/kompl.h"

int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);

    QTextStream stream(stdout);


    kompl x0, x1, x2;
    //x0 = new kompl;
    //x1 = new kompl;

    x0.setRe(1.0);
    x0.setIm(2.0);

    x1.setRe(3.0);
    x1.setIm(4.0);

    stream << QString("x0: %1 %2\n").arg(x0.Re()).arg(x0.Im());
    stream << QString("x1: %1 %2\n").arg(x1.Re()).arg(x1.Im());

    x2 = x0 + x1;
    stream << QString("x0 + x1: %1 %2\n").arg(x2.Re()).arg(x2.Im());

    x2 = x0 - x1;
    stream << QString("x0 - x1: %1 %2\n").arg(x2.Re()).arg(x2.Im());

    x2 = x0 * x1;
    stream << QString("x0 * x1: %1 %2\n").arg(x2.Re()).arg(x2.Im());

    x2 = x0 / x1;
    stream << QString("x0 / x1: %1 %2\n").arg(x2.Re()).arg(x2.Im());

    x2 = x0;
    x2 += x1;

    stream << QString("x0 += x1: %1 %2\n").arg(x2.Re()).arg(x2.Im());

    x2 = x0;
    x2 -= x1;

    stream << QString("x0 -= x1: %1 %2\n").arg(x2.Re()).arg(x2.Im());

    x2 = x0;
    x2 *= x1;

    stream << QString("x0 *= x1: %1 %2\n").arg(x2.Re()).arg(x2.Im());

    x2 = x0;
    x2 /= x1;

    stream << QString("x0 /= x1: %1 %2\n").arg(x2.Re()).arg(x2.Im());

    x2 = x0*10.0;

    stream << QString("x0*10: %1 %2\n").arg(x2.Re()).arg(x2.Im());

    x2 = 10*x0;

    stream << QString("10*x0: %1 %2\n").arg(x2.Re()).arg(x2.Im());

    x2 = x1/2;

    stream << QString("x1/2: %1 %2\n").arg(x2.Re()).arg(x2.Im());


    x2 = x1;
    x2 += 2;

    stream << QString("x1+=2: %1 %2\n").arg(x2.Re()).arg(x2.Im());

    x2 = x1;
    x2 -= 2;

    stream << QString("x1-=2: %1 %2\n").arg(x2.Re()).arg(x2.Im());

    x2 = x1;
    x2 *= 2;

    stream << QString("x1*=2: %1 %2\n").arg(x2.Re()).arg(x2.Im());

    x2 = x1;
    x2 /= 2;

    stream << QString("x1/=2: %1 %2\n").arg(x2.Re()).arg(x2.Im());


    //stream << QString("x0: %1 %2\n").arg(x0->Re()).arg(x0->Im());
    //stream << QString("x1: %1 %2\n").arg(x1->Re()).arg(x1->Im());






    return 0;//a.exec();
}
