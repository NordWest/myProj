#include <QtCore>
#include <QtGui>
#include "ccdframe.h"
#include "ccdmeasurements.h"
#include "fitsio.h"
#include "math.h"
#include ".\..\libs\astro.h"

CCDFrame::CCDFrame(PIMGF imgf, QVector<double> *ResVect, QProgressBar *pb)
{
 pimgf = imgf;
 mRES = ResVect;
 progressBar = pb;
 pl_index = -1;
 pimgf ->frIndex = -1;
 pimgf -> measured_file_name = "";
 pimgf -> inv = 0;
 //CCDdata = new quint16;
 //CCDImage = new uchar;
 //hist = new quint32;
 //naxes = new long[2];
};
CCDFrame::~CCDFrame()
{
   //if (CCDdata) delete CCDdata;
   //if (CCDImage) delete CCDImage;
   //if (naxes) delete naxes;
   //if (hist) delete hist;
};

void CCDFrame::LoadFromFITS(QString FName)
{
 
 /*if (CCDdata) delete CCDdata;
 if (CCDImage) delete CCDImage;
 if (naxes) delete naxes;
 if (hist) delete hist;
 if (fCCDdata) delete fCCDdata;*/
 FileName = FName;
 QByteArray fn = FileName.toAscii();
 char *fname = fn.data();
 fitsfile *fptr;
 status = 0;//status
 fits_open_file(&fptr, fname , READONLY,&status);
 //
 
 fits_get_img_equivtype(fptr, &bpix, &status);
 naxes = new long[2];
 fits_get_img_size(fptr, 2, naxes, &status);
 nelements = naxes[0]*naxes[1];
 //
 QString str, strl;
 str.setNum(bpix);
 //QMessageBox::information(0,"debug",str,QMessageBox::Ok,0,0);
 ////////////
 char *card = new char[FLEN_CARD];
 int nkeys;
 fits_get_hdrspace(fptr, &nkeys, NULL, &status);
 str = "";
 for (int k = 1; k <= nkeys; k++)
 {
   fits_read_record(fptr, k, card, &status);
   strl = QString::QString(card);
 //QMessageBox::information(0,"debug",str,QMessageBox::Ok,0,0);
   //str.append(card);
    str = str + strl + "\n";
 }
 //
 if (nkeys==10)
 {
  pimgf->isOldNA = true;
  fits_read_record(fptr, 8, card, &status);
  QString strf=QString(card);
  strf = strf.mid(10,7);
  exp_time = strf.toDouble();
  fits_read_record(fptr, 9, card, &status);
  date_obs_str=QString(card);
  date_obs = getMJDfromStrNA(date_obs_str, exp_time);
  pimgf->curr_mjd = date_obs;
  pimgf->ra_oc = 0;
  pimgf->de_oc = 0;
 }
 else
 {
 pimgf->isOldNA = false;
 char value[80];
 fits_read_key(fptr, TSTRING, "DATE-OBS", value, NULL, &status);
 date_obs_str=QString(value);
 //fits_read_key(fptr, TDOUBLE, "MJD", &date_obs, NULL, &status);
 fits_read_key(fptr, TDOUBLE, "EXPTIME", &exp_time, NULL, &status);
 //date_obs = date_obs + exp_time/(2*86400);
 date_obs = getMJDfromStrFTN(date_obs_str, exp_time);
 pimgf->curr_mjd = date_obs;
 ////////////////////////////
 fits_read_key(fptr, TSTRING, "RA", value, NULL, &status);
 QString pos_str=QString(value);
 pimgf->ra_oc = hms_to_mas(pos_str,":");
 fits_read_key(fptr, TSTRING, "DEC", value, NULL, &status);
 pos_str=QString(value);
 pimgf->de_oc = damas_to_mas(pos_str,":");
 //////////////////////////////
 }
 delete card;
 str = str + "\n MJD (central moment) = " + QString( "%1" ).arg( date_obs,20,'f',10,QLatin1Char( ' ' ));
 //QMessageBox::information(0,"debug",date_obs_str,QMessageBox::Ok,0,0);
 //str.setNum(date_obs);
 //str = QString( "%1" ).arg( date_obs,20,'f',10,QLatin1Char( ' ' ));
 //QMessageBox::information(0,"debug",str,QMessageBox::Ok,0,0);
 
 int wa32 = naxes[0] + (4*(naxes[0]%4!=0)-naxes[0]%4);
 //str.setNum(naxes[0]);
 //QMessageBox::information(0,"debug",str,QMessageBox::Ok,0,0);
 long *fpixel = new long[2];
 fpixel[0] = 1;
 fpixel[1] = 1;
 long felem = 1;
 int anynul = 0;
 ///////////////////////////////////
 
 CCDImage = new uchar[wa32*naxes[1]];
 fCCDdata = new double[nelements];
 quint32 i;
 ////////////////////////////
 if (pimgf->isOldNA)
 {
   unsigned short *fr_data = new unsigned short[nelements];
   fits_read_img(fptr, TUSHORT, felem, nelements, NULL, fr_data ,&anynul, &status);
   //
   quint32 idx1,idx2;
   for (int x=0;x<naxes[0];x++)
   {
      for (int y=0;y<naxes[1];y++)
	  {
	    idx1 = y*naxes[0] + x;
		idx2 = x*naxes[1] + y;
		fCCDdata[idx2] = fr_data[idx1];
	  }
   }
   //
   delete fr_data;
   XSize = (quint16)naxes[1];
   YSize = (quint16)naxes[0];
   naxes[0] = XSize;
   naxes[1] = YSize;
 }
 else
 {
  if (bpix==USHORT_IMG)  
  { //QMessageBox::information(0,"debug","ushort_img",QMessageBox::Ok,0,0);
    status = 0;
    unsigned short *fdata_16 = new unsigned short[nelements];
    fits_read_img(fptr, TUSHORT, felem, nelements, NULL, fdata_16 ,&anynul, &status);
	//str.setNum(status);
    //QMessageBox::information(0,"debug",str,QMessageBox::Ok,0,0);
	for (i=0;i<nelements;i++)fCCDdata[i] = fdata_16[i];
	delete fdata_16;
  }
  if (bpix==SHORT_IMG)  
  { //QMessageBox::information(0,"debug","short_img",QMessageBox::Ok,0,0);
    status = 0;
    short *fdata16 = new short[nelements];
    fits_read_img(fptr, TSHORT, felem, nelements, NULL, fdata16 ,&anynul, &status);
	for (i=0;i<nelements;i++)fCCDdata[i] = fdata16[i];
	delete fdata16;
  }  
  if (bpix==LONG_IMG)
  { //QMessageBox::information(0,"debug","long_img",QMessageBox::Ok,0,0);
    status = 0;
    long *fdata32 = new long[nelements];
    fits_read_img(fptr, TLONG, felem, nelements, NULL, fdata32 ,&anynul, &status);
	for (i=0;i<nelements;i++)fCCDdata[i] = fdata32[i];
	delete fdata32;
  }
  if (bpix==FLOAT_IMG)
  { //QMessageBox::information(0,"debug","float_img",QMessageBox::Ok,0,0);
    status = 0;
    float *fldata = new float[nelements];
    fits_read_img(fptr, TFLOAT, felem, nelements, NULL, fldata ,&anynul, &status);
	for (i=0;i<nelements;i++)fCCDdata[i] = fldata[i];
	delete fldata;
  }
  if (bpix==DOUBLE_IMG)
  { //QMessageBox::information(0,"debug","double_img",QMessageBox::Ok,0,0);
    status = 0;
    double *fl_data = new double[nelements];
    fits_read_img(fptr, TDOUBLE, felem, nelements, NULL, fl_data ,&anynul, &status);
	for (i=0;i<nelements;i++)fCCDdata[i] = fl_data[i];
	delete fl_data;
  }
  XSize = (quint16)naxes[0];
  YSize = (quint16)naxes[1];
 }
 fits_close_file(fptr, &status); //if previous file has not closed then close it
 ///////////
 //////////////////////////////////////////////
 getRange();
 //QMessageBox::information(0,"debug","ok",QMessageBox::Ok,0,0);
 getHist();
 getHistPr();
 
 setImgData(histL, histR,1,pimgf->inv);
 
 pimgf->xSize = XSize;
 pimgf->ySize = YSize; 
 features = new double[8];
 features[0] = MaxADC; features[1] = MinADC; features[2] = MeanADC;
 features[3] = histL; features[4] = histR; features[5] = histMax;
 features[6] = borderL; features[7] = borderR;
 FNList << FileName;
 keyList << str;
 expList << exp_time;
 mjdList << date_obs;
 pl_naxes << naxes;
 pl_CCDImage << CCDImage;
 //pl_CCDdata << CCDdata;
 pl_fCCDdata << fCCDdata;
 pl_hist << hist;
 pl_features << features;
 pl_index++;
 pimgf ->frIndex = pl_index;
 pimgf->num_of_frames = FNList.count();
 pimgf->keyStr = str;
 pimgf->gamma = 1;
 gammaList << 1;
 raList << pimgf->ra_oc;
 deList << pimgf->de_oc;
 invList << pimgf->inv;
};

