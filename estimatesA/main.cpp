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

int countColStat(colRec *cRec, double *vect, int sz)
{
    int i, sz;

    double mean, rmsMean, rmsOne;

    mean = rmsMean = rmsOne = 0.0;

    for(i=0; i<sz; i++) mean += vect[i];
    mean /= 1.0*sz;

    for(i=0; i<sz; i++) rmsOne += pow(mean - vect[i], 2.0);
    rmsOne = sqrt(rmsOne/(sz-1.0));
    rmsMean = rmsOne/sqrt(sz);

    cRec->num = sz;
    cRec->mean = mean;
    cRec->rmsMean = rmsMean;
    cRec->rmsOne = rmsOne;

    return 0;
}


int main(int argc, char *argv[])//.exe fname typeoftask colSep "colNums,"
    {
		QCoreApplication app(argc, argv);
		//BEGIN settings
                //QSettings *settings = new QSettings("estimates.ini",QSettings::IniFormat);
		//[%general]
                //QString colSep = settings->value("general/filter").toString();//filter
		//END settings
		QTextStream stream(stdout); 
		stream << "\n estimates started....\n";
		//BEGIN arguments
                QTextCodec *codec1 = QTextCodec::codecForName("Windows-1251");
                Q_ASSERT( codec1 );
                QString fname = codec1->toUnicode(argv[1]);
                int typeoftask = codec1->toUnicode(argv[2]).toInt();
                QString colSep = codec1->toUnicode(argv[3]);
                QStringList colNums;
                colNums = codec1->toUnicode(argv[4]).split(",");
                int colSz = colNums.size();
                //QString dataformat = QString(argv[2]);
                //QString str = QString(argv[3]);

                QFile iFile(fname);
                iFile.open(QIODevice::ReadWrite | QIODevice::Text);
                QTextStream iStm(&iFile);



                QList <double*> ocList;
                double *tvect;
                QString tStr;

                while(!iStm.atEnd())
                {
                    tvect = new double[colSz];
                    tStr = iStm.readLine();
                    for(i=0; i<colSz; i++) tvect[i] = tStr.section(colSep, colNums[i], colNums[i], QString::SectionSkipEmpty).toDouble();

                    ocList << tvect;

                };


                colRec *cRec;
                moveModelRec *mmRec;

                int sz0 = ocList.size();
                tvect = new double[sz0];
                for(i=0; i<colSz; i++)
                {

                    for(j=0; j<sz0; j++) tvect[j] = ocList[i][j];



                }

                switch(typeoftask)
                {
                    case 0://mean and its standard error
                        {
                            eqData->countCols(dataformat);
                        }
                        break;
                    case 1:
                        {
                            eqData->countMM();
                        }
                        break;
                }
                eqData->save();
                //const char *fname;
                //QList <colRec *> cols;
                //colRec *cRec;
                /*int j, sz, sz1, k;
		QByteArray tmp;
		QStringList columns = dataformat.split(",");
//

		for(int i=0;i<outflist.count();i++)
		{
			stream << outflist[i] << "\n";
			dataLines.clear();
			switch(typeoftask)
			{
				case 0://mean and its standard error
				{
					eqData->clear();
					tmp = QString(folder+"/"+outflist[i]).toAscii();
					fname = tmp.data();
					eqData->init(fname);

                                        eqData->countCols(dataformat);
                                        /*
					cols.clear();
					sz = eqData->ocList.size();
					if(sz>2)
					{
						sz1 = columns.count();
						for(j=0;j<sz1;j++)
						{
							str = columns[j];
							cRec = new colRec;
							cRec->colNum = str.toInt();
//							means<<0;
//							serrors<<0;
							cols << cRec;
						}
						
						
						for(j=0;j<sz;j++)
						{
							for(k=0;k<sz1;k++)
							{
								switch(cols[k]->colNum)
								{
									case 0:
									cols[k]->mean += eqData->ocList[j]->MJday;
									cols[k]->num++;
									break;
									case 1:
									cols[k]->mean += eqData->ocList[j]->ra;
									cols[k]->num++;
									break;
									case 2:
									cols[k]->mean += eqData->ocList[j]->de;
									cols[k]->num++;
									break;
									case 3:
									cols[k]->mean += eqData->ocList[j]->mag0;
									cols[k]->num++;
									break;
									case 4:
                                                                        cols[k]->mean += eqData->ocList[j]->ocRaCosDe;
									cols[k]->num++;
									break;
									case 5:
									cols[k]->mean += eqData->ocList[j]->ocDe;
									cols[k]->num++;
									break;
									case 6:
									cols[k]->mean += eqData->ocList[j]->ocMag;
									cols[k]->num++;
									break;
									case 7:
									cols[k]->mean += eqData->ocList[j]->ra_oc;
									cols[k]->num++;
									break;
									case 8:
									cols[k]->mean += eqData->ocList[j]->dec_oc;
									cols[k]->num++;
									break;
									case 9:
									cols[k]->mean += eqData->ocList[j]->topDist;
									cols[k]->num++;
									break;
									case 10:
									cols[k]->mean += eqData->ocList[j]->muRa;
									cols[k]->num++;
									break;
									case 11:
									cols[k]->mean += eqData->ocList[j]->muDe;
									cols[k]->num++;
									break;
									case 12:
									cols[k]->mean += eqData->ocList[j]->Vr;
									cols[k]->num++;
									break;
									case 13:
									cols[k]->mean += eqData->ocList[j]->phase;
									cols[k]->num++;
									break;
									case 14:
									cols[k]->mean += eqData->ocList[j]->elong;
									cols[k]->num++;
									break;
									case 17:
									cols[k]->mean += eqData->ocList[j]->expTime;
									cols[k]->num++;
									break;
								}
							}
						}
						for(k=0;k<sz1;k++) cols[k]->mean /= cols[k]->num;
						
						for(j=0;j<sz;j++)
						{
							for(k=0;k<sz1;k++)
							{
								switch(cols[k]->colNum)
								{
									case 0:
                                                                        cols[k]->rmsOne += pow(cols[k]->mean-eqData->ocList[j]->MJday, 2.0);
									break;
									case 1:
                                                                        cols[k]->rmsOne += pow(cols[k]->mean-eqData->ocList[j]->ra, 2.0);
									break;
									case 2:
                                                                        cols[k]->rmsOne += pow(cols[k]->mean-eqData->ocList[j]->de, 2.0);
									break;
									case 3:
                                                                        cols[k]->rmsOne += pow(cols[k]->mean-eqData->ocList[j]->mag0, 2.0);
									break;
									case 4:
                                                                        cols[k]->rmsOne += pow(cols[k]->mean-eqData->ocList[j]->ocRa, 2.0);
									break;
									case 5:
                                                                        cols[k]->rmsOne += pow(cols[k]->mean-eqData->ocList[j]->ocDe, 2.0);
									break;
									case 6:
                                                                        cols[k]->rmsOne += pow(cols[k]->mean-eqData->ocList[j]->ocMag, 2.0);
									break;
									case 7:
                                                                        cols[k]->rmsOne += pow(cols[k]->mean-eqData->ocList[j]->ra_oc, 2.0);
									break;
									case 8:
                                                                        cols[k]->rmsOne += pow(cols[k]->mean-eqData->ocList[j]->dec_oc, 2.0);
									break;
									case 9:
                                                                        cols[k]->rmsOne += pow(cols[k]->mean-eqData->ocList[j]->topDist, 2.0);
									break;
									case 10:
                                                                        cols[k]->rmsOne += pow(cols[k]->mean-eqData->ocList[j]->muRa, 2.0);
									break;
									case 11:
                                                                        cols[k]->rmsOne += pow(cols[k]->mean-eqData->ocList[j]->muDe, 2.0);
									break;
									case 12:
                                                                        cols[k]->rmsOne += pow(cols[k]->mean-eqData->ocList[j]->Vr, 2.0);
									break;
									case 13:
                                                                        cols[k]->rmsOne += pow(cols[k]->mean-eqData->ocList[j]->phase, 2.0);
									break;
									case 14:
                                                                        cols[k]->rmsOne += pow(cols[k]->mean-eqData->ocList[j]->elong, 2.0);
									break;
									case 17:
                                                                        cols[k]->rmsOne += pow(cols[k]->mean-eqData->ocList[j]->expTime, 2.0);
									break;
								}
							}
						}
						
						for(k=0;k<sz1;k++)
						{
                                                        cols[k]->rmsOne = sqrt(cols[k]->rmsOne/(cols[k]->num-1.0));
                                                        cols[k]->rmsMean = cols[k]->rmsOne/sqrt(cols[k]->num);
							str.clear();
							cols[k]->rec2s(&str);
							qDebug() << k << ": " << str << "\n";
							eqData->setColRec(cols[k]);
						}
						qDebug() << "\nsave\n";
//						eqData->sortColList();
						eqData->save();
					}
                                        /
				}
				break;
/*				case 0://mean and its standard error
				{
					QFile dataFile(folder+"/"+outflist[i]);
					dataFile.open(QIODevice::ReadWrite| QIODevice::Text);
					QTextStream dataStream;
					dataStream.setDevice(&dataFile);
					while (!dataStream.atEnd())
					{
						dataLine = dataStream.readLine();
						if(dataLine[0]!='#')dataLines << dataLine;
					}
					if(dataLines.count()>2)
					{
						QVector<double> means;
						QVector<double> serrors;
						QStringList columns = dataformat.split(",");
						QVector<int> cols;
						for (int j=0;j<columns.count();j++)
						{
							str = columns[j];
							cols<<str.toInt();
							means<<0;
							serrors<<0;
						}
						//BEGIN mean
						for (int j=0;j<dataLines.count();j++)
						{
							dataLine = dataLines[j];
							for(int k=0;k<cols.count();k++)
							{
								elS = dataLine.section('|',cols[k],cols[k]);
								means[k] = means[k]+elS.toDouble();
							}
						}
						for(int k=0;k<cols.count();k++) means[k] = means[k]/dataLines.count();
						//END mean
						//BEGIN standart error
						for (int j=0;j<dataLines.count();j++)
						{
							dataLine = dataLines[j];
							for(int k=0;k<cols.count();k++)
							{
								elS = dataLine.section('|',cols[k],cols[k]);
								serrors[k] =serrors[k] + pow(elS.toDouble()-means[k],2);
							}
						}
						for(int k=0;k<cols.count();k++) 
							serrors[k] = sqrt(serrors[k]/(dataLines.count()-1));
						//END standart error
						str = "";
						for(int k=0;k<cols.count();k++)
						{
							str = str+ "\%"+QString( "%1" ).arg( cols[k],3,10,QLatin1Char( ' ' ))+"|"+
								QString( "%1" ).arg(means[k],nd,'f',ndap,QLatin1Char( ' ' ))+"|"+
								QString( "%1" ).arg(serrors[k]/sqrt(dataLines.count()),nd,'f',ndap,QLatin1Char( ' ' ))+"|"+
								QString( "%1" ).arg(serrors[k],nd,'f',ndap,QLatin1Char( ' ' ))+"|"+
								QString( "%1" ).arg( dataLines.count(),5,10,QLatin1Char( ' ' ))+"\n";
						}
						dataStream << str;
					}
					dataFile.close();
					break;
				}*/
/*				case 1://linear motion and velocity
				{
					eqData->clear();
					tmp = QString(folder+"/"+outflist[i]).toAscii();
					fname = tmp.data();
					eqData->init(fname);
					sz = eqData->ocList.size();
					if(sz>2)
					{
						//BEGIN columns
						QStringList columns = dataformat.split(",");
						str = columns[0];
						int tc=str.toInt();//time column
						str = columns[1];
						int xc=str.toInt();//x column
						str = columns[2];
						int xdc=str.toInt();//dx/dt column
						str = columns[3];
						int yc=str.toInt();//x column
						str = columns[4];
						int ydc=str.toInt();//dy/dt column
						//
						str = columns[5];
						int ft=str.toInt();//time format (0 - YYYY MM DD.DDDDDD)
						str = columns[6];
						int fp=str.toInt();//positions format (0 - RA,DEC in HMS,DMS, 1 - relative positions in arcsec)
						//END columns
						
						for (j=0;j<sz;j++)
						{
							
						}
					}
				}
				break;*/
                                /*case 1://linear motion and velocity
				{
					eqData->clear();
					tmp = QString(folder+"/"+outflist[i]).toAscii();
					fname = tmp.data();
					eqData->init(fname);
					eqData->delMMrec();
					eqData->save();
//					break;
					QFile dataFile(folder+"/"+outflist[i]);
					dataFile.open(QIODevice::ReadWrite| QIODevice::Text);
					QTextStream dataStream;
					dataStream.setDevice(&dataFile);
					while (!dataStream.atEnd())
					{
						dataLine = dataStream.readLine();
						if((dataLine[0]!='#')&&(dataLine[0]!='%'))dataLines << dataLine;
					}
					if(dataLines.count()>2)
					{
						//BEGIN columns
						QStringList columns = dataformat.split(",");
						str = columns[0];
						int tc=str.toInt();//time column
						str = columns[1];
						int xc=str.toInt();//x column
						str = columns[2];
						int xdc=str.toInt();//dx/dt column
						str = columns[3];
						int yc=str.toInt();//x column
						str = columns[4];
						int ydc=str.toInt();//dy/dt column
						//
						str = columns[5];
						int ft=str.toInt();//time format (0 - YYYY MM DD.DDDDDD)
						str = columns[6];
						int fp=str.toInt();//positions format (0 - RA,DEC in HMS,DMS, 1 - relative positions in arcsec)
						//END columns
						//BEGIN apparent motion parameters

                                                sz = dataLines.count();
						vector<double>X(dataLines.count());
						vector<double>Y(dataLines.count());
						vector<double>Weights(dataLines.count());
						matrix<double>C(dataLines.count(),2);

                                                double *X = new double[sz];
                                                double *Y = new double[sz];
                                                double *Weights = new double[sz];
                                                double *C = new double[sz*2];
                                                double *Dx = new double[2*2];
                                                double *Dy = new double[2*2];
                                                double *Zx = new double[2];
                                                double *Zy = new double[2];
                                                double *uZx = new double[2];
                                                double *uZy = new double[2];
                                                int *excX = new int [sz];
                                                int *excY = new int [sz];
                                                double uweX, uweY;
                                                double rnX, rnY;


						double mt = 0,mxdot=0,mydot=0;
						QVector<double> times;
						for (j=0;j<dataLines.count();j++)
						{
							dataLine = dataLines[j];
							elS = dataLine.section('|',tc,tc);
							switch(ft)
							{
								case 0:{t = getMJDfromYMD(elS);break;}	
							}
							mt = mt + t;
							times << t;
							if(vflag==1)
							{
								elS = dataLine.section('|',xdc,xdc);
								mxdot = mxdot + elS.toDouble();
								elS = dataLine.section('|',ydc,ydc);
								mydot = mydot + elS.toDouble();
							}
						}
						mt = mt/dataLines.count();
						if(vflag==1)
						{
							mxdot = mxdot/dataLines.count();
							mydot = mydot/dataLines.count();
						}
                                                for (j=0;j<sz;j++)
						{
                                                        Weights[j]=1;
							dataLine = dataLines[j];
							switch(fp)
							{
                                                                case 0:{X[j] = hms_to_mas(dataLine.section('|',xc,xc)," ");Y[j]=damas_to_mas(dataLine.section('|',yc,yc)," ");break;}
								case 1:{elS = dataLine.section('|',xc,xc); X(j) = elS.toDouble();elS = dataLine.section('|',yc,yc); Y(j) = elS.toDouble();break;}	
							}
                                                        C[j*2] = 1;C[j*2+1] = times[j]-mt;
						}
//						matrix<double> Zx = LSM_corr(X, C, Weights, 500);
//						matrix<double> Zy = LSM_corr(Y, C, Weights, 500);

                                                iLSM(2, sz, 500, excX, Zx, C, X, uweX, Dx, 3, rnX, Weights);
                                                iLSM(2, sz, 500, excY, Zy, C, Y, uweY, Dy, 3, rnY, Weights);
                                                uZx[0] = sqrt(uweX*Dx[0]);
                                                uZx[1] = sqrt(uweX*Dx[1]);
                                                uZy[0] = sqrt(uweY*Dy[0]);
                                                uZy[1] = sqrt(uweY*Dy[1]);
                                  //              slsm(2, int h, double *const z, const double* c, const double* L, const double* W = 0);
						//END apparent motion parameters
						//BEGIN output
						QString timeLine;
						switch(ft)
							{
								case 0:{timeLine=getStrFromDATEOBS(getDATEOBSfromMJD(mt), " ", 0, 5);break;}	
							}
						switch(fp)
							{
								case 0://positions in RA and DEC
								{

                                                                                str= "#"+timeLine+"|"+mas_to_hms(Zx(1,0), " ", 4)+"|"+mas_to_damas(Zy(1,0), " ", 3)+"|"+
                                                                                QString( "%1" ).arg( (int)Zx(0,0),3,10,QLatin1Char( ' ' ))+"|"+
                                                                                QString( "%1" ).arg(Zx(0,1),6,'f',1,QLatin1Char( ' ' ))+"|"+
                                                                                QString( "%1" ).arg(Zy(0,1),6,'f',1,QLatin1Char( ' ' ))+"|"+
                                                                                QString( "%1" ).arg(Zx(1,1)*cos(mas_to_rad(Zy(1,0))),6,'f',1,QLatin1Char( ' ' )) + "|"+
                                                                                QString( "%1" ).arg(Zx(2,0)*cos(mas_to_rad(Zy(1,0)))/1440,10,'f',2,QLatin1Char( ' ' )) + "|"+
                                                                                QString( "%1" ).arg(Zx(2,1)*cos(mas_to_rad(Zy(1,0)))/1440,7,'f',2,QLatin1Char( ' ' ));
                                                                                if(vflag==1) str= str+ "|"+ QString( "%1" ).arg(Zx(2,0)*cos(mas_to_rad(Zy(1,0)))/1440 - mxdot,7,'f',2,QLatin1Char( ' ' ));
                                                                                str= str+ "|"+
                                                                                QString( "%1" ).arg(Zy(1,1),6,'f',1,QLatin1Char( ' ' )) + "|"+
                                                                                QString( "%1" ).arg(Zy(2,0)/1440,10,'f',2,QLatin1Char( ' ' )) + "|"+
                                                                                QString( "%1" ).arg(Zy(2,1)/1440,7,'f',2,QLatin1Char( ' ' ));
                                                                                if(vflag==1) str= str+ "|"+ QString( "%1" ).arg(Zy(2,0)/1440 - mydot,7,'f',2,QLatin1Char( ' ' ));
                                                                                str= str+ "\n";
									break;
								}
								case 1://relative positions in arcsec
								{

										str= "#"+timeLine+"|"+QString( "%1" ).arg(Zx(1,0),9,'f',3,QLatin1Char( ' ' ))+"|"+
										QString( "%1" ).arg(Zy(1,0),9,'f',3,QLatin1Char( ' ' ))+"|"+ 
										QString( "%1" ).arg( (int)Zx(0,0),3,10,QLatin1Char( ' ' ))+"|"+
										QString( "%1" ).arg(Zx(0,1)*1000,6,'f',1,QLatin1Char( ' ' ))+"|"+
										QString( "%1" ).arg(Zy(0,1)*1000,6,'f',1,QLatin1Char( ' ' ))+"|"+
										QString( "%1" ).arg(Zx(1,1)*1000,6,'f',1,QLatin1Char( ' ' )) + "|"+
										QString( "%1" ).arg(Zx(2,0)*1000/1440,10,'f',2,QLatin1Char( ' ' )) + "|"+
										QString( "%1" ).arg(Zx(2,1)*1000/1440,7,'f',2,QLatin1Char( ' ' ));
										if(vflag==1)str= str+ "|"+ QString( "%1" ).arg(Zx(2,0)*1000/1440 - mxdot,7,'f',2,QLatin1Char( ' ' ));
										QString( "%1" ).arg(Zy(1,1)*1000,6,'f',1,QLatin1Char( ' ' )) + "|"+
										QString( "%1" ).arg(Zy(2,0)*1000/1440,10,'f',2,QLatin1Char( ' ' )) + "|"+
										QString( "%1" ).arg(Zy(2,1)*1000/1440,7,'f',2,QLatin1Char( ' ' ));
										if(vflag==1)str= str+ "|"+ QString( "%1" ).arg(Zy(2,0)*1000/1440 - mydot,7,'f',2,QLatin1Char( ' ' ));
										str= str+ "\n";
									break;
								}
							}
						dataStream << str;
						//END output
					}
					dataFile.close();
                                        break;


                                }/
                                case 1:
                                {
                                    eqData->clear();
                                    tmp = QString(folder+"/"+outflist[i]).toAscii();
                                    fname = tmp.data();
                                    eqData->init(fname);
                                    eqData->delMMrec();

                                    QStringList columns = dataformat.split(",");
                                    str = columns[5];
                                    int ft=str.toInt();//time format (0 - YYYY MM DD.DDDDDD)
                                    str = columns[6];
                                    int fp=str.toInt();//positions format (0 - RA,DEC in HMS,DMS, 1 - relative positions in arcsec)

                                    eqData->countMM(fp, ft, vflag);
                                    eqData->save();
                                    break;
                                }
				
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				case 2://effect of phase
				{
					QFile dataFile(folder+"/"+outflist[i]);
					dataFile.open(QIODevice::ReadWrite| QIODevice::Text);
					QTextStream dataStream;
					dataStream.setDevice(&dataFile);
					int kphase = 1;
					int krade = 0;
					printf("timeList %d\n", timeList.count());

					while (!dataStream.atEnd())
					{
						dataLine = dataStream.readLine();
						if((dataLine[0]=='#')&&krade<2)
						{
							if(dataLine[1]!=' ')
							{
							printf("#");
							printf("%s", dataLine.data());
								switch(krade)
								{
								case 0:
									elS = dataLine.section('|',0,0);
									elS = elS.mid(1);
									ti = getMJDfromYMD(elS);
									timeList << QString("%1").arg(ti, 15, 'f', 10);
									//
									elS = dataLine.section('|',1,1);
									ra = hms_to_mas(elS, " ");
									RaList << QString("%1").arg(ra/1000.0/206265.0, 15, 'f', 10);
									elS = dataLine.section('|',4,4);
									ocRaList << elS;
									elS = dataLine.section('|',6,6);
									ocMuRaList << elS;
									break;
								case 1:
									elS = dataLine.section('|',1,1);
									de = damas_to_mas(elS, " ");
									DeList << QString("%1").arg(de/1000.0/206265.0, 15, 'f', 10);
									elS = dataLine.section('|',4,4);
									ocDeList << elS;
									elS = dataLine.section('|',6,6);
									ocMuDeList << elS;
	//								oc = sqrt(ocRa*ocRa + ocDe*ocDe);
	//								elS = QString("%1").arg(oc);
	//								ocList << elS;
									break;
								}
								
								krade++;
							}
						}
						if((dataLine[0]!='#')&&(dataLine[0]!='%')&&kphase)
						{
							kphase = 0;
//							dataLine = dataLines[j];
							phaseList << dataLine.section('|',11,11);
							printf("phaseList %d\n", phaseList.count());
//							phaseList = slS.toDouble();
						}
//						printf("#");
					}

					
					dataFile.close();
					break;
				}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			}
		}
		
		int i;
		
		if(typeoftask==2)
		{
			QFile resFile(folder+"/res.txt");
			resFile.open(QIODevice::ReadWrite| QIODevice::Text|QIODevice::Truncate);
			QTextStream resStream;
			resStream.setDevice(&resFile);
/*			printf("timeList.count %d\n", timeList.count());
			printf("ocRaList.count %d\n", ocRaList.count());
			printf("ocDeList.count %d\n", ocDeList.count());
//			printf("ocList.count %d\n", ocList.count());
                        printf("phaseList.count %d\n", phaseList.count());/
			for(i=0; i<timeList.count(); i++)
			{
				timeLine = timeList[i];
//				timeLine = timeLine.mid(1); 
				str= timeLine+"\t"+RaList[i]+"\t"+DeList[i]+"\t"+phaseList[i]+"\t"+ocRaList[i]+"\t"+ocDeList[i]+"\t"+ocMuRaList[i]+"\t"+ocMuDeList[i]+"\n";
				resStream << str;
			}
			
			resFile.close();
			
		}
		//END processing files
                */
		stream << "\n estimates finished...\n";
		return 0;  
	}
