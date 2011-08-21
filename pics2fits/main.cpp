#include <QApplication>
#include <QtCore>
#include <QString>
#include <QByteArray>
#include <QFile>
#include <QTextCodec>
#include <QTextCodec>

#include "./../libs/fitsio.h"

void bmpProc(QString fileName, QString resName, int isInv = 0, int flipH = 0, int flipV = 0);
void tiffProc(QString fileName, QString resName, int isInv = 0, int flipH = 0, int flipV = 0);

fitsfile *fptr_out;

int main(int argc, char *argv[])//pic2fits pictfile resPath [isInv] [flipH] [flipV]
{
    QApplication a(argc, argv);

    if(argc<3)
    {
        qDebug() << "pic2fits pictfile [isInv] [flipH] [flipV]\n";
        return 1;
    }

    QTextCodec *codec1 = QTextCodec::codecForName("Windows-1251");
    Q_ASSERT( codec1 );

    QString picFName;
    QString picName = codec1->toUnicode(argv[1]);
    QString resPath = codec1->toUnicode(argv[2]);

    if(QFile().exists(picName+".fit")) QFile().remove(picName+".fit");

    if(picName.indexOf("\\")==-1) picFName = picName.section("/", -1, -1);
    else if(picName.indexOf("/")==-1) picFName = picName.section("\\", -1, -1);
    else picFName = picName;
     //= QDir().fileName();picName
    QString resName = resPath + picFName + ".fit";
    qDebug() << QString("picFName: %1\n").arg(picFName);
    qDebug() << QString("resName: %1\n").arg(resName);

    QString imgExt;

    imgExt = picName.section(".", -1, -1);
    qDebug() << QString("image extension: %1").arg(imgExt);

    int isInv, flipH, flipV;
    isInv = flipH = flipV = 0;



    if(argc>3) isInv = atoi(argv[2]);
    if(argc>4) flipH = atoi(argv[4]);
    if(argc>5) flipV = atoi(argv[5]);

    if(imgExt.toLower().indexOf("bmp")==0) bmpProc(picName, resName, isInv, flipH, !flipV);
    else if(imgExt.toLower().indexOf("tif")==0) tiffProc(picName, resName, isInv, flipH, flipV);
    else qDebug() << "\nunknown extension\n";

    return 0;//a.exec();
}


