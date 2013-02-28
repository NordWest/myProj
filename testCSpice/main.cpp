#include <QtCore/QCoreApplication>

#include "./../libs/cspice/SpiceUsr.h"
#include "./../libs/astro.h"
#include "./../libs/mpcs.h"
#include "./../libs/comfunc.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    setlocale(LC_NUMERIC, "C");

    mpc mrec;
    double jdUTC;
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
      SpiceDouble             etbeg;
      SpiceDouble             etend;
      SpiceDouble             delta;
      SpiceDouble             lt1;
      SpiceDouble             lt2;
      SpiceDouble             state1 [6];
      SpiceDouble             state2 [6];
      SpiceDouble             times[MAXPTS];
      SpiceDouble             x[MAXPTS];
      SpiceDouble             y[MAXPTS];
      SpiceDouble             z[MAXPTS];
      SpiceDouble             ra[MAXPTS];
      SpiceDouble             dec[MAXPTS];
      SpiceDouble             range[MAXPTS];

      SpiceChar               leap  [FILE_SIZE];
      SpiceChar               obs   [WORD_SIZE];
      SpiceChar               spk   [FILE_SIZE];
      SpiceChar               targ1 [WORD_SIZE];
      SpiceChar               targ2 [WORD_SIZE];
      SpiceChar               utctim[WORD_SIZE];
      SpiceChar               utcbeg[WORD_SIZE];
      SpiceChar               utcend[WORD_SIZE];
      SpiceChar               answer[WORD_SIZE];
      SpiceChar             * corr;
      SpiceChar             * ref;

      SpiceBoolean            cont;

      SpiceInt                i;

      puts (" ");
      puts (" ");
      puts ("                    Welcome to SIMPLE"                  );
      puts (" ");
      /*puts ("This program calculates the angular separation of two"  );
      puts ("target bodies as seen from an observing body."          );
      puts (" ");
      puts ("The angular separations are calculated for each of 10"  );
      puts ("equally spaced times in a given time interval. A table" );
      puts ("of the results is presented.");
      puts (" ");*/


      /*
      Set the time output format, the precision of that output
      and the reference frame.  Note:  The angular separation has the
      same value in all reference frames.  Let's use our favorite, J2000.
      We need an aberration correction.  "LT+S", light time plus stellar
      aberration, satisfies the requirements for this program.
      */
      ref    = "J2000";
      corr   = "LT";
      //corr   = "NONE";


      /*
      Get the various inputs using interactive prompts:
      */
      //puts (" ");
      //prompt_c ( "Enter the name of a leapseconds kernel file: ",
  //                                                      WORD_SIZE, leap );
      //puts (" ");

      /*
      First load the leapseconds file into the kernel pool, so
      we can convert the UTC time strings to ephemeris seconds
      past J2000.
      */
      sprintf(leap,"%s", "./naif0009.tls");
      furnsh_c ( leap );


      //prompt_c ( "Enter the name of a binary SPK ephemeris file: ",
//                                                        WORD_SIZE, spk  );

      /*
      Load the binary SPK file containing the ephemeris data
      that we need.
      */
      sprintf(spk,"%s", "de421.bsp");
      furnsh_c ( spk  );
      furnsh_c ( "./de421.cmt"  );

      //sprintf(spkAst,"%s", "codes_300ast_20100725.bsp");
      //furnsh_c ( "codes_300ast_20100725.bsp"  );
      furnsh_c ( "codes_300ast_20100725.cmt"  );
      furnsh_c ( "small.spk"  );

      cont = SPICETRUE;

      QFile mpcFile("./mpc.txt");
      mpcFile.open(QFile::WriteOnly | QFile::Truncate);
      QTextStream mpcStm(&mpcFile);
      QString tstr;
      char *astr = new char[256];

      /* Loop till the user quits. */
 //     do
 //        {

         /*
         Get the NAIF IDs for the two target bodies and the observing
         body.
         */
/*
         puts(" ");
         prompt_c( "Enter the name of the observing body: ",
                                                        WORD_SIZE, obs );

         puts(" ");
         prompt_c( "Enter the name of the first target body: ",
                                                      WORD_SIZE, targ1 );

         puts(" ");
         prompt_c( "Enter the name of the second target body: ",
                                                      WORD_SIZE, targ2 );
*/
          sprintf(obs,"%s", "Earth");
          sprintf(targ1,"%s", "Ceres");
         /*
         Get the beginning and ending UTC times for the time interval
         of interest.
         */
