#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // изменим каталог плагинов (добавим каталог установки программы)
    QStringList list_path;	// Список путей

    QDir dir = QDir(app.applicationDirPath()+"/QtPlugins/");	//Наш каталог %PATH_TO_APP%/QtPlugins/
    list_path << dir.absolutePath () << app.libraryPaths ();	//добавим в список и наши и стандартные пути
    app.setLibraryPaths( list_path  );				//Установим новый список путей поиска для приложения
    //

    MainWindow w;
    w.show();
    return app.exec();
}
