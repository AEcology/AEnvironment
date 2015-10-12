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
//    food.cpp
//    Definitions for food and such
//
//############################################################################################################//


#include "food.h"
#include "linear.h"
#include "glutil.h"
#include "environment.h"
#include <math.h>
#include <stdio.h>
#include <iostream>

extern SPEED_MODE SPEEDMODE;
extern vec2* SCREEN_POSITION;

Sugar::Sugar(){
	isAtom = false;
	isSugar = true;
	isHunter = false;
	isSquare = true;
	isCircle = false;
	isArsenic = false;
	color[0] = 0.91f;
	color[1] = 0.91f;
	color[2] = 0.91f;

	float posx =  rand() % static_cast<int>(WORLD_WIDTH);
	float posy =  rand() % static_cast<int>(WORLD_HEIGHT);

	energy = 100;
	size = energy/10  + 1;
	needsUpdate = false;

	position = new vec2(posx, posy);
	float xlow = position->x - size;
	float xhigh = position->x + size;
	float ylow = position->y + size;
	float yhigh = position->y - size;

	v1 = new vec2(xlow, ylow);
	v2 = new vec2(xlow, yhigh);
	v3 = new vec2(xhigh, yhigh);
	v4 = new vec2(xhigh, ylow);
}

Sugar::~Sugar(){
	delete position;
	delete v1;
	delete v2;
	delete v3;
	delete v4;
}

bool Sugar::Update(){
	if (energy < 0)
		return false;
	if (needsUpdate){
		size = (energy / 10) + 1;
		needsUpdate = false;
		float xlow = position->x - size;
		float xhigh = position->x + size;
		float ylow = position->y + size;
		float yhigh = position->y - size;

		v1->x = xlow;
		v1->y = ylow;
		v2->x = xlow;
		v2->y = yhigh;
		v3->x = xhigh;
		v3->y = yhigh;
		v4->x = xhigh;
		v4->y = ylow;
	}
	return true;
}

void Sugar::Render(){
	if (SPEEDMODE == Warp)
		return;
	glBegin(GL_QUADS);
	glColor3f(0.91f, 0.91f, 0.91f);
	glVertex2f(v1->x - SCREEN_POSITION->x, v1->y - SCREEN_POSITION->y);
	glVertex2f(v2->x - SCREEN_POSITION->x, v2->y - SCREEN_POSITION->y);
	glVertex2f(v3->x - SCREEN_POSITION->x, v3->y - SCREEN_POSITION->y);
	glVertex2f(v4->x - SCREEN_POSITION->x, v4->y - SCREEN_POSITION->y);
	glEnd();
}


Arsenic::Arsenic(){
	isAtom = false;
	isSugar = false;
	isArsenic = true;
	isHunter = false;
	isSquare = true;
	isCircle = false;
	color[0] = 1.f;
	color[1] = 0.4052f;
	color[2] = 0.17f;

	float posx = WORLD_WIDTH - rand() % static_cast<int>(WORLD_WIDTH);
	float posy = WORLD_HEIGHT - rand() % static_cast<int>(WORLD_HEIGHT);

	energy = 50;
	size = 3;
	needsUpdate = false;

	position = new vec2(posx, posy);
	float xlow = position->x - size;
	float xhigh = position->x + size;
	float ylow = position->y + size;
	float yhigh = position->y - size;

	v1 = new vec2(xlow, ylow);
	v2 = new vec2(xlow, yhigh);
	v3 = new vec2(xhigh, yhigh);
	v4 = new vec2(xhigh, ylow);
}

Arsenic::~Arsenic(){
	delete position;
	delete v1;
	delete v2;
	delete v3;
	delete v4;
}

bool Arsenic::Update(){
	if (energy < 0)
		return false;
	if (needsUpdate){
		size = (energy / 10) + 1;
		needsUpdate = false;
		float xlow = position->x - size;
		float xhigh = position->x + size;
		float ylow = position->y + size;
		float yhigh = position->y - size;

		v1->x = xlow;
		v1->y = ylow;
		v2->x = xlow;
		v2->y = yhigh;
		v3->x = xhigh;
		v3->y = yhigh;
		v4->x = xhigh;
		v4->y = ylow;
	}
	return true;
}

void Arsenic::Render(){
	if (SPEEDMODE == Warp)
		return;
	glBegin(GL_QUADS);
	glColor3f(color[0], color[1], color[2]);
	glVertex2f(v1->x - SCREEN_POSITION->x, v1->y - SCREEN_POSITION->y);
	glVertex2f(v2->x - SCREEN_POSITION->x, v2->y - SCREEN_POSITION->y);
	glVertex2f(v3->x - SCREEN_POSITION->x, v3->y - SCREEN_POSITION->y);
	glVertex2f(v4->x - SCREEN_POSITION->x, v4->y - SCREEN_POSITION->y);
	glEnd();
}