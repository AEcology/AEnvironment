//###########################################################################################################//
//
//  Copyright(c) 2012, Algorithmic Ecology / Anthony Rodriguez (anth.jrod@gmail.com)
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without modification, are permitted provided 
//  that the following conditions are met :
//
//  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the 
//     following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
//     the following disclaimer in the documentation and / or other materials provided with the distribution.
//
//  3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or 
//     promote products derived from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
//  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
//  PARTICULAR PURPOSE ARE DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR 
//  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED 
//  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
//  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
//  POSSIBILITY OF SUCH DAMAGE.
//
//############################################################################################################//

//############################################################################################################//
//
//    evolution.cpp
//    Genetic Algorithm Core Functionality
//
//############################################################################################################//


#include "evolution.h"

//Minimum euclidian distance from black for a new color to be accepted
#define MINBRIGHTNESS 0.2
//Minbrightness euclidian distance squared
#define MINBRIGHTNESSSQUARED MINBRIGHTNESS*MINBRIGHTNESS

//Evolution constraints: shared by main.cpp
//if false, prevents that region of brain from being modified
extern bool ALLOWMUTATIONS_VISION;
extern bool ALLOWMUTATIONS_FOCUS;
extern bool ALLOWMUTATIONS_PURSUE; //Allow mutations for pursue region
extern bool ALLOWMUTATIONS_FLEE;   //Allow for flee region
extern bool ALLOWMUTATIONS_SOCIAL; //etc...
extern bool ALLOWMUTATIONS_NULL;

