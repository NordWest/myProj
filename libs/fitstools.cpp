#include "fitstools.h"

int make_good_fits_header(char *fheader_name)
{
	return 0;
}

void make_date_obs(char *str_date_obs, char *str_time_obs, char *str_date_end, char *str_time_end)
{
	int yr_obs, mth_obs, h_obs, m_obs, iday;
	double day, jd_obs, jd_end, s_obs;
	int h_end, m_end;
	double s_end;
	
	char *str = new char[255];
	char *slo = new char[255];
	char *ssim = new char[255];
	int posb, pose, nmax;
		
	strcpy(ssim, " ");
	
	posb = 0;
	nmax = 5;
	slovo_stopsim(str_date_obs, slo, ssim, posb, &pose, nmax);
	yr_obs = atoi(slo);
	
	posb = pose+1;
	nmax = 2;
	slovo_stopsim(str_date_obs, slo, ssim, posb, &pose, nmax);
	mth_obs = atoi(slo);
	
	posb = pose+1;
	nmax = 2;
	slovo_stopsim(str_date_obs, slo, ssim, posb, &pose, nmax);
	iday = atoi(slo);
	
	posb = 0;
	nmax = 2;
	slovo_stopsim(str_time_obs, slo, ssim, posb, &pose, nmax);
	h_obs = atoi(slo);
	
	posb = pose+1;
	nmax = 2;
	slovo_stopsim(str_time_obs, slo, ssim, posb, &pose, nmax);
	m_obs = atoi(slo);
	
	posb = pose+1;
	nmax = 5;
	slovo_stopsim(str_time_obs, slo, ssim, posb, &pose, nmax);
	s_obs = atof(slo);
	
	HMS2day(&day, h_obs, m_obs, s_obs);
	dat2JD(&jd_obs, yr_obs, mth_obs, iday+day);
	
	posb = 0;
	nmax = 2;
	slovo_stopsim(str_time_end, slo, ssim, posb, &pose, nmax);
	h_end = atoi(slo);
	
	posb = pose+1;
	nmax = 2;
	slovo_stopsim(str_time_end, slo, ssim, posb, &pose, nmax);
	m_end = atoi(slo);
	
	posb = pose+1;
	nmax = 5;
	slovo_stopsim(str_time_end, slo, ssim, posb, &pose, nmax);
	s_end = atof(slo);
	
	HMS2day(&day, h_end, m_end, s_end);
	dat2JD(&jd_end, yr_obs, mth_obs, iday+day);

	if(jd_end<jd_obs) jd_end++;

	dat2YMD(jd_obs, &yr_obs, &mth_obs, &day);
	
	sprintf(str_date_obs, "%4d-%02d-%02dT%02d:%02d:%02d", yr_obs, mth_obs, (int)day, h_obs, m_obs, (int) s_obs);
	
	dat2YMD(jd_end, &yr_obs, &mth_obs, &day);
	
	sprintf(str_date_end, "%4d-%02d-%02dT%02d:%02d:%02d", yr_obs, mth_obs, (int) day, h_end, m_end, (int) s_end);
}

