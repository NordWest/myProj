#include "platesStat.h"

statRec::statRec()
{
//        nums = new statBox[n];
//        for(int i=0; i<n; i++){nums[i]->num = 0; nums[i]->name = 0;}
};

statRec::~statRec()
{
//        delete [] nums;
};

void statRec::addNum(int name)
{
    int i, sz;
    sz = boxes.size();
    for(i=0; i<sz; i++)
    {
        if(boxes.at(i)->name==name)
        {
            boxes.at(i)->num++;
            return;
        }
    }
    statBox *tbox = new statBox;
    tbox->num = 1;
    tbox->name = name;
    boxes << tbox;
}

int statRec::getNum(int name)
{
    int i, sz;
    sz = boxes.size();
    for(i=0; i<sz; i++)
    {
        if(boxes.at(i)->name==name) return boxes.at(i)->num;
    }
    return 0;
}

void statRec::sortName()
{
    int i, j, sz;
    sz = boxes.size();

    for(i=0; i<sz-1; i++)
    {
        for(j=i+1; j<sz; j++)
        {
            if(boxes.at(i)->name>boxes.at(j)->name) boxes.swap(i, j);
        }
    }
}

void plateRec::copyTo(plateRec *plr)
{
    plr->astName = QString(astName);
    plr->desc = QString(desc);
    plr->jD = jD;
    plr->name = QString(name);
    plr->num = num;
};

void plateRec::rec2s(QString *rstr)
{
    char *tstr = new char[255];
    rstr->clear();
    dat2YMD_str(jD, tstr);
    rstr->append(QString("%1 %2 %3 %4\n").arg(jD, 8, 'f', 2).arg(name).arg(tstr).arg(desc));
    delete [] tstr;
}

void plateRec::s2rec(QString rstr)
{
    QString tLine;
    int y, m;
    double d;
    //qDebug() << QString("rstr: %1\n").arg(rstr);
    name = rstr.section(" ", 0, 0);
   // qDebug() << QString("Plate name: %1\n").arg(name);

    tLine = rstr.section(" ", 1, 1, QString::SectionSkipEmpty);
    y = tLine.toInt();
    tLine = rstr.section(" ", 2, 2, QString::SectionSkipEmpty);
    m = tLine.toInt();
    tLine = rstr.section(" ", 3, 3, QString::SectionSkipEmpty);
    d = tLine.toDouble();

    dat2JD(&jD, y, m, d);
    //qDebug() << QString("dateons: %1: %2 %3 %4\n").arg(jD).arg(y).arg(m).arg(d);

    desc = rstr.section("   ", -1, -1);
    desc.chop(0);
    desc2NumName(desc, &num, &astName);
}

int sortPl(QList<plateRec*> *plL)
{
        int i, j, num, pm;

        plateRec *temp;
        double val;

        num = plL->size();

        for(i=0; i<num-1; i++)
        {
                val = plL->at(i)->jD;
                pm = i;
                for(j=i+1; j<num; j++)
                {
                        if(val>plL->at(j)->jD)
                        {
                                val = plL->at(j)->jD;
                                pm = j;
//				out_stream << "val= " << QString("%1").arg(val, 8, 'f', 2) << "\n";
                        }
                }
//		out_stream << QString("%1").arg(i, 5, 10) << ":";// << pm << "\n";
//		out_stream << QString("%1").arg(plL[i]->jD, 8, 'f', 2) << ":" << QString("%1").arg(plL[pm]->jD, 8, 'f', 2) << "\n";
                if(pm!=i) plL->move(pm, i);
//		out_stream << QString("%1").arg(plL[i]->jD, 8, 'f', 2) << "\n";// << ":" << QString("%1").arg(plL[pm]->jD, 8, 'f', 2) << "\n";

        }

//	for(i=0; i<plL.size(); i++) out_stream << QString("%1").arg(plL[i]->jD, 8, 'f', 2) << " " << plL[i]->name << "\n";
        return 0;
}

void plateList::initPlatesFromFile(QString fileName)
{
    QFile wFile;
    QTextStream in_stream;
    QStringList iList;
    QString iLine;
    QString tLine;
    int y, m;
    int num, num1;
    double d, jD;
    int j, i;
    //QStringList iList;
    //QString iLine;

    //QTextCodec* codec1 = QTextCodec::codecForName("Windows-1251");

    plateRec *pl;

    wFile.setFileName(fileName);
    wFile.open(QIODevice::ReadOnly);

    in_stream.setDevice(&wFile);
    //while(!in_stream.atEnd()) iList << in_stream.readLine();
    qDebug() << "init filename: " << fileName << "\n";
    int k = iList.size();
    while(!in_stream.atEnd())
    {
            iLine = in_stream.readLine();
            if(iLine.size()<10) continue;
            pl = new plateRec;
            pl->s2rec(iLine);/*>name = iLine.section(" ", 0, 0);

            tLine = iLine.mid(8, 4);
            y = tLine.toInt();
            tLine = iLine.mid(13, 2);
            m = tLine.toInt();
            tLine = iLine.mid(16, 2);
            d = tLine.toDouble();
            dat2JD(&jD, y, m, d);
            pl->jD = jD;

            pl->desc = iLine.section("   ", -1, -1);
            pl->desc.chop(0);
            desc2NumName(pl->desc, &pl->num, pl->astName);
*/
            plates << pl;
    }

    qDebug() << QString("plList.num= %1\n").arg(plates.size());

    iList.clear();
    wFile.close();
    qDebug() << "return\n";
}

