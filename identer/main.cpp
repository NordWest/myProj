#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDataStream>

#include "./../libs/fitsdata.h"
#include "./../libs/astro.h"
#include "./../libs/identify.h"
//
static QDataStream* clog = 0;
void customMessageHandler(QtMsgType type, const char* msg)
{
    static const char* msgType[] =
    {
        "Debug    : ",
        "Warning  : ",
        "Critical : ",
        "Fatal    : "
    };

    static QTextStream cout(stdout);
    static QTextStream cerr(stderr);

    cerr << msgType[type] << msg << endl;
    if(clog && clog->device())
        *clog << type << msg;
    if(type == QtFatalMsg)
    {
        cerr << "aborting..." << endl;

#if defined(Q_CC_MSVC) && defined(QT_DEBUG) && defined(_CRT_ERROR) && defined(_DEBUG)
        int reportMode = _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_WNDW);
        _CrtSetReportMode(_CRT_ERROR, reportMode);
        int ret = _CrtDbgReport(_CRT_ERROR, __FILE__, __LINE__, QT_VERSION_STR, msg);
        if(ret == 0 && reportMode & _CRTDBG_MODE_WNDW)
            return;
        else if(ret == 1)
            _CrtDbgBreak();
#endif

#if defined(Q_OS_UNIX) && defined(QT_DEBUG)
        abort();
#else
        exit(1);
#endif
    }
}

marksGrid *mgr;
marksGrid *mgrE;

struct headStr
{
	QString tel, plateN, obj, exp, obsr, ra, de, dateObs, dateEnd;
};

void loadHeader(QString fileName, headStr *hs);
void detExp(double *exp, QString expS);

