#include "setupmarkscolor.h"
#include "ui_setupmarkscolor.h"

setupMarksColor::setupMarksColor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::setupMarksColor)
{
    ui->setupUi(this);


    ui_grView = qFindChild<QGraphicsView *>(this, "grView");
    ui_ipixBtn = qFindChild<QPushButton *>(this, "ipixBtn");
    ui_catBtn = qFindChild<QPushButton *>(this, "catBtn");
    ui_refBtn = qFindChild<QPushButton *>(this, "refBtn");
    ui_objBtn = qFindChild<QPushButton *>(this, "objBtn");
    ui_bgBtn = qFindChild<QPushButton *>(this, "bgBtn");


    bgCol = Qt::black;
    ipixCol = Qt::yellow;
    catCol = Qt::blue;
    refCol = Qt::red;
    objCol = Qt::green;


    //bgBrush.setColor(bgCol);

    ipixPen.setColor(ipixCol);
    catPen.setColor(catCol);
    refPen.setColor(refCol);
    objPen.setColor(objCol);

    scene = new QGraphicsScene;
    ui->grView->setRenderHints(QPainter::Antialiasing);
    ui_grView->setScene(scene);
    ui_grView->show();

    scene->setBackgroundBrush(bgCol);

    QRect scR = ui_grView->rect();

    int x0, dx;

    qDebug() << QString("width= %1\n").arg(scR.width());
    dx = scR.width()/4.0;
    x0 = -1.5*dx;

    ipixEll = new QGraphicsEllipseItem(x0, 0, 10, 10);
    scene->addItem(ipixEll);

    x0 += dx;
    catEll = new QGraphicsEllipseItem(x0, 0, 10, 10);
    scene->addItem(catEll);
    x0 += dx;
    refEll = new QGraphicsEllipseItem(x0, 0, 10, 10);
    scene->addItem(refEll);
    x0 += dx;
    objEll = new QGraphicsEllipseItem(x0, 0, 10, 10);
    scene->addItem(objEll);

    ipixEll->setPen(ipixPen);
    catEll->setPen(catPen);
    refEll->setPen(refPen);
    objEll->setPen(objPen);

}

setupMarksColor::~setupMarksColor()
{
    delete ui;
}

void setupMarksColor::changeEvent(QEvent *e)
{
    //qDebug() << "changeEvent\n";
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void setupMarksColor::initColorNames(QString ipixColStr, QString catColStr, QString refColStr, QString objColStr, QString bgColStr)
{
    bgCol.setNamedColor(bgColStr);
    ipixCol.setNamedColor(ipixColStr);
    catCol.setNamedColor(catColStr);
    refCol.setNamedColor(refColStr);
    objCol.setNamedColor(objColStr);

    ipixPen.setColor(ipixCol);
    catPen.setColor(catCol);
    refPen.setColor(refCol);
    objPen.setColor(objCol);

    scene->setBackgroundBrush(bgCol);
    ipixEll->setPen(ipixPen);
    catEll->setPen(catPen);
    refEll->setPen(refPen);
    objEll->setPen(objPen);
}

void setupMarksColor::getColorNames(QString *ipixColStr, QString *catColStr, QString *refColStr, QString *objColStr, QString *bgColStr)
{
    *bgColStr = bgCol.name();
    *ipixColStr = ipixCol.name();
    *catColStr = catCol.name();
    *refColStr = refCol.name();
    *objColStr = objCol.name();
}

void setupMarksColor::on_bgBtn_clicked()
{
    QColorDialog bgColDlg;//(bgCol);
    bgCol = bgColDlg.getColor(bgCol);
    scene->setBackgroundBrush(bgCol);
}

void setupMarksColor::on_ipixBtn_clicked()
{
    QColorDialog bgColDlg;
    ipixCol = bgColDlg.getColor(ipixCol);
    ipixPen.setColor(ipixCol);
    ipixEll->setPen(ipixPen);
}

void setupMarksColor::on_catBtn_clicked()
{
    QColorDialog bgColDlg;
    catCol = bgColDlg.getColor(catCol);
    catPen.setColor(catCol);
    catEll->setPen(catPen);
}

void setupMarksColor::on_refBtn_clicked()
{
    QColorDialog bgColDlg;
    refCol = bgColDlg.getColor(refCol);
    refPen.setColor(refCol);
    refEll->setPen(refPen);
}

void setupMarksColor::on_objBtn_clicked()
{
    QColorDialog bgColDlg;
    objCol = bgColDlg.getColor(objCol);
    objPen.setColor(objCol);
    objEll->setPen(objPen);
}
