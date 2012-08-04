#pragma once
#ifdef SERIAL
#include "CapsuleBase.h"

/**
* This class causes Moody to run in serial mode
*/
class CapsuleSerial: public CapsuleBase {
private:
/**
* RK4 - serial mode
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

	for (i=0;i<this->environmentSetSize;i++) {
		this->environmentSet[i].moveParticle(distance/2);
	}

	for (i=0;i<this->environmentSetSize;i++) {
		this->environmentSet[i].setExternalReferenceVector();
	}

	// find F once again for all allowed pairwise interactions

	for (i=0;i<this->environmentSetSize;i++) {
		this->environmentSet[i].interactWithAll(this->environmentSet,this->environmentSetSize,this->gravitationalConstant,i);
		this->environmentSet[i].accountForInertia();
		this->environmentSet[i].addCurrentVectorToMemory();
	}

	//Move from start back to mipoint using the forces from the first midpoint

	for (i=0;i<this->environmentSetSize;i++) {
		this->environmentSet[i].revertToFirstStoredVector(Advisor::OMIT_FORCES);
		this->environmentSet[i].moveParticle(distance/2);
		this->environmentSet[i].setExternalReferenceVector();
	}

	for (i=0;i<this->environmentSetSize;i++) {
		this->environmentSet[i].interactWithAll(this->environmentSet,this->environmentSetSize,this->gravitationalConstant,i);
		this->environmentSet[i].accountForInertia();
		this->environmentSet[i].addCurrentVectorToMemory();
	}

	//-----
	//  Move all the way across the interval, using the forces from the third midpoint
	//-----

	for (i=0;i<this->environmentSetSize;i++) {
		this->environmentSet[i].revertToFirstStoredVector(Advisor::OMIT_FORCES);
		this->environmentSet[i].moveParticle(distance);
		this->environmentSet[i].setExternalReferenceVector();
	}

	for (i=0;i<this->environmentSetSize;i++) {
		this->environmentSet[i].interactWithAll(this->environmentSet,this->environmentSetSize,this->gravitationalConstant,i);
		this->environmentSet[i].accountForInertia();
		this->environmentSet[i].addCurrentVectorToMemory();
	}

	//-----
	//  Finish the step
	//-----

	for (i=0;i<this->environmentSetSize;i++) {
		this->environmentSet[i].revertToFirstStoredVector(Advisor::OMIT_FORCES);
		this->environmentSet[i].finaliseStep(distance);
		this->environmentSet[i].resetMemory();
	}
}


/**
*  MidPoint Integration - serial mode
*/

void moveParticlesMidPoint(int distance){
	// set the external reference vars for other particles to read
	int i;

	for (i=0;i<this->environmentSetSize;i++) {
		this->environmentSet[i].setExternalReferenceVector();
	}
	// find f

	for (i=0;i<this->environmentSetSize;i++) {
		this->environmentSet[i].interactWithAll(this->environmentSet,this->getEnvironmentSetSize(),this->gravitationalConstant,i);
		this->environmentSet[i].accountForInertia();
		this->environmentSet[i].addCurrentVectorToMemory();
	}
	// move to the midpoint

	for (i=0;i<this->environmentSetSize;i++) {
		this->environmentSet[i].revertToFirstStoredVector(Advisor::OMIT_FORCES);
		this->environmentSet[i].moveParticle(distance/2);
		this->environmentSet[i].setExternalReferenceVector();
	}
	// update external vectors

	for (i=0;i<this->environmentSetSize;i++) {
		this->environmentSet[i].setExternalReferenceVector();
	}
	// find midpoint f

	for (i=0;i<this->environmentSetSize;i++) {
		this->environmentSet[i].interactWithAll(this->environmentSet,this->getEnvironmentSetSize(),this->gravitationalConstant,i);
		this->environmentSet[i].accountForInertia();
		this->environmentSet[i].addCurrentVectorToMemory();
	}
	// move back to the start and traverse entire step with the midpoint f.

	for (i=0;i<this->environmentSetSize;i++) {
		this->environmentSet[i].revertToFirstStoredVector(Advisor::OMIT_FORCES);
		this->environmentSet[i].moveParticle(distance);
		this->environmentSet[i].setExternalReferenceVector();
		this->environmentSet[i].resetMemory();
	}
}



/**
* Symplectic second order Integration  - serial mode
*/
void moveParticlesSym(int distance){
	int i;
	// we still have 1/4 a step to take using the forces calculated on the last step, so lets do that thing..
	for (i=0;i<this->environmentSetSize;i++) {
		this->environmentSet[i].kick_noforceCalc(distance/4);
	}

	//drift half the step
	for (i=0;i<this->environmentSetSize;i++) {
		this->environmentSet[i].drift((distance/2));
	}
	// set the external reference vars for other particles to read
	for (i=0;i<this->environmentSetSize;i++) {
		this->environmentSet[i].setExternalReferenceVector();
	}

	//kick for half a step
	for (i=0;i<this->environmentSetSize;i++) {
		this->environmentSet[i].kick(this->environmentSet,this->getEnvironmentSetSize(),this->gravitationalConstant,i,(distance/2));
	}

	//drift half the step
	for (i=0;i<this->environmentSetSize;i++) {
		this->environmentSet[i].drift((distance/2));
	}

	// set the external reference vars for other particles to read
	for (i=0;i<this->environmentSetSize;i++) {
		this->environmentSet[i].setExternalReferenceVector();
	}

	//kick for the last 1/4 step
	for (i=0;i<this->environmentSetSize;i++) {
		this->environmentSet[i].kick(this->environmentSet,this->getEnvironmentSetSize(),this->gravitationalConstant,i,(distance/4));
	}
}

public:
	/**
	* Iterator for the serial version of Symplectic second order Integration
	*/
	void iterateSym(){
		this->moveParticlesSym(this->stepSize);
	}

	/**
	* Iterator for the serial version of Midpoint Integration
	*/
	void iterateMidPoint(){
		this->moveParticlesMidPoint(this->stepSize);
	}

	/**
	* Iterator for the serial version of RK4
	*/
	void iterateRK4(){
		this->moveParticlesRK4(this->stepSize);
	}

};
#endif