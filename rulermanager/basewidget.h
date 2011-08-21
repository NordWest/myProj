#include <QtCore>
#include <QtGui>


class baseWidget : public QWidget
{
 Q_OBJECT

public:
     baseWidget();
    
private slots:
    
	void slotStartButtonClicked();
	
//	void slotPbInfo(int val);

private:
    //BEGIN Widgets
	QPushButton *startButton;
	QProgressBar *pbInfo;
	QProgressBar *totInfo;
	QLabel *lbInfo;
	//END Widgets
	QSettings *settings;
	QStringList parametersList;
	QStringList flist;
	
	QStringList workDirList;
	QStringList resDirList;
	QStringList dirList;
	//thread
};