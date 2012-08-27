#include <QtCore>
#include <QApplication>

//#include "./../libs/vectGrid3D.h"
//#include <mgl/mgl_eps.h>

#include <QRgb>
#include <QPen>
#include <QBrush>
#include <QTextOption>
#include <QPainter>
#include <QImage>

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

class vfPlot
{
    QPainter painter;
    QRect boxRect, xAxeRect, yAxeRect;
    QPoint pPos, pPos0, pPos1;
    int picWidth, picHeight;
    int legendW;
    QRect xTextRect, yTextRect;
    QRect scaleRect;
    QRect headerRect;
    QRect legendRect;

    int boxW, boxH;
    double scX, scY;
    int baseSize, axesSize, gridSize, dataSize, pointSize, trendSize;

    double xs0, xs1, ys0, ys1;
    double scLine;
    int xdir, ydir;

    int axeLabelsFontSize, headerFontSize, legendFontSize;

    int mainStepNumX, mainStepNumY;
    int subStepNumX,  subStepNumY;
    double minX, maxX, minY, maxY, diapX, diapY;
    double minDX, maxDX, minDY, maxDY, diapDX, diapDY, meanD;

    QString hLabel;
    QString scLabel;
    QString xLabel, yLabel;

    int isAutoScPatt;
    double scPatt;

    int dataNumsFontSize;

    QImage *baseImg;
    QVector<QRgb> colormap;
    QPen basePen;
    QPen axesPen;
    QPen gridPen;
    QPen dataPen;
    QPen pointPen;
    QPen trendPen;

    QBrush baseBrush;
    QBrush pointBrush;
    QTransform transform;

    QStringList colorList;

    //QVector<QRgb> colormap;

    QTextOption xAxeLabelOpt, yAxeLabelOpt, legendOpt;

public:

    vfPlot();
    ~vfPlot();
    void init(QString iniFileName = "./vfPlot.ini");
    void setLimits(double xs0, double xs1, double ys0, double ys1);
    void setScLine(double scale);

    void drawVectField(double *x, double *y, double*dx, double *dy, int num, int isAutoLimits = 0, int isColorList = 0, int *cols = NULL);
    void drawVectFieldNums(double *x, double *y, double*dx, double *dy, int *nums, int num, int isAutoLimits = 0, int numsMin = 0, int isColorList = 0, int *cols = NULL);
    void drawDotPlot(double *x, double *y, int num, int isLines = 0, int isAutoLimits = 0, double*dx = NULL, double *dy = NULL, int *nums = NULL, int isColorList = 0);
    void drawDotPlot1(double *x, double *y, double *dx, double *dy, int num, int isLines, int isAutoLimits, int isLegend, QStringList rowNames, int isColorList);



    void savePlot(QString fileName, const char* picType);
    void detLimits(double *x, double *y, int num);
    void detDLimits(double *dx, double *dy, int num);
    void detDiaps(double *x, double *y, int num);

    double pix2data(int pixX, int pixY, double *valX, double *valY);
    double data2pix(double valX, double valY, int *pixX, int *pixY);

    void setLabels(QString xLab, QString yLab);
    void setScaleLabel(QString scLab);
    void setHeadLabel(QString hLab);

    void drawLabels(QString xLab, QString yLab);
    void drawHeadLabel(QString hLab);
    void drawAxes(int mSNX, int mSNY, int sSNX = 0, int sSNY = 0);
    void drawScale(int aScale, double sc = 0);
    void drawLegend(QStringList namesList, int isColorList);

    void setScAuto(int isAuto);
    void setScPatt(double scP);

