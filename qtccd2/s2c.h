#ifndef S2C_H
#define S2C_H
#include <QObject>

//BEGIN description of structures
struct ZONE //Зона для autoranging
{
       short SizeX; //размер зоны в пиксеолях
       short SizeY;
       long Off;  //Смещение в памяти верхнего левого угла зоны в байтах
};

typedef struct __READIMG__
{
     int FlSmear;           //Разрешение коррекции смаза
     int FlStab;            //Разрешение привязки к уровню чёрного
     int FlEnterBack;       //Ввод темнового кадра
     int FlSubBack;         //Разрешение вычитания темнового кадра
     int FlBad;             //Разрешение коррекции дефектов
     int FlAuto;            //Разрешение Autoranging
     int FlAutoMode;        //Реализация режима "Auturanging" по одному либо по нескольким кадрам
     int FlMap;             //Разрешение коррекции неравномерности чувствительности
     int CountInt;          //Текущее значение счётчика осреднений
     int CountIntSav;       //Количество кадров осреднения в режиме цифрового накопления
    int SubOff;             //Смещение для привязки уровня чёрного
    unsigned char *pImg;    //Указатель на буфер для изображения; НЕОБХОДИМО        
    unsigned char *pImgBack; //Указатель на буфер для темнового кадра; НЕОБХОДИМО
    unsigned *pImgInt;     //Указатель на буфер для интегрирования; НЕОБХОДИМО
    struct ZONE Zone;       // Описание зоны для autoranging в памяти
    unsigned short Contrast;//Контраст
    short int Bright;       //Яркость

}READIMG,*PREADIMG;
  

typedef struct __PARAM__
{
  int *pFlFreq;    //Частота считывания выходного регистра; 1- High,0 - Low;
  short int *pX;   //начало выводимой зоны
  short int *pY;
  short int *pSizeX;  //размер выводимой зоны
  short int *pSizeY;
  short int *pBinX ;  //биннинг
  short int *pBinY;
  short int *pCCD_SizeY;
 int *pFlClrFast;       //Разрешение "быстрой очистки" перед экспонированием;
 int *pFlClrSlow;       //Разрешение "медленной очистки" перед экспонированием;
 float *pKp;            //к-т пересчета АЦП в электроны 
 unsigned char *pDev_ID;  //индивидуальный номер камеры
 int *pTrakt;           //тип тракта (8-16 бит)
 int *pTEnter;          //Время ввода 1 кадра в миллисек

// Данные, поставляемые окном Setup
 int *pFrameCount;     //количество кадров в серии
 int *pFlPause;   //1- однокадровый режим, 0 - непрерывный ввод.
 float *pExpos;    //Время экспозиции в сек 
 int *pFlCool;      // Включение микрохолодильника;
 int *pFlFan;       // Включение вентилятора;
 int *pFlCoolStab;  // Разрешение стабилизации температуры;
 int *pFlLight;
 int *pFlSyncEn;    // Разрешение триггерного режима;
 int *pFlSyncFrame;// Вид синхронизации для триггерного режима; Для стандартной кадровой синхронизации = 1
 int *pFlSyncLine; //Не используется
 int *pFlTest;      // Запуск режима теста интерфейса с камерой. При == 1, из камеры читается "пила по горизонтали"
 int *pDigGain; // Параметр цифрового усиления;
 int *pDigOff;      // Параметр цифрового усиления;
 int *pFlMapEn;//Коррекция неравномерности чувствительности
 unsigned long *pMemSize;     //размер физической памяти в байтах
 short int *pTemperC;//Температура стабилизации;
 int *pExposD;          // Параметр задержки срабатывания затвора; 


 int *pFlErr;               //Код  фатальной ошибки (значения - в конце файла)
 int *pCountWar;            //Счетчик warning
 int *pArrWar;      //Массив кодов warning
}PARAM,*PPARAM;

//END description of structures


typedef int (*_SUM_)(int, int, int);
typedef int (*INITDLL)(int, int, unsigned, unsigned);
typedef int (*READ_IMAGE)(PREADIMG, int);
typedef int (*WAIT_READY)(int);
typedef int (*GET_IMAGE)();
typedef int (*STOP_GET_IMAGE)();
typedef int (*MAKECAM)(void);
typedef int (*GETTEMPERATURE)(short int*, short int*);
typedef int (*GETCURCONTEXT)(unsigned char*);
typedef int (*FREEDRV)();
typedef PPARAM (*GET_PARAM_ADDRESS)();


class S2C: public QObject
{
Q_OBJECT
signals:
void isExposed();
void isStarted();
void isStopped();
void isFinished();


public:

PREADIMG pReadImg;
PPARAM   pParam;
bool     CamStarted;
bool stopGettingImage;
int        ccd_ignore;     
int        adapter_type;
unsigned   adapter_slot;
unsigned   video_size;

//////=========
S2C();
void s2cInitDll();
void s2cGetImage(int FrCount);
//////=========

//int InitDll(int flOff, int FlF, unsigned iDevice, unsigned dwSzVideoPoolFrame);
INITDLL InitDll;
//int Read_Image(PREADIMG pImWind, int NFrmC);
READ_IMAGE Read_Image;
//int Wait_Ready(int NFrmC);
WAIT_READY Wait_Ready;
//int Get_Image();
GET_IMAGE Get_Image;
//int Stop_Get_Image();
STOP_GET_IMAGE Stop_Get_Image;
//int MakeCam();
MAKECAM MakeCam;
//int GetTemperature(short int* ptempr1, short int* ptempr2);
GETTEMPERATURE GetTemperature;
//int GetCurContext(unsigned char* Status);
GETCURCONTEXT GetCurContext;
//void FreeDrv();
FREEDRV FreeDrv;
//PPARAM Get_Param_Address();
GET_PARAM_ADDRESS Get_Param_Address;   
_SUM_ _sum_;

};



#endif
