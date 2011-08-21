#ifndef IMGPROFILE_H
#define IMGPROFILE_H

//QtCore#include <QList>
#include <QtCore>

#define PVECT_SIZE 21

/*
p - ЧЕЛФПТ РБТБНЕФТПЧ (ДМЙОБ НБУУЙЧБ ДПМЦОБ ВЩФШ ТБЧОБ 21)
p[0] = Xo - РЙЛУЕМШОЩЕ ЛППТДЙОБФЩ ЖПФПГЕОФТБ ЙЪПВТБЦЕОЙС ЪЧЕЪДППВТБЪОПЗП ПВЯЕЛФБ
p[1] = Yo
p[2] = Jo - ХТПЧЕОШ ЖПОБ. оЕ ЧЩЮЙУМСЕФУС Ч ДБООПК ЖХОЛГЙЙ, ЧЩЮЙУМСЕФУС ЧОЕ ЖХОЛГЙЙ Й РЕТЕДБЕФУС ЧП ЧИПДОПН НБУУЙЧЕ p
p[3]= Jmax - ХТПЧЕОШ НБЛУЙНХНБ psf
p[4] = A, p[5] = B, p[6] = C - РБТБНЕФТЩ, ПРЙУЩЧБАЭЙЕ ЫЙТЙОХ РТПЖЙМС Й ЕЗП ПТЙЕОФЙТПЧЛХ Й ЬММЕРФЙЮОПУФШ
p[7] = delta - УФЕРЕОШ psf

p[8] - ПЫЙВЛБ ЕДЙОЙГЩ ЧЕУБ
p[9] - p[16] - ПЫЙВЛЙ РБТБНЕФТПЧ p[0] - p[7]

p[17] - ЪОБЮЕОЙЕ ДЕУСФЙЮОПЗП МПЗБТЙЖНБ ЙОФЕЗТБМБ psf ОБД ЖПОПН Ч РТЕДЕМБИ БРЕТФХТЩ
p[18] - ЪОБЮЕОЙЕ ДЕУСФЙЮОПЗП МПЗБТЙЖНБ УХННЩ ПФУЮЕФПЧ ОБ РЙЛУЕМСИ Ч РТЕДЕМБИ БРЕТФХТЩ
рПУМЕДОЙЕ ДЧБ ЪОБЮЕОЙС ИБТБЛФЕТЙЪХАФ РПФПЛ ЙЪМХЮЕОЙС ПФ ЙЪНЕТСЕНПЗП ПВЯЕЛФБ
p[19] - ЮЙУМП РЙЛУЕМЕК Ч РТЕДЕМБИ БРЕТФХТЩ
p[20] - ПВЭЕЕ ЮЙУМП ЙФЕТБГЙК (Й ХУРЕЫОЩИ, Й ОЕХУРЕЫОЩИ)
*/

class imgProfile
{
    double p[PVECT_SIZE];
    QList <double*> pList;
    int model;
    QString mask;
    QString fName;
public:
    imgProfile();
    imgProfile(QString fname);

    int getP(double *p, int pos);
    int setP(double *p, int pos);

    void setMask(QString smask);
    void setModel(int mod);

    void pushPoint(int x, int y, int m);

    int save();
    int saveAs(QString fname);
};

#endif // IMGPROFILE_H