    int isPlotScale;

};


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    setlocale(LC_NUMERIC, "C");

    if(argc!=5) return 1;


   // QTextCodec *codec1 = QTextCodec::codecForName("KOI8-R");
   // Q_ASSERT( codec1 );

    QTextCodec *codec1 = QTextCodec::codecForName("Windows-1251");
    Q_ASSERT( codec1 );



    QString iniFile = codec1->toUnicode(argv[3]);

    QSettings *sett = new QSettings(iniFile, QSettings::IniFormat);

    int taskNum = sett->value("general/taskNum", 0).toInt();
    /*
     taskNum=0 - vectField
     taskNum=1 - dotPlot
     */

    int cx = sett->value("general/cx", 0).toInt();// номер колонки по горизонтали
    int cy = sett->value("general/cy", 1).toInt();// номер колонки по вертикали
    int cdx = sett->value("general/cdx", 2).toInt();// номер колонки по горизонтали
    int cdy = sett->value("general/cdy", 3).toInt();// номер колонки по вертикали*/
    int cn = sett->value("general/cn", 4).toInt();// номер колонки по вертикали*/
    int ccol = sett->value("general/col", 5).toInt();// номер колонки с цветами*/

    QString colSep = sett->value("general/colSep", "|").toString();

    double xs0=sett->value("general/xs0", 0).toDouble();
    double xs1=sett->value("general/xs1", 1).toDouble();
    double ys0=sett->value("general/ys0", 0).toDouble();
    double ys1=sett->value("general/ys1", 1).toDouble();
    int isAutoLimits = sett->value("general/isAutoLimits", 0).toInt();

    int isAutoScPatt = sett->value("general/isAutoScPatt", 0).toInt();
    double scPatt = sett->value("general/scPatt", 100).toDouble();

    double scLine = sett->value("general/scLine", 100).toDouble();

    int isNumsPlot = sett->value("general/isNumsPlot", 0).toInt();
    int numsMin = sett->value("general/numsMin", 0).toInt();
    int isErrBars = sett->value("general/isErrBars", 0).toInt();
    int isLines = sett->value("general/isLines", 0).toInt();
    int isPlotScale = sett->value("general/isPlotScale", 1).toInt();
    int isColorList = sett->value("general/isColorList", 0).toInt();
    int isLegend = sett->value("general/isLegend", 0).toInt();
    QString xLabel =  codec1->toUnicode(sett->value("plot/xLabel", "X").toByteArray());
    QString yLabel =  codec1->toUnicode(sett->value("plot/yLabel", "Y").toByteArray());
    QString hLabel =  codec1->toUnicode(sett->value("plot/hLabel", "Header").toByteArray());
    QString scLabel =  codec1->toUnicode(sett->value("plot/scLabel", ", pix").toByteArray());

    QStringList namesList;


      /*  int cx = QString(argv[2]).toInt();// номер колонки по горизонтали
        int cy = QString(argv[3]).toInt();// номер колонки по вертикали
        int cdx = QString(argv[4]).toInt();// номер колонки по горизонтали
        int cdy = QString(argv[5]).toInt();// номер колонки по вертикали*/

        //QString slimits = QString(argv[6]);// строка, содержащая пределы по осям, подписи и т.п.
        //QString outfname = QString(argv[8]);// имя файла для вывода eps-картинки
        QString outfname = QString(argv[2]);
        //загружаем данные из файла
        QFile rfile(argv[1]);
        rfile.open(QIODevice::ReadOnly);
        QByteArray ba = rfile.readAll();
        rfile.close();

        //QString resFileName(argv[7]);
        //построчно обрабатываем исходный файл и формируем массивы данных по X и Y
        QVector<double>X,Y, dX, dY;
        QVector<int> N;
        QVector<int> Col;
        QTextStream tstream(ba);
        QString tline;
        int ni;
        while(!tstream.atEnd())
        {
            tline = tstream.readLine();
            switch(taskNum)
            {
            case 0:
                {
                    ni = tline.section(colSep,cn,cn, QString::SectionSkipEmpty).toDouble();
                    if(ni>=numsMin)
                    {
                        X<<tline.section(colSep,cx,cx, QString::SectionSkipEmpty).toDouble();
                        Y<<tline.section(colSep,cy,cy, QString::SectionSkipEmpty).toDouble();
                        dX<<tline.section(colSep,cdx,cdx, QString::SectionSkipEmpty).toDouble();
                        dY<<tline.section(colSep,cdy,cdy, QString::SectionSkipEmpty).toDouble();
                        Col << tline.section(colSep,ccol,ccol, QString::SectionSkipEmpty).toDouble();

                        if(isNumsPlot) N<<ni;
                    }
                }
                break;
            case 1:
                {
                    X<<tline.section(colSep,cx,cx, QString::SectionSkipEmpty).toDouble();
                    Y<<tline.section(colSep,cy,cy, QString::SectionSkipEmpty).toDouble();
                    if(isErrBars)
                    {
                        dX<<tline.section(colSep,cdx,cdx, QString::SectionSkipEmpty).toDouble();
                        dY<<tline.section(colSep,cdy,cdy, QString::SectionSkipEmpty).toDouble();
                    }
                    if(isNumsPlot) N<<tline.section(colSep,cn,cn, QString::SectionSkipEmpty).toDouble();
                }
                break;
            case 2:
                {
                    X<<tline.section(colSep,cx,cx, QString::SectionSkipEmpty).toDouble();
                    Y<<tline.section(colSep,cy,cy, QString::SectionSkipEmpty).toDouble();
                    if(isErrBars)
                    {
                        dX<<tline.section(colSep,cdx,cdx, QString::SectionSkipEmpty).toDouble();
                        dY<<tline.section(colSep,cdy,cdy, QString::SectionSkipEmpty).toDouble();
                    }
                    namesList<<tline.section(colSep,ccol,ccol, QString::SectionSkipEmpty);
                }
                break;


            }

        }

        int dotNum = X.size();
        int i;

        double *x, *y, *dx, *dy;
        int *nums, *cols;
        x = new double[dotNum];
        y = new double[dotNum];
        if((taskNum==0)||(taskNum==1&&isErrBars))
        {
            dx = new double[dotNum];
            dy = new double[dotNum];
            dx = dX.data();
            dy = dY.data();
        }
        else
        {
            dx = NULL;
            dy = NULL;
        }

        x = X.data();
        y = Y.data();

        if(isNumsPlot)
        {
            nums = new int[dotNum];
            nums = N.data();
        }
        else nums = NULL;

        if(isColorList)
        {
            cols = new int[dotNum];
            cols = Col.data();
        }
        else cols = NULL;


        vfPlot plot;
        if(argc==5) plot.init(QString(argv[4]));
        else plot.init();
        plot.setLimits(xs0, xs1, ys0, ys1);
        plot.setScLine(scLine);
        plot.setLabels(xLabel, yLabel);
        plot.setHeadLabel(hLabel);
        plot.setScaleLabel(scLabel);
        plot.setScAuto(isAutoScPatt);
        plot.setScPatt(scPatt);
        plot.isPlotScale = isPlotScale;
        //plot.drawLabels(xLabel, yLabel);
       //plot.drawAxes(5, 5, 1, 1);
        switch(taskNum)
        {
        case 0:
            {
                if(isNumsPlot) plot.drawVectFieldNums(x, y, dx, dy, nums, dotNum, isAutoLimits, numsMin, isColorList, cols);
                else plot.drawVectField(x, y, dx, dy, dotNum, isAutoLimits, isColorList, cols);
            }
            break;
        case 1:
            {
                if(isErrBars) plot.drawDotPlot(x, y, dotNum, isLines, isAutoLimits, dx, dy, nums, isColorList);
                else plot.drawDotPlot(x, y, dotNum, isLines, isAutoLimits, NULL, NULL, nums, isColorList);
            }
            break;
        case 2:
            {
                if(isErrBars) plot.drawDotPlot1(x, y, dx, dy, dotNum, isLines, isAutoLimits, isLegend, namesList, isColorList);
                else plot.drawDotPlot1(x, y, NULL, NULL, dotNum, isLines, isAutoLimits, isLegend, namesList, isColorList);
            }
            break;
        }
        plot.savePlot(outfname, "BMP");

        //
       /* double *x, *y, *dx, *dy;
        x = new double[dotNum];
        y = new double[dotNum];
        dx = new double[dotNum];
        dy = new double[dotNum];
        for(i=0; i<dotNum; i++)
        {
            x[i] = X[i];

        }*/

/*

        int nX = 800;
        int nY = 800;
        double fH = fabs(xs1-xs0);
        double fW = fabs(ys1-ys0);
        double scX = boxW/fH;
        double scY = boxH/fW;

    basePen.setColor(Qt::black);
    basePen.setWidth(5);

    axesPen.setColor(Qt::lightGray);
    axesPen.setWidth(5);

    gridPen.setColor(Qt::cyan);
    gridPen.setWidth(3);
    gridPen.setStyle(Qt::DashLine);

    dataPen.setColor(Qt::blue);
    dataPen.setWidth(5);

    pointPen.setColor(Qt::darkGreen);
    pointPen.setWidth(lineSize);

    trendPen.setColor(Qt::red);
    trendPen.setWidth(1);

        //

        baseImg = new QImage(picWidth, picHeight, QImage::Format_RGB444);
        QPainter painter(baseImg);
        if(!painter.isActive())
        {
                qDebug() << "\npainter is\'t active\n";
                return 1;
        }
        painter.fillRect(0, 0, picWidth, picHeight, baseBrush);

        QPoint pPos, pPos0, pPos1;

        QRect boxRect;
        pPos0.setX(picWidth/2 - boxW/2);
        pPos0.setY(picHeight/2 - boxH/2);
        pPos1.setX(picWidth/2 + boxW/2);
        pPos1.setY(picHeight/2 + boxH/2);
        boxRect.setBottomRight(pPos1);
        boxRect.setTopLeft(pPos0);

        painter.setPen(axesPen);
        painter.drawRect(boxRect);
        /*painter.drawLine(-dX, 0, -dX, maxIH);
        painter.drawLine(-dX, 0, -dX-5, 25);
        painter.drawLine(-dX, 0, -dX+5, 25);
        painter.drawLine(0, -dY, maxIW, -dY);
        painter.drawLine(maxIW, -dY, maxIW-25, -dY-5);
        painter.drawLine(maxIW, -dY, maxIW-25, -dY+5);
        /

        painter.setPen(pointPen);
        painter.setBrush(pointBrush);



        for(i=0; i<dotNum; i++)
        {
            pPos.setX(scX*(X.at(i)-xs0)+boxRect.topLeft().x());
            pPos.setY(scY*(ys1-Y.at(i))+boxRect.topLeft().y());
            pPos1.setX(scX*(X[i]+dX[i]*scLine-xs0)+boxRect.topLeft().x());
            pPos1.setY(scY*(ys1-Y[i]+dY[i]*scLine)+boxRect.topLeft().y());
            painter.drawEllipse(pPos, dotSize, dotSize);
            painter.drawLine(pPos.x(), pPos.y(), pPos1.x(), pPos1.y());
        }

        painter.end();
        baseImg->save(outfname, "BMP");
*/
        //
        return 0;
}



