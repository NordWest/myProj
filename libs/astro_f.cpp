//my astronomy functions library
#include "math.h"


//double getJD(int year, int month, int day, int hour, int min, int sec){
double getJD(int year, int month, double d_day){
/*
	double d_day=day;
	double d_hour = hour;
	double d_min = min;
	double d_sec = sec;
	d_day = d_day + (d_hour+(d_min+d_sec/60)/60)/24;
*/
	int y;
	int m;
    if (month<3){
		 y =year-1;
		 m =month+12;
	}
	else {
		 y =year;
		 m =month;
	}
	int A = y/100;
	double B = 2-A+floor(A/4);
    double JD =floor(365.25*y)+floor(30.6001*(m+1))+d_day+1720994.5+B;
return JD;
};
/*
double getS(int year, int month, int day, int hour, int min, int sec){
	double d_hour = hour;
	double d_min = min;
	double d_sec = sec;
	double d_day = d_hour*3600+d_min*60+d_sec;
	double JD = getJD(year,month,day, 0, 0, 0);
	double T=(JD-2451545)/36525;
	//double S0=6*3600+41*60+50.54841+8640184.812866*T+0.093104*T*T-0.0000062*T*T*T;
	double S0=6*3600+41*60+50.54841+236.555367908*(JD-2451545)+0.093104*T*T-0.0000062*T*T*T;
	S0=S0-floor(S0/86400)*86400;
	double S = S0+d_day*366.25/365.25+7278;
	if (S>86400){S=S-86400;}
	return S;
};
*/
   