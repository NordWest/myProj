#pragma once
#ifdef TBBLIB
#include "CapsuleBase.h"
#include "tbb/parallel_for.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/blocked_range.h"

/**
* This class adds Intel Thread Building Blocks support to Moody
*/
class CapsuleTBB: public CapsuleBase {
private:
#ifdef TBBLIB


	/**
	* parallell RK4 using Intel Thread Building Blocks
	*/
	void moveParticlesRK4(const int distance){
		// find and store the initial state
		tbb::parallel_for(tbb::blocked_range<size_t>(0,(size_t)this->environmentSetSize), UpdateExternalReferenceVectors(this->environmentSet));
		tbb::parallel_for(tbb::blocked_range<size_t>(0,(size_t)this->environmentSetSize), FindForces(this->environmentSet,this->environmentSetSize, this->gravitationalConstant));

		// move the particles to the first midpoint
		tbb::parallel_for(tbb::blocked_range<size_t>(0,(size_t)this->environmentSetSize), MoveToMidpoint(this->environmentSet,this->environmentSetSize,distance, this->gravitationalConstant));
		tbb::parallel_for(tbb::blocked_range<size_t>(0,(size_t)this->environmentSetSize), UpdateExternalReferenceVectors(this->environmentSet));

		// find F once again for all allowed pairwise interactions
		tbb::parallel_for(tbb::blocked_range<size_t>(0,(size_t)this->environmentSetSize), FindForces(this->environmentSet,this->environmentSetSize, this->gravitationalConstant));

		//Move from start back to mipoint using the forces from the first midpoint
		tbb::parallel_for(tbb::blocked_range<size_t>(0,(size_t)this->environmentSetSize), RevertToStartAndMoveBackToMidpointAgain(this->environmentSet,distance));

		// find F once again for all allowed pairwise interactions
		tbb::parallel_for(tbb::blocked_range<size_t>(0,(size_t)this->environmentSetSize), FindForces(this->environmentSet,this->environmentSetSize, this->gravitationalConstant));

		//-----
		// Move all the way across the interval, using the forces from the last midpoint
		//-----
		tbb::parallel_for(tbb::blocked_range<size_t>(0,(size_t)this->environmentSetSize), RevertToStartAndMoveAcrossEntireInterval(this->environmentSet,distance));
		// find F once again for all allowed pairwise interactions
		tbb::parallel_for(tbb::blocked_range<size_t>(0,(size_t)this->environmentSetSize), FindForces(this->environmentSet,this->environmentSetSize, this->gravitationalConstant));
		// return to the starting point once more
		tbb::parallel_for(tbb::blocked_range<size_t>(0,(size_t)this->environmentSetSize), RevertToStart(this->environmentSet));

		// And finialise RK4, calculating the final position for all particles
		tbb::parallel_for(tbb::blocked_range<size_t>(0,(size_t)this->environmentSetSize), FinaliseRK4(this->environmentSet,distance));
	}


	/**
	* parallell MidPoint integration using Intel Thread Building Blocks
	*/
	void moveParticlesMidPoint(const int distance){
		// move a copy of each particles starting state into the
		// members that other particles will reference.
		tbb::parallel_for(tbb::blocked_range<size_t>(0,(size_t)this->environmentSetSize), UpdateExternalReferenceVectors(this->environmentSet));
		// find F initially for all allowed pairwise interactions, store the vector and move the particle
		tbb::parallel_for(tbb::blocked_range<size_t>(0,(size_t)this->environmentSetSize), MoveToMidpoint(this->environmentSet,this->environmentSetSize,distance,this->gravitationalConstant));
		tbb::parallel_for(tbb::blocked_range<size_t>(0,(size_t)this->environmentSetSize), UpdateExternalReferenceVectors(this->environmentSet));
		// find F once again for all allowed pairwise interactions
		tbb::parallel_for(tbb::blocked_range<size_t>(0,(size_t)this->environmentSetSize), FindForces(this->environmentSet,this->environmentSetSize, this->gravitationalConstant));
		// Move across the entire interval using the force found at the midpoint.
		tbb::parallel_for(tbb::blocked_range<size_t>(0,(size_t)this->environmentSetSize), RevertToStartAndMoveAcrossEntireIntervalForMidpoint(this->environmentSet,distance));
	}


	/**
	* Symplectic Integration
	*/

