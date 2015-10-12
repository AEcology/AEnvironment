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
//    atoms.cpp
//    Agent functionality
//
//############################################################################################################//


#include "atoms.h"
#include "environment.h"
#include "evolution.h"
#include <math.h>
#include "food.h"
#include <iostream>

#define PI 3.1415926f
#define SHOW_FOV false //TODO fix opacity blending

using namespace std;

int OBJECTS_MAX_CAPACITY = 350;

extern SPEED_MODE SPEEDMODE;
extern bool PREDATORY_ACTIONS_ENABLED;
extern bool VISIONREGION_ENABLED;
extern bool FOCUSREGION_ENABLED;
//from main:
extern double GENETIC_DISTANCE_THRESHOLD; 
extern vec2* SCREEN_POSITION;

//create Atom with randomized genome
//Note: New atoms will always be red
Atom::Atom(){
	numChildren = 0;
	canReproduce = false;
	isAtom = true;
	isSugar = false;
	isArsenic = false;
	isHunter = false;
	isSquare = false;
	isCircle = true;
	currMoveSpeed=0;
	nearestObject = NULL;
	dist1 = visionDist;
	theta1 = 2 * PI;
	age = 0;
	alive = true;
	energy = INITIALENERGY;
	timer = INITIALTIMER;
	int i = 0;
	double eyeMult = 0;
	double eyeMin = 0;
	double eyeMax = 0;
	float speedMult = 0;
	//build randomized genome:
	this->genome = new Genome();
	//initialize brainInputs
	//brainInputs = new double [BRAIN_NUMINPUTS];
	//this->brainInputs = (double*)malloc(sizeof(double)*BRAIN_NUMINPUTS);
	for (double* p = brainInputs; p != brainInputs + BRAIN_NUMINPUTS; ++p){
		*p = 0.0;
	}
	//initialize brainOutputs
	//this->brainOutputs = new double[BRAIN_NUMOUTPUTS];
	for (double* p = brainOutputs; p != brainOutputs+BRAIN_NUMOUTPUTS; ++p){
		*p = 0.0;
	}
	brain = new Brain(this->genome);
	//color[0] = this->genome->genes[0];
	//color[1] = this->genome->genes.at(1);
	//color[2] = this->genome->genes.at(2);
	color[0] = static_cast<float>(this->genome->genes[0]);
	color[1] = static_cast<float>(this->genome->genes[1]);
	color[2] = static_cast<float>(this->genome->genes[2]);
	
	//speedMult = this->genome->genes.at(3);
	speedMult = static_cast<float>(this->genome->genes[3]);
	size = static_cast<int>(ATOM_MINSIZE + (double)(ATOM_SIZE_SPAN)*(1 - speedMult));
	halfSize = static_cast<int>(size / 2);
	moveSpeed = ATOM_MINSPEED + ATOM_SPEED_SPAN*speedMult;
	spinSpeed = ATOM_MINSPIN + ATOM_SPIN_SPAN*(1-speedMult);
	metabolismRate = 500 + static_cast<int>((1 - speedMult) * 1000);

	eyeMult = this->genome->genes[4];
	eyeMin = (double)size / 7;
	eyeMax = (double)halfSize;
	eyeRadius = static_cast<float>(eyeMin + (eyeMax - eyeMin)*eyeMult);
	fieldOfView = static_cast<float>(eyeMult*PI);
	eyeMult = 1 - eyeMult;
	visionDist = static_cast<float>(SCREEN_WIDTH*eyeMult);


	direction = 0;
	position = new vec2(static_cast<float>(rand() % WORLD_WIDTH), static_cast<float>(rand() % WORLD_HEIGHT));
}

