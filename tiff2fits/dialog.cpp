/****************************************************************************
**
** Copyright (C) 2004-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtGui>

#include "dialog.h"

#define MESSAGE \
    Dialog::tr("<p>Message boxes have a caption, a text, " \
               "and any number of buttons, each with standard or custom texts." \
               "<p>Click a button to close the message box. Pressing the Esc button " \
               "will activate the detected escape button (if any).")

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
{
    errorMessageDialog = new QErrorMessage(this);

    int frameStyle = QFrame::Sunken | QFrame::Panel;
	
	headerLabel = new QLabel;
    headerLabel->setFrameStyle(frameStyle);
	headerLabel->setText("Header file");
	
	headerFNLabel = new QLabel;
    headerFNLabel->setFrameStyle(frameStyle);
	
	QPushButton *headerButton = new QPushButton(tr("..."));
	
	tiffLabel = new QLabel;
    tiffLabel->setFrameStyle(frameStyle);
	tiffLabel->setText("Tiff file");

	
	tiffFNLabel = new QLabel;
    tiffFNLabel->setFrameStyle(frameStyle);
	
	QPushButton *tiffButton = new QPushButton(tr("..."));

	telLabel = new QLabel;
    telLabel->setFrameStyle(frameStyle);
	telLabel->setText("TELESCOP");
	telLE = new QLineEdit;
	
	plateNLabel = new QLabel;
    plateNLabel->setFrameStyle(frameStyle);
	plateNLabel->setText("PLATE-N");
	plateNLE = new QLineEdit;
//	telLE->setFrameStyle(frameStyle);
	
	objLabel = new QLabel;
    objLabel->setFrameStyle(frameStyle);
	objLabel->setText("OBJECT");
	objLE = new QLineEdit;
//	objLE->setFrameStyle(frameStyle);

	dateObsLabel = new QLabel;
    dateObsLabel->setFrameStyle(frameStyle);
	dateObsLabel->setText("DATE-OBS");
	dateObsLE = new QLineEdit;
	
	dateEndLabel = new QLabel;
    dateEndLabel->setFrameStyle(frameStyle);
	dateEndLabel->setText("DATE-END");
	dateEndLE = new QLineEdit;
	
	raLabel = new QLabel;
    raLabel->setFrameStyle(frameStyle);
	raLabel->setText("RA");
	raLE = new QLineEdit;
	
	deLabel = new QLabel;
    deLabel->setFrameStyle(frameStyle);
	deLabel->setText("DEC");
	deLE = new QLineEdit;
	
	expLabel = new QLabel;
    expLabel->setFrameStyle(frameStyle);
	expLabel->setText("EXPOSURE");
	expLE = new QLineEdit;
//	expLE->setFrameStyle(frameStyle);
	
	obsLabel = new QLabel;
    obsLabel->setFrameStyle(frameStyle);
	obsLabel->setText("OBSERVER");
	obsLE = new QLineEdit;
	
	flipH = new QCheckBox("flipH");
	flipV = new QCheckBox("flipV");
	IsInv = new QCheckBox("Negative");

	QPushButton *MakeFitsButton = new QPushButton(tr("Make FITS"));
	QPushButton *closeButton = new QPushButton(tr("Close"));

	connect(headerButton, SIGNAL(clicked()), this, SLOT(broseHeader()));
	connect(tiffButton, SIGNAL(clicked()), this, SLOT(broseTiff()));
	connect(MakeFitsButton, SIGNAL(clicked()), this, SLOT(MakeFits()));
	connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    QGridLayout *layout = new QGridLayout;
    layout->setColumnStretch(1, 1);
    layout->setColumnMinimumWidth(1, 250);
	
	layout->addWidget(headerLabel, 0, 0);
	layout->addWidget(headerFNLabel, 0, 1);
	layout->addWidget(headerButton, 0, 2);
	
	layout->addWidget(tiffLabel, 1, 0);
	layout->addWidget(tiffFNLabel, 1, 1);
	layout->addWidget(tiffButton, 1, 2);
	
	layout->addWidget(telLabel, 2, 0);
	layout->addWidget(telLE, 2, 1);
	
	layout->addWidget(plateNLabel, 3, 0);
	layout->addWidget(plateNLE, 3, 1);
	
	layout->addWidget(objLabel, 4, 0);
	layout->addWidget(objLE, 4, 1);
	
	layout->addWidget(dateObsLabel, 5, 0);
	layout->addWidget(dateObsLE, 5, 1);
	
	layout->addWidget(dateEndLabel, 6, 0);
	layout->addWidget(dateEndLE, 6, 1);
	
	layout->addWidget(raLabel, 7, 0);
	layout->addWidget(raLE, 7, 1);
	
	layout->addWidget(deLabel, 8, 0);
	layout->addWidget(deLE, 8, 1);
	
	layout->addWidget(expLabel, 9, 0);
	layout->addWidget(expLE, 9, 1);
	
	layout->addWidget(obsLabel, 10, 0);
	layout->addWidget(obsLE, 10, 1);
	
	layout->addWidget(flipH, 11, 0);
	layout->addWidget(flipV, 11, 1);
	layout->addWidget(IsInv, 11, 2);
	
	layout->addWidget(MakeFitsButton, 12, 0, 1, 0, Qt::AlignCenter);
	layout->addWidget(closeButton, 12, 2);
	
    setLayout(layout);

    setWindowTitle(tr("FITS Header Edit Dialog"));
}

void Dialog::broseHeader()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                tr("QFileDialog::getOpenFileName()"),
                                headerFNLabel->text(),
                                tr("All Files (*);;Text Files (*.txt)"));
    if (!fileName.isEmpty())
	{
		headerFNLabel->setText(fileName);
		
		QTextStream out_stream;
		QFile fout("tiff2fits.log");
		fout.open(QIODevice::Append);
		out_stream.setDevice(&fout);
		
		char *str = new char[1024];
		char *str_name = new char[1024];
		char *str_value = new char[1024];
	
		QByteArray fheadfn, qbarr;
		fheadfn.insert(0, fileName);
		FILE *FI = fopen(fheadfn.data(), "r");
		if(FI==NULL)
		{
			QMessageBox msgBox(QMessageBox::Warning, tr("Can't open Header file"), "Mentioned FITS Header file has't exist", 0, this);
			msgBox.addButton(QMessageBox::Ok);
			msgBox.exec();
			return ;
		}
		
		out_stream << "\nREADING HEADER..." << endl;
	
		int posb, pose, tobs = 0;
		QString *qstr = new QString;
		
		
		
		QTextCodec *codec1 = QTextCodec::codecForName("windows-1251");
		QTextCodec *codec2 = QTextCodec::codecForName("IBM 866");
/*	QTextDecoder *decoder = codec1->makeDecoder();
		QTextEncoder *encoder = codec2->makeEncoder();
*/
	    QString string;
		QChar *cstr = new QChar;
			
		while(fgets(str, 1024, FI))
		{
			out_stream << "string is: _" << str << "_" << endl;
			qbarr.clear();
			qbarr.insert(0, str);

                out_stream << "qbarr\t" << qbarr.data() << endl;
			string = codec1->toUnicode(qbarr);
                out_stream << "string\t" << string << endl;
			qbarr = codec2->fromUnicode(string);
			str = qbarr.data();
		
                printf("_%s_\n", str);
                out_stream << "qbarr\t" << qbarr << endl;
                out_stream << "str\t" << str << endl;
		
			if(!slovo_stopsim(str, str_name, " ", 0, &pose, 8)&&!slovo_stopsim(str, str_value, "\'", 11, &pose, 68))
			{
				string = codec2->toUnicode(str_value);
				if(streqv(str_name, "TELESCOP"))
				{
					telLE->setText(string);
					out_stream << "_" << str_name << "_\t_" << str_value << "_" << endl;
				}
				else if(streqv(str_name, "PLATE-N"))
				{
					plateNLE->setText(string);
					out_stream << "_" << str_name << "_\t_" << str_value << "_" << endl;
				}
				else if(streqv(str_name, "OBJECT"))
				{
					objLE->setText(string);
					out_stream << "_" << str_name << "_\t_" << str_value << "_" << endl;
				}
				else if(streqv(str_name, "EXPOSURE"))
				{
					expLE->setText(string);
					out_stream << "_" << str_name << "_\t_" << str_value << "_" << endl;
				}
				else if(streqv(str_name, "OBSERVER"))
				{
					obsLE->setText(string);
					out_stream << "_" << str_name << "_\t_" << str_value << "_" << endl;
				}
				else if(streqv(str_name, "RA"))
				{
					raLE->setText(string);
					out_stream << "_" << str_name << "_\t_" << str_value << "_" << endl;
				}
				else if(streqv(str_name, "DEC"))
				{
					deLE->setText(string);
					out_stream << "_" << str_name << "_\t_" << str_value << "_" << endl;
				}
				else if(streqv(str_name, "DATE-OBS"))
				{
                                    out_stream << "\nDATE-OBS\n";
					dateObsLE->setText(string);
					out_stream << "_" << str_name << "_\t_" << str_value << "_" << endl;
				}
				else
				{
				/*
					fits_write_key_str(fptr, str_name, str_value, "", &status);
					out_stream << str_name << "\t" << str_value << "\t" << QString("stat=%1").arg(status) << endl;
					status = 0;*/
					out_stream << str_name << " no var" << endl;
				}
			}
		}
		fout.close();
		
