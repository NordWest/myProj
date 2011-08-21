#include <QtGui>
#include <QThread>
#include "mainwindow.h"

class ephthread: public QThread
{
  public:
   // PIMGF mImgf;
        //QProgressBar *pb_;
    QTimer secTimer;
    QMainWindow *mainW;
	QVector<double> *mResult;
    ////////////
        ephthread(QObject *parent = 0);
    ~ephthread();
		
  public:
     void initEphThread(QVector<double> *vResult);
     void updateTable();
  
  protected:
        void run();
        
  private:
  
  
             
};
