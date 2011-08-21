#include "basewidget.h"
#include "rulerprocess.h"

baseWidget::baseWidget()
{
	setWindowTitle (tr("ruler manager"));
	//BEGIN read settings
	settings = new QSettings("rulermanager.ini",QSettings::IniFormat);
	parametersList << settings->value("general/ruler2name").toString();
	parametersList << settings->value("general/ruler2folder").toString();
        parametersList << settings->value("general/ruler2ini").toString();
	parametersList << settings->value("general/workingfolder").toString();
	parametersList << settings->value("general/ruler2outputfolder").toString();
	parametersList << settings->value("general/estimates_name").toString();
	parametersList << settings->value("general/estimates_name_folder").toString();
	parametersList << settings->value("general/objType").toString();
	parametersList << settings->value("general/isRemove").toString();
	parametersList << settings->value("general/waitForFinished").toString();
	
	int isAutolunch = settings->value("general/isAutolunch").toInt();
	//END read settings
	//BEGIN read file list

        qDebug() << "debug\n";

        qDebug() << parametersList.join("\n") << "\n";
/*	
	QDir dir(parametersList.at(2));
//	QStringList filters;
//	filters << "*.fit" << "*.fits" << "*.fts";
	dir.setFilter(QDir::Dirs|QDir::NoDotAndDotDot); 
	dirList = dir.entryList();
	int dnum = dirList.size();
	int i;
	qDebug() << "\ndnum " << dnum << "\n"; 
	for(i=0; i<dnum; i++)
	{
		workDirList << parametersList.at(2) + "/" + dirList.at(i);
		resDirList << parametersList.at(3) + "/" + dirList.at(i);
	}
	
	for(i=0; i<dnum; i++) qDebug() << workDirList[i] << "\n";
	*/
	//END read file list
	//BEGIN create widgets
	QGridLayout *baseGrid = new QGridLayout(this);
	pbInfo = new QProgressBar;
	pbInfo->setRange(0,100);
	totInfo = new QProgressBar;
	totInfo->setRange(0,100);
	lbInfo = new QLabel(tr("info:"));
	startButton = new QPushButton(tr("start"));
	connect(startButton, SIGNAL(clicked()), this, SLOT(slotStartButtonClicked()));
	baseGrid->addWidget(lbInfo,0,0);
	baseGrid->addWidget(pbInfo,1,0);
	baseGrid->addWidget(totInfo,2,0);
	baseGrid->addWidget(startButton,3,0);
	
	if(isAutolunch) slotStartButtonClicked();
	//BEGIN create widgets 
};

void baseWidget::slotStartButtonClicked()
{
	rulerprocess *rp;

	rp = new rulerprocess(pbInfo, totInfo, lbInfo, &parametersList, this);

//	connect(rp, SIGNAL(setPbInfo(int)), this, SLOT(slotPbInfo(int)));
	
	rp->start();
	//QMessageBox::information(0,"debug","start",QMessageBox::Ok,0,0);
};
/*
void baseWidget::slotPbInfo(int val)
{
	pbInfo->setValue(val);
}
*/