vfPlot::vfPlot()
{
    baseSize = 5;
    axesSize = 5;
    gridSize = 3;
    dataSize = 5;
    pointSize = 3;
    trendSize = 1;

    basePen.setColor(Qt::black);
    basePen.setWidth(baseSize);

    axesPen.setColor(Qt::lightGray);
    axesPen.setWidth(axesSize);

    gridPen.setColor(Qt::cyan);
    gridPen.setWidth(gridSize);
    gridPen.setStyle(Qt::DashLine);

    dataPen.setColor(Qt::blue);
    dataPen.setWidth(dataSize);

    pointPen.setColor(Qt::black);
    pointPen.setWidth(pointSize);

    trendPen.setColor(Qt::red);
    trendPen.setWidth(trendSize);

    baseBrush.setColor(Qt::white);
    baseBrush.setStyle(Qt::SolidPattern);
    pointBrush.setColor(Qt::black);
    pointBrush.setStyle(Qt::SolidPattern);

    scLine = 1;
    xs0 = 0;
    xs1 = 1;
    ys0 = 0;
    ys1 = 1;

    mainStepNumX = 5;
    mainStepNumY = 5;
    subStepNumX = 3;
    subStepNumY = 3;

    hLabel = "Header";
    headerFontSize = 45;

    xLabel = "X";
    yLabel = "Y";
    axeLabelsFontSize = 15;

    scLabel = ", pix";

    dataNumsFontSize = 15;

    xAxeLabelOpt.setAlignment(Qt::AlignCenter);
    yAxeLabelOpt.setAlignment(Qt::AlignLeft);

    legendOpt.setAlignment(Qt::AlignCenter);

    isAutoScPatt = 1;
    scPatt = 1;

    isPlotScale=1;
}

vfPlot::~vfPlot()
{

}

void vfPlot::init(QString iniFileName)
{
    QSettings *sett = new QSettings(iniFileName, QSettings::IniFormat);
    picWidth = sett->value("general/picWidth", 800).toInt();
    picHeight = sett->value("general/picHeight", 800).toInt();

    boxW = sett->value("general/boxW", 700).toInt();
    boxH = sett->value("general/boxH", 700).toInt();
    legendW = sett->value("general/legendW", 200).toInt();

    headerFontSize = sett->value("general/headerFontSize", 45).toInt();

    dataNumsFontSize = sett->value("general/dataNumsFontSize", 15).toInt();
    //axeLabelsFontSize = sett->value("general/axeLabelsFontSize", 15).toInt();
    legendFontSize = sett->value("general/legendFontSize", 15).toInt();
    colorList = sett->value("general/colorList", "0x000000").toString().split("|");

    baseSize = sett->value("pens/baseSize", 5).toInt();
    axesSize = sett->value("pens/axesSize", 5).toInt();
    gridSize = sett->value("pens/gridSize", 3).toInt();
    dataSize = sett->value("pens/dataSize", 5).toInt();
    pointSize = sett->value("pens/pointSize", 3).toInt();
    trendSize = sett->value("pens/trendSize", 1).toInt();

    basePen.setWidth(baseSize);
    axesPen.setWidth(axesSize);
    gridPen.setWidth(gridSize);
    dataPen.setWidth(dataSize);
    pointPen.setWidth(pointSize);
    trendPen.setWidth(trendSize);

    mainStepNumX = sett->value("axes/mainStepNumX", 5).toInt();
    mainStepNumY = sett->value("axes/mainStepNumY", 5).toInt();
    subStepNumX = sett->value("axes/subStepNumX", 0).toInt();
    subStepNumY = sett->value("axes/subStepNumY", 0).toInt();
    axeLabelsFontSize = sett->value("axes/axeLabelsFontSize", 15).toInt();



    //isAutoScPatt = sett->value("scale/isAutoScPatt", 1).toInt();
    //scPatt = sett->value("scale/scPatt", 1).toDouble();


}

void vfPlot::setLimits(double x0, double x1, double y0, double y1)
{
    xs0 = x0;
    xs1 = x1;
    ys0 = y0;
    ys1 = y1;
    scX = boxW/fabs(xs1-xs0);
    scY = boxH/fabs(ys1-ys0);
}

void vfPlot::setScLine(double scale)
{
    scLine = scale;
}

void vfPlot::drawVectField(double *x, double *y, double*dx, double *dy, int num, int isAutoLimits, int isColorList, int *cols)
{
    qDebug() << "vfPlot::drawVectField\n";
    int i;
    if(isAutoLimits) detLimits(x, y, num);
    detDLimits(dx, dy, num);

    baseImg = new QImage(picWidth, picHeight, QImage::Format_RGB444);
    //baseImg->setColorTable(colormap);
    painter.begin(baseImg);
    if(!painter.isActive())
    {
            qDebug() << "\npainter is\'t active\n";
            return;
    }

    painter.fillRect(0, 0, picWidth, picHeight, baseBrush);
//boxRect
    /*pPos0.setX(picWidth/2 - boxW/2);
    pPos0.setY(picHeight/2 - boxH/2);
    pPos1.setX(picWidth/2 + boxW/2);
    pPos1.setY(picHeight/2 + boxH/2);
    boxRect.setBottomRight(pPos1);
    boxRect.setTopLeft(pPos0);*/
    pPos0.setX(picWidth - picWidth*0.05 - boxW);
    pPos0.setY(picHeight/2 - boxH/2);
    pPos1.setX(picWidth - picWidth*0.05);
    pPos1.setY(picHeight/2 + boxH/2);
    boxRect.setBottomRight(pPos1);
    boxRect.setTopLeft(pPos0);
//xAxeRect
    pPos0.setX(boxRect.left());
    pPos0.setY(boxRect.bottom());
    pPos1.setX(boxRect.right());
    pPos1.setY(picHeight);
    xAxeRect.setBottomRight(pPos1);
    xAxeRect.setTopLeft(pPos0);
/*xLabelRect
    pPos0.setX(picWidth/2 - boxW/2);
    pPos0.setY(picHeight/2 + boxH/2);
    pPos1.setX(picWidth);
    pPos1.setY(picHeight);
    xAxeRect.setBottomRight(pPos1);
    xAxeRect.setTopLeft(pPos0);*/
//yAxeRect
    pPos0.setX(0);
    pPos0.setY(boxRect.top());
    pPos1.setX(boxRect.left()/2.0);
    pPos1.setY(boxRect.bottom());
    yAxeRect.setTopLeft(pPos0);
    yAxeRect.setBottomRight(pPos1);

    //painter.drawEllipse(yAxeRect.center(), 5, 5);

//headRect
    pPos0.setX(0);
    pPos0.setY(0);
    pPos1.setX(picWidth);
    pPos1.setY(boxRect.top());
    headerRect.setTopLeft(pPos0);
    headerRect.setBottomRight(pPos1);


    painter.setPen(axesPen);
    painter.drawRect(boxRect);


    painter.setBrush(pointBrush);

    int pixX, pixY;
    int pixX0, pixY0;

    qDebug() << "cols: " << cols << "\n";

    for(i=0; i<num; i++)
    {
        if(isColorList&&cols!=NULL)
        {

            if(cols[i]<colorList.size())
            {
                qDebug() << QString("nums= %1\tcolor= %2\n").arg(cols[i]).arg(colorList.at(cols[i]));
                pointPen.setColor(QColor(colorList.at(cols[i])));
                pointBrush.setColor(QColor(colorList.at(cols[i])));
                trendPen.setColor(QColor(colorList.at(cols[i])));
            }
            else
            {
                pointPen.setColor(Qt::black);
                pointBrush.setColor(Qt::black);
                trendPen.setColor(Qt::black);
            }
            //painter.setPen(pointPen);
            //painter.setBrush(pointBrush);
            //painter.drawEllipse(pPos, pointSize, pointSize);
        }


        painter.setPen(pointPen);
        data2pix(x[i], y[i], &pixX, &pixY);
        pPos.setX(pixX);
        pPos.setY(pixY);
        //pPos.setX(scX*(x[i]-xs0)+boxRect.topLeft().x());
        //pPos.setY(scY*(ys1-y[i])+boxRect.topLeft().y());
        data2pix(x[i]+dx[i]*scLine, y[i]+dy[i]*scLine, &pixX, &pixY);
        pPos1.setX(pixX);
        pPos1.setY(pixY);
        //pPos1.setX(scX*(x[i]+dx[i]*scLine-xs0)+boxRect.topLeft().x());
        //pPos1.setY(scY*(ys1-y[i]+dy[i]*scLine)+boxRect.topLeft().y());
        painter.drawEllipse(pPos, pointSize, pointSize);
        painter.setPen(trendPen);
        painter.drawLine(pPos.x(), pPos.y(), pPos1.x(), pPos1.y());
    }

//Header

    drawHeadLabel(hLabel);


//Labels
    drawLabels(xLabel, yLabel);


//Axes
    drawAxes(mainStepNumX, mainStepNumY, subStepNumX, subStepNumY);

//Scale
    drawScale(isAutoScPatt, scPatt);


    painter.end();
}

