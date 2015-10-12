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
//    environment.cpp
//    Functionality for generating and displaying objects in the world
//
//############################################################################################################//


#include "environment.h"
#include "time.h"
#include "evolution.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

using namespace std;

//Note: Evolutionary operator probabilities specified in evolution.h

#define PI 3.1415926f
#define TEXTRESOLUTION 60/TEXT_PER_SEC
#define DATAFILE_FORMAT_VERSION 5 

vec2* SCREEN_POSITION = NULL;

extern SPEED_MODE SPEEDMODE;
extern bool loadFromFile;
extern string fileName;
extern double GENETIC_DISTANCE_THRESHOLD;
extern bool POPULATION_PRESERVATION_ENABLED;



bool withinWorldBoundary(int x, int y){
	if (x < WORLD_WIDTH && y < WORLD_HEIGHT)
		return true;
	else return false;
}

bool locatedOnScreen(int x, int y){
	int x_min = SCREEN_POSITION->x;
	int x_max = x_min + SCREEN_WIDTH;
	int y_min = SCREEN_POSITION->y;
	int y_max = y_min + SCREEN_HEIGHT;
	if (x >= x_min && x <= x_max && y >= y_min && y <= y_max)
		return true;
	else return false;
}

double GeneticDistance(Atom* lhs, Atom* rhs){
	Genome* genome1 = lhs->genome;
	Genome* genome2 = rhs->genome;
	double ans = 0;
	int gSize = genome1->genomeSize;
	double thisDist = 0;

	//Mismatched sizes have max distance (1)
	if (genome2==NULL || gSize != genome2->genomeSize)
		return 99.0;

	//for (int i = 0; i < gSize; ++i){
	//	thisDist = genome1->genes.at(i) - genome2->genes.at(i);
	//	thisDist = thisDist*thisDist;
	//	ans = ans + thisDist;
	//}

	double* lhsPtr = genome1->genes;
	double* rhsPtr = genome2->genes;
	double* lhsEndPtr = lhsPtr + gSize;
	while (lhsPtr < lhsEndPtr){
		thisDist = *lhsPtr - *rhsPtr;
		thisDist = thisDist*thisDist;
		ans = ans + thisDist;
		++lhsPtr;
		++rhsPtr;
	}

	return sqrt(thisDist);
}

