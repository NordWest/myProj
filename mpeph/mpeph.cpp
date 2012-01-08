#include "mpeph.h"
#include "./../astro/astro.h"
/*о командной строчке замолвите словечко...
mpeph.exe -num 308369 pc.txt astpos.txt - вот командная строка, это пример, конечно
1 -num - это значит задается номер астероида (как в примере)
  -name - это значит задается имя (Ceres, 2005_NB7).
2 номер или имя астероида
3 файл содержащий момент, для которого считаются эфемериды в виде mjd
4 файл в который будет выдан результат

пример файла pc.txt
 0       1              2               3
cat| 05 54 58.197 | +33 12 50.61 | 54188.7768690278
можно, конечно было включить mjd как аргумент командной строки, но это неудобно при пакетной обработке.
Поскольку все затевалось для нее, пришлось действовать "запутанным образом"
*/

mpeph::mpeph(QCoreApplication *app)//конструктор
{	
	cdsfapp = app;
	QString msgstr;
        QString mjdfname;
        QString sScale;
        DATEOBS dObs;
        QTextStream stream(stdout);
        //stream.setDevice(QIODevice(stdout));
	//все настройки в файле mpeph.ini мы их читаем 
	settings = new QSettings("mpeph.ini",QSettings::IniFormat);
	hostName = settings->value("general/hostName").toString();// понятно... имя хоста, вдруг изменят. Поэтому разумно его держать в настройках
	proxyName = settings->value("general/proxyName").toString();// ну это прокси
	replyDir = settings->value("general/replyDir").toString();// это папка, в которую выдается файлик с результатом
	proxyPort = settings->value("general/proxyPort").toInt();// не по порядку правда - это порт прокси
	useProxy = settings->value("general/useProxy").toInt();// 0 - не используем прокси, 1 - используем
        useNetworkIni = settings->value("general/useNetworkIni", 0).toInt();
        if(useNetworkIni)
        {
            QSettings *netSett = new QSettings("network.ini",QSettings::IniFormat);
            proxyName = netSett->value("general/proxyName").toString();
            proxyPort = netSett->value("general/proxyPort").toInt();
            useProxy = netSett->value("general/useProxy").toInt();
        }
	eventMessages = settings->value("general/eventMessages").toInt();//0 - не выдаем текстовые сообщения о состоянии запроса, 1 - выдаем
	obsCode = settings->value("general/obsCode").toString();// код обсерватории
        centre = settings->value("general/centre").toString();//Reference center - тип положения наблюдателя. 1-гелиоцентр, 2-геоцентр, 3-хз, 4- по коду обсерватории (UAI_code)
        if(centre.toInt()<0||centre.toInt()>4) centre = QString("4");                      //если такого пункта нет в ini-файле, то параметр будет по старинке равер 4

        keph = settings->value("general/keph").toString();//Номер эфемериды:1-DE200 / LE200; 2-VSOP82 / ELP2000-82; 3-DE403 / LE403; 4-VSOP87 / ELP2000-82B; 5-DE405 / LE405; 6-DE406 / LE406; 7-INPOP06
        if(keph=="") keph = QString("5");                      //если такого пункта нет в ini-файле, то параметр будет по старинке равер 5
//        qDebug() << "keph=" << keph << "\n";
        timescale = settings->value("general/timescale").toInt();// 0 - UTC, 1 - GPS,2 - TT
        if(timescale==2) sScale = QString("TT");
        else sScale = QString("UTC");
	msgout = settings->value("general/msgout").toInt();//std out
	simplemode = settings->value("general/simplemode").toInt();
	/////////////////////////////////////////////прочитали настройки
	if (msgout==1) stream << "mpeph has started...\n";
	////////////////////////////////////////////////////
        http = new QHttp(this);//создаем объект для http запросов

	if (useProxy==1) 
        {
            if (msgout==1) stream << "useProxy...\n";
		http->setProxy(proxyName, proxyPort);//если есть прокси, устанавливаем соответсвующие параметры
	}
        if (msgout==1) stream << "setHost...\n";
	http->setHost(hostName);//устанавливаем имя хоста
	// связываем сигналы и слоты
	connect(http, SIGNAL(done(bool)), this, SLOT(slotProcessingData(bool)));
	connect(http, SIGNAL(stateChanged(int)), this, SLOT(slotStateChanged(int)));
	connect(http, SIGNAL(requestFinished(int, bool)), this, SLOT(slotRequestFinished(int, bool)));
	//
        if (msgout==1) stream << "arguments...\n";
        /*int argSz = cdsfapp->arguments().size();
        QString argI;
        for(i=1; i<argSz; i++)
        {
            argI = cdsfapp->arguments().at(i);
            if(QString().compare(argI[0], "-")==0)
            {

            }
        }*/
	ntype = cdsfapp->arguments().at(1);// это первый аргумент (тип ввода информации об астероиде: номер или имя)
	if(ntype=="-num")astNumber = cdsfapp->arguments().at(2); else astDes = cdsfapp->arguments().at(2);//или номер, или имя
	//BEGIN find mjd
	QString sMJD;
	double mjd;
	if(simplemode==0)
	{
                mjdfname = cdsfapp->arguments().at(3);// имя файла с моментом mjd
		QFile mjdFile(mjdfname);
		mjdFile.open(QIODevice::ReadOnly | QIODevice::Text);
		QTextStream mjdStream;
		mjdStream.setDevice(&mjdFile);
		QString mjdLine = mjdStream.readLine();
		mjdFile.close();
		sMJD = mjdLine.section('|',3,3);// читаем mjd
		mjd = sMJD.toDouble();//конвертируем в число
	}
	if(simplemode==1)
	{
		sMJD = cdsfapp->arguments().at(3);
		mjd = mjd = sMJD.toDouble();
	}
	if(simplemode==2)
	{
		sMJD = cdsfapp->arguments().at(3);
		mjd = getMJDfromStrFTN(sMJD, 0);
	}

  //      uploadfile_dat
	if(timescale==1) mjd = mjd - 14/86400;//если использовалась шкала GPS, переходим в UTC (в таком виде это справедливо с 2006 г) 
        //END find mjd
        QString requestStr = "/cgi-bin/ephepos.cgi/calcul";// создаем http запрос (теория DE405, шкала времени UTC)

            dObs = getDATEOBSfromMJD(mjd);// конвертируем mjd в "нормальную" дату
                requestStr = requestStr +
		"?an="+ QString( "%1" ).arg( dObs.year,4,10,QLatin1Char( '0' ))+
		"&mois="+QString( "%1" ).arg( dObs.month,4,10,QLatin1Char( '0' ))+
		"&jour="+QString( "%1" ).arg( dObs.day,4,10,QLatin1Char( '0' ))+
		"&heure="+QString( "%1" ).arg( dObs.hour,4,10,QLatin1Char( '0' ))+
		"&minute="+QString( "%1" ).arg( dObs.min,4,10,QLatin1Char( '0' ))+
		"&seconde="+QString( "%1" ).arg( dObs.sec,6,'f',3,QLatin1Char( '0' ));

        QString ast_Des = astDes.replace(QString("_"), QString(" "));//все пробелы в имени астероида должны в командной строке заменяться символами подчеркивания, для запроса они заменяются обратно на пробел
        if(ntype=="-num")requestStr = requestStr + "&numaster="+astNumber; else requestStr = requestStr +"&nomaster="+ast_Des;
        requestStr = requestStr + "&labeldatejj=0&UAI_code="+obsCode+"&planete=Aster&nbdates=1&centre="+centre+"&keph="+keph+"&scale="+sScale;
    //                if(simplemode==3) requestStr = requestStr + "&uploadfile_dat="+mjdfname;
        if (msgout==1) stream << requestStr<<"\n";//печатать запрос в консоли
	http->get(requestStr);// посылаем запрос методом get
	
};
//?-to=4&-from=-2&-this=-2&-out.max=unlimited&-out.form=%7C+-Separated-Values&-order=I&-c=20+54+05.689+%2B37+01+17.38&-c.eq=J2000&-oc.form=dec&-c.r=+10&-c.u=arcmin&-c.geom=r&-source=I%2F304%2Fout&-out=CMC14&CMC14=&-out=f_CMC14&f_CMC14=&-out=RAJ2000&-sort=RAJ2000&RAJ2000=&-out=DEJ2000&DEJ2000=&-out=MJD-51263&MJD-51263=&-out=r%27mag&r%27mag=&-out=u_r%27mag&u_r%27mag=&-out=Nt&Nt=&-out=Na&Na=&-out=Np&Np=&-out=e_RAdeg&e_RAdeg=&-out=e_DEdeg&e_DEdeg=&-out=e_r%27mag&e_r%27mag=&-out=Jmag&Jmag=&-out=Hmag&Hmag=&-out=Ksmag&Ksmag=&-out.all=1&-file=.&-meta=2&-meta.foot=1
void mpeph::slotProcessingData(bool error)// обработка ответа на запрос
{
    //if (msgout==1) qDebug() << QString("slotProcessingData\n");
    QTextStream stream(stdout);

        if (msgout==1) stream << "slotProcessingData...\n";
        //if (msgout==1) stream << QString("error: %1\n").arg(error);
	if (error)//если ошибка - сообщить в консоль и выйти из приложения
	{
		if (msgout==1)stream << "error has occured.\n";
		cdsfapp->quit();
	}
	else
		{
                        if (msgout==1) stream << "data is being loaded...\n";
			QByteArray httpData = http->readAll();//чтение данных (ответа)
	        QString str(httpData);// переделываем данные в строку
			//
                        //stream << str << "\n";
			//
                int si, k, ex;
                QStringList sl;
                QStringList nList;
                QString respStr;
                        respStr.clear();
                        si = str.indexOf("Asteroide");
                        k = 0;
                        ex = 0;
                        int se = str.indexOf("<", si);
                        respStr = str.mid(si, se-si);

                        QString astNum = respStr.section(" ", 1, 1, QString::SectionSkipEmpty);
                        QString astName = respStr.section(" ", 2, -1, QString::SectionSkipEmpty);

                        sl.clear();
                        respStr.clear();
                        si = str.indexOf("km");
                        if (msgout==1) stream << QString("si= %1\n").arg(si);

                        k = 0;
                        ex = 0;
			while(ex==0)
			{
				if(str[si]=='>')k++;
				if(k==13) 
				{
					if(str[si]!='<')respStr = respStr + str[si];
					else ex=1;
				}
				si++;
			}
			//
                        //qDebug() << QString("respStr: %1\n").arg(respStr);
			for(int i=1;i<respStr.length();i++)
			{
				if((respStr[i-1]==' ')&&(respStr[i]!=' ')) respStr[i-1]='|';
			}
			//stream << respStr << "\n";
			/////////////////////
			//дальше идет нудное выколачивание из html-кода ответа нужной строчки
			if (msgout==1) stream << str <<"\n";
			///////////////////////
                        sl = respStr.split("|");
			////////////////////////
			QString spos = sl[sl.count()-13]+":"+sl[sl.count()-12]+":"+sl[sl.count()-11];
			double ra = hms_to_mas(spos, ":");
			spos = sl[sl.count()-10]+":"+sl[sl.count()-9]+":"+sl[sl.count()-8];
			double de = damas_to_mas(spos, ":");
			str = sl[sl.count()-3];
			double ra_dot = 60000*str.toDouble();
			str = sl[sl.count()-2];
			double de_dot = 60000*str.toDouble();
			spos = mas_to_hms(ra, ":", 5)+"|"+
				   mas_to_damas(de, ":", 4)+"|"+
				   sl[sl.count()-7].trimmed()+"|"+//distance (a.e)
				   sl[sl.count()-6].trimmed()+"|"+//vis. mag
				   sl[sl.count()-5].trimmed()+"|"+//phase(deg)
				   sl[sl.count()-4].trimmed()+"|"+//elongation(deg)
				   QString( "%1" ).arg( ra_dot,9,'f',2,QLatin1Char( ' ' ))+"|"+//mu_ra*cos(dec) (mas/min)
				   QString( "%1" ).arg( de_dot,9,'f',2,QLatin1Char( ' ' ))+"|"+//mu_de  (mas/min)
				   sl[sl.count()-1]+"|";//radial velocity (km/s)
                        spos += astNum + "|" + astName;

                        //if(ntype=="-num")spos = spos + astNumber; else spos = spos + astDes;
			///////////////////////выдаем результат в файл
			if(simplemode==0)
			{
				QString fname = cdsfapp->arguments().at(4);
				QFile httpFile(replyDir+"/"+fname);
				httpFile.open(QIODevice::WriteOnly | QIODevice::Text);
				QTextStream httpStream;
				httpStream.setDevice(&httpFile);
				httpStream << spos;
				httpFile.close();
			}
			if(simplemode>0)
			{
				stream << spos << "\n";
			}
			if (msgout==1) stream << "mpeph has finished...\n";
                        cdsfapp->quit();//выходим из проги
		}
};

