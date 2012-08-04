#include <QApplication>
#include <QtGui>
#include <QObject>
#include "mainwindow.h"
#include <QTranslator>

int main(int argc, char *argv[])
    {  
        QApplication app(argc, argv);
		QTranslator translator;
		translator.load("qtccd_ru");
		if ((app.arguments().count()==2)&&(app.arguments().at(1)=="ru")) 
			app.installTranslator(&translator);

        MainWindow *mainWin = new MainWindow;
		app.connect(&app, SIGNAL(aboutToQuit()),mainWin,SLOT(close()));
        mainWin->show();
        return app.exec();
    }
