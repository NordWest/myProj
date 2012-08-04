#include <QCoreApplication>
#include <QtCore>
#include "./../libs/fitsio.h"
#include "math.h"


int main(int argc, char *argv[])
    {
		QCoreApplication app(argc, argv);
		QTextStream stream(stdout); 
		stream << "\n flat started....\n";
		//BEGIN arguments
		if(app.arguments().count()==1)
		{
			stream << "error.\n";
			return 0;
		}
		//END arguments
		//BEGIN load flat field
		QString flatfilename = QString(argv[1]);
		fitsfile *flatptr;
		int status = 0;
		long naxes[2];
		long felem = 1;
		int anynul = 0;
		fits_open_file(&flatptr, argv[1],READONLY,&status);
		fits_get_img_size(flatptr, 2, naxes, &status);
		LONGLONG nelements = naxes[0]*naxes[1];
		unsigned short *flat_buff = new unsigned short[nelements];
		fits_read_img(flatptr, TUSHORT, felem, nelements, NULL, flat_buff, &anynul, &status);
		double meanpix=0;
		for (LONGLONG k=0;k<nelements;k++) meanpix = meanpix + (double)flat_buff[k];
		meanpix = meanpix/nelements;
		fits_close_file(flatptr, &status);
		//END load flat field
		//BEGIN list of files
		QDir dir;
		QStringList filters;
		filters << "*.fit" << "*.fits" << "*.fts";
		dir.setNameFilters(filters); 
	    QStringList flist = dir.entryList();
		for (int i=0;i<flist.count();i++) stream << flist.at(i) <<"\n";
		//END list of files
		//BEGIN flat fielding
		QString filename;
		QByteArray ba_fname;
		fitsfile *fptr;
		char *fname;
		unsigned short *fits_buff = new unsigned short[nelements];
		LONGLONG fpixel = 1;
		for (int i=0;i<flist.count();i++)
		{
			filename = flist.at(i);
			if(filename!=flatfilename)
			{
				ba_fname = filename.toAscii();
				fname = ba_fname.data();
				fits_open_file(&fptr, fname , READWRITE ,&status);
				fits_read_img(fptr, TUSHORT, felem, nelements, NULL, fits_buff, &anynul, &status);
				for (LONGLONG k=0;k<nelements;k++)fits_buff[k] = (unsigned short)((double)fits_buff[k]*meanpix/(double)flat_buff[k]);
				fits_write_img(fptr, TUSHORT, fpixel, nelements, fits_buff, &status);
				fits_close_file(fptr, &status);
			}
		}
		//END flat fielding
		stream << "\n flat finished...\n";
		return 0;  
	}