void mpeph::slotStateChanged(int state)//этот слот печатает в консоли состояние запроса, если соответствующий флаг установлен
{
    //qDebug() << QString("slotStateChanged");
	if (eventMessages)
	{
                QTextStream stream(stdout);

                if (msgout==1) stream << "slotStateChanged...\n";
		QString str_state;
		switch(state)
		{
			case QHttp::Unconnected: 
			  str_state = "There is no connection to the host.";
			  break;
			case QHttp::HostLookup: 
			  str_state = "A host name lookup is in progress.";
			  break;
			case QHttp::Connecting:
			  str_state = "An attempt to connect to the host is in progress.";
			  break;
			case QHttp::Sending:
			  str_state = "The client is sending its request to the server.";
			  break;
			case QHttp::Reading:
			  str_state = "The client's request has been sent and the client is reading the server's response.";
			  break;
			case QHttp::Connected:
			  str_state = "The connection to the host is open, but the client is neither sending a request, nor waiting for a response.";
			  break;
			case QHttp::Closing:
			  str_state = "The connection is closing down, but is not yet closed.";
			  break;
		}	
		stream << str_state << "\n";
	}
};

/*void mpeph::slotSslErrors(QList<QSslError> sslErr)
{
	QTextStream stream(stdout);
	stream << "ssl error has occured." << "\n";
};*/

void mpeph::slotRequestFinished(int id, bool error)//запрос завершен, если с ошибкой, то выдается сообщение о ее природе
{
   //if (msgout==1) qDebug() << QString("slotRequestFinished\n");
    QTextStream stream(stdout);
if (msgout==1) stream << "slotRequestFinished...\n";
	if(error)
	{


		if (msgout==1) stream << http->errorString() << "\n";
	}
        //cdsfapp->quit();
};

