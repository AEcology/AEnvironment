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
//    neuralnet.cpp
//    The secret sauce stuff for the Neural networks. Please excuse our dust! This code is constantly being
//    refactored for efficiency, so expect some messiness.
//
//############################################################################################################//


#include "neuralnet.h"


ThresholdDendrite::ThresholdDendrite(SigmoidNeuron* srcN, ThresholdNeuron* destN, double initWeight){
	this->src = srcN;
	this->dest = destN;
	this->weight = initWeight;
}

SigDendrite::SigDendrite(SigmoidNeuron* srcN, SigmoidNeuron* destN, double initWeight){
	this->src = srcN;
	this->dest = destN;
	this->weight = initWeight;
}

ThresholdNeuron::ThresholdNeuron(){
	sum = 0;
	threshold = 0;
	numSources = 0;
}

ThresholdNeuron::ThresholdNeuron(double inheritedThreshold){
	sum = 0;
	threshold = inheritedThreshold;
	numSources = 0;
}

ThresholdNeuron::ThresholdNeuron(double inheritedThreshold, int num_sources){
	sum = 0;
	threshold = inheritedThreshold;
	numSources = 0;
	sources.reserve(num_sources);
}

SigmoidNeuron::SigmoidNeuron(){
	sum = 0;
	numSources = 0;
}

SigmoidNeuron::SigmoidNeuron(int num_sources){
	sum = 0;
	numSources = num_sources;
	sources.reserve(numSources);
}

void SigmoidNeuron::Sum(){
	//unsigned int i = 0;
	//sum = 0;
	/*for (i = 0; i < this->sources.size(); ++i){
		sum = sum + this->sources.at(i)->src->sum*this->sources.at(i)->weight;
	}*/
	//SigDendrite* sourcesEnd = sources[numSources];
	//for (SigDendrite* thisDendrite = *sources; thisDendrite != sourcesEnd; ++thisDendrite){
	//	sum = sum + thisDendrite->src->sum * thisDendrite->weight;
	//}
	for (vector<SigDendrite*>::iterator it = sources.begin(); it != sources.end(); ++it){
		sum = sum + (*it)->src->sum * (*it)->weight;
	}
	sum = static_cast<double>(1 / (double)(1 + exp(-1 * sum)));
}

void ThresholdNeuron::Sum(){
	unsigned int i = 0;
	sum = 0;
	/*for (i = 0; i < this->sources.size(); ++i){
		sum = sum + this->sources.at(i)->src->sum*this->sources.at(i)->weight;
	}*/
	//ThresholdDendrite* sourcesEnd = sources[numSources];
	//for (ThresholdDendrite* thisDendrite = *sources; thisDendrite != sourcesEnd; ++thisDendrite){
	//	sum = sum + thisDendrite->src->sum * thisDendrite->weight;
	//}
	for (vector<ThresholdDendrite*>::iterator it = sources.begin(); it != sources.end(); ++it){
		sum = sum + (*it)->src->sum * (*it)->weight;
	}
	if (sum >= threshold)
		sum = 1;
	else
		sum = 0;
}