void vfPlot::drawVectFieldNums(double *x, double *y, double*dx, double *dy, int *nums, int num, int isAutoLimits, int numsMin, int isColorList, int *cols)
{
    qDebug() << "vfPlot::drawVectFieldNums\n";
    int i;
    if(isAutoLimits) detLimits(x, y, num);
    detDLimits(dx, dy, num);

    baseImg = new QImage(picWidth, picHeight, QImage::Format_RGB444);
    //baseImg->setColorTable(colormap);
    painter.begin(baseImg);
    if(!painter.isActive())
    {
            qDebug() << "\npainter is\'t active\n";
            return;
    }

    painter.fillRect(0, 0, picWidth, picHeight, baseBrush);
//boxRect
/*    pPos0.setX(picWidth/2 - boxW/2);
    pPos0.setY(picHeight/2 - boxH/2);
    pPos1.setX(picWidth/2 + boxW/2);
    pPos1.setY(picHeight/2 + boxH/2);
    boxRect.setBottomRight(pPos1);
    boxRect.setTopLeft(pPos0);*/
    pPos0.setX(picWidth - picWidth*0.05 - boxW);
    pPos0.setY(picHeight/2 - boxH/2);
    pPos1.setX(picWidth - picWidth*0.05);
    pPos1.setY(picHeight/2 + boxH/2);
    boxRect.setBottomRight(pPos1);
    boxRect.setTopLeft(pPos0);
//xAxeRect
    pPos0.setX(boxRect.left());
    pPos0.setY(boxRect.bottom());
    pPos1.setX(boxRect.right());
    pPos1.setY(picHeight);
    xAxeRect.setBottomRight(pPos1);
    xAxeRect.setTopLeft(pPos0);
/*xLabelRect
    pPos0.setX(picWidth/2 - boxW/2);
    pPos0.setY(picHeight/2 + boxH/2);
    pPos1.setX(picWidth);
    pPos1.setY(picHeight);
    xAxeRect.setBottomRight(pPos1);
    xAxeRect.setTopLeft(pPos0);*/
//yAxeRect
    pPos0.setX(0);
    pPos0.setY(boxRect.top());
    pPos1.setX(boxRect.left()/2.0);
    pPos1.setY(boxRect.bottom());
    yAxeRect.setTopLeft(pPos0);
    yAxeRect.setBottomRight(pPos1);

    //painter.drawEllipse(yAxeRect.center(), 5, 5);

//headRect
    pPos0.setX(0);
    pPos0.setY(0);
    pPos1.setX(picWidth);
    pPos1.setY(boxRect.top());
    headerRect.setTopLeft(pPos0);
    headerRect.setBottomRight(pPos1);


    painter.setPen(axesPen);
    painter.drawRect(boxRect);


    painter.setBrush(pointBrush);

    int pixX, pixY;
    int pixX0, pixY0;
    painter.setFont(QFont("Times", dataNumsFontSize, -1, true));
    QString dataText;
    QRect tempRect;
    for(i=0; i<num; i++)
    {
        if(nums[i]<numsMin) continue;

        if(isColorList&&cols!=NULL)
        {

            if(cols[i]<colorList.size())
            {
                qDebug() << QString("nums= %1\tcolor= %2\n").arg(cols[i]).arg(colorList.at(cols[i]));
                pointPen.setColor(QColor(colorList.at(cols[i])));
                pointBrush.setColor(QColor(colorList.at(cols[i])));
                trendPen.setColor(QColor(colorList.at(cols[i])));
            }
            else
            {
                pointPen.setColor(Qt::black);
                pointBrush.setColor(Qt::black);
                trendPen.setColor(Qt::black);
            }
            //painter.setPen(pointPen);
            //painter.setBrush(pointBrush);
            //painter.drawEllipse(pPos, pointSize, pointSize);
        }
    /*    else
        {
            tempRect.setRect(pixX-40, pixY-20, 80, 40);
            painter.drawText(tempRect, QString("%1").arg(nums[i]), xAxeLabelOpt);
        }*/

        painter.setPen(pointPen);
        data2pix(x[i], y[i], &pixX, &pixY);
        pPos.setX(pixX);
        pPos.setY(pixY);
        tempRect.setRect(pixX-40, pixY-20, 80, 40);
        //pPos.setX(scX*(x[i]-xs0)+boxRect.topLeft().x());
        //pPos.setY(scY*(ys1-y[i])+boxRect.topLeft().y());
        data2pix(x[i]+dx[i]*scLine, y[i]+dy[i]*scLine, &pixX, &pixY);
        pPos1.setX(pixX);
        pPos1.setY(pixY);

        //pPos1.setX(scX*(x[i]+dx[i]*scLine-xs0)+boxRect.topLeft().x());
        //pPos1.setY(scY*(ys1-y[i]+dy[i]*scLine)+boxRect.topLeft().y());
        //painter.drawEllipse(pPos, pointSize, pointSize);
        //dataText = QString("%1").arg(nums[i]);
        painter.drawText(tempRect, QString("%1").arg(nums[i]), xAxeLabelOpt);
        painter.setPen(trendPen);
        painter.drawLine(pPos.x(), pPos.y(), pPos1.x(), pPos1.y());
    }

//Header

    drawHeadLabel(hLabel);


//Labels
    drawLabels(xLabel, yLabel);


//Axes
    drawAxes(mainStepNumX, mainStepNumY, subStepNumX, subStepNumY);

//Scale
    drawScale(isAutoScPatt, scPatt);


    painter.end();
}