//More robust and simple implementation of random mutation
void MutateRefactor(Genome* newGenome, int X){
	//Precondition: newGenome->size > 3
	int r;
	int q;
	double colorFactor;
	//int i = 0;
	double tempVal;
	bool canMutate = true;
	//Note: Color vector is always first 3 genes
	//Non-negative mutations:
	/*for (i = 0; i < WEIGHTS_STARTING_INDEX; ++i){
		r = rand() % 101;
		if (r < X){
			r = rand();
			tempVal = newGenome->genes.at(i);
			newGenome->genes.at(i) = ((double)(r)) / RAND_MAX;
			colorFactor = 0;
			if (i < 3){
				for (int j = 0; j < 3; ++j)
					colorFactor = colorFactor + newGenome->genes.at(j)*newGenome->genes.at(j);
				colorFactor = sqrt(colorFactor);
				if (colorFactor < MINBRIGHTNESS) //Reset if new color is too dark to see
					newGenome->genes.at(i) = tempVal;
			}
			if (i == DIET_INDEX){
				r = rand() % 101;
				if (r < 50)
					newGenome->genes.at(i) = -1 * newGenome->genes.at(i);
			}
		}
	}*/
	double* colorRegionEnd = newGenome->genes + 3;
	double* genePtr = newGenome->genes;
	for (genePtr; genePtr != newGenome->visionRegionPtr; ++genePtr){
		r = rand() % 101;
		if (r < X){
			r = rand();
			tempVal = *genePtr;
			*genePtr = ((double)(r)) / RAND_MAX;
			if (genePtr < colorRegionEnd){
				colorFactor = newGenome->genes[0] * newGenome->genes[0] + newGenome->genes[1] * newGenome->genes[1] + newGenome->genes[2] * newGenome->genes[2];
				if (colorFactor < MINBRIGHTNESSSQUARED) //Reset if new color is too dark to see
					*genePtr = tempVal;
			}
			if (genePtr == newGenome->genes+DIET_INDEX){
				r = rand() % 101;
				if (r < 50)
					*genePtr = -1 * (*genePtr);
			}
		}
	}
	/*for (i = i; i < newGenome->genes.size(); ++i){
		//determine if current index can be mutated,
		//if not, skip that region
		if (!ALLOWMUTATIONS_VISION && i >= newGenome->visionNet_startIdx && i < newGenome->focusRegion_startIdx)
			i = newGenome->focusRegion_startIdx;
		if (!ALLOWMUTATIONS_FOCUS && i >= newGenome->focusRegion_startIdx && i < newGenome->pursuitRegion_startIdx)
			i = newGenome->pursuitRegion_startIdx;
		if (!ALLOWMUTATIONS_PURSUE && i >= newGenome->pursuitRegion_startIdx && i < newGenome->fleeRegion_startIdx)
			i = newGenome->fleeRegion_startIdx;
		if (!ALLOWMUTATIONS_FLEE && i >= newGenome->fleeRegion_startIdx && i < newGenome->socialRegion_startIdx)
			i = newGenome->socialRegion_startIdx;
		if (!ALLOWMUTATIONS_SOCIAL && i >= newGenome->socialRegion_startIdx && i < newGenome->nullRegion_startIdx)
			i = newGenome->nullRegion_startIdx;
		r = rand() % 101;
		if (r <= X){
			r = 5*rand();
			q = rand() % 101;
			if (q<50) //positive
				newGenome->genes.at(i) = static_cast<double>(r) / RAND_MAX;
			else //negative
				newGenome->genes.at(i) = -1*(static_cast<double>(r) / RAND_MAX);
		}
	}*/
	for (genePtr; genePtr != newGenome->endPtr; ++genePtr){
		//determine if current index can be mutated,
		//if not, skip that region
		/*if (!ALLOWMUTATIONS_VISION && i >= newGenome->visionNet_startIdx && i < newGenome->focusRegion_startIdx)
			i = newGenome->focusRegion_startIdx;
		if (!ALLOWMUTATIONS_FOCUS && i >= newGenome->focusRegion_startIdx && i < newGenome->pursuitRegion_startIdx)
			i = newGenome->pursuitRegion_startIdx;
		if (!ALLOWMUTATIONS_PURSUE && i >= newGenome->pursuitRegion_startIdx && i < newGenome->fleeRegion_startIdx)
			i = newGenome->fleeRegion_startIdx;
		if (!ALLOWMUTATIONS_FLEE && i >= newGenome->fleeRegion_startIdx && i < newGenome->socialRegion_startIdx)
			i = newGenome->socialRegion_startIdx;
		if (!ALLOWMUTATIONS_SOCIAL && i >= newGenome->socialRegion_startIdx && i < newGenome->nullRegion_startIdx)
			i = newGenome->nullRegion_startIdx;
			*/
		if (!ALLOWMUTATIONS_VISION && genePtr >= newGenome->visionRegionPtr && genePtr < newGenome->focusRegionPtr)
			genePtr = newGenome->focusRegionPtr;
		if (!ALLOWMUTATIONS_FOCUS && genePtr >= newGenome->focusRegionPtr && genePtr < newGenome->pursuitRegionPtr)
			genePtr = newGenome->pursuitRegionPtr;
		if (!ALLOWMUTATIONS_PURSUE && genePtr >= newGenome->pursuitRegionPtr && genePtr < newGenome->fleeRegionPtr)
			genePtr = newGenome->fleeRegionPtr;
		if (!ALLOWMUTATIONS_SOCIAL && genePtr >= newGenome->fleeRegionPtr && genePtr < newGenome->socialRegionPtr)
			genePtr = newGenome->socialRegionPtr;
		if (!ALLOWMUTATIONS_SOCIAL && genePtr >= newGenome->socialRegionPtr && genePtr < newGenome->nullRegionPtr)
			genePtr = newGenome->nullRegionPtr;
		if (!ALLOWMUTATIONS_NULL && genePtr >= newGenome->nullRegionPtr)
			break;
		r = rand() % 101;
		if (r <= X){
			r = 5 * rand();
			q = rand() % 101;
			if (q<50) //positive
				*genePtr = static_cast<double>(r) / RAND_MAX;
			else //negative
				*genePtr = -1 * (static_cast<double>(r) / RAND_MAX);
		}
	}

}

