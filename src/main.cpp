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
//    main.cpp
//    Main gateway (with items for terminal UI)
//
//############################################################################################################//

#include "glutil.h"
#include "environment.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#define VERSION "0.8"
//Just for fun:
#define LINES_OF_CODE 3815
//Last counted 7-7-14

//Default params
//best 50-30-2
#define NUM_ATOMS 60
#define NUM_SUGARS 50
#define NUM_ARSENIC 40
#define NUM_HUNTERS 0


using namespace std;

SPEED_MODE SPEEDMODE = Normal;
Environment* testEnvironment;
bool loadFromFile;
std::string fileName;
extern vec2* SCREEN_POSITION;

//Evolution constraints: used by evolution.cpp
//if false, prevents that region of brain from being modified
//Note: If VISION mutations are disabled, check VISIONREGION_TRAINED to see if FeedForward uses manual logic to determine predator/prey relationships,
//which is necessary for training the reflex regions
bool PREDATORY_ACTIONS_ENABLED = true;
bool POPULATION_PRESERVATION_ENABLED = true;
bool ALLOWMUTATIONS_VISION; //Allow for mutations in vision region (TODO COMMAND LINE IMPLEMENT)
bool VISIONREGION_ENABLED;  //Set to false if visual region is untrained (forces explicit logical handling in brain feedforward) (TODO IMPLEMENT IN COMMAND LINE)
bool ALLOWMUTATIONS_FOCUS;
bool FOCUSREGION_ENABLED;
bool ALLOWMUTATIONS_PURSUE; //Allow mutations for pursue region
bool ALLOWMUTATIONS_FLEE;   //Allow for flee region
bool ALLOWMUTATIONS_SOCIAL; //etc...
bool ALLOWMUTATIONS_NULL;

double GENETIC_DISTANCE_THRESHOLD = 8.0;


void runMainLoop(int val);


void PrintHelp(){
	cout << endl <<  "Commands:" << endl << endl;

	cout << "\"start\" runs the environment with default parameters and random weights" << endl << endl;

	cout << "\"params\"" << endl;
	cout << "Print all environment parameters." << endl << endl;
	
	cout << "\"load <filename.ext>\"" << endl;
	cout << "Load saved atoms from file (note: press \"s\" in game to save a file)" << endl << endl;

	cout << "\"set <attribute> <objects> <num>\"" << endl;
	cout << "attribute: num" << endl;
	cout << "objects: atoms, hunters, sugars, arsenic" << endl;
	cout << "Example: \"set num hunters 45\" commands the environment to create 45 hunters" << endl << endl;

	cout << "\"enable <operation> <instance>\"" << endl;
	cout << "operation: mutations, operation, mode" << endl;
	cout << "instance: vision, focus, pursue, flee, social, null, preservation, predators" << endl;
	cout << "Example: \"enable mutations pursue\" enables mutations in pursue brain region" << endl << endl;

	cout << "\"disable <operation> <instance>\"" << endl;
	cout << "operation: mutations, operation, mode" << endl;
	cout << "instance: vision, focus, pursue, flee, social, null, preservation, predators" << endl;
	cout << "Example: \"disable mutations pursue\" disables mutations in pursue brain region" << endl << endl;

	cout << "Note: When starting from no load file, only primitive (flee, pursue, social) regions\nof brain are enabled,\nand species preservation is enabled." << endl;
	return;
}

