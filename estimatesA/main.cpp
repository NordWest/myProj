#include <QCoreApplication>
#include <QtCore>
#include "math.h"
//#include "astro.h"
#include "./../libs/astro.h"
#include "./../libs/redStat.h"

//#include "./../libs/redStat.h"
//#include "./../../libs/orbcat.h"

//#include "./../libs/mb.hpp"
//#include "ccdmeasurements.h"




int main(int argc, char *argv[])//.exe fname typeoftask colSep "colNums,"
    {
		QCoreApplication app(argc, argv);

                setlocale(LC_NUMERIC, "C");
                QString codecName;
                #if defined(Q_OS_LINUX)
                codecName = "UTF-8";
                #elif defined(Q_OS_WIN)
                codecName = "CP1251";
                #endif

		//BEGIN settings
                //QSettings *settings = new QSettings("estimates.ini",QSettings::IniFormat);
		//[%general]
                //QString colSep = settings->value("general/filter").toString();//filter
		//END settings
		QTextStream stream(stdout); 
		stream << "\n estimates started....\n";
		//BEGIN arguments

                QString fname = QString(argv[1]);
                QFileInfo fi(fname);
                QString rejName = QString("%1%2_rej.txt").arg(fi.absolutePath()).arg(fi.completeBaseName());
                int typeoftask = QString(argv[2]).toInt();
                QString colSep = "|";

                QList <ocRec*> ocRej;
                int i;
                QString tStr;




                eqFile eqData;
                eqData.init(argv[1]);

                switch(typeoftask)
                {
                    case 0://mean and its standard error
                        {
                            eqData.countCols(QString(argv[3]));
                        }
                        break;
                    case 1:
                        {
                            eqData.countMM();
                        }
                        break;
                    case 2:
                        {
                            eqData.do3sigma(0.05, 3.0, &ocRej);
                            QFile oFile(rejName);
                            oFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
                            QTextStream oStm(&oFile);
                            for(i=0; i<ocRej.size(); i++)
                            {
                                ocRej.at(i)->rec2s(&tStr);
                                oStm << tStr << "\n";
                            }
                            oFile.close();
                        }
                        break;
                }
                eqData.save();

		stream << "\n estimates finished...\n";
		return 0;  
	}