//TODO: Currently there is no mechanism for reconfiguring genomes for discrepancies between file dimensions and initialized ANN dimensions in atoms.h
//TODO: Figure out how to re-initialize istringstreams so that we don't have 20+ istringstreams being created (ex: ls1, ls2, ...)
bool Environment::LoadEnvironmentFromFile(int numAtoms){
	bool quitParseEarly = false;
	ifstream inFS;
	string inputBuffer;
	string junkBuffer;
	vector<double> inData;
	double thisValue = 0;
	Genome* thisGenome = NULL;
	Genome* fittestGenome = NULL;
	Genome* newGenome = NULL;
	int populationSize = 0;
	int visionInputs = 0;
	int visionHidden = 0;
	int visionOutputs = 0;
	int visionWeights = 0;
	int visionStart = 0;
	int motorInputs = 0;
	int motorHidden = 0;
	int motorOutputs = 0;
	int motorWeights = 0;
	int pursuitStart = 0;
	int fleeStart = 0;
	int socialStart = 0;
	int nullInputs = 0;
	int nullHidden = 0;
	int nullOutputs = 0;
	int nullWeights = 0;
	int nullStart = 0;


	inFS.open(fileName);
	if (!inFS.is_open()){
		cout << "Environment Error: Could not open file " << fileName << endl;
		return false;
	}

	//Read file version
	getline(inFS, inputBuffer);
	if (!inputBuffer.compare(0, 15, "FORMAT_VERSION:") == 0){
		cout << "Parse error: Could not find FORMAT_VERSION argument" << endl;
		return false;
	}
	else{
		istringstream linestream(inputBuffer);
		linestream >> junkBuffer >> thisValue;
		if (thisValue != DATAFILE_FORMAT_VERSION){
			cout << "Warning: Detected out-of-date file format.\nData loaded to environment has a high probability of corruption." << endl;
		}
	}

	//Read population size
	getline(inFS, inputBuffer);
	if (!inputBuffer.compare(0, 16, "POPULATION_SIZE:") == 0){
		cout << "Parse error: Could not find POPULATION_SIZE argument" << endl;
		return false;
	}
	else{
		istringstream linestream(inputBuffer);
		linestream >> junkBuffer >> thisValue;
		if (thisValue <= 2){
			cout << "Parse error: POPULATION_SIZE out of range" << endl;
			return false;
		}
		else
			populationSize = static_cast<int>(thisValue);
	}

	//Read vision region inputs
	getline(inFS, inputBuffer);
	istringstream ls1(inputBuffer);
	ls1 >> junkBuffer >> thisValue;
	if (ls1.fail() || junkBuffer != "VISIONREGION_NUMINPUTS:"){
		cout << "Parse error: VISIONREGION_NUMINPUTS" << endl;
		return false;
	}
	else
		visionInputs = static_cast<int>(thisValue);

	//Read vision region hidden units
	getline(inFS, inputBuffer);
	istringstream ls2(inputBuffer);
	ls2 >> junkBuffer >> thisValue;
	if (ls2.fail() || junkBuffer != "VISIONREGION_HIDDENUNITS:"){
		cout << "Parse error: VISIONREGION_HIDDENUNITS" << endl;
		return false;
	}
	else
		visionHidden = static_cast<int>(thisValue);

	//Read vision region outputs
	getline(inFS, inputBuffer);
	istringstream ls3(inputBuffer);
	ls3 >> junkBuffer >> thisValue;
	if (ls3.fail() || junkBuffer != "VISIONREGION_NUMOUTPUTS:"){
		cout << "Parse error: VISIONREGION_NUMOUTPUTS" << endl;
		return false;
	}
	else
		visionOutputs = static_cast<int>(thisValue);

	//Read vision region num weights
	getline(inFS, inputBuffer);
	istringstream ls4(inputBuffer);
	ls4 >> junkBuffer >> thisValue;
	if (ls4.fail() || junkBuffer != "VISIONREGION_NUMWEIGHTS:"){
		cout << "Parse error: VISIONREGION_NUMWEIGHTS" << endl;
		return false;
	}
	else
		visionWeights = static_cast<int>(thisValue);

	//Read vision region start idx
	getline(inFS, inputBuffer);
	istringstream ls5(inputBuffer);
	ls5 >> junkBuffer >> thisValue;
	if (ls5.fail() || junkBuffer != "VISIONREGION_STARTINDEX:"){
		cout << "Parse error: VISIONREGION_STARTINDEX" << endl;
		return false;
	}
	else
		visionStart = static_cast<int>(thisValue);

	//Read motor region inputs
	getline(inFS, inputBuffer);
	istringstream ls6(inputBuffer);
	ls6 >> junkBuffer >> thisValue;
	if (ls6.fail() || junkBuffer!="MOTORREGIONS_NUMINPUTS:"){
		cout << "Parse error: MOTORREGIONS_NUMINPUTS" << endl;
		return false;
	}
	else
		motorInputs = static_cast<int>(thisValue);

	//Read motor region hidden units
	getline(inFS, inputBuffer);
	istringstream ls7(inputBuffer);
	ls7 >> junkBuffer >> thisValue;
	if (ls7.fail() || junkBuffer != "MOTORREGIONS_HIDDENUNITS:"){
		cout << "Parse error: MOTORREGIONS_HIDDENUNITS" << endl;
		return false;
	}
	else
		motorHidden = static_cast<int>(thisValue);

	//Read motor region outputs
	getline(inFS, inputBuffer);
	istringstream ls8(inputBuffer);
	ls8 >> junkBuffer >> thisValue;
	if (ls8.fail() || junkBuffer != "MOTORREGIONS_NUMOUTPUTS:"){
		cout << "Parse error: MOTORREGIONS_NUMOUTPUTS" << endl;
		return false;
	}
	else
		motorOutputs = static_cast<int>(thisValue);

	//Read motor region num weights
	getline(inFS, inputBuffer);
	istringstream ls9(inputBuffer);
	ls9 >> junkBuffer >> thisValue;
	if (ls9.fail() || junkBuffer != "MOTORREGION_NUMWEIGHTS:"){
		cout << "Parse error: MOTORREGION_NUMWEIGHTS" << endl;
		return false;
	}
	else
		motorWeights = static_cast<int>(thisValue);

	//Read pursuit region start idx
	getline(inFS, inputBuffer);
	istringstream ls10(inputBuffer);
	ls10 >> junkBuffer >> thisValue;
	if (ls10.fail() || junkBuffer != "PURSUITREGION_STARTINDEX:"){
		cout << "Parse error: PURSUITREGION_STARTINDEX" << endl;
		return false;
	}
	else
		pursuitStart = static_cast<int>(thisValue);

	//Read flee region start idx
	getline(inFS, inputBuffer);
	istringstream ls11(inputBuffer);
	ls11 >> junkBuffer >> thisValue;
	if (ls11.fail() || junkBuffer != "FLEEREGION_STARTINDEX:"){
		cout << "Parse error: FLEEREGION_STARTINDEX" << endl;
		return false;
	}
	else
		fleeStart = static_cast<int>(thisValue);

	//Read social region start idx
	getline(inFS, inputBuffer);
	istringstream ls12(inputBuffer);
	ls12 >> junkBuffer >> thisValue;
	if (ls12.fail() || junkBuffer != "SOCIALREGION_STARTINDEX:"){
		cout << "Parse error: SOCIALREGION_STARTINDEX" << endl;
		return false;
	}
	else
		socialStart = static_cast<int>(thisValue);

	//Read null region inputs
	getline(inFS, inputBuffer);
	istringstream ls13(inputBuffer);
	ls13 >> junkBuffer >> thisValue;
	if (ls13.fail() || junkBuffer != "NULLREGION_NUMINPUTS:"){
		cout << "Parse error: NULLREGION_NUMINPUTS" << endl;
		return false;
	}
	else
		nullInputs = static_cast<int>(thisValue);

	//Read null region hidden
	getline(inFS, inputBuffer);
	istringstream ls14(inputBuffer);
	ls14 >> junkBuffer >> thisValue;
	if (ls14.fail() || junkBuffer != "NULLREGION_HIDDENUNITS:"){
		cout << "Parse error: NULLREGION_HIDDENUNITS" << endl;
		return false;
	}
	else
		nullHidden = static_cast<int>(thisValue);
	
	//Read null region outputs
	getline(inFS, inputBuffer);
	istringstream ls15(inputBuffer);
	ls15 >> junkBuffer >> thisValue;
	if (ls15.fail() || junkBuffer != "NULLREGION_NUMOUTPUTS:"){
		cout << "Parse error: NULLREGION_NUMOUTPUTS" << endl;
		return false;
	}
	else
		nullOutputs = static_cast<int>(thisValue);

	//Read null region numweights
	getline(inFS, inputBuffer);
	istringstream ls16(inputBuffer);
	ls16 >> junkBuffer >> thisValue;
	if (ls16.fail() || junkBuffer != "NULLREGION_NUMWEIGHTS:"){
		cout << "Parse error: NULLREGION_NUMWEIGHTS" << endl;
		return false;
	}
	else
		nullWeights = static_cast<int>(thisValue);

	//Read null region startidx
	getline(inFS, inputBuffer);
	istringstream ls17(inputBuffer);
	ls17 >> junkBuffer >> thisValue;
	if (ls17.fail() || junkBuffer != "NULLREGION_STARTINDEX:"){
		cout << "Parse error: NULLREGION_STARTINDEX" << endl;
		return false;
	}
	else
		nullStart = static_cast<int>(thisValue);

	while (inFS.good()){
		getline(inFS, inputBuffer);
		if (!inFS.bad()){
			//read best atom
			if (inputBuffer.compare(0, 10, "BEST_ATOM:") == 0){
				istringstream linestream(inputBuffer);
				junkBuffer = inputBuffer;
				while (!inFS.bad()){
					getline(inFS, inputBuffer);
					if (inputBuffer.compare(0, 16, "POPULATION_BEGIN") == 0)
						break;
					istringstream ls8(inputBuffer);
					ls8 >> thisValue;
					inData.push_back(thisValue);
				}
				if (inFS.bad()){
					cout << "Parse error: could not locate POPULATION_BEGIN tag" << endl;
					return false;
				}
				else{
					fittestGenome = new Genome(inData);
					inData.clear();
					objects.push_back(new Atom(*fittestGenome));
					((Atom*)objects.back())->energy = 100;
					this->bestAtom = (Atom*)objects.at(0);
				}
			}

			//Read population members
			else if (inputBuffer.compare(0, 12, "GENOME_BEGIN")==0){
				istringstream linestream(inputBuffer);
				junkBuffer = inputBuffer;
				while (!inFS.bad()){
					getline(inFS, inputBuffer);
					if (inputBuffer.compare(0, 10, "GENOME_END") == 0)
						break;
					istringstream ls8(inputBuffer);
					ls8 >> thisValue;
					inData.push_back(thisValue);
				}
				if (inFS.bad()){
					cout << "Parse error: could not locate GENOME_END tag" << endl;
					return false;
				}
				else{
					thisGenome = new Genome(inData);
					inData.clear();
					if (objects.size() == numAtoms){
						quitParseEarly = true;
						break;
					}
					objects.push_back(new Atom(*thisGenome));
					((Atom*)objects.back())->energy = 100;
					this->bestAtom = (Atom*)objects.at(0);
					delete thisGenome;
				}
			}
			else if (inputBuffer.compare(0, 14, "POPULATION_END")==0)
				break;
		}
		else break;
	}

	if (populationSize != objects.size() && !quitParseEarly){
		cout << "Parse warning: Declared population: " << populationSize << ". Parsed objects: " << objects.size() << endl;
		if (populationSize > objects.size()){
			cout << "The AE environment will fill empty Atoms with mutated children of fittest genome" << endl;
			for (int i = objects.size()-1; i < populationSize; ++i){
				newGenome = new Genome(*fittestGenome);
				StepMutateRefactor(newGenome, GENE_MUTATION_PROBABILITY);
				objects.push_back(new Atom(*newGenome));
				((Atom*)objects.back())->energy = 100;
				this->bestAtom = (Atom*)objects.at(0);
				delete newGenome;
			}
			delete fittestGenome;
		}
		else
			return false;
	}
	if (numAtoms > objects.size()){
		cout << "Parse warning: Set population: " << numAtoms << ". Parsed objects: " << objects.size() << endl;

		cout << "The AE environment will fill empty Atoms with mutated children of fittest genome" << endl;
		for (int i = objects.size() - 1; i < numAtoms; ++i){
			newGenome = new Genome(*fittestGenome);
			StepMutateRefactor(newGenome, GENE_MUTATION_PROBABILITY);
			objects.push_back(new Atom(*newGenome));
			((Atom*)objects.back())->energy = 100;
			delete newGenome;
		}
		delete fittestGenome;

	}



	for (int i = 0; i < static_numSugars; ++i){
		objects.push_back(new Sugar());
	}
	for (int i = 0; i < static_numHunters; ++i){
		objects.push_back(new Hunter());
	}
	for (int i = 0; i < static_numArsenic; ++i){
		objects.push_back(new Arsenic());
	}
	//delete fittestGenome;

	return true;
}

