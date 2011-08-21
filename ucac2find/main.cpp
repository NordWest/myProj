#include <QCoreApplication>
#include "./../libs/astro.h"
#include "./../libs/comfunc.h"



int main(int argc, char *argv[])
    {  
        QCoreApplication app(argc, argv);
		QTextStream stream(stdout);
		//
		QSettings *settings = new QSettings(QCoreApplication::applicationDirPath()+"/ucac2find.ini",QSettings::IniFormat);
		QString ucac2path = settings->value("general/ucac2path").toString();
		QString outPath = settings->value("general/outPath").toString();
		QString colSeparator = settings->value("general/colSeparator").toString();
		int msgOut = settings->value("general/msgOut").toInt();
		int pform = settings->value("general/pform").toInt();
		//
		if(msgOut==1) stream << "ucac2find has started...\n";
		//
		QString sRA = app.arguments().at(2);
		QString sDE = app.arguments().at(3);
		QString sRHO = app.arguments().at(4);
		QString sMAG1 = app.arguments().at(5);
		QString sMAG2 = app.arguments().at(6);
		//stream << sMAG1 << "\n";
		QString outFname;
		QString str,pstr;
		double ra_c, de_c, rhoX,rhoY, mag1, mag2;
		int fshape;
		if (app.arguments().at(1)=="-b")
		{
			str = sRHO.section('x',0,0);
			rhoX = 60000*str.toDouble();
			str = sRHO.section('x',1,1);
			rhoY = 60000*str.toDouble();
			fshape=1;
		}	
		if (app.arguments().at(1)=="-r")
		{
			rhoX = 60000*sRHO.toDouble();
			rhoY = rhoX;
			fshape=0;
		}
		mag1 = sMAG1.toDouble();
		mag2 = sMAG2.toDouble();
		if (app.arguments().at(1)=="-l")
		{
		    if (sRA=="-r") 
			{
				rhoX = 60000*sRHO.toDouble();
				rhoY = rhoX;
				fshape = 0;
			}
			if (sRA=="-b")
			{
				str = sRHO.section('x',0,0);
				rhoX = 60000*str.toDouble();
				str = sRHO.section('x',1,1);
				rhoY = 60000*str.toDouble();
				fshape = 1;
			}
			QString lfname = sDE;
			QFile lFile(outPath+"/"+lfname);
			stream << outPath+"/"+lfname << "\n";
			lFile.open(QIODevice::ReadOnly | QIODevice::Text);
			QTextStream lStream;
			lStream.setDevice(&lFile);
			while (!lStream.atEnd())
			{
				str = lStream.readLine();
				pstr = str.section('|',0,0);
				pstr.trimmed();
				outFname = "ucac2_" +pstr+".txt";
				pstr = str.section('|',1,1);
				pstr.trimmed();
				ra_c = hms_to_mas(pstr, " ");
				pstr = str.section('|',2,2);
				pstr.trimmed();
				de_c = damas_to_mas(pstr, " ");
				int ucac2sel = UCAC2dataToFile(ra_c, de_c, rhoX, rhoY, mag1, mag2, fshape, ucac2path, outPath, outFname, colSeparator, pform);				
				if (ucac2sel!=0)
				{
				    switch(ucac2sel)
					{
						case 1: stream << "ucac2 data file has not been opened." << "\n"; break;
						case 2: stream << "u2index.da has not been opened." << "\n"; break;
						case 3: stream << "out of ucac2 zone." << "\n"; break;
					}
				}
			} 
			lFile.close();
		}
		else 
		{
			outFname = app.arguments().at(7);
			ra_c = hms_to_mas(sRA, ":");
			de_c = damas_to_mas(sDE, ":");
			int ucac2sel = UCAC2dataToFile(ra_c, de_c, rhoX, rhoY, mag1, mag2, fshape, ucac2path, outPath, outFname, colSeparator, pform);
			if (ucac2sel!=0)
			{
			    switch(ucac2sel)
				{
					case 1: stream << "ucac2 data file has not been opened." << "\n"; break;
					case 2: stream << "u2index.da has not been opened." << "\n"; break;
					case 3: stream << "out of ucac2 zone." << "\n"; break;
				}
			}
		}
		//
		if(msgOut==1)stream << "ucac2find has finished.\n";
		return 0;  
	}
	
