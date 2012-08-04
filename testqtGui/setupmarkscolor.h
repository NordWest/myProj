#ifndef SETUPMARKSCOLOR_H
#define SETUPMARKSCOLOR_H

#include <QDialog>
#include <QGraphicsView>
#include <QImage>
#include <QDebug>
#include <QColorDialog>
#include <QGraphicsEllipseItem>

namespace Ui {
    class setupMarksColor;
}

class setupMarksColor : public QDialog
{
    Q_OBJECT

public:
    explicit setupMarksColor(QWidget *parent = 0);
    ~setupMarksColor();

    QGraphicsView *ui_grView;
    QPushButton *ui_ipixBtn;
    QPushButton *ui_catBtn;
    QPushButton *ui_refBtn;
    QPushButton *ui_objBtn;
    QPushButton *ui_bgBtn;

    QColor ipixCol, catCol, refCol, objCol, bgCol;
    QGraphicsEllipseItem *ipixEll, *catEll, *refEll, *objEll;
    QPen ipixPen, catPen, refPen, objPen;

    QBrush bgBrush;
    QBrush imgBrush;

    void initColorNames(QString ipixColStr, QString catColStr, QString refColStr, QString objColStr, QString bgColStr);
    void getColorNames(QString *ipixColStr, QString *catColStr, QString *refColStr, QString *objColStr, QString *bgColStr);

    //void paintEvent(QPaintEvent *event); //переопределение события прорисовки
    //void updateBaseImage();//перерисовка изображения и выброс его на экран

protected:
    void changeEvent(QEvent *e);

private:
    Ui::setupMarksColor *ui;

    QGraphicsScene *scene;
    QGraphicsRectItem *bgRect;

private slots:
    void on_objBtn_clicked();
    void on_refBtn_clicked();
    void on_catBtn_clicked();
    void on_ipixBtn_clicked();
    void on_bgBtn_clicked();
};

#endif // SETUPMARKSCOLOR_H