Environment::Environment(int numAtoms, int numSugars, int numArsenic, int numHunters, bool loadFromFile, string fileName){
	static_numAtoms = numAtoms;
	static_numSugars = numSugars;
	static_numHunters = numHunters;
	static_numArsenic = numArsenic;
	frameCounter = 0;
	generation = 1;
	deathsThisGeneration = 0;
	highestFitness = 0;
	srand(time(NULL));
	clickAtom = NULL;
	bestGenome2 = NULL;
	bestAtom2 = NULL;
	highestFitness2 = 0;

	if (loadFromFile){
		if (!LoadEnvironmentFromFile(numAtoms))
			cout << "Error: Incorrect save file format. Strongly urge application abort" << endl;
	}
	else{
		int i = 0;
		float xpos = 0;
		float ypos = 0;
		for (int i = 0; i < numAtoms; ++i){
			objects.push_back(new Atom());
		}
		Atom* tempAtom = (Atom*)objects.at(0);
		Genome* tempGenome = new Genome(*tempAtom->genome);
		bestGenome = tempGenome;
		for (int i = 0; i < numSugars; ++i){
			objects.push_back(new Sugar());
		}
		for (int i = 0; i < numHunters; ++i){
			objects.push_back(new Hunter());
		}
		for (int i = 0; i < numArsenic; ++i){
			objects.push_back(new Arsenic());
		}
	}
}