void CCDFrame::LoadFromFITS_ex(QString FName, QString FNameEx, int rate)
{
 int n, m;
 /*if (CCDdata) delete CCDdata;
 if (CCDImage) delete CCDImage;
 if (naxes) delete naxes;
 if (hist) delete hist;
 if (fCCDdata) delete fCCDdata;*/
 FileName = FName;
 QByteArray fn = FileName.toAscii();
 char *fname = fn.data();
 fitsfile *fptr;
 status = 0;//status
 fits_open_file(&fptr, fname , READONLY,&status);
 //
 QMessageBox mess;
 
 fits_get_img_equivtype(fptr, &bpix, &status);
 naxes = new long[2];
 fits_get_img_size(fptr, 2, naxes, &status);
 
 int x_ex = naxes[0]*pow(2.0, -rate);
 int y_ex = naxes[1]*pow(2.0, -rate);
 
 nelements_ex = x_ex*y_ex;
 nelements = naxes[0]*naxes[1];
 //
 QString str, strl;
 str.setNum(bpix);
 //QMessageBox::information(0,"debug",str,QMessageBox::Ok,0,0);
 ////////////
 char *card = new char[FLEN_CARD];
 int nkeys;
 fits_get_hdrspace(fptr, &nkeys, NULL, &status);
 str = "";
 for (int k = 1; k <= nkeys; k++)
 {
   fits_read_record(fptr, k, card, &status);
   strl = QString::QString(card);
 //QMessageBox::information(0,"debug",str,QMessageBox::Ok,0,0);
   //str.append(card);
    str = str + strl + "\n";
 }
 //
 if (nkeys==10)
 {
  pimgf->isOldNA = true;
  fits_read_record(fptr, 8, card, &status);
  QString strf=QString(card);
  strf = strf.mid(10,7);
  exp_time = strf.toDouble();
  fits_read_record(fptr, 9, card, &status);
  date_obs_str=QString(card);
  date_obs = getMJDfromStrNA(date_obs_str, exp_time);
  pimgf->curr_mjd = date_obs;
  pimgf->ra_oc = 0;
  pimgf->de_oc = 0;
 }
 else
 {
 pimgf->isOldNA = false;
 char value[80];
 fits_read_key(fptr, TSTRING, "DATE-OBS", value, NULL, &status);
 date_obs_str=QString(value);
 //fits_read_key(fptr, TDOUBLE, "MJD", &date_obs, NULL, &status);
 fits_read_key(fptr, TDOUBLE, "EXPTIME", &exp_time, NULL, &status);
 //date_obs = date_obs + exp_time/(2*86400);
 date_obs = getMJDfromStrFTN(date_obs_str, exp_time);
 pimgf->curr_mjd = date_obs;
 ////////////////////////////
 fits_read_key(fptr, TSTRING, "RA", value, NULL, &status);
 QString pos_str=QString(value);
 pimgf->ra_oc = hms_to_mas(pos_str,":");
 fits_read_key(fptr, TSTRING, "DEC", value, NULL, &status);
 pos_str=QString(value);
 pimgf->de_oc = damas_to_mas(pos_str,":");
 //////////////////////////////
 }
 delete card;
 str = str + "\n MJD (central moment) = " + QString( "%1" ).arg( date_obs,20,'f',10,QLatin1Char( ' ' ));
 //QMessageBox::information(0,"debug",date_obs_str,QMessageBox::Ok,0,0);
 //str.setNum(date_obs);
 //str = QString( "%1" ).arg( date_obs,20,'f',10,QLatin1Char( ' ' ));
 //QMessageBox::information(0,"debug",str,QMessageBox::Ok,0,0);
 
// int wa32 = naxes[0] + (4*(naxes[0]%4!=0)-naxes[0]%4);
 int wa32 = x_ex + (4*(x_ex%4!=0)-x_ex%4);
 //str.setNum(naxes[0]);
 //QMessageBox::information(0,"debug",str,QMessageBox::Ok,0,0);
 long *fpixel = new long[2];
 fpixel[0] = 1;
 fpixel[1] = 1;
 long felem = 1;
 int anynul = 0;
 ///////////////////////////////////
 
// CCDImage = new uchar[wa32*naxes[1]];
 CCDImage = new uchar[wa32*y_ex];
 fCCDdata = new double[nelements_ex];
 quint32 i;
 QString mess_str;
 char *tempstr;
 tempstr = new char[255];
 strcpy(tempstr, "");
 ////////////////////////////
 if (pimgf->isOldNA)
 {
   unsigned short *fr_data = new unsigned short[nelements];
   fits_read_img(fptr, TUSHORT, felem, nelements, NULL, fr_data ,&anynul, &status);
   //
   quint32 idx1,idx2;
   for (int x=0;x<naxes[0];x++)
   {
      for (int y=0;y<naxes[1];y++)
	  {
	    idx1 = y*naxes[0] + x;
		idx2 = x*naxes[1] + y;
		fCCDdata[idx2] = fr_data[idx1];
	  }
   }
   //
   delete fr_data;
   XSize = (quint16)naxes[1];
   YSize = (quint16)naxes[0];
   naxes[0] = XSize;
   naxes[1] = YSize;
 }
 else
 {
  if (bpix==USHORT_IMG)  
  { //QMessageBox::information(0,"debug","ushort_img",QMessageBox::Ok,0,0);
    status = 0;
    unsigned short *fdata_16 = new unsigned short[nelements];
    fits_read_img(fptr, TUSHORT, felem, nelements, NULL, fdata_16 ,&anynul, &status);
	//str.setNum(status);
    //QMessageBox::information(0,"debug",str,QMessageBox::Ok,0,0);
	
	// mess.setText("USHORT_IMG");
	// mess.exec();
	
	unsigned short fsumm;
	int jump = (int) pow(2, rate);
	for (i=0;i<nelements;i++)
	{
		fsumm = 0;
		for(n=0; n<jump; n++)
		for(m=0; m<jump; m++)
		{
			fsumm += fdata_16[i*jump+m+naxes[0]*n];
		}
	
		fCCDdata[i] = fsumm*pow(jump, -2.0);
	}
	
	
	delete fdata_16;
  }
  if (bpix==SHORT_IMG)  
  { //QMessageBox::information(0,"debug","short_img",QMessageBox::Ok,0,0);
    status = 0;
    short *fdata16 = new short[nelements];
    fits_read_img(fptr, TSHORT, felem, nelements, NULL, fdata16 ,&anynul, &status);
//	for (i=0;i<nelements;i++)fCCDdata[i] = fdata16[i];

	
	/*mess.setText("SHORT_IMG");
	mess.exec();*/
	
	short fsumm;
	int jump = (int) pow(2, rate);
	for (i=0;i<nelements;i++)
	{
		fsumm = 0;
		for(n=0; n<jump; n++)
		for(m=0; m<jump; m++)
		{
			fsumm += fdata16[i*jump+m+naxes[0]*n];
		}
	
		fCCDdata[i] = fsumm*pow(jump, -2.0);
	}
	
	delete fdata16;
  }  
  if (bpix==LONG_IMG)
  { //QMessageBox::information(0,"debug","long_img",QMessageBox::Ok,0,0);
  
	/*
	mess.setText("LONG_IMG");
	mess.exec();*/
  
    status = 0;
    long *fdata32 = new long[nelements];
    fits_read_img(fptr, TLONG, felem, nelements, NULL, fdata32 ,&anynul, &status);
//	for (i=0;i<nelements;i++)fCCDdata[i] = fdata32[i];

	long fsumm;
	int jump = (int) pow(2, rate);
	for (i=0;i<nelements;i++)
	{
		fsumm = 0;
		for(n=0; n<jump; n++)
		for(m=0; m<jump; m++)
		{
			fsumm += fdata32[i*jump+m+naxes[1]*n];
		}
	
		fCCDdata[i] = fsumm*pow(jump, -2.0);
	}


	delete fdata32;
  }
  if (bpix==FLOAT_IMG)
  { //QMessageBox::information(0,"debug","float_img",QMessageBox::Ok,0,0);
	
	//FILE *FEX = fopen("fex.log", "w+");
	//fprintf(FEX, "rate\t%d\nnelem\t%d\nx_ex\t%d\ny_ex\t%d\n", rate, nelements, x_ex, y_ex);
	
    status = 0;
    float *fldata = new float[nelements];
    fits_read_img(fptr, TFLOAT, felem, nelements, NULL, fldata ,&anynul, &status);
//	for (i=0;i<nelements;i++)fCCDdata[i] = fldata[i];

	float fsumm;
	int jump = (int) pow(2, rate);
		
	//fprintf(FEX, "jump\t%d\nnelem\t%d\tnelem_ex\t%d\nx_ex\t%d\ty_ex\t%d\nnaxes[0]\t%d\tnaxes[1]\t%d\n", jump, nelements, nelements_ex, x_ex, y_ex, naxes[0], naxes[1]);
		
	for (i=0;i<nelements_ex;i++)
	{
		fsumm = 0;
		for(n=0; n<jump; n++)
		for(m=0; m<jump; m++)
		{
			fsumm += fldata[i*jump+m+naxes[0]*n+naxes[0]*(jump-1)*(i/x_ex)];
//			fprintf(FEX, "%d:%d\t%d\t%d\n", m, n, i, i*jump+m+naxes[0]*n+naxes[0]*(jump-1)*(i/x_ex));
		}
	
		fCCDdata[i] = fsumm*pow(jump, -2.0);
	}

	//fclose(FEX);


	delete fldata;
  }
  if (bpix==DOUBLE_IMG)
  { //QMessageBox::information(0,"debug","double_img",QMessageBox::Ok,0,0);
/*	mess.setText("DOUBLE_IMG");
	mess.exec();
	*/
    status = 0;
    double *fl_data = new double[nelements];
    fits_read_img(fptr, TDOUBLE, felem, nelements, NULL, fl_data ,&anynul, &status);
	for (i=0;i<nelements;i++)fCCDdata[i] = fl_data[i];
	delete fl_data;
  }
  /* XSize = (quint16)naxes[0];
  YSize = (quint16)naxes[1]; */
  XSize = (quint16)x_ex;
  YSize = (quint16)y_ex;
 }
 fits_close_file(fptr, &status); //if previous file has not closed then close it
 ///////////
 delete [] tempstr;
 //////////////////////////////////////////////
 getRange();
 //QMessageBox::information(0,"debug","ok",QMessageBox::Ok,0,0);
 getHist();
 getHistPr();
 
 setImgData(histL, histR,1,pimgf->inv);
 
 pimgf->xSize = XSize;
 pimgf->ySize = YSize; 
 features = new double[8];
 features[0] = MaxADC; features[1] = MinADC; features[2] = MeanADC;
 features[3] = histL; features[4] = histR; features[5] = histMax;
 features[6] = borderL; features[7] = borderR;
 FNList << FileName;
 keyList << str;
 expList << exp_time;
 mjdList << date_obs;
 pl_naxes << naxes;
 pl_CCDImage << CCDImage;
 //pl_CCDdata << CCDdata;
 pl_fCCDdata << fCCDdata;
 pl_hist << hist;
 pl_features << features;
 pl_index++;
 pimgf ->frIndex = pl_index;
 pimgf->num_of_frames = FNList.count();
 pimgf->keyStr = str;
 pimgf->gamma = 1;
 gammaList << 1;
 raList << pimgf->ra_oc;
 deList << pimgf->de_oc;
 invList << pimgf->inv;
};


void CCDFrame::slotLeftFrame()
{//QMessageBox::information(0,"debug","left",QMessageBox::Ok,0,0);
  if (pl_index!=0)
  {
    pl_index--;
    pimgf ->frIndex = pl_index;	
    SetFrame(pl_index);
  }
};


void CCDFrame::slotRightFrame()
{//QMessageBox::information(0,"debug","Right",QMessageBox::Ok,0,0);
  if (pl_index!=FNList.count()-1)
  {
    pl_index++;
    pimgf ->frIndex = pl_index;	
    SetFrame(pl_index);
  }
};

void CCDFrame::slotDeleteFrame()
{
  DeleteFrame(pl_index);
};

