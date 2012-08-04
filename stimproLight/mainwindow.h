#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QtCore>
#include <QtGui>
#include <QMainWindow>

#include "imagelabel.h"

class MainWindow : public QMainWindow//РєР»Р°СЃСЃ РіР»Р°РІРЅРѕРіРѕ РѕРєРЅР° РїСЂРѕРіСЂР°РјРјС‹, РЅР°СЃР»РµРґСѓРµРјС‹Р№ РѕС‚ QMainWindow
{
 Q_OBJECT
 
signals:

    
private slots:
	//BEGIN menu slots
	//menu file
	void slotOpenFItsFile();
	void slotExit();
	//menu view
	void slotBackLeft();
	void slotBackRight();
	void slotSignalLeft();
	void slotSignalRight();
	void slotSetDefault();
	void slotSetFullDiapason();
	void slotViewHeader();
	void slotViewFocusX();
	void slotViewFocusY();
	void slotViewHist();
	void slotIncStepAct();
	void slotDecStepAct();
        void slotViewFoundStars();
	//menu frame
	void slotIncrease();
	void slotDecrease();
	void slotNext();
	void slotPreviouse();
	void slotDelete();
	void slotScrUpdate();
	//menu tools
	void slotCenterMass();
	void slotViewApertures();
	void slotIncAperture();
	void slotDecAperture();
	void slotPSF();
	void slotViewPSF();
	void slotNextModel();//СЃР»РµРґСѓСЋС‰Р°СЏ РјРѕРґРµР»СЊ
	void slotPrevModel();//РїСЂРµРґС‹РґСѓС‰Р°СЏ РјРѕРґРµР»СЊ
	void slotIncOrder();//СѓРІРµР»РёС‡РёС‚СЊ РїРѕРєР°Р·Р°С‚РµР»СЊ СЃС‚РµРїРµРЅРё РјРѕРґРµР»Рё PSF
	void slotDecOrder();//СѓРјРµРЅСЊС€РёС‚СЊ РїРѕРєР°Р·Р°С‚РµР»СЊ СЃС‚РµРїРµРЅРё РјРѕРґРµР»Рё PSF
	void slotOrderToUnit();
        void slotFindStars();
        void slotMatchStars();
        void slotCorrAct();
        void slotSldAct();
	//END menu slots
	void slotUpdateScrollBars();
	
	
public:
	MainWindow();
	void setMenu();
	void setWidgets();
	void setScrollBars();
	/////////////////////////
	
	
	
private:
	//BEGIN menu
	//file
	QMenu *fileMenu;
	QAction *openFitsAct;//РѕС‚РєСЂС‹С‚РёРµ fits С„Р°Р№Р»Р°
	QAction *exitAct;//РІС‹С…РѕРґ РёР· РїСЂРѕРіСЂР°РјРјС‹ (Р·Р°РєСЂС‹С‚РёРµ РїСЂРёР»РѕР¶РµРЅРёСЏ)
	//view
	QMenu *viewMenu;
	QAction *incAct;//СѓРІРµР»РёС‡РµРЅРёРµ РёР·РѕР±СЂР°Р¶РµРЅРёСЏ
	QAction *decAct;//СѓРјРµРЅСЊС€РµРЅРёРµ РёР·РѕР±СЂР°Р¶РµРЅРёСЏ
	/////////////////// СЃР»РµРґСѓСЋС‰РёРµ 6 "РґРµР№СЃС‚РІРёР№" (action) СѓРїСЂР°РІР»СЏСЋС‚ РїР°СЂР°РјРµС‚СЂР°РјРё "С‰РµР»Рё РїСЂРѕСЂРёСЃРѕРІРєРё"
	QAction *backLeftAct;//1. СЃРјРµС‰РµРЅРёРµ Р»РµРІРѕР№ РіСЂР°РЅРёС†С‹ "С‰РµР»Рё РїСЂРѕСЂРёСЃРѕРІРєРё" РІР»РµРІРѕ
	QAction *backRightAct;//2. СЃРјРµС‰РµРЅРёРµ Р»РµРІРѕР№ РіСЂР°РЅРёС†С‹ "С‰РµР»Рё РїСЂРѕСЂРёСЃРѕРІРєРё" РІРїСЂР°РІРѕ
	QAction *signalLeftAct;//3. СЃРјРµС‰РµРЅРёРµ РїСЂР°РІРѕР№ РіСЂР°РЅРёС†С‹ "С‰РµР»Рё РїСЂРѕСЂРёСЃРѕРІРєРё" РІР»РµРІРѕ
	QAction *signalRightAct;//4. СЃРјРµС‰РµРЅРёРµ РїСЂР°РІРѕР№ РіСЂР°РЅРёС†С‹ "С‰РµР»Рё РїСЂРѕСЂРёСЃРѕРІРєРё" РІРїСЂР°РІРѕ
	QAction *setDefaultAct;//5. РІРѕР·РІСЂР°С‰РµРЅРёРµ Рє РїР°СЂР°РјРµС‚СЂР°Рј РїРѕ-СѓРјРѕР»С‡Р°РЅРёСЋ, РІС‹С‡РёСЃР»РµРЅРЅС‹Рј РїСЂРё РѕС‚РєСЂС‹С‚РёРё fits С„Р°Р№Р»Р°
	QAction *setFullDiapasonAct;//6. СѓСЃС‚Р°РЅРѕРІРєР° "С‰РµР»Рё РїСЂРѕСЂРёСЃРѕРІРєРё" СЂР°РІРЅРѕР№ РІСЃРµРјСѓ РґРёР°РїР°Р·РѕРЅСѓ Р·РЅР°С‡РµРЅРёР№ РЅР° РїРёРєСЃРµР»СЏС…
	//
	QAction *viewHeaderAct;//7. Р’С‹РІРѕРґ Р·Р°РіРѕР»РѕРІРєР° fits-С„Р°Р№Р»Р° РІ РєРѕРЅСЃРѕР»СЊ
	QAction *viewFocusXAct;//8. РІРєР»СЋС‡РёС‚СЊ/РІС‹РєР»СЋС‡РёС‚СЊ С‚РѕС‡РєСѓ С„РѕРєСѓСЃР°
	QAction *viewFocusYAct;//9. РІРєР»СЋС‡РёС‚СЊ/РІС‹РєР»СЋС‡РёС‚СЊ С‚РѕС‡РєСѓ С„РѕРєСѓСЃР°
	QAction *viewHistAct;//10. РІРєР»СЋС‡РёС‚СЊ/РІС‹РєР»СЋС‡РёС‚СЊ РіРёСЃС‚РѕРіСЂР°РјРјСѓ
	QAction *incStepAct;//11. СѓРІРµР»РёС‡РёС‚СЊ С€Р°Рі РёР·РјРµРЅРµРЅРёСЏ С‰РµР»Рё РїСЂРѕСЂРёСЃРѕРІРєРё
	QAction *decStepAct;//12. СѓРјРµРЅСЊС€РёС‚СЊ С€Р°Рі РёР·РјРµРЅРµРЅРёСЏ С‰РµР»Рё РїСЂРѕСЂРёСЃРѕРІРєРё
        QAction *viewFoundStarsAct;
	//frame
	QMenu *frameMenu;
	QAction *nextAct;//РїРµСЂРµС…РѕРґ Рє СЃР»РµРґСѓСЋС‰РµРјСѓ РєР°РґСЂСѓ
	QAction *prevAct;//РїРµСЂРµС…РѕРґ Рє РїСЂРµРґС‹РґСѓС‰РµРјСѓ РєР°РґСЂСѓ
	QAction *delAct;//СѓРґР°Р»РµРЅРёРµ С‚РµРєСѓС‰РµРіРѕ РєР°РґСЂР°
	//menu tool
	QMenu *toolsMenu;
	QAction *cmAct;//РІС‹С‡РёСЃР»РµРЅРёРµ С†РµРЅС‚СЂР° РјР°СЃСЃ
	QAction *apertureAct;//РІРєР»СЋС‡РёС‚СЊ/РІС‹РєР»СЋС‡РёС‚СЊ Р°РїРµСЂС‚СѓСЂСѓ
	QAction *apertureIncAct;//СѓРІРµР»РёС‡РёС‚СЊ Р°РїРµСЂС‚СѓСЂСѓ
	QAction *apertureDecAct;//СѓРјРµРЅСЊС€РёС‚СЊ Р°РїРµСЂС‚СѓСЂСѓ
	QAction *psfAct;//РІРїРёСЃР°С‚СЊ РїСЂРѕС„РёР»СЊ PSF
	QAction *viewPSFAct;//РїРѕРєР°Р·Р°С‚СЊ/СЃРєСЂС‹С‚СЊ РїСЂРѕС„РёР»СЊ PSF
	QAction *nextModelAct;//СЃР»РµРґСѓСЋС‰Р°СЏ РјРѕРґРµР»СЊ
	QAction *prevModelAct;//РїСЂРµРґС‹РґСѓС‰Р°СЏ РјРѕРґРµР»СЊ
	QAction *incOrderAct;//СѓРІРµР»РёС‡РёС‚СЊ РїРѕРєР°Р·Р°С‚РµР»СЊ СЃС‚РµРїРµРЅРё РјРѕРґРµР»Рё PSF
	QAction *decOrderAct;//СѓРјРµРЅСЊС€РёС‚СЊ РїРѕРєР°Р·Р°С‚РµР»СЊ СЃС‚РµРїРµРЅРё РјРѕРґРµР»Рё PSF
	QAction *orderToUnitAct;//РїСЂРёСЂР°РІРЅСЏС‚СЊ РїРѕРєР°Р·Р°С‚РµР»СЊ СЃС‚РµРїРµРЅРё РјРѕРґРµР»Рё PSF Рє РµРґРёРЅРёС†Рµ
        QAction *findStarsAct;
        QAction *matchStarsAct;
        QAction *corrAct;
        QAction *sldAct;
	//END menu 
	//BEGIN widgets
	QScrollArea *scrollArea;//РѕР±Р»Р°СЃС‚СЊ СЃ РїРѕР»РѕСЃР°РјРё РїСЂРѕРєСЂСѓС‚РєРё, РєРѕРЅС‚РµР№РЅРµСЂ РґР»СЏ imageLabel 
	imageLabel *imgLabel;//РІРёРґР¶РµС‚ РґР»СЏ РїСЂРѕСЂРёСЃРѕРІРєРё РёР·РѕР±СЂР°Р¶РµРЅРёСЏ "РЅР° СЌРєСЂР°РЅРµ" 
	//END widgets
	//BEGIN members
	QSettings *stimproSettings;
	QString currentFolder;
	int hscrbValue;
	int vscrbValue; 
	//END members
};
#endif