Atom::Atom(const Genome &rhsGenome, float x, float y){
	numChildren = 0;
	canReproduce = false;
	isHunter = false;
	isAtom = true;
	isSugar = false;
	isArsenic = false;
	isCircle = true;
	isSquare = false;
	currMoveSpeed = 0;
	nearestObject = NULL;
	dist1 = visionDist;
	theta1 = 2 * PI;
	age = 0;
	alive = true;
	double eyeMult = 0;
	double eyeMin = 0;
	double eyeMax = 0;
	float speedMult = 0;
	energy = INITIALENERGY;
	timer = INITIALTIMER;
	int i = 0;
	//clone genome:
	this->genome = new Genome(rhsGenome);
	//initialize brainInputs
	this->brainInputs = new double[BRAIN_NUMINPUTS];
	for (double* p = brainInputs; p != brainInputs + BRAIN_NUMINPUTS; ++p){
		*p = 0.0;
	}
	//initialize brainOutputs
	this->brainOutputs = new double[BRAIN_NUMOUTPUTS];
	for (double* p = brainOutputs; p != brainOutputs + BRAIN_NUMOUTPUTS; ++p){
		*p = 0.0;
	}
	brain = new Brain(this->genome);
	color[0] = static_cast<float>(this->genome->genes[0]);
	color[1] = static_cast<float>(this->genome->genes[1]);
	color[2] = static_cast<float>(this->genome->genes[2]);

	//determine properties using genome
	speedMult = static_cast<float>(this->genome->genes[3]);
	size = static_cast<int>(ATOM_MINSIZE + (double)(ATOM_SIZE_SPAN)*speedMult);
	halfSize = size / 2;
	moveSpeed = ATOM_MINSPEED + ATOM_SPEED_SPAN*(1 - speedMult);
	spinSpeed = ATOM_MINSPIN + ATOM_SPIN_SPAN*(1 - speedMult);
	metabolismRate = 500 + static_cast<int>((1 - speedMult) * 1000);

	eyeMult = this->genome->genes[4];
	eyeMin = (double)size / 8;
	eyeMax = (double)size / 3;
	eyeRadius = static_cast<float>(eyeMin + (eyeMax - eyeMin)*eyeMult);
	fieldOfView = static_cast<float>(eyeMult*PI);
	eyeMult = 1 - eyeMult;
	visionDist = static_cast<float>(SCREEN_WIDTH*eyeMult);
	direction = 0;
	position = new vec2(x, y);
}

//see atom.h for descriptions
Atom::Atom(const Genome &rhsGenome){
	numChildren = 0;
	canReproduce = false;
	isHunter = false;
	isAtom = true;
	isSugar = false;
	isCircle = true;
	isSquare = false;
	currMoveSpeed = 0;
	nearestObject = NULL;
	dist1 = visionDist;
	theta1 = 2 * PI;
	age = 0;
	alive = true;
	double eyeMult = 0;
	double eyeMin = 0;
	double eyeMax = 0;
	float speedMult = 0;
	energy = INITIALENERGY;
	timer = INITIALTIMER;
	int i = 0;
	//clone genome:
	this->genome = new Genome(rhsGenome);
	//initialize brainInputs
	this->brainInputs = new double[BRAIN_NUMINPUTS];
	for (double* p = brainInputs; p != brainInputs + BRAIN_NUMINPUTS; ++p){
		*p = 0.0;
	}
	//initialize brainOutputs
	this->brainOutputs = new double[BRAIN_NUMOUTPUTS];
	for (double* p = brainOutputs; p != brainOutputs + BRAIN_NUMOUTPUTS; ++p){
		*p = 0.0;
	}
	brain = new Brain(this->genome);
	color[0] = static_cast<float>(this->genome->genes[0]);
	color[1] = static_cast<float>(this->genome->genes[1]);
	color[2] = static_cast<float>(this->genome->genes[2]);

	//determine properties using genome
	speedMult = static_cast<float>(this->genome->genes[3]);
	size = static_cast<int>(ATOM_MINSIZE + (double)(ATOM_SIZE_SPAN)*speedMult);
	halfSize = size / 2;
	moveSpeed = ATOM_MINSPEED + ATOM_SPEED_SPAN*(1 - speedMult);
	spinSpeed = ATOM_MINSPIN + ATOM_SPIN_SPAN*(1 - speedMult);
	metabolismRate = 500 + static_cast<int>((1 - speedMult) * 1000);

	eyeMult = this->genome->genes[4];
	eyeMin = (double)size / 8;
	eyeMax = (double)size / 3;
	eyeRadius = static_cast<float>(eyeMin + (eyeMax - eyeMin)*eyeMult);
	fieldOfView = static_cast<float>(eyeMult*PI);
	eyeMult = 1 - eyeMult;
	visionDist = static_cast<float>((float)SCREEN_WIDTH*eyeMult);
	direction = 0;
	position = new vec2(static_cast<float>(rand() % WORLD_WIDTH), static_cast<float>(rand() % WORLD_HEIGHT));
}

