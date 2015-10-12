//###########################################################################################################//
//
//  Copyright(c) 2012, Algorithmic Ecology, Anthony Rodriguez (anth.jrod@gmail.com)
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
//    atoms.h
//    General declarations for Genome, Atom and nearbyObject classes
//
//############################################################################################################//


#ifndef ATOMS_H
#define ATOMS_H

#include "glutil.h"
#include "math.h"
#include "linear.h"
#include "neuralnet.h"
#include "food.h"
#include "brain.h"
#include "object.h"
#include <vector>


#define DIET_INDEX 5

//spot in genome where weights begin
//separates nonnegative values
const int WEIGHTS_STARTING_INDEX = 6;

const int ATOM_MAXSIZE = 32;
const int ATOM_MINSIZE = 12;
const float ATOM_MAXSPIN = 0.314f;
const float ATOM_MINSPIN = 0.0523f;
const float ATOM_MAXSPEED = 7.f;
const float ATOM_MINSPEED = 0.f;

const int ATOM_SIZE_SPAN = ATOM_MAXSIZE - ATOM_MINSIZE;
const float ATOM_SPIN_SPAN = ATOM_MAXSPIN - ATOM_MINSPIN;
const float ATOM_SPEED_SPAN = ATOM_MAXSPEED - ATOM_MINSPEED;
const int INITIALENERGY = 20;
const unsigned int INITIALCLOCK = 15000;
const unsigned int LIFESPAN = 120;
const unsigned int INITIALTIMER = 60;

//Brain specifications:
const int NUM_DETECTABLE_OBJECTS = 2;

const int BRAIN_NUMINPUTS = 21;
const int BRAIN_NUMOUTPUTS = 3;

const int VISIONREGION_NUMINPUTS = 6;
const int VISIONREGION_HIDDENUNITS = 9;
const int VISIONREGION_NUMOUTPUTS = 3;
const int VISIONREGION_NUMWEIGHTS = VISIONREGION_NUMOUTPUTS + VISIONREGION_HIDDENUNITS*(VISIONREGION_NUMINPUTS + VISIONREGION_NUMOUTPUTS);

const int FOCUSREGION_NUMINPUTS = 10;
const int FOCUSREGION_HIDDENUNITS = 6;
const int FOCUSREGION_NUMOUTPUTS = 2;
const int FOCUSREGION_NUMWEIGHTS = FOCUSREGION_NUMOUTPUTS + FOCUSREGION_HIDDENUNITS*(FOCUSREGION_NUMINPUTS + FOCUSREGION_NUMOUTPUTS);

const int NUM_MOTORREGIONS = 3;
const int MOTORREGION_NUMINPUTS = 3;
const int MOTORREGION_HIDDENUNITS = 9;
const int MOTORREGION_NUMOUTPUTS = 3;
const int MOTORREGION_NUMWEIGHTS = MOTORREGION_HIDDENUNITS*(MOTORREGION_NUMINPUTS + MOTORREGION_NUMOUTPUTS);

const int NULLREGION_NUMINPUTS = 3;
const int NULLREGION_HIDDENUNITS = 9;
const int NULLREGION_NUMOUTPUTS = 3;
const int NULLREGION_NUMWEIGHTS = NULLREGION_HIDDENUNITS*(NULLREGION_NUMINPUTS + NULLREGION_NUMOUTPUTS);

const int GENOME_SIZE = WEIGHTS_STARTING_INDEX + VISIONREGION_NUMWEIGHTS + FOCUSREGION_NUMWEIGHTS + MOTORREGION_NUMWEIGHTS * NUM_MOTORREGIONS + NULLREGION_NUMWEIGHTS;

class Atom;

typedef enum objClassifications_enum{predator, prey, friendly, null} objClassification;

//Used for classifying nearby objects
class NearbyObject{
public:
	Object* theObject;
	float theDistance;
	float theAngle;
	objClassification classifier; //updated when brain decides what to classify it as

	NearbyObject();
	NearbyObject(Object* obj, float angle, float dist);
	NearbyObject(const NearbyObject &rhs);
};


class Genome{
	/*    Genome vector (make a copy of this in brain.cpp whenever this is updated):
	*begin   |colors(3)|size|eye|diet|vision 0 weights|focus weights|pursuit weights|flee weights|social weights|null weights|   end
	* note: new brain regions can be added but their order in genome CANNOT be modified (without changing mutation functions)
	*/
private:
	void InstantiatePtrs();
	void InstantiateSizes();

public:
	double* genes = new double[GENOME_SIZE];
	int genomeSize;

	double* visionRegionPtr;
	int num_visionRegion_weights; 
	int visionNet_startIdx;

	double* focusRegionPtr;
	int num_focusRegion_weights;
	int focusRegion_startIdx;

	int num_motorRegion_weights;

	double* pursuitRegionPtr;
	int pursuitRegion_startIdx;
	
	double* fleeRegionPtr;
	int fleeRegion_startIdx;

	double* socialRegionPtr;
	int socialRegion_startIdx;

	double* nullRegionPtr;
	int num_nullRegion_weights;//TODO: IMPLEMENT
	int nullRegion_startIdx;

	double* endPtr;

	int fitness;


	Genome(const Genome &rhs); //copy constructor
	
	//Create a new genome based on raw data (note: dimensions must match)
	Genome(vector<double> geneData);


	Genome(); //Create random Genome based upon global dimensions
	~Genome();

	friend class Environment;
	friend class Atom;
};


class Atom: public Object{
private:
	int size;
	int halfSize; //calculate in constructor since it is used often
	int clock;
	int timer;
	int age;
	int metabolismRate; //how many clocks are taken per tick(depends on size)
	float direction; //direction in rads
	float eyeRadius;
	float spinSpeed;
	float moveSpeed; //default
	float currMoveSpeed; //move speed comm
	float fieldOfView;
	float visionDist;
	//vector<double> brainInputs;
	//vector<double>* brainOutputs = new vector<double>;
	double* brainInputs = new double[BRAIN_NUMINPUTS];
	double* brainOutputs = new double[BRAIN_NUMOUTPUTS];
	//Keep track of fitnesses of children:
	vector<Genome*> children;
	Brain* brain;
	Object* nearestObject;
	NearbyObject* focusObject;


	void FindFOVSugar(); //build vector of Sugar objects within FOV
	void Action(vector<Object*>* allObjects);


public: 
	int numChildren;
	bool alive;
	bool canReproduce;
	Genome* genome;
	Atom();
	Atom(const Genome &rhsGenome); //create a clone Atom
	Atom(const Genome &rhsGenome, float x, float y);
	Atom(float x, float y); //Create Atom with randomized ANN
	~Atom();

	//Find single closest object (getting dated: used when focusNet is disabled
	//bool FindClosestObject(vector<Object*> allObjects);
	//Build a vector "closeObjects" of the n closest objects
	bool FindClosestObjects(const vector<Object*> allObjects, vector<NearbyObject*>* nearbyObjects, int n); 
	bool Update(vector<Object*>* allObjects);
	bool UpdatePosition(float x, float y);
	void Render();
	void Render3D();

	/********NETWORK INPUT MAP*************/
	//distance and angle to nearest object
	float dist1;
	float theta1;

	/*******NETWORK OUTPUT MAP*************/
	//Output 0:
	float spin;
	void Spin(float direction); // -1 < direction <1
	//Output 1:
	float move;
	void Move(float direction); // -2pi < direction < 2pi

	friend class Environment;
	friend class Hunter;
};


#endif