//More robust and simple implementation of gradient/step mutation
void StepMutateRefactor(Genome* newGenome, int X){
	//Precondition: newGenome->size > 3
	int r;
	int q;
	double colorFactor;
	//int i = 0;
	double tempVal;
	//Non-negative
	/*for (i = 0; i < WEIGHTS_STARTING_INDEX; ++i){
		r = rand() % 101;
		if (r < X){
			r = rand();
			q = rand() % 101;
			tempVal = newGenome->genes.at(i);
			if (q<50)
				newGenome->genes.at(i) = newGenome->genes.at(i) + ((double)(r)) / RAND_MAX;
			else
				newGenome->genes.at(i) = newGenome->genes.at(i) - ((double)(r)) / RAND_MAX;
			if (newGenome->genes.at(i) < 1 && newGenome->genes.at(i)>0){
				if (i < 3){
					colorFactor = 0;
					for (int j = 0; j < 3; ++j)
						colorFactor = colorFactor + newGenome->genes.at(j)*newGenome->genes.at(j);
					colorFactor = sqrt(colorFactor);
					if (colorFactor < MINBRIGHTNESS) //Reset if new color is too dark to see
						newGenome->genes.at(i) = tempVal;
				}
			}
			else if (i!=DIET_INDEX)
				newGenome->genes.at(i) = tempVal;
		}
	}*/
	double* colorRegionEnd = newGenome->genes + 3;
	double* genePtr = newGenome->genes;
	for (genePtr; genePtr != newGenome->visionRegionPtr; ++genePtr){
		r = rand() % 101;
		if (r < X){
			r = rand();
			q = rand() % 101;
			tempVal = *genePtr;
			//*genePtr = ((double)(r)) / RAND_MAX;
			if (q<50)
				*genePtr = (*genePtr) + ((double)(r)) / RAND_MAX;
			else
				*genePtr = (*genePtr) - ((double)(r)) / RAND_MAX;
			if ((*genePtr) < 1 && (*genePtr) > 0){
				if (genePtr < colorRegionEnd){
					colorFactor = newGenome->genes[0] * newGenome->genes[0] + newGenome->genes[1] * newGenome->genes[1] + newGenome->genes[2] * newGenome->genes[2];
					if (colorFactor < MINBRIGHTNESSSQUARED) //Reset if new color is too dark to see
						*genePtr = tempVal;
				}
			}
			else if (genePtr != newGenome->genes+DIET_INDEX)
				*genePtr = tempVal;
			//if (i == DIET_INDEX){
			//	r = rand() % 101;
			//	if (r < 50)
			//		*genePtr = -1 * (*genePtr);
			//}
		}
	}
	//Note: Color vector is always first 3 genes
	/*for (i = i; i < newGenome->genes.size(); ++i){
		if (!ALLOWMUTATIONS_VISION && i >= newGenome->visionNet_startIdx && i < newGenome->focusRegion_startIdx)
			i = newGenome->focusRegion_startIdx;
		if (!ALLOWMUTATIONS_FOCUS && i >= newGenome->focusRegion_startIdx && i < newGenome->pursuitRegion_startIdx)
			i = newGenome->pursuitRegion_startIdx;
		if (!ALLOWMUTATIONS_PURSUE && i >= newGenome->pursuitRegion_startIdx && i < newGenome->fleeRegion_startIdx)
			i = newGenome->fleeRegion_startIdx;
		if (!ALLOWMUTATIONS_FLEE && i >= newGenome->fleeRegion_startIdx && i < newGenome->socialRegion_startIdx)
			i = newGenome->socialRegion_startIdx;
		if (!ALLOWMUTATIONS_SOCIAL && i >= newGenome->socialRegion_startIdx && i < newGenome->nullRegion_startIdx)
			i = newGenome->nullRegion_startIdx;
		r = rand() % 101;
		if (r <= X){
			r = 3*rand();
			q = rand() % 101;
			if (q<50) //positive
				newGenome->genes.at(i) = newGenome->genes.at(i)+ static_cast<double>(r) / RAND_MAX;
			else //negative
				newGenome->genes.at(i) = newGenome->genes.at(i)- (static_cast<double>(r) / RAND_MAX);
		}
	}*/
	for (genePtr; genePtr != newGenome->endPtr; ++genePtr){
		if (!ALLOWMUTATIONS_VISION && genePtr >= newGenome->visionRegionPtr && genePtr < newGenome->focusRegionPtr)
			genePtr = newGenome->focusRegionPtr;
		if (!ALLOWMUTATIONS_FOCUS && genePtr >= newGenome->focusRegionPtr && genePtr < newGenome->pursuitRegionPtr)
			genePtr = newGenome->pursuitRegionPtr;
		if (!ALLOWMUTATIONS_PURSUE && genePtr >= newGenome->pursuitRegionPtr && genePtr < newGenome->fleeRegionPtr)
			genePtr = newGenome->fleeRegionPtr;
		if (!ALLOWMUTATIONS_SOCIAL && genePtr >= newGenome->fleeRegionPtr && genePtr < newGenome->socialRegionPtr)
			genePtr = newGenome->socialRegionPtr;
		if (!ALLOWMUTATIONS_SOCIAL && genePtr >= newGenome->socialRegionPtr && genePtr < newGenome->nullRegionPtr)
			genePtr = newGenome->nullRegionPtr;
		if (!ALLOWMUTATIONS_NULL && genePtr >= newGenome->nullRegionPtr)
			break;
		r = rand() % 101;
		if (r <= X){
			r = 3 * rand();
			q = rand() % 101;
			if (q<50) //positive
				*genePtr = (*genePtr) + static_cast<double>(r) / RAND_MAX;
			else //negative
				(*genePtr) = (*genePtr) - (static_cast<double>(r) / RAND_MAX);
		}
	}
}