	/**
	* parallell Symplectic Second Order integration using Intel Thread Building Blocks
	*/
	void moveParticlesSym(const int distance){
		// kick the particles for 1/4 of a steps step worth of changes in v, using the v calculated in the last step
		tbb::parallel_for(tbb::blocked_range<size_t>(0,(size_t)this->environmentSetSize), kickWithoutForceCalculation(this->environmentSet,(distance/4)));

		//before calculating v, move the particles to the half way mark
		tbb::parallel_for(tbb::blocked_range<size_t>(0,(size_t)this->environmentSetSize), drift(this->environmentSet,(distance/2)));

		// move a copy of each particles state into the
		// members that other particles will reference.
		tbb::parallel_for(tbb::blocked_range<size_t>(0,(size_t)this->environmentSetSize), UpdateExternalReferenceVectors(this->environmentSet));

		// kick the particles for  half a steps step worth of changes in v
		tbb::parallel_for(tbb::blocked_range<size_t>(0,(size_t)this->environmentSetSize), kick(this->environmentSet,this->environmentSetSize,(distance/2), this->gravitationalConstant));

		//advance the particles to the end of the step using the current forces
		tbb::parallel_for(tbb::blocked_range<size_t>(0,(size_t)this->environmentSetSize), drift(this->environmentSet,(distance/2)));

		//update external reference positions
		tbb::parallel_for(tbb::blocked_range<size_t>(0,(size_t)this->environmentSetSize), UpdateExternalReferenceVectors(this->environmentSet));

		// kick the particles for  1/4 of a steps step worth of changes in v
		tbb::parallel_for(tbb::blocked_range<size_t>(0,(size_t)this->environmentSetSize), kick(this->environmentSet,this->environmentSetSize,(distance/4), this->gravitationalConstant));
	}







	/**
	* TBB implementation of the for loop that sets the external reference vectors for all particles
	*/
	class UpdateExternalReferenceVectors {
		Particle *const lp;
	public:
		void operator()(const tbb::blocked_range<size_t>& r) const {
			Particle *localParticles = lp;
			for(size_t i=r.begin(); i!=r.end(); ++i)  {
				localParticles[i].setExternalReferenceVector();
			}
		}
		UpdateExternalReferenceVectors(Particle localParticles[]) :
		lp(localParticles)
		{}
	};

	/**
	* TBB implementation of the for loop that obtains initial forces acting on all particles and moves them to the first midpoint
	*/
	class MoveToMidpoint {
		Particle *const lp;
		int numP;
		int dist;
		double grav;
	public:
		void operator()(const tbb::blocked_range<size_t>& r) const {
			Particle *localParticles = lp;
			int numParticles = numP;
			int distance = dist;
			double G = grav;
			for(size_t i=r.begin(); i!=r.end(); ++i)  {
				localParticles[i].interactWithAll(localParticles,numParticles,G,(int)i);
				localParticles[i].accountForInertia();
				localParticles[i].moveParticle(distance/2);
			}
		}
		MoveToMidpoint(Particle localParticles[], int numParticles, int distance, double G) :
		lp(localParticles), numP(numParticles),dist(distance), grav(G)
		{}
	};

	/**
	* TBB implementation of the for loop that obtains initial forces acting on all particles
	*/
	class FindInitialF {
		Particle *const lp;
		int numP;
		double grav;
	public:
		void operator()(const tbb::blocked_range<size_t>& r) const {
			Particle *localParticles = lp;
			int numParticles = numP;
			double G = grav;
			for(size_t i=r.begin(); i!=r.end(); ++i)  {
				localParticles[i].interactWithAll(localParticles,numParticles,G,(int)i);
				localParticles[i].accountForInertia();
				localParticles[i].addCurrentVectorToMemory();
			}
		}
		FindInitialF(Particle localParticles[], int numParticles, double G) :
		lp(localParticles), numP(numParticles),grav(G)
		{}
	};

	/**
	* TBB implementation of the for loop that finds the new F for all particles at the midpoint
	*/
	class FindForces{
		Particle *const lp;
		int numP;
		double grav;
	public:
		void operator()(const tbb::blocked_range<size_t>& r) const {
			Particle *localParticles = lp;
			int numParticles = numP;
			double G = grav;
			for(size_t i=r.begin(); i!=r.end(); ++i)  {
				localParticles[i].interactWithAll(localParticles,numParticles,G,(int)i);
				localParticles[i].accountForInertia();
				localParticles[i].addCurrentVectorToMemory();
			}
		}
		FindForces(Particle localParticles[], int numParticles, double G) :
		lp(localParticles), numP(numParticles), grav(G)
		{}
	};

	/**
	* TBB implementation of the for loop that moves all particles back to the start of a step from the midpoint
	*/
	class RevertToStart{
		Particle *const lp;
		int numP;
	public:
		void operator()(const tbb::blocked_range<size_t>& r) const {
			Particle *localParticles = lp;
			for(size_t i=r.begin(); i!=r.end(); ++i)  {
				localParticles[i].revertToFirstStoredVector(Advisor::OMIT_FORCES);
				localParticles[i].setExternalReferenceVector();
			}
		}
		RevertToStart(Particle localParticles[]) :
		lp(localParticles)
		{}
	};

	/**
	* TBB implementation of the for loop that moves all particles back to the start of a step from the midpoint
	* and moves them back to the midpoint using the forces previously found to be acting on them at the midpoint
	*/
	class RevertToStartAndMoveBackToMidpointAgain{
		Particle *const lp;
		int numP;
		int dist;
	public:
		void operator()(const tbb::blocked_range<size_t>& r) const {
			Particle *localParticles = lp;
			int distance = dist;
			for(size_t i=r.begin(); i!=r.end(); ++i)  {
				localParticles[i].revertToFirstStoredVector(Advisor::OMIT_FORCES);
				localParticles[i].moveParticle(distance/2);
				localParticles[i].setExternalReferenceVector();
			}
		}
		RevertToStartAndMoveBackToMidpointAgain(Particle localParticles[], int distance) :
		lp(localParticles), dist(distance)
		{}
	};

