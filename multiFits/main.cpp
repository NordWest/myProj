#include <QtCore>
#include "./../libs/fitsdata.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QProcess cutProcess, szProcess;
    QStringList outerArguments;

    QString archFile = "./e.lor.000.mks.fits.7z";
    QString fitsDir = archFile.left(archFile.lastIndexOf("."));
    qDebug() << QString("fitsDir: %1\n").arg(fitsDir);

    QString arch7z_prog="7z";
    QString arch7z_prog_folder = "./";
    QStringList arch7z_arguments=QString("x|-t7z").split("|");


    szProcess.setWorkingDirectory(arch7z_prog_folder);
    outerArguments.clear();
    outerArguments << arch7z_arguments << archFile;
    qDebug() << arch7z_prog << outerArguments.join(" ");
    szProcess.start(arch7z_prog, outerArguments);

    int resSZ = szProcess.waitForFinished(-1);

    QTextStream szStream(szProcess.readAllStandardOutput());
    qDebug() << szStream.readAll();

    return 0;//a.exec();
}
