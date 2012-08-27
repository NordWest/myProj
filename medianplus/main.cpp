#include <QCoreApplication>
#include <QtCore>
#include "./../libs/fitsio.h"
#include "math.h"

unsigned short freqpix(unsigned short *fdata, quint32 nelements)
{
	unsigned short maxv = fdata[0];
	unsigned short minv = fdata[0];
	for(quint32 k=0;k<nelements;k++)
	{
		if(fdata[k]>maxv)maxv=fdata[k];
		if(fdata[k]<minv)minv=fdata[k];
	}
	int hsize = maxv-minv+1;
	quint32 *pf = new quint32[hsize];
	for(int i=0;i<hsize;i++) pf[i]=0;
	for(quint32 k=0;k<nelements;k++) pf[fdata[k]-minv]++;
	int fpn=0;
	for(int i=0;i<hsize;i++) if(pf[i]>pf[fpn])fpn=i;
	fpn = fpn + minv;
	return fpn;
};


int main(int argc, char *argv[])
{
/*    	QTextStream out_stream;
	QFile fout("fout.dat");
	fout.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
	out_stream.setDevice(&fout);	
	out_stream << "\nBEGIN\n";
	*/	
		QCoreApplication app(argc, argv);
		QTextStream stream(stdout);
		stream << "\n medianplus started....\n";
		//BEGIN arguments
		QString argS;
		int argI;
		if(app.arguments().count()>1)
		{
			argS = QString(argv[1]);
			argI = argS.toInt();
		}
		else 
		{
			stream << "error.\n";
			return 0;
		}
		//END arguments

		
		
		//BEGIN list of files
//		out_stream << "\nBEGIN list of files\n";
		QDir dir;
		QStringList filters;
		filters << "*.fit" << "*.fits" << "*.fts";
		dir.setNameFilters(filters); 
	    QStringList flist = dir.entryList();
		for (int i=0;i<flist.count();i++) stream << flist.at(i) <<"\n";
		//END list of files
		 
		//BEGIN load files
//		out_stream << "\nBEGIN load files\n";
		QString filename;
		QByteArray ba_fname;
		fitsfile *fptr;
		int status = 0;
		char *fname;
		long naxes[2];
		long felem = 1;
		LONGLONG nelements;
		int anynul = 0;
		unsigned short *fits_buff;
		QList<unsigned short*> dlist;
		QVector<double> kappa;
		unsigned short meanpix,meanpix0;
		double mpix;
		for (int i=0;i<flist.count();i++)
		{
			filename = flist.at(i);
			ba_fname = filename.toAscii();
			fname = ba_fname.data();
			fits_open_file(&fptr, fname , READONLY,&status);
			fits_get_img_size(fptr, 2, naxes, &status);
			nelements = naxes[0]*naxes[1];
			fits_buff = new unsigned short[nelements];
			fits_read_img(fptr, TUSHORT, felem, nelements, NULL, fits_buff, &anynul, &status);
			fits_close_file(fptr, &status);
			if(argI==1)
			{
				mpix = 0;
				for (LONGLONG k=0;k<nelements;k++) mpix = mpix + (double)fits_buff[k];
				meanpix = (unsigned short)(mpix/nelements);
				if(i==0)
				{
					meanpix0 = meanpix;
					kappa << 1;
				}
				else
				{
					kappa << ((double)meanpix0)/((double)meanpix);
				}
			}
			if(argI==2)
			{
				meanpix = freqpix(fits_buff, nelements);
				if(i==0)
				{
					meanpix0 = meanpix;
					kappa << 1;
				}
				else
				{
					kappa << ((double)meanpix0)/((double)meanpix);
				}
			}
			if(argI==0) kappa << 1;
			dlist << fits_buff;
		}
		//END load files
		//BEGIN filtering
		unsigned short *res_buff = new unsigned short[nelements];
		unsigned short *pixdata = new unsigned short[flist.count()];
		unsigned short max_el;
		int max_index;
		for (LONGLONG k=0;k<nelements;k++)
		{
			for (int i=0;i<flist.count();i++)
			{
				fits_buff = dlist.at(i);
				pixdata[i] =(unsigned short)( kappa[i]*(double)fits_buff[k]);
			}
			//BEGIN sort
			for (int i=0;i<(flist.count()/2+1);i++)
			{
				max_el = pixdata[0];
				max_index = 0;
				for (int j=0;j<flist.count();j++)
				{
					if(pixdata[j]>max_el)
						{
							max_el = pixdata[j];
							max_index = j;
						}
				}
				pixdata[max_index] = 0;
			}
			//END sort
			res_buff[k] = max_el;
		}
		//END filtering
		//BEGIN save results
		fitsfile *nfptr;
		fits_create_file(&nfptr, "!medianplus.fit", &status);
		fits_create_hdu(nfptr, &status);
		int  naxis = 2;
		fits_create_img(nfptr, USHORT_IMG, naxis, naxes, &status);
		char orig[] = "flat field formed by medianplus";
		fits_update_key(nfptr, TSTRING, "ORIGIN", orig , "", &status);
		LONGLONG fpixel = 1;
		fits_write_img(nfptr, TUSHORT, fpixel, nelements, res_buff, &status);
		fits_close_file(nfptr, &status);
		delete res_buff;
		//END save results
		//BEGIN delete data
		for (int i=0;i<flist.count();i++)
		{
			fits_buff = dlist.at(i);
			delete fits_buff;
		}
		//END delete data
		stream << "\n medianplus finished...\n";
		return 0;  
	}