Atom::~Atom(){
	delete brain;
	delete genome;
	delete position;
	delete [] brainInputs;
	delete [] brainOutputs;
}


bool Atom::UpdatePosition(float x, float y){
	if (x > SCREEN_WIDTH || x<0 || y>SCREEN_HEIGHT || y < 0)
		return false;
	position->x = x;
	position->y = y;
	return true;
}


// -2pi < direction < 2pi
void Atom::Move(float direction){
	float velocity = this->currMoveSpeed*this->moveSpeed;
	if (direction >  2*PI || direction < -2 * PI)
		return;
	float newx = position->x + velocity*cosf(direction);
	float newy = position->y + velocity*sinf(direction);
	//Enforce toroidal transitions:
	//if (newx < 0)
	//	newx = WORLD_WIDTH-this->halfSize;
	//else if (newx > WORLD_WIDTH)
	//	newx = 0+this->halfSize;
	//else if (newy < 0)
	//	newy = WORLD_HEIGHT-this->halfSize;
	//else if (newy > WORLD_HEIGHT)
	//	newy = 0+this->halfSize;
	if (!(newx <= 0 || newx >= WORLD_WIDTH || newy <= 0 || newy >= WORLD_HEIGHT)){
		position->x = newx;
		position->y = newy;
	}
}

//-1 < direction < 1
void Atom::Spin(float direction){
	if (direction < (-1) || direction>1)
		return;
	this->direction = this->direction + direction*this->spinSpeed;
	if (this->direction < -2*PI || this->direction>2*PI)
		this->direction = 0;
}

