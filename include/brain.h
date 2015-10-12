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
//    brain.h
//    General declarations for the "Brain" structure, which is a collection of networked smaller Neural Nets.
//
//############################################################################################################//

#include "neuralnet.h"
#include "atoms.h"

#ifndef BRAIN_H
#define BRAIN_H

/*
* Brain structure:
* 2 types of networks: Classification & Reflex
* 
*  Classification: thresholded neurons, binary outputs
*	Typically represent boolean functions to identify things
*
*  Reflex: Linear function, sigmoid neurons, linear outputs
*		Represents a linear function for reflexes, motor control, etc
*
*/

//variables defining network dimensions/gene map found in atoms.h

/*
* Current brain map:
* Inputs->visionNet->(pursuitRegion/fleeRegion/socialRegion/nullRegion)->Output 
* Inputs:	(0) focus0.isCircle
*			(1) focus0.isSquare
*			(2) focus0.color[0]
*			(3) focus0.color[1]
*			(4) focus0.color[2]
*			(5) focus0.size/thisSize
*			(6) Distance to focus0
*			(7) Angle to focus0
*			(8) focus0 energy/fitness reference (higher => more desirable)
*			(9) focus1.isCircle
*			(10) focus1.isSquare
*			(11) focus1.color[0]
*			(12) focus1.color[1]
*			(13) focus1.color[2]
*			(14) focus1.size/thisSize
*			(15) Distance to focus1
*			(16) Angle to focus1
*			(17) focus1 energy/fitness reference
*			(18) Self angle
*			(19) xposition/world width
*			(20) yposition/world height
*
*
* visionNet: interpret characteristics of object in focus
*			 For more than 1 object in FOV, just pass each through this net
* Inputs:	(0) object.isCircle
*			(1) object.isSquare
*			(2) object.color[0]
*			(3) object.color[1]
*			(4) object.color[2]
*			(5) object.size
*
* Outputs:	(0) focus0.isPredator
*			(1) focus0.isPrey
*			(2) focus0.isFriendly
*
*
* focusNet: Decide which object in FOV to focus on
* Inputs:	(0) focus0.isPredator
*			(1) focus0.isPrey
*			(2) focus0.isFriendly
*			(3) focus0.dist
*			(4) focus0.energy/fitness
*			(5) focus1.isPredator
*			(6) focus1.isPrey
*			(7) focus1.isFriendly
*			(8) focus1.dist
*			(9) focus1.energy/fitness
*
* Outputs:	(0) focus0   *note: if both=0 or both=1, net is "confused", so null region will gain control
*			(1) focus1
*
* pursuitRegion/fleeRegion/socialRegion
* Inputs:   (0) Distance to focus0
*			(1) Angle to object in focus0
*			(2) Self angle
*
* Outputs:  (0) Move direction
*			(1) Move speed
*			(2) Spin direction
*
* nullRegion (nothing in FOV)
* Inputs:   (0) Self Angle
*			(1) xPosition/world width
*			(2) yPosition/world height
*
* Outputs:  (0) Move direction
*			(1) Move speed
*			(2) Spin direction
*
*
*/

class NearbyObject;
class Atom;

class Brain{
private:
	//Classify objects in field of view:
	ClassificationNet* visionNet; 

	//Pick an object in FOV to focus on:
	ClassificationNet* focusNet;

	//Motor control for pursuit region of brain:
	//Controls pursuit of object in focus
	ReflexNet* pursuitRegion;
	
	//Motor control for pursuit region of brain:
	//Controls fleeing from object in focus
	ReflexNet* fleeRegion;

	//Motor control for pursuit region of brain:
	//Control actions when focused on another atom of same species
	ReflexNet* socialRegion;

	//Control actions when not focused on anything in particular
	ReflexNet* nullRegion;

public:


	Brain();
	~Brain();
	//Construct regions based on genome
	Brain(Genome* newGenes);
	
	//build output vector from given input vector, return false in error
	//bool FeedForward(vector<double> inputs, vector<double>* outputs);
	bool FeedForward(double* inputs, double* outputs);

	//Called for manually manipulating information when brain regions are disabled
	//bool FeedForward(const vector<double> inputs, vector<double>* outputs, vector<NearbyObject*> nearbyObj, NearbyObject &focusObj, const Atom* thisAtom);
	bool FeedForward(double* inputs, double* outputs, vector<NearbyObject*> nearbyObj, NearbyObject &focusObj, const Atom* thisAtom);

};



#endif BRAIN_H