void CCDFrame::DeleteFrame(int N)
{
  if ((N>=0)&&(N<FNList.count()))
  {
   delete pl_naxes.at ( N );
   delete pl_CCDImage.at ( N );
   //delete pl_CCDdata.at ( N );
   delete pl_fCCDdata.at ( N );
   delete pl_hist.at ( N );
   delete pl_features.at ( N ); 
   gammaList.remove ( N );
   raList.remove ( N );
   deList.remove ( N );
   invList.remove (N);
   FNList.removeAt ( N );
   keyList.removeAt (N);
   pl_naxes.removeAt ( N );
   pl_CCDImage.removeAt ( N );
   //pl_CCDdata.removeAt ( N );
   pl_fCCDdata.removeAt ( N );
   pl_hist.removeAt ( N );
   pl_features.removeAt ( N );
   expList.remove ( N );
   mjdList.remove ( N );
   pimgf->num_of_frames = FNList.count();
   if (N==FNList.count())
   {
     pl_index--;
	 pimgf ->frIndex = pl_index;
     SetFrame(pl_index);
   }
   else 
   {
     SetFrame(N);
	 pl_index =N;
	 pimgf ->frIndex = pl_index;
   }
  }
};

void CCDFrame::SetFrame(int N)
{
  if ((N>=0)&&(N<FNList.count()))
  {
   FileName = FNList.at ( N );
   exp_time = expList.at ( N );
   date_obs = mjdList.at ( N );
   pimgf->curr_mjd = date_obs;
   //QMessageBox::information(0,"debug",FileName,QMessageBox::Ok,0,0);
   naxes = pl_naxes.at ( N );
   CCDImage = pl_CCDImage.at ( N );
   //CCDdata = pl_CCDdata.at ( N );
   fCCDdata = pl_fCCDdata.at ( N );
   hist = pl_hist.at ( N );
   features = pl_features.at ( N );
   ///////////////////////////
   MaxADC = features[0];
   MinADC = features[1];
   MeanADC = features[2];
   histL = features[3];
   histR = features[4];
   histMax = features[5];
   borderL = features[6];
   borderR = features[7];
   XSize = naxes[0];
   YSize = naxes[1];
   ////////////////////////////
   pimgf->ADCmax = MaxADC;
   pimgf->ADCmin = MinADC;
   pimgf->ADCmean = MeanADC;
   pimgf->lHist = histL;
   pimgf->rHist = histR;
   pimgf->maxHist = histMax;
   pimgf->leftB = borderL;
   pimgf->rightB = borderR;
   pimgf->xSize = XSize;
   pimgf->ySize = YSize;
   pimgf->imgCCD = CCDImage;
   //pimgf->rdata = CCDdata;
   pimgf->fCCD = fCCDdata;
   pimgf->file_name = FileName;
   pimgf->keyStr = keyList.at ( N );
   pimgf->gamma = gammaList.at(N);
   pimgf->ra_oc = raList.at(N);
   pimgf->de_oc = deList.at(N);
   pimgf->histOfImg = hist;
   pimgf->inv = invList.at(N);
  }
  //setImgData(histL, histR,pimgf->gamma,pimgf->inv);
  emitImageProcessed(1);
};

double CCDFrame::getValue(quint16 x_pos, quint16 y_pos)
{
     quint32 i = y_pos*XSize+x_pos;
     return fCCDdata[i];
};

void CCDFrame::getRange()
{
    MaxADC = fCCDdata[0];
    MinADC = fCCDdata[0];
    double meanADC = 0;
    quint32 imsize = XSize*YSize;
    for (quint32 i=0;i<imsize;i++) 
       {
         if (MaxADC<fCCDdata[i])MaxADC=fCCDdata[i];
         if (MinADC>fCCDdata[i])MinADC=fCCDdata[i];
         meanADC = meanADC+fCCDdata[i];
       }//end for
    MeanADC = (quint16)(meanADC/imsize);
};//end getRange()


void CCDFrame::setImgData(double minV, double maxV, double Gamma, int inverse)
{
    pimgf->gamma = Gamma;
	gammaList[pl_index] = Gamma;
	quint32 imsize = XSize*YSize;
    double valueI=0;
    double diap=maxV-minV;
    double valueU=0;
	Gamma = 1/Gamma;
	int wa32 = XSize + (4*(XSize%4!=0)-XSize%4);
	quint32 i,j;
    if (diap>0){
       for (int x=0;x<XSize;x++)
       {	   
	    for (int y=0;y<YSize;y++)
         {
		    i = x+y*XSize;
            valueU = fCCDdata[i];
            if (valueU<minV)valueU=minV;
            if (valueU>maxV)valueU=maxV;       
            if (inverse)
			   valueI = 255 - 255*pow((valueU-minV)/diap,Gamma);
			else 
			   valueI = 255*pow((valueU-minV)/diap,Gamma);
			j = x + y*wa32;
            CCDImage[j] = (uchar)valueI;
         }//end for
	   }
    }//diap>0
};//setImgData

void CCDFrame::getHist()
{
quint32 imsize = XSize*YSize;
histsize = (quint32)(MaxADC - MinADC+1);
hist = new quint32[histsize];
quint32 i;
for (i=0;i<histsize;i++) hist[i] = 0;
for (i=0;i<imsize;i++) 
         {
           hist[(int)(fCCDdata[i]-MinADC)]++; 
         }//end for
};

void CCDFrame::getHistPr()
{
 quint32 maxHist = 0;
 quint32 i,max_i;
 for (i=0;i<histsize;i++)
 {
    if (hist[i]>maxHist) 
	   {
	    maxHist = hist[i];
		max_i = i;
	   }
 }
 histMax = maxHist;
 //
 //
 pimgf->fr_pix = max_i + MinADC;
 histR = max_i+histsize/75 + MinADC;
 histL = max_i + MinADC;
 borderR = max_i+histsize/50 + MinADC;
 borderL = (double)max_i-histsize/50 + MinADC;
 if (borderL<0) borderL = 0;
};

void CCDFrame::getRangeIndx(int indexF)
{
    //QString str;
    double *fData = pl_fCCDdata.at(indexF);
	double *ftrs_indx = pl_features.at(indexF);
	double maxP = fData[0];
    double minP = fData[0];
    double meanP = 0;
	long *nx = pl_naxes.at(indexF);
    quint32 imsize = nx[0]*nx[1];
    for (quint32 i=0;i<imsize;i++) 
       {
         if (maxP<fData[i])maxP=fData[i];
         if (minP>fData[i])minP=fData[i];
         meanP = meanP+fData[i];
       }//end for
    meanP = meanP/imsize;
	ftrs_indx[0] = maxP;
	ftrs_indx[1] = minP;
	ftrs_indx[2] = meanP;
	//str.setNum(maxP);
	//QMessageBox::information(0,"debug",str,QMessageBox::Ok,0,0);
};


void CCDFrame::getHistIndx(int indexF)
{
    //QString str;
    double *fData = pl_fCCDdata.at(indexF);
	long *nx = pl_naxes.at(indexF);
    quint32 imsize = nx[0]*nx[1];
	double *ftrs_indx = pl_features.at(indexF);
	quint32 hsize = (quint32)(ftrs_indx[0] - ftrs_indx[1]+1);
	//str.setNum((quint32)(ftrs_indx[0] - ftrs_indx[1]+1));
	//QMessageBox::information(0,"debug",str,QMessageBox::Ok,0,0);
	quint32 *hst = new quint32[hsize];
	quint32 i;
	for (i=0;i<hsize;i++) hst[i] = 0;
	//QMessageBox::information(0,"debug","469",QMessageBox::Ok,0,0);
	for (i=0;i<imsize;i++) 
	         {
	           hst[(int)(fData[i]-ftrs_indx[1])]++; 
	         }//end for
	//QMessageBox::information(0,"debug","473",QMessageBox::Ok,0,0);
	delete pl_hist.at(indexF);
    pl_hist[indexF] = hst;
};

void CCDFrame::getHistPrIndx(int indexF)
{
 quint32 maxHist = 0;
 quint32 i,max_i;
 double *ftrs_indx = pl_features.at(indexF);
 quint32 hsize = quint32(ftrs_indx[0] - ftrs_indx[1]+1);
 quint32 *hst = pl_hist[indexF];
 for (i=0;i<hsize;i++)
 {
    if (hst[i]>maxHist) 
	   {
	    maxHist = hst[i];
		max_i = i;
	   }
 }
 ftrs_indx[5] = maxHist;
 //
 // pimgf->fr_pix = max_i + MinADC;
 ftrs_indx[4] = max_i+hsize/75 + ftrs_indx[1];
 ftrs_indx[3] = max_i + ftrs_indx[1];
 ftrs_indx[7] = max_i+hsize/50 + ftrs_indx[1];
 ftrs_indx[6] = (double)max_i-hsize/50 + ftrs_indx[1];
 if (ftrs_indx[6]<0) ftrs_indx[0] = 0;
};

void CCDFrame::setImgDataIndx(int indexF)
{//double minV, double maxV, double Gamma, 
    int inverse = invList[indexF];
    double *fData = pl_fCCDdata.at(indexF);
	uchar* fImg = pl_CCDImage.at(indexF);
	double Gamma = 1;//gammaList[indexF]; 
	gammaList[indexF] = 1;
	long *nx = pl_naxes.at (indexF);
    quint32 imsize = nx[0]*nx[1];
    double valueI=0;
	double *ftrs_indx = pl_features.at (indexF);
	double minV = ftrs_indx[3];
	double maxV = ftrs_indx[4];
    double diap=maxV-minV;
    double valueU=0;
	Gamma = 1/Gamma;
	int wa32 = nx[0] + (4*(nx[0]%4!=0)-nx[0]%4);
	quint32 i,j;
    if (diap>0){
       for (int x=0;x<nx[0];x++)
       {	   
	    for (int y=0;y<nx[1];y++)
         {
		    i = x+y*nx[0];
            valueU = fData[i];
            if (valueU<minV)valueU=minV;
            if (valueU>maxV)valueU=maxV;       
            if (inverse)
			   valueI = 255 - 255*pow((valueU-minV)/diap,Gamma);
			else 
			   valueI = 255*pow((valueU-minV)/diap,Gamma);
			j = x + y*wa32;
            fImg[j] = (uchar)valueI;
         }//end for
	   }
    }//diap>0
};
	
	