/*         puts(" ");
         prompt_c ( "Enter the beginning UTC time: ", WORD_SIZE, utcbeg );

         puts(" ");
         prompt_c ( "Enter the ending UTC time: ",    WORD_SIZE, utcend );

         puts (" ");
         puts ("Working ... Please wait.");
         puts (" ");
*/
          utbeg = 2456010.5;
          utend = 2456090.5;
         //sprintf(utcbeg,"%f JD", "2456000.5 JD");
         //sprintf(utcend,"%s", "2456100.5 JD");

         /*
         Convert the UTC times to ephemeris seconds past J2000 (ET),
         since that is what the SPICELIB readers are expecting.
         */
        // str2et_c ( utcbeg, &etbeg );
         //str2et_c ( utcend, &etend );
         //et2utc_c ( etbeg, "C", 0, WORD_SIZE, utcbeg );
         //et2utc_c ( etend, "C", 0, WORD_SIZE, utcend );


         /*
         Calculate the difference between evaluation times.
         */
         delta  = ( utend - utbeg ) / ( (SpiceDouble) MAXPTS  - 1.);

         /*
         For each time, get the apparent states of the two target
         bodies as seen from the observer.
         */
         //et = etbeg;
         //sprintf(utctim,"%f JD", 2456000.5);
         ut = utbeg;

         puts ("Ceres to ssb"  );
         puts (""  );

         for ( i=0; i < MAXPTS; ++i )
            {
            /*
            Compute the state of targ1 and targ2 from obs at et then
            calculate the angular separation between targ1 and targ2
            as seen from obs. Convert that angular value from radians
            to degrees.
            */
             sprintf(utctim,"%f JD", ut);
             str2et_c ( utctim, &et);
             spkezr_c (  targ1, et, ref, corr, obs, state1, &lt1 );
             spkezr_c (  "Ceres", et, ref, "NONE", "ssb", state2, &lt2 );
             x[i] = state1[0];
             y[i] = state1[1];
             z[i] = state1[2];
            //spkezr_c (  targ2, et, ref, corr, obs, state2, &lt2 );
             recrad_c (state1, &range[i], &ra[i], &dec[i]);
             times[i] = et;
            //et   = et + delta;
             ut   = ut + delta;

             et2utc_c ( et, "ISOC", 0, WORD_SIZE, utctim );
             printf ( "  %.20s:\t%15.8f\t%15.8f\t%15.8f\n", utctim, state2[0]/AUKM,  state2[1]/AUKM, state2[2]/AUKM);
            /*
            Save the time and the separation between the target bodies
            (in degrees), as seen from the observer, for output to the
            screen.
            */
            /*
            x[i] = et;
            y[i] = vsep_c ( state1, state2) * dpr_c();
            et   = et + delta;*/
            }

         /*
         Display the time and angular separation of the desired
         target bodies for the requested observer for each of the
         equally spaced evaluation times in the given time interval.

         If you have a graphics package, you may wish to write the
         time and angular separation data to a file, and then plot
         them for added effect.
         */
         puts( " ");
         printf( "The angular separation between bodies %s ,\n",
                                                       targ1);
         printf( "as seen from body %s.\n", obs );

         puts( " ");
         printf( "From: %s\n", utcbeg );
         printf( "To  : %s\n", utcend );

         puts (" ");
         puts ("       UTC Time                 Separation" );
         puts ("----------------------------------------------");

         for ( i = 0; i < MAXPTS; ++i )
            {
            et2utc_c ( times[i], "ISOC", 0, WORD_SIZE, utctim );
            //printf ( "  %.20s:\t%s\t%s\t%15.8f\n", utctim, mas_to_hms(rad_to_mas(ra[i]), " ", 2).toAscii().data(),  mas_to_damas(rad_to_mas(dec[i]), " ", 3).toAscii().data(), range[i]/AUKM);

            mrec.r = ra[i];// + dRa;
            mrec.d = dec[i];// + dDec;

            tstr = QString(utctim);
            jdUTC = mjd2jd(getMJDfromStrT(tstr));
            mrec.eJD = jdUTC;
            mrec.num = 1;
            mrec.head->set_Snum(1);
            //mCat.record->getNumStr(mrec.head->Snum);
            //strcpy(, mCat.record->getNumStr(>number);
            mrec.tail->set_numOfObs("500");
            mrec.toString(astr);

            mpcStm << astr << "\n";
            printf ( "  %.20s:\t%15.8f\t%15.8f\t%15.8f\t%15.8f\n", utctim, x[i]/AUKM,  y[i]/AUKM, z[i]/AUKM, range[i]/AUKM);
            //printf ( "  %.20s:\t%15.8f\t%15.8f\t%15.8f\t%15.8f\n", utctim, x[i],  y[i], z[i], range[i]);
            }
/*
         puts( " " );
         prompt_c ( "Continue? (Enter Y or N): ", WORD_SIZE, answer );
         puts( " " );
         puts( " " );

         /*
         Perform a logical test to see if the user wants to
         continue.
         /
         if ( eqstr_c( "N", answer) )
            {
            cont = SPICEFALSE;
            }
*/
//         }
//      while ( cont == SPICETRUE );
         mpcFile.close();
    
    return 0;//a.exec();
}
