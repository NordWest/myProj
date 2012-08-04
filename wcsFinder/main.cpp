#include <QtCore>

int main(int argc, char *argv[])//wcsFinder.exe wcsFinder.ini [workdir]
{
    int i, resWcs;
    QFileInfo fi;

    setlocale(LC_NUMERIC, "C");


    QString cfgFile = QString(argv[1]);
    QSettings *sett = new QSettings(cfgFile, QSettings::IniFormat);
    QString workDir;

    if(argc==2) workDir = sett->value("general/workDir", "./").toString();
    else workDir = QString(argv[2]);
    qDebug() << QString("workDir: %1\n").arg(workDir);

    QString pwcsCfg = sett->value("general/pwcsCfg", "./conf/plateWCS.ini").toString();

    QString wcs_prog = sett->value("general/wcs_prog", "./plateWCS.exe").toString();
    QString wcs_prog_folder = sett->value("general/wcs_prog_folder", "./").toString();
    int wcs_wait = sett->value("general/wcs_wait", -1).toInt();
    if(wcs_wait>0) wcs_wait *=1000;

    QString filters = sett->value("general/filtersList", ".v21|.v22|.v23|.v32|.v33").toString();
    QStringList filList = filters.split("|");

    //int plNameType = sett->value("general/plNameType", 0).toInt();

    QProcess wcsProcess;
    QStringList outerArguments;
    outerArguments.clear();

    QString tFile, pnStr, curDir;
    QStringList dataFiles;

    qDebug() << QString("workDir: %1\n").arg(workDir);
    qDebug() << QString("filters: %1\n").arg(filList.join("|"));

    QDirIterator it(workDir, QDirIterator::Subdirectories);

     while (it.hasNext()) {
         tFile = it.next();
         if((tFile.lastIndexOf("/..")==tFile.lastIndexOf("/"))||(tFile.lastIndexOf("/.")==tFile.lastIndexOf("/"))||(tFile.right(1)==QString("."))) continue;

         for(i=0;i<filList.size();i++)
         {
            if((tFile.lastIndexOf(filList.at(i))==tFile.size()-filList.at(i).size())&&(tFile.size()>filList.at(i).size()))
            {
                dataFiles << tFile;
            }
         }


     }

     qDebug() << QString("dataFiles:\n%1").arg(dataFiles.join("\n"));

     int dfNum = dataFiles.size();

     for(i=0; i<dfNum; i++)
     {
         tFile = dataFiles.at(i);
         qDebug() << QString("tFile: %1\n").arg(tFile);

         /*getPlateName(tFile, &pnStr, plNameType);

         qDebug() << QString("pnStr: %1\n").arg(pnStr);*/

         fi.setFile(tFile);
         curDir = fi.absoluteFilePath();

         curDir = tFile.left(tFile.lastIndexOf("/"));
         qDebug() << QString("curDir: %1\n").arg(curDir);

         wcsProcess.setWorkingDirectory(wcs_prog_folder);
         outerArguments.clear();
         outerArguments << tFile << "config="+pwcsCfg ;// << "0" << pnStr;
         qDebug() << wcs_prog << outerArguments.join(" ");
         wcsProcess.start(wcs_prog, outerArguments);

         resWcs = wcsProcess.waitForFinished(wcs_wait);

         if(!resWcs)
         {
             wcsProcess.close();
         }

     }


    return 0;
}
