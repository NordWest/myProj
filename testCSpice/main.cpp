#include <QtCore/QCoreApplication>

#include "./../libs/cspice/SpiceUsr.h"
#include "./../libs/astro.h"
#include "./../libs/mpcs.h"
#include "./../libs/comfunc.h"
#include "./../libs/mpccat.h"
#include "./../libs/observatory.h"
#include "./../libs/observ.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    setlocale(LC_NUMERIC, "C");

    mpc mrec;
    double jdUTC;
    observatory obsy;
    observ obsPos;
    /*
      Local constants
      */

      #define                 FILE_SIZE 128
      #define                 WORD_SIZE  48
      #define                 MAXPTS     9

      /*
      Local variables
      */

      SpiceDouble             et;
      SpiceDouble             ut, utbeg, utend;
      SpiceDouble             delta;
      SpiceDouble             lt1;
      SpiceDouble             lt2, lt3;
      SpiceDouble             state1 [6];
      SpiceDouble             state2 [6];
      SpiceDouble             state3 [6];
      SpiceDouble             times[MAXPTS];
      SpiceDouble             x[MAXPTS];
      SpiceDouble             y[MAXPTS];
      SpiceDouble             z[MAXPTS];
      //SpiceDouble             ra[MAXPTS];
      //SpiceDouble             dec[MAXPTS];
      //SpiceDouble             range[MAXPTS];
      double ra, dec, range;

      SpiceChar               leap  [FILE_SIZE];
      SpiceChar               obs   [WORD_SIZE];
      SpiceChar               targ1 [WORD_SIZE];
      SpiceChar               utctim[WORD_SIZE];
      SpiceChar               utcbeg[WORD_SIZE];
      SpiceChar               utcend[WORD_SIZE];
      SpiceChar             * corr;
      SpiceChar             * ref;
      SpiceChar               *namelen;
      SpiceChar               *stimestr, *timestr, *ampmstr;
      namelen = new SpiceChar[12];
      stimestr = new SpiceChar[64];
      timestr = new SpiceChar[64];
      ampmstr = new SpiceChar[64];
      SpiceInt                hr, mn, sc;


      SpiceBoolean            cont;

      SpiceInt                i, p;

      QString tstr;
      QFile mpcFile;
      QTextStream mpcStm;
      QString mpcFileName;
      double stime;

      //erract_c ( "set", 0, "REPORT" );

      char *astr = new char[256];
      ref = new SpiceChar[256];
      corr = new SpiceChar[256];

      mpccat mCat;

      puts (" ");
      puts (" ");
      puts ("                    Welcome to SIMPLE test program"                  );
      puts (" ");


      QSettings *sett = new QSettings("tcs.ini", QSettings::IniFormat);
      QString bspName = sett->value("general/bspName", "./de421.bsp").toString();
      QString spkName = sett->value("general/spkName", "./smp.spk").toString();
      QString lskName = sett->value("general/leapName", "naif0010.tls").toString();

      sprintf(ref,"%s", sett->value("general/ref", "J2000").toString().toAscii().data());
      sprintf(corr,"%s", sett->value("general/corr", "LT").toString().toAscii().data());
      sprintf(obs,"%s", sett->value("general/obs", "Earth").toString().toAscii().data());

      QStringList objList = sett->value("general/objList").toString().split("|");
      QString mpcCatFile = sett->value("general/mpcCatFile", "mocorb.txt").toString();
      QString obsFile = sett->value("general/obsFile", "./Obs.txt").toString();
      QString obsCode = sett->value("general/obsCode", "084").toString();

      int initMpc = mCat.init(mpcCatFile.toAscii().data());
      if(obsy.init(obsFile.toAscii().data(), OBS_SIZE)) exit(1);
      obsy.getobsynumO(obsCode.toAscii().data());

      obsPos.initObservatory(obsFile.toAscii().data());
      if(obsPos.initSPICE(bspName, lskName)) return 1;
      obsPos.initSPK(spkName);
      obsPos.set_spice_parpam("Earth", obsCode, "sun", "J2000");

      furnsh_c ( "./codes_300ast.tf"  );
      //sprintf(leap,"%s", "./naif0010.tls");
      //furnsh_c ( leap );                        //load LSK kernel
