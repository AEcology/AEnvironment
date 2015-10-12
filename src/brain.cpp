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
//    brain.cpp
//    Definitions of modular neural net structure
//
//############################################################################################################//



#include "brain.h"
#include "environment.h"
#include <iostream>

//where to start mapping values:
#define MAP_BEGIN_INDEX 4

using namespace std;

//See brain.h for brain map

//used to determine feedforward strategies:
extern bool FOCUSREGION_ENABLED;
extern bool VISIONREGION_ENABLED;

Brain::Brain(){
	pursuitRegion = NULL;
	fleeRegion = NULL;
	socialRegion = NULL;
	visionNet = NULL;
}

Brain::Brain(Genome* newGenes){
	/*    Genome vector (copy from genome class declarations in atoms.h):
	*begin|colors(3)|size|eye|diet|vision 0 weights|focus weights|pursuit weights|flee weights|social weights|null weights|end
	* note: new brain regions can be added but their order in genome CANNOT be modified (without changing mutation functions)
	*/


	visionNet     = new ClassificationNet(VISIONREGION_NUMINPUTS, VISIONREGION_HIDDENUNITS, VISIONREGION_NUMOUTPUTS, newGenes->visionNet_startIdx, newGenes->visionRegionPtr);
	focusNet      = new ClassificationNet(FOCUSREGION_NUMINPUTS, FOCUSREGION_HIDDENUNITS, FOCUSREGION_NUMOUTPUTS, newGenes->focusRegion_startIdx, newGenes->focusRegionPtr);
	pursuitRegion = new ReflexNet(MOTORREGION_NUMINPUTS, MOTORREGION_HIDDENUNITS, MOTORREGION_NUMOUTPUTS, newGenes->pursuitRegion_startIdx, newGenes->pursuitRegionPtr);
	fleeRegion    = new ReflexNet(MOTORREGION_NUMINPUTS, MOTORREGION_HIDDENUNITS, MOTORREGION_NUMOUTPUTS, newGenes->fleeRegion_startIdx, newGenes->fleeRegionPtr);
	socialRegion  = new ReflexNet(MOTORREGION_NUMINPUTS, MOTORREGION_HIDDENUNITS, MOTORREGION_NUMOUTPUTS, newGenes->socialRegion_startIdx, newGenes->socialRegionPtr);
	nullRegion    = new ReflexNet(NULLREGION_NUMINPUTS, NULLREGION_HIDDENUNITS, NULLREGION_NUMOUTPUTS, newGenes->nullRegion_startIdx, newGenes->nullRegionPtr);
}

Brain::~Brain(){
	delete visionNet;
	delete focusNet;
	delete pursuitRegion;
	delete fleeRegion;
	delete socialRegion;
	delete nullRegion;
}