//		delete [] str;
//		delete [] str_name;
//		delete [] str_value;
		
	}
//	fout.close();

}

void Dialog::broseTiff()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                tr("QFileDialog::getOpenFileName()"),
                                tiffFNLabel->text(),
                                tr("All Files (*);;Text Files (*.txt)"));
    if (!fileName.isEmpty())
        tiffFNLabel->setText(fileName);
}

void Dialog::MakeFits()
{
	int i, j;
	char *str = new char[255];
	strcpy(str, "");
	QByteArray qbarr;
		
	QTextStream out_stream;
	QFile fout("tiff2fits.log");
	fout.open(QIODevice::Append);
	out_stream.setDevice(&fout);
	
//	int flipH, flipV;
		
	out_stream << "\nMAKING FITS..." << endl;  
	
//	char *fname = new char[255];
//	QString ltext;
	QByteArray fname;
	
//	ltext = tiffFNLabel->text();
	fname.insert(0, tiffFNLabel->text());
	

	FILE *dt = fopen(fname.data(), "rb");
	if(dt==NULL)
	{
//		errorMessageDialog->showMessage(tr("Can't open FITS file"));
		QMessageBox msgBox(QMessageBox::Warning, tr("Can't open .tiff file"), "Mentioned .tiff file are not exist", 0, this);
		msgBox.addButton(QMessageBox::Ok);
		msgBox.exec();

		return;
	}

	int Width; // выходной параметр – ширина изображения
    int Height; // выходной параметр – высота изображения
    char ch1,ch2;
    fread(&ch1, 1, 1, dt);//dt.Read(&ch1,1);
    fread(&ch2, 1, 1, dt);//dt.Read(&ch2,1);
    if (ch1!='I'|| ch2!='I') 
    {
		QMessageBox msgBox(QMessageBox::Warning, tr("Wrong .tiff file"), "Mentioned .tiff file is unsuitable", 0, this);
		msgBox.addButton(QMessageBox::Ok);
		msgBox.exec();
        return;//это не tif
    };
    unsigned short vg;
    fread(&vg, 2, 1, dt);//dt.Read(&vg,2);
    if (vg!=42) 
    {
		QMessageBox msgBox(QMessageBox::Warning, tr("Wrong .tiff file"), "Mentioned .tiff file has wrong type", 0, this);
		msgBox.addButton(QMessageBox::Ok);
		msgBox.exec();
        return;// не известная разновидность tif-а
    };
    
	int der;
    fread(&der, 4, 1, dt);//dt.Read(&der,4);
    fseek(dt, der, SEEK_SET);//dt.Seek(der,CFile::begin);// сдвигаемся в файле к началу шапки
    unsigned short vg2;
    fread(&vg2, 2, 1, dt);//dt.Read(&vg2,2);
	out_stream << "vg2 " << vg2 << endl;
	int fres;
    for(int iop=0;iop<vg2; iop++)
    { 
        unsigned short teg;
        fres = fread(&teg, 2, 1, dt);//dt.Read(&teg,2);
        fread(&vg, 2, 1, dt);//dt.Read(&vg,2);
	 
		out_stream << "teg " << teg;
		int kol,dat;
        if(vg==4) 
        {
            fread(&kol, 4, 1, dt);//dt.Read(&kol,4);
            fread(&dat, 4, 1, dt);//dt.Read(&dat,4);
        }
        else
        {
            fread(&kol, 4, 1, dt);//dt.Read(&kol,4);
            fread(&dat, 2, 1, dt);//dt.Read(&dat,2);
            int yyu;
            fread(&yyu, 2, 1, dt);//dt.Read(&yyu,2);
        };
		out_stream << "\tdat " << dat << endl;
        if (teg==257) Height=dat;
        if (teg==256) Width=dat;
        if (teg==258&&dat!=16)
        {
			out_stream << "don't 16bit " << endl;
			QMessageBox msgBox(QMessageBox::Warning, tr("Wrong .tiff file"), "Mentioned .tiff file is not 16bit", 0, this);
			msgBox.addButton(QMessageBox::Ok);
			msgBox.exec();
			return;// tif не 16-ти битный
        }
	}
	out_stream << "Width " << Width << "\tHeight" << Height << endl;
	fseek(dt, 0, SEEK_END);
    unsigned int df2 = (unsigned int) ftell(dt);//dt.GetLength();// получаем длину файла
    unsigned int df1=((unsigned int)(Width))*((unsigned int)(Height));
	out_stream << "df1 " << df1 << endl;
	out_stream << "df2 " << df2 << endl;
    df2=df2-df1*2;
	out_stream << "df2 " << df2 << endl;
    fseek(dt, df2, SEEK_SET);//dt.Seek(df2,CFile::begin);//сдвигаем к началу изображения
	unsigned short *img, *uimg;
    img = new unsigned short [Width];//выделяем память под изображение
	uimg = new unsigned short [Width];//выделяем память под обращаемое изображение
	
	unsigned short minI, maxI, tim;		//минимальный и максимальный отсчет для инвертирования
	
	int is_inv = IsInv->checkState()==Qt::Checked;
	
	out_stream << "is_inv " << is_inv << endl;
	
	if(is_inv);
	{
		minI = 4294967294;
		maxI = 0;
		out_stream << QString("maxI %1\tminI %2").arg(maxI).arg(minI) << endl;
		while(!feof(dt))
		{
			if(!fread(img, sizeof(unsigned short), Width, dt)) break;
			for(i=0; i<Width; i++)
			{
				if(img[i]<minI) minI = img[i];
				if(img[i]>maxI) maxI = img[i];
			}
		}
		out_stream << QString("maxI %1\tminI %2").arg(maxI).arg(minI) << endl;
		fseek(dt, df2, SEEK_SET);
	}
	
	out_stream << "is_inv " << is_inv << endl;

	fitsfile *fptr_out;
	int status = 0;
	
	
	char *suff = new char[256];
	int len = strlen(fname.data());
	char *pos = strrchr(fname.data(), 46);
	int len1 = strlen(pos);
	strcpy(suff, "");
	strncpy(suff, fname.data(), len-len1);
	strcpy(&suff[len-len1], ".fits");
				
	fits_open_file(&fptr_out, suff, READWRITE, &status);

	out_stream << QString("open_old %1").arg(status) << endl;
	status = 0;
	fits_delete_file(fptr_out, &status);
	out_stream << QString("delete %1").arg(status) << endl;
	status = 0;
	fits_create_file(&fptr_out, suff, &status);
	out_stream << QString("create %1").arg(status) << endl;
	status = 0;
		
	long nelements;
	long *naxes;
	naxes = new long[2];
		
	fits_create_hdu(fptr_out, &status);
	out_stream << QString("create_hdu %1").arg(status) << endl;
	status = 0;
		
	naxes[1] = Height;
	naxes[0] = Width;
	nelements = naxes[0]*naxes[1];
		
	fits_create_img(fptr_out, USHORT_IMG, 2, naxes, &status);
	fname.clear();
	fname.insert(0, headerFNLabel->text());
		if(streqv(fname.data(), ""))
		{
			fits_write_key_str(fptr_out, "OBSERVER", "                  ", "", &status);
			status = 0;
			
			fits_write_key_str(fptr_out, "TELESCOP", "Normal Astrograph ", "", &status);
			status = 0;
			
			fits_write_key_str(fptr_out, "OBSERVAT", "Pulkovo           ", "", &status);
			status = 0;
			
			fits_write_key_str(fptr_out, "DATE-OBS", "0000-00-00        ", "", &status);
			status = 0;
			
			fits_write_key_str(fptr_out, "TIME-OBS", "00:00:00.00       ", "", &status);
			status = 0;
			
			fits_write_key_str(fptr_out, "RA", "00:00:00.000      ", "", &status);
			status = 0;
			
			fits_write_key_str(fptr_out, "DEC", "+00:00:00.00      ", "", &status);
			status = 0;
			
			fits_write_key_str(fptr_out, "EXPTIME", "15", "", &status);
			status = 0;
			
		}
		else
		{
			
			make_fits_header(fptr_out, fname.data());
			
			qbarr.clear();
			qbarr.insert(0, telLE->text());
			fits_write_key_str(fptr_out, "TELESCOP", qbarr.data(), "", &status);
			status = 0;
			qbarr.clear();
			qbarr.insert(0, objLE->text());
			fits_write_key_str(fptr_out, "OBJECT", qbarr.data(), "", &status);
			status = 0;
			qbarr.clear();
			qbarr.insert(0, expLE->text());
			fits_write_key_str(fptr_out, "EXPTIME", qbarr.data(), "", &status);
			status = 0;
			qbarr.clear();
			qbarr.insert(0, obsLE->text());
			fits_write_key_str(fptr_out, "OBSERVER", qbarr.data(), "", &status);
			status = 0;
			
			fits_write_key_str(fptr_out, "OBSERVAT", "Pulkovo           ", "", &status);
			status = 0;
			
		}
	
//	if(flipH) 
	int k;
	
	int flH = flipH->checkState()==Qt::Checked;
	int flV = flipV->checkState()==Qt::Checked;
	
	
	
//	if(flV) fseek(dt, Width*(i+1), SEEK_END);
//	is_inv = flipH->checkState()==Qt::Checked;
	out_stream << "is_inv " << is_inv << endl;

	for(i=0; i<naxes[1];i++)
    {
//		if(flV) fseek(dt, Width*(i+1), SEEK_END);
		fread(img, sizeof(unsigned short), naxes[0], dt);	//читаем построчно
		if(flH)
		{
//			out_stream << "Inverting" << endl;
			for(k=0; k<Width; k++) uimg[k] = img[k];
//			out_stream << "1 done" << endl;
			for(k=0; k<Width; k++) img[k] = uimg[Width-k];
//			out_stream << "2 done" << endl;
			
		}
		
		
		if(is_inv)
		{
			
		
			for(j=0; j<naxes[0]; j++)
			{
				tim = img[j];
				img[j] = maxI - tim + minI;
				
				
			}
		}
		if(flV) fits_write_img(fptr_out, TUSHORT, naxes[0]*(naxes[1] - i)+1, naxes[0], (void*) img, &status);
		else fits_write_img(fptr_out, TUSHORT, naxes[0]*i+1, naxes[0], (void*) img, &status);
		status = 0;
    }
	
    fclose(dt);
		
	fits_close_file(fptr_out, &status);

	fout.close();
	
    QMessageBox::StandardButton reply;
    reply = QMessageBox::information(this, tr("Make FIST..."), "Done");
}


