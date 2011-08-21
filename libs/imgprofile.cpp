#include "imgprofile.h"

imgProfile::imgProfile()
{
//    int i;
//    for(i=0; i<PVECT_SIZE; i++) p[i] = 0.0;
    model = -1;
    mask = "";
    fName = "";
}

imgProfile::imgProfile(QString fname)
{
    imgProfile();

    fName = fname;
    QFile fM(fName);
    bool isOpen = fM.open(QIODevice::ReadOnly | QIODevice::Text);
    QString sStr;
    QStringList sL, mL;
    double *p;
    int sz, szML, i;
    if(isOpen)
    {
        pList.clear();
        QTextStream input(&fM);

        sStr = input.readLine();
        sL = sStr.split(" ");
        QString tstr = sL[0].mid(1);
        model = tstr.toInt();
        mask = sL[1];
        mL = mask.split("|");
        szML = mL.size();
        while(!input.atEnd())
        {
            p = new double[PVECT_SIZE];
            for(i=0; i<PVECT_SIZE; i++) p[i] = 0.0;
            sStr = input.readLine();
            sL = sStr.split("|");
            if(sL.size()<szML) return;
            for(i=0; i<szML; i++)
            {
                p[mL[i].toInt()] = sL[i].toDouble();
            }
        }
    }
}

int imgProfile::getP(double *p, int pos)
{
    if(pos<0||pos>=pList.size()) return 1;
    p = pList.at(pos);
    return 0;
}

int imgProfile::setP(double *p, int pos)
{
    int i;
    if(pos<0||pos>=pList.size()) return 1;
    double *pp = new double[PVECT_SIZE];
    for(i=0; i<PVECT_SIZE; i++) pp[i] = p[i];
    pList.insert(pos, pp);
}

void imgProfile::setMask(QString smask)
{
    mask = smask;
}

void imgProfile::setModel(int mod)
{
    model = mod;
}

void imgProfile::pushPoint(int x, int y, int m)
{
    int i;
    double *pv = new double[PVECT_SIZE];
    for(i=0; i<PVECT_SIZE; i++) pv[i] = 0;
    pv[18] = m;
    pv[0] = x;
    pv[1] = y;

//    int pos = pList.size();
    pList.push_back(pv);
 //   setP(pv, pos);
}

int imgProfile::save()
{
    QString sStr;
    QStringList sL, mL;
    int sz, szML, i;
    QFile fM(fName);
    bool isOpen = fM.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    if(isOpen)
    {
         QTextStream output(&fM);

         output << QString("#%1|%2\n").arg(model).arg(mask);

         mL = mask.split("|");
         szML = mL.size();
         sz = pList.size();

         switch(model)
         {
         }

    }
}

int imgProfile::saveAs(QString fname)
{
    return 0;
}