ClassificationNet::ClassificationNet(int numInputs, int numHidden, int numOutputs, int startIdx, double* genes){
	int i = 0; //generic iterator
	//int j = 0; //inner generic iterator
	/***********INITIALIZE NEURONS************/
	/*for (i = 0; i < numInputs; ++i){
		this->inputs.push_back(new SigmoidNeuron);
	}
	for (i = 0; i < numHidden; ++i){
		this->hidden.push_back(new SigmoidNeuron);
	}
	for (i = 0; i < numOutputs; ++i){
		this->outputs.push_back(new ThresholdNeuron(genes.at(k)));
		++k;
	}*/

	//inputNeurons = new SigmoidNeuron*[numInputs];
	//hiddenNeurons = new SigmoidNeuron*[numOutputs];
	//outputNeurons = new ThresholdNeuron*[numOutputs];
	//inputEdges = new SigDendrite*[numInputEdges];
	//hiddenEdges = new ThresholdDendrite*[numHiddenEdges];

	//inputNeuronsEnd = inputNeurons[numInputs];
	//hiddenNeuronsEnd = hiddenNeurons[numHidden];
	//outputNeuronsEnd = outputNeurons[numOutputs];
	//for (SigmoidNeuron* neuronPtr = *inputNeurons; neuronPtr != inputNeuronsEnd; ++neuronPtr)
	//	neuronPtr = new SigmoidNeuron;
	double* genePtr = genes; //genome iterator
	numInputNeurons = numInputs;
	numHiddenNeurons = numHidden;
	numOutputNeurons = numOutputs;
	numInputEdges = numInputs*numHidden;
	numHiddenEdges = numHidden*numOutputs;
	inputNeurons.reserve(numInputs);
	hiddenNeurons.reserve(numHidden);
	outputNeurons.reserve(numOutputs);
	inputEdges.reserve(numInputEdges);
	hiddenEdges.reserve(numHiddenEdges);
	for (i=0; i < numInputs; ++i){
		this->inputNeurons.push_back(new SigmoidNeuron(1));
	}
	for (i = 0; i < numHidden; ++i){
		this->hiddenNeurons.push_back(new SigmoidNeuron(numInputEdges));
	}
	for (i = 0; i < numOutputs; ++i){
		this->outputNeurons.push_back(new ThresholdNeuron(*genePtr, numHiddenEdges));
		++genePtr;
	}
	for (vector<SigmoidNeuron*>::iterator it1 = inputNeurons.begin(); it1 != inputNeurons.end(); ++it1){
		for (vector<SigmoidNeuron*>::iterator it2 = hiddenNeurons.begin(); it2 != hiddenNeurons.end(); ++it2){
			inputEdges.push_back(new SigDendrite(*it1, *it2, *genePtr));
			(*it2)->sources.push_back(inputEdges.back());
			++genePtr;
		}
	}
	for (vector<SigmoidNeuron*>::iterator it1 = hiddenNeurons.begin(); it1 != hiddenNeurons.end(); ++it1){
		for (vector<ThresholdNeuron*>::iterator it2 = outputNeurons.begin(); it2 != outputNeurons.end(); ++it2){
			hiddenEdges.push_back(new ThresholdDendrite(*it1, *it2, *genePtr));
			(*it2)->sources.push_back(hiddenEdges.back());
			++genePtr;
		}
	}

	//for (SigmoidNeuron* neuronPtr = *hiddenNeurons; neuronPtr != hiddenNeuronsEnd; ++neuronPtr){
	//	neuronPtr = new SigmoidNeuron;
	//	neuronPtr->sources = new SigDendrite*[numInputs];
	//	neuronPtr->numSources = numInputs;
	//}
	//for (ThresholdNeuron* neuronPtr = *outputNeurons; neuronPtr != outputNeuronsEnd; ++neuronPtr){
	//	neuronPtr = new ThresholdNeuron(*genePtr);
	//	neuronPtr->sources = new ThresholdDendrite*[numHidden];
	//	neuronPtr->numSources = numHidden;
	//	++genePtr;
	//}
	/*SigDendrite* layer1EdgePtr = *inputEdges;
	i = 0;
	for (SigmoidNeuron* inputNeuronPtr = *inputNeurons; inputNeuronPtr != inputNeuronsEnd; ++inputNeuronPtr){
		for (SigmoidNeuron* hiddenNeuronPtr = *hiddenNeurons; hiddenNeuronPtr != hiddenNeuronsEnd; ++hiddenNeuronPtr){
			layer1EdgePtr = new SigDendrite(inputNeuronPtr, hiddenNeuronPtr, *genePtr);
			hiddenNeuronPtr->sources[i] = layer1EdgePtr;
			++layer1EdgePtr;
			++genePtr;
		}
		++i;
	}
	ThresholdDendrite* layer2EdgePtr = *hiddenEdges;
	i = 0;
	for (SigmoidNeuron* hiddenNeuronPtr = *hiddenNeurons; hiddenNeuronPtr != hiddenNeuronsEnd; ++hiddenNeuronPtr){
		for (ThresholdNeuron* outputNeuronPtr = *outputNeurons; outputNeuronPtr != outputNeuronsEnd; ++outputNeuronPtr){
			layer2EdgePtr = new ThresholdDendrite(hiddenNeuronPtr, outputNeuronPtr, *genePtr);
			outputNeuronPtr->sources[i] = layer2EdgePtr;
			++layer2EdgePtr;
			++genePtr;
		}
		++i;
	}*/

	/***********INITIALIZE DENDRITES************/
	/*for (i = 0; i < numInputs; ++i){ //build first layer Dendrite tree
		for (j = 0; j < numHidden; ++j){
			inputEdges.push_back(new SigDendrite(this->inputs.at(i), this->hidden.at(j), genes.at(k)));
			this->hidden.at(j)->sources.push_back(inputEdges.back()); //add the new dendrite to hidden neuron input list
			++k;
		}
	}
	for (i = 0; i < numHidden; ++i){ //build second layer Dendrite tree
		for (j = 0; j < numOutputs; ++j){
			hiddenEdges.push_back(new ThresholdDendrite(this->hidden.at(i), this->outputs.at(j), genes.at(k)));
			this->outputs.at(j)->sources.push_back(hiddenEdges.back()); //add the new dendrite to hidden neuron input list
			++k;
		}
	}
	*/

}