void vfPlot::drawDotPlot(double *x, double *y, int num, int isLines, int isAutoLimits, double *dx, double *dy, int *nums, int isColorList)
{
    qDebug() << "vfPlot::drawDotPlot\n";
    int i;
    if(isAutoLimits) detLimits(x, y, num);
    //detDLimits(dx, dy, num);

    baseImg = new QImage(picWidth, picHeight, QImage::Format_RGB444);
    //baseImg->setColorTable(colormap);
    painter.begin(baseImg);
    if(!painter.isActive())
    {
            qDebug() << "\npainter is\'t active\n";
            return;
    }

    painter.fillRect(0, 0, picWidth, picHeight, baseBrush);
//boxRect
    pPos0.setX(picWidth - picWidth*0.05 - boxW);
    pPos0.setY(picHeight/2 - boxH/2);
    pPos1.setX(picWidth - picWidth*0.05);
    pPos1.setY(picHeight/2 + boxH/2);
    boxRect.setBottomRight(pPos1);
    boxRect.setTopLeft(pPos0);
    /*
    pPos0.setX(picWidth/2 - boxW/2);
    pPos0.setY(picHeight/2 - boxH/2);
    pPos1.setX(picWidth/2 + boxW/2);
    pPos1.setY(picHeight/2 + boxH/2);
    boxRect.setBottomRight(pPos1);
    boxRect.setTopLeft(pPos0);
    */
//xAxeRect
    pPos0.setX(boxRect.left());
    pPos0.setY((boxRect.bottom()+picHeight)/2.0);
    pPos1.setX(boxRect.right());
    pPos1.setY(picHeight);
    xAxeRect.setBottomRight(pPos1);
    xAxeRect.setTopLeft(pPos0);
/*xLabelRect
    pPos0.setX(picWidth/2 - boxW/2);
    pPos0.setY(picHeight/2 + boxH/2);
    pPos1.setX(picWidth);
    pPos1.setY(picHeight);
    xAxeRect.setBottomRight(pPos1);
    xAxeRect.setTopLeft(pPos0);*/
//yAxeRect
    pPos0.setX(0);
    pPos0.setY(boxRect.top());
    pPos1.setX(boxRect.left()/2.0);
    pPos1.setY(boxRect.bottom());
    yAxeRect.setTopLeft(pPos0);
    yAxeRect.setBottomRight(pPos1);

    //painter.drawEllipse(yAxeRect.center(), 5, 5);

//headRect
    pPos0.setX(0);
    pPos0.setY(0);
    pPos1.setX(picWidth);
    pPos1.setY(boxRect.top());
    headerRect.setTopLeft(pPos0);
    headerRect.setBottomRight(pPos1);


    painter.setPen(axesPen);
    painter.drawRect(boxRect);


    painter.setBrush(pointBrush);

    int pixX, pixY;
    int pixX0, pixY0;
    painter.setFont(QFont("Times", dataNumsFontSize, -1, true));
    QString dataText;
    QRect tempRect;
    QPoint tempPos;
    for(i=0; i<num; i++)
    {
        if(i>0)
        {
            tempPos = pPos;
        }
        painter.setPen(pointPen);
        data2pix(x[i], y[i], &pixX, &pixY);
        pPos.setX(pixX);
        pPos.setY(pixY);


        if(i==0)
        {
            tempPos = pPos;
        }


        //pPos.setX(scX*(x[i]-xs0)+boxRect.topLeft().x());
        //pPos.setY(scY*(ys1-y[i])+boxRect.topLeft().y());


        //pPos1.setX(scX*(x[i]+dx[i]*scLine-xs0)+boxRect.topLeft().x());
        //pPos1.setY(scY*(ys1-y[i]+dy[i]*scLine)+boxRect.topLeft().y());
        //painter.drawEllipse(pPos, pointSize, pointSize);
        //dataText = QString("%1").arg(nums[i]);
        if(nums!=NULL)
        {
            if(isColorList)
            {

                if(nums[i]<colorList.size())
                {
                    qDebug() << QString("nums= %1\tcolor= %2\n").arg(nums[i]).arg(colorList.at(nums[i]));
                    pointPen.setColor(colorList.at(nums[i]));
                    pointBrush.setColor(colorList.at(nums[i]));
                }
                else
                {
                    pointPen.setColor(Qt::black);
                    pointBrush.setColor(Qt::black);
                }
                painter.setPen(pointPen);
                painter.setBrush(pointBrush);
                painter.drawEllipse(pPos, pointSize, pointSize);
            }
            else
            {
                tempRect.setRect(pixX-40, pixY-20, 80, 40);
                painter.drawText(tempRect, QString("%1").arg(nums[i]), xAxeLabelOpt);
            }
        }
        else
        {
            painter.drawEllipse(pPos, pointSize, pointSize);
        }

        if(isLines)
        {
          //  qDebug() << "\ndrawLine\n";
            painter.setPen(pointPen);
            painter.drawLine(pPos.x(), pPos.y(), tempPos.x(), tempPos.y());
        }

        if(dx!=NULL)
        {
         //   qDebug() << "\ndrawDx\n";
            painter.setPen(trendPen);
            data2pix(x[i]+dx[i]*scLine, y[i], &pixX, &pixY);
            pPos1.setX(pixX);
            pPos1.setY(pixY);
            painter.drawLine(pPos.x(), pPos.y(), pPos1.x(), pPos1.y());
            data2pix(x[i]-dx[i]*scLine, y[i], &pixX, &pixY);
            pPos1.setX(pixX);
            pPos1.setY(pixY);
            painter.drawLine(pPos.x(), pPos.y(), pPos1.x(), pPos1.y());
        }
        if(dy!=NULL)
        {
         //   qDebug() << "\ndrawDy\n";
            painter.setPen(trendPen);
            data2pix(x[i], y[i]+dy[i]*scLine, &pixX, &pixY);
            pPos1.setX(pixX);
            pPos1.setY(pixY);
            painter.drawLine(pPos.x(), pPos.y(), pPos1.x(), pPos1.y());
            data2pix(x[i], y[i]-dy[i]*scLine, &pixX, &pixY);
            pPos1.setX(pixX);
            pPos1.setY(pixY);
            painter.drawLine(pPos.x(), pPos.y(), pPos1.x(), pPos1.y());
        }

    }

//Header

    drawHeadLabel(hLabel);


//Labels
    drawLabels(xLabel, yLabel);


//Axes
    drawAxes(mainStepNumX, mainStepNumY, subStepNumX, subStepNumY);

//Scale
    //drawScale(isAutoScPatt, scPatt);


    painter.end();
}

