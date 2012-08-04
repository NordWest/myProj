#pragma once
/**
* This class is availble for any extentions or changes you want to make to the CapsuleBase class.
*/

#ifdef OMPLIB
#include "capsuleBase/CapsuleOpenMP.h"
class Capsule: public CapsuleOpenMP {
#endif

#ifdef TBBLIB
#include "capsuleBase/CapsuleTBB.h"
	class Capsule: public CapsuleTBB {
#endif

#ifdef SERIAL
#include "capsuleBase/CapsuleSerial.h"
		class Capsule: public CapsuleSerial {
#endif

		public:
			Capsule(void) {
			  
			}
			~Capsule(void) {
			  
			}

			Capsule(const Capsule& cb) {
				this->cfgFolder = cb.cfgFolder;
				this->resultFolder = cb.resultFolder;
				this->projectFolder = cb.projectFolder;
				this->particleFolder = cb.particleFolder;
				this->stepSize = cb.stepSize;
				this->numStates = cb.numStates;
				this->numSteps = cb.numSteps;
				this->gravitationalConstant = cb.gravitationalConstant;
				this->MopName = cb.MopName;
				this->version = cb.version;
				this->cooler = cb.cooler;
				if (this->environmentSetSize!=0) {
					this->environmentSetSize = 0;
					delete this->environmentSet;
				}
				for (int x(0);x<cb.environmentSetSize;x++) {
					this->addToEnvironmentSet(cb.environmentSet[x]);
				}
				if (this->populationSetSize!=0) {
					this->populationSetSize = 0;
					delete this->populationSet;
				}
				for (int y(0);y<cb.populationSetSize;y++) {
					this->addToPopulationSet(cb.environmentSet[y]);
				}
			}

			void fillFromExistingCapsule (const Capsule & other)
			{
				if (this != &other) // protect against invalid self-assignment
				{
					this->cfgFolder = other.cfgFolder;
					this->resultFolder = other.resultFolder;
					this->projectFolder = other.projectFolder;
					this->particleFolder = other.particleFolder;
					this->stepSize = other.stepSize;
					this->numStates = other.numStates;
					this->numSteps = other.numSteps;
					this->gravitationalConstant = other.gravitationalConstant;
					this->MopName = other.MopName;
					this->version = other.version;
					this->cooler = other.cooler;
					if (this->environmentSetSize!=0) {
						this->environmentSetSize = 0;
						delete this->environmentSet;
					}
					for (int x(0);x<other.environmentSetSize;x++) {
						this->addToEnvironmentSet(other.environmentSet[x]);
					}
					if (this->populationSetSize!=0) {
						this->populationSetSize = 0;
						delete this->populationSet;
					}
					for (int y(0);y<other.populationSetSize;y++) {
						this->addToPopulationSet(other.environmentSet[y]);
					}
				}
			}
			
			//###########################
			// Your code code goes here #
			//###########################
                        			
			
			
		};