bool Brain::FeedForward(double* inputs, double* outputs, vector<NearbyObject*> nearbyObj, NearbyObject &focusObj, const Atom* thisAtom){

	double* perceptionInputs;
	double* perceptionOutputs0 = new double[VISIONREGION_NUMOUTPUTS];
	double* perceptionOutputs1 = new double[VISIONREGION_NUMOUTPUTS];
	//Initialize visionnet outputs to 0
	double* it1 = perceptionOutputs1;
	for (double* it0 = perceptionOutputs0; it0 != (perceptionOutputs0 + VISIONREGION_NUMOUTPUTS); ++it0){
		*it0 = 0.0;
		*it1 = 0.0;
		++it1;
	}

	double* focusInputs = new double[FOCUSREGION_NUMINPUTS];
	double* focusOutputs = new double[FOCUSREGION_NUMOUTPUTS];
	for (it1 = focusInputs; it1 != focusInputs + FOCUSREGION_NUMINPUTS; ++it1){
		*it1 = 0.0;
	}
	for (it1 = focusOutputs; it1 != focusOutputs + FOCUSREGION_NUMOUTPUTS; ++it1){
		*it1 = 0.0;
	}


	double* object0Params = new double[MOTORREGION_NUMINPUTS];
	object0Params[0] = inputs[6];
	object0Params[1] = inputs[7];
	object0Params[2] = inputs[18];




	double* object1Params = new double[MOTORREGION_NUMINPUTS];
	object1Params[0] = inputs[15];
	object1Params[1] = inputs[16];
	object1Params[2] = inputs[18];


	if (VISIONREGION_ENABLED){

		perceptionInputs = inputs;


		if (!visionNet->FeedForward(perceptionInputs, perceptionOutputs0)){
			cout << "Feedforward error in Vision region (object 0)" << endl;
			return false;
		}
		//reconfigure inputs for next object in fov
		//perceptionInputs.at(0) = inputs.at(9);
		//perceptionInputs.at(1) = inputs.at(10);
		//perceptionInputs.at(2) = inputs.at(11);
		//perceptionInputs.at(3) = inputs.at(12);
		//perceptionInputs.at(4) = inputs.at(13);
		//perceptionInputs.at(5) = inputs.at(14);
		perceptionInputs = inputs+9;

		if (!visionNet->FeedForward(perceptionInputs, perceptionOutputs1)){
			cout << "Feedforward error in Vision region (object 1)" << endl;
			return false;
		}

	}
	else if (nearbyObj.size() > 0){
		if ((nearbyObj.at(0)->theObject->isSugar && *(thisAtom->genome->genes + (DIET_INDEX))>0) || (nearbyObj.at(0)->theObject->isArsenic && *(thisAtom->genome->genes+(DIET_INDEX))<0)){

			perceptionOutputs0[1] = 1;
		}
		else if (nearbyObj.at(0)->theObject->isHunter){
	
			perceptionOutputs0[0] = 1;
		}
		else if (nearbyObj.at(0)->theObject->isAtom && ((Atom*)nearbyObj.at(0)->theObject)->canReproduce && thisAtom->genome->fitness != 0 && (((Atom*)nearbyObj.at(0)->theObject)->genome->fitness / thisAtom->genome->fitness>0.50)){

			perceptionOutputs0[2] = 1;
		}

		if ((nearbyObj.at(1)->theObject->isSugar && *(thisAtom->genome->genes+(DIET_INDEX))>0) || (nearbyObj.at(1)->theObject->isArsenic && *(thisAtom->genome->genes+(DIET_INDEX))<0)){

			perceptionOutputs1[1] = 1;
		}
		else if (nearbyObj.at(1)->theObject->isHunter){

			perceptionOutputs1[0] = 1;
		}
		else if (nearbyObj.at(1)->theObject->isAtom && ((Atom*)nearbyObj.at(1)->theObject)->canReproduce && thisAtom->genome->fitness!=0 && (((Atom*)nearbyObj.at(1)->theObject)->genome->fitness / thisAtom->genome->fitness>0.50)){

			perceptionOutputs1[2] = 1;
		}

	}

	if (FOCUSREGION_ENABLED){

		//count how many outputs are 1 (there should only be one of them, else net is confused)
		int classCounter0 = 0;
		int classCounter1 = 0;


		it1 = perceptionOutputs1;
		for (double* it0 = perceptionOutputs0; it0 != (perceptionOutputs0 + VISIONREGION_NUMOUTPUTS); ++it0){
			if (*it0 == 1)
				++classCounter0;
			if (*it1 == 1)
				++classCounter1;
			++it1;
		}

		//pass perceptions through focus net to determine which object to focus on
		//if we didnt successfully classify an object, set all of its data to 0
		/*if (classCounter0 != 1){
			focusInputs.push_back(0.0);
			focusInputs.push_back(0.0);
			focusInputs.push_back(0.0);
			focusInputs.push_back(0.0);
			focusInputs.push_back(0.0);
		}
		else{
			focusInputs.push_back(perceptionOutputs0.at(0));
			focusInputs.push_back(perceptionOutputs0.at(1));
			focusInputs.push_back(perceptionOutputs0.at(2));
			focusInputs.push_back(inputs.at(6));
			focusInputs.push_back(inputs.at(8));
		}*/
		if (classCounter0 == 1){
			focusInputs[0] = perceptionOutputs0[0];
			focusInputs[1] = perceptionOutputs0[1];
			focusInputs[2] = perceptionOutputs0[2];
			focusInputs[3] = inputs[6];
			focusInputs[4] = inputs[8];
		}
		/*if (classCounter1 != 1){
			focusInputs.push_back(0.0);
			focusInputs.push_back(0.0);
			focusInputs.push_back(0.0);
			focusInputs.push_back(0.0);
			focusInputs.push_back(0.0);
		}
		else
		{
			focusInputs.push_back(perceptionOutputs1.at(0));
			focusInputs.push_back(perceptionOutputs1.at(1));
			focusInputs.push_back(perceptionOutputs1.at(2));
			focusInputs.push_back(inputs.at(15)); //dist
			focusInputs.push_back(inputs.at(17)); //energy/fitness ref
		}*/
		if (classCounter1 == 1){
			focusInputs[5] = perceptionOutputs1[0];
			focusInputs[6] = perceptionOutputs1[1];
			focusInputs[7] = perceptionOutputs1[2];
			focusInputs[8] = inputs[15];
			focusInputs[9] = inputs[17];
		}

		//feed vision classifier information through focus net
		//focusOutputs.push_back(0);
		//focusOutputs.push_back(0);

		if (!focusNet->FeedForward(focusInputs, focusOutputs)){
			cout << "Feedforward error in focusNet" << endl;
			return false;
		}
	}
	else{ //hard-code to decide on which to watch
		//if (nearbyObj.size() == 0){
		//	focusOutputs.push_back(0.0);
		//	focusOutputs.push_back(0.0);
		//}
		if (nearbyObj.size() > 0 && ((nearbyObj.at(1)->theObject->isHunter && !nearbyObj.at(0)->theObject->isHunter) || (nearbyObj.at(0)->theObject->isAtom && !perceptionOutputs0[2] == 1.0)) ){
			//focusOutputs.push_back(0.0);
			//focusOutputs.push_back(1.0);
			focusOutputs[1] = 1;
		}
		else if (nearbyObj.size() > 0){
			//focusOutputs.push_back(1.0);
			//focusOutputs.push_back(0.0);
			focusOutputs[0] = 1;
		}
	}

	//count classifications from focusNet
	int classCounter = 0;
	if (focusOutputs[0] == 1)
		++classCounter;
	if (focusOutputs[1] == 1)
		++classCounter;


	if (classCounter != 1){ //null: Just feed current facing angle to nullRegion
		//vector<double> nullInputs;
		//nullInputs.push_back(inputs.at(18));
		//nullInputs.push_back(inputs.at(19));
		//nullInputs.push_back(inputs.at(20));
		double nullInputs[NULLREGION_NUMINPUTS] = { inputs[18], inputs[19], inputs[20] };
		
		if (!nullRegion->FeedForward(nullInputs, outputs))
			return false;
	}

	//build motor/perception Inputs based upon which obj is in focus
	else{
		int k = 0;
		//vector<double> perceptionOutputs;
		//vector<double> motorInputs;
		double* perceptionOutputs;
		double* motorInputs;

		if (focusOutputs[0] == 1){ //react to object 0
			focusObj = *nearbyObj.at(0);
			perceptionOutputs = perceptionOutputs0;
			motorInputs = object0Params;
		}
		else if (focusOutputs[1] == 1){ //react to object 1
			focusObj = *nearbyObj.at(1);
			perceptionOutputs = perceptionOutputs1;
			motorInputs = object1Params;
		}
		else{
			focusObj.theObject = NULL;
			focusObj.classifier = null;
			perceptionOutputs = new double[VISIONREGION_NUMOUTPUTS];
			motorInputs = new double[MOTORREGION_NUMINPUTS];
			for (double* it = motorInputs; it != motorInputs + MOTORREGION_NUMINPUTS; ++it)
				*it = 0.0;
			for (double* it = perceptionOutputs; it != perceptionOutputs + VISIONREGION_NUMOUTPUTS; ++it)
				*it = 0.0;
			printf("Warning: Unintended brain feedforward path (feeding empty motor input vector to FF\n");
		}
		if (perceptionOutputs[0] == 1){ //predator
			focusObj.classifier = predator;
			if (!fleeRegion->FeedForward(motorInputs, outputs))
				return false;
		}
		else if (perceptionOutputs[1] == 1){ //prey
			focusObj.classifier = prey;
			if (!pursuitRegion->FeedForward(motorInputs, outputs))
				return false;
		}
		else if (perceptionOutputs[2] == 1){ //friendly
			if (focusObj.theObject != NULL && focusObj.theObject->isAtom)
				focusObj.classifier = friendly;
			else
				focusObj.classifier = null;
			if (!socialRegion->FeedForward(motorInputs, outputs))
				return false;
		}
		else{ //null
			focusObj.classifier = null;
			//vector<double> nullInputs;
			//nullInputs.push_back(inputs.at(14));
			double nullInputs[NULLREGION_NUMINPUTS] = { inputs[18], inputs[19], inputs[20] };
			if (!nullRegion->FeedForward(nullInputs, outputs))
				return false;
		}
		if (motorInputs != object0Params && motorInputs != object1Params){
			delete[] motorInputs;
			delete[] perceptionOutputs;
		}
	}


	delete[] perceptionOutputs0;
	delete[] perceptionOutputs1;
	delete[] focusInputs;
	delete[] focusOutputs;
	delete[] object0Params;
	delete[] object1Params;

	return true;
}