void vfPlot::drawDotPlot1(double *x, double *y, double *dx, double *dy, int num, int isLines, int isAutoLimits, int isLegend, QStringList rowNames, int isColorList)
{
    qDebug() << "vfPlot::drawDotPlot\n";
    int i;
    if(isAutoLimits) detLimits(x, y, num);
    //detDLimits(dx, dy, num);

    QStringList uniqNames;

    baseImg = new QImage(picWidth, picHeight, QImage::Format_RGB444);
    //baseImg->setColorTable(colormap);
    painter.begin(baseImg);
    if(!painter.isActive())
    {
            qDebug() << "\npainter is\'t active\n";
            return;
    }

    painter.fillRect(0, 0, picWidth, picHeight, baseBrush);
//boxRect
    if(!isLegend) legendW = 0;
    pPos0.setX(picWidth - picWidth*0.05 - boxW - legendW);
    pPos0.setY(picHeight/2 - boxH/2);
    pPos1.setX(picWidth - picWidth*0.05 - legendW);
    pPos1.setY(picHeight/2 + boxH/2);
    boxRect.setBottomRight(pPos1);
    boxRect.setTopLeft(pPos0);
    /*
    pPos0.setX(picWidth/2 - boxW/2);
    pPos0.setY(picHeight/2 - boxH/2);
    pPos1.setX(picWidth/2 + boxW/2);
    pPos1.setY(picHeight/2 + boxH/2);
    boxRect.setBottomRight(pPos1);
    boxRect.setTopLeft(pPos0);
    */
//xAxeRect
    pPos0.setX(boxRect.left());
    pPos0.setY((boxRect.bottom()+picHeight)/2.0);
    pPos1.setX(boxRect.right());
    pPos1.setY(picHeight);
    xAxeRect.setBottomRight(pPos1);
    xAxeRect.setTopLeft(pPos0);
/*xLabelRect
    pPos0.setX(picWidth/2 - boxW/2);
    pPos0.setY(picHeight/2 + boxH/2);
    pPos1.setX(picWidth);
    pPos1.setY(picHeight);
    xAxeRect.setBottomRight(pPos1);
    xAxeRect.setTopLeft(pPos0);*/
//yAxeRect
    pPos0.setX(0);
    pPos0.setY(boxRect.top());
    pPos1.setX(boxRect.left()/2.0);
    pPos1.setY(boxRect.bottom());
    yAxeRect.setTopLeft(pPos0);
    yAxeRect.setBottomRight(pPos1);

    //painter.drawEllipse(yAxeRect.center(), 5, 5);

//headRect
    pPos0.setX(0);
    pPos0.setY(0);
    pPos1.setX(picWidth);
    pPos1.setY(boxRect.top());
    headerRect.setTopLeft(pPos0);
    headerRect.setBottomRight(pPos1);


    painter.setPen(axesPen);
    painter.drawRect(boxRect);


    painter.setBrush(pointBrush);

    int pixX, pixY;
    int pixX0, pixY0;
    painter.setFont(QFont("Times", dataNumsFontSize, -1, true));
    QString dataText;
    QRect tempRect;
    QPoint tempPos;
    int uniqNum;
    int j, szj;
    for(i=0; i<num; i++)
    {
        if(i>0)
        {
            tempPos = pPos;
        }
        painter.setPen(pointPen);
        data2pix(x[i], y[i], &pixX, &pixY);
        pPos.setX(pixX);
        pPos.setY(pixY);


        if(i==0)
        {
            tempPos = pPos;
        }
        szj = uniqNames.size();
        uniqNum = -1;
        for(j=0; j<szj; j++)
        {
            if(QString().compare(rowNames.at(i), uniqNames[j])==0)
            {
                uniqNum = j;
                break;
            }
        }
        if(uniqNum==-1)
        {
            uniqNames << QString(rowNames.at(i));
            uniqNum = uniqNames.size()-1;
        }



        //pPos.setX(scX*(x[i]-xs0)+boxRect.topLeft().x());
        //pPos.setY(scY*(ys1-y[i])+boxRect.topLeft().y());


        //pPos1.setX(scX*(x[i]+dx[i]*scLine-xs0)+boxRect.topLeft().x());
        //pPos1.setY(scY*(ys1-y[i]+dy[i]*scLine)+boxRect.topLeft().y());
        //painter.drawEllipse(pPos, pointSize, pointSize);
        //dataText = QString("%1").arg(nums[i]);

        if(isColorList)
        {

            if(uniqNum<colorList.size())
            {
                //qDebug() << QString("nums= %1\tcolor= %2\n").arg(uniqNum).arg(colorList.at(uniqNum));
                pointPen.setColor(colorList.at(uniqNum));
                pointBrush.setColor(colorList.at(uniqNum));
            }
            else
            {
                pointPen.setColor(Qt::black);
                pointBrush.setColor(Qt::black);
            }
            painter.setPen(pointPen);
            painter.setBrush(pointBrush);
            painter.drawEllipse(pPos, pointSize, pointSize);
        }
        else
        {
            tempRect.setRect(pixX-40, pixY-20, 80, 40);
            painter.drawText(tempRect, QString("%1").arg(uniqNames[uniqNum]), xAxeLabelOpt);
        }


        if(isLines)
        {
          //  qDebug() << "\ndrawLine\n";
            painter.setPen(pointPen);
            painter.drawLine(pPos.x(), pPos.y(), tempPos.x(), tempPos.y());
        }

        if(dx!=NULL)
        {
         //   qDebug() << "\ndrawDx\n";
            painter.setPen(trendPen);
            data2pix(x[i]+dx[i]*scLine, y[i], &pixX, &pixY);
            pPos1.setX(pixX);
            pPos1.setY(pixY);
            painter.drawLine(pPos.x(), pPos.y(), pPos1.x(), pPos1.y());
            data2pix(x[i]-dx[i]*scLine, y[i], &pixX, &pixY);
            pPos1.setX(pixX);
            pPos1.setY(pixY);
            painter.drawLine(pPos.x(), pPos.y(), pPos1.x(), pPos1.y());
        }
        if(dy!=NULL)
        {
         //   qDebug() << "\ndrawDy\n";
            painter.setPen(trendPen);
            data2pix(x[i], y[i]+dy[i]*scLine, &pixX, &pixY);
            pPos1.setX(pixX);
            pPos1.setY(pixY);
            painter.drawLine(pPos.x(), pPos.y(), pPos1.x(), pPos1.y());
            data2pix(x[i], y[i]-dy[i]*scLine, &pixX, &pixY);
            pPos1.setX(pixX);
            pPos1.setY(pixY);
            painter.drawLine(pPos.x(), pPos.y(), pPos1.x(), pPos1.y());
        }

    }

//Header

    drawHeadLabel(hLabel);


//Labels
    drawLabels(xLabel, yLabel);


//Axes
    drawAxes(mainStepNumX, mainStepNumY, subStepNumX, subStepNumY);

//Scale
    //drawScale(isAutoScPatt, scPatt);
    if(isLegend) drawLegend(uniqNames, isColorList);


    painter.end();
}


