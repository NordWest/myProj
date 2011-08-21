#include <QtCore>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QStringList fileNames;
    int i, sz0;
    sz0 = argc - 1;
    for(i=0; i<sz0; i++)
    {
        fileNames << QString(argv[i+1]);
    }

    QFile f0(argv[1]);
    f0.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream f0in(&f0);
    QStringList opers0, opers1;
    int x0, y0, x1, y1;
    int dist;
    int distMax = 30;
    //int *dist = new int[sz0];
    QString dataStr;
    QList <QStringList> files;
    QStringList flist0, flist1;

    while(!f0in.atEnd())
    {
        dataStr = f0in.readLine();
        opers0 = dataStr.split(" ", QString::SkipEmptyParts);
        x0 = opers0.at(1).toInt();
        y0 = opers0.at(2).toInt();


    }

    return 0;//a.exec();
}