void CCDFrame::testing()
{
QMessageBox::information(0,"debug","signal has been received",QMessageBox::Ok,0,0);
};
/////////////////////////slots
void CCDFrame::slotLoadFITS(QString FileName)
{
  LoadFromFITS(FileName);
  pimgf->ADCmax = MaxADC;
  pimgf->ADCmin = MinADC;
  pimgf->ADCmean = MeanADC;
  pimgf->lHist = histL;
  pimgf->rHist = histR;
  pimgf->maxHist = histMax;
  pimgf->leftB = borderL;
  pimgf->rightB = borderR;
  pimgf->xSize = XSize;
  pimgf->ySize = YSize;
  pimgf->imgCCD = CCDImage;
  //pimgf->rdata = CCDdata;
  pimgf->fCCD = fCCDdata;
  pimgf->file_name = FileName;
  pimgf->histOfImg = hist;
  //pimgf->rMtrx = &mCCD;
  pimgf->pr_ol = &r_ol;
  pimgf->pm_ol = &m_ol;
  pimgf->p_refcat = &refcat;
  pimgf->p_cel_pos = &cel_pos;
  pimgf->p_tangpos = &tangpos;
  pimgf->p_resOUT = &resOUT;
  emitImageProcessed(1);
};

void CCDFrame::slotLoadFITS_ex(QString FileName, QString FileNameEx, int rate)
{
  LoadFromFITS_ex(FileName, FileNameEx, rate);
  pimgf->ADCmax = MaxADC;
  pimgf->ADCmin = MinADC;
  pimgf->ADCmean = MeanADC;
  pimgf->lHist = histL;
  pimgf->rHist = histR;
  pimgf->maxHist = histMax;
  pimgf->leftB = borderL;
  pimgf->rightB = borderR;
  pimgf->xSize = XSize;
  pimgf->ySize = YSize;
  pimgf->imgCCD = CCDImage;
  //pimgf->rdata = CCDdata;
  pimgf->fCCD = fCCDdata;
  pimgf->file_name = FileName;
  pimgf->histOfImg = hist;
  //pimgf->rMtrx = &mCCD;
  pimgf->pr_ol = &r_ol;
  pimgf->pm_ol = &m_ol;
  pimgf->p_refcat = &refcat;
  pimgf->p_cel_pos = &cel_pos;
  pimgf->p_tangpos = &tangpos;
  pimgf->p_resOUT = &resOUT;
  emitImageProcessed(1);
};


void CCDFrame::slotBC(double b, double c, double g, int inv)
{
 setImgData(b, c, g, inv);
 double *ftrs_ndx = pl_features.at (pl_index);
 ftrs_ndx[3] = b;
 ftrs_ndx[4] = c;
 ftrs_ndx[3] = pimgf->leftB;
 ftrs_ndx[4] = pimgf->rightB;
 invList[pl_index] = inv;
 emitImageProcessed(0);
};

void CCDFrame::slotCenterOfMass(int x, int y, int r)
{
 CCDDATA data = ApertureData(fCCDdata, x, y, r, XSize, YSize);
 STIMG cofm = CenterOfMas(data);
 mRES->resize(0);
 mRES->append(cofm.X);
 mRES->append(cofm.Y);
 mRES->append(cofm.M);
 delete data.Buffer;
 emitMeasured();
};

void CCDFrame::slotApproxImg(int x, int y, int r, double delta, int psft)
{

  CCDDATA data = ApertureData(fCCDdata, x, y, r, XSize, YSize);
  int N;
  vector<double> P;
  switch(psft)
 {
   case 0://Lorentzian
		  N = 8;
		  P = StartConditionsL(data, delta);
		  break;
   case 1://Gaussian
		  N = 7;
		  P = StartConditionsG(data);
		  break;
   case 2://Moffat
		  N = 8;
		  P = StartConditionsM(data, delta);
		  break;
 }	
//QMessageBox::information(0,"debug","ok",QMessageBox::Ok,0,0);
int *actv = new int[N];
for (int i=0;i<N;i++) actv[i] = 1;
mRES->resize(0);
//QVector<double> Parameters = 
*mRES = NLSLevenbergMarquardt(data, P, psft, actv, N, 
                          pimgf->max_num_it, pimgf->max_num_reg, pimgf->min_par_step, pimgf->min_uwe_step);

//for (int i=0;i<Parameters.size();i++) mRES->append(Parameters[i]);
delete data.Buffer;
delete actv;
 emitSignalPSF();
};


void CCDFrame::slotMeasureMarkedObjects(int r, double delta, int psft, int centr, int vstar)
{
   int N = 0;
  switch(psft)
	 {
	   case 0://Lorentzian
			  N = 8;
			  break;
	   case 1://Gaussian
			  N = 7;
			  break;
	   case 2://Moffat
			  N = 8;
			  break;
	 }	
   ////////////////////
   int *actv = new int[N];
   for (int j=0;j<N;j++) actv[j] = 1;
   ////////////////////////////////
   vector<double> P;
   QVector<double> rV;
   CCDDATA data;
   CCDDATA obj_data;
   STIMG CM;
   STIMG st_center;
   bool isObj;
   //////////////////////////////
   int i,x,y,j;
   for (i = 0; i < r_ol.size1(); i++)
   {
     x = (int) r_ol(i,0);
	 y = (int) r_ol(i,1);
	 if ((x>0)&&(x<XSize)&&(y>0)&&(y<YSize))
	 {
		 //////////////////////
		 if (centr==1)
		 {
		   //st_center = StarCentering(fCCDdata, x, y, r, 2, XSize, YSize);
		   st_center = getStarCenter(fCCDdata, x, y, r, 2, XSize, YSize);
		   x = (int)st_center.X;
		   y = (int)st_center.Y;
		 }
		 //////////////////////
		 isObj = true;
		 if (vstar==1)
		 {
		   obj_data = ProfileData(fCCDdata, x, y, 20, XSize);
		   isObj = isObject(obj_data, pimgf->fr_pix);
		   delete obj_data.Buffer;
		 }
	 }
	 else isObj = false;
	 //////////////////////////////////////////////////
	 if (isObj)
	 {
		 data = ApertureData(fCCDdata, x, y, r, XSize, YSize);
		 switch(psft)
		 {
		   case 0://Lorentzian
				  P = StartConditionsL(data, delta);
				  break;
		   case 1://Gaussian
				  P = StartConditionsG(data);
				  break;
		   case 2://Moffat
				  P = StartConditionsM(data, delta);
				  break;
		 }
		 rV.resize(0);
		 rV = NLSLevenbergMarquardt(data, P, psft, actv, N, 
							  pimgf->max_num_it, pimgf->max_num_reg, pimgf->min_par_step, pimgf->min_uwe_step);
		 delete data.Buffer;
		 /////////////
		 if(rV[0]!=-1)
		 {
		   x = (int) rV[5];
	       y = (int) rV[6];
		   data = ApertureData(fCCDdata, x, y, r, XSize, YSize);
		   CM = CenterOfMas(data);
		   delete data.Buffer;
		   rV << date_obs;
		   rV << exp_time;
		   rV << CM.X;
		   rV << CM.Y;
		 }
		 else for(j=0;j<4;j++) rV<<-1;
		 ////////////////
		 for (int k = 0; k < rV.count(); k++) r_ol(i,k) = rV[k];
	 }
	 else for (int k = 0; k < 25; k++) r_ol(i,k) = -1;
     //////////////////////	 
     progressBar-> setValue ( (int) 100*i/(r_ol.size1()+m_ol.size1()) );
	 if (pimgf->abort) break;
   }
   ///////////////
   for (i = 0; i < m_ol.size1(); i++)
   {
     x = (int) m_ol(i,0);
	 y = (int) m_ol(i,1);
	 if ((x>0)&&(x<XSize)&&(y>0)&&(y<YSize))
	 {
		 //////////////////////
		 if (centr==1)
		 {
		   //st_center = StarCentering(fCCDdata, x, y, r, 2, XSize, YSize);
		   st_center = getStarCenter(fCCDdata, x, y, r, 2, XSize, YSize);
		   x = (int)st_center.X;
		   y = (int)st_center.Y;
		 }
		 //////////////////////
		 isObj = true;
		 if (vstar==1)
		 {
		   obj_data = ProfileData(fCCDdata, x, y, 20, XSize);
		   isObj = isObject(obj_data, pimgf->fr_pix);
		   delete obj_data.Buffer;
		 }
	 }
	 else isObj=false;
	 //////////////////////////////////////////////////
	 if (isObj)
	 {
		 data = ApertureData(fCCDdata, x, y, r, XSize, YSize);
		 switch(psft)
		 {
		   case 0://Lorentzian
				  P = StartConditionsL(data, delta);
				  break;
		   case 1://Gaussian
				  P = StartConditionsG(data);
				  break;
		   case 2://Moffat
				  P = StartConditionsM(data, delta);
				  break;
		 }
		 rV = NLSLevenbergMarquardt(data, P, psft, actv, N, 
							  pimgf->max_num_it, pimgf->max_num_reg, pimgf->min_par_step, pimgf->min_uwe_step);
		 delete data.Buffer;
		 /////////////
		 if(rV[0]!=-1)
		 {
		   x = (int) rV[5];
	       y = (int) rV[6];
		   data = ApertureData(fCCDdata, x, y, r, XSize, YSize);
		   CM = CenterOfMas(data);
		   delete data.Buffer;
		   rV << date_obs;
		   rV << exp_time;
		   rV << CM.X;
		   rV << CM.Y;
		 }
		 else for(j=0;j<4;j++) rV<<-1;
		 ////////////////
		 for (int k = 0; k < rV.count(); k++) m_ol(i,k) = rV[k];
		 
		 //////////////////////
	 }
	 else for (int k = 0; k < 25; k++) m_ol(i,k) = -1;
     progressBar-> setValue ( (int) 100*(i+r_ol.size1())/(r_ol.size1()+m_ol.size1()) );
	 if (pimgf->abort) break;
   }
   if (!pimgf->abort) progressBar-> setValue (100);
   delete actv;
   pimgf -> measured_file_name = FileName;
   emitSignalDone();
};


void CCDFrame::slotAddMFrame()
{
   if ((r_ol.size1()>0)&&(m_ol.size1()>0))
   {
     int i;
     int N = r_ol.size1()*r_ol.size2();
     double *rbuff = new double[N];
	 double *md = r_ol.data().begin();
	 for (i = 0; i<N; i++) rbuff[i] = md[i];
	 roList << rbuff;
	 ro_list_sizes << r_ol.size1();
	 N = m_ol.size1()*m_ol.size2();
     double *mbuff = new double[N];
	 md = m_ol.data().begin();
	 for (i = 0; i<N; i++) mbuff[i] = md[i];
	 moList << mbuff;
	 mo_list_sizes << m_ol.size1();
	 emit signalMFrameAdded();
   }   
};