ClassificationNet::~ClassificationNet(){
	//int i = 0;
	while (!inputNeurons.empty()){
		SigmoidNeuron* thisNeuron = inputNeurons.back();
		inputNeurons.pop_back();
		delete thisNeuron;
	}
	while (!hiddenNeurons.empty()){
		SigmoidNeuron* thisNeuron = hiddenNeurons.back();
		hiddenNeurons.pop_back();
		delete thisNeuron;
	}
	while (!outputNeurons.empty()){
		ThresholdNeuron* thisNeuron = outputNeurons.back();
		outputNeurons.pop_back();
		delete thisNeuron;
	}
	while (!inputEdges.empty()){
		SigDendrite* thisDendrite = inputEdges.back();
		inputEdges.pop_back();
		delete thisDendrite;
	}
	while (!hiddenEdges.empty()){
		ThresholdDendrite* thisDendrite = hiddenEdges.back();
		hiddenEdges.pop_back();
		delete thisDendrite;
	}
	/*
	SigDendrite* layer1EdgePtr = *inputEdges;
	for (SigmoidNeuron* inputNeuronPtr = *inputNeurons; inputNeuronPtr != inputNeuronsEnd; ++inputNeuronPtr){
		for (SigmoidNeuron* hiddenNeuronPtr = *hiddenNeurons; hiddenNeuronPtr != hiddenNeuronsEnd; ++hiddenNeuronPtr){
			delete layer1EdgePtr;
			++layer1EdgePtr;
		}
		delete inputNeuronPtr;
	}
	ThresholdDendrite* layer2EdgePtr = *hiddenEdges;
	for (SigmoidNeuron* hiddenNeuronPtr = *hiddenNeurons; hiddenNeuronPtr != hiddenNeuronsEnd; ++hiddenNeuronPtr){
		for (ThresholdNeuron* outputNeuronPtr = *outputNeurons; outputNeuronPtr != outputNeuronsEnd; ++outputNeuronPtr){
			delete layer2EdgePtr;
			++layer2EdgePtr;
		}
		delete hiddenNeuronPtr;
	}

	for (ThresholdNeuron* neuronPtr = *outputNeurons; neuronPtr != outputNeuronsEnd; ++neuronPtr)
		delete neuronPtr;
	
	delete [] outputNeurons;
	delete [] hiddenNeurons;
	delete [] inputNeurons;
	delete [] inputEdges;
	delete [] hiddenEdges;
	*/
}

