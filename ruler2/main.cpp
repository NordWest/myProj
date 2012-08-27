#include <QApplication>
#include <QtCore>
#include <QtGui>
#include "astro.h"
#include "ccdmeasurements.h"
#include "./../libs/fitsio.h"


int main(int argc, char *argv[])
    {  
        QString msgstr;
		QApplication app(argc, argv);
///////// 1. Reading settings ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//BEGIN settings
		
		QSettings *settings = new QSettings("ruler2.ini",QSettings::IniFormat);
		//[%general]
		QString outfolder = settings->value("general/outfolder").toString();//output folder
		QString obscode = settings->value("general/obscode").toString();//MPC code of Observatory
		int objType = settings->value("general/objtype").toInt();//type of object: 0 - asteroid, 1 - planetary satellite, 2 - star like object
		int timescale = settings->value("general/timescale").toInt();//timescale: 0 - UTC, 1 - GPS
		double timescalediff = settings->value("general/timescalediff").toDouble();//difference between time scales
		int endmessage = settings->value("general/endmessage").toInt();//make windows standard message about end of work
		//[psf]
		int model = settings->value("psf/model").toInt();//PSF model: 0 - Lorentz PSF, 1 - Gauss PSF, 2 - Moffat PSF
		double delta = settings->value("psf/delta").toDouble();//initial power in Lorentz PSF or Moffat PSF
		int aperture = settings->value("psf/aperture").toInt();//aperture to get pixels values to PSF fitting
		int subgrad = settings->value("psf/subgrad").toInt();
		int ringradius = settings->value("psf/ringradius").toInt();
		int ringwidth = settings->value("psf/ringwidth").toInt();
		//[reductions]
		double maxres = settings->value("reductions/maxres").toDouble();//maximum of residuals for positional reduction in mas
		double maxresMAG = settings->value("reductions/maxresMAG").toDouble();//maximum of residuals for photometric reduction
		double minrefMag = settings->value("reductions/minrefMag").toDouble();//bottom limit for magnitude for reduction
		double maxrefMag = settings->value("reductions/maxrefMag").toDouble();//upper limit for magnitude for reduction
		int syscorr = settings->value("reductions/syscorr").toInt();//1 - taking systematic into account
		double distfromplanet = settings->value("reductions/distfromplanet").toDouble();
		double distfromsmear=settings->value("reductions/distfromsmear").toDouble();
		double planetKsi,planetEta;
		//[outputlimits]
		double maxOCMAG = settings->value("outputlimits/maxOCMAG").toDouble();//maximum of (O-C) in magnitude for output
		double maxOCRA = settings->value("outputlimits/maxOCRA").toDouble();//maximum of (O-C) in RA for output
		double maxOCDE = settings->value("outputlimits/maxOCDE").toDouble();//maximum of (O-C) in DEC for output
		double maxOCMAGrc = settings->value("outputlimits/maxOCMAGrc").toDouble();//maximum of residuals in magnitude for output
		double maxOCRArc = settings->value("outputlimits/maxOCRArc").toDouble();//maximum of residuals in RA for output
		double maxOCDErc = settings->value("outputlimits/maxOCDErc").toDouble();//maximum of residuals in DEC for output
		//[whatoutput] all settings within this section must be equal to 1 if you want to use them
		int mpcreport=settings->value("whatoutput/mpcreport").toInt();//make MPC report
		int residuals=settings->value("whatoutput/residuals").toInt();//make RESIDUALS report
		int ocreject=settings->value("whatoutput/ocreject").toInt();//if ocreject=0 then results with BAD (O-C) (see maxOCMAG, maxOCRA and maxOCDE) will be inluded to output
		int astreport=settings->value("whatoutput/astreport").toInt();//make ASTROREPORT
		int errorreport=settings->value("whatoutput/errorreport").toInt();//make ERRORREPORT
		int relativepos=settings->value("whatoutput/relativepos").toInt();//relative positions of the satellites
		//[outputforms]
		int time_form = settings->value("outputforms/time_form").toInt();//set form for output moments of time: 0 - YYYY MM DD.DDDD, 1 - MJD, 2 - (MJD - MJDo) in minutes
		int pos_form = settings->value("outputforms/pos_form").toInt();//set form for positional data: 0 - (h,m,s; deg, arcmin, arcsec); 1 - mas; 2 - degrees.
		int phot_form = settings->value("outputforms/phot_form").toInt();//set form for photometric data
		QString sepsymb = settings->value("outputforms/sepsymb").toString();//symbol for separation between records
		if (sepsymb=="space") sepsymb=" ";
		double mjd0 = settings->value("outputforms/mjd0").toDouble();//zero point of MJD to output (MJDo). See time_form
		//[catalogue]
		QString cat = settings->value("catalogue/refcatalogue").toString();// reference catalogue name (ucac2 or 2mass)
		int catindex = -1;// more useful integer value to indicate reference catalogue 
		if(cat=="ucac2")catindex=0;
		if(cat=="2mass")catindex=1;
		QString fov = settings->value("catalogue/fov").toString();// field of view size in arcmin (radius of selection)
		QString minMag = settings->value("catalogue/minMag").toString();//bottom limit for magnitude
		QString maxMag = settings->value("catalogue/maxMag").toString();//upper limit for magnitude
		//[programs]
		QString ast_eph_prog = settings->value("programs/ast_eph_prog").toString();//soft to find asteroids 
		QString ast_eph_prog_folder = settings->value("programs/ast_eph_prog_folder").toString();//working folder of soft to find asteroids
		QString ucac2findn = settings->value("programs/ucac2findn").toString();//soft to find ucac2 data
		QString ucac2find_folder = settings->value("programs/ucac2find_folder").toString();//working folder for soft to find ucac2 data
		QString cdsfindn = settings->value("programs/cdsfindn").toString();//soft to find 2mass data
		QString cdsfind_folder = settings->value("programs/cdsfind_folder").toString();//working folder for soft to find 2mass data
		QString sateph_prog = settings->value("programs/sateph_prog").toString();//soft to find satellites 
		QString sateph_prog_folder = settings->value("programs/sateph_prog_folder").toString();//working folder of soft to find satellites
		//[bugs]
		int timebug2008 = settings->value("bugs/timebug2008").toInt();
		//END settings
///////////2. Reading fits file //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//BEGIN read fits
		char fitsvalue[80];
		QString fname = QString(argv[1]);
		//QMessageBox::information(0,"debug",fname,QMessageBox::Ok,0,0);
		fitsfile *fptr;
		int status = 0;
		fits_open_file(&fptr, argv[1] , READONLY,&status);
		fits_read_key(fptr, TSTRING, "DATE-OBS", fitsvalue, NULL, &status);
		QString date_obs_str = QString(fitsvalue);
		double exp_time;
		fits_read_key(fptr, TDOUBLE, "EXPTIME", &exp_time, NULL, &status);
		double mjd = getMJDfromStrFTN(date_obs_str, exp_time);
		if(timebug2008==1) mjd = mjd - exp_time/86400;
		if(timescale==1) 
		{
			mjd = mjd + timescalediff/86400;
			if(time_form==2) mjd0 = mjd0 + timescalediff/86400;
			//QMessageBox::information(0,"debug","ok",QMessageBox::Ok,0,0);
		}
		//
		double ra_c,de_c;
		double wcs_data[13];
		double wcsV;
		fits_read_key(fptr, TDOUBLE, "CRPIX1", &wcsV, NULL, &status); wcs_data[0] = wcsV;
		fits_read_key(fptr, TDOUBLE, "CRPIX2", &wcsV, NULL, &status); wcs_data[1] = wcsV;
		fits_read_key(fptr, TDOUBLE, "CRVAL1", &wcsV, NULL, &status); wcs_data[2] = wcsV;
		fits_read_key(fptr, TDOUBLE, "CRVAL2", &wcsV, NULL, &status); wcs_data[3] = wcsV;
		fits_read_key(fptr, TDOUBLE, "CDELT1", &wcsV, NULL, &status); wcs_data[4] = wcsV;
		fits_read_key(fptr, TDOUBLE, "CDELT2", &wcsV, NULL, &status); wcs_data[5] = wcsV;
		fits_read_key(fptr, TDOUBLE, "CROTA1", &wcsV, NULL, &status); wcs_data[6] = wcsV;
		fits_read_key(fptr, TDOUBLE, "CROTA2", &wcsV, NULL, &status); wcs_data[7] = wcsV;
		//
		fits_read_key(fptr, TDOUBLE, "CD1_1", &wcsV, NULL, &status); wcs_data[8] = wcsV;
		fits_read_key(fptr, TDOUBLE, "CD1_2", &wcsV, NULL, &status); wcs_data[9] = wcsV;
		fits_read_key(fptr, TDOUBLE, "CD2_1", &wcsV, NULL, &status); wcs_data[10] = wcsV;
		fits_read_key(fptr, TDOUBLE, "CD2_2", &wcsV, NULL, &status); wcs_data[11] = wcsV;
		wcs_data[12] = 1;
		ra_c = wcs_data[2]*3600000;
		de_c = wcs_data[3]*3600000;
		///////////////////////////////////
		long naxes[2];
		long felem = 1;
		fits_get_img_size(fptr, 2, naxes, &status);
		LONGLONG nelements = naxes[0]*naxes[1];
		int anynul = 0;
		unsigned short *fits_buff = new unsigned short[nelements];
		fits_read_img(fptr, TUSHORT, felem, nelements, NULL, fits_buff, &anynul, &status);
		double *fdata = new double[nelements];
		for(LONGLONG k=0;k<nelements;k++)fdata[k] = (double)fits_buff[k];
		delete fits_buff;
		fits_close_file(fptr, &status);
		//BEGIN find most frequent pixel value
		double fpv = freqpix(fdata, (quint32)nelements);
		//END find most frequent pixel value
		//END read fits
///////////3. Reading objects data//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		QProcess outerProcess;
		QStringList outerArguments;
		QString ObjNum;//number of object (for example, MPC number of asteroid)
		QString sMJD = QString( "%1" ).arg( mjd,17,'f',10,QLatin1Char( ' ' ));
		sMJD = sMJD.trimmed();
		
		QString ObjNum1;
		QString ObjName;
		bool is_ok;
		//BEGIN read objects positions
		switch(objType)
		{
			case 0://asteroids with mpeph.exe program
			{
				QFileInfo fileinfo(fname);
				msgstr = fileinfo.fileName();
				ObjNum =  msgstr.section('_',0,0);
				ObjName =  msgstr.section('_',0,0);
				outerProcess.setWorkingDirectory(ast_eph_prog_folder);
				outerProcess.setProcessChannelMode(QProcess::MergedChannels);
				outerProcess.setReadChannel(QProcess::StandardOutput);
				do
				{
					ObjNum1 = QString("%1").arg(ObjNum.toInt(&is_ok));
					ObjNum = ObjNum.left(ObjNum.size()-1);
				}while(!is_ok);
				outerArguments<<"-num" <<ObjNum1<<sMJD;
				outerProcess.start(ast_eph_prog, outerArguments);
				break;
			}
			case 1://saturnian satellites with sateph.exe program
			{
				outerProcess.setWorkingDirectory(sateph_prog_folder);
				outerProcess.setProcessChannelMode(QProcess::MergedChannels);
				outerProcess.setReadChannel(QProcess::StandardOutput);
				outerArguments << sMJD;
				outerProcess.start(sateph_prog, outerArguments);
				break;
			}
		}
		outerProcess.waitForFinished(-1);
		QTextStream objStream(outerProcess.readAllStandardOutput());
		QString objDataStr,elS;
		double *objdata;
		QList<double*> objects;
		int *actv;
		//
		double ra,de,pmra,pmde,mag,dist,numb;
		vector<double> xy(2);
		vector<double> tp(2);
		int NofV;
		CCDDATA pixdata;
		CCDDATA ringdata;
		vector<double> P;
		vector<double> PG;
		QVector<double> PSFr;
		//
		while (!objStream.atEnd())
		{
			objDataStr = objStream.readLine();
			//QMessageBox::information(0,"debug",objDataStr,QMessageBox::Ok,0,0);
			switch(objType)
			{
				case 0://asteroids
				{
					elS = objDataStr.section('|',0,0);ra = hms_to_mas(elS, ":");
					elS = objDataStr.section('|',1,1);de = damas_to_mas(elS, ":");
					xy = getPixPosFromWCS(ra, de, wcs_data);
					if((xy(0)>=0)&&(xy(1)>=0)&&(xy(0)<=naxes[0])&&(xy(1)<=naxes[1]))
					{
						//BEGIN fit and subtract gradient
						if(subgrad==1)
						{
							ringdata = getRingArray(fdata, (int)xy(0),(int)xy(1), 
											ringradius, ringradius + ringwidth, naxes[0],naxes[1]);
							PG = fit2(ringdata);
							subgradfit2(fdata, (int)xy(0),(int)xy(1), 
										ringradius, ringradius + ringwidth, naxes[0],naxes[1], PG, fpv);
						}
						delete ringdata.Buffer;
						//END fit and subtract gradient 
						//BEGIN PSF
						pixdata = ApertureData(fdata, (int)xy(0),(int)xy(1), aperture, naxes[0],naxes[1]);
						switch(model)
						{
						 case 0:{NofV = 8;P = StartConditionsL(pixdata, delta);break;}
						 case 1:{NofV = 7;P = StartConditionsG(pixdata);break;}
						 case 2:{NofV = 8;P = StartConditionsM(pixdata, delta); break;}
						}
						actv = new int[NofV];
						for (int j=0;j<NofV;j++) actv[j] = 1;
						PSFr.resize(0);
						PSFr = NLSLevenbergMarquardt(pixdata, P, model, actv, NofV, 10, 10, 1e-7, 1e-6);
						//END PSF
						if(PSFr[0]!=-1)
						{
							objdata = new double[16];
							objdata[0]=ra;//RA from theory of motion or catalogue
							objdata[1]=de;//DEC from theory of motion  or catalogue
							tp = getRaDeToTang(ra, de, ra_c, de_c);
							objdata[2]=tp(0);//tangential position KSI based on RA (C)
							objdata[3]=tp(1);//tangential position ETA based on DE (C)
							objdata[4]=PSFr[5] - wcs_data[0];//measured pixel position X (O)
							objdata[5]=PSFr[6] - wcs_data[1];//measured pixel position Y (O)
							objdata[6]=log10(pixdata.meanADC);//mean value of pixels within aperture
							objdata[7]=PSFr[16];//unut weight error of PSF fitting
							elS = objDataStr.section('|',3,3);objdata[8]=elS.toDouble();//magnitude from theory of motion or catalogue
							elS = objDataStr.section('|',2,2);objdata[9]=elS.toDouble();//topocentric distance from theory of motion or catalogue
							elS = objDataStr.section('|',6,6);objdata[10]=elS.toDouble();//mu_ra*cos(dec) [mas/min]
							elS = objDataStr.section('|',7,7);objdata[11]=elS.toDouble();//mu_de [mas/min]
							elS = objDataStr.section('|',8,8);objdata[12]=elS.toDouble();//v_r [km/s]
							elS = objDataStr.section('|',4,4);objdata[13]=elS.toDouble();//phase [deg]
							elS = objDataStr.section('|',5,5);objdata[14]=elS.toDouble();//elongation [deg]
							elS = objDataStr.section('|',9,9);objdata[15]=elS.toDouble();//object number
							//QMessageBox::information(0,"debug",msgstr,QMessageBox::Ok,0,0);
							objects << objdata;
						}
						delete pixdata.Buffer;
						delete actv;
					}
					break;
				}
				case 1://saturnian satellite
				{
					elS = objDataStr.section('|',0,0);ra = hms_to_mas(elS, ":");
					elS = objDataStr.section('|',1,1);de = damas_to_mas(elS, ":");
					xy = getPixPosFromWCS(ra, de, wcs_data);
					//BEGIN find planet
					elS = objDataStr.section('|',7,7);numb = elS.toDouble();
					if (numb==0)
					{
						tp = getRaDeToTang(ra, de, ra_c, de_c);
						planetKsi = tp(0);planetEta = tp(1);
					}
					//END find planet
					if((xy(0)>=0)&&(xy(1)>=0)&&(xy(0)<=naxes[0])&&(xy(1)<=naxes[1])&&(numb!=0))
					{
						//BEGIN fit and subtract gradient
						if(subgrad==1)
						{
							ringdata = getRingArray(fdata, (int)xy(0),(int)xy(1), 
											ringradius, ringradius + ringwidth, naxes[0],naxes[1]);
							PG = fit2(ringdata);
							subgradfit2(fdata, (int)xy(0),(int)xy(1), 
										ringradius, ringradius + ringwidth, naxes[0],naxes[1], PG, fpv);
						}
						delete ringdata.Buffer;
						//END fit and subtract gradient 
						//BEGIN PSF
						pixdata = ApertureData(fdata, (int)xy(0),(int)xy(1), aperture, naxes[0],naxes[1]);
						switch(model)
						{
						 case 0:{NofV = 8;P = StartConditionsL(pixdata, delta);break;}
						 case 1:{NofV = 7;P = StartConditionsG(pixdata);break;}
						 case 2:{NofV = 8;P = StartConditionsM(pixdata, delta); break;}
						}
						actv = new int[NofV];
						for (int j=0;j<NofV;j++) actv[j] = 1;
						PSFr.resize(0);
						PSFr = NLSLevenbergMarquardt(pixdata, P, model, actv, NofV, 10, 10, 1e-7, 1e-6);
						//END PSF
						if(PSFr[0]!=-1)
						{
							objdata = new double[16];
							objdata[0]=ra;//RA from theory of motion or catalogue
							objdata[1]=de;//DEC from theory of motion  or catalogue
							tp = getRaDeToTang(ra, de, ra_c, de_c);
							objdata[2]=tp(0);//tangential position KSI based on RA (C)
							objdata[3]=tp(1);//tangential position ETA based on DE (C)
							objdata[4]=PSFr[5] - wcs_data[0];//measured pixel position X (O)
							objdata[5]=PSFr[6] - wcs_data[1];//measured pixel position Y (O)
							objdata[6]=log10(pixdata.meanADC);//mean value of pixels within aperture
							objdata[7]=PSFr[16];//unut weight error of PSF fitting
							elS = objDataStr.section('|',2,2);objdata[8]=elS.toDouble();//Obj.mag. - satellite magnitude (V)
							elS = objDataStr.section('|',3,3);objdata[9]=elS.toDouble();//Sun-plan[deg] -  Apparent topocentric distance Sun-Planet
							elS = objDataStr.section('|',4,4);objdata[10]=elS.toDouble();//Phase[deg] - phase angle
							elS = objDataStr.section('|',5,5);objdata[11]=elS.toDouble();//Rad.plan. - apparent planetary radius [arcesec]
							elS = objDataStr.section('|',7,7);objdata[12]=elS.toDouble();//satellite number
							dist = sqrt(pow(tp(0)-planetKsi,2)+pow(tp(1)-planetEta,2));
							if((dist>distfromplanet)&&(fabs(tp(1)-planetEta)>distfromsmear))
																			objects << objdata;
						}
						delete pixdata.Buffer;
						delete actv;
					}
					break;
				}
			}
			//QMessageBox::information(0,"debug",objDataStr,QMessageBox::Ok,0,0);
		}
		//QString objDataStr = QString(outerProcess.readAllStandardOutput());
		//QMessageBox::information(0,"debug",objDataStr,QMessageBox::Ok,0,0);
		//END read objects positions
/////////4. Reading data from reference catalogue ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
        //BEGIN load data from catalogue
		outerArguments.clear();
		switch(catindex)
		{
			case 0://ucac2
			{
				outerProcess.setWorkingDirectory(ucac2find_folder);
				outerProcess.setProcessChannelMode(QProcess::MergedChannels);
				outerProcess.setReadChannel(QProcess::StandardOutput);
				outerArguments<<"-r" << mas_to_hms(ra_c, ":", 3)<<mas_to_damas(de_c,":",2)<<fov<<minMag<<maxMag<<"-stdout";
				outerProcess.start(ucac2findn, outerArguments);
				break;
			}
			case 1://2mass
			{
				outerProcess.setWorkingDirectory(cdsfind_folder);
				outerProcess.setProcessChannelMode(QProcess::MergedChannels);
				outerProcess.setReadChannel(QProcess::StandardOutput);
				outerArguments<<"-f"<<"2mass"<<mas_to_hms(ra_c, ":", 3)<<mas_to_damas(de_c,":",2)<<fov<<"-stdout"<<"Bmag="+minMag+".."+maxMag;
				outerProcess.start(cdsfindn, outerArguments);
				break;
			}
		}
		outerProcess.waitForFinished(-1);
		QTextStream catStream(outerProcess.readAllStandardOutput());
		QString catLine;
		QList<double*> refstars;
		double *sdata;
		while (!catStream.atEnd())
		{
			catLine = catStream.readLine();
			//QMessageBox::information(0,"debug",catLine,QMessageBox::Ok,0,0);
			switch(catindex)
			{
				case 0://ucac2
				{
					elS = catLine.section('|',0,0);ra = hms_to_mas(elS, " ");
					elS = catLine.section('|',1,1);de = damas_to_mas(elS, " ");
					elS = catLine.section('|',11,11);pmra = elS.toDouble();
					elS = catLine.section('|',12,12);pmde = elS.toDouble();
					ra = ra +pmra*(getYearFromMJD(mjd)-2000);//taking proper motion into account
					de = de +pmde*(getYearFromMJD(mjd)-2000);//taking proper motion into account
					xy = getPixPosFromWCS(ra, de, wcs_data);
					if((xy(0)>=0)&&(xy(1)>=0)&&(xy(0)<=naxes[0])&&(xy(1)<=naxes[1]))
					{
						//BEGIN fit and subtract gradient
						if(subgrad==1)
						{
							ringdata = getRingArray(fdata, (int)xy(0),(int)xy(1), 
											ringradius, ringradius + ringwidth, naxes[0],naxes[1]);
							PG = fit2(ringdata);
							subgradfit2(fdata, (int)xy(0),(int)xy(1), 
										ringradius, ringradius + ringwidth, naxes[0],naxes[1], PG, fpv);
						}
						delete ringdata.Buffer;
						//END fit and subtract gradient 
						//BEGIN PSF
						pixdata = ApertureData(fdata, (int)xy(0),(int)xy(1), aperture, naxes[0],naxes[1]);
						switch(model)
						{
						 case 0:{NofV = 8;P = StartConditionsL(pixdata, delta);break;}
						 case 1:{NofV = 7;P = StartConditionsG(pixdata);break;}
						 case 2:{NofV = 8;P = StartConditionsM(pixdata, delta); break;}
						}
						actv = new int[NofV];
						for (int j=0;j<NofV;j++) actv[j] = 1;
						PSFr.resize(0);
						PSFr = NLSLevenbergMarquardt(pixdata, P, model, actv, NofV, 10, 10, 1e-7, 1e-6);
						//END PSF
						if(PSFr[0]!=-1)
						{
							sdata = new double[12];
							sdata[0]=ra;//RA from UCAC2
							sdata[1]=de;//DEC from UCAC2
							tp = getRaDeToTang(ra, de, ra_c, de_c);
							sdata[2]=tp(0);//tangential position KSI based on catalogue RA
							sdata[3]=tp(1);//tangential position ETA based on catalogue DE
							sdata[4]=PSFr[5] - wcs_data[0];//measured pixel position X (O)
							sdata[5]=PSFr[6] - wcs_data[1];//measured pixel position Y (O)
							sdata[6]=log10(pixdata.meanADC);//mean value of pixels within aperture
							sdata[7]=PSFr[16];//unut weight error of PSF fitting
							elS = catLine.section('|',2,2);sdata[8]=elS.toDouble();//UCAC2 Rmag
							elS = catLine.section('|',18,18);sdata[9]=elS.toDouble();//Jmag from 2MASS
							elS = catLine.section('|',19,19);sdata[10]=elS.toDouble();//Hmag from 2MASS
							elS = catLine.section('|',20,20);sdata[11]=elS.toDouble();//Ks_mag from 2MASS
							if(objType==1)
							{
								dist = sqrt(pow(tp(0)-planetKsi,2)+pow(tp(1)-planetEta,2));
								if((dist>distfromplanet)&&(fabs(tp(1)-planetEta)>distfromsmear))
												refstars << sdata;
							}
							else refstars << sdata;
						}
						delete pixdata.Buffer;
						delete actv;
					}
					break;
				}
				case 1://2mass
				{
					if ((catLine[0]!='#')||(catLine[0]!='_')||(catLine[0]!='-')||(catLine[0]!='a'))
					{
						elS = catLine.section('|',1,1);ra = elS.toDouble()*3600000;
						elS = catLine.section('|',2,2);de = elS.toDouble()*3600000;
						xy = getPixPosFromWCS(ra, de, wcs_data);
						if((xy(0)>=0)&&(xy(1)>=0)&&(xy(0)<=naxes[0])&&(xy(1)<=naxes[1]))
						{
							//BEGIN fit and subtract gradient
							if(subgrad==1)
							{
								ringdata = getRingArray(fdata, (int)xy(0),(int)xy(1), 
												ringradius, ringradius + ringwidth, naxes[0],naxes[1]);
								PG = fit2(ringdata);
								subgradfit2(fdata, (int)xy(0),(int)xy(1), 
											ringradius, ringradius + ringwidth, naxes[0],naxes[1], PG, fpv);
							}
							delete ringdata.Buffer;
							//END fit and subtract gradient 
							//BEGIN PSF
							pixdata = ApertureData(fdata, (int)xy(0),(int)xy(1), aperture, naxes[0],naxes[1]);
							switch(model)
							{
							 case 0:{NofV = 8;P = StartConditionsL(pixdata, delta);break;}
							 case 1:{NofV = 7;P = StartConditionsG(pixdata);break;}
							 case 2:{NofV = 8;P = StartConditionsM(pixdata, delta); break;}
							}
							actv = new int[NofV];
							for (int j=0;j<NofV;j++) actv[j] = 1;
							PSFr.resize(0);
							PSFr = NLSLevenbergMarquardt(pixdata, P, model, actv, NofV, 10, 10, 1e-7, 1e-6);
							//END PSF
							if(PSFr[0]!=-1)
							{
								sdata = new double[13];
								sdata[0]=ra;//RA from 2MASS
								sdata[1]=de;//DEC from 2MASS
								tp = getRaDeToTang(ra, de, ra_c, de_c);
								sdata[2]=tp(0);//tangential position KSI based on catalogue RA
								sdata[3]=tp(1);//tangential position ETA based on catalogue DE
								sdata[4]=PSFr[5] - wcs_data[0];//measured pixel position X (O)
								sdata[5]=PSFr[6] - wcs_data[1];//measured pixel position Y (O)
								sdata[6]=log10(pixdata.meanADC);//mean value of pixels within aperture
								sdata[7]=PSFr[16];//unut weight error of PSF fitting
								elS = catLine.section('|',54,54);sdata[8] = elS.toDouble();//Bmag from USNO A2.0
								elS = catLine.section('|',55,55);sdata[9] = elS.toDouble();//Rmag from USNO A2.0
								elS = catLine.section('|',7,7);sdata[10] = elS.toDouble();//Jmag
								elS = catLine.section('|',11,11);sdata[11] = elS.toDouble();//Hmag
								elS = catLine.section('|',15,15);sdata[12] = elS.toDouble();//Ks_mag
								if(objType==1)
								{
									dist = sqrt(pow(tp(0)-planetKsi,2)+pow(tp(1)-planetEta,2));
									if((dist>distfromplanet)&&(fabs(tp(1)-planetEta)>distfromsmear))
													refstars << sdata;
								}
							else refstars << sdata;
							}
							delete pixdata.Buffer;
							delete actv;
						}
					}
					break;
				}
		}
		}
		//END load data from catalogue
/////////5. Reduction procedures ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//BEGIN reduction
		QVector<int> rsindex;
		for (int i=0;i<refstars.count();i++) 
		{
			sdata = refstars.at(i);
			if((sdata[8]>=minrefMag)&&(sdata[8]<=maxrefMag)) rsindex << i;
		}
		vector<double>KSI(rsindex.count());
		vector<double>ETA(rsindex.count());
		vector<double>MAG(rsindex.count());
		vector<double>Weights(rsindex.count());
		matrix<double>C(rsindex.count(),3);
		matrix<double>CMAG(rsindex.count(),2);
		for (int i=0;i<rsindex.count();i++)
		{
			Weights(i)=1;
			sdata = refstars.at(rsindex[i]);
			KSI(i) = sdata[2];
			ETA(i) = sdata[3];
			MAG(i) = sdata[8];
			C(i,0) = sdata[4];C(i,1) = sdata[5];C(i,2) = 1;
			CMAG(i,0) = sdata[6];CMAG(i,1) = 1;
		}
		matrix<double> ZKSI = LSM_corr(KSI, C, Weights, maxres/1000);
		matrix<double> ZETA = LSM_corr(ETA, C, Weights, maxres/1000);
		matrix<double> ZMAG = LSM_corr(MAG, CMAG, Weights, maxresMAG);
		//END reduction
/////////6. Calculation of objects positions//////////////////////////////////////////////////////////////////////////////////////////////////
		//BEGIN of calculation of objects positions
		QList<double*> outlist;
		double *outdata;
		//double *objdata;
		//QList<double*> objects;
		double x,y,pixmag,ksi,eta;
		vector<double> cp(2);
		for (int i=0;i<objects.count();i++)
		{
			objdata = objects.at(i);
			x = objdata[4];
			y = objdata[5];
			pixmag = objdata[6];
			ksi = ZKSI(1,0)*x+ZKSI(2,0)*y+ZKSI(3,0);
			eta = ZETA(1,0)*x+ZETA(2,0)*y+ZETA(3,0);
			//BEGIN add systematic corrections
			if (syscorr==1)
			{
				ksi = ksi - syscorr_ra(objdata[8])/1000;
				eta = eta - syscorr_de(objdata[8])/1000;
			}
			//END add systematic corrections
			cp = getTangToRaDe(ksi,eta,ra_c,de_c);
			mag = ZMAG(1,0)*pixmag+ZMAG(2,0);//syscorr_mag
			//BEGIN add systematic corrections to mag
			if (syscorr==1)
			{
				mag = mag - syscorr_mag(objdata[8]);
			}
			//END add systematic corrections  to mag
			//
			outdata = new double[22];
			outdata[0] = mjd;//MJD
			outdata[1] = cp(0);//RA(O)
			outdata[2] = cp(1);//DEC(O)
			outdata[3] = mag;//MAG(O)
			outdata[4] = ksi - objdata[2];//O-C in RA
			outdata[5] = eta - objdata[3];//O-C in DEC
			outdata[6] = mag - objdata[8];//O-C in MAG
			
			outdata[20] = ra_c;//RA optical center
			outdata[21] = de_c;//DEC optical center
			//
			switch(objType)
			{
				case 0:
				{
					outdata[7] = objdata[9];//topocentric distance [a.e.]
					outdata[8] = objdata[10];//mu_ra*cos(dec) [mas/min]
					outdata[9] = objdata[11];//mu_de [mas/min]
					outdata[10] = objdata[12];//v_r [km/s]
					outdata[11] = objdata[13];//phase [deg]
					outdata[12] = objdata[14];//elongation [deg]
					outdata[13] = objdata[15];//number of the asteroid
					outdata[14] = objdata[2];//ksi(c)
					outdata[15] = objdata[3];//eta(c)
					outdata[16] = ksi;//ksi(o)
					outdata[17] = eta;//eta(o)
					outdata[18] = objdata[8];//mag(c)
					break;
				}
				case 1:
				{
					outdata[7] = objdata[9];//Sun-plan[deg]
					outdata[8] = objdata[10];//Phase[deg]
					outdata[9] = objdata[11];//Rad.plan. - apparent planetary radius [arcesec]
					outdata[10] = objdata[12];//sat number
					outdata[11] = objdata[8];//mag
					outdata[12] = objdata[2] - planetKsi;//planetocentric position in Ksi
					outdata[13] = objdata[3] - planetEta;//planetocentric position in Eta
					outdata[14] = sqrt(pow(objdata[2] - planetKsi,2)+pow(objdata[3] - planetEta,2));//angular distance from planet
					outdata[15] = ksi;//ksi(o)
					outdata[16] = eta;//eta(o)
					break;
				}
			}
			//
			if((fabs(mag - objdata[8])<=maxOCMAG)&&
				(1000*fabs(ksi - objdata[2])<=maxOCRA)&&
				(1000*fabs(eta - objdata[3])<=maxOCDE)) outlist << outdata;
		}
		//END of calculation of objects positions
/////////7. OUTPUT of the results to file(s) //////////////////////////////////////////////////////////////////////////////////////////////////
		//BEGIN output
		DATEOBS dateObs;
		QString outstr;
		double tTm;
		//BEGIN astreport
		if((astreport==1)&&(outlist.count()!=0))
		{
			for (int i=0;i<outlist.count();i++)
			{
				outdata = outlist.at(i);
				outstr ="";
				tTm = outdata[0];
				switch(time_form)
					{
						 case 0:{dateObs = getDATEOBSfromMJD(outdata[0]); outstr = outstr + getStrFromDATEOBS(dateObs," ", 0, 5)+sepsymb; break;}
						 case 1:{outstr = outstr + QString( "%1" ).arg(outdata[0],17,'f',10,QLatin1Char( ' ' ))+sepsymb; break;}
						 case 2:{outstr = outstr + QString( "%1" ).arg( (mjd-mjd0)*1440,8,'f',3,QLatin1Char( ' ' ))+sepsymb; break;}
					}
				switch(pos_form)
					{
						case 0:{outstr = outstr + mas_to_hms(outdata[1], " ", 4)+ sepsymb + mas_to_damas(outdata[2], " ", 3)+sepsymb;break;}
						case 1:{outstr = outstr + QString( "%1" ).arg( outdata[1],12,'f',1,QLatin1Char( ' ' ))+ sepsymb + QString( "%1" ).arg( outdata[2],12,'f',1,QLatin1Char( ' ' ))+sepsymb;break;}
						case 2:{outstr = outstr + QString( "%1" ).arg( outdata[1]/3600000,12,'f',8,QLatin1Char( ' ' ))+ sepsymb + QString( "%1" ).arg( outdata[2]/3600000,12,'f',8,QLatin1Char( ' ' ))+sepsymb;break;}
					}
				outstr = outstr + QString( "%1" ).arg(outdata[3],6,'f',3,QLatin1Char( ' ' ))+sepsymb+
						QString( "%1" ).arg(outdata[4]*1000,8,'f',1,QLatin1Char( ' ' ))+sepsymb+
						QString( "%1" ).arg(outdata[5]*1000,8,'f',1,QLatin1Char( ' ' ))+sepsymb+
						QString( "%1" ).arg(outdata[6],8,'f',1,QLatin1Char( ' ' ))+sepsymb;
						
				switch(pos_form)
				{
					case 0:{outstr = outstr + mas_to_hms(outdata[20], " ", 4)+ sepsymb + mas_to_damas(outdata[21], " ", 3)+sepsymb;break;}
					case 1:{outstr = outstr + QString( "%1" ).arg( outdata[20],12,'f',1,QLatin1Char( ' ' ))+ sepsymb + QString( "%1" ).arg( outdata[21],12,'f',1,QLatin1Char( ' ' ))+sepsymb;break;}
					case 2:{outstr = outstr + QString( "%1" ).arg( outdata[20]/3600000,12,'f',8,QLatin1Char( ' ' ))+ sepsymb + QString( "%1" ).arg( outdata[21]/3600000,12,'f',8,QLatin1Char( ' ' ))+sepsymb;break;}
				}
				//
				switch(objType)
				{
					case 0://asteroids
					{
						outstr = outstr + 
						QString( "%1" ).arg(outdata[7],12,'f',9,QLatin1Char( ' ' ))+sepsymb+//topocentric distance [a.e.]
						QString( "%1" ).arg(outdata[8],9,'f',2,QLatin1Char( ' ' ))+sepsymb+//mu_ra*cos(dec) [mas/min]
						QString( "%1" ).arg(outdata[9],9,'f',2,QLatin1Char( ' ' ))+sepsymb+//mu_de [mas/min]
						QString( "%1" ).arg(outdata[10],6,'f',2,QLatin1Char( ' ' ))+sepsymb+//v_r [km/s]
						QString( "%1" ).arg(outdata[11],8,'f',3,QLatin1Char( ' ' ))+sepsymb+//phase [deg]
						QString( "%1" ).arg(outdata[12],8,'f',3,QLatin1Char( ' ' ))+sepsymb;//elongation [deg]
						msgstr = QString( "%1" ).arg((int)outdata[13],5,10,QLatin1Char( '0' ));
						msgstr = msgstr.trimmed();
						outstr = outstr + msgstr + sepsymb + QString( "%1" ).arg(catindex,1,10,QLatin1Char( ' ' ))+sepsymb+
						QString( "%1" ).arg(exp_time,8,'f',3,QLatin1Char( ' ' ))+"\n";
						break;
					}
					case 1://saturnian satellites
					{
						outstr = outstr + 
						QString( "%1" ).arg(outdata[11],6,'f',2,QLatin1Char( ' ' ))+sepsymb+//mag 
						QString( "%1" ).arg(outdata[7],8,'f',3,QLatin1Char( ' ' ))+sepsymb+//Sun-plan[deg]
						QString( "%1" ).arg(outdata[8],8,'f',3,QLatin1Char( ' ' ))+sepsymb+//Phase[deg]
						QString( "%1" ).arg(outdata[9],6,'f',3,QLatin1Char( ' ' ))+sepsymb+//Rad.plan. - apparent planetary radius [arcesec]
						QString( "%1" ).arg(outdata[12],8,'f',3,QLatin1Char( ' ' ))+sepsymb+//Ksi referred to planet
						QString( "%1" ).arg(outdata[13],8,'f',3,QLatin1Char( ' ' ))+sepsymb+//Eta referred to planet
						QString( "%1" ).arg(outdata[14],8,'f',3,QLatin1Char( ' ' ))+sepsymb+//angular distance from planet
						QString( "%1" ).arg((int)outdata[10],3,10,QLatin1Char( '0' ));//satellite number
						msgstr = "S"+QString( "%1" ).arg((int)outdata[10],3,10,QLatin1Char( '0' ));
						msgstr = msgstr.trimmed();
						outstr = outstr  + sepsymb + QString( "%1" ).arg(catindex,1,10,QLatin1Char( ' ' ))+sepsymb+
						QString( "%1" ).arg(exp_time,8,'f',3,QLatin1Char( ' ' ))+"\n";
						break;
					}
				}
				///////////////////////////////////////////////////
//				QFile astFile(outfolder+"/"+msgstr+"_eq.txt");ObjNum
				QFile astFile(outfolder+"/"+QString("%1").arg(ObjName, 7,  QLatin1Char('0'))+"_eq.txt");
				astFile.open(QIODevice::Append| QIODevice::Text);
				QTextStream astStream;
				astStream.setDevice(&astFile);
				astStream << outstr;
				astFile.close();
			}
		}
		//END astreport
		//BEGIN relative positions of satellites 
		if((relativepos==1)&&(outlist.count()>1))
		{
			double *outdata1;
			double *outdata2;
			for (int i=0;i<outlist.count();i++)
			{
				for (int j=i+1;j<outlist.count();j++)
				{
					outdata1 = outlist.at(i);
					outdata2 = outlist.at(j);
					outstr ="";
					switch(time_form)
						{
							 case 0:{dateObs = getDATEOBSfromMJD(outdata1[0]); outstr = outstr + getStrFromDATEOBS(dateObs," ", 0, 5)+sepsymb; break;}
							 case 1:{outstr = outstr + QString( "%1" ).arg(outdata1[0],17,'f',10,QLatin1Char( ' ' ))+sepsymb; break;}
							 case 2:{outstr = outstr + QString( "%1" ).arg( (mjd-mjd0)*1440,8,'f',3,QLatin1Char( ' ' ))+sepsymb; break;}
						}
					switch(objType)
					{
						case 0://asteroids
						{
							outstr = outstr + 
							QString( "%1" ).arg(outdata2[16]-outdata1[16],8,'f',3,QLatin1Char( ' ' ))+sepsymb+//delta ksi (o) [arcsec]
							QString( "%1" ).arg(outdata2[17]-outdata1[17],8,'f',3,QLatin1Char( ' ' ))+sepsymb+//delta eta (o) [arcsec]
							QString( "%1" ).arg(outdata2[3]-outdata1[3],8,'f',3,QLatin1Char( ' ' ))+sepsymb+//delta mag
							QString( "%1" ).arg(((outdata2[16]-outdata1[16])-(outdata2[14]-outdata1[14]))*1000,8,'f',3,QLatin1Char( ' ' ))+sepsymb+//(o-c) ksi [mas]
							QString( "%1" ).arg(((outdata2[17]-outdata1[17])-(outdata2[15]-outdata1[15]))*1000,8,'f',3,QLatin1Char( ' ' ))+sepsymb+//o-c) eta [mas]
							QString( "%1" ).arg((outdata2[3]-outdata1[3])-(outdata2[18]-outdata1[18]),8,'f',3,QLatin1Char( ' ' ))+sepsymb+//o-c)  mag
							QString( "%1" ).arg((int)outdata2[13],3,10,QLatin1Char( '0' ))+"-"+
							QString( "%1" ).arg((int)outdata1[13],3,10,QLatin1Char( '0' ));//satellite numbers
							outstr = outstr  + sepsymb + QString( "%1" ).arg(catindex,1,10,QLatin1Char( ' ' ))+sepsymb+
							QString( "%1" ).arg(exp_time,8,'f',3,QLatin1Char( ' ' ))+"\n";
							msgstr = QString( "%1" ).arg((int)outdata2[13],3,10,QLatin1Char( '0' ))+"-"+
							QString( "%1" ).arg((int)outdata1[13],3,10,QLatin1Char( '0' ));
							msgstr = msgstr.trimmed();
							break;
						}
						case 1://saturnian satellites
						{
							outstr = outstr + 
							QString( "%1" ).arg(outdata2[15]-outdata1[15],8,'f',3,QLatin1Char( ' ' ))+sepsymb+//delta ksi (o) [arcsec]
							QString( "%1" ).arg(outdata2[16]-outdata1[16],8,'f',3,QLatin1Char( ' ' ))+sepsymb+//delta eta (o) [arcsec]
							QString( "%1" ).arg(outdata2[3]-outdata1[3],8,'f',3,QLatin1Char( ' ' ))+sepsymb+//delta mag
							QString( "%1" ).arg(((outdata2[15]-outdata1[15])-(outdata2[12]-outdata1[12]))*1000,8,'f',3,QLatin1Char( ' ' ))+sepsymb+//(o-c) ksi [mas]
							QString( "%1" ).arg(((outdata2[16]-outdata1[16])-(outdata2[13]-outdata1[13]))*1000,8,'f',3,QLatin1Char( ' ' ))+sepsymb+//o-c) eta [mas]
							QString( "%1" ).arg((outdata2[3]-outdata1[3])-(outdata2[11]-outdata1[11]),8,'f',3,QLatin1Char( ' ' ))+sepsymb+//o-c)  mag
							QString( "%1" ).arg((int)outdata2[10],3,10,QLatin1Char( '0' ))+"-"+
							QString( "%1" ).arg((int)outdata1[10],3,10,QLatin1Char( '0' ));//satellite numbers
							outstr = outstr  + sepsymb + QString( "%1" ).arg(catindex,1,10,QLatin1Char( ' ' ))+sepsymb+
							QString( "%1" ).arg(exp_time,8,'f',3,QLatin1Char( ' ' ))+"\n";
							msgstr = "S"+QString( "%1" ).arg((int)outdata2[10],3,10,QLatin1Char( '0' ))+"-S"+
							QString( "%1" ).arg((int)outdata1[10],3,10,QLatin1Char( '0' ));
							msgstr = msgstr.trimmed();
							break;
						}
					}
					///////////////////////////////////////////////////
					QFile relFile(outfolder+"/"+msgstr+"_rel.txt");
					relFile.open(QIODevice::Append| QIODevice::Text);
					QTextStream relStream;
					relStream.setDevice(&relFile);
					relStream << outstr;
					relFile.close();
				}
			}
		}
		//END relative positions of satellites
		//BEGIN errorreport
		if(errorreport)
		{
			outstr = QString( "%1" ).arg((int)ZKSI(0,0),3,10,QLatin1Char( ' ' ))+sepsymb+
					QString( "%1" ).arg(ZKSI(0,1)*1000,6,'f',1,QLatin1Char( ' ' ))+sepsymb+
					QString( "%1" ).arg(ZKSI(1,0),10,'f',5,QLatin1Char( ' ' ))+sepsymb+
					QString( "%1" ).arg(ZKSI(1,1),10,'f',5,QLatin1Char( ' ' ))+sepsymb+
					QString( "%1" ).arg(ZKSI(2,0),10,'f',5,QLatin1Char( ' ' ))+sepsymb+
					QString( "%1" ).arg(ZKSI(2,1),10,'f',5,QLatin1Char( ' ' ))+sepsymb+
					QString( "%1" ).arg(ZKSI(3,0),10,'f',5,QLatin1Char( ' ' ))+sepsymb+
					QString( "%1" ).arg(ZKSI(3,1),10,'f',5,QLatin1Char( ' ' ))+sepsymb;
			outstr = outstr + QString( "%1" ).arg((int)ZETA(0,0),3,10,QLatin1Char( ' ' ))+sepsymb+
					QString( "%1" ).arg(ZETA(0,1)*1000,6,'f',1,QLatin1Char( ' ' ))+sepsymb+
					QString( "%1" ).arg(ZETA(1,0),10,'f',5,QLatin1Char( ' ' ))+sepsymb+
					QString( "%1" ).arg(ZETA(1,1),10,'f',5,QLatin1Char( ' ' ))+sepsymb+
					QString( "%1" ).arg(ZETA(2,0),10,'f',5,QLatin1Char( ' ' ))+sepsymb+
					QString( "%1" ).arg(ZETA(2,1),10,'f',5,QLatin1Char( ' ' ))+sepsymb+
					QString( "%1" ).arg(ZETA(3,0),10,'f',5,QLatin1Char( ' ' ))+sepsymb+
					QString( "%1" ).arg(ZETA(3,1),10,'f',5,QLatin1Char( ' ' ))+sepsymb;
			outstr = outstr + QString( "%1" ).arg((int)ZMAG(0,0),3,10,QLatin1Char( ' ' ))+sepsymb+
					QString( "%1" ).arg(ZMAG(0,1),5,'f',2,QLatin1Char( ' ' ))+sepsymb+
					QString( "%1" ).arg(ZMAG(1,0),10,'f',5,QLatin1Char( ' ' ))+sepsymb+
					QString( "%1" ).arg(ZMAG(1,1),10,'f',5,QLatin1Char( ' ' ))+sepsymb+
					QString( "%1" ).arg(ZMAG(2,0),10,'f',5,QLatin1Char( ' ' ))+sepsymb+
					QString( "%1" ).arg(ZMAG(2,1),10,'f',5,QLatin1Char( ' ' ))+"\n";
			QFile errFile(outfolder+"/err_budget.txt");
			errFile.open(QIODevice::Append| QIODevice::Text);
			QTextStream errStream;
			errStream.setDevice(&errFile);
			errStream << outstr;
			errFile.close();
		}			
		//END astreport
		//BEGIN residuals
		if(residuals)
		{
			outstr = "";
			double a1 = ZKSI(1,0);
			double b1 = ZKSI(2,0);
			double c1 = ZKSI(3,0);
			double a2 = ZETA(1,0);
			double b2 = ZETA(2,0);
			double c2 = ZETA(3,0);
			double am = ZMAG(1,0);
			double bm = ZMAG(2,0);
			double xc,yc,pmag;
			for (int i=0;i<refstars.count();i++)
			{
				sdata = refstars.at(i);
				x = sdata[4];
				y = sdata[5];
				pixmag = sdata[6];
				ksi = a1*x+b1*y+c1;
				eta = a2*x+b2*y+c2;
				//BEGIN add systematic corrections
				if (syscorr==1)
				{
					ksi = ksi - syscorr_ra(sdata[8])/1000;
					eta = eta - syscorr_de(sdata[8])/1000;
				}
				//END add systematic corrections
				mag = am*pixmag+bm;
				//BEGIN add systematic corrections to mag
				if (syscorr==1)
				{
					mag = mag - syscorr_mag(sdata[8]);
				}
				//END add systematic corrections  to mag
				if((fabs(mag - sdata[8])<=maxOCMAGrc)&&(1000*fabs(ksi - sdata[2])<=maxOCRArc)&&(1000*fabs(eta - sdata[3])<=maxOCDErc))
				{
					yc = ((sdata[3]-c2)-a2*(sdata[2]-c1)/a1)/(b2-a2*b1/a1);
					xc = ((sdata[2]-c1)-b1*yc)/a1;
					pmag = (sdata[8]-bm)/am;
					outstr = outstr + QString( "%1" ).arg(mjd, 11, 'f', 3, QLatin1Char(' '))+sepsymb+ 
									  QString( "%1" ).arg(sdata[0]/3600000,12,'f',8,QLatin1Char( ' ' ))+sepsymb+
									  QString( "%1" ).arg(sdata[1]/3600000,12,'f',8,QLatin1Char( ' ' ))+sepsymb+
									  QString( "%1" ).arg(ksi,10,'f',4,QLatin1Char( ' ' ))+sepsymb+
									  QString( "%1" ).arg(eta,10,'f',4,QLatin1Char( ' ' ))+sepsymb+
									  QString( "%1" ).arg(mag,5,'f',2,QLatin1Char( ' ' ))+sepsymb+
									  QString( "%1" ).arg((ksi - sdata[2])*1000,7,'f',1,QLatin1Char( ' ' ))+sepsymb+
									  QString( "%1" ).arg((eta - sdata[3])*1000,7,'f',1,QLatin1Char( ' ' ))+sepsymb+
									  QString( "%1" ).arg(mag - sdata[8],5,'f',2,QLatin1Char( ' ' ))+sepsymb+
									  QString( "%1" ).arg(x,10,'f',4,QLatin1Char( ' ' ))+sepsymb+
									  QString( "%1" ).arg(y,10,'f',4,QLatin1Char( ' ' ))+sepsymb+
									  QString( "%1" ).arg(pixmag,10,'f',6,QLatin1Char( ' ' ))+sepsymb+
									  QString( "%1" ).arg((x - xc)*1000,7,'f',1,QLatin1Char( ' ' ))+sepsymb+
									  QString( "%1" ).arg((y - yc)*1000,7,'f',1,QLatin1Char( ' ' ))+sepsymb+
									  QString( "%1" ).arg(pixmag - pmag,9,'f',6,QLatin1Char( ' ' ))+sepsymb;
					switch(catindex)
					{
						 case 0:
						 {
							outstr = outstr + QString( "%1" ).arg(sdata[8],6,'f',3,QLatin1Char( ' ' ))+sepsymb+
											  QString( "%1" ).arg(sdata[9],6,'f',3,QLatin1Char( ' ' ))+sepsymb+
											  QString( "%1" ).arg(sdata[10],6,'f',3,QLatin1Char( ' ' ))+sepsymb+
											  QString( "%1" ).arg(sdata[11],6,'f',3,QLatin1Char( ' ' ))+sepsymb+
											  QString( "%1" ).arg(sdata[8]-sdata[11],7,'f',3,QLatin1Char( ' ' ))+sepsymb+
											  QString( "%1" ).arg(catindex,1,10,QLatin1Char( ' ' ))+sepsymb+
											  QString( "%1" ).arg(exp_time,8,'f',3,QLatin1Char( ' ' ))+"\n";
							break;
						 }
						 case 1:
						 {
							outstr = outstr + QString( "%1" ).arg(sdata[8],4,'f',1,QLatin1Char( ' ' ))+sepsymb+
											  QString( "%1" ).arg(sdata[9],4,'f',1,QLatin1Char( ' ' ))+sepsymb+
											  QString( "%1" ).arg(sdata[10],6,'f',3,QLatin1Char( ' ' ))+sepsymb+
											  QString( "%1" ).arg(sdata[11],6,'f',3,QLatin1Char( ' ' ))+sepsymb+
											  QString( "%1" ).arg(sdata[12],6,'f',3,QLatin1Char( ' ' ))+sepsymb+
											  QString( "%1" ).arg(sdata[8]-sdata[12],7,'f',3,QLatin1Char( ' ' ))+sepsymb+
											  QString( "%1" ).arg(catindex,1,10,QLatin1Char( ' ' ))+sepsymb+
											  QString( "%1" ).arg(exp_time,8,'f',3,QLatin1Char( ' ' ))+"\n";
							break;
						 }
					}
				}
			}
			QFile resFile(outfolder+"/residuals.txt");
			resFile.open(QIODevice::Append| QIODevice::Text);
			QTextStream resStream;
			resStream.setDevice(&resFile);
			resStream << outstr;
			resFile.close();
		}
		//END residuals
		//BEGIN MPC report
		if((objType==0)&&(mpcreport==1))
		{
			outstr ="";
			int obj_num;
			for (int i=0;i<outlist.count();i++)
			{
				outdata = outlist.at(i);
				dateObs = getDATEOBSfromMJD(outdata[0]);
				obj_num = (int)outdata[13];
				outstr = outstr +QString( "%1" ).arg( obj_num,5,10,QLatin1Char( '0' ))+"         C";
				outstr = outstr +getStrFromDATEOBS(dateObs," ", 0, 5) +" ";
			    outstr = outstr +mas_to_hms(outdata[1], " ",3);
				outstr = outstr +mas_to_damas(outdata[2], " ",2)+"         ";
				outstr = outstr  +QString( "%1" ).arg( outdata[3],3,'f',1,QLatin1Char( '0' ));
				outstr = outstr  +"        "+obscode+"\n";
				msgstr = QString( "%1" ).arg(obj_num,5,10,QLatin1Char( '0' ));
				msgstr = msgstr.trimmed();
				//QMessageBox::information(0,"debug",outfolder+"/"+msgstr+"_mpc.txt",QMessageBox::Ok,0,0);
//				QFile mpcFile(outfolder+"/"+msgstr+"_mpc.txt");
				QFile mpcFile(outfolder+"/"+QString("%1").arg(ObjName, 7,  QLatin1Char('0'))+"_mpc.txt");
				mpcFile.open(QIODevice::Append| QIODevice::Text);
				QTextStream mpcStream;
				mpcStream.setDevice(&mpcFile);
				mpcStream << outstr;
				mpcFile.close();
			}
		}
		//END MPC report
		//END output
/////////HAPPY END////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if(endmessage==1) QMessageBox::information(0,"debug","calculations have been finished",QMessageBox::Ok,0,0);
		return 0;  
	}
