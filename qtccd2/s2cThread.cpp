#include <QtGui>
#include <QLibrary>
#include <QThread>
#include "s2cThread.h"
#include "S2C.h"


s2cThread::s2cThread()
{
 //QThread::QThread();//old constructor
  /////////s1cLowDll assignment
  s2c = new S2C;//BEGIN load s2c dll
  QLibrary S2CLib("s1c3PCI");
  S2CLib.load();
  if (S2CLib.isLoaded()){
        s2c->_sum_ = (_SUM_) S2CLib.resolve("_sum_");
        s2c->InitDll = (INITDLL) S2CLib.resolve("InitDll");
        s2c->Read_Image = (READ_IMAGE) S2CLib.resolve("Read_Image");
        s2c->Wait_Ready = (WAIT_READY) S2CLib.resolve("Wait_Ready");
        s2c->Get_Image = (GET_IMAGE) S2CLib.resolve("Get_Image");
        s2c->Stop_Get_Image = (STOP_GET_IMAGE) S2CLib.resolve("Stop_Get_Image");
        s2c->MakeCam = (MAKECAM) S2CLib.resolve("MakeCam");
        s2c->GetTemperature = (GETTEMPERATURE) S2CLib.resolve("GetTemperature");
        s2c->GetCurContext = (GETCURCONTEXT) S2CLib.resolve("GetCurContext");
        s2c->FreeDrv = (FREEDRV) S2CLib.resolve("FreeDrv");
        s2c->Get_Param_Address = (GET_PARAM_ADDRESS) S2CLib.resolve("Get_Param_Address");
        s2c->CamStarted=false;
        s2c->pReadImg = new READIMG;		
    }//end if
    //END load s2c dll
	stopGettingImage = false;
  ///////////////////////////////////
  
};//END s2cThread constructor

void s2cThread::s2cInitDll()
{
     s2cFUNC = s2c_InitDll;
     start(QThread::NormalPriority);
}//END of s2cInitDll

void s2cThread::s2cGetImage()
{
   if (s2c->CamStarted){
    s2cFUNC = s2c_GetImage;
    start(QThread::NormalPriority);
    }//end CamStarted
};//end of s2cGetImage

void s2cThread::run()
{
    switch(s2cFUNC)
    {
       case s2c_InitDll:
                       if (!s2c->CamStarted)
                          {
                            QWidget *modalWidget = new QWidget;
                            QLabel *modalLabel = new QLabel("camera is being started...");
                            QGridLayout *modalGrid = new QGridLayout(modalWidget);
                            modalGrid->addWidget(modalLabel,0,0);
                            modalWidget->setFixedSize(300,100);
                            modalWidget->setWindowFlags (Qt::WindowTitleHint);
                            modalWidget->setWindowModality(Qt::ApplicationModal);
                            modalWidget->move(500,350);
                            modalWidget->show();
							/*
							QString Msg1;
							QString Msg2;
							QString Msg3;
							QString Msg4;
                                                                           Msg1.setNum(s2c->ccd_ignore);
							Msg2.setNum(s2c->adapter_type);
							Msg3.setNum(s2c->adapter_slot);
							Msg4.setNum(s2c->video_size);
                                                                           QMessageBox::information(0,"debug",Msg1+" "+Msg2+" "+Msg3+" "+Msg4,QMessageBox::Ok,0,0);
							*/
                            if (s2c->InitDll(s2c->ccd_ignore,s2c->adapter_type,s2c->adapter_slot,s2c->video_size)!=1)
                                        {
                                          modalWidget->hide();
                                          QMessageBox::information(0,"debug","Camera has been initialized.",QMessageBox::Ok,0,0);
                                          s2c->CamStarted = true;
                                          s2c->pParam = s2c->Get_Param_Address();
										  s2c->emitStarted();
                                        }//end of InitDll
                           else 
                                      {
                                       modalWidget->hide();
                                       s2c->pParam = s2c->Get_Param_Address();
                                       QString Msg;
                                       Msg.setNum(*s2c->pParam->pFlErr);
                                       QMessageBox::information(0,"debug","Camera has not been initialized. Error: "+Msg,QMessageBox::Ok,0,0);
									   s2c->emitStarted();
                                      }//end else           
                        }//end if s2c->CamStarted
                       break;
       case s2c_GetImage:
                         //QString Msg;
                         //Msg.setNum(*s2c->pParam->pX);
                         //QMessageBox::information(0,"debug",Msg,QMessageBox::Ok,0,0);
						 QMessageBox::information(0,"debug","exposure started",QMessageBox::Ok,0,0);
                         s2c->Get_Image();
                         //for (quint64 i=0;i<100000;i++){for (quint64 j=0;j<10000;j++){;}}
                         do{
                             if (stopGettingImage) 
							         {
									     QMessageBox::information(0,"debug","exposure has been stopped by user.",QMessageBox::Ok,0,0);
							             s2c->Stop_Get_Image();
										 break;
										 //s2c->emitStoped();
									}
									;
                            }while(!s2c->Wait_Ready(0));
						 if (!stopGettingImage) 
                           {						 
                            s2c->Read_Image(s2c->pReadImg, 0);
                            s2c->emitExposed();
						   }
                           else {
						     //s2c->Stop_Get_Image();
							 s2c->emitStopped();
						   }						   
                         break;
    }//END switch
};//END run() method


/*
             unsigned char CurContext;
             if(!s2c->GetCurContext(&CurContext) )
             {
                if(CurContext & 8 || CurContext & 0x10 || CurContext & 4)
                    {
                    //повторить запрос
                    if(!s2c->GetCurContext(&CurContext) )
                             {
                                    if(CurContext & 4)
                                                  QMessageBox::information(0,"debug","Risk T°C!",QMessageBox::Ok,0,0);
                                    else
                                      if(CurContext & 8 || CurContext & 0x10)
                                              QMessageBox::information(0,"debug","Idle!",QMessageBox::Ok,0,0);
                                      else
                                              QMessageBox::information(0,"debug","Camera status: OK",QMessageBox::Ok,0,0);
                             }
                    }//repeat request
             else
                 QMessageBox::information(0,"debug","Camera status: OK",QMessageBox::Ok,0,0);

           }//request
           else
                  QMessageBox::information(0,"debug","Get CurContext error!",QMessageBox::Ok,0,0);
             
          */        
         ///////////////////////////////////////////////////////////////