//increment clock if eating
//if not eating, decrement clock (rate depends on size and exertion)
//energy decrements when clock hits 0 (averages about once per sec)
void Atom::Action(vector<Object*>* allObjects){
	if (nearestObject != NULL && focusObject!=NULL && nearestObject == focusObject->theObject && focusObject->classifier == prey){
		if (nearestObject->isSugar){
			Sugar* thisSugar = (Sugar*)nearestObject;
			if (dist1 < ((float)this->size / 2) + ((float)nearestObject->size) * 2){
				//food is healthy for this atom:
				if (this->genome->genes[DIET_INDEX] > 0 && this->genome->genes[DIET_INDEX] <= 0.5){
					this->energy = this->energy + 2;
					thisSugar->energy = thisSugar->energy - 2;
					thisSugar->needsUpdate = true;
				}
				else if (this->genome->genes[DIET_INDEX] > 0.5 && this->genome->genes[DIET_INDEX] <= 1.5){
					++energy;
					--thisSugar->energy;
					thisSugar->needsUpdate = true;
				}
				//food is poisonous to this atom:
				else{
					energy = energy - 2;
					--thisSugar->energy;
					thisSugar->needsUpdate = true;
				}
			}
		}
		else if (nearestObject->isAtom){
			Atom* thisAtom = (Atom*)nearestObject;
			if (PREDATORY_ACTIONS_ENABLED && dist1 < ((float)this->size / 2) + ((float)nearestObject->size) / 2){
				//Sugar predators can only eat atoms who eat sugar, and vice-versa
				if (this->genome->genes[DIET_INDEX] < -0.5 && this->genome->genes[DIET_INDEX]>-1.5){
					if (thisAtom->genome->genes[DIET_INDEX] < 0){
						this->energy = this->energy + (int)this->size;
						thisAtom->energy = thisAtom->energy - (int)this->size;
					}
					else{
						this->energy = this->energy - 2*(int)this->size;
						thisAtom->energy = thisAtom->energy - (int)this->size;
					}
				}
				else if (this->genome->genes[DIET_INDEX] < -1.5 ){
					if (thisAtom->genome->genes[DIET_INDEX < 0]){
						this->energy = this->energy + 2*(int)this->size;
						thisAtom->energy = thisAtom->energy - 2*(int)this->size;
					}
					else{
						this->energy = this->energy - 2*(int)this->size;
						thisAtom->energy = thisAtom->energy - (int)this->size;
					}
				}
				else if (this->genome->genes[DIET_INDEX] > 0.5 && this->genome->genes[DIET_INDEX]<1.5){
					if (thisAtom->genome->genes[DIET_INDEX] > 0){
						this->energy = this->energy + (int)this->size;
						thisAtom->energy = thisAtom->energy - (int)this->size;
					}
					else{
						this->energy = this->energy - 2*(int)this->size;
						thisAtom->energy = thisAtom->energy - (int)this->size;
					}
				}
				else if (this->genome->genes[DIET_INDEX] > 1.5){
					if (thisAtom->genome->genes[DIET_INDEX] > 0){
						this->energy = this->energy + 2*(int)this->size;
						thisAtom->energy = thisAtom->energy - 2*(int)this->size;
					}
					else{
						this->energy = this->energy - (int)this->size;
						thisAtom->energy = thisAtom->energy - (int)this->size;
					}
				}
				//nearestObject->energy = nearestObject->energy - (int)this->size;
				if (((Atom*)nearestObject)->energy <= 0)
					((Atom*)nearestObject)->alive = false;
			}
		}
		else if (nearestObject->isArsenic){
			Arsenic* thisArsenic = (Arsenic*)nearestObject;
			if (dist1 < ((float)this->size / 2) + ((float)nearestObject->size) * 2){
				//food healthy for this atom:
				if ( this->genome->genes[DIET_INDEX] < 0 && this->genome->genes[DIET_INDEX] >=-0.5){
					this->energy = this->energy + 20;
					thisArsenic->energy = -1;
					thisArsenic->needsUpdate = true;
				}
				else if (this->genome->genes[DIET_INDEX] < -0.5){
					this->energy = this->energy + 10;
					thisArsenic->energy = -1;
					thisArsenic->needsUpdate = true;
				}
				else{
					this->energy = this->energy - 40;
					thisArsenic->energy = -1;
					thisArsenic->needsUpdate = true;
				}
			}
		}
	}
	else if (nearestObject != NULL && focusObject!= NULL && nearestObject == focusObject->theObject && nearestObject->isAtom && focusObject->classifier == friendly && (dist1 < ((float)this->size / 2) + ((float)nearestObject->size) / 2)){
		Atom* otherAtom = (Atom*)nearestObject;
		if (this->canReproduce && otherAtom->canReproduce && GeneticDistance(this, otherAtom)<GENETIC_DISTANCE_THRESHOLD && (int)allObjects->size()<OBJECTS_MAX_CAPACITY){
			int r1 = 1 + rand() % 2;
			for (int i = 0; i < r1; ++i){
				int r2 = rand() % 101;
				Genome* newGenome = new Genome(*this->genome);
				if (r2 <= 80)
					CrossOver(newGenome, *otherAtom->genome);
				r2 = rand() % 101;
				if (r2 <= 30)
					MutateRefactor(newGenome, GENE_MUTATION_PROBABILITY);
				else if (r2 <= 80)
					StepMutateRefactor(newGenome, GENE_MUTATION_PROBABILITY);
				allObjects->push_back(new Atom(*newGenome, this->position->x, this->position->y));
				this->energy = this->energy - 10;
				otherAtom->energy = otherAtom->energy - 10;
				++this->numChildren;
				++otherAtom->numChildren;
				this->canReproduce = false;
				otherAtom->canReproduce = false;
				otherAtom->children.push_back(newGenome);
				this->children.push_back(newGenome);
				delete newGenome;
			}
		}
	}
	if (clock <= 0){
		--energy;
		clock = INITIALCLOCK;
	}
	else
		clock = clock - (1 + (int)(((float)metabolismRate)*currMoveSpeed));
	if (timer <= 0){
		++age;
		timer = INITIALTIMER;
	}
	else
		--timer;
	if (energy <= 0 || age >= LIFESPAN || this->genome->fitness<=0)
		alive = false;
	if (age % 4 == 0 && this->energy > INITIALENERGY)
		canReproduce = true;
}