bool ClassificationNet::FeedForward(double* inputs, double* actions){
	for (vector<SigmoidNeuron*>::iterator it = inputNeurons.begin(); it != inputNeurons.end(); ++it){
		(*it)->sum = *inputs;
		++inputs;
	}
	for (vector<SigmoidNeuron*>::iterator it = hiddenNeurons.begin(); it != hiddenNeurons.end(); ++it){
		(*it)->Sum();
	}
	for (vector<ThresholdNeuron*>::iterator it = outputNeurons.begin(); it != outputNeurons.end(); ++it){
		(*it)->Sum();
		*actions = static_cast<double>((*it)->sum);
		++actions;
	}
	/* int i = 0;
	for (i = 0; i < inputs.size(); ++i){
		this->inputs.at(i)->sum = inputs.at(i);
	}
	for (i = 0; i < this->hidden.size(); ++i){
		this->hidden.at(i)->Sum();
	}
	for (i = 0; i < this->outputs.size(); ++i){
		this->outputs.at(i)->Sum();
		outputs->at(i) = static_cast<double>(this->outputs.at(i)->sum);
	}*/
	/*
	double* it = inputs;
	for (SigmoidNeuron* inputNeuronPtr = *inputNeurons; inputNeuronPtr != inputNeuronsEnd; ++inputNeuronPtr){
		inputNeuronPtr->sum = *it;
		++it;
	}
	for (SigmoidNeuron* hiddenNeuronPtr = *hiddenNeurons; hiddenNeuronPtr != hiddenNeuronsEnd; ++hiddenNeuronPtr)
		hiddenNeuronPtr->Sum();
	it = actions;
	for (ThresholdNeuron* outputNeuronPtr = *outputNeurons; outputNeuronPtr != outputNeuronsEnd; ++outputNeuronPtr){
		outputNeuronPtr->Sum();
		*it = static_cast<double>(outputNeuronPtr->sum);
		++it;
	}
	*/
	return true;
}