/*
      sprintf(leap,"%s", "./naif0009.tls");
      furnsh_c ( leap );                        //load LSK kernel
      furnsh_c ( bspName.toAscii().data()  );    //load SPK/BSP kernel with planets ephemerides
      furnsh_c ( "./codes_300ast.tf"  );        //load KPL/FK kernel with asteroids names and indexes
      furnsh_c ( spkName.toAscii().data() );                  //load SPK kernel with asteroids aphemerides
*/
        //furnsh_c ( "earth_latest_high_prec.bpc"  );
        //furnsh_c ( "earth_fixed.tf"  );
        //furnsh_c ( "pck00010.tpc"  );

      cont = SPICETRUE;

      mpcFileName = QString("./mpc.txt");
      mpcFile.setFileName(mpcFileName);
      mpcFile.open(QFile::WriteOnly | QFile::Truncate);
      mpcStm.setDevice(&mpcFile);


      //sprintf(obs,"%s", "Earth");

     utbeg = 2456010.5;
     utend = 2456090.5;

     //utbeg = 2434000.5;
     //utend = 2434100.5;

     //utbeg = 2455700.5;
     //utend = 2455800.5;

     delta  = ( utend - utbeg ) / ( (SpiceDouble) MAXPTS  - 1.);

     for(p=0; p<objList.size(); p++)
     {
         sprintf(targ1,"%s", objList.at(p).toAscii().data());

         if(!initMpc) mCat.GetRecName(targ1);


         ut = utbeg;

         printf( "%s rectangilar coords in %s", targ1, ref);
         puts (""  );

         for ( i=0; i < MAXPTS; ++i )
        {

         sprintf(utctim,"%f JD", ut);

         obsPos.setUTC(ut);
         obsPos.det_obj_radec(objList.at(p), &ra, &dec, &range);

         mrec.r = ra;// + dRa;
         mrec.d = dec;// + dDec;

         //tstr = QString(utctim);
         //jdUTC = mjd2jd(getMJDfromStrT(tstr));
         mrec.eJD = obsPos.ctime.UTC();
         //mrec.num = 1;

         if(!initMpc) mCat.record->getNumStr(mrec.head->Snum);
         else mrec.head->set_Snum(1);
         //strcpy(, mCat.record->getNumStr(>number);
         mrec.tail->set_numOfObs(obsCode.toAscii().data());
         mrec.toString(astr);

         mpcStm << astr << "\n";

         ut += delta;

//         str2et_c ( utctim, &et);
/*
         tstr = QString(utctim);
         jdUTC = mjd2jd(getMJDfromStrT(tstr));
         et2lst_c(et, 399, obsy.record->Long, "PLANETOCENTRIC", 64, 64, &hr, &mn, &sc, timestr, ampmstr);
         obsy.det_state(jdUTC);

         qDebug() << QString("obsy: %1\t%2\t%3\n").arg(obsy.dcx).arg(obsy.dcy).arg(obsy.dcz);
         qDebug() << QString("%1:%2:%3 - %4 - %5\n").arg(hr).arg(mn).arg(sc).arg(timestr).arg(ampmstr);
         rad2hms_str(obsy.stime, stimestr);
         qDebug() << QString("stime: %1 - %2\n").arg(24.0*obsy.stime/(2.0*PI)).arg(stimestr);

         stime = ((sc/60.0+mn)/60.0 + hr)/24.0;
         obsy.dcx = obsy.h*obsy.record->Cos*cos(stime);
         obsy.dcy = obsy.h*obsy.record->Cos*sin(stime);
         obsy.dcz = obsy.h*obsy.record->Sin;
/
         //find state vector of targ1 body relative to obs body and ssb point
         spkezr_c (  targ1, et, ref, corr, obs, state1, &lt1 );
         qDebug() << QString("lt: %1\n").arg(lt1);
         spkezr_c (  targ1, et, ref, "NONE", "ssb", state2, &lt2 );
         spkezr_c (  obs, et, ref, "NONE", "sun", state3, &lt3 );
/*
         state1[0] -= obsy.dcx*AUKM;
         state1[1] -= obsy.dcy*AUKM;
         state1[2] -= obsy.dcz*AUKM;
/
         x[i] = state1[0];
         y[i] = state1[1];
         z[i] = state1[2];

         //determine a celestial coordinates of state vector in selected reference frame
         recrad_c (state1, &range[i], &ra[i], &dec[i]);
         times[i] = et;
         ut   = ut + delta;

         et2utc_c ( et, "ISOC", 0, WORD_SIZE, utctim );
         printf ( "  %.20s:\t%15.8f\t%15.8f\t%15.8f\t%15.8f\n", utctim, state2[0]/AUKM,  state2[1]/AUKM, state2[2]/AUKM, sqrt(state2[0]*state2[0]+state2[1]*state2[1]+state2[2]*state2[2])/AUKM);

         qDebug() << QString("Earth@sun: %1\t%2\t%3\t%4\t%5\t%6\n").arg(state3[0]/AUKM, 15, 'e', 10).arg(state3[1]/AUKM, 15, 'e', 10).arg(state3[2]/AUKM, 15, 'e', 10).arg(state3[3]/AUKM, 15, 'e', 10).arg(state3[4]/AUKM, 15, 'e', 10).arg(state3[5]/AUKM, 15, 'e', 10);
*/
        }
/*
             puts( " ");
             printf( "The rec coords of %s relative to obs ,\n",
                                                           targ1);
             printf( "as seen from body %s.\n", obs );

             puts( " ");
             printf( "From: %f\n", utbeg );
             printf( "To  : %f\n", utend );

             puts (" ");
             puts ("       UTC Time                 X Y Z R" );
             puts ("----------------------------------------------");
             if(!initMpc) mCat.GetRecName(targ1);

             for ( i = 0; i < MAXPTS; ++i )
                {
                et2utc_c ( times[i], "ISOC", 0, WORD_SIZE, utctim );

                mrec.r = ra[i];// + dRa;
                mrec.d = dec[i];// + dDec;

                tstr = QString(utctim);
                jdUTC = mjd2jd(getMJDfromStrT(tstr));
                mrec.eJD = jdUTC;
                //mrec.num = 1;

                if(!initMpc) mCat.record->getNumStr(mrec.head->Snum);
                else mrec.head->set_Snum(1);
                //strcpy(, mCat.record->getNumStr(>number);
                mrec.tail->set_numOfObs(obsCode.toAscii().data());
                mrec.toString(astr);

                mpcStm << astr << "\n";
                printf ( "  %.20s:\t%15.8f\t%15.8f\t%15.8f\t%15.8f\n", utctim, x[i]/AUKM,  y[i]/AUKM, z[i]/AUKM, range[i]/AUKM);
                //printf ( "  %.20s:\t%15.8f\t%15.8f\t%15.8f\t%15.8f\n", utctim, x[i],  y[i], z[i], range[i]);
                }

             puts( " ");
             puts( "#######################################################");
             puts( " ");
*/

     }
    mpcFile.close();
    return 0;//a.exec();
}
