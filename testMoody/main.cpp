#include <QtCore/QCoreApplication>

#define OMPLIB

#include <iostream>
#include <iomanip>

#include "moody/moody.h"
int main(int argc, char *argv[]) {
    if (argc!=3) {
        std::cout <<std::endl << "> Error: Incorrect number of parameters." << std::endl;
        std::cout  << "> Usage: moody <integrator> <projectfolder>" << std::endl;
        std::cout  << ">      : Where the integrator is either rk4 (Runge-kutta), mp (Midpoint), or sym (2nd order symplectic)," << std::endl;
        std::cout  << ">      : and projectfolder is the location of the project folder itself," << std::endl;
        std::cout  << ">      : i.e moody sym I:\\workspace\\baseProject" << std::endl;
        exit(0);
    }
    std::cout <<  "> Moody example - Dr Carey Pridgeon - 2011" << std::endl;
#ifdef OMPLIB
    std::cout << "> OpenMP API In Use" << std::endl;
#endif
#ifdef SERIAL
    std::cout << "> Serial Mode In Use" << std::endl;
#endif
#ifdef TBBLIB
    std::cout << "> Intel Thread Building Blocks Template Library In Use" << std::endl;
    tbb::task_scheduler_init automatic;
#endif
    Capsule * experiment = new Capsule;
    experiment->loadProject(argv[2]);


    std::cout  << "> Succesfully Loaded: " << argv[2]<< std::endl;
    std::cout  << "> Project contains " <<experiment->getEnvironmentSetSize()<< " particles. "<<std::endl;
    std::cout  << "> Number of states specified : " <<experiment->getNumStates()<<std::endl;
    std::cout  << "> Number of steps per state  : " <<experiment->getNumSteps()<<std::endl;
    std::cout  << "> Stepsize                   : " <<experiment->getStepSize()<<std::endl;
    std::cout  << "> G                          : " <<experiment->getGravitationalConstant()<<std::endl;
    // set up the output filename we are going to use
    experiment->mop = new MopFile;
    experiment->mop->setFilename(experiment->getResultFolder(),experiment->getMopName());
    experiment->mop->resetFile();

    double pct;
    std::string integrator(argv[1]);
    if (integrator=="rk4") {
        std::cout  << "> Integrator Selected: 4th Order Runge-Kutta." << std::endl;
        std::cout  << "> Starting.." << std::endl;
        //give the particles five memory blocks (can remember 5 previous states)
        experiment->createParticleMemory(4);
        //start the timer
        experiment->timer.start();
        for (int i = 1; i<experiment->getNumStates()+1;i++) {
            for (int j=0;j<experiment->getNumSteps();j++) {
                experiment->iterateRK4();
            }

            pct = (float) ((float)i/experiment->getNumStates())*100;
            std::cout <<"> State "<<i<<" of "<<experiment->getNumStates()<< " Completed - " << std::setprecision(3) <<pct<<"%"<<std::endl;
            experiment->writeCurrentStateToMopFile();


        }
        experiment->timer.stop();
    } else if (integrator =="mp") {
        std::cout  << "> Integrator Selected: 2nd Order Midpoint." << std::endl;
        std::cout  << "> Starting.." << std::endl;
        //give the particles two memory blocks
        experiment->createParticleMemory(2);
        //start the timer
        experiment->timer.start();
        for (int i = 1; i<experiment->getNumStates()+1;i++) {
            for (int j=0;j<experiment->getNumSteps();j++) {
                experiment->iterateMidPoint();
            }

            pct = (float) ((float)i/experiment->getNumStates())*100;
            std::cout <<"> State "<<i<<" of "<<experiment->getNumStates()<< " Completed - " << std::setprecision(3) <<pct<<"%"<<std::endl;
            experiment->writeCurrentStateToMopFile();
        }
        experiment->timer.stop();
    } else if (integrator=="sym") {
        std::cout  << "> Integrator Selected: 2nd Order Symplectic." << std::endl;
        std::cout  << "> Starting.." << std::endl;
        //start the timer
        experiment->timer.start();

        for (int i = 1; i<experiment->getNumStates()+1;i++) {
            experiment->writeCurrentStateToMopFile();
            for (int j=0;j<experiment->getNumSteps();j++) {
                experiment->iterateSym();
            }

            pct = (float) ((float)i/experiment->getNumStates())*100;
            std::cout <<"> State "<<i<<" of "<<experiment->getNumStates()<< " Completed - " << std::setprecision(3) <<pct<<"%"<<std::endl;
        }
        experiment->timer.stop();
    }
    delete experiment;
    std::cout <<"> fin "<<std::endl;

    return 0;
}
