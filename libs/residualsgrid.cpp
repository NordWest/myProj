#include "residualsgrid.h"

residualsGrid::residualsGrid()
{
    xLevels = yLevels = mLevels = NULL;
}

int residualsGrid::detPos(int x, int y, int m, int &pos)
{
    pos = m*(xLevelsNum*yLevelsNum) + y*(xLevelsNum) + x;
    if(pos<0||pos>=nums) return 1;
    return 0;
}

int residualsGrid::detCoords(int &xPos, int &yPos, int &mPos, int pos)
{
    if(pos>=nums) return 1;
    mPos = floor(pos/(1.0*xLevelsNum*yLevelsNum));
    if(mPos>=mLevelsNum) return 2;
    pos -= mPos*xLevelsNum*yLevelsNum;
    yPos = floor(pos/(xLevelsNum*1.0));
    if(yPos>=yLevelsNum) return 3;
    pos -= yPos*xLevelsNum;
    xPos = pos;
    if(xPos>=xLevelsNum) return 4;
    return 0;
}

int residualsGrid::detCenters(double &x, double &y, double &m, int pos)
{
    int xPos, yPos, mPos;
    if(detCoords(xPos, yPos, mPos, pos)) return 1;
    x = (xLevels[xPos]+xLevels[xPos+1])/2.0;
    y = (yLevels[yPos]+yLevels[yPos+1])/2.0;
    m = (mLevels[mPos]+mLevels[mPos+1])/2.0;

    return 0;
}

int residualsGrid::initLevels(double *xLev, int xLevNum, double *yLev, int yLevNum, double *mLev, int mLevNum)
{
    int i, sz;
    residualFile *resFileRec;
    xLevelsNum = xLevNum;
    xLevels = new double[xLevNum];
    for(i=0; i<xLevelsNum; i++) xLevels[i] = xLev[i];
    yLevelsNum = yLevNum;
    yLevels = new double[yLevNum];
    for(i=0; i<yLevelsNum; i++) yLevels[i] = yLev[i];
    mLevelsNum = mLevNum;
    mLevels = new double[mLevNum];
    for(i=0; i<mLevelsNum; i++) mLevels[i] = mLev[i];
    nums = xLevelsNum*yLevelsNum*mLevelsNum;

    for(i=0; i<nums; i++)
    {
        resFileRec = new residualFile;
        rfList << resFileRec;
    }

}

int residualsGrid::addPointXY(residualsRec *resRec)
{
    int i, xPos, yPos, mPos, pos;
    double x, y, m;

    mPos = -1;
    m = resRec->catMag();
    for(i=0; i<mLevelsNum-1; i++)
    {

        if((m<mLevels[i+1])&&(m>=mLevels[i]))
        {
            mPos = i;
            break;
        }
    }
    if(mPos==-1) return 1;

    yPos = -1;
    y = resRec->catY();
    for(i=0; i<yLevelsNum-1; i++)
    {

        if((y<yLevels[i+1])&&(y>=yLevels[i]))
        {
            yPos = i;
            break;
        }
    }
    if(yPos==-1) return 1;

    xPos = -1;
    x = resRec->catX();
    for(i=0; i<xLevelsNum-1; i++)
    {

        if((x<xLevels[i+1])&&(x>=xLevels[i]))
        {
            xPos = i;
            break;
        }
    }
    if(xPos==-1) return 1;

    if(detPos(xPos, yPos, mPos, pos)) return 1;
    rfList.at(pos)->resList << resRec;
    return 0;
}

void residualsGrid::detSigmaXY(double sg, double proofP, int isRef)
{
    for(int i=0; i<nums; i++) rfList.at(i)->detSigmaXY(sg, proofP, isRef);
}

void residualsGrid::detStatXY(int isRef)
{
    for(int i=0; i<nums; i++) rfList.at(i)->detStatXY(isRef);
}

int residualsGrid::getResFile(residualFile *resFileRec, int x, int y, int m)
{
    int pos;
    if(detPos(x, y, m, pos)) return 1;
    resFileRec = rfList.at(pos);
    return 0;
}
