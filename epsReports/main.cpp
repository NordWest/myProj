#include <QtCore>
#include <QCoreApplication>

#include "./../libs/vectGrid3D.h"
#include <mgl/mgl_eps.h>

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
int main(int argc, char *argv[])
{
 //   QCoreApplication a(argc, argv);
    int cx = QString(argv[2]).toInt();// номер колонки по горизонтали
        int cy = QString(argv[3]).toInt();// номер колонки по вертикали
        QString slimits = QString(argv[4]);// строка, содержащая пределы по осям, подписи и т.п.
        QString outfname = QString(argv[6]);// имя файла для вывода eps-картинки
        //загружаем данные из файла
        QFile rfile(argv[1]);
        rfile.open(QIODevice::ReadOnly);
        QByteArray ba = rfile.readAll();
        rfile.close();
        //построчно обрабатываем исходный файл и формируем массивы данных по X и Y
        QVector<double>X,Y;
        QTextStream tstream(ba);
        QString tline;
        while(!tstream.atEnd())
        {
            tline = tstream.readLine();
            X<<tline.section('|',cx,cx).toDouble();
            Y<<tline.section('|',cy,cy).toDouble();
        }
        //
        mglGraph *gr = new mglGraphPS;// создаем экземпляр класса для построения EPS-графика
        mglData x,y;// объявляем массивы типа mglData - с ними работает класс mglGraph
        x.Set(X.data(),X.count());//копируем данные из масивов X и Y в специфические массивы для построения графика
        y.Set(Y.data(),Y.count());
        //
        mglData xs(2),ys(2);//задаем пределы по осям X,Y
        xs.a[0]=slimits.section('|',1,1).toDouble();
        xs.a[1]=slimits.section('|',2,2).toDouble();
        ys.a[0]=slimits.section('|',4,4).toDouble();
        ys.a[1]=slimits.section('|',5,5).toDouble();
        //
        gr->XRange(xs);//эти функции устанавливают требуемые пределы для области рисования
        gr->YRange(ys);
        gr->Label('x',qPrintable(slimits.section('|',0,0)),0);//задаем подписи осей по центру (этим управляет последний параметр, если 1 - то справа, если -1, то слева)
        gr->Label('y',qPrintable(slimits.section('|',3,3)),0);
        gr->Axis("xy");//задаем оси ("xy" - это плоский график)
        gr->Box();//рисуем рамку
        gr->Plot(x,y,argv[5]);//рисуем точки. То, как будут выглядеть точки,  зависит от строки в третьем параметре. Дальше приведу примеры.
        gr->WriteEPS(qPrintable(outfname),qPrintable(outfname));//сохраняем все в eps-файл
        delete gr;
        //
        return 0;
}
*/

struct report3data
{
    double x0, x1, y0, y1;
    int cx, cy;
    QString labelX, labelY, plotParam;
    int isDetCurv, curvDeg;
};

struct report4data
{
    double x0, x1, y0, y1;
    int cx, cy;
    QString labelX, labelY, plotParam;
    int isDetCurv, curvDeg;
};


