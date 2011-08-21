#include "mpcqt.h"


void mpcqtrec::fromString(QString inStr)
{
    string.clear();
    string.append(inStr);
}

void mpcqtrec::toString(QString *outStr)
{
    outStr->clear();
    outStr->append(string);
}

mpcqt::mpcqt()
{
    record = new mpcqtrec;

}

void mpcqt::initMPCCAT(QString catFileName)
{
    fileName.clear();
    fileName.append(catFileName);
}

int mpcqt::readRecord(int pos)
{
    QFile tFile(fileName);
    if(!tFile.open(QIODevice::ReadOnly | QIODevice::Text)) return 1;
    QTextStream tStm(&tFile);

    tStm.seek((pos-1)*MPC_SIZE);
    record->fromString(tStm.readLine(MPC_SIZE));

    return 0;
}
