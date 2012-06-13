#include <QtCore>
#include "./../libs/fitsdata.h"

int main(int argc, char *argv[])
{
    fitsdata fitsd;

    QCoreApplication a(argc, argv);

    int i, szi, j, szj;
    QProcess cutProcess, szProcess;
    QStringList outerArguments;
    QStringList entryList;

    QString cfgFileName("./multiFits.ini");

    QSettings *sett = new QSettings(cfgFileName, QSettings::IniFormat);

    measureParam mesPar;
    mesPar.model = sett->value("psf/model", 0).toInt();//settings->value("psf/model").toInt();//PSF model: 0 - Lorentz PSF, 1 - Gauss PSF, 2 - Moffat PSF, 3 - CoM
    mesPar.nofit = sett->value("psf/nofit", 10).toInt();
    mesPar.delta = sett->value("psf/delta", 1.2).toDouble();
    mesPar.ringradius = sett->value("psf/ringradius", 10).toInt();
    mesPar.ringwidth = sett->value("psf/ringwidth", 6).toInt();
    mesPar.lb = sett->value("psf/lb", 1).toInt();
    mesPar.sg = sett->value("psf/subgrad", 1).toInt();
    mesPar.apRadius = sett->value("psf/apRadius", 10).toInt();

    QString archFile = "./e.lor.000.mks.fits.7z";
    QString fitsDir = archFile.left(archFile.lastIndexOf("."));
    qDebug() << QString("fitsDir: %1\n").arg(fitsDir);

    QString resFname = archFile.left(fitsDir.lastIndexOf("."));
    qDebug() << QString("resFname: %1\n").arg(resFname);

    QDir wDir("./");

    QDir tDir;

    QStringList filterL;
    filterL << "*.*";

    QStringList fitsFL;
    fitsFL << "*.fits";

    tDir.setPath(fitsDir);
    entryList = tDir.entryList(filterL);

    /*
    szi = entryList.size();
    for(i=0; i<szi; i++) tDir.remove(entryList[i]);

    int rmRes = wDir.rmdir(fitsDir);
    qDebug() << QString("\nrmdir: %1\n").arg(rmRes);

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
    //qDebug() << szStream.readAll();
*/


    entryList.clear();
    entryList = tDir.entryList(fitsFL);




    szi = entryList.size();

    marksGrid resGrid;
    //fitsd.ipixMarks = new marksGrid();
    marksP *tMark = new marksP;

    for(i=0; i<szi; i++)
    {
        fitsd.clear();
        fitsd.openFile(tDir.path()+QDir().separator()+entryList[i], 2);

        qDebug() << QString("addImg mark\n");
        //fitsd.ipixMarks->addImgMark(fitsd.workFrame.center().x(), fitsd.workFrame.center().y());
        qDebug() << QString("cRect: %1\t%2\n").arg(fitsd.WCSdata[0]).arg(fitsd.WCSdata[1]);
        //fitsd.ipixMarks->addImgMark(fitsd.WCSdata[0], fitsd.WCSdata[1]);
        tMark->mTanImg[0] = fitsd.WCSdata[0];
        tMark->mTanImg[1] = fitsd.WCSdata[1];

        qDebug() << QString("measureMarksGrid\n");
//        fitsd.measureMarksGrid(fitsd.ipixMarks, mesPar);
        if(measureMark(fitsd.imgArr, tMark, mesPar))
        {
            resGrid.marks << tMark;
            tMark = new marksP;
        }

    }

    qDebug() << QString("resMarks num: %1\n").arg(resGrid.marks.size());

    resGrid.saveTanImg(resFname, " ", "2,3,1,4,5");


    return 0;//a.exec();
}