int make_fits_header(fitsfile *fptr, char *fheadfn)
{
	int i, status;
	char *str = new char[255];
	char *str_name = new char[255];
	char *str_value = new char[255];
	char *str_date_obs = new char[255];
	char *str_time_obs = new char[255];
	char *str_date_end = new char[255];
	char *str_time_end = new char[255];
	
//	QTextStream out_stream;
	
	FILE *FI = fopen(fheadfn, "r");
	if(FI==NULL) return 1;
	
	int posb, pose, tobs = 0;
	QString *qstr = new QString;
	QByteArray qbarr;
	
	QTextCodec *codec1 = QTextCodec::codecForName("windows-1251");
	QTextCodec *codec2 = QTextCodec::codecForName("IBM 866");
	
	QTextStream out_stream;
	QFile fout("tiff2fits.log");
	fout.open(QIODevice::Append);
	out_stream.setDevice(&fout);
	
	
/*	QTextDecoder *decoder = codec1->makeDecoder();
	QTextEncoder *encoder = codec2->makeEncoder();
*/
    QString string;
	QChar *cstr = new QChar;
		
	while(fgets(str, 82, FI))
	{
		qbarr.clear();
		qbarr.insert(0, str);

//		out_stream << "qbarr\t" << qbarr.data() << endl;
		string = codec1->toUnicode(qbarr);
//		out_stream << "string\t" << string << endl;
		qbarr = codec2->fromUnicode(string);
		str = qbarr.data();
	
//		printf("_%s_\n", str);
//		out_stream << "qbarr\t" << qbarr << endl;
//		out_stream << "str\t" << str << endl;
	
		if(!slovo_stopsim(str, str_name, " ", 0, &pose, 8)&&!slovo_stopsim(str, str_value, "\'", 11, &pose, 68))
		{
//			printf("_%s_ _%s_\n", str_name, str_value);
//			out_stream << "_" << str_name << "_\t_" << str_value << "_" << endl;
			if(streqv(str_name, "DATE-OBS"))
			{
				strcpy(str_date_obs, str_value);
				strcpy(str_date_end, str_value);
			}
			else if(streqv(str_name, "TIME-OBS")&&tobs==0)
			{
				strcpy(str_time_obs, str_value);
				tobs++;
			}
			else if(streqv(str_name, "TIME-OBS")&&tobs==1)
			{
				strcpy(str_time_end, str_value);
				make_date_obs(str_date_obs, str_time_obs, str_date_end, str_time_end);
				status = 0;
//				fits_write_key_str(fptr, "DATE-OBS", str_date_obs, "", &status);
				fits_update_key(fptr, TSTRING, "DATE-OBS", str_date_obs, "date of observations", &status);
				out_stream << "write DATE-OBS = _" << str_name << "_\t_" << str_value << "_" << QString("write_key %1").arg(status)<< endl;
				status = 0;
				fits_write_key_str(fptr, "DATE-END", str_date_end, "", &status);
				status = 0;
			}
			else if(streqv(str_name, "RA")&&tobs==1)
			{
				make_good_RADEC(str_value, 0);
				fits_write_key_str(fptr, str_name, str_value, "", &status);
				status = 0;
			}
			else if(streqv(str_name, "DEC")&&tobs==1)
			{
				make_good_RADEC(str_value, 1);
				fits_write_key_str(fptr, str_name, str_value, "", &status);
				status = 0;
			}
			else if(streqv(str_name, "TELESCOP")&&tobs==1)
			{
				make_good_RADEC(str_value, 1);
				fits_write_key_str(fptr, str_name, str_value, "", &status);
				status = 0;
			}
			else
			{
/*				fits_write_key_str(fptr, str_name, str_value, "", &status);
				out_stream << str_name << "\t" << str_value << "\t" << QString("stat=%1").arg(status) << endl;
				status = 0;*/
			}
		}
	}
	
	fout.close();

	return 0;
}

void make_good_RADEC(char *str_ra, int type)
{
	int hr, min;
	double sec;
	
	char *str = new char[255];
	char *slo = new char[255];
	char *ssim = new char[255];
	int posb, pose, nmax;
	
	strcpy(ssim, " ");
	
	posb = 0;
	nmax = 2;
	slovo_stopsim(str_ra, slo, ssim, posb, &pose, nmax);
	hr = atoi(slo);
	
	posb = pose+1;
	nmax = 2;
	slovo_stopsim(str_ra, slo, ssim, posb, &pose, nmax);
	min = atoi(slo);
	
	posb = pose+1;
	nmax = 5;
	slovo_stopsim(str_ra, slo, ssim, posb, &pose, nmax);
	sec = atof(slo);
	
	if(type) sprintf(str_ra, "%+02d:%02d:%06.3f", hr, min, sec);
	else sprintf(str_ra, "%02d:%02d:%06.3f", hr, min, sec);
}
