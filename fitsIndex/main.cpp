#include <QtCore>

#include "./../libs/fitsdata.h"
#include "./../libs/redStat.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int i;

    QString workDir("./test");

    QDir tDir;
    QString tFile;
    QStringList filList, dataFiles;
    filList << "*.fit" << "*.fits";

    QDirIterator it(workDir, QDirIterator::Subdirectories);

     while (it.hasNext()) {
         tFile = it.next();
         if((tFile.lastIndexOf("/..")==tFile.lastIndexOf("/"))||(tFile.lastIndexOf("/.")==tFile.lastIndexOf("/"))||(tFile.right(1)==QString("."))) continue;

         qDebug() << QString("tFile: %1\n").arg(tFile);
         //tDir.setPath(tFile);
         //qDtDir

         for(i=0;i<filList.size();i++)
         {
            if((tFile.lastIndexOf(filList.at(i))==tFile.size()-filList.at(i).size())&&(tFile.size()>filList.at(i).size()))
            {
                dataFiles << tFile;
            }
         }


     }

     qDebug() << QString("dataFiles:\n%1\n").arg(dataFiles.join("\n"));



    return 0;//a.exec();
}