int main(int argc, char** argv)
{
    qInstallMsgHandler(customMessageHandler); 

//    QApplication app(argc, argv);
	QCoreApplication app(argc, argv);

    QFile* logFile = new QFile("messages.log");
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
        clog = new QDataStream(logFile);
        
	fitsdata *fd = new fitsdata();
	
	double ra_oc, de_oc;

	mgr = new marksGrid;
	mgrE = new marksGrid;
	
	QSettings *settings = new QSettings("conf.ini",QSettings::IniFormat);
	QString mSep = settings->value("marks/marksFileSep").toString();
	QString mCol = settings->value("marks/marksLoadFileColumn").toString();
	
	fd->marksGIpix->loadTanImg(argv[1], mSep, mCol);
//	fd->marksGIpix->flipH();
//	fd->marksGIpix->flipV();
	
	headStr hs;
	loadHeader(argv[2], &hs);
	double texp;
	double scale = 469.8936;
	fd->marksG->setInstrProp(scale, scale, 0.0);
	fd->marksGIpix->setInstrProp(scale, scale, 0.0);
	
	ra_oc = hms_to_mas(hs.ra, " ");
//	ra_oc = hms_to_mas("5 18 43.50", " ");
	de_oc = damas_to_mas(hs.de, " ");
//	de_oc = damas_to_mas("16 27 23.7", " ");
	fd->WCSdata[12] = 0;
	fd->WCSdata[2] = mas_to_grad(ra_oc);
	fd->WCSdata[3] = mas_to_grad(de_oc);
//	qDebug() << "\nra: " << mas_to_grad(fd->WCSdata[2])/15.0 << "\nde: " << mas_to_grad(fd->WCSdata[3]) << "\n";
	
	detExp(&texp, hs.exp);
	qDebug() << "\ntExp\t" << texp << "\n";
	fd->MJD = getMJDfromStrFTN(hs.dateObs, texp);
	
	fd->getMarksGrid(0, 120, 6.0, 13);
	fd->detTan();
	
//////////////////////////////////////////////
	marksGrid *resGrid = new marksGrid;
	
	int nSize, kSize;
	int i, j;
	double OCp0, OCp1;
	
//	fitsdata *fitsd = getCurFits();
	marksGrid *mGr = fd->marksG;
	marksGrid *mGrI = fd->marksGIpix;
	
	mGr->sortMagn();
	mGrI->sortAper(1);
	
	nSize = mGrI->marks.size();
	kSize = mGr->marks.size();
	
	if(nSize>20) nSize = 20;
	if(kSize>100) kSize = 100;
	
	OCp0 = grad_to_mas(fd->WCSdata[2]);
	OCp1 = grad_to_mas(fd->WCSdata[3]);

	double *x = new double[nSize];
	double *y = new double[nSize];
	
	double *ksiI = new double[kSize];
	double *etaI = new double[kSize];
	double *ksi = new double[kSize];
	double *eta = new double[kSize];
	double *magn = new double[kSize];
	double *ra = new double[kSize];
	double *de = new double[kSize];
	
	qDebug() << "\nx y\n";
	for(i = 0; i< nSize; i++)
	{
		x[i] = mGrI->marks[i]->mTanImg[0];
		y[i] = mGrI->marks[i]->mTanImg[1];
		qDebug() << "\n" << x[i] << "\t" << y[i] << "\t" << mGrI->marks[i]->mTanImg[2];
	}
	qDebug() << "\nksi eta\n";
	for(i = 0; i< kSize; i++)
	{
		ksi[i] = mGr->marks[i]->mTan[0];
		eta[i] = mGr->marks[i]->mTan[1];
		ksiI[i] = mGr->marks[i]->mTanImg[0];
		etaI[i] = mGr->marks[i]->mTanImg[1];
		ra[i] = mGr->marks[i]->mEkv[0];
		de[i] = mGr->marks[i]->mEkv[1];
		magn[i] = mGr->marks[i]->mEkv[2];
		qDebug() << "\n" << ksi[i] << "\t" << eta[i] << "\t" << magn[i];
	}
	double xscale = mGr->scales[0]/1000.0;
	double yscale = mGr->scales[1]/1000.0;
	
	qDebug() << "\nxscale " << xscale << "\tyscale " << yscale;
	
	bool fvm=false;
	int *sootv = new int[nSize];
	
	otogdestv(xscale, yscale, kSize, nSize, fvm, ksi, eta, x, y, sootv);
	
	int oSize = 0;
	
	double *xW = new double[nSize];
	double *yW = new double[nSize];
	
	double *ksiW = new double[nSize];
	double *etaW = new double[nSize];
	double *raW = new double[nSize];
	double *deW = new double[nSize];
	double *magnW = new double[nSize];
	
	int k = 0;	
	
	for(i = 0; i< nSize; i++)
	{
		qDebug() << "\n" << i << ":" << sootv[i] << "\n";
		if(sootv[i]!=-1)
		{
			xW[oSize] = x[i];
			yW[oSize] = y[i];
			ksiW[oSize] = ksi[sootv[i]];
			etaW[oSize] = eta[sootv[i]];
			raW[oSize] = ra[sootv[i]];
			deW[oSize] = de[sootv[i]];
			magnW[oSize] = magn[sootv[i]];
			qDebug() << "\n" << ksiW[oSize] << "\t" << etaW[oSize] << "\t" << magnW[oSize];
			oSize++;
		}
	}
	qDebug() << "\noSize\t" << oSize << "\n";
	
//	marksGrid *maG = new marksGrid;
	qDebug() << "\nra\tde\tmagn\tksi\teta\tx\ty\t\n";
	for(i = 0; i< oSize; i++)
	{
		resGrid->addEkvMark(raW[i], deW[i], magnW[i]);
		resGrid->marks[i]->mTan[0] = ksiW[i];
		resGrid->marks[i]->mTan[1] = etaW[i];
		resGrid->marks[i]->mTanImg[0] = xW[i];
		resGrid->marks[i]->mTanImg[1] = yW[i];
		
		qDebug() << "\n" << QString("%1\t%2\t").arg(resGrid->marks[i]->mEkv[0], 12, 'f', 2).arg(resGrid->marks[i]->mEkv[1], 12, 'f', 2) << resGrid->marks[i]->mEkv[2] << resGrid->marks[i]->mTan[0] << resGrid->marks[i]->mTan[1] << resGrid->marks[i]->mTanImg[0] << resGrid->marks[i]->mTanImg[1] << "\n";
	}
	
	fd->detWCS(oSize, resGrid);




//////////////////////////////////////////////
	
	
	



//    int exit_code = app.exec();

    delete clog;
    clog = 0;
    delete logFile;
    logFile = 0;

    qInstallMsgHandler(0);

    return 0;
}

void detExp(double *exp, QString expS)
{
	int num = expS.size();
	double t = 1;
	int it;
	bool isok;
	for(int i=0; i<num; i++)
	{
		it = expS.toInt(&isok);
		if(isok) t *= it;
	}
	*exp = t*60;
}

