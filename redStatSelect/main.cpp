#include <QCoreApplication>
#include <QtCore>

#include "./../libs/redStat.h"
#include "./../libs/fitsdata.h"

static QDataStream* clog = 0;
void customMessageHandler(QtMsgType type, const char* msg)
{
    static const char* msgType[] =
    {
        "Debug    : ",
        "Warning  : ",
        "Critical : ",
        "Fatal    : "
    };

    static QTextStream cout(stdout);
    static QTextStream cerr(stderr);

    cerr << msgType[type] << msg << endl;
    if(clog && clog->device())
        *clog << type << msg;
    if(type == QtFatalMsg)
    {
        cerr << "aborting..." << endl;

#if defined(Q_CC_MSVC) && defined(QT_DEBUG) && defined(_CRT_ERROR) && defined(_DEBUG)
        int reportMode = _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_WNDW);
        _CrtSetReportMode(_CRT_ERROR, reportMode);
        int ret = _CrtDbgReport(_CRT_ERROR, __FILE__, __LINE__, QT_VERSION_STR, msg);
        if(ret == 0 && reportMode & _CRTDBG_MODE_WNDW)
            return;
        else if(ret == 1)
            _CrtDbgBreak();
#endif

#if defined(Q_OS_UNIX) && defined(QT_DEBUG)
        abort();
#else
        exit(1);
#endif
    }
}

/*
 Выбирает данные на основании err_budget.txt пользуясь путями,
 указанными в originName по маске originName* в соответствующую диресторию.
 */

int main(int argc, char *argv[])
{
    qInstallMsgHandler(customMessageHandler);
    QCoreApplication a(argc, argv);

    QFile* logFile = new QFile("messages.log");
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
        clog = new QDataStream(logFile);

    //reductionStat rStat;
    errBudgetFile *ebFile;
    //int redType;
    int i, sz0, j, sz1;

    QTextCodec *codec2 = QTextCodec::codecForName("Windows-1251");

    QString workErrB, resPath;

    QSettings *sett = new QSettings("redStatSelect.ini", QSettings::IniFormat);
    if(argc>1) workErrB = codec2->toUnicode(argv[1]);
    else workErrB = codec2->toUnicode(sett->value("general/workErrB", "./err_budget.txt").toByteArray());

    if(argc>2) resPath = codec2->toUnicode(argv[2]);
    else resPath = codec2->toUnicode(sett->value("general/resPath", "./").toByteArray());

    qDebug() << QString("workErrB: %1\n").arg(workErrB);
    qDebug() << QString("resPath: %1\n").arg(resPath);

    QDir().mkpath(resPath);



    //int redType = sett->value("general/redType", 0).toInt();            //0 - common; 1 - obj
    int plNameType = sett->value("general/plNameType", 0).toInt();
/*
    if(!redType)
    {
        ebFileName = new QString(workPath + "err_budget.txt");
        resFileName = new QString(workPath + "residuals.txt");
    }
    else
    {
        ebFileName = new QString(workPath + "err_budget_obj.txt");
        resFileName = new QString(workPath + "residuals_obj.txt");
        mpeFileName = new QString(workPath + "aggregate_eq.txt");
        ssFileName = new QString(workPath + "aggregate_sstar.txt");
    }
*/
    ebFile = new errBudgetFile;
    ebFile->init(workErrB.toAscii().constData());

    sz0 = ebFile->errList.size();
    QString plName, origName;
    QFileInfo fi;
    QString fileName, filePath, nFile, tFile;
    QDir wDir;
    QStringList filters, eList;
    QString filt;

    for(i=0; i<sz0;i++)
    {
        origName = ebFile->errList.at(i)->originName;
        fi.setFile(origName);
        detPlateName(&plName, origName, plNameType);
        fileName = fi.fileName();
        filePath = fi.absolutePath();
        qDebug() << QString("fileName: %1\n").arg(fileName);
        qDebug() << QString("filePath: %1\n").arg(filePath);
        filt = QString("%1*").arg(fileName);

        filters.clear();
        filters << filt;
        qDebug() << QString("filt: %1\n").arg(filt);
        wDir.setPath(filePath);
        qDebug() << QString("wDir.absolutePath: %1\n").arg(wDir.absolutePath());
        wDir.setNameFilters(filters);
        eList.clear();
        eList = wDir.entryList();
        qDebug() << QString("entryList: %1\n").arg(eList.join("\n"));
        sz1 = eList.size();
        for(j=0; j<sz1; j++)
        {
            tFile = filePath+QDir().separator()+eList.at(j);
            if(plNameType) nFile = QString("%1%2").arg(resPath).arg(eList.at(j));
            else nFile = QString("%1%2.%3").arg(resPath).arg(plName).arg(eList.at(j));

            qDebug() << QString("tFile: %1\nnFile: %2\n").arg(tFile).arg(nFile);

            QFile().copy(tFile, nFile);
        }
    }



    delete clog;
    clog = 0;
    delete logFile;
    logFile = 0;

    qInstallMsgHandler(0);

    return 0;//a.exec();
}