/*
void Dialog::setInteger()
{
    bool ok;
    int i = QInputDialog::getInteger(this, tr("QInputDialog::getInteger()"),
                                     tr("Percentage:"), 25, 0, 100, 1, &ok);
    if (ok)
        integerLabel->setText(tr("%1%").arg(i));
}

void Dialog::setDouble()
{
    bool ok;
    double d = QInputDialog::getDouble(this, tr("QInputDialog::getDouble()"),
                                       tr("Amount:"), 37.56, -10000, 10000, 2, &ok);
    if (ok)
        doubleLabel->setText(QString("$%1").arg(d));
}

void Dialog::setItem()
{
    QStringList items;
    items << tr("Spring") << tr("Summer") << tr("Fall") << tr("Winter");

    bool ok;
    QString item = QInputDialog::getItem(this, tr("QInputDialog::getItem()"),
                                         tr("Season:"), items, 0, false, &ok);
    if (ok && !item.isEmpty())
        itemLabel->setText(item);
}

void Dialog::setText()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                         tr("User name:"), QLineEdit::Normal,
                                         QDir::home().dirName(), &ok);
    if (ok && !text.isEmpty())
        textLabel->setText(text);
}

void Dialog::setColor()
{
    QColor color = QColorDialog::getColor(Qt::green, this);
    if (color.isValid()) {
        colorLabel->setText(color.name());
        colorLabel->setPalette(QPalette(color));
        colorLabel->setAutoFillBackground(true);
    }
}

void Dialog::setFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont(fontLabel->text()), this);
    if (ok) {
        fontLabel->setText(font.key());
        fontLabel->setFont(font);
    }
}

void Dialog::setExistingDirectory()
{
    QString directory = QFileDialog::getExistingDirectory(this,
                                tr("QFileDialog::getExistingDirectory()"),
                                directoryLabel->text(),
                                QFileDialog::DontResolveSymlinks
                                | QFileDialog::ShowDirsOnly);
    if (!directory.isEmpty())
        directoryLabel->setText(directory);
}

void Dialog::setOpenFileName()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                tr("QFileDialog::getOpenFileName()"),
                                openFileNameLabel->text(),
                                tr("All Files (*);;Text Files (*.txt)"));
    if (!fileName.isEmpty())
        openFileNameLabel->setText(fileName);
}

void Dialog::setOpenFileNames()
{
    QStringList files = QFileDialog::getOpenFileNames(
                                this, tr("QFileDialog::getOpenFileNames()"),
                                openFilesPath,
                                tr("All Files (*);;Text Files (*.txt)"));
    if (files.count()) {
        openFilesPath = files[0];
        openFileNamesLabel->setText(QString("[%1]").arg(files.join(", ")));
    }
}

void Dialog::setSaveFileName()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                tr("QFileDialog::getSaveFileName()"),
                                saveFileNameLabel->text(),
                                tr("All Files (*);;Text Files (*.txt)"));
    if (!fileName.isEmpty())
        saveFileNameLabel->setText(fileName);
}

void Dialog::criticalMessage()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::critical(this, tr("QMessageBox::critical()"),
                                    MESSAGE,
                                    QMessageBox::Abort | QMessageBox::Retry | QMessageBox::Ignore);
    if (reply == QMessageBox::Abort)
        criticalLabel->setText(tr("Abort"));
    else if (reply == QMessageBox::Retry)
        criticalLabel->setText(tr("Retry"));
    else
        criticalLabel->setText(tr("Ignore"));
}

void Dialog::informationMessage()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::information(this, tr("QMessageBox::information()"), MESSAGE);
    if (reply == QMessageBox::Ok)
        informationLabel->setText(tr("OK"));
    else
        informationLabel->setText(tr("Escape"));
}

void Dialog::questionMessage()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("QMessageBox::question()"),
                                    MESSAGE,
                                    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    if (reply == QMessageBox::Yes)
        questionLabel->setText(tr("Yes"));
    else if (reply == QMessageBox::No)
        questionLabel->setText(tr("No"));
    else
        questionLabel->setText(tr("Cancel"));
}

void Dialog::warningMessage()
{
    QMessageBox msgBox(QMessageBox::Warning, tr("QMessageBox::warning()"),
                       MESSAGE, 0, this);
    msgBox.addButton(tr("Save &Again"), QMessageBox::AcceptRole);
    msgBox.addButton(tr("&Continue"), QMessageBox::RejectRole);
    if (msgBox.exec() == QMessageBox::AcceptRole)
        warningLabel->setText(tr("Save Again"));
    else
        warningLabel->setText(tr("Continue"));

}

void Dialog::errorMessage()
{
    errorMessageDialog->showMessage(
            tr("This dialog shows and remembers error messages. "
               "If the checkbox is checked (as it is by default), "
               "the shown message will be shown again, "
               "but if the user unchecks the box the message "
               "will not appear again if QErrorMessage::showMessage() "
               "is called with the same message."));
    errorLabel->setText(tr("If the box is unchecked, the message "
                           "won't appear again."));
}
*/