void loadHeader(QString fileName, headStr *hs)
{
	
    if (!fileName.isEmpty())
	{
		char *str = new char[1024];
		char *str_name = new char[1024];
		char *str_value = new char[1024];
	
		QByteArray fheadfn, qbarr;
		fheadfn.insert(0, fileName);
		FILE *FI = fopen(fheadfn.data(), "r");
		if(FI==NULL)
		{
			QMessageBox msgBox(QMessageBox::Warning, "Can't open Header file", "Mentioned FITS Header file has't exist", 0, 0);
			msgBox.addButton(QMessageBox::Ok);
			msgBox.exec();
			return ;
		}
		
		qDebug() << "\nREADING HEADER..." << endl;
	
		int posb, pose, tobs = 0;
		QString *qstr = new QString;
		
		
		
		QTextCodec *codec1 = QTextCodec::codecForName("windows-1251");
		QTextCodec *codec2 = QTextCodec::codecForName("IBM 866");

	    QString string;
		QChar *cstr = new QChar;
		int endKey = 0;
		QString dateObs, timeObs, timeEnd;
		QStringList obsList;
			
		while(fgets(str, 1024, FI))
		{
			qDebug() << "string is: _" << str << "_" << endl;
			qbarr.clear();
			qbarr.insert(0, str);

//		out_stream << "qbarr\t" << qbarr.data() << endl;
			string = codec1->toUnicode(qbarr);
//		out_stream << "string\t" << string << endl;
			qbarr = codec2->fromUnicode(string);
			str = qbarr.data();
		
//		printf("_%s_\n", str);
//		out_stream << "qbarr\t" << qbarr << endl;
//		out_stream << "str\t" << str << endl;
		
			if(!slovo_stopsim(str, str_name, " ", 0, &pose, 8)&&!slovo_stopsim(str, str_value, "\'", 11, &pose, 68))
			{
				string = codec2->toUnicode(str_value);
				if(streqv(str_name, "TELESCOP"))
				{
					hs->tel.insert(0, string);
					qDebug() << "_" << str_name << "_\t_" << str_value << "_" << endl;
				}
				else if(streqv(str_name, "PLATE-N"))
				{
					hs->plateN.insert(0, string);
					qDebug() << "_" << str_name << "_\t_" << str_value << "_" << endl;
				}
				else if(streqv(str_name, "OBJECT"))
				{
					hs->obj.insert(0, string);
					qDebug() << "_" << str_name << "_\t_" << str_value << "_" << endl;
				}
				else if(streqv(str_name, "EXPOSURE"))
				{
					hs->exp.insert(0, string);
					qDebug() << "_" << str_name << "_\t_" << str_value << "_" << endl;
				}
				else if(streqv(str_name, "OBSERVER"))
				{
					hs->obsr.insert(0, string);
					qDebug() << "_" << str_name << "_\t_" << str_value << "_" << endl;
				}
				else if(streqv(str_name, "RA"))
				{
					hs->ra.insert(0, string);
					qDebug() << "_" << str_name << "_\t_" << str_value << "_" << endl;
				}
				else if(streqv(str_name, "DEC"))
				{
					hs->de.insert(0, string);
					qDebug() << "_" << str_name << "_\t_" << str_value << "_" << endl;
				}
				else if(streqv(str_name, "DATE-OBS"))
				{
					obsList = string.split(" ");
					dateObs.insert(0, obsList.join("-"));
//					dateObsLE->setText(obsList.join("-"));
					qDebug() << "_" << str_name << "_\t_" << str_value << "_" << endl;
				}
				else if(streqv(str_name, "TIME-OBS"))
				{
					obsList = string.split(" ");
					switch(endKey)
					{
						case 0:
						timeObs.insert(0, obsList.join(":"));
//						dateObsLE->setText(obsList.join(":"));
						endKey = 1;
						break;
						case 1:
						timeEnd.insert(0, obsList.join(":"));
//						dateEndLE->setText(obsList.join(":"));
						break;
					}
					qDebug() << "_" << str_name << "_\t_" << str_value << "_" << endl;
				}
				else
				{

					qDebug() << str_name << " no var" << endl;
				}
			}
			hs->dateObs.insert(0, dateObs+"T"+timeObs);
			hs->dateEnd.insert(0, dateObs+"T"+timeEnd);
		}
	}
}