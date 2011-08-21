#include "rulerprocess.h"
#include "math.h"
#include "astro.h"
#include "ccdmeasurements.h"

rulerprocess::rulerprocess(QProgressBar *pb, QProgressBar *pbTot, QLabel *lb, QStringList *pl, QObject *parent)
{
	progressbar = pb;
	pbTotal = pbTot;
	label = lb;
	parsList = pl;
//	filesList = fl;
//	connect(this, SIGNAL(setPbInfo(int)), parent, SLOT(slotPbInfo(int)));

	isRemove = 0;
	
        QDir dir(parsList->at(3));
		
//	QStringList filters;
//	filters << "*.fit" << "*.fits" << "*.fts";
	dir.setFilter(QDir::Dirs|QDir::NoDotAndDotDot); 
	dirList = dir.entryList();
	int dnum = dirList.size();
	int i;
	qDebug() << "\ndnum " << dnum << "\n"; 
	for(i=0; i<dnum; i++)
	{
                workDirList << parsList->at(3) + "/" + dirList.at(i);
                resDirList << parsList->at(4) + "/" + dirList.at(i);
	}
	
	for(i=0; i<dnum; i++) qDebug() << workDirList[i] << "\n";
}; 

void rulerprocess::run()
{
	//BEGIN parameters
//	label->setText(tr("process is being performed"));
	QString ruler2name = QDir(parsList->at(0)).absolutePath();
	QString ruler2folder = QDir(parsList->at(1)).absolutePath();
        QString ruler2ini = QDir(parsList->at(2)).absolutePath();
        QString workingfolder = QDir(parsList->at(3)).absolutePath();
        QString ruler2outputfolder = QDir(parsList->at(4)).absolutePath();
        QString estimates_name = QDir(parsList->at(5)).absolutePath();
        QString estimates_name_folder = QDir(parsList->at(6)).absolutePath();
        QString str = parsList->at(7);
	int objType = str.toInt();

         QTime t;
         t.start();

         qDebug() << "\nworkingfolder= " << workingfolder << "\n";



	
        str = parsList->at(8);
	isRemove = str.toInt();
        str = parsList->at(9);
	int wff = str.toInt();
	//END parameters
	//BEGIN make reductions
	QProcess outerProcess;
	QStringList outerArguments;
	
	int i, j, fnum;
	int dnum = dirList.size();
	
	QDir wDir;	
        QDir resDirs(parsList->at(4));
//	QSettings rulerSett(ruler2folder+"/ruler2.ini", QSettings::IniFormat);
/*	qDebug() << "\nfileName:" << rulerSett.fileName() << "\n";
	qDebug() << "\nrulerSett: " << ruler2folder+"/ruler2.ini" << "\n";
	if(rulerSett.isWritable()) qDebug() << "\nSettings is Writable\n";
	else qDebug() << "\nSettings is Not Writable\n";
	qDebug() << "\noutpath:" << rulerSett.value("general/outfolder").toString() << "\n";
*/	
        QSettings *rulerSett = rulerSett = new QSettings(ruler2ini, QSettings::IniFormat);
	QStringList filters;
	filters << "*.fit" << "*.fits" << "*.fts";
	
//	slotSetPbInfo(11);

	QString resPath = resDirs.absolutePath();
	qDebug() << "\ndnum=" << dnum << "\n"; 
	QStringList filesList;
	for(i=0; i<dnum; i++)
	{
		pbTotal->setValue((int)(double)i*100/(double)dnum);
		resDirs.mkdir(dirList.at(i));
//		rulerSett = new QSettings(ruler2folder+"/ruler2.ini", QSettings::IniFormat);
		rulerSett->setValue("general/outfolder", resPath+"/"+dirList.at(i));
		qDebug() << "\nresPath: " << resPath+"/"+dirList.at(i) << "\n";
		if(rulerSett->status()==QSettings::NoError) qDebug() << "\nSettings No Error\n";
		else qDebug() << "\nSettings No Error\n";
		rulerSett->sync();
//		delete rulerSett;
		
//		qDebug() << "\noutpath:" << rulerSett-value("general/outfolder").toString() << "\n";
		
		wDir.setPath(workDirList.at(i));
		wDir.setNameFilters(filters);
		wDir.setSorting(QDir::Name|QDir::IgnoreCase);
		
		filesList.clear();
		filesList << wDir.entryList();
		
		fnum = filesList.size();
		qDebug() << "\nfnum= " << fnum << "\n";
		for(j=0; j<fnum; j++) qDebug() << "\n" << filesList[j] << "\n";
		
		progressbar->setValue(0);
		
		qDebug() << "\nstarting...\n";
		
		outerProcess.setWorkingDirectory(ruler2folder);
		outerProcess.setProcessChannelMode(QProcess::MergedChannels);
		outerProcess.setReadChannel(QProcess::StandardOutput);
		
		for(j=0;j<fnum;j++)
		{
			outerArguments.clear();
			label->setText(tr("info: ")+workingfolder+"/" + dirList.at(i) + "/" +filesList.at(j));
			qDebug() << "\n" << workingfolder+"/" + dirList.at(i) + "/" +filesList.at(j) << "\n";
			outerArguments << workingfolder+"/" + dirList.at(i) + "/" +filesList.at(j);
			qDebug() << "\nruler2:" << ruler2name << "|" <<  outerArguments.join("|") << "\n";
			outerProcess.start(ruler2name, outerArguments);
			outerProcess.waitForFinished(wff);
			progressbar->setValue((int)((double)j*100/(double)fnum));
			label->setText(tr("info: "));
			if(isRemove) QDir(workingfolder+"/" + dirList.at(i)).remove(filesList.at(j));
		}
		//END make reductions
		outerArguments.clear();
		//BEGIN statistics
		switch(objType)
			{
				case 0://asteroids 
				{
					outerProcess.setWorkingDirectory(estimates_name_folder);
					outerProcess.setProcessChannelMode(QProcess::MergedChannels);
					outerProcess.setReadChannel(QProcess::StandardOutput);
					outerArguments<<ruler2outputfolder+"/" + dirList.at(i) <<"0,1,2,3,4,5,6,7,8,9,10,11,12,13,14"<<"0"<<"10"<<"3"<<"*_eq.txt"<<"0";
					qDebug() << "\nestimates str: " << estimates_name << "|" << outerArguments.join("|");
					outerProcess.start(estimates_name, outerArguments);
					outerProcess.waitForFinished(wff);
					outerArguments.clear();
					outerArguments<<ruler2outputfolder+"/" + dirList.at(i) <<"0,1,10,2,11,0,0"<<"1"<<"10"<<"3"<<"*_eq.txt"<<"1";
//					outerArguments<<ruler2outputfolder+"/" + dirList.at(i) <<"0,1,8,2,9,0,0"<<"1"<<"10"<<"3"<<"*_eq.txt"<<"1";
					outerProcess.start(estimates_name, outerArguments);
					outerProcess.waitForFinished(wff);
					break;
				}
				case 1://saturnian satellites 
				{
					outerProcess.setWorkingDirectory(estimates_name_folder);
					outerProcess.setProcessChannelMode(QProcess::MergedChannels);
					outerProcess.setReadChannel(QProcess::StandardOutput);
					outerArguments<<ruler2outputfolder <<"4,5"<<"0"<<"10"<<"3"<<"*_eq.txt"<<"0";
					outerProcess.start(estimates_name, outerArguments);
					outerProcess.waitForFinished(wff);
					outerArguments.clear();
					outerArguments<<ruler2outputfolder <<"0,1,8,2,9,0,0"<<"1"<<"10"<<"3"<<"*_eq.txt"<<"0";
					outerProcess.start(estimates_name, outerArguments);
					outerProcess.waitForFinished(wff);
					outerArguments.clear();
					outerArguments<<ruler2outputfolder <<"4,5"<<"0"<<"10"<<"3"<<"*_rel.txt"<<"0";
					outerProcess.start(estimates_name, outerArguments);
					outerProcess.waitForFinished(wff);
					outerArguments.clear();
					outerArguments<<ruler2outputfolder <<"0,1,1,2,2,0,1"<<"1"<<"10"<<"3"<<"*_rel.txt"<<"0";
					outerProcess.start(estimates_name, outerArguments);
					outerProcess.waitForFinished(wff);
					break;
				}
			}
		//END statistics
		if(isRemove) QDir(workingfolder).rmdir(dirList.at(i));
		
		progressbar->setValue(100);
//		label->setText(tr("process has been finished"));
//		progressbar->setValue(100);
//		label->setText(tr("process has been finished"));
		//END calculate mean values and standard errors
	}
	pbTotal->setValue(100);
	label->setText(tr("process has been finished"));

        qDebug("Time elapsed: %d ms", t.elapsed());
};
/*
void rulerprocess::slotSetPbInfo(int val)
{
	emit signalSetPbInfo(val);
}

void rulerprocess::signalSetPbInfo(int val){};
*/
