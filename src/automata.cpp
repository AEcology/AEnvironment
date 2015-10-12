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
//    General functionality for Analytical agents
//
//############################################################################################################//

#include "object.h"
#include "automata.h"
#include "environment.h"
#include <vector>
#include <algorithm>

#define PI 3.1415926f

extern SPEED_MODE SPEEDMODE;
extern vec2* SCREEN_POSITION;

Hunter::Hunter(){
	isAtom = false;
	isSugar = false;
	isArsenic = false;
	isHunter = true;
	isSquare = false;
	isCircle = true;
	direction = 0;
	distance = SCREEN_HEIGHT;
	focusAtom = NULL;
	this->position = new vec2(static_cast<float>(rand() % (WORLD_WIDTH)), static_cast<float>(rand() % (WORLD_HEIGHT)));
	this->moveSpeed = HUNTER_SPEED;
	this->energy = 1000;
}

Hunter::~Hunter(){
	delete this->position;
}

bool Hunter::Move(float direction){
	if (direction >  2 * PI || direction < -2 * PI)
		return false;
	float newx = position->x + this->moveSpeed*cosf(direction);
	float newy = position->y + this->moveSpeed*sinf(direction);
	//Enforce toroidal transitions:
	if (newx < 0)
		newx = WORLD_WIDTH - HUNTER_HALFSIZE;
	else if (newx > WORLD_WIDTH)
		newx = HUNTER_HALFSIZE;
	else if (newy < 0)
		newy = WORLD_HEIGHT - HUNTER_HALFSIZE;
	else if (newy > WORLD_HEIGHT)
		newy = HUNTER_HALFSIZE;
	position->x = newx;
	position->y = newy;
	return true;
}

bool Hunter::FindPrey(vector<Object*> allObjects){
	int vecSize = allObjects.size();
	float smallestDist;
	float currDist = SCREEN_HEIGHT;

	if (vecSize <= 1)
		return false;
	
	distance = SCREEN_HEIGHT;
	focusAtom = NULL;
	//find closest Atom
	for (int i = 0; i < vecSize; ++i){
		if (allObjects.at(i)!=this && allObjects.at(i)->isHunter){
			if (Distance(this->position, allObjects.at(i)->position) < HUNTER_HALFSIZE){
				this->position->y = rand() % WORLD_HEIGHT;
				this->position->x = rand() % WORLD_WIDTH;
			}
		}
		if (allObjects.at(i)->isAtom && allObjects.at(i)->energy>0){
			currDist = Distance(this->position, allObjects.at(i)->position);
			if (currDist < distance){
				focusAtom = (Atom*)allObjects.at(i);
				distance = currDist;
			}
		}
	}
	return true;
}

bool Hunter::Render(){
	if (SPEEDMODE == Warp)
		return true;
	int i = 0;
	float xtemp = 0;
	float ytemp = 0;
	float theta = direction;
	float eyeX = position->x + 2.f*HUNTER_HALFSIZE / 3.f*cosf(direction);
	float eyeY = position->y + 2.f*HUNTER_HALFSIZE / 3.f*sinf(direction);
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(HUNTER_COLOR_0, HUNTER_COLOR_1, HUNTER_COLOR_2);
	for (i = 0; i < 24; ++i){
		theta = 2.f*PI*static_cast<float>(i) / 24.f;
		xtemp = HUNTER_HALFSIZE*cosf(theta);
		ytemp = HUNTER_HALFSIZE*sinf(theta);
		glVertex2f(position->x + xtemp, position->y + ytemp);
	}
	glEnd();
	glColor3f(HUNTER_EYECOLOR_0, HUNTER_EYECOLOR_1, HUNTER_EYECOLOR_2);
	glBegin(GL_TRIANGLE_FAN);
	for (i = 0; i < 12; ++i){
		theta = 2.f*PI*static_cast<float>(i) / 12.f;
		xtemp = HUNTER_EYE_RADIUS*cosf(theta);
		ytemp = HUNTER_EYE_RADIUS*sinf(theta);
		glVertex2f(eyeX + xtemp, eyeY + ytemp);
	}
	glEnd();
	return true;
}