bool CommandPrompt(){
	bool goFlag = false;
	loadFromFile = false;
	string command;
	ifstream inFS;
	int n_arg = 0;
	int numAtoms = NUM_ATOMS;
	int numHunters = NUM_HUNTERS;
	int numSugars = NUM_SUGARS;
	int numArsenic = NUM_ARSENIC;
	ALLOWMUTATIONS_VISION = true;
	ALLOWMUTATIONS_FOCUS = true;
	ALLOWMUTATIONS_PURSUE = true; //Allow mutations for pursue region
	ALLOWMUTATIONS_FLEE = true;   //Allow for flee region
	ALLOWMUTATIONS_SOCIAL = true; //etc...
	ALLOWMUTATIONS_NULL = true;
	VISIONREGION_ENABLED = true;
	FOCUSREGION_ENABLED = true;


	cout << "**************************************************************" << endl;
	cout << "*************** Algorithmic Ecology Project ******************" << endl;
	cout << "************** "<< LINES_OF_CODE << " lines of code and counting! **************" << endl;
	cout << "****************** pre-alpha version " << VERSION << " ********************" << endl;
	cout << "**************************************************************" << endl;

	while (!goFlag){
		cin.clear();
		cout << endl;
		cout << "AE " << VERSION << " >> ";
		getline(cin, command);
		if (cin.good() && command == "start"){
			goFlag = true;
			if (!loadFromFile){
				cout << "Warning: When starting without a load file, perception networks\nin brain are disabled. Type \"help\" for more info" << endl;
				ALLOWMUTATIONS_VISION = false;
				VISIONREGION_ENABLED = false;
				ALLOWMUTATIONS_FOCUS = false;
				FOCUSREGION_ENABLED = false;
				POPULATION_PRESERVATION_ENABLED = true;
			}
			cout << "Running simulation..." << endl;
		}
		else if (cin.good() && command.substr(0, 4) == "load"){
			if (command.length() >= 6){
				fileName = command.substr(5);
				inFS.open(fileName.c_str());
				if (!inFS.good())
					cout << "Invalid read file: " << fileName;
				else{
					cout << "Mounting population from " << fileName;
					loadFromFile = true;
					inFS.close();
				}
			}
			else
				cout << "Must specify filename for command 'load'";
		}
		else if (cin.good() && command.substr(0, 3) == "set"){
			if (command.length() >= 15){
				command = command.substr(4, command.length() - 1);
				if (cin.good() && command.substr(0, 3) == "num"){
					command = command.substr(4, command.length() - 1);
					if (cin.good() && command.substr(0, 5) == "atoms"){
						command = command.substr(6, command.length() - 1);
						n_arg = static_cast<int>(atof(command.c_str()));
						if (n_arg < 1000 && n_arg>1){
							numAtoms = n_arg;
							cout << "Atom population set to " << numAtoms;
						}
						else
							cout << "Set command out of range (min: 2, max: 1000)";
					}
					else if (cin.good() && command.substr(0, 6) == "sugars"){
						command = command.substr(7, command.length() - 1);
						n_arg = static_cast<int>(atof(command.c_str()));
						if (n_arg < 1000 && n_arg>=0){
							numSugars = n_arg;
							cout << "Sugar population set to " << numSugars;
						}
						else
							cout << "Set command out of range";
					}
					else if (cin.good() && command.substr(0, 7) == "arsenic"){
						command = command.substr(8, command.length() - 1);
						n_arg = static_cast<int>(atof(command.c_str()));
						if (n_arg < 1000 && n_arg >= 0){
							numArsenic = n_arg;
							cout << "Arsenic population set to " << numArsenic;
						}
						else
							cout << "Set command out of range";
					}
					else if (cin.good() && command.substr(0, 7) == "hunters"){
						command = command.substr(8, command.length() - 1);
						n_arg = static_cast<int>(atof(command.c_str()));
						if (n_arg < 100 && n_arg>=0){
							numHunters = n_arg;
							cout << "Hunter population set to " << numHunters;
						}
						else
							cout << "Set command out of range";
					}
					else
						cout << "Set command not recognized";
				}
			}
			else
				cout << "set usage: \"set attribute objects num\"";
		}
		else if (cin.good() && command == "params"){
			cout << endl << "**************Environment mode**************" << endl;
			cout << "Predatory actions allowed: " << PREDATORY_ACTIONS_ENABLED << endl;
			cout << "Population preservation allowed: " << POPULATION_PRESERVATION_ENABLED << endl;
			cout << endl << "***********Objects in environment***********" << endl;
			cout << "Atoms: " << numAtoms << endl;
			cout << "Sugars: " << numSugars << endl;
			cout << "Arsenic: " << numArsenic << endl;
			cout << "Hunters: " << numHunters << endl;
			cout << endl << "***************Neural Modules***************" << endl;
			cout << "Brain region operations enabled: " << endl;
			cout << "Vision classifier region: " << VISIONREGION_ENABLED << endl;
			cout << "Focus classifier region: " << FOCUSREGION_ENABLED << endl;
			cout << "Brain region mutations allowed:" << endl;
			cout << "Vision classifier region: " << ALLOWMUTATIONS_VISION << endl;
			cout << "Focus classifier region: " << ALLOWMUTATIONS_FOCUS << endl;
			cout << "Pursue motor region: " << ALLOWMUTATIONS_PURSUE << endl;
			cout << "Flee motor region: " << ALLOWMUTATIONS_FLEE << endl;
			cout << "Social motor region: " << ALLOWMUTATIONS_SOCIAL << endl;
			cout << "Null motor region: " << ALLOWMUTATIONS_NULL << endl;

		}
		else if (cin.good() && command.substr(0, 7) == "disable"){
			if (command.length() >= 19){
				command = command.substr(8, command.length() - 1);
				if (command.substr(0, 9) == "mutations"){
					command = command.substr(10, command.length() - 1);
					if (command == "pursue"){
						ALLOWMUTATIONS_PURSUE = false;
						cout << "Pursue region mutations disabled";
					}
					else if (command == "vision"){
						ALLOWMUTATIONS_VISION = false;
						cout << "Vision region mutations disabled";
					}
					else if (command == "focus"){
						ALLOWMUTATIONS_FOCUS = false;
						cout << "Focus region mutations disabled";
					}
					else if (command == "flee"){
						ALLOWMUTATIONS_FLEE = false;
						cout << "Flee region mutations disabled";
					}
					else if (command == "social"){
						ALLOWMUTATIONS_SOCIAL = false;
						cout << "Social region mutations disabled";
					}
					else if (command == "null"){
						ALLOWMUTATIONS_NULL = false;
						cout << "Null region mutations disabled";
					}
					else
						cout << "Unrecognized instance parameter";
				}
				else if (command.substr(0, 9) == "operation"){
					command = command.substr(10, command.length() - 1);
					if (command == "vision"){
						VISIONREGION_ENABLED = false;
						cout << "Vision region operation disabled";
					}
					else if (command == "focus"){
						FOCUSREGION_ENABLED = false;
						cout << "Focus region operation disabled";
					}
					else
						cout << "Unrecognized instance parameter";
				}
				else if (command.substr(0, 4) == "mode"){
					command = command.substr(5, command.length() - 1);
					if (command == "preservation"){
						POPULATION_PRESERVATION_ENABLED = false;
						cout << "Population preservation disabled";
					}
					else if (command == "predators"){
						PREDATORY_ACTIONS_ENABLED = false;
						cout << "Predatory actions disabled";
					}
					else{
						cout << "Unrecognized instance parameter";
					}
				}
				else
					cout << "Unrecognized disable operation paramater";
			}
			else
				cout << "disable use: disable <operation> <instance>";
		}
		else if (cin.good() && command.substr(0, 6) == "enable"){
			if (command.length() >= 19){
				command = command.substr(7, command.length() - 1);
				if (command.substr(0, 9) == "mutations"){
					command = command.substr(10, command.length() - 1);
					if (command == "pursue"){
						ALLOWMUTATIONS_PURSUE = true;
						cout << "Pursue region mutations enabled";
					}
					else if (command == "vision"){
						ALLOWMUTATIONS_VISION = true;
						cout << "Vision region mutations enabled";
					}
					else if (command == "focus"){
						ALLOWMUTATIONS_FOCUS = true;
						cout << "Focus region mutations enabled";
					}
					else if (command == "flee"){
						ALLOWMUTATIONS_FLEE = true;
						cout << "Flee region mutations enabled";
					}
					else if (command == "social"){
						ALLOWMUTATIONS_SOCIAL = true;
						cout << "Social region mutations enabled";
					}
					else if (command == "null"){
						ALLOWMUTATIONS_NULL = true;
						cout << "Null region mutations enabled";
					}
					else
						cout << "Unrecognized instance parameter";
				}
				else if (command.substr(0, 9) == "operation"){
					command = command.substr(10, command.length() - 1);
					if (command == "vision"){
						VISIONREGION_ENABLED = true;
						cout << "Vision region operation enabled";
					}
					else if (command == "focus"){
						FOCUSREGION_ENABLED = true;
						cout << "Focus region operation enabled";
					}
					else
						cout << "Unrecognized instance parameter";
				}
				else if (command.substr(0, 4) == "mode"){
					command = command.substr(5, command.length() - 1);
					if (command == "preservation"){
						POPULATION_PRESERVATION_ENABLED = true;
						cout << "Population preservation enabled";
					}
					else if (command == "predators"){
						PREDATORY_ACTIONS_ENABLED = true;
						cout << "Predatory actions enabled";
					}
					else{
						cout << "Unrecognized instance parameter";
					}
				}
				else
					cout << "Unrecognized enable operation paramater";
			}
			else
				cout << "enable use: enable <operation> <instance>";
		}
		else if (command == "help")
			PrintHelp();
		else{
			cout << "Command not recognized";
			cin.clear();
		}
	}

	SCREEN_POSITION = new vec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	testEnvironment = new Environment(numAtoms, numSugars, numArsenic, numHunters, loadFromFile, fileName);

	if (testEnvironment == NULL)
		return false;
	else
		return true;
}



