#include <QWidget>
#include <QDialog>

class QLabel;
class QPushButton;
class QLineEdit;

class newNameDlg : public QDialog
{
    Q_OBJECT
signals:
	void signalExit();
public:
    newNameDlg(QWidget *parent = 0);
	QWidget *formWidget;

private slots:
//    void on_buttonBox_buttonClicked();
	void on_buttonBox_accepted();
//    void on_inputSpinBox2_valueChanged(int value);

public:
    QLineEdit *ui_newNameLine;
//    QSpinBox *ui_inputSpinBox2;
//    QLabel *ui_outputWidget;
};