//Only supports 10 closest objects at most
bool Atom::FindClosestObjects(const vector<Object*> allObjects, vector<NearbyObject*>* nearbyObjects, int n){
	int i = 0; //allObjects iterator
	int j = 0; //keep track of which index we are working with in array/vector
	for (i = 0; i < n; ++i){
		nearbyObjects->push_back(new NearbyObject(allObjects.at(0), 0, visionDist + 1));
	}
	float currAngle = 2 * PI;
	float tempDirection = 0;
	float currDist = visionDist+1;
	float tempVal = 0;
	vec2 *src = new vec2(this->position->x, this->position->y); //unit vector of agent
	vec2 *dest = new vec2(0, 0); //unit vector for object of interest
	vec2 *magnitude = new vec2(0, 0);
	vec2 *direction = new vec2(0, 0);
	if ((int)allObjects.size() <= (n - 1)){
		cout << "FindClosestObjects error: input vector size" << endl;
		return false;
	}
	nearestObject = allObjects.at(0);
	for (i = 0; i < (int)allObjects.size(); ++i){
		currDist = Distance(this->position, allObjects.at(i)->position);
		j = n - 1;
		if (currDist<nearbyObjects->at(j)->theDistance && allObjects.at(i)!=this){
			//decrement until we find index where this object would fit
			while (j>0 && currDist < nearbyObjects->at(j-1)->theDistance)
				--j;
			dest->x = allObjects.at(i)->position->x;
			dest->y = allObjects.at(i)->position->y;
			//Normalize(dest); //get unit vector
			direction->x = cos(this->direction);
			direction->y = sin(this->direction);
			magnitude->x = dest->x - src->x;
			magnitude->y = dest->y - src->y;
			Normalize(magnitude);
			//Get the angle between the two (absolute value)
			currAngle = acos(Dot(magnitude, direction));
			//Rotate direction vector 90 degrees and Dot again to get sign
			tempDirection = this->direction + PI / 2;
			direction->x = cos(tempDirection);
			direction->y = sin(tempDirection);
			magnitude->x = dest->x - src->x;
			magnitude->y = dest->y - src->y;
			if (Dot(magnitude, direction) < 0)
				currAngle = currAngle*-1;
			if (currAngle > (-1 * (this->fieldOfView) / 2) && currAngle < (this->fieldOfView / 2)){
				NearbyObject* newNearbyObj = new NearbyObject(allObjects.at(i), currAngle, currDist);
				nearbyObjects->insert(nearbyObjects->begin() + j, newNearbyObj);
				newNearbyObj = nearbyObjects->back();
				nearbyObjects->pop_back(); 
				delete newNearbyObj;
			}
		}
	}
	nearestObject = nearbyObjects->at(0)->theObject;
	dist1 = nearbyObjects->at(0)->theDistance;
	theta1 = nearbyObjects->at(0)->theAngle;
	if (nearbyObjects->at(0)->theDistance > visionDist){
		nearestObject = NULL;
		focusObject = NULL;
		dist1 = visionDist;
		theta1 = 0;
	}
	delete src;
	delete dest;
	delete magnitude;
	delete direction;
	return true;
}