bool Hunter::Render3D(){
	if (SPEEDMODE == Warp)
		return true;
	float eyeX = position->x + 2.f*HUNTER_HALFSIZE / 3.f*cosf(direction);
	float eyeY = position->y + 2.f*HUNTER_HALFSIZE / 3.f*sinf(direction);

	GLfloat hunterColor[] = {HUNTER_COLOR_0, HUNTER_COLOR_1, HUNTER_COLOR_2, 1 };
	GLfloat hunterSpecular[] = { 0.01, 0.01, 0.01, 1 };
	GLfloat eyeColor[] = { HUNTER_EYECOLOR_0, HUNTER_EYECOLOR_1, HUNTER_EYECOLOR_2, 1 };
	GLfloat eyeSpecular[] = { .4, .4, .4, 1 };

	GLfloat qaAmbientLight[] = { 0.1, 0.1, 0.1, 1.0 };
	GLfloat qaDiffuseLight[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat qaSpecularLight[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat emitLight[] = { 0.9, 0.9, 0.9, 0.01 };
	GLfloat Noemit[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat qaLightPosition[] = { 400, 0, 500, 1 };
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
	glTranslatef(this->position->x-SCREEN_POSITION->x, this->position->y-SCREEN_POSITION->y, -HUNTER_HALFSIZE);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, hunterColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, hunterColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, hunterSpecular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20);

	glutSolidSphere(HUNTER_HALFSIZE, 25, 25);

	glFlush();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(eyeX - SCREEN_POSITION->x, eyeY - SCREEN_POSITION->y, -HUNTER_EYE_RADIUS);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, eyeColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, eyeColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, eyeSpecular);
	glutSolidSphere(HUNTER_EYE_RADIUS, 12, 12);

	glFlush();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, HUNTER_HALFSIZE, -HUNTER_HALFSIZE);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	return true;
}

//Move toward Prey of interest
bool Hunter::Pursue(){
	float tempDirection = 0;
	
	if (focusAtom == NULL)
		return true;
	vec2 *src = new vec2(this->position->x, this->position->y); //unit vector of agent
	vec2 *dest = new vec2(focusAtom->position->x, focusAtom->position->y); //unit vector for object of interest
	vec2 *direction = new vec2(0,0);
	vec2 *magnitude = new vec2(0, 0);

	direction->x = 1.0;  //cos(0)
	direction->y = 0.0;  //sin(0)
	magnitude->x = dest->x - src->x;
	magnitude->y = dest->y - src->y;
	Normalize(magnitude);
	//Get the angle between the two (absolute value)
	
	
	this->direction = acos(Dot(magnitude, direction));
	//Rotate direction vector 90 degrees and Dot again to get sign
	tempDirection = PI / 2;
	direction->x = cos(tempDirection);
	direction->y = sin(tempDirection);
	magnitude->x = dest->x - src->x;
	magnitude->y = dest->y - src->y;
	if (Dot(magnitude, direction) < 0)
		this->direction = this->direction*-1;
	Move(this->direction);

	delete src;
	delete dest;
	delete magnitude;
	delete direction;

	return true;
}

bool Hunter::Update(vector<Object*> allObjects){
	
	if (allObjects.size() <= 1)
		return false;

	if (counter > 0)
		--counter;
	else
		counter = 8;

	if (counter == 0)
		if (energy>0)
			--energy;


	FindPrey(allObjects);
	if (focusAtom!= NULL){
		Pursue();
		if (distance < (HUNTER_HALFSIZE + focusAtom->halfSize)){
			this->energy = min(this->energy + focusAtom->energy, 1000);
			focusAtom->energy = focusAtom->energy - HUNTER_ATTACK_DAMAGE;
			focusAtom = NULL;
			distance = SCREEN_HEIGHT;
		}
	}

	double arg = 1 - 0.001*(double)(energy);
	this->moveSpeed = 0.7 + 15 * max(arg, 0.04);


	return true;
}