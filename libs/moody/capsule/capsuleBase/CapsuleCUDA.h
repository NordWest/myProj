#pragma once
#ifdef CUDALIB
#include "CapsuleBase.h"
#include <cuda.h>

/**
* This class adds CUDA support to Moody
*/
class CapsuleCUDA: public CapsuleBase {
private:
	/**
    * parallell RK4 using CUDA
	*/
	void moveParticlesRK4(const int distance){
		int i;

		// get and store the starting state
		for (i=0;i<this->environmentSetSize;i++) {
			this->environmentSet[i].setExternalReferenceVector();
		}
		for (i=0;i<this->environmentSetSize;i++) {
			this->environmentSet[i].interactWithAll(this->environmentSet,this->environmentSetSize,this->gravitationalConstant,i);
			this->environmentSet[i].accountForInertia();
			this->environmentSet[i].addCurrentVectorToMemory();
		}
		//move the particles to the first midpoint

		#pragma omp parallel for
		for (i=0;i<this->environmentSetSize;i++) {
			this->environmentSet[i].moveParticle(distance/2);
		}

		#pragma omp parallel for
		for (i=0;i<this->environmentSetSize;i++) {
			this->environmentSet[i].setExternalReferenceVector();
		}

		// find F once again for all allowed pairwise interactions
		#pragma omp parallel for
		for (i=0;i<this->environmentSetSize;i++) {
			this->environmentSet[i].interactWithAll(this->environmentSet,this->environmentSetSize,this->gravitationalConstant,i);
			this->environmentSet[i].accountForInertia();
			this->environmentSet[i].addCurrentVectorToMemory();
		}

		//Move from start back to mipoint using the forces from the first midpoint
		#pragma omp parallel for
		for (i=0;i<this->environmentSetSize;i++) {
			this->environmentSet[i].revertToFirstStoredVector(Advisor::OMIT_FORCES);
			this->environmentSet[i].moveParticle(distance/2);
			this->environmentSet[i].setExternalReferenceVector();
		}
		#pragma omp parallel for
		for (i=0;i<this->environmentSetSize;i++) {
			this->environmentSet[i].interactWithAll(this->environmentSet,this->environmentSetSize,this->gravitationalConstant,i);
			this->environmentSet[i].accountForInertia();
			this->environmentSet[i].addCurrentVectorToMemory();
		}

		//-----
		//  Move all the way across the interval, using the forces from the third midpoint
		//-----
		#pragma omp parallel for
		for (i=0;i<this->environmentSetSize;i++) {
			this->environmentSet[i].revertToFirstStoredVector(Advisor::OMIT_FORCES);
			this->environmentSet[i].moveParticle(distance);
			this->environmentSet[i].setExternalReferenceVector();
		}
		#pragma omp parallel for
		for (i=0;i<this->environmentSetSize;i++) {
			this->environmentSet[i].interactWithAll(this->environmentSet,this->environmentSetSize,this->gravitationalConstant,i);
			this->environmentSet[i].accountForInertia();
			this->environmentSet[i].addCurrentVectorToMemory();
		}
		//-----
		//  Finish the step
		//-----
		#pragma omp parallel for
		for (i=0;i<this->environmentSetSize;i++) {
			this->environmentSet[i].revertToFirstStoredVector(Advisor::OMIT_FORCES);
			this->environmentSet[i].finaliseStep(distance);
			this->environmentSet[i].resetMemory();
		}
	}



	 /**
	* parallel MidPoint Integration using OpenMP
	*/

	void moveParticlesMidPoint(int distance){
		// set the external reference vars for other particles to read
		int i;
		#pragma omp parallel for
		for (i=0;i<this->environmentSetSize;i++) {
			this->environmentSet[i].setExternalReferenceVector();
		}
		// find f
		#pragma omp parallel for
		for (i=0;i<this->environmentSetSize;i++) {
			this->environmentSet[i].interactWithAll(this->environmentSet,this->getEnvironmentSetSize(),this->gravitationalConstant,i);
			this->environmentSet[i].accountForInertia();
			this->environmentSet[i].addCurrentVectorToMemory();
		}
		// move to the midpoint
		#pragma omp parallel for
		for (i=0;i<this->environmentSetSize;i++) {
			this->environmentSet[i].revertToFirstStoredVector(Advisor::OMIT_FORCES);
			this->environmentSet[i].moveParticle(distance/2);
			this->environmentSet[i].setExternalReferenceVector();
		}
		// update external vectors
		#pragma omp parallel for
		for (i=0;i<this->environmentSetSize;i++) {
			this->environmentSet[i].setExternalReferenceVector();
		}
		// find midpoint f
		#pragma omp parallel for
		for (i=0;i<this->environmentSetSize;i++) {
			this->environmentSet[i].interactWithAll(this->environmentSet,this->getEnvironmentSetSize(),this->gravitationalConstant,i);
			this->environmentSet[i].accountForInertia();
			this->environmentSet[i].addCurrentVectorToMemory();
		}
		// move back to the start and traverse entire step with the midpoint f.
		#pragma omp parallel for
		for (i=0;i<this->environmentSetSize;i++) {
			this->environmentSet[i].revertToFirstStoredVector(Advisor::OMIT_FORCES);
			this->environmentSet[i].moveParticle(distance);
			this->environmentSet[i].setExternalReferenceVector();
			this->environmentSet[i].resetMemory();
		}
	}


	/**
	* parallell Symplectic second order Integration using OpenMP
	*/
	void moveParticlesSym(int distance){
		int i;
		// we still have 1/4 a step to take using the forces calculated on the last step, so lets do that thing..
		#pragma omp parallel for
		for (i=0;i<this->environmentSetSize;i++) {
			this->environmentSet[i].kick_noforceCalc(distance/4);
		}

		//drift half the step
		#pragma omp parallel for
		for (i=0;i<this->environmentSetSize;i++) {
			this->environmentSet[i].drift((distance/2));
		}
		// set the external reference vars for other particles to read
		#pragma omp parallel for
		for (i=0;i<this->environmentSetSize;i++) {
			this->environmentSet[i].setExternalReferenceVector();
		}

		//kick for half a step
		#pragma omp parallel for
		for (i=0;i<this->environmentSetSize;i++) {
			this->environmentSet[i].kick(this->environmentSet,this->getEnvironmentSetSize(),this->gravitationalConstant,i,(distance/2));
		}

		//drift half the step
		#pragma omp parallel for
		for (i=0;i<this->environmentSetSize;i++) {
			this->environmentSet[i].drift((distance/2));
		}

		// set the external reference vars for other particles to read
		#pragma omp parallel for
		for (i=0;i<this->environmentSetSize;i++) {
			this->environmentSet[i].setExternalReferenceVector();
		}

		//kick for the last 1/4 step
		#pragma omp parallel for
		for (i=0;i<this->environmentSetSize;i++) {
			this->environmentSet[i].kick(this->environmentSet,this->getEnvironmentSetSize(),this->gravitationalConstant,i,(distance/4));
		}
	}

public:
	/**
	* Iterator for the OpenMP version of Symplectic second order Integration
	*/
	void iterateSym(){
		this->moveParticlesSym(this->stepSize);
	}

	/**
	* Iterator for the OpenMP version of Midpoint Integration
	*/
	void iterateMidPoint(){
		this->moveParticlesMidPoint(this->stepSize);
	}
	
	/**
	* Iterator for the OpenMP version of RK4
	*/
	void iterateRK4(){
		this->moveParticlesRK4(this->stepSize);
	}

};

/* CAPSULE_H_ */
#endif
