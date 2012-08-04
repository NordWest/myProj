#ifndef IMGLABEL_H
#define IMGLABEL_H
#include <QWidget>
#include "fitsdata.h"

class imageLabel: public QWidget//РІРёРґР¶РµС‚ РґР»СЏ РїСЂРѕСЂРёСЃРѕРІРєРё РёР·РѕР±СЂР°Р¶РµРЅРёСЏ "РЅР° СЌРєСЂР°РЅРµ" РЅР°СЃР»РµРґСѓРµС‚СЃСЏ С‚СѓРїРѕ РѕС‚ QWidget
{

public:
	//data
    QList<fitsdata *> fitsdataList;// СЃРїРёСЃРѕРє СѓРєР°Р·Р°С‚РµР»РµР№ РЅР° РѕР±СЉРµРєС‚С‹ fitsdata, РєРѕС‚РѕСЂС‹Рµ СЃРѕРґРµСЂР¶Р°С‚ РґР°РЅРЅС‹Рµ fits-С„Р°Р№Р»Р° Рё РїР°СЂР°РјРµС‚СЂС‹ РїСЂРѕСЂРёСЃРѕРІРєРё РёР·РѕР±СЂР°Р¶РµРЅРёСЏ
	int frameIndex;// С‚РµРєСѓС‰РёР№ РёРЅРґРµРєСЃ РґР»СЏ РѕР±РѕР·РЅР°С‡РµРЅРёСЏ РџР—РЎ РєР°РґСЂР°, РєРѕС‚РѕСЂС‹Р№ РїСЂРѕСЂРёСЃРѕРІС‹РІР°РµС‚СЃСЏ РІ РґР°РЅРЅС‹Р№ РјРѕРјРµРЅС‚
	int cX,cY,aperture;// cX,cY- РєРѕРѕСЂРґРёРЅР°С‚С‹ С„РѕРєСѓСЃР° РґР»СЏ РїСЂРѕС„РёР»РµР№ Рё РёРЅСЃС‚СЂСѓРјРµРЅС‚РѕРІ, imgW, imgH - СЂР°Р·РјРµСЂС‹ РёР·РѕР±СЂР°Р¶РµРЅРёСЏ СЃ СѓС‡РµС‚РѕРј РјР°СЃС€С‚Р°Р±Р° 
	int maxImgWidth, maxImgHeight;// РјР°РєСЃРёРјР°Р»СЊРЅС‹Рµ СЂР°Р·РјРµСЂС‹ РґР»СЏ "Р±СѓС„РµСЂР° РїСЂРѕСЂРёСЃРѕРІРєРё"
        int focusX, focusY, viewHist, viewAperture, viewPSF, psfmodel, viewFoundStars;
	double meanX,meanY,seX,seY;
	double scale;// РјР°СЃС€С‚Р°Р± РґР»СЏ РїСЂРѕСЂРёСЃРѕРІРєРё РЅР° СЌРєСЂР°РЅРµ
	QImage *baseImg;// "Р±СѓС„РµСЂ РїСЂРѕСЂРёСЃРѕРІРєРё" - СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РѕР±СЉРµРєС‚ QImage. 
	QScrollArea *sa;
	QStatusBar *sb;
	///////////////////////
	imageLabel(int maxIW,int maxIH, QScrollArea *parentScrollArea, QStatusBar *parentStatusBar,  QWidget *parent);//РєРѕРЅСЃС‚СЂСѓРєС‚РѕСЂ РІРёРґР¶РµС‚Р°. maxImgWidth = maxIW, maxImgHeight = maxIH Рё СЃРѕР·РґР°РµС‚СЃСЏ "Р±СѓС„РµСЂ РїСЂРѕСЂРёСЃРѕРІРєРё"
	void paintEvent(QPaintEvent *event); //РїРµСЂРµРѕРїСЂРµРґРµР»РµРЅРёРµ СЃРѕР±С‹С‚РёСЏ РїСЂРѕСЂРёСЃРѕРІРєРё
	void mouseMoveEvent(QMouseEvent * event);
	void mousePressEvent(QMouseEvent * event);
	void updateBaseImage(int xc, int yc, QPainter *painter);//РїРµСЂРµСЂРёСЃРѕРІРєР° РёР·РѕР±СЂР°Р¶РµРЅРёСЏ Рё РІС‹Р±СЂРѕСЃ РµРіРѕ РЅР° СЌРєСЂР°РЅ
	void updateProfileX(QPainter *painter, fitsdata *fitsd, int profileType, QColor color);
	void updateProfileY(QPainter *painter, fitsdata *fitsd, int profileType, QColor color);
	void updateHist(QPainter *painter, fitsdata *fitsd);
	//
};
#endif 