void plateList::remFilePlates(QString remFileName)
{
    QFile eFile(remFileName);
    QTextStream exc_stream;
    QStringList iList;
    QString iLine;
    int k, i, j, num;
    qDebug() << QString("Remove %1 file\n").arg(remFileName);
    if(!eFile.open(QIODevice::ReadOnly))
    {
            qDebug() << "\nCan't open excluded plates file\n";
    }
    else
    {
            exc_stream.setDevice(&eFile);
            iList.clear();
            while(!exc_stream.atEnd()) iList << exc_stream.readLine();
            k = iList.size();
            QString eStrName;
            for(j=0; j<k; j++)
            {
                    iLine = iList.at(j);

                    eStrName = iLine.section(" ", 0, 0);
                    num = plates.size();
                    for(i=0; i<num; i++)
                    {
                            if(plates[i]->name == eStrName)
                            {
                                    plates.removeAt(i);
                                    break;
                            }
                    }
            }
    }
}

void plateList::task0(int nodeNum)
{
    double t, t0, t1, vMin;
    int pMin, i, num;
    QList <double> nodes;
    double node;
    plateRec* resRec;
    QList <plateRec*> plList;
    char *tstr = new char[255];
    qDebug() << QString("task0\n");

    num = plates.size();
    qDebug() << QString("plates.size= %1\n").arg(num);
    for(i=0; i<num; i++)
    {
        resRec = new plateRec;
        plates[i]->copyTo(resRec);
        //qDebug() << QString("resRec->jD= %1\n").arg(resRec->jD);
        plList << resRec;
    }

    plates.clear();

    num = plList.size();
   /* qDebug() << QString("plList.size= %1\n").arg(num);
    qDebug() << QString("plList.at(num-1)->jD= %1\n").arg(plList.at(num-1)->jD);
    qDebug() << QString("plList.at(0)->jD= %1\n").arg(plList.at(0)->jD);*/

    t1 = plList.at(num-1)->jD;
    t0 = plList[0]->jD;

    double dT = (t1-t0)/(nodeNum*1.0);

    qDebug() << "nodeNum= " << nodeNum << "\n";
    dat2YMD_str(t0, tstr);
    qDebug() << "t0\t" << QString("%1\t%2").arg(t0, 10, 'f', 2).arg(tstr) << "\n";
    dat2YMD_str(t1, tstr);
    qDebug() << "t1\t" << QString("%1\t%2").arg(t1, 10, 'f', 2).arg(tstr) << "\n";
    qDebug() << "dT= " << dT << "\n";

    for(i=0; i<nodeNum; i++)
    {
            node = t0 + i*dT;
            nodes << node;
    }


    for(t=t0; t<t1; t+=dT)
    {
        vMin = t1-t0;
        pMin = -1;
        num = plList.size();
        for(i=0; i<num; i++)
        {
            if(vMin>fabs(plList[i]->jD-t))
            {
                vMin = fabs(plList[i]->jD-t);
                pMin = i;

            }
        }
        resRec = new plateRec;
        if((pMin!=-1)&&(vMin<=dT/2))
        {
            plList[pMin]->copyTo(resRec);

            plList.removeAt(pMin);
        }
        else
        {
            resRec->desc.append("None");
            resRec->name.append("Noname");
            resRec->jD = 0.0;
        }

        plates << resRec;
    }

    qDebug() << QString("plates.size= %1\n").arg(plates.size());
    qDebug() << QString("End of task0\n");

 //   out_stream << "res\n";


}

void plateList::findNodesList(QList <plateRec*> selList, double dT)
{
    qDebug() << QString("findNodesList\n");
    int numPl, numSel;
    int i, j;
    numPl = plates.size();

    qDebug() << QString("plates.size= %1\n").arg(numPl);
    for(i=numPl-1; i>=0; i--)
    {
        numSel = selList.size();
        for(j=numSel-1; j>=0; j--)
        {
            if(fabs(plates[i]->jD-selList[j]->jD)<=dT)
            {
                selList.at(j)->copyTo(plates.at(i));
                plates.at(i)->desc.clear();
                plates.at(i)->desc.append("Scanned");
                selList.removeAt(j);
                break;
            }
        }
    }
    qDebug() << QString("plates.size= %1\n").arg(plates.size());
    qDebug() << QString("End of findNodesList\n");
}