ReflexNet::ReflexNet(int numInputs, int numHidden, int numOutputs, int startIdx, double* genes){
	int i = 0;
	double* genePtr = genes; //genome iterator
	numInputNeurons = numInputs;
	numHiddenNeurons = numHidden;
	numOutputNeurons = numOutputs;
	numInputEdges = numInputs*numHidden;
	numHiddenEdges = numHidden*numOutputs;
	inputNeurons.reserve(numInputs);
	hiddenNeurons.reserve(numHidden);
	outputNeurons.reserve(numOutputs);
	inputEdges.reserve(numInputEdges);
	hiddenEdges.reserve(numHiddenEdges);
	for (i = 0; i < numInputs; ++i){
		this->inputNeurons.push_back(new SigmoidNeuron(1));
	}
	for (i = 0; i < numHidden; ++i){
		this->hiddenNeurons.push_back(new SigmoidNeuron(numInputEdges));
	}
	for (i = 0; i < numOutputs; ++i){
		this->outputNeurons.push_back(new SigmoidNeuron(numHiddenEdges));
		++genePtr;
	}
	for (vector<SigmoidNeuron*>::iterator it1 = inputNeurons.begin(); it1 != inputNeurons.end(); ++it1){
		for (vector<SigmoidNeuron*>::iterator it2 = hiddenNeurons.begin(); it2 != hiddenNeurons.end(); ++it2){
			inputEdges.push_back(new SigDendrite(*it1, *it2, *genePtr));
			(*it2)->sources.push_back(inputEdges.back());
			++genePtr;
		}
	}
	for (vector<SigmoidNeuron*>::iterator it1 = hiddenNeurons.begin(); it1 != hiddenNeurons.end(); ++it1){
		for (vector<SigmoidNeuron*>::iterator it2 = outputNeurons.begin(); it2 != outputNeurons.end(); ++it2){
			hiddenEdges.push_back(new SigDendrite(*it1, *it2, *genePtr));
			(*it2)->sources.push_back(hiddenEdges.back());
			++genePtr;
		}
	}
	//int i = 0;
	/*int j = 0;
	/***********INITIALIZE NEURONS************/
	/*for (i = 0; i < numInputs; ++i){
		this->inputs.push_back(new SigmoidNeuron);
	}
	for (i = 0; i < numHidden; ++i){
		this->hidden.push_back(new SigmoidNeuron);
	}
	for (i = 0; i < numOutputs; ++i){
		this->outputs.push_back(new SigmoidNeuron);
	}

	/***********INITIALIZE DENDRITES************/
	/*int k = 0;
	for (i = 0; i < numInputs; ++i){ //build first layer Dendrite tree
		for (j = 0; j < numHidden; ++j){
			inputEdges.push_back(new SigDendrite(this->inputs.at(i), this->hidden.at(j), genes.at(k)));
			this->hidden.at(j)->sources.push_back(inputEdges.back()); //add the new dendrite to hidden neuron input list
			++k;
		}
	}
	for (i = 0; i < numHidden; ++i){ //build second layer Dendrite tree
		for (j = 0; j < numOutputs; ++j){
			hiddenEdges.push_back(new SigDendrite(this->hidden.at(i), this->outputs.at(j), genes.at(k)));
			this->outputs.at(j)->sources.push_back(hiddenEdges.back()); //add the new dendrite to hidden neuron input list
			++k;
		}
	}*/

	/************************SECOND TRY**********************************
	double* genePtr = genes; //genome iterator
	numInputNeurons = numInputs;
	numHiddenNeurons = numHidden;
	numOutputNeurons = numOutputs;
	numInputEdges = numInputs*numHidden;
	numHiddenEdges = numHidden*numOutputs;
	inputNeurons = new SigmoidNeuron*[numInputs];
	hiddenNeurons = new SigmoidNeuron*[numOutputs];
	outputNeurons = new SigmoidNeuron*[numOutputs];
	inputEdges = new SigDendrite*[numInputEdges];
	hiddenEdges = new SigDendrite*[numHiddenEdges];
	inputNeuronsEnd = inputNeurons[numInputs];
	hiddenNeuronsEnd = hiddenNeurons[numHidden];
	outputNeuronsEnd = outputNeurons[numOutputs];
	for (SigmoidNeuron* neuronPtr = *inputNeurons; neuronPtr != inputNeuronsEnd; ++neuronPtr)
		neuronPtr = new SigmoidNeuron;
	for (SigmoidNeuron* neuronPtr = *hiddenNeurons; neuronPtr != hiddenNeuronsEnd; ++neuronPtr){
		neuronPtr = new SigmoidNeuron;
		neuronPtr->sources = new SigDendrite*[numInputs];
		neuronPtr->numSources = numInputs;
	}
	for (SigmoidNeuron* neuronPtr = *outputNeurons; neuronPtr != outputNeuronsEnd; ++neuronPtr){
		neuronPtr = new SigmoidNeuron;
		neuronPtr->sources = new SigDendrite*[numHidden];
		neuronPtr->numSources = numHidden;
	}
	SigDendrite* layer1EdgePtr = *inputEdges;
	i = 0;
	for (SigmoidNeuron* inputNeuronPtr = *inputNeurons; inputNeuronPtr != inputNeuronsEnd; ++inputNeuronPtr){
		for (SigmoidNeuron* hiddenNeuronPtr = *hiddenNeurons; hiddenNeuronPtr != hiddenNeuronsEnd; ++hiddenNeuronPtr){
			layer1EdgePtr = new SigDendrite(inputNeuronPtr, hiddenNeuronPtr, *genePtr);
			hiddenNeuronPtr->sources[i] = layer1EdgePtr;
			++layer1EdgePtr;
			++genePtr;
		}
		++i;
	}
	SigDendrite* layer2EdgePtr = *hiddenEdges;
	i = 0;
	for (SigmoidNeuron* hiddenNeuronPtr = *hiddenNeurons; hiddenNeuronPtr != hiddenNeuronsEnd; ++hiddenNeuronPtr){
		for (SigmoidNeuron* outputNeuronPtr = *outputNeurons; outputNeuronPtr != outputNeuronsEnd; ++outputNeuronPtr){
			layer2EdgePtr = new SigDendrite(hiddenNeuronPtr, outputNeuronPtr, *genePtr);
			outputNeuronPtr->sources[i] = layer2EdgePtr;
			++layer2EdgePtr;
			++genePtr;
		}
		++i;
	}
	*/

}

