#include "miriadeeph.h"

/*о командной строчке замолвите словечко...
miriadeEph.exe -num 308369 pc.txt astpos.txt - вот командная строка, это пример, конечно
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

miriadeEph::miriadeEph(QCoreApplication *app)//конструктор
{	
	cdsfapp = app;
	QString msgstr;
        QString mjdfname;
        QString sScale;
        DATEOBS dObs;
        int i;
        QTextStream stream(stdout);
        //stream.setDevice(QIODevice(stdout));
	//все настройки в файле miriadeEph.ini мы их читаем 
        settings = new QSettings("miriadeeph.ini",QSettings::IniFormat);
        hostName = QString("vo.imcce.fr");//settings->value("general/hostName").toString();// понятно... имя хоста, вдруг изменят. Поэтому разумно его держать в настройках
        proxyName = settings->value("general/proxyName", "proxy.gao.spb.ru").toString();// ну это прокси
        replyDir = settings->value("general/replyDir", "./").toString();// это папка, в которую выдается файлик с результатом
        proxyPort = settings->value("general/proxyPort", 3128).toInt();// не по порядку правда - это порт прокси
        useProxy = settings->value("general/useProxy", 0).toInt();// 0 - не используем прокси, 1 - используем
        useNetworkIni = settings->value("general/useNetworkIni", 0).toInt();
        if(useNetworkIni)
        {
            QSettings *netSett = new QSettings("network.ini",QSettings::IniFormat);
            proxyName = netSett->value("general/proxyName", "proxy.gao.spb.ru").toString();
            proxyPort = netSett->value("general/proxyPort", 3128).toInt();
            useProxy = netSett->value("general/useProxy", 0).toInt();
        }
	eventMessages = settings->value("general/eventMessages").toInt();//0 - не выдаем текстовые сообщения о состоянии запроса, 1 - выдаем
        //obsCode = settings->value("general/obsCode").toString();// код обсерватории
        //centre = settings->value("general/centre").toString();//Reference center - тип положения наблюдателя. 1-гелиоцентр, 2-геоцентр, 3-хз, 4- по коду обсерватории (UAI_code)
        if(centre.toInt()<0||centre.toInt()>4) centre = QString("4");                      //если такого пункта нет в ini-файле, то параметр будет по старинке равер 4

        //keph = settings->value("general/keph").toString();//Номер эфемериды:1-DE200 / LE200; 2-VSOP82 / ELP2000-82; 3-DE403 / LE403; 4-VSOP87 / ELP2000-82B; 5-DE405 / LE405; 6-DE406 / LE406; 7-INPOP06
        //if(keph=="") keph = QString("5");                      //если такого пункта нет в ini-файле, то параметр будет по старинке равер 5
//        qDebug() << "keph=" << keph << "\n";
        //timescale = settings->value("general/timescale").toInt();// 0 - UTC, 1 - GPS,2 - TT
        //if(timescale==2) sScale = QString("TT");
        //else sScale = QString("UTC");
	msgout = settings->value("general/msgout").toInt();//std out
	simplemode = settings->value("general/simplemode").toInt();

        /////////////////////////////////////////////прочитали настройки
	if (msgout==1) stream << "miriadeEph has started...\n";
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

        QStringList reqList;
        QString requestStr = "/webservices/miriade/ephemcc_query.php?";// создаем http запрос (теория DE405, шкала времени UTC)

        int argSz = cdsfapp->arguments().size();

        for(i=1; i<argSz; i++)
        {
            reqList << cdsfapp->arguments().at(i);
        }

        settings->beginGroup("params");
        QStringList parKeys;

        parKeys << settings->allKeys();
        int szPar = parKeys.size();
        int isExist, j;
        //if (msgout==1) stream << QString("szPar: \n").arg(i).arg(parKeys.at(i));
        for(i=0; i<szPar; i++)
        {
            //parKeys.at(i)if (msgout==1) stream << QString("par[%1]: %2\n").arg(i).arg(parKeys.at(i));
            isExist = 0;
            for(j=0;j<reqList.size(); j++) if(reqList.at(j).contains(parKeys.at(i))) isExist = 1;

            if(!isExist)
            {
                reqList << QString("-%1=%2").arg(parKeys.at(i)).arg(settings->value(parKeys.at(i)).toString());
            }
        }




        requestStr += reqList.join("&");

        if (msgout==1) stream << requestStr<<"\n";//печатать запрос в консоли
	http->get(requestStr);// посылаем запрос методом get
	
};
//?-to=4&-from=-2&-this=-2&-out.max=unlimited&-out.form=%7C+-Separated-Values&-order=I&-c=20+54+05.689+%2B37+01+17.38&-c.eq=J2000&-oc.form=dec&-c.r=+10&-c.u=arcmin&-c.geom=r&-source=I%2F304%2Fout&-out=CMC14&CMC14=&-out=f_CMC14&f_CMC14=&-out=RAJ2000&-sort=RAJ2000&RAJ2000=&-out=DEJ2000&DEJ2000=&-out=MJD-51263&MJD-51263=&-out=r%27mag&r%27mag=&-out=u_r%27mag&u_r%27mag=&-out=Nt&Nt=&-out=Na&Na=&-out=Np&Np=&-out=e_RAdeg&e_RAdeg=&-out=e_DEdeg&e_DEdeg=&-out=e_r%27mag&e_r%27mag=&-out=Jmag&Jmag=&-out=Hmag&Hmag=&-out=Ksmag&Ksmag=&-out.all=1&-file=.&-meta=2&-meta.foot=1
void miriadeEph::slotProcessingData(bool error)// обработка ответа на запрос
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
                        stream <<str.mid(str.indexOf("#!")) << "\n";


                        cdsfapp->quit();//выходим из проги
                }
};

void miriadeEph::slotStateChanged(int state)//этот слот печатает в консоли состояние запроса, если соответствующий флаг установлен
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

/*void miriadeEph::slotSslErrors(QList<QSslError> sslErr)
{
	QTextStream stream(stdout);
	stream << "ssl error has occured." << "\n";
};*/

void miriadeEph::slotRequestFinished(int id, bool error)//запрос завершен, если с ошибкой, то выдается сообщение о ее природе
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