Environment::~Environment(){
	while (!objects.empty()){
		Object* thisObject = objects.back();
		objects.pop_back();
		delete thisObject;
	}
	delete bestGenome;
}

//Draw circle around clicked object
void Environment::Encircle(){
	if (SPEEDMODE == Warp)
		return;
	float theta = 0;
	float xtemp = 0;
	int i = 0;
	float ytemp = 0;
	glBegin(GL_LINE_LOOP);
	glColor3f(0.049f, 0.7f, 0.5264f);
	for (i = 0; i < 24; ++i){
		theta = 2.f*3.141*static_cast<float>(i) / 24.f;
		xtemp = 20*cosf(theta);
		ytemp = 20*sinf(theta);
		glVertex2f(clickAtom->position->x-SCREEN_POSITION->x + xtemp, clickAtom->position->y-SCREEN_POSITION->y + ytemp);
	}
	glEnd();
}

void Environment::GenerateNewAtoms(const vector<Genome*> fitPool, int n){
	int r;
	Genome* genePtr = NULL;
	if (n <= 0)
		return;
	for (int i = 0; i < n; ++i){
		if (fitPool.size() == 0){
			genePtr = new Genome(*bestGenome);
			MutateRefactor(genePtr, 100);
		}
		else{
			r = rand() % fitPool.size();
			genePtr = new Genome(*fitPool.at(r));
			r = rand() % 101;
			if (r <= 90){
				r = rand() % fitPool.size();
				CrossOver(genePtr, *fitPool.at(r));
			}
			r = rand() % 101;
			if (r <= 50)
				MutateRefactor(genePtr, 30);
			else if (r <= 80)
				StepMutateRefactor(genePtr, 30);
		}

		objects.push_back(new Atom(*genePtr));
		((Atom*)objects.back())->Render3D();
		delete genePtr;
	}
}

