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
//    environment.h
//    General information about the environment (objects placed around the map, sizes, object generation, etc)
//
//############################################################################################################//

#include "atoms.h"
#include "food.h"
#include "glutil.h"
#include "automata.h"
#include "linear.h"

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H


//World dimensions
const int WORLD_WIDTH = SCREEN_WIDTH * 2;
const int WORLD_HEIGHT = SCREEN_HEIGHT * 2;

//how many times per second text is rendered (lower: better performance)
const int TEXT_PER_SEC = 4; 

double GeneticDistance(Atom* lhs, Atom* rhs);
bool withinWorldBoundary(int x, int y);
bool locatedOnScreen(int x, int y);

class Environment{
private:
	int static_numAtoms;
	int static_numSugars;
	int static_numHunters;
	int static_numArsenic;
	int environmentalPressure; //drives mutation ratios (TODO)
	unsigned int generation;
	unsigned int deathsThisGeneration;
	int averageFitness;

	vector<Object*> objects;
	//Keep track of bests
	int currBest;
	int currBest2;
	Atom* bestAtom;
	Atom* bestAtom2;
	Genome* bestGenome;
	Genome* bestGenome2;
	int highestFitness;
	int highestFitness2;
	

	//Used for viewing detail upon click
	int frameCounter;//used to determine when to render text (count up from zero and render text when frameCounter==TEXT_PER_SEC)
	Atom* clickAtom;
	Sugar* clickSugar;
	Hunter* clickHunter;

	void Encircle();
	//Add and render n new Atoms to atoms vector using evolutionary operators
	void GenerateNewAtoms(const vector<Genome*> fitPool, int n);
	Genome* Evolve();

	//called from "load" command
	bool LoadEnvironmentFromFile(int numAtoms);


public:
	Environment();
	Environment(int numAtoms, int numSugars, int numArsenic, int numHunters, bool loadFromFile, string fileName);
	~Environment();
	bool Update();
	void ClickSet(float x, float y); //Focus on Atom when clicked on, otherwise clear details from screen
	bool SaveCurrentState(bool autoSave);
	//return species distance (euclidian distance between vectors) between two atoms
};


#endif