//Update current state
//Note: See brain.h for neural input/output map
bool Atom::Update(vector<Object*>* allObjects){
	vector<NearbyObject*> nearbyObjects;
	Object* thisObject;
	bool isFocused = false;

	if (focusObject != NULL)
		delete focusObject;

	FindClosestObjects(*allObjects, &nearbyObjects, NUM_DETECTABLE_OBJECTS);

	genome->fitness = (energy - age)*(numChildren+1);

	int i = 0;
	/*while (i<children.size()){
		if (children.at(i) == NULL){
			children.erase(children.begin()+i);
		}
		else{
			genome->fitness = genome->fitness + 2*children.at(i)->fitness;
			++i;
		}
	}*/

	/*if (nearestObject == NULL){
		brainInputs.at(0) = 0;
		brainInputs.at(1) = 0;
		brainInputs.at(2) = 0;
		brainInputs.at(3) = 0;
		brainInputs.at(4) = 0;
		brainInputs.at(5) = 0;
		brainInputs.at(6) = 0;
		brainInputs.at(7) = 0;
		brainInputs.at(8) = 0;
		brainInputs.at(9) = 0;
		brainInputs.at(10) = 0;
		brainInputs.at(11) = 0;
		brainInputs.at(12) = 0;
		brainInputs.at(13) = 0;
		brainInputs.at(14) = 0;
		brainInputs.at(15) = 0;
		brainInputs.at(16) = 0;
		brainInputs.at(17) = 0;
		brainInputs.at(18) = 0;
		brainInputs.at(19) = 0;
		brainInputs.at(20) = 0;
	}*/
	if (nearestObject == NULL){
		for (double* p = brainInputs; p != brainInputs + BRAIN_NUMINPUTS; ++p){
			*p = 0.0;
		}
	}
	else {
		int j = 0; //iterate through inputs
		for (int i = 0; i < NUM_DETECTABLE_OBJECTS; ++i){
			if (nearbyObjects.size()>0)
				thisObject = nearbyObjects.at(i)->theObject;
			else
				thisObject = NULL;
			if (thisObject == NULL){
				brainInputs[j] = 0;
				brainInputs[j + 1] = 0;
				brainInputs[j + 2] = 0;
				brainInputs[j + 3] = 0;
				brainInputs[j + 4] = 0;
				brainInputs[j + 5] = 0;
				brainInputs[j + 6] = 0;
				brainInputs[j + 7] = 0;
				brainInputs[j + 8] = 0;
				j += 9;
			}
			else{
				if (thisObject->isCircle){
					brainInputs[j] = 1;
					brainInputs[j + 1] = 0;
				}
				else if (thisObject->isSquare){
					brainInputs[j] = 0;
					brainInputs[j + 1] = 1;
				}
				j += 2;
				brainInputs[j] = thisObject->color[0];
				brainInputs[j + 1] = thisObject->color[1];
				brainInputs[j + 2] = thisObject->color[2];
				brainInputs[j + 3] = nearbyObjects.at(i)->theObject->size / this->size;
				brainInputs[j + 4] = nearbyObjects.at(i)->theDistance / visionDist;
				brainInputs[j + 5] = nearbyObjects.at(i)->theAngle;
				if (nearbyObjects.at(i)->theObject->isSugar)
					brainInputs[j + 6] = nearbyObjects.at(i)->theObject->energy / 100;
				else if (nearbyObjects.at(i)->theObject->isArsenic)
					brainInputs[j + 6] = nearbyObjects.at(i)->theObject->energy / 100;
				else if (nearbyObjects.at(i)->theObject->isAtom){
					if (this->genome->fitness > 0)
						brainInputs[j + 6] = ((Atom*)nearbyObjects.at(i)->theObject)->genome->fitness / this->genome->fitness;
					else
						brainInputs[j + 6] = 0;
				}
				j += 7;
			}
		}
	}


	//14: Direction facing currently
	brainInputs[18] = this->direction;
	brainInputs[19] = this->position->x / (double)WORLD_WIDTH;
	brainInputs[20] = this->position->y / (double)WORLD_HEIGHT;

	focusObject = new NearbyObject();

	//Pass sensory inputs to ANNs, get action vector
	if (!brain->FeedForward(brainInputs, brainOutputs, nearbyObjects, *focusObject, this))
		return false;
	if (nearestObject == NULL)
		focusObject = NULL;
	spin = static_cast<float>(brainOutputs[0]) * 2.0 - 1.0;
	move = (static_cast<float>(brainOutputs[1]) * 4.0 - 2.0)*PI; //subtract direction to get angle relative to agent
	currMoveSpeed = static_cast<float>(brainOutputs[2]);
	Spin(spin);
	Move(move); //convert [-1:1] to [-PI:pi];
	while (!nearbyObjects.empty()){
		NearbyObject* thisNearbyObject = nearbyObjects.back();
		nearbyObjects.pop_back();
		if (thisNearbyObject != focusObject)
			delete thisNearbyObject;
		//else
			//isFocused = true;
	}
	//if (isFocused == false)
	//	focusObject = NULL;

	Action(allObjects); //metabolism function

	return true;
}

void Atom::Render(){
	if (SPEEDMODE == Warp)
		return;
	int i = 0;
	float xtemp = 0;
	float ytemp = 0;
	float theta = direction;
	float eyeX = position->x + 2.f*halfSize / 3.f*cosf(direction);
	float eyeY = position->y + 2.f*halfSize / 3.f*sinf(direction);
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(color[0], color[1], color[2]);
	for (i = 0; i < 24; ++i){
		theta = 2.f*PI*static_cast<float>(i) / 24.f;
		xtemp = halfSize*cosf(theta);
		ytemp = halfSize*sinf(theta);
		glVertex2f(position->x + xtemp, position->y + ytemp);
	}
	glEnd();
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0,0,0);
	for (i = 0; i < 12; ++i){
		theta = 2.f*PI*static_cast<float>(i) / 12.f;
		xtemp = eyeRadius*cosf(theta);
		ytemp = eyeRadius*sinf(theta);
		glVertex2f(eyeX + xtemp, eyeY + ytemp);
	}
	glEnd();
	if (SHOW_FOV){
		glBegin(GL_TRIANGLE_FAN);
		glColor3f(0.192f, 0.72f, 0.0864f);
		float startAngle = direction -fieldOfView / 2.f;
		float endAngle = startAngle + fieldOfView;
		float span = 2.f*PI / fieldOfView;
		for (int i = 0; i <= 12; ++i){
			theta = startAngle + fieldOfView*static_cast<float>(i) / 12;
			xtemp = visionDist*cosf(theta);
			ytemp = visionDist*sinf(theta);
			glVertex2f(eyeX+xtemp, eyeY+ytemp);
		}
		glEnd();
	}
}

