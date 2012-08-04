#include <QCoreApplication>
#include <QtCore>
#include "cdsfind.h"



int main(int argc, char *argv[])
    {  
        QCoreApplication app(argc, argv);
		if (argc==1) 
		{
			QTextStream stream(stdout);
			stream << "cdsfind: command line needed." << "\n";
			return 0;
		}
		else
		{
			cdsfind *cdsFind = new cdsfind(&app);
			return app.exec();
		}
	   
	}
