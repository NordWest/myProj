//эта незатейливая прога может брать эфемериды малых планет с сайта http://www.imcce.fr/en/ephemerides/formulaire/form_ephepos.php
//это консольное приложение с циклом обработки сообщений или событий (event loop)
#include <QCoreApplication>
#include <QtCore>
#include "miriadeeph.h"



int main(int argc, char *argv[])
    {  
        QCoreApplication app(argc, argv);//создаем объект приложения
		if (argc==1) // если нет аргументов в командной строке, то выходим с выводом сообщения в консоль
		{
			QTextStream stream(stdout);
                        stream << "miriadeEph: command line needed." << "\n";
			return 1;
		}
		else// какие-никакие, а аргументы есть.
		{
                        miriadeEph *mpeProc = new miriadeEph(&app);// создаем объект каласса miriadeEph и передаем указатель на объект приложение, это поможет легко добраться до аргументов командной строки и посылать сообщения.
			return app.exec();// все, готово дело, запускаем цикл обработки сообщений.
		}
	   
	}