void Atom::Render3D(){
	if (SPEEDMODE == Warp)
		return;
	float eyeX = position->x + 2.f*halfSize / 3.f*cosf(direction);
	float eyeY = position->y + 2.f*halfSize / 3.f*sinf(direction);

	GLfloat atomColor[] = {color[0], color[1], color[2]};
	GLfloat colorSpecular[] = { 0.01f, 0.01f, 0.01f, 1.f };
	GLfloat eyeColor[] = { 0.f, 0.f, 0.f, 1.f };
	GLfloat eyeSpecular[] = { .4f, .4f, .4f, 1.f };

	GLfloat qaAmbientLight[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat qaDiffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat qaSpecularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat emitLight[] = { 0.9f, 0.9f, 0.9f, 0.01f };
	GLfloat Noemit[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat qaLightPosition[] = { 400.f, 0.f, 500.f, 1.f };
	// Enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// Set lighting intensity and color
	glLightfv(GL_LIGHT0, GL_AMBIENT, qaAmbientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, qaDiffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, qaSpecularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, qaLightPosition);

	// Set the light position
	glMatrixMode(GL_MODELVIEW);

	// clear the identity matrix.
	glLoadIdentity();
	glTranslatef(this->position->x - SCREEN_POSITION->x, this->position->y - SCREEN_POSITION->y, -(int)halfSize);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, atomColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, atomColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, colorSpecular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20);

	glutSolidSphere((int)halfSize, 25, 25);

	glFlush();


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(eyeX - SCREEN_POSITION->x, eyeY - SCREEN_POSITION->y, -eyeRadius);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, eyeColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, eyeColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, eyeSpecular);
	glutSolidSphere(eyeRadius, 25, 25);

	glFlush();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, halfSize, -halfSize);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	return;
}

/**********************************************************************************/
/*********************************GENOME*******************************************/
/**********************************************************************************/


void Genome::InstantiatePtrs(){
	visionRegionPtr = genes + visionNet_startIdx;
	focusRegionPtr = visionRegionPtr + num_visionRegion_weights;
	pursuitRegionPtr = focusRegionPtr + num_focusRegion_weights;
	fleeRegionPtr = pursuitRegionPtr + num_motorRegion_weights;
	socialRegionPtr = fleeRegionPtr + num_motorRegion_weights;
	nullRegionPtr = socialRegionPtr + num_motorRegion_weights;
	endPtr = nullRegionPtr + num_nullRegion_weights;
}

void Genome::InstantiateSizes(){
	visionNet_startIdx = WEIGHTS_STARTING_INDEX;
	/*num_visionRegion_weights = VISIONREGION_NUMOUTPUTS + VISIONREGION_HIDDENUNITS*(VISIONREGION_NUMINPUTS + VISIONREGION_NUMOUTPUTS);
	num_focusRegion_weights = FOCUSREGION_NUMOUTPUTS + FOCUSREGION_HIDDENUNITS*(FOCUSREGION_NUMINPUTS + FOCUSREGION_NUMOUTPUTS);
	num_motorRegion_weights = MOTORREGION_HIDDENUNITS*(MOTORREGION_NUMINPUTS + MOTORREGION_NUMOUTPUTS);
	num_nullRegion_weights = NULLREGION_HIDDENUNITS*(NULLREGION_NUMINPUTS + NULLREGION_NUMOUTPUTS);*/
	num_visionRegion_weights = VISIONREGION_NUMWEIGHTS;
	num_focusRegion_weights = FOCUSREGION_NUMWEIGHTS;
	num_motorRegion_weights = MOTORREGION_NUMWEIGHTS;
	num_nullRegion_weights = NULLREGION_NUMWEIGHTS;
	focusRegion_startIdx = visionNet_startIdx + num_visionRegion_weights;
	pursuitRegion_startIdx = focusRegion_startIdx + num_focusRegion_weights;
	fleeRegion_startIdx = pursuitRegion_startIdx + num_motorRegion_weights;
	socialRegion_startIdx = fleeRegion_startIdx + num_motorRegion_weights;
	nullRegion_startIdx = socialRegion_startIdx + num_motorRegion_weights;
	//this->genomeSize = WEIGHTS_STARTING_INDEX + num_visionRegion_weights + num_focusRegion_weights + num_motorRegion_weights*3 + num_nullRegion_weights;*/
	this->genomeSize = GENOME_SIZE;
}