void CCDFrame::slotDelMFrame(int del_index)
{ 
  delete roList.at (del_index);
  delete moList.at (del_index);
  roList.removeAt(del_index);
  moList.removeAt(del_index);
  ro_list_sizes.remove(del_index);
  mo_list_sizes.remove(del_index);
};

void CCDFrame::slotSetMFrame(int index)
{
  //QMessageBox::information(0,"debug","ok",QMessageBox::Ok,0,0);
  int i,N;
  r_ol.resize(ro_list_sizes.at(index),26);
  double *rbuff = roList.at(index);
  double *md = r_ol.data().begin();
  N = 26*ro_list_sizes.at(index);
  for (i = 0; i<N; i++) md[i] = rbuff[i];
  ////////////////////////////////////////////////
  m_ol.resize(mo_list_sizes.at(index),26);
  double *mbuff = moList.at(index);
  md = m_ol.data().begin();
  N = 26*mo_list_sizes.at(index);
  for (i = 0; i<N; i++) md[i] = mbuff[i];
};


void CCDFrame::slotStartProc1(int index, int ang_un, double ang_sc)
{
   int i,N,K,j;
   bool singular;
   double uwe_x,uwe_y,mjd,mjd_0,min_err;
   N = ro_list_sizes[index];
   K = mo_list_sizes[index];
   matrix<double> res(roList.count()*K,6);
   matrix<double> C;
   vector<double> Lx;
   vector<double> Ly;
   vector<double> Px;
   vector<double> Py;
   //vector<double> Rx;
   //vector<double> Ry;
   vector<double> Zx(3);
   vector<double> Zy(3);
   matrix<double> RSx;
   matrix<double> RSy;
   //////////////////
   QVector<double>C_x;
   QVector<double>C_y;
   QVector<double>L_x;
   QVector<double>L_y;
   QVector<double>P_x;
   QVector<double>P_y;
   /////////////////
   matrix<double> Cm(K,3);
   vector<double> Lmx(K);
   vector<double> Lmy(K);
   /////////////////
   matrix<double> refM = fromDataToMatrix(roList[index], N);
   /////////////////////
	 j = 0;
	 do
	 {
	  mjd_0 = refM(j,20);
	  j++;
	 }
	 while (mjd_0==-1);
   /////////////////////////
   matrix<double> currM;
   matrix<double> currMm(K,26);
   for (i=0;i<roList.count();i++)
   {
     if (i==index)
	 {
		 currMm = fromDataToMatrix(moList[i], K);
		 for (j=0;j<K;j++)
		 {
		  res(i+j*roList.count(),0) = currMm(j,5);
		  res(i+j*roList.count(),1) = currMm(j,6);
		  res(i+j*roList.count(),2) = mjd_0;
		  res(i+j*roList.count(),3) = 0;
		  res(i+j*roList.count(),4) = 0;
		  res(i+j*roList.count(),5) = 0;
		 }
		 //////////////////////////////
	 }
	 else
	 {
		 currM = fromDataToMatrix(roList[i], N);
		 j = 0;
		 do
		 {
		  mjd = currM(j,20);
		  j++;
		 }
		 while (mjd==-1);
		 ///////////////////////
		 C_x.resize(0);C_y.resize(0);L_x.resize(0);L_y.resize(0);P_x.resize(0);P_y.resize(0);
		 for (j=0;j<N;j++)
		 {
		   if ((refM(j,0)!=-1)&&(currM(j,0)!=-1))
		   {
		     C_x << currM(j,5);
			 C_y << currM(j,6);
			 L_x << refM(j,5);
			 L_y << refM(j,6);
			 P_x << pow(refM(j,13),2) + pow(currM(j,13),2);
			 P_y << pow(refM(j,14),2) + pow(currM(j,14),2);
		   }
		 }
		 ///////////////////////
		 min_err =  MinOfVector(P_x);
		 for (j=0;j<P_x.count();j++)P_x[j] = min_err/P_x[j];
		 min_err =  MinOfVector(P_y);
		 for (j=0;j<P_y.count();j++)P_y[j] = min_err/P_y[j];
		 ///////////////////////
		 Lx.resize(C_x.count());
		 Ly.resize(C_x.count());
		 Px.resize(C_x.count());
		 Py.resize(C_x.count());
		 C.resize(C_x.count(),3);
		 for (j=0;j<C_x.count();j++)
		 {
		  C(j,0) = C_x[j];C(j,1) = C_y[j];C(j,2) = 1;
		  Lx(j) = L_x[j];Ly(j) = L_y[j];
		  Px(j) = P_x[j];Py(j) = P_y[j];
		 }
		 ////////////////////////
		 RSx = LSM_corr(Lx,C,Px,pimgf->max_residual);
		 RSy = LSM_corr(Ly,C,Py,pimgf->max_residual);
		 ///////////////////
		 uwe_x = RSx(0,1);
		 uwe_y = RSy(0,1);
		 for (j=0;j<3;j++)
		 {
		   Zx(j) = RSx(j+1,0);
		   Zy(j) = RSy(j+1,0);
		 }
		 /////////////////////
		 currMm = fromDataToMatrix(moList[i], K);
		 for (j=0;j<K;j++)
		 {
		  Cm(j,0) = currMm(j,5);Cm(j,1) = currMm(j,6);Cm(j,2) = 1;
		 }
		 Lmx = prod(Cm,Zx);
		 Lmy = prod(Cm,Zy);
		 /////////////////////
		 for (j=0;j<K;j++)
		 {
		  res(i+j*roList.count(),0) = Lmx(j);
		  res(i+j*roList.count(),1) = Lmy(j);
		  res(i+j*roList.count(),2) = mjd;
		  res(i+j*roList.count(),3) = uwe_x;
		  res(i+j*roList.count(),4) = uwe_y;
		  res(i+j*roList.count(),5) = RSx(0,0);
		 }
		 //////////////////////////////
	 }
   }
   /////////////////////////////////////
   //MtrxToFile("positions.txt", res);
   /////////////////////////////////////////
   QString str = "MJDo = ";
   str = str + QString( "%1" ).arg( mjd_0,12,'f',6,QLatin1Char( ' ' ))+"\n";
   STDATA std;
   double m_x, err_x, m_y, err_y;
   for (i=0;i<K;i++)
	{
	  L_x.resize(0);L_y.resize(0);
	  str = str +"=================\n";
	  for (j=0;j<roList.count();j++)
      {
	    L_x << res(i*roList.count()+j,0);
		L_y << res(i*roList.count()+j,1);
	  }
	  ////////////////////////////////////
	  std = StDataFromVector(L_x);
      m_x = std.mean; err_x = std.st_err;
      std = StDataFromVector(L_y);
      m_y = std.mean; err_y = std.st_err;
	  ////////////////////////////////////
	  double scF = 1;
	  if(ang_un==1)
	  {
	     scF = ang_sc;
		 for (j=0;j<roList.count();j++)
		      {
			    L_x[j] = scF*(L_x[j]-m_x);
				L_y[j] = scF*(L_y[j]-m_y);
			  }
		 
	  }
	  ////////////////////////////////////
	  for (j=0;j<roList.count();j++)
      {
	    str = str + QString( "%1" ).arg( L_x[j],10,'f',4,QLatin1Char( ' ' ))+" ";
		str = str + QString( "%1" ).arg( L_y[j],10,'f',4,QLatin1Char( ' ' ))+" ";
		str = str + QString( "%1" ).arg( 1440*(res(i*roList.count()+j,2)-mjd_0),7,'f',3,QLatin1Char( ' ' ))+" ";
		str = str + QString( "%1" ).arg( scF*res(i*roList.count()+j,3),5,'f',3,QLatin1Char( ' ' ))+" ";
		str = str + QString( "%1" ).arg( scF*res(i*roList.count()+j,4),5,'f',3,QLatin1Char( ' ' ))+" ";
		str = str + QString( "%1" ).arg( res(i*roList.count()+j,5),4,'f',0,QLatin1Char( ' ' ))+"\n";
	  }
	  /////////////////////////////////////
     str = str +"errors = ";
     str = str + QString( "%1" ).arg(scF*err_x,5,'f',3,QLatin1Char( ' ' ))+" ";
	 str = str + QString( "%1" ).arg(scF*err_y,5,'f',3,QLatin1Char( ' ' ))+"\n\n";
      ///////////////////////	 
	}
	//////////////////////////
	
	//////////////////////////
	pimgf->simple_message = str;
	signalProcCompleted();
   /////////////////////////////////////////
};

void CCDFrame::slotSubtractPSF(int x, int y, int r, int psft)
{
  //QVector<double> psfP;
  //for (int i=0;i<mRES->count();i++) psfP<<mRES->at(i);
  //QMessageBox::information(0,"debug","ok",QMessageBox::Ok,0,0);
  subtractPSF(mRES, psft, fCCDdata, x, y, r, XSize, YSize);
  //quint32 i, imsize = XSize*YSize;
  //for (i = 0; i<imsize; i++) CCDdata[i] = (int)fCCDdata[i];
  setImgData(histL, histR,pimgf->gamma,pimgf->inv);
  emitImageProcessed(1);
};