void vfPlot::drawScale(int aScale, double sc)
{
    double sc0;
    int pixX, pixY, pixX0, pixY0;
    painter.setPen(trendPen);
    scaleRect.setRect(0, xAxeRect.center().y(), picWidth/2.0, xAxeRect.height()/2.0);

    if(aScale)
    {
        sc0 = pow(10, (int)log10(fabs(diapDX+diapDY)/4.0));
        qDebug() << QString("diapDX= %1\tdiapDY= %2\n").arg(diapDX).arg(diapDY);
    }
    else
    {
        sc0 = sc;
    }

    qDebug() << QString("sc= %1\n").arg(sc0);
    data2pix(sc0*scLine/2.0, 0, &pixX, &pixY);
    data2pix(0, 0, &pixX0, &pixY0);
    pixX -= pixX0;
    pixY -= pixY0;
    qDebug() << QString("pixX= %1\tpixY= %2\n").arg(pixX).arg(pixY);
    pPos0.setX(scaleRect.center().x()-pixX);// + boxRect.center().x());
    pPos0.setY(scaleRect.center().y());
//    data2pix(sc0*scLine/2.0, 0, &pixX, &pixY);
    pPos1.setX(scaleRect.center().x()+pixX);// - boxRect.center().x());
    pPos1.setY(scaleRect.center().y());
    painter.drawLine(pPos0, pPos1);
    painter.drawLine(pPos0.x(),  pPos0.y()+5, pPos0.x(),  pPos0.y()-5);
    painter.drawLine(pPos1.x(),  pPos1.y()+5, pPos1.x(),  pPos1.y()-5);
    //
    painter.setPen(basePen);
    //
    //data2pix(xs0, ys0, &pixX, &pixY);
    QRect tempRect;
    pPos.setX(pixX);
    pPos.setY(pixY);
    tempRect.setRect(-scaleRect.width()/2.0, 0,scaleRect.width(), scaleRect.height()/2.0);
    transform.translate(scaleRect.center().x(), scaleRect.center().y());
    painter.setTransform(transform);
    painter.drawText(tempRect, QString("%1%2").arg(sc0, 6, 'g').arg(scLabel), xAxeLabelOpt);
    transform.translate(-scaleRect.center().x(), -scaleRect.center().y());
    painter.setTransform(transform);
}

void vfPlot::drawLabels(QString xLab, QString yLab)
{
    QRect textRect(-yAxeRect.height()/2.0, -yAxeRect.width()/2.0, yAxeRect.height(), yAxeRect.width());
    painter.setPen(basePen);
    //infoStr = QString("%1").arg(dims[0][j]+dX);
    painter.setFont(QFont("Times", axeLabelsFontSize, -1, true));
    painter.drawText(xAxeRect, xLab, xAxeLabelOpt);

    transform.translate(yAxeRect.center().x(), yAxeRect.center().y());
    transform.rotate(-90);
    painter.setTransform(transform);
    painter.drawText(textRect, yLab, xAxeLabelOpt);
    transform.rotate(90);
    transform.translate(-yAxeRect.center().x(), -yAxeRect.center().y());
    painter.setTransform(transform);
}

void vfPlot::drawHeadLabel(QString hLab)
{
    painter.setPen(basePen);
    //infoStr = QString("%1").arg(dims[0][j]+dX);
    painter.setFont(QFont("Times", headerFontSize, -1, true));
    painter.drawText(headerRect, hLab, xAxeLabelOpt);
}

void vfPlot::drawAxes(int mSNX, int mSNY, int sSNX, int sSNY)
{
    int i, j;
    int pixX, pixY;
    int pixXs, pixYs;

    painter.setFont(QFont("Times", axeLabelsFontSize, -1, true));

    xTextRect.setRect(-boxRect.width()/(mSNX+1.0)/2.0, 20, boxRect.width()/(mSNX+1.0), 40);
    yTextRect.setRect(-yAxeRect.width()-50, -20, yAxeRect.width()+120, 40);
    painter.setPen(basePen);
    //x0
    data2pix(xs0, ys0, &pixX, &pixY);
    pPos.setX(pixX);
    pPos.setY(pixY);
    transform.translate(pixX, pixY);
    painter.setTransform(transform);
    painter.drawText(xTextRect, QString("%1").arg(xs0), xAxeLabelOpt);
    transform.translate(-pixX, -pixY);
    painter.setTransform(transform);

    //
    double xt, sxt;
    double step = (xs1-xs0)/((mSNX+1)*1.0);
    double sStep;
    sStep = step/(sSNX+1.0);
    qDebug() << QString("step= %1\n").arg(step);
    for(i=0, xt=xs0+step; i<mSNX+1; i++, xt+=step)
    {
        painter.setPen(basePen);
        qDebug() << QString("xt= %1\n").arg(xt);
        //xt = xs0 + step;
        data2pix(xt, ys0, &pixX, &pixY);
        pPos.setX(pixX);
        pPos.setY(pixY);
        transform.translate(pixX, pixY);
        painter.setTransform(transform);
        painter.drawText(xTextRect, QString("%1").arg(xt), xAxeLabelOpt);
        transform.translate(-pixX, -pixY);
        painter.setTransform(transform);

        for(j=0, sxt=sStep; j<sSNX; j++, sxt+=sStep)
        {
            painter.setPen(axesPen);
            data2pix(xt-step+sxt, ys0, &pixXs, &pixYs);
            painter.drawLine(pixXs, pixYs, pixXs, pixYs+10);
        }

        painter.setPen(axesPen);
        painter.drawLine(pixX, pixY, pixX, pixY+20);
    }
    //
/*
    //x1
    painter.setPen(basePen);
    data2pix(xs1, ys0, &pixX, &pixY);
    pPos.setX(pixX);
    pPos.setY(pixY);
    transform.translate(pixX, pixY);
    painter.setTransform(transform);
    painter.drawText(xTextRect, QString("%1").arg(xs1), xAxeLabelOpt);
    transform.translate(-pixX, -pixY);
    painter.setTransform(transform);
    */
    //y0
    painter.setPen(basePen);
    data2pix(xs0, ys0, &pixX, &pixY);
    pPos.setX(pixX);
    pPos.setY(pixY);
    transform.translate(pixX, pixY);
    painter.setTransform(transform);
    painter.drawText(yTextRect, QString("%1").arg(ys0, 5, 'g', 2), yAxeLabelOpt);
    transform.translate(-pixX, -pixY);
    painter.setTransform(transform);

    //
    step = (ys1-ys0)/((mSNY+1)*1.0);
    sStep = step/(sSNY+1.0);
    qDebug() << QString("step= %1\n").arg(step);
    for(i=0, xt=ys0+step; i<mSNY+1; i++, xt+=step)
    {
        painter.setPen(basePen);
        qDebug() << QString("xt= %1\n").arg(xt);
        //xt = xs0 + step;
        data2pix(xs0, xt, &pixX, &pixY);
        pPos.setX(pixX);
        pPos.setY(pixY);
        transform.translate(pixX, pixY);
        painter.setTransform(transform);
        painter.drawText(yTextRect, QString("%1").arg(xt, yTextRect.width()/axeLabelsFontSize, 'g', 2), yAxeLabelOpt);
        transform.translate(-pixX, -pixY);
        painter.setTransform(transform);

        for(j=0, sxt=sStep; j<sSNY; j++, sxt+=sStep)
        {
            painter.setPen(axesPen);
            data2pix(xs0, xt-step+sxt, &pixXs, &pixYs);
            painter.drawLine(pixXs, pixYs, pixXs-10, pixYs);
        }

        painter.setPen(axesPen);
        painter.drawLine(pixX, pixY, pixX-20, pixY);
    }
    //
/*
    //y1
    painter.setPen(basePen);
    data2pix(xs0, ys1, &pixX, &pixY);
    pPos.setX(pixX);
    pPos.setY(pixY);
    transform.translate(pixX, pixY);
    painter.setTransform(transform);
    painter.drawText(yTextRect, QString("%1").arg(ys1), yAxeLabelOpt);
    transform.translate(-pixX, -pixY);
    painter.setTransform(transform);
*/
}

