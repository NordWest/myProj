#ifndef MB_H
#define MB_H


extern "C"
{



bool ssleg(const double* a, double *const x, const double* b, int size);
/*
СЂРµС€РµРЅРёРµ СЃРёСЃС‚РµРјС‹ Р»РёРЅРµР№РЅС‹С… Р°Р»РіРµР±СЂР°РёС‡РµСЃРєРёС… СѓСЂР°РІРЅРµРЅРёР№ РјРµС‚РѕРґРѕРј Р“Р°СѓСЃСЃР°
a - РјР°СЃСЃРёРІ РєРѕСЌС„С„РёС†РёРµРЅС‚РѕРІ РїРµСЂРµРґ РЅРµРёР·РІРµСЃС‚РЅС‹РјРё,
b - РєРѕСЌС„С„РёС†РёРµРЅС‚С‹ РїСЂР°РІС‹С… С‡Р°СЃС‚РµР№,
x - РјР°СЃСЃРёРІ, РІ РєРѕС‚РѕСЂС‹Р№ РїРѕРјРµС‰Р°СЋС‚СЃСЏ СЂРµР·СѓР»СЊС‚Р°С‚С‹,
size - С‡РёСЃР»Рѕ СѓСЂР°РІРЅРµРЅРёР№ Рё РЅРµРёР·РІРµСЃС‚РЅС‹С….
С„СѓРЅРєС†РёСЏ РІРѕР·РІСЂР°С‰Р°РµС‚ true, РµСЃР»Рё СЂРµС€РµРЅРёРµ РІС‹РїРѕР»РЅРµРЅРѕ СѓСЃРїРµС€РЅРѕ, Рё - false РІ РїСЂРѕС‚РёРІРЅРѕРј СЃР»СѓС‡Р°Рµ.
*/


bool invgj(double *const inv, const double* a, int size);
        /*
        РѕР±СЂР°С‰РµРЅРёРµ РјР°С‚СЂРёС†С‹ РјРµС‚РѕРґРѕРј Р“Р°СѓСЃСЃР°-Р–РѕСЂРґР°РЅР°
        a - СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РјР°СЃСЃРёРІ СЌР»РµРјРµРЅС‚РѕРІ РјР°С‚СЂРёС†С‹,
        inv - СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РјР°СЃСЃРёРІ, РІ РєРѕС‚РѕСЂС‹Р№ РґРѕР»Р¶РµРЅ Р±С‹С‚СЊ Р·Р°РїРёСЃР°РЅ СЂРµР·СѓР»СЊС‚Р°С‚
        size - СЂР°Р·РјРµСЂ РјР°С‚СЂС†С‹.
        С„СѓРЅРєС†РёСЏ РІРѕР·РІСЂР°С‰Р°РµС‚ true, РµСЃР»Рё РѕР±СЂР°С‰РµРЅРёРµ РІС‹РїРѕР»РЅРµРЅРѕ СѓСЃРїРµС€РЅРѕ, Рё - false РІ РїСЂРѕС‚РёРІРЅРѕРј СЃР»СѓС‡Р°Рµ.
        */


bool mvprod(double *const r, const double* m, const double* v, int w, int h);
/*
СѓРјРЅРѕР¶РµРЅРёРµ РјР°С‚СЂРёС†С‹  m РЅР° РІРµРєС‚РѕСЂ v.
r - СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РјР°СЃСЃРёРІ СЌР»РµРјРµРЅС‚РѕРІ СЂРµР·СѓР»СЊС‚РёСЂСѓСЋС‰РµР№ РјР°С‚СЂРёС†С‹,
m - СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РјР°СЃСЃРёРІ СЌР»РµРјРµРЅС‚РѕРІ РёСЃС…РѕРґРЅРѕР№ РјР°С‚СЂРёС†С‹
v - СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РјР°СЃСЃРёРІ СЌР»РµРјРµРЅС‚РѕРІ РІРµРєС‚РѕСЂР° СЂР°Р·РјРµСЂРѕРј w
w,h - СЂР°Р·РјРµСЂС‹ РјР°С‚СЂС†С‹ РїРѕ РіРѕСЂРёР·РѕРЅС‚Р°Р»Рё Рё РІРµСЂС‚РёРєР°Р»Рё.
С„СѓРЅРєС†РёСЏ РІРѕР·РІСЂР°С‰Р°РµС‚ true
*/


bool mmprod(double *const r, const double* ml, const double* mr, int wl, int hl, int wr, int hr);
/*
СѓРјРЅРѕР¶РµРЅРёРµ РјР°С‚СЂРёС†С‹  ml(СЂР°Р·РјРµСЂС‹ РјР°С‚СЂРёС†С‹ wl X hl) РЅР° РјР°С‚СЂРёС†Сѓ mr (СЂР°Р·РјРµСЂС‹ РјР°С‚СЂРёС†С‹ wr X hr).
r - СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РјР°СЃСЃРёРІ СЌР»РµРјРµРЅС‚РѕРІ СЂРµР·СѓР»СЊС‚РёСЂСѓСЋС‰РµР№ РјР°С‚СЂРёС†С‹ СЂР°Р·РјРµСЂРѕРј wr X hl,
ml - СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РјР°СЃСЃРёРІ СЌР»РµРјРµРЅС‚РѕРІ РјР°С‚СЂРёС†С‹, СЃС‚РѕСЏС‰РµР№ СЃР»РµРІР°,
mr - СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РјР°СЃСЃРёРІ СЌР»РµРјРµРЅС‚РѕРІ РјР°С‚СЂРёС†С‹, СЃС‚РѕСЏС‰РµР№ СЃРїСЂР°РІР°,
С„СѓРЅРєС†РёСЏ РІРѕР·РІСЂР°С‰Р°РµС‚ true РїСЂРё СѓСЃРїРµС€РЅРѕРј РІС‹РїРѕР»РЅРµРЅРёРё РѕРїРµСЂР°С†РёРё Рё false РІ СЃР»СѓС‡Р°Рµ wl!=hr, РєРѕРіРґР° СѓРјРЅРѕР¶РµРЅРёРµ РЅРµРІРѕР·РјРѕР¶РЅРѕ
*/


void transpose(double *const r, const double* m, int w, int h);
/*
С‚СЂР°РЅСЃРїРѕРЅРёСЂРѕРІР°РЅРёРµ РјР°С‚СЂРёС†С‹ m (СЂР°Р·РјРµСЂС‹ РјР°С‚СЂРёС†С‹ w X h).
r - СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РјР°СЃСЃРёРІ СЌР»РµРјРµРЅС‚РѕРІ СЂРµР·СѓР»СЊС‚РёСЂСѓСЋС‰РµР№ РјР°С‚СЂРёС†С‹ СЂР°Р·РјРµСЂРѕРј h X w,
m - СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РјР°СЃСЃРёРІ СЌР»РµРјРµРЅС‚РѕРІ РёСЃС…РѕРґРЅРѕР№ РјР°С‚СЂРёС†С‹,
*/


bool mdiagprod(double *const r, const double* m, const double* d, int w, int h);
/*
СѓРјРЅРѕР¶РµРЅРёРµ РјР°С‚СЂРёС†С‹  m РЅР° РґРёР°РіРѕРЅР°Р»СЊРЅСѓСЋ РјР°С‚СЂРёС†Сѓ d.
r - СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РјР°СЃСЃРёРІ СЌР»РµРјРµРЅС‚РѕРІ СЂРµР·СѓР»СЊС‚РёСЂСѓСЋС‰РµР№ РјР°С‚СЂРёС†С‹,
m - СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РјР°СЃСЃРёРІ СЌР»РµРјРµРЅС‚РѕРІ РёСЃС…РѕРґРЅРѕР№ РјР°С‚СЂРёС†С‹
d - СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РѕРґРЅРѕРјРµСЂРЅС‹Р№ РјР°СЃСЃРёРІ РґРёР°РіРѕРЅР°Р»СЊРЅС‹С… СЌР»РµРјРµРЅС‚РѕРІ РјР°С‚СЂРёС†С‹ d,
w,h - СЂР°Р·РјРµСЂС‹ РјР°С‚СЂС†С‹ m РїРѕ РіРѕСЂРёР·РѕРЅС‚Р°Р»Рё Рё РІРµСЂС‚РёРєР°Р»Рё.
С„СѓРЅРєС†РёСЏ РІРѕР·РІСЂР°С‰Р°РµС‚ true
*/



void slsm(int w, int h, double *const z, const double* c, const double* L, const double* W = 0);
/*
РњРµС‚РѕРґ РЅР°РёРјРµРЅСЊС€РёС… РєРІР°РґСЂР°С‚РѕРІ. Р РµС€Р°РµРј СЃРёСЃС‚РµРјСѓ РІРёРґР° L = СЃ*z
Р­С‚Р° С„СѓРЅРєС†РёСЏ РїРѕР·РІРѕР»СЏРµС‚ Р±С‹СЃС‚СЂРѕ РІС‹С‡РёСЃР»РёС‚СЊ РІРµРєС‚РѕСЂ РЅРµРёР·РІРµСЃС‚РЅС‹С… (z), С‡С‚РѕР±С‹ СЃСЌРєРѕРЅРѕРјРёС‚СЊ РІСЂРµРјСЏ Рё СЂРµСЃСѓСЂСЃС‹.
РљРѕРІР°СЂРёР°С†РёРѕРЅРЅР°СЏ РјР°С‚СЂРёС†Р°, РѕС€РёР±РєР° РµРґРёРЅРёС†С‹ РІРµСЃР° Рё РѕС€РёР±РєРё РЅРµРёР·РІРµСЃС‚РЅС‹С… РЅРµ РІС‹С‡РёСЃР»СЏСЋС‚СЃСЏ,
L - СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РјР°СЃСЃРёРІ СЌР»РµРјРµРЅС‚РѕРІ, РїСЂРµРґСЃС‚Р°РІР»СЏСЋС‰РёР№ РІРµРєС‚РѕСЂ РґР°РЅРЅС‹С….
W - СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РјР°СЃСЃРёРІ СЌР»РµРјРµРЅС‚РѕРІ, РїСЂРµРґСЃС‚Р°РІР»СЏСЋС‰РёР№ РІРµСЃРѕРІСѓСЋ РґРёР°РіРѕРЅР°Р»СЊРЅСѓСЋ РјР°С‚СЂРёС†Сѓ.
Р”Р»СЏ СЌРєРѕРЅРѕРјРёРё РІСЂРµРјРµРЅРё Рё СЂРµСЃСѓСЂСЃРѕРІ РґРёР°РіРѕРЅР°Р»СЊРЅР°СЏ РјР°С‚СЂРёС†Р° С…СЂР°РЅРёС‚СЃСЏ РєР°Рє РѕРґРЅРѕРјРµСЂРЅС‹Р№ РјР°СЃСЃРёРІ РґРёР°РіРѕРЅР°Р»СЊРЅС‹С… СЌР»РµРјРµРЅС‚РѕРІ.
РџРѕ СѓРјРѕР»С‡Р°РЅРёСЋ СЌС‚РѕС‚ СѓРєР°Р·Р°С‚РµР»СЊ СЂР°РІРµРЅ 0 (С‚Рѕ РµСЃС‚СЊ РµСЃР»Рё РІРµСЃР° РЅРµ РЅСѓР¶РЅС‹)
w,h - СЂР°Р·РјРµСЂС‹ РјР°С‚СЂС†С‹ РїР»Р°РЅР° (СЃ) РїРѕ РіРѕСЂРёР·РѕРЅС‚Р°Р»Рё Рё РІРµСЂС‚РёРєР°Р»Рё. РћРґРЅРѕРІСЂРµРјРµРЅРЅРѕ w - С‡РёСЃР»Рѕ РЅРµРёР·РІРµСЃС‚РЅС‹С…, h - С‡РёСЃР»Рѕ СѓСЃР»РѕРІРЅС‹С… СѓСЂР°РІРЅРµРЅРёР№
z - РІРµРєС‚РѕСЂ РѕС†РµРЅРѕРє РїР°СЂР°РјРµС‚СЂРѕРІ РјРѕРґРµР»Рё (СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РѕРґРЅРѕРјРµСЂРЅС‹Р№ РјР°СЃСЃРёРІ, РІ РєРѕС‚РѕСЂС‹Р№ РІС‹РІРѕРґРёС‚СЃСЏ СЂРµР·СѓР»СЊС‚Р°С‚)
*/


void hclsm(int w, int h, double *const z, const double* c, const double* L, double& uwe, double *const D, const double* W,
           int q, const double* E, const double* t);

/*
РњРµС‚РѕРґ РЅР°РёРјРµРЅСЊС€РёС… РєРІР°РґСЂР°С‚РѕРІ c "Р¶РµСЃС‚РєРёРјРё" СѓСЃР»РѕРІРёСЏРјРё.
Р РµС€Р°РµРј СЃРёСЃС‚РµРјСѓ РІРёРґР° L = СЃ*z + r, r - СЌС‚Рѕ РІРµРєС‚РѕСЂ РЅРµРІСЏР·РѕРє СЃ РЅСѓР»РµРІС‹Рј РјР°С‚РѕР¶РёРґР°РЅРёРµРј
"Р¶РµСЃС‚РєРёРµ СѓСЃР»РѕРІРёСЏ" РІРІРѕРґСЏС‚СЃСЏ РєР°Рє РјРѕРґРµР»СЊ РІРёРґР° E*z = t (СЃС‚СЂРѕРіРѕРµ СЂР°РІРµРЅСЃС‚РІРѕ). q - СЂР°Р·РјРµСЂ РјР°С‚СЂРёС†С‹ Р• РїРѕ РІРµСЂС‚РёРєР°Р»Рё.
L - СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РјР°СЃСЃРёРІ СЌР»РµРјРµРЅС‚РѕРІ, РїСЂРµРґСЃС‚Р°РІР»СЏСЋС‰РёР№ РІРµРєС‚РѕСЂ РґР°РЅРЅС‹С….
W - СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РјР°СЃСЃРёРІ СЌР»РµРјРµРЅС‚РѕРІ, РїСЂРµРґСЃС‚Р°РІР»СЏСЋС‰РёР№ РІРµСЃРѕРІСѓСЋ РґРёР°РіРѕРЅР°Р»СЊРЅСѓСЋ РјР°С‚СЂРёС†Сѓ.
Р”Р»СЏ СЌРєРѕРЅРѕРјРёРё РІСЂРµРјРµРЅРё Рё СЂРµСЃСѓСЂСЃРѕРІ РґРёР°РіРѕРЅР°Р»СЊРЅР°СЏ РјР°С‚СЂРёС†Р° С…СЂР°РЅРёС‚СЃСЏ РєР°Рє РѕРґРЅРѕРјРµСЂРЅС‹Р№ РјР°СЃСЃРёРІ РґРёР°РіРѕРЅР°Р»СЊРЅС‹С… СЌР»РµРјРµРЅС‚РѕРІ.
РџРѕ СѓРјРѕР»С‡Р°РЅРёСЋ СЌС‚РѕС‚ СѓРєР°Р·Р°С‚РµР»СЊ СЂР°РІРµРЅ 0 (С‚Рѕ РµСЃС‚СЊ РµСЃР»Рё РІРµСЃР° РЅРµ РЅСѓР¶РЅС‹)
w,h - СЂР°Р·РјРµСЂС‹ РјР°С‚СЂС†С‹ РїР»Р°РЅР° (СЃ) m РїРѕ РіРѕСЂРёР·РѕРЅС‚Р°Р»Рё Рё РІРµСЂС‚РёРєР°Р»Рё. РћРґРЅРѕРІСЂРµРјРµРЅРЅРѕ w - С‡РёСЃР»Рѕ РЅРµРёР·РІРµСЃС‚РЅС‹С…, h - С‡РёСЃР»Рѕ СѓСЃР»РѕРІРЅС‹С… СѓСЂР°РІРЅРµРЅРёР№
uwe - СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РїРµСЂРµРјРµРЅРЅСѓСЋ, РІ РєРѕС‚РѕСЂСѓСЋ РІС‹РІРѕРґРёС‚СЃСЏ Р·РЅР°С‡РµРЅРёРµ РѕС€РёР±РєРё РµРґРёРЅРёС†С‹ РІРµСЃР° РІ РєРІР°РґСЂР°С‚Рµ
z - РІРµРєС‚РѕСЂ РѕС†РµРЅРѕРє РїР°СЂР°РјРµС‚СЂРѕРІ РјРѕРґРµР»Рё (СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РѕРґРЅРѕРјРµСЂРЅС‹Р№ РјР°СЃСЃРёРІ, РІ РєРѕС‚РѕСЂС‹Р№ РІС‹РІРѕРґРёС‚СЃСЏ СЂРµР·СѓР»СЊС‚Р°С‚)
D - СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РјР°СЃСЃРёРІ СЌР»РµРјРµРЅС‚РѕРІ РєРѕРІР°СЂРёР°С†РёРѕРЅРЅРѕР№ РјР°С‚СЂРёС†С‹
*/



void lsm(int w, int h, double *const z, const double* c, const double* L, double& uwe, double *const D, const double* W = 0);

/*
РњРµС‚РѕРґ РЅР°РёРјРµРЅСЊС€РёС… РєРІР°РґСЂР°С‚РѕРІ c РІС‹РІРѕРґРѕРј РєРѕРІР°СЂРёР°С†РёРѕРЅРЅРѕР№ РјР°С‚СЂРёС†С‹. Р РµС€Р°РµРј СЃРёСЃС‚РµРјСѓ РІРёРґР° L = СЃ*z + r, r - СЌС‚Рѕ РІРµРєС‚РѕСЂ РЅРµРІСЏР·РѕРє СЃ РЅСѓР»РµРІС‹Рј РјР°С‚РѕР¶РёРґР°РЅРёРµРј
L - СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РјР°СЃСЃРёРІ СЌР»РµРјРµРЅС‚РѕРІ, РїСЂРµРґСЃС‚Р°РІР»СЏСЋС‰РёР№ РІРµРєС‚РѕСЂ РґР°РЅРЅС‹С….
W - СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РјР°СЃСЃРёРІ СЌР»РµРјРµРЅС‚РѕРІ, РїСЂРµРґСЃС‚Р°РІР»СЏСЋС‰РёР№ РІРµСЃРѕРІСѓСЋ РґРёР°РіРѕРЅР°Р»СЊРЅСѓСЋ РјР°С‚СЂРёС†Сѓ.
Р”Р»СЏ СЌРєРѕРЅРѕРјРёРё РІСЂРµРјРµРЅРё Рё СЂРµСЃСѓСЂСЃРѕРІ РґРёР°РіРѕРЅР°Р»СЊРЅР°СЏ РјР°С‚СЂРёС†Р° С…СЂР°РЅРёС‚СЃСЏ РєР°Рє РѕРґРЅРѕРјРµСЂРЅС‹Р№ РјР°СЃСЃРёРІ РґРёР°РіРѕРЅР°Р»СЊРЅС‹С… СЌР»РµРјРµРЅС‚РѕРІ.
РџРѕ СѓРјРѕР»С‡Р°РЅРёСЋ СЌС‚РѕС‚ СѓРєР°Р·Р°С‚РµР»СЊ СЂР°РІРµРЅ 0 (С‚Рѕ РµСЃС‚СЊ РµСЃР»Рё РІРµСЃР° РЅРµ РЅСѓР¶РЅС‹)
w,h - СЂР°Р·РјРµСЂС‹ РјР°С‚СЂС†С‹ РїР»Р°РЅР° (СЃ) m РїРѕ РіРѕСЂРёР·РѕРЅС‚Р°Р»Рё Рё РІРµСЂС‚РёРєР°Р»Рё. РћРґРЅРѕРІСЂРµРјРµРЅРЅРѕ w - С‡РёСЃР»Рѕ РЅРµРёР·РІРµСЃС‚РЅС‹С…, h - С‡РёСЃР»Рѕ СѓСЃР»РѕРІРЅС‹С… СѓСЂР°РІРЅРµРЅРёР№
uwe - СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РїРµСЂРµРјРµРЅРЅСѓСЋ, РІ РєРѕС‚РѕСЂСѓСЋ РІС‹РІРѕРґРёС‚СЃСЏ Р·РЅР°С‡РµРЅРёРµ РѕС€РёР±РєРё РµРґРёРЅРёС†С‹ РІРµСЃР° РІ РєРІР°РґСЂР°С‚Рµ
z - РІРµРєС‚РѕСЂ РѕС†РµРЅРѕРє РїР°СЂР°РјРµС‚СЂРѕРІ РјРѕРґРµР»Рё (СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РѕРґРЅРѕРјРµСЂРЅС‹Р№ РјР°СЃСЃРёРІ, РІ РєРѕС‚РѕСЂС‹Р№ РІС‹РІРѕРґРёС‚СЃСЏ СЂРµР·СѓР»СЊС‚Р°С‚)
D - СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РјР°СЃСЃРёРІ СЌР»РµРјРµРЅС‚РѕРІ РєРѕРІР°СЂРёР°С†РёРѕРЅРЅРѕР№ РјР°С‚СЂРёС†С‹
*/


void iLSM(int w, int h, double maxdiv, int *const exclind, double *const z, const double* c, const double* L, double& uwe, double *const D, double fl, int& rn, const double* W =0);

/*
РњРµС‚РѕРґ РЅР°РёРјРµРЅСЊС€РёС… РєРІР°РґСЂР°С‚РѕРІ c РІС‹РІРѕРґРѕРј РєРѕРІР°СЂРёР°С†РёРѕРЅРЅРѕР№ РјР°С‚СЂРёС†С‹. Р РµС€Р°РµРј СЃРёСЃС‚РµРјСѓ РІРёРґР° L = СЃ*z
L - СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РјР°СЃСЃРёРІ СЌР»РµРјРµРЅС‚РѕРІ, РїСЂРµРґСЃС‚Р°РІР»СЏСЋС‰РёР№ РІРµРєС‚РѕСЂ РґР°РЅРЅС‹С….
W - СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РјР°СЃСЃРёРІ СЌР»РµРјРµРЅС‚РѕРІ, РїСЂРµРґСЃС‚Р°РІР»СЏСЋС‰РёР№ РІРµСЃРѕРІСѓСЋ РґРёР°РіРѕРЅР°Р»СЊРЅСѓСЋ РјР°С‚СЂРёС†Сѓ.
Р”Р»СЏ СЌРєРѕРЅРѕРјРёРё РІСЂРµРјРµРЅРё Рё СЂРµСЃСѓСЂСЃРѕРІ РґРёР°РіРѕРЅР°Р»СЊРЅР°СЏ РјР°С‚СЂРёС†Р° С…СЂР°РЅРёС‚СЃСЏ РєР°Рє РѕРґРЅРѕРјРµСЂРЅС‹Р№ РјР°СЃСЃРёРІ РґРёР°РіРѕРЅР°Р»СЊРЅС‹С… СЌР»РµРјРµРЅС‚РѕРІ.
РџРѕ СѓРјРѕР»С‡Р°РЅРёСЋ СЌС‚РѕС‚ СѓРєР°Р·Р°С‚РµР»СЊ СЂР°РІРµРЅ 0 (С‚Рѕ РµСЃС‚СЊ РµСЃР»Рё РІРµСЃР° РЅРµ РЅСѓР¶РЅС‹)
w,h - СЂР°Р·РјРµСЂС‹ РјР°С‚СЂС†С‹ РїР»Р°РЅР° (СЃ) m РїРѕ РіРѕСЂРёР·РѕРЅС‚Р°Р»Рё Рё РІРµСЂС‚РёРєР°Р»Рё. РћРґРЅРѕРІСЂРµРјРµРЅРЅРѕ w - С‡РёСЃР»Рѕ РЅРµРёР·РІРµСЃС‚РЅС‹С…, h - С‡РёСЃР»Рѕ СѓСЃР»РѕРІРЅС‹С… СѓСЂР°РІРЅРµРЅРёР№
uwe - СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РїРµСЂРµРјРµРЅРЅСѓСЋ, РІ РєРѕС‚РѕСЂСѓСЋ РІС‹РІРѕРґРёС‚СЃСЏ Р·РЅР°С‡РµРЅРёРµ РѕС€РёР±РєРё РµРґРёРЅРёС†С‹ РІРµСЃР° РІ РєРІР°РґСЂР°С‚Рµ
z - РІРµРєС‚РѕСЂ РѕС†РµРЅРѕРє РїР°СЂР°РјРµС‚СЂРѕРІ РјРѕРґРµР»Рё (СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РѕРґРЅРѕРјРµСЂРЅС‹Р№ РјР°СЃСЃРёРІ, РІ РєРѕС‚РѕСЂС‹Р№ РІС‹РІРѕРґРёС‚СЃСЏ СЂРµР·СѓР»СЊС‚Р°С‚)
D - СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РјР°СЃСЃРёРІ СЌР»РµРјРµРЅС‚РѕРІ РєРѕРІР°СЂРёР°С†РёРѕРЅРЅРѕР№ РјР°С‚СЂРёС†С‹
maxdiv - РјР°РєСЃРёРјР°Р»СЊРЅРѕРµ Р·РЅР°С‡РµРЅРёРµ РЅРµРІСЏР·РєРё СѓСЃР»РѕРІС‹РЅС‹С… СѓСЂР°РІРЅРµРЅРёР№, СѓСЂР°РІРЅРµРЅРёСЏ СЃ РЅРµРІСЏР·РєРѕР№, Р±РѕР»СЊС€РµР№ maxdiv РёСЃРєР»СЋС‡Р°СЋС‚СЃСЏ РёР· СЂРµС€РµРЅРёР№
exclind - РѕРґРЅРѕРјРµСЂРЅС‹Р№ РјР°СЃСЃРёРІ, СЃРѕРґРµСЂР¶РёС‚ РЅРѕРјРµСЂ СѓСЂР°РІРЅРµРЅРёСЏ, РµСЃР»Рё РѕРЅРѕ РёСЃРїРѕР»СЊР·РѕРІР°Р»РѕСЃСЊ Рё -1, РµСЃР»Рё РѕРЅРѕ РёСЃРєР»СЋС‡РµРЅРѕ
fl - С„Р»Р°Рі, РјР°СЂРєРёСЂСѓСЋС‰РёР№ СЃРїРѕСЃРѕР± РѕС‚Р±СЂР°РєРѕРІРєРё:
РµСЃР»Рё fl>0, С‚Рѕ РЅР° РїРµСЂРІРѕР№ РёС‚РµСЂР°С†РёРё РІС‹С‡РёСЃР»СЏРµС‚СЃСЏ РѕС€РёР±РєР° РµРґРёРЅРёС†С‹ РІРµСЃР° Рё РїРµСЂРµРІС‹С‡РёСЃР»СЏРµС‚СЃСЏ maxdiv = fl*'РѕС€РёР±РєСѓ РµРґРёРЅРёС†С‹ РІРµСЃР°'.
Р­С‚Рѕ РїРѕР·РІРѕР»СЏРµС‚ РїСЂРёРјРµРЅСЏС‚СЊ РєСЂРёС‚РµСЂРёРё С‚РёРїР° "3-СЃРёРіРјР°"
РµСЃР»Рё fl<=0, С‚Рѕ РґР»СЏ РѕС‚Р±СЂР°РєРѕРІРєРё РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ maxdiv, Р·Р°РґР°РЅРЅС‹Р№ РїСЂРё РІС‹Р·РѕРІРµ С„СѓРЅРєС†РёРё
rn - РїРѕ Р·Р°РІРµСЂС€РµРЅРёРё СЃРѕРґРµСЂР¶РёС‚ С‡РёСЃР»Рѕ СѓСЂР°РІРЅРµРЅРёР№, РєРѕС‚РѕСЂРѕРµ СЂРµР°Р»СЊРЅРѕ РёСЃРїРѕР»СЊР·РѕРІР°Р»РѕСЃСЊ.
*/


void centerofmass(double *const cm, const double* fd, int rho);


void getFWHM(double *const fwhm, const double* fd, int rho, double blevel);


double getpsf(const double* p, double x, double y, int model);
//С„СѓРЅРєС†РёСЏ РІС‹РґР°РµС‚ Р·РЅР°С‡РµРЅРёРµ PSF РґР»СЏ РІРµРєС‚РѕСЂР° РїР°СЂР°РјРµС‚СЂРѕРІ p Рё РїРёРєСЃРµР»СЊРЅС‹С… РєРѕРѕСЂРґРёРЅР°С‚ x,y.
// model - РјРѕРґРµР»СЊ PSF (0 - РїСЂРѕС„РёР»СЊ Р›РѕСЂРµРЅС†Р°, 1 - РїСЂРѕС„РёР»СЊ РњРѕС„С„Р°С‚Р°, 2 - РїСЂРѕС„РёР»СЊ Р“Р°СѓСЃСЃР° СЃРѕ СЃС‚РµРїРµРЅСЊСЋ).


double getpdpsf(const double* p, double x, double y, int model, int nv);
/*С„СѓРЅРєС†РёСЏ РІС‹РґР°РµС‚ Р·РЅР°С‡РµРЅРёРµ С‡Р°СЃС‚РЅРѕР№ РїСЂРѕРёР·РІРѕРґРЅРѕР№ PSF РґР»СЏ РІРµРєС‚РѕСЂР° РїР°СЂР°РјРµС‚СЂРѕРІ p Рё РїРёРєСЃРµР»СЊРЅС‹С… РєРѕРѕСЂРґРёРЅР°С‚ x,y.
model - РјРѕРґРµР»СЊ PSF (0 - РїСЂРѕС„РёР»СЊ Р›РѕСЂРµРЅС†Р°, 1 - РїСЂРѕС„РёР»СЊ РњРѕС„С„Р°С‚Р°, 2 - РїСЂРѕС„РёР»СЊ Р“Р°СѓСЃСЃР° СЃРѕ СЃС‚РµРїРµРЅСЊСЋ).
nv - РЅРѕРјРµСЂ РїРµСЂРµРјРµРЅРЅРѕР№ РґР»СЏ РєРѕС‚РѕСЂРѕР№ РІС‹С‡РёСЃР»СЏРµС‚СЃСЏ Р·РЅР°С‡РµРЅРёРµ С‡Р°СЃРЅРѕР№ РїСЂРѕРёР·РІРѕРґРЅРѕР№.
p[0] = Xo - РїРёРєСЃРµР»СЊРЅС‹Рµ РєРѕРѕСЂРґРёРЅР°С‚С‹ С„РѕС‚РѕС†РµРЅС‚СЂР° РёР·РѕР±СЂР°Р¶РµРЅРёСЏ Р·РІРµР·РґРѕРѕР±СЂР°Р·РЅРѕРіРѕ РѕР±СЉРµРєС‚Р°
p[1] = Yo
p[2] = Jo - СѓСЂРѕРІРµРЅСЊ С„РѕРЅР°. РќРµ РІС‹С‡РёСЃР»СЏРµС‚СЃСЏ РІ РґР°РЅРЅРѕР№ С„СѓРЅРєС†РёРё, РІС‹С‡РёСЃР»СЏРµС‚СЃСЏ РІРЅРµ С„СѓРЅРєС†РёРё Рё РїРµСЂРµРґР°РµС‚СЃСЏ РІРѕ РІС…РѕРґРЅРѕРј РјР°СЃСЃРёРІРµ p
p[3]= Jmax - СѓСЂРѕРІРµРЅСЊ РјР°РєСЃРёРјСѓРјР° psf
p[4] = A, p[5] = B, p[6] = C - РїР°СЂР°РјРµС‚СЂС‹, РѕРїРёСЃС‹РІР°СЋС‰РёРµ С€РёСЂРёРЅСѓ РїСЂРѕС„РёР»СЏ Рё РµРіРѕ РѕСЂРёРµРЅС‚РёСЂРѕРІРєСѓ Рё СЌР»Р»РµРїС‚РёС‡РЅРѕСЃС‚СЊ
p[7] = delta - СЃС‚РµРїРµРЅСЊ psf
*/

double lorenzian(double *const p, double x, double y);
double Jlorenzian(double *const p, double x, double y,int n);
bool lorenzianFit(double *const p, const double* fd,  int rho, int nit, double minstep);

bool psffit(double *const p, const double* fd, int rho, int model, int nit, double kappa, double lambda, double regk, int& it, int ipfl);
/*
С„СѓРЅРєС†РёСЏ СЂРµР°Р»РёР·СѓРµС‚ РІС‹С‡РёСЃР»РµРЅРёСЏ РїРѕ Р°Р»РіРѕСЂРёС‚РјСѓ Р›РµРІРµРЅР±РµСЂРіР°-РњР°СЂРєРІР°СЂРґС‚Р° РґР»СЏ РѕРїСЂРµРґРµР»РµРЅРёСЏ РїР°СЂР°РјРµС‚СЂРѕРІ psf
p - РІРµРєС‚РѕСЂ РїР°СЂР°РјРµС‚СЂРѕРІ (РґР»РёРЅР° РјР°СЃСЃРёРІР° РґРѕР»Р¶РЅР° Р±С‹С‚СЊ СЂР°РІРЅР° 21)
p[0] = Xo - РїРёРєСЃРµР»СЊРЅС‹Рµ РєРѕРѕСЂРґРёРЅР°С‚С‹ С„РѕС‚РѕС†РµРЅС‚СЂР° РёР·РѕР±СЂР°Р¶РµРЅРёСЏ Р·РІРµР·РґРѕРѕР±СЂР°Р·РЅРѕРіРѕ РѕР±СЉРµРєС‚Р°
p[1] = Yo
p[2] = Jo - СѓСЂРѕРІРµРЅСЊ С„РѕРЅР°. РќРµ РІС‹С‡РёСЃР»СЏРµС‚СЃСЏ РІ РґР°РЅРЅРѕР№ С„СѓРЅРєС†РёРё, РІС‹С‡РёСЃР»СЏРµС‚СЃСЏ РІРЅРµ С„СѓРЅРєС†РёРё Рё РїРµСЂРµРґР°РµС‚СЃСЏ РІРѕ РІС…РѕРґРЅРѕРј РјР°СЃСЃРёРІРµ p
p[3]= Jmax - СѓСЂРѕРІРµРЅСЊ РјР°РєСЃРёРјСѓРјР° psf
p[4] = A, p[5] = B, p[6] = C - РїР°СЂР°РјРµС‚СЂС‹, РѕРїРёСЃС‹РІР°СЋС‰РёРµ С€РёСЂРёРЅСѓ РїСЂРѕС„РёР»СЏ Рё РµРіРѕ РѕСЂРёРµРЅС‚РёСЂРѕРІРєСѓ Рё СЌР»Р»РµРїС‚РёС‡РЅРѕСЃС‚СЊ
p[7] = delta - СЃС‚РµРїРµРЅСЊ psf

fd - РјР°СЃСЃРёРІ РѕС‚СЃС‡РµС‚РѕРІ РІ РїСЂРµРґРµР»Р°С… Р°РїРµСЂС‚СѓСЂС‹.
model - РјРѕРґРµР»СЊ PSF (0 - РїСЂРѕС„РёР»СЊ Р›РѕСЂРµРЅС†Р°, 1 - РїСЂРѕС„РёР»СЊ РњРѕС„С„Р°С‚Р°, 2 - РїСЂРѕС„РёР»СЊ Р“Р°СѓСЃСЃР° СЃРѕ СЃС‚РµРїРµРЅСЊСЋ)
rho - СЂР°РґРёСѓСЃ Р°РїРµСЂС‚СѓСЂС‹
nit - РјР°РєСЃРёРјР°Р»СЊРЅРѕ-РІРѕР·РјРѕР¶РЅРѕРµ С‡РёСЃР»Рѕ СѓСЃРїРµС€РЅС‹С… РёС‚РµСЂР°С†РёР№ (РѕРїС‹С‚ РїРѕРґСЃРєР°Р·С‹РІР°РµС‚, С‡С‚Рѕ Р»СѓС‡С€Рµ РѕРіСЂР°РЅРёС‡РёС‚СЊСЃСЏ Р·РЅР°С‡РµРЅРёРµРј РІ РїСЂРµРґРµР»Р°С… 20 -50)
kappa - РєСЂРёС‚РµСЂРёР№ Р·Р°РІРµСЂС€РµРЅРёСЏ РІС‹С‡РёСЃР»РµРЅРёР№, СЌС‚Рѕ РѕС‚РЅРѕС€РµРЅРёРµ РґР»РёРЅС‹ РІРµРєС‚РѕСЂР° РїРѕРїСЂР°РІРѕРє Рє РєРѕСЂРЅСЋ РёР· РґРёСЃРїРµСЂСЃРёРё РїРѕСЃР»РµРґРЅРёС… С€РµСЃС‚Рё РІРµРєС‚РѕСЂРѕРІ РїРѕРїСЂР°РІРѕРє. Р­С‚Рѕ СЃРІРѕРµРѕР±СЂР°Р·РЅС‹Р№ РєСЂРёС‚РµСЂРёР№ СЃС…РѕРґРёРјРѕСЃС‚Рё, РІРѕР·РјРѕР¶РЅРѕ РЅРµ СЃР°РјС‹Р№ Р»СѓС‡С€РёР№.Р Р°Р·СѓРјРЅС‹Рµ Р·РЅР°С‡РµРЅРёСЏ СѓРјРµСЃС‚РЅРѕ Р±СЂР°С‚СЊ РІ РїСЂРµРґРµР»Р°С… РѕС‚ 0.01 РґРѕ 0.1. РўРѕ РµСЃС‚СЊ i-С‚Р°СЏ РїРѕРїСЂР°РІРєР° Р»РµР¶РёС‚ РіР»СѓР±РѕРєРѕ РІ РїСЂРµРґРµР»Р°С… РґРёСЃРїРµСЂСЃРёРё РїРѕСЃР»РµРґРЅРёС… РїРѕРїСЂР°РІРѕРє.

lambda - СЂРµРіСѓР»СЏСЂРёР·СѓСЋС‰РёР№ РєРѕСЌС„С„РёС†РёРµРЅС‚. РћР±С‹С‡РЅРѕ РЅР° РІС…РѕРґРµ СЃРѕСЃС‚Р°РІР»СЏРµС‚ 0.001 - 0.01. РџРѕРєР°Р·С‹РІР°РµС‚ РєР°РєР°СЏ РґРѕР»СЏ РѕС‚ РєР°Р¶РґРѕРіРѕ РґРёР°РіРѕРЅР°Р»СЊРЅРѕРіРѕ СЌР»РµРјРµРЅС‚Р° Р“РµСЃСЃРёР°РЅР° РґРѕР»Р¶РЅР° Рє РЅРµРјСѓ РїСЂРёР±Р°РІР»СЏС‚СЊСЃСЏ.
regk - РєРѕСЌС„С„РёС†РёРµРЅС‚ РґР»СЏ СѓРїСЂР°РІР»РµРЅРёСЏ Р·РЅР°С‡РµРЅРёРµРј lambda. РџСЂРё СѓСЃРїРµС€РЅРѕР№ РёС‚РµСЂР°С†РёРё lambda СѓРјРµРЅСЊС€Р°РµС‚СЃСЏ РІ СЃРѕРѕС‚РІРµС‚СЃС‚РІРёРµ СЃ regk РІ СЃС‚РµРїРµРЅРё С‡РёСЃР»Р° РЅРµСѓСЃРїРµС€РЅС‹С… РёС‚РµСЂР°С†РёР№ (СѓСЃРїРµС€РЅР°СЏ РёС‚РµСЂР°С†РёСЏ - РёС‚РµСЂР°С†РёСЏ, РїСЂРё РєРѕС‚РѕСЂРѕР№ СѓРјРµРЅСЊС€Р°РµС‚СЃСЏ СЃСѓРјРјР° РєРІР°РґСЂР°С‚РѕРІ РЅРµРІСЏР·РѕРє).
Р’ СЃР»СѓС‡Р°Рµ РЅРµСѓСЃРїРµС€РЅРѕР№ РёС‚РµСЂР°С†РёРё lambda СѓРјРЅРѕР¶Р°РµС‚СЃСЏ РЅР° regk.

it - РјРѕР¶РЅРѕ РїРµСЂРµРґР°С‚СЊ 0-РѕРµ Р·РЅР°С‡РµРЅРёРµ. РџРѕСЃР»Рµ РІС‹С…РѕРґР° РёР· С„СѓРЅРєС†РёРё it Р±СѓРґРµС‚ СЃРѕРґРµСЂР¶Р°С‚СЊ С‡РёСЃР»Рѕ СѓСЃРїРµС€РЅС‹С… РёС‚РµСЂР°С†РёР№

ipfl - С„Р»Р°Рі Р·Р°РґР°СЋС‰РёР№ СЃРїРѕСЃРѕР± РІС‹С‡РёСЃР»РµРЅРёСЏ РЅР°С‡Р°Р»СЊРЅС‹С… Р·РЅР°С‡РµРЅРёР№ Xo,Yo. Р•СЃР»Рё ipfl = 0 - РїРѕ РјР°РєСЃРёРјР°Р»СЊРЅРѕРјСѓ РїРёРєСЃРµР»СЋ (РґР»СЏ РџР—РЎ - СЌС‚Рѕ Р»СѓС‡С€Рµ), ipfl = 1 - РїРѕ С†РµРЅС‚СЂСѓ РјР°СЃСЃ (РїСЂРµРґРїРѕР»Р°РіР°РµС‚СЃСЏ, С‡С‚Рѕ СЌС‚Рѕ Р»СѓС‡С€Рµ РїРѕРґС…РѕРґРёС‚ РґР»СЏ СЃРєР°РЅРёСЂРѕРІР°РЅРЅС‹С… РёР·РѕР±СЂР°Р¶РµРЅРёР№ С„РѕС‚РѕРїР»Р°СЃС‚РёРЅРѕРє)

РќР° РІС‹С…РѕРґРµ РІ РјР°СЃСЃРёРІРµ p СЃРѕРґРµСЂР¶Р°С‚СЃСЏ: СЌР»РµРјРµРЅС‚С‹ 0 - 7 - Р·РЅР°С‡РµРЅРёСЏ РїР°СЂР°РјРµС‚СЂРѕРІ РїСЂРѕС„РёР»СЏ РєР°Рє РѕРїРёСЃР°РЅРѕ РІС‹С€Рµ.

p[8] = РѕС€РёР±РєР° РµРґРёРЅРёС†С‹ РІРµСЃР° РґР»СЏ РїРѕСЃР»РµРґРЅРµР№ РёС‚РµСЂР°С†РёРё
СЌР»РµРјРµРЅС‚С‹ 9 - 16 - РѕС†РµРЅРєРё РЎРљРћ СЃРѕРѕС‚РІРµС‚СЃС‚РІСѓСЋС‰РёС… РїР°СЂР°РјРµС‚СЂРѕРІ 0-7
p[17] - Р·РЅР°С‡РµРЅРёРµ РґРµСЃСЏС‚РёС‡РЅРѕРіРѕ Р»РѕРіР°СЂРёС„РјР° РёРЅС‚РµРіСЂР°Р»Р° psf РЅР°Рґ С„РѕРЅРѕРј РІ РїСЂРµРґРµР»Р°С… Р°РїРµСЂС‚СѓСЂС‹
p[18] - Р·РЅР°С‡РµРЅРёРµ РґРµСЃСЏС‚РёС‡РЅРѕРіРѕ Р»РѕРіР°СЂРёС„РјР° СЃСѓРјРјС‹ РѕС‚СЃС‡РµС‚РѕРІ РЅР° РїРёРєСЃРµР»СЏС… РІ РїСЂРµРґРµР»Р°С… Р°РїРµСЂС‚СѓСЂС‹
РџРѕСЃР»РµРґРЅРёРµ РґРІР° Р·РЅР°С‡РµРЅРёСЏ С…Р°СЂР°РєС‚РµСЂРёР·СѓСЋС‚ РїРѕС‚РѕРє РёР·Р»СѓС‡РµРЅРёСЏ РѕС‚ РёР·РјРµСЂСЏРµРјРѕРіРѕ РѕР±СЉРµРєС‚Р°
p[19] - С‡РёСЃР»Рѕ РїРёРєСЃРµР»РµР№ РІ РїСЂРµРґРµР»Р°С… Р°РїРµСЂС‚СѓСЂС‹
p[20] - РѕР±С‰РµРµ С‡РёСЃР»Рѕ РёС‚РµСЂР°С†РёР№ (Рё СѓСЃРїРµС€РЅС‹С… Рё РЅРµСѓСЃРїРµС€РЅС‹С…)
*/


bool ringfit(double *const p, const double* fd, int rho2);

void subtractSurface(const double* p, double *const fd, double bp, int rho);

double meanRingValue(const double* fd, int rho2);

int Cubic(double *x,double a,double b,double c);

void angletr(double X0, double Y0, double X1, double Y1,double X2, double Y2, double& angle, double& ratio);

bool matchstars(int *tangNum,const  double *ksi,const  double *eta, int *pixNum,const  double *X,const  double *Y, int Nstang, int Nspix, double err, double eAng, double eRatio);


}

#endif // MB_H
