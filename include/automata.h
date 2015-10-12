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
//    automata.h
//    General declarations for Analytical AI ( In this case, just Hunter(s) )
//
//############################################################################################################//


#include "object.h"
#include "atoms.h"

#ifndef AUTOMATA_H
#define AUTOMATA_H

#define HUNTER_SPEED 2.f

const int HUNTER_ATTACK_DAMAGE = 100;
const float HUNTER_COLOR_0 = 1.f;
const float HUNTER_COLOR_1 = 1.f;
const float HUNTER_COLOR_2 = 1.f;
const float HUNTER_EYECOLOR_0 = 1.f;
const float HUNTER_EYECOLOR_1 = 0.f;
const float HUNTER_EYECOLOR_2 = 0.f;
const float HUNTER_SIZE = 26.f;
const float HUNTER_HALFSIZE = HUNTER_SIZE / 2.f;
const float HUNTER_EYE_RADIUS = HUNTER_SIZE / 5.f;

class Hunter : public Object{
private:
	Atom* focusAtom;
	//direction and distance to prey:
	float direction;
	float distance;
	float moveSpeed;

	int energy;
	int counter;

	bool Pursue();
	bool Move(float direction);

public:
	Hunter();
	~Hunter();
	//Find prey and set to focusAtom
	bool FindPrey(vector<Object*> allObjects);
	bool Update(vector<Object*> allObjects);
	bool Render();
	bool Render3D();

	friend class Environment;
};


#endif