void vfPlot::savePlot(QString fileName, const char* picType)
{

    baseImg->save(fileName, picType);
}

void vfPlot::detLimits(double *x, double *y, int num)
{
    //double minX, maxX, minY, maxY;
    minX = maxX = x[0];
    minY = maxY = y[0];
    for(int i=1; i<num; i++)
    {
        if(x[i]>maxX) maxX = x[i];
        if(x[i]<minX) minX = x[i];
        if(y[i]>maxY) maxY = y[i];
        if(y[i]<minY) minY = y[i];
    }

    qDebug() << QString("minX= %1\tmaxX= %2\nminY= %3\tmaxY= %4\n").arg(minX).arg(maxX).arg(minY).arg(maxY);

    double diapX = fabs(maxX-minX);
    double diapY = fabs(maxY-minY);

    qDebug() << QString("diapX= %1\tdiapY= %2\n").arg(diapX).arg(diapY);

    int bX = (int)diapX*0.1;
    int bY = (int)diapY*0.1;

    qDebug() << QString("bX= %1\tbY= %2\n").arg(bX).arg(bY);

    xs0 = minX - bX;
    xs1 = maxX + bX;
    ys0 = minY - bY;
    ys1 = maxY + bY;

    qDebug() << QString("xs0= %1\txs1= %2\tys0= %3\tys1= %4\n").arg(xs0).arg(xs1).arg(ys0).arg(ys1);

    scX = boxW/fabs(xs1-xs0);
    scY = boxH/fabs(ys1-ys0);
}

void vfPlot::detDLimits(double *dx, double *dy, int num)
{
    //double minDX, maxX, minY, maxY;
    minDX = maxDX = dx[0];
    minDY = maxDY = dy[0];
    meanD = dx[0] + dy[0];
    for(int i=1; i<num; i++)
    {
        if(dx[i]>maxDX) maxDX = dx[i];
        if(dx[i]<minDX) minDX = dx[i];
        if(dy[i]>maxDY) maxDY = dy[i];
        if(dy[i]<minDY) minDY = dy[i];
        meanD += dx[i] + dy[i];
    }
    meanD /= num*2.0;

    qDebug() << QString("minX= %1\tmaxX= %2\nminY= %3\tmaxY= %4\n").arg(minDX).arg(maxDX).arg(minDY).arg(maxDY);

    diapDX = fabs(maxDX-minDX);
    diapDY = fabs(maxDY-minDY);

    qDebug() << QString("diapDX= %1\tdiapDY= %2\n").arg(diapDX).arg(diapDY);
/*
    int bX = (int)diapX*0.1;
    int bY = (int)diapY*0.1;

    qDebug() << QString("bX= %1\tbY= %2\n").arg(bX).arg(bY);

    xs0 = minX - bX;
    xs1 = maxX + bX;
    ys0 = minY - bY;
    ys1 = maxY + bY;

    qDebug() << QString("xs0= %1\txs1= %2\tys0= %3\tys1= %4\n").arg(xs0).arg(xs1).arg(ys0).arg(ys1);

    scX = boxW/fabs(xs1-xs0);
    scY = boxH/fabs(ys1-ys0);
    */
}

double vfPlot::pix2data(int pixX, int pixY, double *valX, double *valY)
{
    *valX = scX*(pixX-boxRect.topLeft().x())+xs0;
    *valY = scY*(boxRect.bottomRight().y() - pixY)+ys0;
}

double vfPlot::data2pix(double valX, double valY, int *pixX, int *pixY)
{
    *pixX = scX*(valX-xs0)+boxRect.topLeft().x();
    *pixY = scY*(ys1-valY)+boxRect.topLeft().y();
}

void vfPlot::detDiaps(double *x, double *y, int num)
{
    minX = maxX = x[0];
    minY = maxY = y[0];
    for(int i=1; i<num; i++)
    {
        if(x[i]>maxX) maxX = x[i];
        if(x[i]<minX) minX = x[i];
        if(y[i]>maxY) maxY = y[i];
        if(y[i]<minY) minY = y[i];
    }

    qDebug() << QString("minX= %1\tmaxX= %2\nminY= %3\tmaxY= %4\n").arg(minX).arg(maxX).arg(minY).arg(maxY);

    diapX = fabs(maxX-minX);
    diapY = fabs(maxY-minY);

    qDebug() << QString("diapX= %1\tdiapY= %2\n").arg(diapX).arg(diapY);
}

void vfPlot::setLabels(QString xLab, QString yLab)
{
    xLabel = xLab;
    yLabel = yLab;
}

void vfPlot::setHeadLabel(QString hLab)
{
    hLabel = hLab;
}
void vfPlot::setScaleLabel(QString scLab)
{
    scLabel = scLab;
}

void vfPlot::setScAuto(int isAuto)
{
    isAutoScPatt = isAuto;
}

void vfPlot::setScPatt(double scP)
{
    scPatt = scP;
}

/*
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
*/

void vfPlot::drawLegend(QStringList namesList, int isColorList)
{
//legendRect
    pPos1.setX(picWidth - picWidth*0.05);
    pPos1.setY(boxRect.bottom());
    legendRect.setBottomRight(pPos1);
    legendRect.setTopLeft(boxRect.topRight());
    int i, szi;
    szi = namesList.size();
    QRect nameRect, labelRect;
    painter.setFont(QFont("Times", legendFontSize, -1, true));


    for(i=0; i<szi; i++)
    {
        labelRect.setCoords(legendRect.left(), legendRect.top() + (legendFontSize*2)*i, legendRect.left() + pointPen.width()*2+15, legendRect.top() - legendFontSize*2 - pointPen.width()*2+15);
        nameRect.setCoords(labelRect.right(), labelRect.top(), legendRect. right(), labelRect.bottom());

        if(isColorList)
        {

            if(i<colorList.size())
            {
                //qDebug() << QString("nums= %1\tcolor= %2\n").arg(uniqNum).arg(colorList.at(uniqNum));
                pointPen.setColor(colorList.at(i));
                pointBrush.setColor(colorList.at(i));
            }
            else
            {
                pointPen.setColor(Qt::black);
                pointBrush.setColor(Qt::black);
            }
            painter.setPen(pointPen);
            painter.setBrush(pointBrush);
            painter.drawEllipse(labelRect.center(), pointSize, pointSize);
        }


        painter.drawText(nameRect, QString("%1").arg(namesList[i]), legendOpt);

    }
}