ReflexNet::~ReflexNet(){
	int i = 0;
	while (!inputNeurons.empty()){
		SigmoidNeuron* thisNeuron = inputNeurons.back();
		inputNeurons.pop_back();
		delete thisNeuron;
	}
	while (!hiddenNeurons.empty()){
		SigmoidNeuron* thisNeuron = hiddenNeurons.back();
		hiddenNeurons.pop_back();
		delete thisNeuron;
	}
	while (!outputNeurons.empty()){
		SigmoidNeuron* thisNeuron = outputNeurons.back();
		outputNeurons.pop_back();
		delete thisNeuron;
	}
	while (!inputEdges.empty()){
		SigDendrite* thisDendrite = inputEdges.back();
		inputEdges.pop_back();
		delete thisDendrite;
	}
	while (!hiddenEdges.empty()){
		SigDendrite* thisDendrite = hiddenEdges.back();
		hiddenEdges.pop_back();
		delete thisDendrite;
	}
/*	SigDendrite* layer1EdgePtr = *inputEdges;
	for (SigmoidNeuron* inputNeuronPtr = *inputNeurons; inputNeuronPtr != inputNeuronsEnd; ++inputNeuronPtr){
		for (SigmoidNeuron* hiddenNeuronPtr = *hiddenNeurons; hiddenNeuronPtr != hiddenNeuronsEnd; ++hiddenNeuronPtr){
			delete layer1EdgePtr;
			++layer1EdgePtr;
		}
		delete inputNeuronPtr;
	}
	SigDendrite* layer2EdgePtr = *hiddenEdges;
	for (SigmoidNeuron* hiddenNeuronPtr = *hiddenNeurons; hiddenNeuronPtr != hiddenNeuronsEnd; ++hiddenNeuronPtr){
		for (SigmoidNeuron* outputNeuronPtr = *outputNeurons; outputNeuronPtr != outputNeuronsEnd; ++outputNeuronPtr){
			delete layer2EdgePtr;
			++layer2EdgePtr;
		}
		delete hiddenNeuronPtr;
	}

	for (SigmoidNeuron* neuronPtr = *outputNeurons; neuronPtr != outputNeuronsEnd; ++neuronPtr)
		delete neuronPtr;

	delete outputNeurons;
	delete hiddenNeurons;
	delete inputNeurons;
	delete inputEdges;
	delete hiddenEdges;*/
}

bool ReflexNet::FeedForward(double* inputs, double* actions){
	for (vector<SigmoidNeuron*>::iterator it = inputNeurons.begin(); it != inputNeurons.end(); ++it){
		(*it)->sum = *inputs;
		++inputs;
	}
	for (vector<SigmoidNeuron*>::iterator it = hiddenNeurons.begin(); it != hiddenNeurons.end(); ++it){
		(*it)->Sum();
	}
	for (vector<SigmoidNeuron*>::iterator it = outputNeurons.begin(); it != outputNeurons.end(); ++it){
		(*it)->Sum();
		*actions = static_cast<double>((*it)->sum);
		++actions;
	}
	/*unsigned int i = 0;
	for (i = 0; i < inputs.size(); ++i){
		this->inputs.at(i)->sum = inputs.at(i);
	}
	for (i = 0; i < this->hidden.size(); ++i){
		this->hidden.at(i)->Sum();
	}
	for (i = 0; i < this->outputs.size(); ++i){
		this->outputs.at(i)->Sum();
		outputs->at(i) = static_cast<double>(this->outputs.at(i)->sum);
	}
	return true;*/
	/*double* it = inputs;
	for (SigmoidNeuron* inputNeuronPtr = *inputNeurons; inputNeuronPtr != inputNeuronsEnd; ++inputNeuronPtr){
		inputNeuronPtr->sum = *it;
		++it;
	}
	for (SigmoidNeuron* hiddenNeuronPtr = *hiddenNeurons; hiddenNeuronPtr != hiddenNeuronsEnd; ++hiddenNeuronPtr)
		hiddenNeuronPtr->Sum();
	it = actions;
	for (SigmoidNeuron* outputNeuronPtr = *outputNeurons; outputNeuronPtr != outputNeuronsEnd; ++outputNeuronPtr){
		outputNeuronPtr->Sum();
		*it = static_cast<double>(outputNeuronPtr->sum);
		++it;
	}*/
	return true;
}