void CrossOver(Genome* newGenome, const Genome rhs){
	if (newGenome->genomeSize != rhs.genomeSize){
		printf("Error: Crossover called on genomes with different sizes\n");
		return;
	}
	bool allSame = true;
	int start = rand() % newGenome->genomeSize;
	int end = start + rand() % (newGenome->genomeSize-start+1);
	int r = rand() % 101;
	int q = rand() % 101;
	//2-way crossover
	if (q < 50){
		//Keep left side
		if (r < 50){
			double* leftPointer = newGenome->genes+start;
			double* rightPointer = rhs.genes+start;
			while (leftPointer < newGenome->genes+(newGenome->genomeSize)){
				*leftPointer = *rightPointer;
				++leftPointer;
				++rightPointer;
			}
		}
		//Keep right side
		else{
			double* leftPointer = newGenome->genes;
			double* rightPointer = rhs.genes;
			while (leftPointer < newGenome->genes + start){
				*leftPointer = *rightPointer;
				++leftPointer;
				++rightPointer;
			}
		}
	}
	//3-way crossover
	else{
		//cross inner genes:
		if (r < 50){
			double* leftPointer = newGenome->genes + start;
			double* rightPointer = rhs.genes + start;
			double* leftEndPtr = newGenome->genes + end;
			while (leftPointer < leftEndPtr){
				*leftPointer = *rightPointer;
				++leftPointer;
				++rightPointer;
			}
			//for (int i = start; i < end; ++i)
				//newGenome->genes.at(i) = rhs.genes.at(i);
		}
		//cross outer genes
		else{
			double* leftPointer = newGenome->genes;
			double* rightPointer = rhs.genes;
			double* leftEndPtr = leftPointer + start;
			while (leftPointer < leftEndPtr){
				*leftPointer = *rightPointer;
				++leftPointer;
				++rightPointer;
			}
			leftPointer = newGenome->genes + end;
			rightPointer = rhs.genes + end;
			leftEndPtr = newGenome->genes + newGenome->genomeSize;
			while (leftPointer < leftEndPtr){
				*leftPointer = *rightPointer;
				++leftPointer;
				++rightPointer;
			}
			//for (int i = 0; i < start; ++i)
			//	newGenome->genes.at(i) = rhs.genes.at(i);
			//for (int i = end; i < newGenome->genes.size(); ++i)
			//	newGenome->genes.at(i) = rhs.genes.at(i);
		}
	}
}
