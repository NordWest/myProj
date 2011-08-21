#ifndef S2CTHREAD_H
#define S2CTHREAD_H
#include "S2C.h"
#include <QThread>
#include <QObject>
#include <QtGui>

#define s2c_InitDll 1
#define s2c_GetImage 2

class s2cThread: public QThread
{

   
public:
      s2cThread();
  void s2cInitDll();
  void s2cGetImage();
      
      QPointer<S2C> s2c;
      int s2cFUNC;
	  bool stopGettingImage;
      
protected:
     void run();

private:
        
    
};

#endif