void CCDFrame::slotExecute(QString comLine)
{
   //QMessageBox::information(0,"debug",comLine,QMessageBox::Ok,0,0);
   comLine = comLine.trimmed();
   QStringList operands = comLine.split(" ");
   if (operands[0]=="getUCAC2")
   {
    double ra = hms_to_mas(operands[1],":");
	double de = damas_to_mas(operands[2],":");
	QString nums = operands[3];
	double rho = nums.toDouble()*60000;
	nums = operands[4];
    double mag1 = nums.toDouble();
    nums = operands[5];
    double mag2 = nums.toDouble();
	//nums.setNum(de);
	//QMessageBox::information(0,"debug",nums,QMessageBox::Ok,0,0);
	refcat = getUCAC2data(ra, de, rho, 0, 0, pimgf->ucac2_path, mag1, mag2);
	MtrxToFile("ucac.txt", refcat);
   }
   if (operands[0]=="getPirf")
   {
     //QMessageBox::information(0,"debug",operands[0],QMessageBox::Ok,0,0);
     QString nums = operands[1];
     int index = nums.toInt();
	 nums = operands[2];
     int angUn = nums.toInt();
	 nums = operands[3];
     double angSc = nums.toDouble();
	 slotStartProc1(index, angUn, angSc);
   }
   if (operands[0]=="getEQ")
   {
     //QMessageBox::information(0,"debug",operands[0],QMessageBox::Ok,0,0);
     QString nums = operands[1];
     int indexM = nums.toInt();
	 nums = operands[2];
     int indexF = nums.toInt();
	 nums = operands[3];
     double maxr = nums.toDouble();
	 nums = operands[4];
	 int r_cat = nums.toInt();
	 nums = operands[5];
	 int use_w = nums.toInt();
	 nums = operands[6];
	 int s_log = nums.toInt();
	 QString fnr = operands[7];
	 nums = operands[8];
	 int diffSat = nums.toInt();
	 nums = operands[9];
	 int out_s = nums.toInt();
	 astrometric_reduction(indexM, indexF, maxr, r_cat, use_w, s_log, fnr, diffSat,out_s);
   }
   if (operands[0]=="saveDates") saveDates();
   if (operands[0]=="ephemeris") getEph();
   if (operands[0]=="hFilter") 
     {
	   QString nums = operands[1];
       int index = nums.toInt();
	   nums = operands[2];
       int size = nums.toInt();
	   hFilter(index,size);
	 }
  if (operands[0]=="mFilter") 
	 {
	   QString nums = operands[1];
	   int index = nums.toInt();
	   nums = operands[2];
	   int size = nums.toInt();
	   nums = operands[3];
	   int usePB = nums.toInt();
	   if(usePB==1)
	   {
	     progressBar->setValue(0);
	     emit signalProgressDialogShow();
	   }
	   mFilter(index,size, usePB);
	   if(usePB==1)emit signalProgressDialogHide();
	 }
  if (operands[0]=="mFilterToAll") 
	 {
	   progressBar->setValue(0);
	   emit signalProgressDialogShow();
	   QString nums = operands[1];
	   int size = nums.toInt();
	   for (int i=0; i<raList.count();i++)
	   {
	     mFilter(i,size, 0);
		 progressBar->setValue((int)((double)i/(double)raList.count()*100));
	   }
	   progressBar->setValue(100);
	   emit signalProgressDialogHide();
	 }
 if (operands[0]=="setImgAsIndx") 
	 {
	   QString nums = operands[1];
	   int index = nums.toInt();
	   setImgDataAsIndx(index);
	 }
 if(operands[0]=="getBox")
    {
	 QString nums = operands[1];
	 int ksi = nums.toInt();
	 nums = operands[2];
	 int eta = nums.toInt();
	 nums = operands[3];
	 int scw = nums.toInt();
	 CCDDATA b_data = BoxData(fCCDdata, ksi, eta, 22, 24, XSize, YSize);
	 //////////////////
	 QFile outputfile(operands[4]);
     outputfile.open(QIODevice::WriteOnly | QIODevice::Text);
     QTextStream out_stream;
     out_stream.setDevice(&outputfile);
     QString S;
     /////////////////////////////////////////b_data.Size
      for (int i=0; i<512;i++)
       {
         S = "";
         S = S+QString( "%1" ).arg( b_data.Buffer[i].X,4,10,QLatin1Char( ' ' ))+" "+
		       QString( "%1" ).arg( b_data.Buffer[i].Y,4,10,QLatin1Char( ' ' ))+" "+
		       QString( "%1" ).arg( b_data.Buffer[i].I,10,'f',3,QLatin1Char( ' ' ));
	     S = S+'\n';
	     //QMessageBox::information(0,"debug",S,QMessageBox::Ok,0,0);
	     out_stream << S;
	   }
     //for (int i=0; i<6;i++)	out_stream << "0000 0000       0\n";   
      outputfile.close();
	  delete b_data.Buffer;
	  /////////////////
	}
	 
};

void CCDFrame::slotGetMarks(QString comLine)
{
   comLine = comLine.trimmed();
   QStringList operands = comLine.split(" ");
   pimgf->ra_oc = hms_to_mas(operands[0] +" "+ operands[1] +" "+ operands[2]," ");
   pimgf->de_oc = damas_to_mas(operands[3] +" "+ operands[4] +" "+ operands[5]," ");
   QString str = operands[6];
   pimgf->mag_1 = str.toDouble();
   str = operands[7];
   pimgf->mag_2 = str.toDouble();
   //str.setNum(pimgf->ra_oc);
   //QMessageBox::information(0,"debug",str,QMessageBox::Ok,0,0);
   refcat = getUCAC2data(pimgf->ra_oc, pimgf->de_oc, pimgf->fov, 1, date_obs, pimgf->ucac2_path, pimgf->mag_1, pimgf->mag_2);
   tangpos = getTangPos(refcat, pimgf->ra_oc, pimgf->de_oc, 0, 1, 2);
   //MtrxToFile("ucac.txt", refcat);
   //MtrxToFile("tp.txt", tangpos);
   emit signalTangPosCalculated();
   //QMessageBox::information(0,"debug",comLine,QMessageBox::Ok,0,0);
};