Genome::Genome(const Genome &rhs){
	//this->genes = new double[rhs.genomeSize];
	this->genomeSize = rhs.genomeSize;
	double* rhsGenes = rhs.genes;
	for (double* genePtr = genes; genePtr != genes + genomeSize; ++genePtr){
		*genePtr = *rhsGenes;
		++rhsGenes;
	}
	this->fitness = -99;
	this->visionNet_startIdx = rhs.visionNet_startIdx;
	this->num_focusRegion_weights = rhs.num_focusRegion_weights;
	this->focusRegion_startIdx = rhs.focusRegion_startIdx;
	this->num_visionRegion_weights = rhs.num_visionRegion_weights;
	this->pursuitRegion_startIdx = rhs.pursuitRegion_startIdx;
	this->num_motorRegion_weights = rhs.num_motorRegion_weights;
	this->fleeRegion_startIdx = rhs.fleeRegion_startIdx;
	this->socialRegion_startIdx = rhs.socialRegion_startIdx;
	this->nullRegion_startIdx = rhs.nullRegion_startIdx;
	this->num_nullRegion_weights = rhs.num_nullRegion_weights;
	this->InstantiatePtrs();
}

//Create a new genome based on raw data (note: dimensions must match)
Genome::Genome(vector<double> geneData){
	//genes = new double[geneData.size()];
	double* genePtr = this->genes;
	//for (unsigned int i = 0; i < geneData.size(); ++i)
	//	this->genes.push_back(geneData.at(i));
	for (vector<double>::iterator it = geneData.begin(); it != geneData.end(); ++it){
		*genePtr = *it;
		++genePtr;
	}
	this->fitness = -99;
	this->InstantiateSizes();
	this->InstantiatePtrs();
}

//Create new randomized genome
//Uses definitions at the top of atoms.h
Genome::Genome(){
	int i = 0;
	InstantiateSizes();
	//genes = new double[genomeSize];
	this->InstantiatePtrs();
	//initial colors are always the same:
	genes[0] = 0.61;
	genes[1] = 0.0793;
	genes[2] = 0.0793;

	//size (initially 16):
	genes[3] = 0.2;

	//eye multiplier:
	genes[4] = 0.5;

	//diet index:
	genes[5] = 1.0;

	double* genePtr = visionRegionPtr;
	while (genePtr != focusRegionPtr){
		*genePtr = static_cast<double>(rand() % 10001) / 10000.0;
		++genePtr;
	}


	while (genePtr != pursuitRegionPtr){
		*genePtr = static_cast<double>(rand() % 10001) / 10000.0;
		++genePtr;
	}


	while (genePtr != fleeRegionPtr){
		*genePtr = static_cast<double>(rand() % 10001) / 10000.0;
		++genePtr;
	}



	while (genePtr != socialRegionPtr){
		*genePtr = static_cast<double>(rand() % 10001) / 10000.0;
		++genePtr;
	}



	while (genePtr != nullRegionPtr){
		*genePtr = static_cast<double>(rand() % 10001) / 10000.0;
		++genePtr;
	}


	while (genePtr != endPtr){
		*genePtr = static_cast<double>(rand() % 10001) / 10000.0;
		++genePtr;
	}

}

Genome::~Genome(){
	delete[] genes;
}


/**********************************************************************************/
/*******************************NEARBYOBJ******************************************/
/**********************************************************************************/

NearbyObject::NearbyObject(){
	theObject = NULL;
	theDistance = 0;
	theAngle = 0;
	classifier = null;
}

NearbyObject::NearbyObject(Object* obj, float angle, float dist){
	theObject = obj;
	theDistance = dist;
	theAngle = angle;
	classifier = null;
}

NearbyObject::NearbyObject(const NearbyObject &rhs){
	theObject = rhs.theObject;
	theAngle = rhs.theAngle;
	theDistance = rhs.theDistance;
	classifier = rhs.classifier;
}