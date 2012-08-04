#include <QtCore>
#include <QtGui>
#include <QLibrary>
#include "s2c.h"


S2C::S2C()
{
  QLibrary S2CLib("s1c3PCI");
  S2CLib.load();
  if (S2CLib.isLoaded()){
        _sum_ = (_SUM_) S2CLib.resolve("_sum_");
        InitDll = (INITDLL) S2CLib.resolve("InitDll");
        Read_Image = (READ_IMAGE) S2CLib.resolve("Read_Image");
        Wait_Ready = (WAIT_READY) S2CLib.resolve("Wait_Ready");
        Get_Image = (GET_IMAGE) S2CLib.resolve("Get_Image");
        Stop_Get_Image = (STOP_GET_IMAGE) S2CLib.resolve("Stop_Get_Image");
        MakeCam = (MAKECAM) S2CLib.resolve("MakeCam");
        GetTemperature = (GETTEMPERATURE) S2CLib.resolve("GetTemperature");
        GetCurContext = (GETCURCONTEXT) S2CLib.resolve("GetCurContext");
        FreeDrv = (FREEDRV) S2CLib.resolve("FreeDrv");
        Get_Param_Address = (GET_PARAM_ADDRESS) S2CLib.resolve("Get_Param_Address");
        CamStarted=false;
        pReadImg = new READIMG;		
    }//end if
    //END load s2c dll
	stopGettingImage = false;

};

void S2C::s2cInitDll()
{
  if (InitDll(ccd_ignore,adapter_type,adapter_slot,video_size)!=1)
	{
	  QMessageBox::information(0,"message",tr("Camera has been initialized."),QMessageBox::Ok,0,0);
	  CamStarted = true;
	  pParam = Get_Param_Address();
	  emit isStarted();
	}//end of InitDll
  else 
	  {
	   pParam = Get_Param_Address();
	   QString Msg;
	   Msg.setNum(*pParam->pFlErr);
	   QMessageBox::information(0,"message","Camera has not been initialized. Error: "+Msg,QMessageBox::Ok,0,0);
	  }//end else           

};

void S2C::s2cGetImage(int FrCount)
{
   Get_Image();
   int i = 0;
   do
    {
     QApplication::processEvents(QEventLoop::AllEvents);
	 if (Wait_Ready(0))
		 {
		    Read_Image(pReadImg,0);
			i++;
		    emit isExposed();
			QApplication::processEvents(QEventLoop::AllEvents);
		 }
	 if (stopGettingImage) 
		{
		 QMessageBox::information(0,"debug","exposure has been stopped by user.",QMessageBox::Ok,0,0);
		 Stop_Get_Image();
		 emit isStopped();
		 break;
		}
	}
	while(i<FrCount);
   emit isFinished();
};