int main(int argc, char* args[]){

        std::cout << "Main()\n";

	//Get user commands about environment to be used
	CommandPrompt();

        std::cout << "CommandPrompt successful\n";


	glutInit(&argc, args);

	glutInitContextVersion(2, 1);
	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow("Algorithmic Ecology Project v0.8");
	if (!GL_Init())
	{
		printf("Unable to initialize graphics library!\n");
		return 1;
	}
	glutDisplayFunc(GL_Render);
	glutKeyboardFunc(handleKeys);
	glutMouseFunc(handleClicks);
	glutMotionFunc(handleMotion);
	//glutCloseFunc(safeClose);
	//Main loop:
	glutTimerFunc(1000 / SCREEN_FPS, runMainLoop, 0);

	glutMainLoop();

	delete SCREEN_POSITION;
	delete testEnvironment;

	return 0;
}

void runMainLoop(int val)
{

	if (SPEEDMODE == Warp){
		for (int i = 0; i < 2000; ++i)
			testEnvironment->Update();
		SPEEDMODE = Normal;
		//Frame logic
		GL_Update();
		GL_Render();
		glutTimerFunc(1, runMainLoop, val);
		SPEEDMODE = Warp;
	}
	else{
		//Frame logic
		GL_Update();
		GL_Render();
		//Run frame one more time
		glutTimerFunc(1000 / SCREEN_FPS, runMainLoop, val);
		//SCREEN_FPS=60 => in 1000/60ms, call runMainLoop(val) again
		//loops so that this is updating 60 times per second
	}
}