	/**
	* TBB implementation of the for loop that moves all particles back to the start of a step from the midpoint
	* and moves them across the entire intervalnusing the forces previously found to be acting on them at the midpoint
	*/
	class RevertToStartAndMoveAcrossEntireInterval{
		Particle *const lp;
		int dist;
	public:
		void operator()(const tbb::blocked_range<size_t>& r) const {
			Particle *localParticles = lp;
			int distance = dist;
			for(size_t i=r.begin(); i!=r.end(); ++i)  {
				localParticles[i].revertToFirstStoredVector(Advisor::OMIT_FORCES);
				localParticles[i].moveParticle(distance);
				localParticles[i].setExternalReferenceVector();
			}
		}
		RevertToStartAndMoveAcrossEntireInterval(Particle localParticles[], int distance) :
		lp(localParticles), dist(distance)
		{}
	};

	/**
	* TBB implementation of the for loop that finds the final position of all particles
	* at the end of Rk4
	*/
	class FinaliseRK4{
		Particle *const lp;
		int dist;
	public:
		void operator()(const tbb::blocked_range<size_t>& r) const {
			Particle *localParticles = lp;
			int distance = dist;
			for(size_t i=r.begin(); i!=r.end(); ++i)  {
				localParticles[i].finaliseStep(distance);
				localParticles[i].resetMemory();
			}
		}
		FinaliseRK4(Particle localParticles[], int distance) :
		lp(localParticles),dist(distance)
		{}
	};

	/**
	* TBB implementation of the for loop that moves all particles across the step
	* at the end of MidPoint integration
	*/
	class RevertToStartAndMoveAcrossEntireIntervalForMidpoint{
		Particle *const lp;
		int dist;
	public:
		void operator()(const tbb::blocked_range<size_t>& r) const {
			Particle *localParticles = lp;
			int distance = dist;
			for(size_t i=r.begin(); i!=r.end(); ++i)  {
				localParticles[i].revertToFirstStoredVector(Advisor::OMIT_FORCES);
				localParticles[i].moveParticle(distance);
				localParticles[i].resetMemory();
			}
		}
		RevertToStartAndMoveAcrossEntireIntervalForMidpoint(Particle localParticles[], int distance) :
		lp(localParticles), dist(distance)
		{}
	};

	/**
	* TBB implementation of drift-kick
	*/
	class drift {
		Particle *const lp;
		int dist;
	public:
		void operator()(const tbb::blocked_range<size_t>& r) const {
			Particle *localParticles = lp;
			int distance = dist;
			for(size_t i=r.begin(); i!=r.end(); ++i)  {
				localParticles[i].drift(distance);
			}
		}
		drift(Particle localParticles[], int distance) :
		lp(localParticles),dist(distance)
		{}
	};

	/**
	* TBB implementation of kick (for leapfrog)
	*/
	class kick {
		Particle *const lp;
		int numP;
		int dist;
		double grav;
	public:
		void operator()(const tbb::blocked_range<size_t>& r) const {
			Particle *localParticles = lp;
			int numParticles = numP;
			int distance = dist;
			double G = grav;
			for(size_t i=r.begin(); i!=r.end(); ++i)  {
				localParticles[i].kick(localParticles,numParticles,G,(int)i,distance);
			}
		}
		kick(Particle localParticles[], int numParticles, int distance, double G) :
		lp(localParticles), numP(numParticles),dist(distance), grav(G)
		{}
	};

	/**
	* TBB implementation of kick (for leapfrog) that uses the forces calculated on the previous step
	*/
	class kickWithoutForceCalculation {
		Particle *const lp;
		int dist;
	public:
		void operator()(const tbb::blocked_range<size_t>& r) const {
			Particle *localParticles = lp;
			int distance = dist;
			for(size_t i=r.begin(); i!=r.end(); ++i)  {
				localParticles[i].kick_noforceCalc(distance);
			}
		}
		kickWithoutForceCalculation(Particle localParticles[], int distance) :
		lp(localParticles),dist(distance)
		{}
	};



public:
	CapsuleTBB() {
	}

	~CapsuleTBB() {
	}

	/**
	* Iterator for the Thread Building Blocks using Runge-Kutta Fourth Order Integrator
	*/
	void iterateRK4(){
		this->moveParticlesRK4(this->stepSize);
	}
	
	/**
	* Iterator for the Thread Building Blocks using MidPoint Second Order Integrator
	*/
	void iterateMidPoint(){
		this->moveParticlesMidPoint(this->stepSize);
	}


	/**
	* Iterator for the Thread Building Blocks using Symplectic (leapfrog) Second Order Integrator
	*/
	void iterateSym(){
		this->moveParticlesSym(this->stepSize);
	}
	
	

#endif
};

/* CAPSULE_H_ */
#endif