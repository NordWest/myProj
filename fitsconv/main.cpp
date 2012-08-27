#include <QApplication>
#include <QtGui>
//#include "OpenFileExDlg.h"
#include "./../libs/fitsio.h"
#include <iostream.h>
#include <QtCore>
#include "math.h"
#include "./../libs/astro.h"

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>

//FitsConv
/*
typedef struct __IMGF__
{
  double ADCmax;
  double ADCmin;
  double ADCmean;
  double lHist; 
  double rHist;
  double maxHist;
  double fr_pix;
  double leftB;
  double rightB;
  double gamma;
  quint16 xSize;
  quint16 ySize;
  uchar* imgCCD;
  double *fCCD;
  quint32 *histOfImg;
  matrix <double> *rMtrx;
  matrix <double> *pr_ol;
  matrix<double> *pm_ol;
  matrix<double> *p_refcat;//pointer to matrix with reference stars
  matrix<double> *p_cel_pos;
  matrix<double> *p_tangpos;//pointer to tangential coordinates
  matrix<double> *p_resOUT;//pointer to result output
  int max_num_it;  
  int max_num_reg;
  int frIndex;
  double min_par_step;
  double min_uwe_step;
  bool abort;
  int num_of_frames;
  QString file_name;
  QString measured_file_name;
  QString keyStr;
  QString simple_message;
  QString ucac2_path;
  QString log_path;
  QString EphStr;//EPHEMERIS OF SOLAR SYSTEM BODIES 
  QString obs_code;
  QStringList objStrList;
  bool isOldNA;
  int refCCDFrame;
  double max_residual;
  int inv;
  double ra_oc;
  double de_oc;
  double fov;
  double flength;
  double xpsize;
  double ypsize;
  double curr_mjd;
  double mag_1;
  double mag_2;
}IMGF,*PIMGF;
*/

