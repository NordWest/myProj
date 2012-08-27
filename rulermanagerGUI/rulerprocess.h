#include <QThread>
#include <QtGui>
#include <QtCore>

class rulerprocess: public QThread
{
  public:
    
	rulerprocess(QProgressBar *pb, QProgressBar *pbTot, QLabel *lb, QStringList *pl, QObject *parent = 0);  	
		
  protected:
        void run();
        
public slots:
//	void slotSetPbInfo(int val);
        
  private:
	QProgressBar *progressbar;
	QProgressBar *pbTotal;
	QLabel *label;
	QStringList *parsList;
//    QStringList *filesList;
    
    QStringList workDirList;
	QStringList resDirList;
	QStringList dirList;
	
	int isRemove;
	
	signals:
//	void signalSetPbInfo(int val);

};


