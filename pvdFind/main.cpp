#include <QtCore/QCoreApplication>
#include <QList>


#include "./../libs/pvdK.h"
#include "./../libs/ast.h"
#include "./../libs/coord.h"




int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ast *obsFile = new ast;
    obsFile->initFile("121.gmt");

    List4 *cod = new List4(3);
    obsFile->GetCoordList(cod);

    pvdK *pK = new pvdK(1);

    pK->initCoord(cod);
    pK->findPVD();

    char *tstr = new char[256];
//    rad2hms_str(pK->r->val, tstr);
    qDebug() << QString("ra:\t%1\t+-%2\n").arg(pK->r->val).arg(rad2mas(pK->r->acc));
//    rad2gms_str(pK->d->val, tstr);
    qDebug() << QString("de:\t%1\t+-%2\n").arg(pK->d->val).arg(rad2mas(pK->d->acc));
    qDebug() << QString("psi:\t%1\t+-%2\n").arg(rad2grad(pK->psi->val)).arg(rad2mas(pK->psi->acc));
    qDebug() << QString("m:\t%1\t+-%2\n").arg(pK->m->val).arg(rad2mas(pK->m->acc));
    qDebug() << QString("mt:\t%1\t+-%2\n").arg(pK->mt->val).arg(rad2mas(pK->mt->acc));
    qDebug() << QString("c:\t%1\t+-%2\n").arg(pK->c->val).arg(rad2mas(pK->c->acc));






//    return a.exec();
    return 0;
}