bool Environment::Update(){
	unsigned int i = 0;
	float theta = 0;
	float xtemp = 0;
	float ytemp = 0;
	int fitnessCounter = 0;
	int fitCount = 0;
	int deathsThisIteration = 0;
	Atom* tempAtom = NULL;
	Sugar* tempSugar = NULL;
	Hunter* tempHunter = NULL;
	Arsenic* tempArsenic = NULL;
	vector<Genome*> fitGenes;
	currBest = -99;
	currBest2 = -99;

	//Loop through atom list, delete dead ones, build fit genes vector
	for (i = 0; i < objects.size(); ++i){
		if (objects.at(i)->isAtom){
			tempAtom = (Atom*)objects.at(i);
			if (!tempAtom->Update(&objects))
				return false;
			if (tempAtom->genome->fitness > 10){
				fitnessCounter = fitnessCounter + tempAtom->genome->fitness;
				++fitCount;
			}
			//Build gene pool for new births
			//Explicitly: Create clone genome in new Genome vector
			if (tempAtom->genome->fitness >= averageFitness)
				fitGenes.push_back(new Genome(*tempAtom->genome));
			//Handle dead agents:
			if (!tempAtom->alive){
				++deathsThisGeneration;
				if (deathsThisGeneration >= static_numAtoms){
					deathsThisGeneration = 0;
					++generation;
					if (generation % 10 == 0)
						SaveCurrentState(true);
				}
				if (tempAtom == clickAtom)
					clickAtom = NULL;
				//if fittest, reincarnate it to make sure it stays in gene pool
				if (tempAtom->genome->fitness==highestFitness || tempAtom->genome->fitness==highestFitness2){
					Atom* newAtom = new Atom(*bestGenome);
					highestFitness = -99; //some absurd number
					Atom* oldAtom = (Atom*)objects.at(i);
					objects.at(i) = newAtom;
					delete oldAtom;
				}
				//if not, keep track of death so that we can generate a new Atom later
				else{
					++deathsThisIteration;
					Atom* badAtom = (Atom*)objects.at(i);
					objects.erase(objects.begin() + i);
					delete badAtom;
					--i;
				}
			}
			else if (tempAtom->genome->fitness >= currBest){
				if (currBest2>-99 && GeneticDistance(tempAtom, bestAtom2) <= GENETIC_DISTANCE_THRESHOLD){
					bestAtom2 = bestAtom;
					currBest2 = currBest;
				}
				bestAtom = tempAtom;
				currBest = tempAtom->genome->fitness;
				Atom* currAtom = ((Atom*)objects.at(i));
				if (locatedOnScreen(currAtom->position->x, currAtom->position->y))
					currAtom->Render3D();
			}
			else if (tempAtom->genome->fitness >= currBest2 && GeneticDistance(tempAtom, bestAtom) > GENETIC_DISTANCE_THRESHOLD){
				bestAtom2 = tempAtom;
				currBest2 = tempAtom->genome->fitness;
				Atom* currAtom = ((Atom*)objects.at(i));
				if (locatedOnScreen(currAtom->position->x, currAtom->position->y))
					currAtom->Render3D();
			}
			else{
				Atom* currAtom = ((Atom*)objects.at(i));
				if (locatedOnScreen(currAtom->position->x, currAtom->position->y))
					currAtom->Render3D();
			}
		} 
		//else current object is a sugar, so update it
		else if (objects.at(i)->isSugar){
			tempSugar = (Sugar*)objects.at(i);
			//Delete sugar and replace with a new one if it has been eaten
			if (tempSugar->Update() == false){
				Sugar* oldSugar = tempSugar;
				Sugar* newSugar = new Sugar();
				objects.at(i) = newSugar;
				delete oldSugar;
			}
			Sugar* currSugar = ((Sugar*)objects.at(i));
			if (locatedOnScreen(currSugar->position->x, currSugar->position->y))
				currSugar->Render();
		}
		else if (objects.at(i)->isHunter){
			tempHunter = (Hunter*)objects.at(i);
			tempHunter->Update(objects);
			if (locatedOnScreen(tempHunter->position->x, tempHunter->position->y))
				tempHunter->Render3D();
		}
		else if (objects.at(i)->isArsenic){
			tempArsenic = (Arsenic*)objects.at(i);
			//Delete sugar and replace with a new one if it has been eaten
			if (tempArsenic->Update() == false){
				Arsenic* oldArsenic = tempArsenic;
				Arsenic* newArsenic = new Arsenic();
				objects.at(i) = newArsenic;
				delete oldArsenic;
			}
			((Arsenic*)objects.at(i))->Render();
		}
	}
	//Generate and render new atoms:
	if (POPULATION_PRESERVATION_ENABLED){
		//int r = rand() % 11;
		int objectCap = static_numAtoms + static_numHunters + static_numSugars + static_numArsenic;
		/*while (objects.size() < objectCap){
			Genome* newGenes;
			if (r <= 5 || bestGenome2 == NULL || highestFitness2 < 10)
				newGenes = new Genome(*bestGenome);
			else
				newGenes = new Genome(*bestGenome2);
			StepMutateRefactor(newGenes, 30);
			objects.push_back(new Atom(*newGenes));
		}*/
		if (objects.size() < objectCap){
			int objectDeficiency = objectCap - objects.size();
			GenerateNewAtoms(fitGenes, objectDeficiency);
		}
	}

	//delete fitness gene pool since we don't need it anymore:
	while (!fitGenes.empty()){
		Genome* thisGenome = fitGenes.back();
		fitGenes.pop_back();
		delete thisGenome;
	}


	//We found fittest atom above, so update environment to reflect that:
	Genome* tempGenome = bestGenome;
	bestGenome = new Genome(*bestAtom->genome);
	highestFitness = currBest;
	highestFitness2 = currBest2;
	if (highestFitness2 > 0)
		bestGenome2 = new Genome(*bestAtom2->genome);
	delete tempGenome;
	if (SPEEDMODE != Warp){
		if (locatedOnScreen(bestAtom->position->x, bestAtom->position->y)){
			glLoadIdentity();
			glBegin(GL_TRIANGLE_FAN);
			glColor3f(0.049f, 0.7f, 0.5624f);
			for (int j = 0; j < 24; ++j){
				theta = 2.f*PI*static_cast<float>(j) / 24.f;
				xtemp = bestAtom->size*cosf(theta);
				ytemp = bestAtom->size*sinf(theta);
				glVertex2f(bestAtom->position->x - SCREEN_POSITION->x + xtemp, bestAtom->position->y - SCREEN_POSITION->y + ytemp);
			}
			glEnd();
			bestAtom->Render3D(); //re-render over our identifying circle
		}
		if (currBest2>-99 && locatedOnScreen(bestAtom2->position->x, bestAtom2->position->y)){
			glLoadIdentity();
			glBegin(GL_TRIANGLE_FAN);
			glColor3f(0.049f, 0.7f, 0.5624f);
			for (int j = 0; j < 24; ++j){
				theta = 2.f*PI*static_cast<float>(j) / 24.f;
				xtemp = bestAtom2->size*cosf(theta);
				ytemp = bestAtom2->size*sinf(theta);
				glVertex2f(bestAtom2->position->x - SCREEN_POSITION->x + xtemp, bestAtom2->position->y - SCREEN_POSITION->y + ytemp);
			}
			glEnd();
			bestAtom2->Render3D(); //re-render over our identifying circle
		}
	}
	
	//Calculate fitness thresholds for reproduction
	if (fitCount == 0)
		averageFitness = highestFitness;
	else
		averageFitness = fitnessCounter / fitCount;


	if (SPEEDMODE != Warp){
		++frameCounter;
		if (bestGenome != NULL){
			glColor3f(0.049f, 0.7f, 0.5264f);
			printw(760, 20, 0, "Generation:   %u", generation);
			printw(760, 30, 0, "Best Fitness: %d", highestFitness);
			printw(760, 40, 0, "Best Species2: %d", highestFitness2);
			printw(760, 50, 0, "Population size: %d", objects.size()-static_numSugars-static_numHunters-static_numArsenic);
		}

		if (frameCounter >= TEXTRESOLUTION){
			if (clickAtom != NULL){
				Encircle();
				glColor3f(0.049f, 0.7f, 0.5264f);
				printw(20, 20, 0, "Fitness:        %d", clickAtom->genome->fitness);
				printw(20, 30, 0, "Age      :      %d", clickAtom->age);
				printw(20, 40, 0, "Energy   :      %d", clickAtom->energy);
				printw(20, 50, 0, "FOV      :      %.1f deg", clickAtom->fieldOfView*57.2957795);
				printw(20, 60, 0, "Vis dist :      %.1f", clickAtom->visionDist);
				if (clickAtom->focusObject==NULL)
					printw(20, 70, 0, "Focus: NULL");
				else if (clickAtom->focusObject->classifier == null)
					printw(20, 70, 0, "Focus: Null");
				else if (clickAtom->focusObject->classifier == prey)
					printw(20, 70, 0, "Focus: Prey");
				else if (clickAtom->focusObject->classifier == friendly)
					printw(20, 70, 0, "Focus: Friend");
				else if (clickAtom->focusObject->classifier == predator)
					printw(20, 70, 0, "Focus: Predator");
				if (!clickAtom->focusObject == NULL){
					printw(20, 80, 0, "Focus dist:    %.2f", clickAtom->focusObject->theDistance);
					printw(20, 90, 0, "Focus angle:   %.2f deg", clickAtom->focusObject->theAngle*57.2957795);
				}
				if (clickAtom->canReproduce)
					printw(20, 100, 0, "Ready for reproduction");
				else
					printw(20, 100, 0, "Not ready for reproduction");
				if (clickAtom->focusObject != NULL && clickAtom->focusObject->theObject != NULL && clickAtom->focusObject->theObject->position != NULL && clickAtom->focusObject->classifier == friendly)
					printw(20, 110, 0, "Genetic Distance:      %.4f", GeneticDistance(clickAtom, ((Atom*)(clickAtom->focusObject->theObject))));
				else
					printw(20, 110, 0, "Genetic Distance:      NA");
				printw(20, 120, 0, "Clks/tick :      %d", (int)((float)clickAtom->metabolismRate*clickAtom->currMoveSpeed));
				printw(20, 130, 0, "Burn rate:      %.4f/sec", (clickAtom->metabolismRate*clickAtom->currMoveSpeed * 2) / 1000);
				printw(20, 140, 0, "MoveSpeed:     %.2f percent", clickAtom->currMoveSpeed*100);
				printw(20, 150, 0, "Children:      %d", clickAtom->numChildren);
				if (clickAtom->genome->genes[DIET_INDEX]<-1.5)
					printw(20, 160, 0, "Arsenic Carnivore");
				else if (clickAtom->genome->genes[DIET_INDEX]<-0.5)
					printw(20, 160, 0, "Arsenic Omnivore");
				else if (clickAtom->genome->genes[DIET_INDEX]<0)
					printw(20, 160, 0, "Arsenic Herbivore");
				else if (clickAtom->genome->genes[DIET_INDEX]>1.5)
					printw(20, 160, 0, "Sugar Carnivore");
				else if (clickAtom->genome->genes[DIET_INDEX]>0.5)
					printw(20, 160, 0, "Sugar Omnivore");
				else if (clickAtom->genome->genes[DIET_INDEX]>0)
					printw(20, 160, 0, "Sugar Herbivore");
			}
			else if (clickSugar != NULL){
				glColor3f(0.049f, 0.7f, 0.5264f);
				printw(20, 20, 0, "Energy   :      %d", clickSugar->energy);
			}
			else if (clickHunter != NULL){
				glColor3f(0.049f, 0.7f, 0.5264f);
				printw(20, 20, 0, "Energy   :      %d", clickHunter->energy);
				printw(20, 30, 0, "Speed    :      %.4f", clickHunter->moveSpeed);
			}
		}
	}
	return true;
}