void CCDFrame::astrometric_reduction(int indexM, int indexF,  double max_res, int rcat, int uw, int saveLog, QString mpcFName, int diff_sat, int signal_out)
{  
   int i,j,N,K;
   //QMessageBox::information(0,"debug","ok",QMessageBox::Ok,0,0);
   N = ro_list_sizes[indexM];
   K = mo_list_sizes[indexM];
   //QString msg;
   //msg.setNum(indexM);
   //QMessageBox::information(0,"debug",msg,QMessageBox::Ok,0,0);
   //QMessageBox::information(0,"debug","ok_1",QMessageBox::Ok,0,0);
   /////////////////
   matrix<double> roM = fromDataToMatrix(roList[indexM], N);//reference stars matrix
   matrix<double> moM = fromDataToMatrix(moList[indexM], K);//measured objects matrix
   //QMessageBox::information(0,"debug","ok_2",QMessageBox::Ok,0,0);
   ///////////////
   matrix<double> C;
   vector<double> Lx;
   vector<double> Ly;
   vector<double> Px;
   vector<double> Py;
   //////////////////////
   vector<double> Zx(3);
   vector<double> Zy(3);
   matrix<double> RSx;
   matrix<double> RSy;
   //////////////////
   QVector<double>C_x;
   QVector<double>C_y;
   QVector<double>L_x;
   QVector<double>L_y;
   QVector<double>P_x;
   QVector<double>P_y;
   QVector<double>CM_x;
   QVector<double>CM_y;
   QVector<int> ref_to_refcat;
   /////////////////
   //optical center recalculation
   //QMessageBox::information(0,"debug","ok1",QMessageBox::Ok,0,0);
   double ra_oc_indx = raList.at(indexF);
   double de_oc_indx = deList.at(indexF);
   double date_obs_indx = mjdList.at(indexF);
   long *naxes_red = pl_naxes.at ( indexF );
   double x_oc = (double) naxes_red[0]/2;
   double y_oc = (double) naxes_red[1]/2;
   ///////////////
   for (i = 0; i< N;i++)
   {
     if(roM(i,5)!=-1)
	 {
		 j = (int)roM(i,25);
		 L_x << tangpos(j,0);
		 L_y << tangpos(j,1);
		 C_x << roM(i,5);
		 C_y << roM(i,6);
	 }
   }
     Lx.resize(C_x.count());
	 Ly.resize(C_x.count());
	 Px.resize(C_x.count());
	 Py.resize(C_x.count());
	 C.resize(C_x.count(),3);
	 for (j=0;j<C_x.count();j++)
	 {
	  C(j,0) = C_x[j];C(j,1) = C_y[j];C(j,2) = 1;
	  Lx(j) = L_x[j];Ly(j) = L_y[j];
	  Px(j) = 1;Py(j) = 1;
	 }
	 Zx = LSM(Lx,C,Px);
	 Zy = LSM(Ly,C,Py);
	 double ksi_oc = Zx(0)*x_oc + Zx(1)*y_oc + Zx(2);
	 double eta_oc = Zy(0)*x_oc + Zy(1)*y_oc + Zy(2);
	 vector<double> EP = getTangToRaDe(ksi_oc, eta_oc, ra_oc_indx, de_oc_indx);
	 double ra_oc_rec = EP(0);
	 double de_oc_rec = EP(1);
	 /////////////////////
	 // recalculation of tangential cordinates of the reference stars
	 /////////////////
	 //tangpos = getTangPos(refcat, ra_oc_rec, de_oc_rec, 0, 1, 2);
   ///////////////
   //QMessageBox::information(0,"debug","ok2",QMessageBox::Ok,0,0);
   //reduction with new optical center
   ////////////////////
   L_x.clear();
   L_y.clear();
   C_x.clear();
   C_y.clear();
   P_x.clear();
   P_y.clear();
   for (i = 0; i< N;i++)
   {
     if(roM(i,5)!=-1)
	 {
		 j = (int)roM(i,25);
		 EP = getRaDeToTang(refcat(j,0), refcat(j,1), ra_oc_rec, de_oc_rec);
		 L_x << EP(0);
		 L_y << EP(1);
		 C_x << roM(i,5) - x_oc;
		 C_y << y_oc - roM(i,6);
		 ////////////////////////
		 CM_x << (roM(i,5) - roM(i,22))*1000;
		 CM_y << (roM(i,6) - roM(i,23))*1000;
		 ////////////////////////
		 ref_to_refcat << j;
		 if (uw==1)
		 {
		   P_x << pow(roM(i,13),2);
		   P_y << pow(roM(i,14),2);
		 }
		 else
		 {
		    P_x << 1;P_y << 1;
		 }
	 }
   }
   ////////////
   if (uw==1)
		 {
			 double min_err =  MinOfVector(P_x);
			 for (j=0;j<P_x.count();j++)P_x[j] = min_err/P_x[j];
			 min_err =  MinOfVector(P_y);
			 for (j=0;j<P_y.count();j++)P_y[j] = min_err/P_y[j];
		 }
	 ///////////////////////
	 Lx.resize(C_x.count());
	 Ly.resize(C_x.count());
	 Px.resize(C_x.count());
	 Py.resize(C_x.count());
	 C.resize(C_x.count(),3);
	 for (j=0;j<C_x.count();j++)
	 {
	  C(j,0) = C_x[j];C(j,1) = C_y[j];C(j,2) = 1;
	  Lx(j) = L_x[j];Ly(j) = L_y[j];
	  Px(j) = P_x[j];Py(j) = P_y[j];
	 }
	 ////////////////////////
	 double n_star1 = (double)C_x.count(); 
	 ////////////////////////
	 RSx = LSM_corr(Lx,C,Px,max_res);
	 RSy = LSM_corr(Ly,C,Py,max_res);
	 double n_star_x = RSx(0,0);
	 double n_star_y = RSy(0,0);
	 ///////////////////
	 double uwe_x = RSx(0,1);
	 double uwe_y = RSy(0,1);
	 for (j=0;j<3;j++)//frame parameters
	 {
	   Zx(j) = RSx(j+1,0);
	   Zy(j) = RSy(j+1,0);
	 }
	 ////////////////////
	 matrix<double> Cm(K,3);
	 for (i=0;i<K;i++)
	 {
	    Cm(i,0) = moM(i,5) - x_oc;
		Cm(i,1) =  y_oc - moM(i,6);
		Cm(i,2) = 1;
	 }
	 //tangential coordinates of measured objects
	 vector<double> Lmx = prod(Cm,Zx);
	 vector<double> Lmy = prod(Cm,Zy);
	 //////////////////////////
	 //QMessageBox::information(0,"debug","ok3",QMessageBox::Ok,0,0);
	 //output the results
	 resOUT.resize(Lmx.size()+Lx.size()+4,20);
	 vector<double> EQ(2);
	 vector<double> TPO(2);
	 for (i=0;i<Lmx.size();i++)
	 {
	    EQ = getTangToRaDe(Lmx(i), Lmy(i), ra_oc_rec, de_oc_rec);
		TPO = getRaDeToTang(cel_pos(i,0), cel_pos(i,1), ra_oc_rec, de_oc_rec);
		resOUT(i,0) = EQ(0);
		resOUT(i,1) = EQ(1);
		resOUT(i,2) = date_obs_indx;
		resOUT(i,3) = (Lmx(i) - TPO(0))*1000;
		resOUT(i,4) = (Lmy(i) - TPO(1))*1000;
	 }
	 //////////////////////////////////
	 resOUT(Lmx.size(),0) = n_star_x;//number of reference stars for x-solution
	 resOUT(Lmx.size(),1) = uwe_x;// unit weight error for x-solution
	 resOUT(Lmx.size(),2) = n_star_y;//number of reference stars for y-solution
	 resOUT(Lmx.size(),3) = uwe_y;// unit weight error for y-solution
	 resOUT(Lmx.size(),4) = n_star1;////number of reference stars before solution
	 resOUT(Lmx.size(),5) = ra_oc_indx;//initial optical center position
	 resOUT(Lmx.size(),6) = de_oc_indx;
	 resOUT(Lmx.size(),7) = ra_oc_rec;//final optical center position
	 resOUT(Lmx.size(),8) = de_oc_rec;
	 resOUT(Lmx.size(),9) = x_oc;//pixel optical center position
	 resOUT(Lmx.size(),10) = y_oc;
	 resOUT(Lmx.size(),11) = ang_b_dir(ra_oc_indx, de_oc_indx, ra_oc_rec, de_oc_rec)/1000;
	 /////////////////////////////
	 for (i=1;i<4;i++)
	 {
		resOUT(Lmx.size()+i,0) = RSx(i,0);
		resOUT(Lmx.size()+i,1) = RSx(i,1);
		resOUT(Lmx.size()+i,2) = RSy(i,0);
		resOUT(Lmx.size()+i,3) = RSy(i,1);
	 }
	 /////////////////////////////
	 double x,y,yr,xr,ksi,eta;
	 for (i=0;i<Lx.size();i++)
	 {
		x =  C(i,0);y =  C(i,1);
		ksi =  Lx(i);eta =  Ly(i);
		resOUT(Lmx.size()+4+i,0) = x;//pixel position of reference star
		resOUT(Lmx.size()+4+i,1) = y;
		//////////////////////////////////////
		yr = ((eta-Zy(2))-Zy(0)*(ksi-Zx(2))/Zx(0))/(Zy(1)-Zx(1)*Zy(0)/Zx(0));
		xr = ((ksi-Zx(2))-Zx(1)*yr)/Zx(0);
		//////////////////////////////////////
		resOUT(Lmx.size()+4+i,2) = (xr-x)*1000;//residuals in pixel reference frame
		resOUT(Lmx.size()+4+i,3) = (yr-y)*1000;
		//////////////////////////////////////
		resOUT(Lmx.size()+4+i,4) = ksi;//tangential coordinates of reference star
		resOUT(Lmx.size()+4+i,5) = eta;
		resOUT(Lmx.size()+4+i,6) = (ksi - (Zx(0)*x+Zx(1)*y+Zx(2)))*1000;//residuals in tangential coordinates
		resOUT(Lmx.size()+4+i,7) = (eta - (Zy(0)*x+Zy(1)*y+Zy(2)))*1000;
		////////////////////////////////////////////
		resOUT(Lmx.size()+4+i,8) = CM_x[i];//residuals 'profile' - 'center of mass'
		resOUT(Lmx.size()+4+i,9) = CM_y[i];
		/////////////////////////////////////////////
		if (rcat==0)
		{
		  resOUT(Lmx.size()+4+i,10) =refcat(ref_to_refcat[i],2);// ucac2 mag
		  resOUT(Lmx.size()+4+i,11) =refcat(ref_to_refcat[i],18);// 2mass J
		  resOUT(Lmx.size()+4+i,12) =refcat(ref_to_refcat[i],19);// 2mass H
		  resOUT(Lmx.size()+4+i,13) =refcat(ref_to_refcat[i],20);// 2mass Ks
		  resOUT(Lmx.size()+4+i,14) =refcat(ref_to_refcat[i],23);// ucac2 number
		}
	 }
	 ////////////////////////////
	 QString str ="The results of astrometric reduction of " + FNList.at ( indexF ) +"\n";
	 if (rcat==0) str = str + "reference catalogue = ucac2\n";
	 str = str + "mjd = "+QString( "%1" ).arg( date_obs_indx,15,'f',7,QLatin1Char( ' ' ))+"\n";
	 ////////////////////
	 DATEOBS dateObs = getDATEOBSfromMJD(date_obs_indx);
	 str = str + "central moment (UTC) = "+ getStrFromDATEOBS(dateObs,":", 1, 3)+"\n";
	 str = str +"central moment (mpc format) = "+ getStrFromDATEOBS(dateObs," ", 0, 5)+"\n";
	 ////////////////////
	 str = str + "measured object(s):\n";
	 str = str + "RA (h,m,s)     DEC (deg, arcmin, arcsec)\n";
	 for (i=0;i<Lmx.size();i++)
	 {
	    str = str+" "+pimgf->objStrList[i]+" "+mas_to_hms(resOUT(i,0), ":",5) +" ";
		str = str +mas_to_damas(resOUT(i,1), ":",4)+" ";
		str = str +QString( "%1" ).arg( resOUT(i,3),6,'f',1,QLatin1Char( ' ' ))+" ";
		str = str +QString( "%1" ).arg( resOUT(i,4),6,'f',1,QLatin1Char( ' ' ))+"\n";
		//str = str + QString( "%1" ).arg( resOUT(i,2),15,'f',7,QLatin1Char( ' ' ))+"\n";
	 }
	 str = str + "=================================\n";
	 str = str + "info:\n";
	 str = str + "n_x =" +QString( "%1" ).arg( n_star_x,3,'f',0,QLatin1Char( ' ' ));
	 str = str + "; n_y =" +QString( "%1" ).arg( n_star_y,3,'f',0,QLatin1Char( ' ' ));
	 str = str + "; n =" +QString( "%1" ).arg( n_star1,3,'f',0,QLatin1Char( ' ' ))+"\n";
	 str = str + "initial optical center (RA,DEC):\n";
	 str = str +mas_to_hms(ra_oc_indx, ":",3) +" ";
	 str = str +mas_to_damas(de_oc_indx, ":",2)+"\n";
	 str = str + "final optical center (RA,DEC):\n";
	 str = str +mas_to_hms(ra_oc_rec, ":",3) +" ";
	 str = str +mas_to_damas(de_oc_rec, ":",2)+"\n";
	 str = str + "oc offset(arcmin) =" +QString( "%1" ).arg(resOUT(Lmx.size(),11)/60,4,'f',2,QLatin1Char( ' ' ))+"\n";
	 str = str + "pixel optical center (x,y):\n";
	 str = str +QString( "%1" ).arg( x_oc,6,'f',2,QLatin1Char( ' ' )) +" ";
	 str = str +QString( "%1" ).arg( y_oc,6,'f',2,QLatin1Char( ' ' ))+"\n";
	 //////////////////////////////
	 str = str + "=================================\n";
	 str = str + "astrometric reduction parameters:\n";
	 str = str + "unit weight errors (mas):\n";
	 str = str +"uwe_ksi = "+QString( "%1" ).arg( uwe_x*1000,6,'f',2,QLatin1Char( ' ' ));
	 str = str +"; uwe_eta = "+QString( "%1" ).arg( uwe_y*1000,6,'f',2,QLatin1Char( ' ' ))+"\n";
	 str = str + "  Zksi         error_ksi    Zeta         error_eta\n";
	 /////////////////////////////
	 for (i=1;i<4;i++)
	 {
	    str = str +QString( "%1" ).arg( resOUT(Lmx.size()+i,0),12,'f',8,QLatin1Char( ' ' ))+" ";
		str = str +QString( "%1" ).arg( resOUT(Lmx.size()+i,1),12,'f',8,QLatin1Char( ' ' ))+" ";
		str = str +QString( "%1" ).arg( resOUT(Lmx.size()+i,2),12,'f',8,QLatin1Char( ' ' ))+" ";
	    str = str +QString( "%1" ).arg( resOUT(Lmx.size()+i,3),12,'f',8,QLatin1Char( ' ' ))+"\n";
	 }
	 /////////////////////////////
	 str = str + "=================================\n";
	 str = str + "residuals:\n";
	 str = str + "    x       y     delta_x delta_y| ksi       eta      d_ksi  d_eta |    cmx    cmy |";
	 if (rcat==0) str = str + "U2Rmag    J     H     Ks     ucac2_n\n";
	 QString res_str="";
	 for (i=0;i<Lx.size();i++)
	 {
		res_str = res_str +QString( "%1" ).arg(resOUT(Lmx.size()+4+i,0),8,'f',3,QLatin1Char( ' ' ))+" ";
		res_str = res_str +QString( "%1" ).arg(resOUT(Lmx.size()+4+i,1),8,'f',3,QLatin1Char( ' ' ))+" ";
		res_str = res_str +QString( "%1" ).arg(resOUT(Lmx.size()+4+i,2),7,'f',1,QLatin1Char( ' ' ))+" ";
	    res_str = res_str +QString( "%1" ).arg(resOUT(Lmx.size()+4+i,3),7,'f',1,QLatin1Char( ' ' ))+" ";
		res_str = res_str +QString( "%1" ).arg(resOUT(Lmx.size()+4+i,4),8,'f',3,QLatin1Char( ' ' ))+" ";
		res_str = res_str +QString( "%1" ).arg(resOUT(Lmx.size()+4+i,5),8,'f',3,QLatin1Char( ' ' ))+" ";
		res_str = res_str +QString( "%1" ).arg(resOUT(Lmx.size()+4+i,6),7,'f',1,QLatin1Char( ' ' ))+" ";
	    res_str = res_str +QString( "%1" ).arg(resOUT(Lmx.size()+4+i,7),7,'f',1,QLatin1Char( ' ' ))+" ";
		res_str = res_str +QString( "%1" ).arg(resOUT(Lmx.size()+4+i,8),7,'f',1,QLatin1Char( ' ' ))+" ";
	    res_str = res_str +QString( "%1" ).arg(resOUT(Lmx.size()+4+i,9),7,'f',1,QLatin1Char( ' ' ))+" ";
		if (rcat==0)
		{
		  res_str = res_str +QString( "%1" ).arg(resOUT(Lmx.size()+4+i,10),5,'f',2,QLatin1Char( ' ' ))+" ";
		  res_str = res_str +QString( "%1" ).arg(resOUT(Lmx.size()+4+i,11),6,'f',3,QLatin1Char( ' ' ))+" ";
		  res_str = res_str +QString( "%1" ).arg(resOUT(Lmx.size()+4+i,12),6,'f',3,QLatin1Char( ' ' ))+" ";
		  res_str = res_str +QString( "%1" ).arg(resOUT(Lmx.size()+4+i,13),6,'f',3,QLatin1Char( ' ' ))+" ";
		  res_str = res_str +QString( "%1" ).arg(resOUT(Lmx.size()+4+i,14),10,'f',0,QLatin1Char( ' ' ))+" ";
		}
		res_str = res_str+"\n";
	 }
	 str = str + res_str;
	 str = str+"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n\n";
	 /////////////////////////////
	 pimgf->simple_message = str;
	 QFileInfo fi(FNList.at ( indexF ));
	 QString fn = fi.baseName();
	 QString tn = fn.section('_',0,0);
	 int targ_n = tn.toInt();
	 if (saveLog>0)
	 {
	  QFile outputfile(pimgf->log_path+"/"+fn+".txt");
      outputfile.open(QIODevice::Append | QIODevice::Text);
      QTextStream out_stream;
      out_stream.setDevice(&outputfile);
	  out_stream << str;   
      outputfile.close();
	 }
	 if (saveLog==2)
	 {
	  QFile logfile(pimgf->log_path+"/"+mpcFName+".log");
      logfile.open(QIODevice::Append | QIODevice::Text);
      QTextStream log_stream;
      log_stream.setDevice(&logfile);
	  log_stream << res_str;   
      logfile.close();
	 }
	 if((mpcFName!="n")&&(mpcFName!="sep"))
	 {
	  QString mpc_str = "";
	  for (i=0;i<Lmx.size();i++)
	  {
	    mpc_str = mpc_str +pimgf->objStrList[i]+"         C";
		mpc_str = mpc_str +getStrFromDATEOBS(dateObs," ", 0, 5) +" ";
	    mpc_str = mpc_str +mas_to_hms(resOUT(i,0), " ",3);
		mpc_str = mpc_str +mas_to_damas(resOUT(i,1), " ",2);
		mpc_str = mpc_str  +"         00.0        "+pimgf->obs_code+"\n";
	  }
	  QFile mpcfile(pimgf->log_path+"/"+mpcFName+".txt");
      mpcfile.open(QIODevice::Append | QIODevice::Text);
      QTextStream mpc_stream;
      mpc_stream.setDevice(&mpcfile);
	  mpc_stream << mpc_str;   
      mpcfile.close();
	 }
	 if(mpcFName=="sep")
	 {
	   for (i=0;i<Lmx.size();i++)
	   {
	     QString mpc_str = "";
	     mpc_str = mpc_str +pimgf->objStrList[i]+"         C";
		 mpc_str = mpc_str +getStrFromDATEOBS(dateObs," ", 0, 5) +" ";
	     mpc_str = mpc_str +mas_to_hms(resOUT(i,0), " ",3);
		 mpc_str = mpc_str +mas_to_damas(resOUT(i,1), " ",2);
		 mpc_str = mpc_str  +"         00.0        "+pimgf->obs_code+"\n";
		////////////////////////////////////////
		QFile mpcfile(pimgf->log_path+"/"+pimgf->objStrList[i]+"_mpc.txt");
        mpcfile.open(QIODevice::Append | QIODevice::Text);
        QTextStream mpc_stream;
        mpc_stream.setDevice(&mpcfile);
	    mpc_stream << mpc_str;   
        mpcfile.close();
		////////////////////////////////////////
		QString ourf_str = "";
		 ourf_str = ourf_str +getStrFromDATEOBS(dateObs," ", 0, 5) +" ";
	     ourf_str = ourf_str +mas_to_hms(resOUT(i,0), " ",4) +" ";
		 ourf_str = ourf_str +mas_to_damas(resOUT(i,1), " ",3) +"\n";
		 /////////////////////////////////////////////////////////////////////////////////
		 QFile ourf_file(pimgf->log_path+"/"+pimgf->objStrList[i]+"_"+pimgf->obs_code+".txt");
         ourf_file.open(QIODevice::Append | QIODevice::Text);
         QTextStream ourf_stream;
         ourf_stream.setDevice(&ourf_file);
	     ourf_stream << ourf_str;   
         ourf_file.close();
		////////////////////////////////////////
	   }
	 }
	 if((diff_sat==1)&&(Lmx.size()>1))
	 {
	   QString sat_str;
	   for (i=0;i<Lmx.size();i++)
	   {
	     for (j=i+1;j<Lmx.size();j++)
		 {
		   sat_str="";
		   sat_str = sat_str + getStrFromDATEOBS(dateObs," ", 0, 5);
		   sat_str = sat_str + " " + QString( "%1" ).arg(Lmx(j) - Lmx(i),10,'f',3,QLatin1Char( ' ' ));
           sat_str = sat_str + " " + QString( "%1" ).arg(Lmy(j) - Lmy(i),10,'f',3,QLatin1Char( ' ' ));
           sat_str = sat_str + "\n";
           ////////////////////////////////////////
		   //QMessageBox::information(0,"debug",sat_str,QMessageBox::Ok,0,0);
			QFile satfile(pimgf->log_path+"/"+pimgf->objStrList[j]+"-"+pimgf->objStrList[i] +".txt");
	        satfile.open(QIODevice::Append | QIODevice::Text);
	        QTextStream sat_stream;
	        sat_stream.setDevice(&satfile);
		    sat_stream << sat_str;   
	        satfile.close();
		   
		////////////////////////////////////////		   
		 }
		 
	   }
	 }
	 if(signal_out==1)signalProcCompleted();
	 ///////////////////////////////
};

