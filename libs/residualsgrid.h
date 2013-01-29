#ifndef RESIDUALSGRID_H
#define RESIDUALSGRID_H

#ifndef REDSTAT_H
#include "redStat.h"
#endif

class residualsGrid
{
public:
    QList <residualFile*> rfList;
    double *mLevels, *xLevels, *yLevels;
    int mLevelsNum, xLevelsNum, yLevelsNum, nums;
    residualsGrid();

    int detPos(int x, int y, int m, int &pos);
    int detCoords(int &xPos, int &yPos, int &mPos, int pos);
    int detCenters(double &x, double &y, double &m, int pos);
    int initLevels(double *xLev, int xLevNum, double *yLev, int yLevNum, double *mLev, int mLevNum);
    int addPointXY(residualsRec *resRec);
    void remSigmaXY(double sg, double proofP=0.0, int isRef=0);
    void detStatXY(int isRef = 0);

    int getResFile(residualFile *resFileRec, int x, int y, int m);
};

#endif // RESIDUALSGRID_H
