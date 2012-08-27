#include "sateph.h"
#include "astro.h"

QTextStream err_stream;
		


sateph::sateph(QCoreApplication *app)
{
	QFile fout("error.log");
	fout.open(QIODevice::Truncate | QIODevice::WriteOnly);
	err_stream.setDevice(&fout);
		
	sateph_app = app;
	//BEGIN read settings
	settings = new QSettings("sateph.ini",QSettings::IniFormat);
	hostName = settings->value("general/hostName").toString();
	proxyName = settings->value("general/proxyName").toString();
	proxyPort = settings->value("general/proxyPort").toInt();
	useProxy = settings->value("general/useProxy").toInt();
	obsCode = settings->value("general/obsCode").toString();// код обсерватории
	timescale = settings->value("general/timescale").toInt();// 0 - UTC, 1 - GPS
	theory = settings->value("general/theory").toString();
	sattheory = settings->value("general/sattheory").toString();
	noutput = settings->value("general/noutput").toInt();
	timemode = settings->value("general/timemode").toInt();
	noutput++;
	planet=settings->value("general/planet").toString();
	//END read settings
	//BEGIN set host
	http = new QHttp(this);
	connect(http, SIGNAL(done(bool)), this, SLOT(slotProcessingData(bool)));
	connect(http, SIGNAL(requestFinished(bool)), this, SLOT(slotProcessingData(bool)));
	http->setHost(hostName);
	if (useProxy==1) http->setProxy(proxyName, proxyPort);
	//END set host
	//BEGIN arguments
	QString sMJD;
	if(timemode==0)	sMJD = sateph_app->arguments().at(1);
	if(timemode==1) sMJD = QString( "%1" ).arg( getMJDfromStrFTN(sateph_app->arguments().at(1), 0),18,'f',10,QLatin1Char( ' ' ));
	err_stream << sMJD;
	//QTextStream stream(stdout);
	//stream << sMJD << "mjd\n";
	//END arguments
	//BEGIN request for positions
	busy = 0;
	QString posrequest;
	QString scriptpath;
	if(hostName=="www.imcce.fr")scriptpath ="/cgi-bin/saimirror/nss-eph3.cgi?langue=30";
	if(hostName=="lnfm1.sai.msu.su")scriptpath = "/neb/nss/cgi-bin/nss-eph3.cgi?langue=32";
	posrequest = scriptpath+
			"&plnvar="+sattheory+"&satellite="+planet+"001&relative=-1&nde="+theory+"&observatory=" + obsCode +
				"&epoch=ICRF&tscale=UTC&initform=3&outputtype=0&steptype=-2&initmom=%23+NSAT%3A1";
				for(int i=0;i<noutput;i++) 
					posrequest = posrequest + "%0D%0A" + QString( "%1" ).arg(i,2,10,QLatin1Char( '+' ))+"+"+sMJD;
				posrequest = posrequest + "&timestep=1&ntimes=4&visible=&vangle=0&reserve=0";
	http->get(posrequest);
	while(busy==0)sateph_app->processEvents();
	posrequest =  scriptpath +
			"&plnvar="+sattheory+"&satellite="+planet+"001&relative=-1&nde="+theory+"&observatory=" + 
				obsCode +"&epoch=ICRF&tscale=UTC&initform=3&outputtype=51&steptype=-2&initmom=%23+NSAT%3A1";
				for(int i=0;i<noutput;i++) 
					posrequest = posrequest + "%0D%0A" + QString( "%1" ).arg(i,2,10,QLatin1Char( '+' ))+"+"+sMJD;
				posrequest = posrequest + "&timestep=1&ntimes=4&visible=&vangle=0&reserve=0";
	http->get(posrequest);
	//END request for positions 
};

void sateph::slotProcessingData(bool error)
{
	QTextStream stream(stdout);
	if (error)
	{
		stream << "error has occured.\n";
		sateph_app->quit();
	}
	else
		{
			QTextStream responseStream(http->readAll());
			QString line;
			QString sRA,sDE;
			double ra,de;
			int ls,k,q;
			if (busy==0)
			{
				while (!responseStream.atEnd())
				{
					line = responseStream.readLine();
					ls = line.size()-1;
					if((line[ls]=='0')||(line[ls]=='1')||(line[ls]=='2')||(line[ls]=='3')||(line[ls]=='4')||
					(line[ls]=='5')||(line[ls]=='6')||(line[ls]=='7')||(line[ls]=='8')||(line[ls]=='9'))
					{
						outputList << line;
					}
				}
				busy = 1;
			}
			else
			{
				k = 0;
				outStr ="";
				while (!responseStream.atEnd())
				{
					line = responseStream.readLine();
					//stream << line << "\n";
					ls = line.size()-1;
					if((line[ls]=='0')||(line[ls]=='1')||(line[ls]=='2')||(line[ls]=='3')||(line[ls]=='4')||
					(line[ls]=='5')||(line[ls]=='6')||(line[ls]=='7')||(line[ls]=='8')||(line[ls]=='9'))
					{
						//outputList[k] = outputList[k] + line + "\n";
						//stream << outputList[k];
						//BEGIN convert line
						line = outputList[k] + line + "\n";
						line = line.trimmed();
						q=0;
						for(int i=1;i<line.size();i++)
						{
							if((line[i-1]!='|')&&(line[i-1]!=' ')&&(line[i]==' '))
							{
								line.replace(i,1,'|');
								q++;
							}
						}
						//END convert line
						//stream << line << "\n";
						sRA = line.section('|',1,1)+":"+line.section('|',2,2)+":"+line.section('|',3,3);
						sDE = line.section('|',4,4)+":"+line.section('|',5,5)+":"+line.section('|',6,6);
						ra = hms_to_mas(sRA, ":");
						de = damas_to_mas(sDE, ":");
						outStr = outStr + mas_to_hms(ra, ":", 6) + "|" + //0 - ra
								mas_to_damas(de, ":", 6) + "|" + //1 - de
								line.section('|',12,12) + "|" +  //2 - Obj.mag. - satellite magnitude (V)
								line.section('|',9,9) + "|" +    //3 - Sun-plan[deg] -  Apparent topocentric distance Sun-Planet (degrees);
								line.section('|',10,10) + "|" +  //4 - Phase[deg] - phase angle (degrees)
								line.section('|',11,11) + "|" +  //5 - Rad.plan. - apparent planetary radius (arcesec)
								line.section('|',8,8) + "|" +    //6 - mjd
								line.section('|',7,7) + "\n";     //7 - sat number
						k++;
					}
				}
				stream << outStr;
				sateph_app->quit();
			}
		}		
}