int main(int argc, char *argv[])
{  
        QApplication app(argc, argv);
		///////////////////////////
		//QTranslator translator;
        //translator.load("astrored2_ru");
        //app.installTranslator(&translator);
        /////////////////////////////////
/*         COpenFileExDlg *mainWin = new COpenFileExDlg;
        mainWin->show(); */
		
		int rate = atoi(argv[2]);
		
		int n, m, k, hdus;
		quint32 i;
		char *card = new char[FLEN_CARD];

		char *errstr;
		errstr = new char[30];
		errstr = "";

		QFile fout("fout.dat");
		fout.open(QIODevice::WriteOnly | QIODevice::Text);
		
		QTextStream out_stream;
		out_stream.setDevice(&fout);
		out_stream << QString("rate %1").arg(rate) << endl;
		
		int hdunum0, hdunum1;
		
		char *fnm = argv[1];
		
		fitsfile *fptr;
		fitsfile *fptr_out;
		int status = 0;//status
		int stat_out = 0;
		fits_open_file(&fptr, fnm, READONLY, &status);
		if(status) 
		{
			out_stream << "error open fits" << endl;
			return 1;
		}
		status = 0;
		fits_get_hdrspace(fptr, &hdunum0, NULL, &status);
		status = 0;
		fits_get_num_hdus(fptr, &hdus, &status);
		status = 0;
		if(status) 
		{
			out_stream << "error hdus" << endl;
			return 1;
		}
//		QString str = QString("test %1").arg(11, 2);
		//
		out_stream << QString("hdrspace0 %1").arg(hdunum0) << endl;
		out_stream << QString("num_hdus %1").arg(hdus) << endl;
///////////////////////////////////////
///////Create new file	///////////////////////////////////

		char *suff = new char[256];
		int len = strlen(argv[1]);
		char *pos = strrchr(argv[1], 46);
		int len1 = strlen(pos);
		strcpy(suff, "");
		strncpy(suff, argv[1], len-len1);
		strcpy(&suff[len-len1], "_sc.fits");

		fits_open_file(&fptr_out, suff, READWRITE, &stat_out);
		out_stream << QString("open_old %1").arg(stat_out) << endl;
		stat_out = 0;
		fits_delete_file(fptr_out, &stat_out);
		out_stream << QString("delete %1").arg(stat_out) << endl;
		stat_out = 0;
		fits_create_file(&fptr_out, suff, &stat_out);
		out_stream << QString("create %1").arg(stat_out) << endl;
		stat_out = 0;
		
//////////////////////////////////////////////////////
		long *naxes, *naxes_ex, *na32;
		long nelements;
		long nelements_ex;
		quint16 XSize, YSize;
		int bpix;
		double *fCCDdata;
//		IMGF pimgf;
		double exp_time;
		double date_obs;
		QString date_obs_str;
		
		
		status = 0;
		fits_get_img_equivtype(fptr, &bpix, &status);
		 
		out_stream << QString("get_img_equivtype %1").arg(status) << endl;
		status = 0;
		 
		naxes = new long[2];
		na32 = new long[2];
		naxes_ex = new long[2];
		fits_get_img_size(fptr, 2, naxes, &status);
		out_stream << QString("get_img_size %1").arg(status) << endl;
		status = 0;
//////////////////////////////////////////////////////

		fits_create_hdu(fptr_out, &status);
		out_stream << QString("create_hdu %1").arg(status) << endl;
		status = 0;
		
		int jump = (int) pow(2, rate);
		int sc = jump;
		na32[0] = naxes[0] + (sc*(naxes[0]%sc!=0)-naxes[0]%sc);
		na32[1] = naxes[1] + (sc*(naxes[1]%sc!=0)-naxes[1]%sc);
		nelements = naxes[0]*naxes[1];
		
		long x_ex, y_ex;
		
/*		if(hdunum0==10)
		{
 			x_ex = na32[1]*pow(2.0, -rate);
			y_ex = na32[0]*pow(2.0, -rate);
			
			naxes_ex[0] = x_ex;
			naxes_ex[1] = y_ex;
			fits_create_img(fptr, USHORT_IMG, 2, naxes_ex, &status);
		}
		else
		{*/
 			x_ex = na32[0]*pow(2.0, -rate);
			y_ex = na32[1]*pow(2.0, -rate);
			naxes_ex[0] = x_ex;
			naxes_ex[1] = y_ex;
//		}
		
		out_stream << QString("naxes[0] %1\tna32[0] %2\tnaxes_ex[0] %3").arg(naxes[0]).arg(na32[0]).arg(naxes_ex[0]) << endl;
		out_stream << QString("naxes[1] %1\tna32[1] %2\tnaxes_ex[1] %3").arg(naxes[1]).arg(na32[1]).arg(naxes_ex[1]) << endl;
				
		switch(bpix)
		{
			case USHORT_IMG:
				fits_create_img(fptr_out, USHORT_IMG, 2, naxes_ex, &status);
				break;
			case SHORT_IMG:
				fits_create_img(fptr_out, SHORT_IMG, 2, naxes_ex, &status);
				break;
			case LONG_IMG:
				fits_create_img(fptr_out, LONG_IMG, 2, naxes_ex, &status);
				break;
			case FLOAT_IMG:
				fits_create_img(fptr_out, FLOAT_IMG, 2, naxes_ex, &status);
				break;
			case DOUBLE_IMG:
				fits_create_img(fptr_out, DOUBLE_IMG, 2, naxes_ex, &status);
				break;
		}
		
		nelements_ex = x_ex*y_ex;
		
		out_stream << QString("create_img %1").arg(status) << endl;
		status = 0;
			
		for(i=0; i<hdunum0; i++)
		{
			fits_read_record(fptr, i, card, &status);
			status = 0;
			fits_write_record(fptr_out, card, &stat_out);
			stat_out = 0;
		}
		
////////////////////////////////////////////////////////
		
		QString str, strl;
		str.setNum(bpix);
		
		int nkeys = hdunum0;
		 
		out_stream << QString("hdrspace %1").arg(nkeys) << endl;
		status = 0;
		 
		str = "";
		for (k = 1; k <= nkeys; k++)
		 {
		   fits_read_record(fptr, k, card, &status);
		   strl = QString::QString(card);
    	   str = str + strl + "\n";
		 }
		 //
		out_stream << QString("read_record %1").arg(status) << endl;
		status = 0;
		 
		 delete card;
		
		 long *fpixel = new long[2];
		 fpixel[0] = 1;
		 fpixel[1] = 1;
		 long *lpixel = new long[2];
		 lpixel[0] = 1;
		 lpixel[1] = 1;
		 long felem = 1;
		 long eelem;
		 int anynul = 0;
		 double fsumm;
		 int j, l, s;
		 ///////////////////////////////////
		 
		out_stream << QString("nelements %1").arg(nelements) << endl;
		status = 0;
		out_stream << QString("nelements_ex %1").arg(nelements_ex) << endl;
		status = 0;
/*		 		 
		if (hdunum0==10)
		{
			
		   unsigned short *fr_data = new unsigned short[nelements];
		   unsigned short *fldata_new = new unsigned short[nelements_ex];
		   out_stream << QString("OLDNA_USHORT_IMG") << endl;
		   status = 0;
		   fits_read_img(fptr, TUSHORT, felem, nelements, NULL, fr_data ,&anynul, &status);
		   out_stream << QString("read_img %1").arg(status) << endl;
		   status = 0;
		   
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
		   delete [] fr_data;
		   XSize = (quint16)naxes[1];
		   YSize = (quint16)naxes[0];
		   naxes[0] = XSize;
		   naxes[1] = YSize;
		   
		   double fsumm;
			for (i=0;i<nelements_ex;i++)
			{
				fsumm = 0;
				for(n=0; n<jump; n++)
				for(m=0; m<jump; m++)
				{
					fsumm += fCCDdata[i*jump+m+naxes[0]*n+naxes[0]*(jump-1)*(i/x_ex)];
				}
	
				fldata_new[i] = fsumm*pow(jump, -2.0);
			}
			fits_write_img(fptr_out, TUSHORT, 1, nelements_ex, (void*) fldata_new, &status);
			out_stream << QString("write_img %1").arg(status) << endl;
			status = 0;
		   
		   
		 }
		 else
		 {
*/			
		 
			if (bpix==USHORT_IMG)  
			{
				out_stream << QString("USHORT_IMG") << endl;
				status = 0;
				
								
				unsigned short *fdata_16 = new unsigned short[na32[0]*jump];
				unsigned short *fldata_new = new unsigned short[naxes_ex[0]];
				
				for(i=0;i<na32[0]*jump; i++) fdata_16[i] = 0;
			
				int na32elems = na32[0]*na32[1];
				out_stream << QString("na32elems\t%1").arg(na32elems) << endl;
				
				long apnd = na32[0] - naxes[0];
				out_stream << QString("razn\t%1").arg(apnd) << endl;
				
				int partpix = naxes[0]*jump;
				out_stream << QString("partpix\t%1").arg(partpix) << endl;
				out_stream << QString("jump\t%1").arg(jump) << endl;
				
				for(k=0; k<naxes_ex[1]; k++)
				{
					s = 0;
					for(i=1; i<=partpix; i += naxes[0])
					{
						fits_read_img(fptr, TUSHORT, i+k*partpix, naxes[0], NULL, &fdata_16[i-1 + apnd*s] ,&anynul, &status);
						s++;
						status = 0;
					}
					
					for(n=0; n<naxes_ex[0]; n++)
					{
						fsumm = 0;
						for(l=0; l<jump; l++)
						{
							for(m=0; m<jump; m++)
							{
								fsumm += fdata_16[n*jump + m + na32[0]*l];
							}
						}
						
						fldata_new[n] = fsumm*pow(jump, -2.0);
					}
					fits_write_img(fptr_out, TUSHORT, k*naxes_ex[0] + 1, naxes_ex[0], (void*) fldata_new, &status);
				}
			
			delete fldata_new;
			
			delete fdata_16;
		  }
		  if (bpix==SHORT_IMG)  
		  { 
		    out_stream << QString("SHORT_IMG") << endl;
			status = 0;
		    short *fdata16 = new short[nelements];
			short *fldata_new = new short[nelements_ex];
			
			
		    fits_read_img(fptr, TSHORT, felem, nelements, NULL, fdata16 ,&anynul, &status);
			
			for (i=0;i<nelements_ex;i++)
			{
				fsumm = 0;
				for(n=0; n<jump; n++)
				for(m=0; m<jump; m++)
				{
					fsumm += fdata16[i*jump+m+naxes[0]*n+naxes[0]*(jump-1)*(i/x_ex)];
				}
	
				fldata_new[i] = fsumm*pow(jump, -2.0);
			}
			
			fits_write_img(fptr_out, TSHORT, 1, nelements_ex, (void*) fldata_new, &status);
			out_stream << QString("write_img %1").arg(status) << endl;
			status = 0;
			
			delete fldata_new;
			delete fdata16;
		  }  
		  if (bpix==LONG_IMG)
		  { 
		    out_stream << QString("LONG_IMG") << endl;
			status = 0;
				long *fdata32 = new long[na32[0]*jump];
				long *fldata_new = new long[naxes_ex[0]];
				for(i=0;i<na32[0]*jump; i++) fdata32[i] = 0;
			
				int na32elems = na32[0]*na32[1];
				out_stream << QString("na32elems\t%1").arg(na32elems) << endl;
				
				long apnd = na32[0] - naxes[0];
				out_stream << QString("razn\t%1").arg(apnd) << endl;
				
				int partpix = naxes[0]*jump;
				out_stream << QString("partpix\t%1").arg(partpix) << endl;
				out_stream << QString("jump\t%1").arg(jump) << endl;
				
				for(k=0; k<naxes_ex[1]; k++)
				{
					s = 0;
					for(i=1; i<=partpix; i += naxes[0])
					{
						fits_read_img(fptr, TLONG, i+k*partpix, naxes[0], NULL, &fdata32[i-1 + apnd*s] ,&anynul, &status);
						s++;
						status = 0;
					}
					
					for(n=0; n<naxes_ex[0]; n++)
					{
						fsumm = 0;
						for(l=0; l<jump; l++)
						{
							for(m=0; m<jump; m++)
							{
								fsumm += fdata32[n*jump + m + na32[0]*l];
							}
						}
						
						fldata_new[n] = fsumm*pow(jump, -2.0);
					}
					fits_write_img(fptr_out, TFLOAT, k*naxes_ex[0] + 1, naxes_ex[0], (void*) fldata_new, &status);
				}

			
			delete fldata_new;
			delete fdata32;
		  }
		  
		if (bpix==FLOAT_IMG)
		{ 
//			
				float *fldata = new float[na32[0]*jump];
				float *fldata_new = new float[naxes_ex[0]];
				for(i=0;i<na32[0]*jump; i++) fldata[i] = 0;
			
				int na32elems = na32[0]*na32[1];
				out_stream << QString("na32elems\t%1").arg(na32elems) << endl;
				
				long apnd = na32[0] - naxes[0];
				out_stream << QString("razn\t%1").arg(apnd) << endl;
				
				int partpix = naxes[0]*jump;
				out_stream << QString("partpix\t%1").arg(partpix) << endl;
				out_stream << QString("jump\t%1").arg(jump) << endl;
				
				for(k=0; k<naxes_ex[1]; k++)
				{
					s = 0;
					for(i=1; i<=partpix; i += naxes[0])
					{
						fits_read_img(fptr, TFLOAT, i+k*partpix, naxes[0], NULL, &fldata[i-1 + apnd*s] ,&anynul, &status);
						s++;
						status = 0;
					}
					
					for(n=0; n<naxes_ex[0]; n++)
					{
						fsumm = 0;
						for(l=0; l<jump; l++)
						{
							for(m=0; m<jump; m++)
							{
								fsumm += fldata[n*jump + m + na32[0]*l];
							}
						}
						
						fldata_new[n] = fsumm*pow(jump, -2.0);
					}
					fits_write_img(fptr_out, TFLOAT, k*naxes_ex[0] + 1, naxes_ex[0], (void*) fldata_new, &status);
				}
			
			
			delete fldata_new;
			
			delete fldata;
			
		  }
		  if (bpix==DOUBLE_IMG)
			{
				double *fdata_16 = new double[na32[0]*jump];
				double *fldata_new = new double[naxes_ex[0]];
				
				for(i=0;i<na32[0]*jump; i++) fdata_16[i] = 0;
			
				int na32elems = na32[0]*na32[1];
				out_stream << QString("na32elems\t%1").arg(na32elems) << endl;
				
				long apnd = na32[0] - naxes[0];
				out_stream << QString("razn\t%1").arg(apnd) << endl;
				
				int partpix = naxes[0]*jump;
				out_stream << QString("partpix\t%1").arg(partpix) << endl;
				out_stream << QString("jump\t%1").arg(jump) << endl;
				
				for(k=0; k<naxes_ex[1]; k++)
				{
					s = 0;
					for(i=1; i<=partpix; i += naxes[0])
					{
						fits_read_img(fptr, TDOUBLE, i+k*partpix, naxes[0], NULL, &fdata_16[i-1 + apnd*s] ,&anynul, &status);
						s++;
						status = 0;
					}
					
					for(n=0; n<naxes_ex[0]; n++)
					{
						fsumm = 0;
						for(l=0; l<jump; l++)
						{
							for(m=0; m<jump; m++)
							{
								fsumm += fdata_16[n*jump + m + na32[0]*l];
							}
						}
						fldata_new[n] = fsumm*pow(jump, -2.0);
					}
					fits_write_img(fptr_out, TDOUBLE, k*naxes_ex[0] + 1, naxes_ex[0], (void*) fldata_new, &status);
				}
			
			delete fldata_new;
			
			delete fdata_16;
		  }
		  XSize = (quint16)naxes[0];
		  YSize = (quint16)naxes[1];
		  

//		 }

		fits_get_hdrspace(fptr_out, &hdunum1, NULL, &status);
		out_stream << QString("hdrspace1 %1").arg(hdunum1) << endl;
		status = 0;
		
 		for(i=hdunum1; i>5; i--)
		{
			fits_delete_record(fptr_out, i, &status);
			status = 0;
		}
		 
		for(i=6; i<=hdunum0; i++)
		{
			fits_read_record(fptr, i, card, &status);
			status = 0;
			fits_write_record(fptr_out, card, &stat_out);
			stat_out = 0;
		}
		
		fits_get_hdrspace(fptr_out, &hdunum1, NULL, &status);
		out_stream << QString("hdrspace1 %1").arg(hdunum1) << endl;
		status = 0;

		fits_write_key_str(fptr_out, "FSOURCE", fnm, "Source file name", &status);
		out_stream << QString("FSOURCE\t%1").arg(status) << endl; 
		status = 0;
		
		sprintf(fnm, "%d", jump);
		fits_write_key_str(fptr_out, "SCFACTOR", fnm, "Scaling factor", &status);
		status = 0;

		fits_close_file(fptr, &status);
		fits_close_file(fptr_out, &stat_out);

		fout.close();
		fits_report_error(stderr, status);
		
        return 0; 
    }
	
