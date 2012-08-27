#ifndef CATSVIEW_H
#define CATSVIEW_H

#include <QDialog>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QDialogButtonBox>
#include <QPushButton>

#include "addcatdlg.h"
#include "./../libs/skyarealf.h"

namespace Ui {
class catsView;
}

class catsView : public QDialog
{
    Q_OBJECT
    
public:
    explicit catsView(catList *cl, QWidget *parent = 0);
    ~catsView();

    QHBoxLayout *hLayout;
    QTableWidget *tableView;
    QDialogButtonBox *buttons;
    QPushButton *addCatBtn;
    QPushButton *delCatBtn;

    catList *cat_list;
    QString workPath;

private slots:
    void on_addCatButton_clicked();
    void updateCatsTable();

    void on_delCatButton_clicked();

    void on_editCatBtn_clicked();

private:
    Ui::catsView *ui;
};

#endif // CATSVIEW_H