//Called when there is a mouse leftclick detected at x,y to check for user selections
void Environment::ClickSet(float x, float y){
	vec2* clickSpot = new vec2(x, y);
	float currDist=100;
	//search for object to focus on
	for (int i = 0; i < objects.size(); ++i){
		vec2* absPosition = new vec2(objects.at(i)->position->x - SCREEN_POSITION->x, objects.at(i)->position->y - SCREEN_POSITION->y);
		currDist = Distance(clickSpot, absPosition);
		if (currDist < 20){
			if (objects.at(i)->isAtom){
				clickAtom = (Atom*)objects.at(i);
				clickSugar = NULL;
				clickHunter = NULL;
			}
			else if (objects.at(i)->isSugar){
				clickSugar = (Sugar*)objects.at(i);
				clickAtom = NULL;
				clickHunter = NULL;
			}
			else if (objects.at(i)->isHunter){
				clickHunter = (Hunter*)objects.at(i);
				clickAtom = NULL;
				clickSugar = NULL;
			}
			delete clickSpot;
			return;
		}
		delete absPosition;
	}


	//No clicked objects found
	delete clickSpot;
	clickAtom = NULL;
	clickSugar = NULL;
	clickHunter = NULL;
}

bool Environment::SaveCurrentState(bool autoSave){
	ofstream outFS;
	time_t t = time(0);
	struct tm now;
	localtime_s(&now, &t);
	stringstream strLabel;
	//construct file name:
	strLabel << (now.tm_year + 1900) << "-" << (now.tm_mon + 1) << '-' << now.tm_mday << "_" << now.tm_hour << "-" << now.tm_min << ".dat";
	string fileName;
	if (autoSave)
		fileName = "session-autosave.dat";
	else
		fileName = strLabel.str();
	outFS.open(fileName);
	if (!outFS.good()){
		cout << "Could not open file " << fileName << endl;
		return false;
	}
	int n = 0;
	for (int i = 0; i < this->objects.size(); ++i){
		if (objects.at(i)->isAtom){
			++n;
		}
	}

	outFS << "FORMAT_VERSION: " << DATAFILE_FORMAT_VERSION << endl; //used to check for readability of file (update this #define at top whenever changes are made to this format)
	outFS << "POPULATION_SIZE: " << n << endl;
	outFS << "VISIONREGION_NUMINPUTS: " << VISIONREGION_NUMINPUTS << endl;
	outFS << "VISIONREGION_HIDDENUNITS: " << VISIONREGION_HIDDENUNITS << endl;
	outFS << "VISIONREGION_NUMOUTPUTS: " << VISIONREGION_NUMOUTPUTS << endl;
	outFS << "VISIONREGION_NUMWEIGHTS: " << bestAtom->genome->num_visionRegion_weights << endl;
	outFS << "VISIONREGION_STARTINDEX: " << bestAtom->genome->visionNet_startIdx << endl;
	outFS << "MOTORREGIONS_NUMINPUTS: " << MOTORREGION_NUMINPUTS << endl;
	outFS << "MOTORREGIONS_HIDDENUNITS: " << MOTORREGION_HIDDENUNITS << endl;
	outFS << "MOTORREGIONS_NUMOUTPUTS: " << MOTORREGION_NUMOUTPUTS << endl;
	outFS << "MOTORREGION_NUMWEIGHTS: " << bestAtom->genome->num_motorRegion_weights << endl;
	outFS << "PURSUITREGION_STARTINDEX: " << bestAtom->genome->pursuitRegion_startIdx << endl;
	outFS << "FLEEREGION_STARTINDEX: " << bestAtom->genome->fleeRegion_startIdx << endl;
	outFS << "SOCIALREGION_STARTINDEX: " << bestAtom->genome->socialRegion_startIdx << endl;
	outFS << "NULLREGION_NUMINPUTS: " << NULLREGION_NUMINPUTS << endl;
	outFS << "NULLREGION_HIDDENUNITS: " << NULLREGION_HIDDENUNITS << endl;
	outFS << "NULLREGION_NUMOUTPUTS: " << NULLREGION_NUMOUTPUTS << endl;
	outFS << "NULLREGION_NUMWEIGHTS: " << bestAtom->genome->num_nullRegion_weights << endl;
	outFS << "NULLREGION_STARTINDEX: " << bestAtom->genome->nullRegion_startIdx << endl;
	outFS << endl;

	//print best atom
	outFS << "BEST_ATOM:" << endl;
	//for (int i = 0; i < bestAtom->genome->genes.size(); ++i)
	//	outFS << std::fixed << std::setprecision(8) << bestAtom->genome->genes.at(i) << endl;
	for (double* genePtr = bestAtom->genome->genes; genePtr != bestAtom->genome->genes + bestAtom->genome->genomeSize; ++genePtr)
		outFS << std::fixed << std::setprecision(8) << *genePtr << endl;

	outFS << "POPULATION_BEGIN" <<  endl;

	//print all atoms in population except for bestAtom
	for (int i = 0; i < this->objects.size(); ++i){
		if (objects.at(i)->isAtom && objects.at(i)!=bestAtom){
			outFS << "GENOME_BEGIN" << endl;
			Genome* thisGenome = ((Atom*)objects.at(i))->genome; 
			//for (int j = 0; j < thisGenome->genes.size(); ++j){
			//	outFS << std::fixed << std::setprecision(8) << thisGenome->genes.at(j) << endl;
			//}
			for (double* genePtr = thisGenome->genes; genePtr != thisGenome->genes + thisGenome->genomeSize; ++genePtr)
				outFS << std::fixed << std::setprecision(8) << *genePtr << endl;
			outFS << "GENOME_END" << endl;
		}
	}
	outFS << "POPULATION_END" << endl;

	outFS.close();
	return true;
}