void CCDFrame::saveDates()
{
  QString str = "";
  for (int i =0; i<mjdList.count();i++)
  {
	str = str + QString( "%1" ).arg(mjdList[i]+2400000.5,19,'f',11,QLatin1Char( ' ' ))+"\n";
  }
  QFile outputfile(pimgf->log_path+"/"+"date_obs.txt");
  outputfile.open(QIODevice::WriteOnly | QIODevice::Text);
  QTextStream out_stream;
  out_stream.setDevice(&outputfile);
  out_stream << str;   
  outputfile.close();
};

void CCDFrame::getEph()
{
  QString str = pimgf->EphStr;
  QTextStream in(&str, QIODevice::ReadOnly);
  QStringList eph_list;
  while (!in.atEnd()) eph_list << in.readLine();
  QStringList Line;
  cel_pos.resize(eph_list.count(), 2);
  QString spos;
  for (int i=0;i<eph_list.count();i++)
  {
    str = eph_list[i];
	str = str.trimmed();
    Line.clear();
    Line = str.split(" ");
    spos = Line[6]+" "+	Line[7]+" "+Line[8];
	QMessageBox::information(0,"debug",spos,QMessageBox::Ok,0,0);
	cel_pos(i,0) = hms_to_mas(spos, " ");
	spos = Line[9]+" "+	Line[10]+" "+Line[11];
	QMessageBox::information(0,"debug",spos,QMessageBox::Ok,0,0);
	cel_pos(i,1) = damas_to_mas(spos, " ");
  }
};

void CCDFrame::slotUpdateOC(double ra, double de)
{
  for (int i=0;i<raList.count();i++)
  {
	  raList[i] = ra;
	  deList[i] = de;
  }
  pimgf->ra_oc = ra;
  pimgf->de_oc = de;
  //QMessageBox::information(0,"debug",mas_to_hms(pimgf->ra_oc, ":",3)+" "+mas_to_damas(pimgf->de_oc, ":",2),QMessageBox::Ok,0,0);
};

void CCDFrame::hFilter(int indexF, int size)
{
   long *nx = pl_naxes.at (indexF);
   histFilter(pl_fCCDdata.at(indexF), nx[0], nx[1], size);
   getRangeIndx(indexF);
   getHistIndx(indexF);
   //QMessageBox::information(0,"debug","filter",QMessageBox::Ok,0,0);
   getHistPrIndx(indexF);
   setImgDataIndx(indexF);
   if(indexF == pl_index)
     {
	   SetFrame(indexF);
       emitImageProcessed(0);
     }
};

void CCDFrame::mFilter(int indexF, int size, int use_pb)
{
   long *nx = pl_naxes.at (indexF);
   medianFilter(pl_fCCDdata.at(indexF), nx[0], nx[1], size, progressBar,use_pb);
   getRangeIndx(indexF);
   getHistIndx(indexF);
   //QMessageBox::information(0,"debug","filter",QMessageBox::Ok,0,0);
   getHistPrIndx(indexF);
   setImgDataIndx(indexF);
   if(indexF == pl_index)
     {
	   SetFrame(indexF);
       emitImageProcessed(0);
     }
};

void CCDFrame::setImgDataAsIndx(int index)
{
    double *ftrs_ndx = pl_features.at (index);
	double *ftrs_cur;
    for (int i=0;i<pl_features.count();i++)	
	{
	  if(i!=index)
	  {
	     ftrs_cur = pl_features.at(i);
		 ftrs_cur[3] = ftrs_ndx[3];
		 ftrs_cur[4] = ftrs_ndx[4];
		 ftrs_cur[6] = ftrs_ndx[6];
		 ftrs_cur[7] = ftrs_ndx[7];
		 invList[i] = invList[index];
		 setImgDataIndx(i);
		 SetFrame(i);
         emitImageProcessed(0);
	  }
	}
	emitImageProcessed(0);
};
///////////////////////////signals

void CCDFrame::emitImageProcessed(int state)
{
 emit signalImageProcessed(state);
};

void CCDFrame::emitMeasured()
{
 emit signalMeasured();
};

void CCDFrame::emitSignalPSF()
{
 emit signalPSF();
};

void CCDFrame::emitSignalDone()
{
 emit signalDone();
};

