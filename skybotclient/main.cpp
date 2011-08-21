#include <QCoreApplication>
#include <QtCore>
#include "skybotclient.h"



int main(int argc, char *argv[])
    {  
        QCoreApplication app(argc, argv);
		if (argc==1) 
		{
			QTextStream stream(stdout);
			stream << "skybotclient: command line needed." << "\n";
			return 0;
		}
		else
		{
			skybotclient *sc = new skybotclient(&app);
			return app.exec();
		}
	   
	}