void tiffProc(QString fileName, QString resName, int isInv, int flipH, int flipV)
{
    //TIFF open
        int i;
        char *str = new char[255];
        strcpy(str, "");
        QByteArray qbarr;

        //QTextStream out_stream;
        //QFile fout("tiff2fits.log");
        //fout.open(QIODevice::Append);
        //out_stream.setDevice(&fout);

    //	int flipH, flipV;

        qDebug() << "\nMAKING FITS..." << endl;

    //	char *fname = new char[255];
    //	QString ltext;
        QByteArray fname;

    //	ltext = tiffFNLabel->text();
        fname.insert(0, fileName);


        FILE *dt = fopen(fname.data(), "rb");
        if(dt==NULL)
        {
    //		errorMessageDialog->showMessage(tr("Can't open FITS file"));
                //QMessageBox msgBox(QMessageBox::Warning, tr("Can't open .tiff file"), "Mentioned .tiff file are not exist", 0, this);
                //msgBox.addButton(QMessageBox::Ok);
                //msgBox.exec();
                qDebug() << "Can't open .bmp file. Mentioned .tiff file are not exist\n";
                return;
        }

        int Width; // выходной параметр – ширина изображения
    int Height; // выходной параметр – высота изображения
    char ch1,ch2;
    fread(&ch1, 1, 1, dt);//dt.Read(&ch1,1);
    fread(&ch2, 1, 1, dt);//dt.Read(&ch2,1);
    if (ch1!='I'|| ch2!='I')
    {
                //QMessageBox msgBox(QMessageBox::Warning, tr("Wrong .tiff file"), "Mentioned .tiff file is unsuitable", 0, this);
                //msgBox.addButton(QMessageBox::Ok);
                //msgBox.exec();
        qDebug() << "Wrong .tiff file. Mentioned .tiff file is unsuitable\n";
        return;//это не tif
    };
    unsigned short vg;
    fread(&vg, 2, 1, dt);//dt.Read(&vg,2);
    if (vg!=42)
    {
                //QMessageBox msgBox(QMessageBox::Warning, tr("Wrong .tiff file"), "Mentioned .tiff file has wrong type", 0, this);
                //msgBox.addButton(QMessageBox::Ok);
                //msgBox.exec();
                qDebug() << "Wrong .tiff file. Mentioned .tiff file has wrong type\n";
        return;// не известная разновидность tif-а
    };

        int der;
    fread(&der, 4, 1, dt);//dt.Read(&der,4);
    fseek(dt, der, SEEK_SET);//dt.Seek(der,CFile::begin);// сдвигаемся в файле к началу шапки
    unsigned short vg2;
    fread(&vg2, 2, 1, dt);//dt.Read(&vg2,2);
        qDebug() << "vg2 " << vg2 << endl;
        int fres;
    for(int iop=0;iop<vg2; iop++)
    {
        unsigned short teg;
        fres = fread(&teg, 2, 1, dt);//dt.Read(&teg,2);
        fread(&vg, 2, 1, dt);//dt.Read(&vg,2);

                qDebug() << "teg " << teg;
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
                qDebug() << "\tdat " << dat << endl;
        if (teg==257) Height=dat;
        if (teg==256) Width=dat;
        if (teg==258&&dat!=16)
        {
                        qDebug() << "don't 16bit " << endl;
                        //QMessageBox msgBox(QMessageBox::Warning, tr("Wrong .tiff file"), "Mentioned .tiff file is not 16bit", 0, this);
                        //msgBox.addButton(QMessageBox::Ok);
                        //msgBox.exec();
                        qDebug() << "Wrong .tiff file. Mentioned .tiff file is not 16bit\n";
                        return;// tif не 16-ти битный
        }
        }
        qDebug() << "Width " << Width << "\tHeight" << Height << endl;
        fseek(dt, 0, SEEK_END);
    unsigned int df2 = (unsigned int) ftell(dt);//dt.GetLength();// получаем длину файла
    unsigned int df1=((unsigned int)(Width))*((unsigned int)(Height));
        qDebug() << "df1 " << df1 << endl;
        qDebug() << "df2 " << df2 << endl;
    df2=df2-df1*2;
        qDebug() << "df2 " << df2 << endl;
    fseek(dt, df2, SEEK_SET);//dt.Seek(df2,CFile::begin);//сдвигаем к началу изображения
        unsigned short *img, *uimg;
    img = new unsigned short [Width];//выделяем память под изображение
        uimg = new unsigned short [Width];//выделяем память под обращаемое изображение

        unsigned short minI, maxI, tim;		//минимальный и максимальный отсчет для инвертирования

        //int is_inv = ui_negativeChBox->checkState()==Qt::Checked;

        qDebug() << "isInv " << isInv << endl;

        if(isInv);
        {
                minI = 4294967294;
                maxI = 0;
                qDebug() << QString("maxI %1\tminI %2").arg(maxI).arg(minI) << endl;
                while(!feof(dt))
                {
                        if(!fread(img, sizeof(unsigned short), Width, dt)) break;
                        for(i=0; i<Width; i++)
                        {
                                if(img[i]<minI) minI = img[i];
                                if(img[i]>maxI) maxI = img[i];
                        }
                }
                qDebug() << QString("maxI %1\tminI %2").arg(maxI).arg(minI) << endl;
                fseek(dt, df2, SEEK_SET);
        }

        //qDebug() << "is_inv " << is_inv << endl;





    //
    //Header + WCS
        //fptr = new fitsdata;
        //if(ui_headerChBox->checkState()==Qt::Checked)fptr->loadHeaderFile(ui_headerFileName->text());
        //if(ui_wcsChBox->checkState()==Qt::Checked)fptr->loadWCSFile(ui_wcsFileName->text());
    //
    //Reading tiff & wrighting
        qDebug() << "\nReading tiff & wrighting\n";
        long nelements;
        long *naxes;
        naxes = new long[2];

        naxes[1] = Height;
        naxes[0] = Width;
        nelements = naxes[0]*naxes[1];

        //fptr->naxes[1] = naxes[1];
        //fptr->naxes[0] = naxes[0];

        //fptr->imgArr->ushD = new unsigned short[nelements];
        int status;

        char *fnameF = new char[256];
        strcpy(fnameF, resName.toAscii().data());
        //strcat(fnameF, ".fit");

        fits_create_file(&fptr_out, fnameF, &status);
        qDebug() << QString("create %1\n").arg(status);
        status = 0;
        fits_create_hdu(fptr_out, &status);
        qDebug() << QString("create_hdu %1\n").arg(status);
        status = 0;

        fits_create_img(fptr_out, USHORT_IMG, 2, naxes, &status);
        qDebug() << QString("create_img %1\n").arg(status) << endl;
        status = 0;






        int k;
        long begPos, endPos, j, sk;

        //int flH = ui_hFlipChBox->checkState()==Qt::Checked;
        //int flV = ui_vFlipChBox->checkState()==Qt::Checked;

        qDebug() << QString("flH: %1\tflV: %2").arg(flipH).arg(flipV);

        //ui_progBar->setValue(10);

        for(i=0; i<naxes[1];i++)
        {
        //		if(flV) fseek(dt, Width*(i+1), SEEK_END);
                    fread(img, sizeof(unsigned short), naxes[0], dt);	//читаем построчно
                    if(flipH)
                    {
        //			out_stream << "Inverting" << endl;
                            for(k=0; k<Width; k++) uimg[k] = img[k];
        //			out_stream << "1 done" << endl;
                            for(k=0; k<Width; k++) img[k] = uimg[Width-k];
        //			out_stream << "2 done" << endl;

                    }


                    if(isInv)
                    {


                            for(j=0; j<naxes[0]; j++)
                            {
                                    tim = img[j];
                                    img[j] = maxI - tim + minI;


                            }
                    }
                    if(flipV)
                    {
                        begPos = naxes[0]*(naxes[1] - i) +1 ;
                        //endPos = begPos + naxes[0]+1;

                        //fits_write_img(fptr_out, TUSHORT, naxes[0]*(naxes[1] - i)+1, naxes[0], (void*) img, &status);
                    }
                    else
                    {
                        begPos = naxes[0]*i+1;
                        //endPos = begPos + naxes[0]+1;
                        //fits_write_img(fptr_out, TUSHORT, naxes[0]*i+1, naxes[0], (void*) img, &status);
                    }
                    //endPos = begPos + naxes[0];

                    //qDebug() << QString("begPos: %1\tendPos: %2\n").arg(begPos).arg(endPos);

                    fits_write_img(fptr_out, TUSHORT, begPos, naxes[0], (void*) img, &status);
/*
                    sk = 0;
                    for(j=begPos; j<endPos; j++)
                    {
                        fptr->imgArr->ushD[j] = img[sk];
                        sk++;
                        ui_progBar->setValue(10+(int)(j/nelements)*80);
                        update();
                    }
                    */
                    //status = 0;
        }

        fclose(dt);

        fits_close_file(fptr_out, &status);
}