int main(int argc, char *argv[])
{
    qInstallMsgHandler(customMessageHandler);

    QFile* logFile = new QFile("messages.log");
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
        clog = new QDataStream(logFile);

    QDir tDir;
    int i, szi, j, szj, k, szk;
    QFile rfile;
    QByteArray ba;
    QVector<double>X,Y, Z;
    QTextStream tstream;
    QString outfname, tfname;
    QStringList dirFilters;
    QString r3Dir;
    QStringList fList;

    mglGraph *gr;
    mglData x,y;// объявляем массивы типа mglData - с ними работает класс mglGraph
    mglData xs(2),ys(2);//задаем пределы по осям X,Y

    QString tline;

    QSettings *sett = new QSettings("./epsReports.ini", QSettings::IniFormat);

    QString workDir = sett->value("general/workDir", "./").toString();
    QString dataSeparator = sett->value("general/dataSeparator", "|").toString();

    int isReport0 = sett->value("reports/report0", 0).toInt();
    int isReport3 = sett->value("reports/report3", 0).toInt();
    int isReport4 = sett->value("reports/report4", 0).toInt();


    qDebug() << "workDir: " << workDir << "\n";


    //report3
    report3data r3data;
    r3data.x0 = sett->value("report3/x0", 0).toDouble();
    r3data.x1 = sett->value("report3/x1", 0).toDouble();
    r3data.y0 = sett->value("report3/y0", 0).toDouble();
    r3data.y1 = sett->value("report3/y1", 0).toDouble();
    r3data.cx = sett->value("report3/cx", 0).toInt();
    r3data.cy = sett->value("report3/cy", 1).toInt();
    r3data.labelX = sett->value("report3/labelX", "./").toString();
    r3data.labelY = sett->value("report3/labelY", "./").toString();
    r3data.plotParam = sett->value("report3/plotParam", "./").toString();
    r3data.isDetCurv = sett->value("report3/isDetCurv", 0).toInt();
    r3data.curvDeg = sett->value("report3/curvDeg", 2).toInt();

    //report4
    report4data r4data;
    r4data.x0 = sett->value("report3/x0", 0).toDouble();
    r4data.x1 = sett->value("report3/x1", 0).toDouble();
    r4data.y0 = sett->value("report3/y0", 0).toDouble();
    r4data.y1 = sett->value("report3/y1", 0).toDouble();


//////  report3

    if(isReport3)
    {

        dirFilters << QString("*.txt");
        r3Dir = workDir+"/report3";
        tDir.setPath(r3Dir);
        tDir.setNameFilters(dirFilters);
        fList = tDir.entryList();

        tDir.mkdir("eps");

        szi = fList.size();
        qDebug() << QString("report3 list num= %1\n").arg(szi);

        //qDebug() << "fList:\n" << r3Dir+fList.join("\n");

        gr = new mglGraphPS;

        xs.a[0]=r3data.x0;
        xs.a[1]=r3data.x1;
        ys.a[0]=r3data.y0;
        ys.a[1]=r3data.y1;
        //
        gr->XRange(xs);//эти функции устанавливают требуемые пределы для области рисования
        gr->YRange(ys);
        gr->Label('x',qPrintable(r3data.labelX),0);//задаем подписи осей по центру (этим управляет последний параметр, если 1 - то справа, если -1, то слева)
        gr->Label('y',qPrintable(r3data.labelY),0);
        gr->Axis("xy");//задаем оси ("xy" - это плоский график)

        for(i=0; i<szi; i++)
        {
            tfname = r3Dir+"/"+fList[i];
            outfname = r3Dir+"/eps/"+fList.at(i).left(tfname.lastIndexOf(".txt"))+".eps";

            qDebug() << "tfname: " << tfname << "\n";

            rfile.setFileName(tfname);
            rfile.open(QIODevice::ReadOnly);
            //rfile.readAll();
           // qDebug() << ba.data() << "\n";


            X.clear();
            Y.clear();

            tstream.setDevice(&rfile);

            while(!tstream.atEnd())
            {
                tline = tstream.readLine();
                X<<tline.section(dataSeparator,r3data.cx,r3data.cx).toDouble();
                Y<<tline.section(dataSeparator,r3data.cy,r3data.cy).toDouble();
                //qDebug() << QString("X: %1\tY: %2\n").arg(X[i]).arg(Y[i]);
            }


            qDebug() << QString("Xnum= %1\nYnum= %2\n").arg(X.count()).arg(Y.count());

            mglData y1(5000);
            //y1.Modify("-(2.1688297*10^(-12))*x^3+(1.1074109*10^(-9))*x^2+(3.7306033*10^(-5))*x-0.0156563");
            //y1.Modify("sin(rnd*1000)");
            y1.Modify("-8.98966*10^(-8)*x^3-1.53619*10^(-5)*x^2-1.43487*10(-6)*x^1-1.75675*10^(-7)");



            x.Set(X.data(),X.count());//копируем данные из масивов X и Y в специфические массивы для построения графика
            y.Set(Y.data(),Y.count());

            gr->Box();//рисуем рамку
            gr->Plot(x,y,r3data.plotParam.toAscii().data());//рисуем точки. То, как будут выглядеть точки,  зависит от строки в третьем параметре. Дальше приведу примеры.

            gr->Plot(y1);

            gr->WriteEPS(qPrintable(outfname),qPrintable(outfname));//сохраняем все в eps-файл
            //delete gr;


            rfile.close();

        }
    }

//////
    //report4
    if(isReport4)
    {
        QString r4Dir = workDir+"/report4";
        QString vfFile = r4Dir+"/res.vf";

         vectGrid3D *vectF;
         vectF = new vectGrid3D;
         vectF->init(vfFile.toAscii().data());

         int pos[3];

         gr = new mglGraphPS;

         xs.a[0]=r4data.x0;
         xs.a[1]=r4data.x1;
         ys.a[0]=r4data.y0;
         ys.a[1]=r4data.y1;

         //
         gr->XRange(xs);//эти функции устанавливают требуемые пределы для области рисования
         gr->YRange(ys);




//         gr->Label('x',qPrintable(r3data.labelX),0);//задаем подписи осей по центру (этим управляет последний параметр, если 1 - то справа, если -1, то слева)
 //        gr->Label('y',qPrintable(r3data.labelY),0);
  //       gr->Axis("xy");//задаем оси ("xy" - это плоский график)

         mglData ocX(vectF->ax[0], vectF->ax[1]), Xpos(vectF->ax[0]), ocY(vectF->ax[0], vectF->ax[1]), Ypos(vectF->ax[1]);

         //QVector fX, fY;
         long cpos;
         double *fx, *fy;
         fx = new double[vectF->ax[0]*vectF->ax[1]];
         fy = new double[vectF->ax[0]*vectF->ax[1]];

         QStringList tList;
         tList.clear();
         for(i=0; i<vectF->ax[0]; i++)
         {
             Xpos.a[i] = 1000.0*(vectF->dims.at(0)[i]+vectF->dims.at(0)[i+1])/2.0;
             tList << QString("%1").arg(Xpos.a[i]);
         }
         qDebug() << "Xpos: " << tList.join("|") << "\n";
         tList.clear();
         for(i=0; i<vectF->ax[1]; i++)
         {
            Ypos.a[i] = 1000.0*(vectF->dims.at(1)[i]+vectF->dims.at(1)[i+1])/2.0;
            tList << QString("%1").arg(Xpos.a[i]);
        }
        qDebug() << "Ypos: " << tList.join("|") << "\n";

         for(k=0; k<vectF->ax[2]; k++)
         {
                 pos[2] = k;

                 outfname = vfFile.left(vfFile.lastIndexOf(".")) + QString("%1").arg(k) + ".eps";
                 qDebug() << QString("outfname: %1\n").arg(outfname);

                // X.clear();
                // Y.clear();

                 for(j=0; j<vectF->ax[1]; j++)
                 {
                         pos[1] = j;
                         //Y << vectF->yarr(j);
                         //Ypos.a[j] = (vectF->dims.at(1)[j]+vectF->dims.at(1)[j+1])/2.0;
                         for(i=0; i<vectF->ax[0]; i++)
                         {
                                 pos[0] = i;

                                // ocX.Put(vectF->fX(i, j), i, j);
                                // ocY.Put(fY(i, j), i, j);
                                 //X << vectF->xarr(i);



                                 cpos = vectF->xVectField->detPos(pos);
                                 //ocX.Set(vectF->xVectField->getD(cpos), i, j);
                                 fx[i+j*vectF->ax[0]] = vectF->xVectField->getD(cpos);
                                 cpos = vectF->yVectField->detPos(pos);
                                 fy[i+j*vectF->ax[0]] = vectF->yVectField->getD(cpos);
                                 //ocY.Set(vectF->yVectField->getD(cpos), i, j);
                                 //ocY.a[i + j*vectF->ax[0]] = vectF->yVectField->getD(cpos);

                                 qDebug() << QString("fx= %1\tfy= %2\n").arg(ocX.a[i + j*vectF->ax[0]]).arg(ocY.a[i + j*vectF->ax[0]]);
                                 //cpos = vectNumField->detPos(pos);
                                 //vectF->fN(i, j) = vectF->vectNumField->getL(cpos);
     //				outStream << fX(i, j) << "\t";
                         }
     //			outStream << "\n";
                 }

                 ocX.Set(fx, vectF->ax[0], vectF->ax[1]);
                 ocY.Set(fy, vectF->ax[0], vectF->ax[1]);

                 gr->Box();//рисуем рамку
                 gr->Vect(Xpos, Ypos, ocX,ocY,r3data.plotParam.toAscii().data());
                 gr->WriteEPS(qPrintable(outfname),qPrintable(outfname));

         }
     }


    delete clog;
    clog = 0;
    delete logFile;
    logFile = 0;

    qInstallMsgHandler(0);

    return 0;
}

int detCurv(QVector<double> X, QVector<double> Y, report3data r3data, QVector<double> *Z)
{
    int i, j;
    int num = X.count();
    int deg = r3data.curvDeg;
    double *z = new double[deg];
    double *c = new double[deg*num];
    double *we = new double[num];
    double *x = new double[num];
    double *y = new double[num];

    for(i=0; i<num; i++)
    {

        we[i] = 0;
    }

}
