#include <QtGui>
#include "ephthread.h"


ephthread::ephthread(QObject *parent)
        : QThread(parent)
    {
        //mImgf = imgf;
        //pb_ = pb;
    mainW = (QMainWindow*) parent;
    }

ephthread::~ephthread()
    {
        
    }


 void ephthread::run()
 {
     /*
   //QMessageBox::information(0,"debug","thread started",QMessageBox::Ok,0,0);
   //CCDFrame frame(mImgf,mResult,pb_);
   //connect(parent(), SIGNAL(signalTest()), &frame, SLOT(testing()),Qt::QueuedConnection);
   //connect(parent(), SIGNAL(signalLoadFITS(QString)), &frame, SLOT(slotLoadFITS(QString)),Qt::QueuedConnection);
   connect(parent(), SIGNAL(signalBC(double, double, double, int)), &frame, SLOT(slotBC(double, double, double, int)),Qt::QueuedConnection);
   connect(parent(), SIGNAL(signalCenterOfMass(int, int, int)), &frame, SLOT(slotCenterOfMass(int, int, int)),Qt::QueuedConnection);
   connect(parent(), SIGNAL(signalApproxImg(int, int, int, double, int)), &frame, SLOT(slotApproxImg(int, int, int, double, int)),Qt::QueuedConnection);
   connect(parent(), SIGNAL(signalMeasureMarkedObjects(int, double, int, int, int)), &frame, SLOT(slotMeasureMarkedObjects(int, double, int, int, int)),Qt::QueuedConnection);
   connect(parent(), SIGNAL(signalLeftFrame()), &frame, SLOT(slotLeftFrame()),Qt::QueuedConnection);
   connect(parent(), SIGNAL(signalRightFrame()), &frame, SLOT(slotRightFrame()),Qt::QueuedConnection);
   connect(parent(), SIGNAL(signalDeleteFrame()), &frame, SLOT(slotDeleteFrame()),Qt::QueuedConnection);
   connect(parent(), SIGNAL(signalAddMFrame()), &frame, SLOT(slotAddMFrame()),Qt::QueuedConnection);
   connect(parent(), SIGNAL(signalDelMFrame(int)), &frame, SLOT(slotDelMFrame(int)),Qt::QueuedConnection);
   connect(parent(), SIGNAL(signalComboFramesChanged(int)), &frame, SLOT(slotSetMFrame(int)),Qt::QueuedConnection);
   //connect(parent(), SIGNAL(signalStartProc1(int)), &frame, SLOT(slotStartProc1(int)),Qt::QueuedConnection);
   connect(parent(), SIGNAL(signalSubPsf(int, int, int, int)), &frame, SLOT(slotSubtractPSF(int, int, int, int)),Qt::QueuedConnection);
   connect(parent(), SIGNAL(signalExecute(QString)), &frame, SLOT(slotExecute(QString)),Qt::QueuedConnection);
   connect(parent(), SIGNAL(signalGetMarks(QString,int)), &frame, SLOT(slotGetMarks(QString,int)),Qt::QueuedConnection);
   connect(parent(), SIGNAL(signalUpdateOC(double,double)), &frame, SLOT(slotUpdateOC(double,double)),Qt::QueuedConnection);
   connect(parent(), SIGNAL(signalSaveAsFIT(QString)), &frame, SLOT(slotSaveAsFIT(QString)),Qt::QueuedConnection);
   /////////////////////////////////////////////////////////////////////////////////////
   connect(&frame, SIGNAL(signalMeasured()), parent(), SLOT(slotMeasured()),Qt::QueuedConnection);
   connect(&frame, SIGNAL(signalImageProcessed(int)), parent(), SLOT(slotShowImg(int)),Qt::QueuedConnection);
   connect(&frame, SIGNAL(signalPSF()), parent(), SLOT(slotPSF()),Qt::QueuedConnection);
   connect(&frame, SIGNAL(signalDone()), parent(), SLOT(slotDone()),Qt::QueuedConnection);
   connect(&frame, SIGNAL(signalMFrameAdded()), parent(), SLOT(slotMFrameAdded()),Qt::QueuedConnection);
   connect(&frame, SIGNAL(signalProcCompleted()), parent(), SLOT(slotProcCompleted()),Qt::QueuedConnection);
   connect(&frame, SIGNAL(signalTangPosCalculated()), parent(), SLOT(slotTangPosCalculated()),Qt::QueuedConnection);
   connect(&frame, SIGNAL(signalProgressDialogShow()), parent(), SLOT(slotProgressDialogShow()),Qt::QueuedConnection);
   connect(&frame, SIGNAL(signalProgressDialogHide()), parent(), SLOT(slotProgressDialogHide()),Qt::QueuedConnection);*/

     connect(&secTimer,  SIGNAL(timeout()), this, SLOT(updateTable()));

   exec();
 };
  
 
 void ephthread::initEphThread(QVector<double> *vResult)
 {
   mResult = vResult;
   start(NormalPriority);
 };
 
 void ephthread::updateTable()
 {
     secTimer.start(1000);
 }
 