typedef struct tagBITMAPFILEHEADER
 {
   wchar_t bfType;
   long bfSize;
   wchar_t bfReserved1;
   wchar_t bfReserved2;
   long bfOffBits;
 } BITMAPFILEHEADER, *PBITMAPFILEHEADER;

struct bmpInfoHeader
 {
   wchar_t  biSize;
   long   biWidth;
   long   biHeight;
   ushort   biPlanes;
   ushort   biBitCount;
   wchar_t  biCompression;
   wchar_t  biSizeImage;
   long   biXPelsPerMeter;
   long   biYPelsPerMeter;
   wchar_t  biClrUsed;
   wchar_t  biClrImportant;
 };

void bmpProc(QString fileName, QString resName, int isInv, int flipH, int flipV)
{
    char *str = new char[255];
    strcpy(str, "");
    QByteArray qbarr;
    QByteArray fname;

    qDebug() << QString("isInv: %1\tflipH: %2\tflipV: %3\n").arg(isInv).arg(flipH).arg(flipV);

    fname.insert(0, fileName);


    FILE *dt = fopen(fname.data(), "rb");
    if(dt==NULL)
    {
//		errorMessageDialog->showMessage(tr("Can't open FITS file"));
            //QMessageBox msgBox(QMessageBox::Warning, tr("Can't open .tiff file"), "Mentioned .tiff file are not exist", 0, this);
            //msgBox.addButton(QMessageBox::Ok);
            //msgBox.exec();

        qDebug() << "\nCan't open .tiff file. Mentioned picture file are not exist\n";

        return;
    }

    int Width; // выходной параметр – ширина изображения
    int Height; // выходной параметр – высота изображения
    char *bfType = new char[2];
    long bfSize;
    fread(bfType, 1, 2, dt);
    //BITMAPFILEHEADER head;// = new BITMAPFILEHEADER;
    //fread(&head, sizeof(BITMAPFILEHEADER), 1, dt);
    qDebug() << "bfType: " << bfType << "\n";
    fread(&bfSize, sizeof(long), 1, dt);
    qDebug() << "bfSize: " << bfSize << "\n";
    char *bfReserved1 = new char[2];
    char *bfReserved2 = new char[2];
    fread(bfReserved1, 2, 1, dt);
    qDebug() << "bfReserved1: " << bfReserved1 << "\n";
    fread(bfReserved2, 2, 1, dt);
    qDebug() << "bfReserved2: " << bfReserved2 << "\n";
    long bfOffBits;
    fread(&bfOffBits, sizeof(long), 1, dt);
    qDebug() << "bfOffBits: " << bfOffBits << "\n";
    bmpInfoHeader bmpInfo;
    fread(&bmpInfo, sizeof(bmpInfoHeader), 1, dt);
    qDebug() << "biWidth: " << bmpInfo.biWidth << "\n";
    qDebug() << "biHeight: " << bmpInfo.biHeight << "\n";
    qDebug() << "biPlanes: " << bmpInfo.biPlanes << "\n";
    qDebug() << "biBitCount: " << bmpInfo.biBitCount << "\n";
    qDebug() << "biCompression: " << bmpInfo.biCompression << "\n";

    fseek(dt, bfOffBits, SEEK_SET);

    Width = bmpInfo.biWidth;
    Height = bmpInfo.biHeight;

//    qDebug() << QString("size of ushort: %1\n").arg(sizeof(uchar));

    long i;
    uchar *img, *uimg;        img = new uchar [Width];//выделяем память под изображение
            uimg = new uchar [Width];//выделяем память под обращаемое изображение

            uchar minI, maxI, tim;		//минимальный и максимальный отсчет для инвертирования

            //int is_inv = ui_negativeChBox->checkState()==Qt::Checked;

            qDebug() << "isInv " << isInv << endl;

            if(isInv)
            {
                    minI = 255;
                    maxI = 0;
                    qDebug() << QString("maxI %1\tminI %2").arg(maxI).arg(minI) << endl;
                    while(!feof(dt))
                    {
                            if(!fread(img, sizeof(uchar), Width, dt)) break;
                            for(i=0; i<Width; i++)
                            {
                                    if(img[i]<minI) minI = img[i];
                                    if(img[i]>maxI) maxI = img[i];
                            }
                    }
                    qDebug() << QString("maxI %1\tminI %2").arg(maxI).arg(minI) << endl;
                    fseek(dt, bfOffBits, SEEK_SET);
            }

            //qDebug() << "is_inv " << is_inv << endl;





        //
        //Header + WCS
            //fptr = new fitsdata;

            //if(ui_headerChBox->checkState()==Qt::Checked)fptr->loadHeaderFile(ui_headerFileName->text());
            //if(ui_wcsChBox->checkState()==Qt::Checked)fptr->loadWCSFile(ui_wcsFileName->text());
        //
        //Reading tiff & wrighting
            qDebug() << "\nReading tiff & wrighting\n";
            long nelements;
            long *naxes;
            naxes = new long[2];

            //fitsfile *fptr_out;
            int status = 0;

            char *fnameF = new char[256];
            strcpy(fnameF, resName.toAscii().data());
            //strcat(fnameF, ".fit");

            naxes[1] = Height;
            naxes[0] = Width;
            nelements = naxes[0]*naxes[1];

            qDebug() << QString("nelements: %1\n").arg(nelements);

            //fptr->naxes[1] = naxes[1];
            //fptr->naxes[0] = naxes[0];

            //fptr->imgArr = new img2d(SHORT_IMG, naxes[0], naxes[1]);

            fits_create_file(&fptr_out, fnameF, &status);
            qDebug() << QString("create %1\n").arg(status);
            status = 0;
            fits_create_hdu(fptr_out, &status);
            qDebug() << QString("create_hdu %1\n").arg(status);
            status = 0;

            fits_create_img(fptr_out, USHORT_IMG, 2, naxes, &status);
            qDebug() << QString("create_img %1\n").arg(status) << endl;
            status = 0;



            //fptr->imgArr->ushD = (unsigned short*) malloc(sizeof(unsigned short)*nelements);
            //fptr->imgArr->shD = new short[nelements];
/*
            qDebug() << "fptr->imgArr->shD: " << fptr->imgArr->shD << "\n";
            if(!fptr->imgArr->shD)
            {
                qDebug() << "\nbad_alloc\n";
                exit(1);
            }
            //for(i=0; i<nelements; i++){ qDebug() << i << "\n"; fptr->imgArr->ushD[i] = 0;}
*/
            int k;
            long begPos, endPos, j, sk;

            //int flH = ui_hFlipChBox->checkState()==Qt::Checked;
            //int flV = ui_vFlipChBox->checkState()==Qt::Checked;

            unsigned short *ushD = new unsigned short[Width];

            qDebug() << QString("flipH: %1\tflipV: %2").arg(flipH).arg(flipV);

            //ui_progBar->setValue(10);

            fseek(dt, bfOffBits, SEEK_SET);

            for(i=0; i<naxes[1];i++)
            {
            //		if(flV) fseek(dt, Width*(i+1), SEEK_END);
                        //qDebug() << "fread: " <<  << "\n";	//читаем построчно
                        fread(img, sizeof(uchar), naxes[0], dt);
                        if(flipH)
                        {
            //			out_stream << "Inverting" << endl;
                                for(k=0; k<Width; k++) uimg[k] = img[k];
            //			out_stream << "1 done" << endl;
                                for(k=0; k<Width; k++) img[k] = uimg[Width-k];
            //			out_stream << "2 done" << endl;

                        }


                        if(isInv)
                        {


                                for(j=0; j<naxes[0]; j++)
                                {
                                        tim = img[j];
                                        img[j] = maxI - tim + minI;


                                }
                        }
                        if(flipV)
                        {
                            begPos = naxes[0]*(naxes[1] - i)+1;
                            //endPos = begPos + naxes[0]+1;

                            //fits_write_img(fptr_out, TUSHORT, naxes[0]*(naxes[1] - i)+1, naxes[0], (void*) img, &status);
                        }
                        else
                        {
                            begPos = naxes[0]*i+1;
                            //endPos = begPos + naxes[0]+1;
                            //fits_write_img(fptr_out, TUSHORT, naxes[0]*i+1, naxes[0], (void*) img, &status);
                        }
                        //endPos = begPos + naxes[0];
                        for(j=0; j<naxes[0]; j++)
                        {
                            //qDebug() << QString("j= %1\timg[j]= %2\n").arg(j).arg(img[j]);

                            ushD[j] = img[j];

                            //ui_progBar->setValue(10+(int)(j/nelements)*80);
                            //update();
                        }

                        //qDebug() << QString("begPos: %1\tendPos: %2\n").arg(begPos).arg(endPos);
                        fits_write_img(fptr_out, TUSHORT, begPos, naxes[0], (void*) ushD, &status);
                        //qDebug() << QString("write_img %1\n").arg(status);
                        status=0;
                        /*
                        sk = 0;
                        for(j=begPos; j<endPos; j++)
                        {
                            //qDebug() << QString("j= %1\timg[j]= %2\n").arg(j).arg(img[j]);

                            fptr->imgArr->ushD[j] = img[sk];
                            sk++;
                            //ui_progBar->setValue(10+(int)(j/nelements)*80);
                            //update();
                        }
                        */
                        //status = 0;
            }


    fclose(dt);
    fits_close_file(fptr_out, &status);
}
