#include <QCoreApplication>
#include <QtCore>
#include "sateph.h"



int main(int argc, char *argv[])
    {  
        QCoreApplication app(argc, argv);
		if (argc==1) 
		{
			QTextStream stream(stdout);
			stream << "sateph: command line needed." << "\n";
			return 0;
		}
		else
		{
			sateph *se = new sateph(&app);
			return app.exec();
		}
	   
	}

