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
//    neuralnet.h
//    Declarations for Modular Neural Net structures
//
//############################################################################################################//


#include <vector>
#include <math.h>
#include <cmath>
#include "atoms.h"


//Definitions for neural net construction/destruction/operations
//See brain.h for global modular ANN network construction (creating the entire brain from several ANNs)

//Define maximum capacities for allocations in ANNs (update this whenever a new module is added)
//Classification network capacities:
//#define CNET_INPUT_CAPACITY fmax(VISIONREGION_NUMINPUTS, FOCUSREGION_NUMINPUTS);
//#define CNET_HIDDEN_CAPACITY fmax(VISIONREGION_HIDDENUNITS, FOCUSREGION_HIDDENUNITS);
//#define CNET_OUTPUT_CAPACITY fmax(VISIONREGION_NUMOUTPUTS, FOCUSREGION_NUMOUTPUTS);
//#define CNET_LAYER1_CAPACITY fmax(VISIONREGION_NUMINPUTS*VISIONREGION_HIDDENUNITS, FOCUSREGION_NUMINPUTS*FOCUSREGION_HIDDENUNITS);
//#define CNET_LAYER2_CAPACITY fmax(VISIONREGION_HIDDENUNITS*VISIONREGION_NUMOUTPUTS, FOCUSREGION_HIDDENUNITS*FOCUSREGION_NUMOUTPUTS);

#ifndef NEURALNET_H
#define NEURALNET_H

using namespace std;

//Declarations
class SigDendrite;
class Genome;
class ThresholdNeuron;
class SigmoidNeuron;

//Used for perceptron classification networks
//ThresholdDendrites connect a Sigmoid neuron to a threshold neuron
class ThresholdDendrite{
private:
	ThresholdDendrite();
	ThresholdDendrite(SigmoidNeuron* srcN, ThresholdNeuron* destN, double initWeight);
	SigmoidNeuron *src;
	ThresholdNeuron *dest;
	double weight;
	friend class ClassificationNet;
	friend class ThresholdNeuron;
};

//Outputs to classification nets
//Output is 1 when sum>=threshold, else =0
class ThresholdNeuron{
private:
	//ThresholdDendrite** sources;
	vector<ThresholdDendrite*> sources;
	int numSources;
	double sum;
	double threshold;

	ThresholdNeuron();
	ThresholdNeuron(double inheritedThreshold); //initialize threshold from gene
	ThresholdNeuron(double inheritedThreshold, int num_sources);
	void Sum();
	friend class ClassificationNet;
	friend class SigmoidNeuron;

};

//Used for sigmoid approximation networks
//logistic function 1/(1+e^-t)
class SigmoidNeuron{
private:
	//SigDendrite** sources;
	vector<SigDendrite*> sources;
	int numSources;
	double sum;

	SigmoidNeuron();
	SigmoidNeuron(int num_sources);
	void Sum();
	friend class ReflexNet;
	friend class ClassificationNet;
	friend class ThresholdNeuron;
};


class SigDendrite{
private:
	SigDendrite();
	SigDendrite(SigmoidNeuron* srcN, SigmoidNeuron* destN, double initWeight);
	SigmoidNeuron *src;
	SigmoidNeuron *dest;
	double weight;
	friend class ReflexNet;
	friend class SigmoidNeuron;
	friend class ClassificationNet;
};


//Neural network that produces binary classification outputs
//Uses SIGMOID neurons at input/hidden layers and threshold neurons at output layer
//Note: Genes for ClassificationNets are similar to those of ReflexNets (inputweights, hiddenweights, outputweights)
//EXCEPT the weights are preceded by output neuron thresholds. So, genes look like: 
// [output threshold 1, ouput threshold 2, ..., output threshold n, inputweights, hiddenweights, outputweights]
class ClassificationNet{
public:
	ClassificationNet();
	//ClassificationNet(int numInputs, int numHidden, int numOutputs, int startIdx, vector<double> genes);
	ClassificationNet(int numInputs, int numHidden, int numOutputs, int startIdx, double* genes);
	~ClassificationNet();
	//bool FeedForward(vector<double> inputs, vector<double>* actions); //feed inputs through network and receive outputs
	bool FeedForward(double* inputs, double* actions); //feed inputs through network and receive outputs

private:
	vector<SigmoidNeuron*> inputNeurons;
	vector<SigmoidNeuron*> hiddenNeurons;
	vector<ThresholdNeuron*> outputNeurons;
	vector<SigDendrite*> inputEdges;
	vector<ThresholdDendrite*> hiddenEdges;
	int numInputNeurons;
	int numHiddenNeurons;
	int numOutputNeurons;
	int numInputEdges;
	int numHiddenEdges;
	/*SigmoidNeuron* inputNeuronsEnd;
	SigmoidNeuron* hiddenNeuronsEnd;
	ThresholdNeuron* outputNeuronsEnd;
	SigmoidNeuron** inputNeurons;
	SigmoidNeuron** hiddenNeurons;
	ThresholdNeuron** outputNeurons;
	SigDendrite** inputEdges;
	ThresholdDendrite** hiddenEdges;*/
};


//Neural network that produces a nonlinear function at outputs
//Uses SIGMOID neurons
class ReflexNet{
public:
	ReflexNet();
	//ReflexNet(int numInputs, int numHidden, int numOutputs, int startIdx, vector<double> genes);
	ReflexNet(int numInputs, int numHidden, int numOutputs, int startIdx, double* genes);
	~ReflexNet();
	//bool FeedForward(vector<double> inputs, vector<double>* actions); //feed inputs through network and receive outputs
	bool FeedForward(double* inputs, double* actions); //feed inputs through network and receive outputs

private:
	vector<SigmoidNeuron*> inputNeurons;
	vector<SigmoidNeuron*> hiddenNeurons;
	vector<SigmoidNeuron*> outputNeurons;
	vector<SigDendrite*> inputEdges;
	vector<SigDendrite*> hiddenEdges;
	int numInputNeurons;
	int numHiddenNeurons;
	int numOutputNeurons;
	int numInputEdges;
	int numHiddenEdges;
	//SigmoidNeuron* inputNeuronsEnd;
	//SigmoidNeuron* hiddenNeuronsEnd;
	//SigmoidNeuron* outputNeuronsEnd;
	//SigmoidNeuron** inputNeurons;
	//SigmoidNeuron** hiddenNeurons;
	//SigmoidNeuron** outputNeurons;
	//SigDendrite** inputEdges;
	//SigDendrite** hiddenEdges;
};



#endif
