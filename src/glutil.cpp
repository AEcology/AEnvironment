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
//    glutil.cpp
//    General UI items
//
//############################################################################################################//


#include "glutil.h"
#include "environment.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <GL/glut.h>


extern bool loadFromFile;
extern string fileName;
extern SPEED_MODE SPEEDMODE;
int SAVE_MESSAGE_COUNTER = 0;
extern Environment* testEnvironment;
extern vec2* SCREEN_POSITION;

//currBest 50,30
//Environment testEnvironment(50,30, 0, loadFromFile, fileName);


bool GL_Init(){
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, 1.0, -1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(0.06f, 0.06f, 0.06f, 0.06f); //Matte black


	GLenum error = glGetError();
	if (error != GL_NO_ERROR){
		printf("OpenGL initialization error: %s\n", gluErrorString(error));
		return false;
	}
	else return true;
}

void GL_Update(){
}

void safeClose(){
	SPEEDMODE = Normal;
}

void RenderSpeedButton(){
	float reference_Y = 0;
	float reference_X = 0;
	reference_X = SCREEN_WIDTH*.95f;
	reference_Y = SCREEN_HEIGHT*.95f;
	glBegin(GL_TRIANGLES);
	glColor3f(0.049f, 0.7f, 0.5624f);
	glVertex2f(reference_X - 10, reference_Y - 10);
	glVertex2f(reference_X - 10, reference_Y + 10);
	glVertex2f(reference_X, reference_Y);
	glEnd();
	if (SPEEDMODE == Normal){
		glBegin(GL_TRIANGLES);
		glColor3f(0.049f, 0.7f, 0.5624f);
		glVertex2f(reference_X, reference_Y - 10);
		glVertex2f(reference_X, reference_Y + 10);
		glVertex2f(reference_X + 10, reference_Y);
		glEnd();
	}
}

void RenderPauseButton(){
	float reference_Y = 0;
	float reference_X = 0;
	reference_X = SCREEN_WIDTH*.9f;
	reference_Y = SCREEN_HEIGHT*.95f-10;
	glBegin(GL_QUADS);
	glColor3f(0.049f, 0.7f, 0.5624f);
	glVertex2f(reference_X, reference_Y + 20);
	glVertex2f(reference_X, reference_Y);
	glVertex2f(reference_X+4, reference_Y);
	glVertex2f(reference_X + 4, reference_Y+20);
	glEnd();
	reference_X += 7;
	glBegin(GL_QUADS);
	glColor3f(0.049f, 0.7f, 0.5624f);
	glVertex2f(reference_X, reference_Y + 20);
	glVertex2f(reference_X, reference_Y);
	glVertex2f(reference_X + 4, reference_Y);
	glVertex2f(reference_X + 4, reference_Y + 20);
	glEnd();
}

//Source: http://mycodelog.com/2010/03/23/printw/
void printw(float x, float y, float z, char* format, ...){
        //TODO Make this work (uses functions specific to Microsoft)

	/*va_list args;   //  Variable argument list
	int len;        // String length
	int i;          //  Iterator
	char * text;    // Text

	//  Fonts supported by GLUT are: GLUT_BITMAP_8_BY_13,
	//  GLUT_BITMAP_9_BY_15, GLUT_BITMAP_TIMES_ROMAN_10,
	//  GLUT_BITMAP_TIMES_ROMAN_24, GLUT_BITMAP_HELVETICA_10,
	//  GLUT_BITMAP_HELVETICA_12, and GLUT_BITMAP_HELVETICA_18.
	GLvoid *font_style = GLUT_BITMAP_HELVETICA_12;


	//  Initialize a variable argument list
	va_start(args, format);

	//  Return the number of characters in the string referenced the list of arguments.
	// _vscprintf doesn't count terminating '\0' (that's why +1)
	len = _vscprintf(format, args) + 1;

	//  Allocate memory for a string of the specified size
	text = (char*)malloc(len);

	//  Write formatted output using a pointer to the list of arguments
	vsprintf_s(text, len, format, args);

	//  End using variable argument list
	va_end(args);

	//  Specify the raster position for pixel operations.
	glRasterPos3f(x, y, z);

	//  Draw the characters one by one
	for (i = 0; text[i] != '\0'; i++)
		glutBitmapCharacter(font_style, text[i]);

	//  Free the allocated memory for the string
	//delete text;
	free(text);*/
}

void GL_Render(){


	//Clear color buffer
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//move to center
	glTranslatef(0.0, 0.0, 0.f);

	if (SPEEDMODE!=Pause)
		testEnvironment->Update();
	
	if (SAVE_MESSAGE_COUNTER > 0){
		printw(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 0, "Genome state saved");
		--SAVE_MESSAGE_COUNTER;
	}

	//Draw buttons:
	RenderSpeedButton();
	RenderPauseButton();

	glutSwapBuffers();
}

void handleMotion(int x, int y){
	int offsetX = ((x - (float)SCREEN_WIDTH / 2)/(float)SCREEN_WIDTH)*40;
	int offsetY = ((y - (float)SCREEN_HEIGHT / 2)/(float)SCREEN_HEIGHT)*40;
	SCREEN_POSITION->x = SCREEN_POSITION->x + offsetX;
	SCREEN_POSITION->y = SCREEN_POSITION->y + offsetY;
	if (SCREEN_POSITION->x + SCREEN_WIDTH > WORLD_WIDTH)
		SCREEN_POSITION->x = WORLD_WIDTH - SCREEN_WIDTH;
	else if (SCREEN_POSITION->x < 0)
		SCREEN_POSITION->x = 0;
	if (SCREEN_POSITION->y + SCREEN_HEIGHT > WORLD_HEIGHT)
		SCREEN_POSITION->y = WORLD_HEIGHT - SCREEN_HEIGHT;
	else if (SCREEN_POSITION->y < 0)
		SCREEN_POSITION->y = 0;

}


void handleKeys(unsigned char key, int x, int y){
	switch (key){
	case 's':
		testEnvironment->SaveCurrentState(false);
		SAVE_MESSAGE_COUNTER = 240;
		break;
	case 'd':
		if (SCREEN_POSITION->x>0)
			SCREEN_POSITION->x = SCREEN_POSITION->x - 10;
		break;
	case 'g':
		if (SCREEN_POSITION->x<WORLD_WIDTH-SCREEN_WIDTH)
			SCREEN_POSITION->x = SCREEN_POSITION->x + 10;
		break;
	case 'r':
		if (SCREEN_POSITION->y>0)
			SCREEN_POSITION->y = SCREEN_POSITION->y - 10;;
		break;
	case 'f':
		if (SCREEN_POSITION->y<WORLD_HEIGHT-SCREEN_HEIGHT)
			SCREEN_POSITION->y = SCREEN_POSITION->y + 10;
		break;
	deafault:
		break;

	}

}

void handleClicks(int button, int state, int x, int y){
	if (!(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN))
		return; //we only care about left clicks
	else if (y > SCREEN_HEIGHT*.95f-10.f && y<SCREEN_HEIGHT*.95f + 10.f && x>SCREEN_WIDTH*.9f && x < SCREEN_WIDTH*.9f + 11.f)
		SPEEDMODE = Pause;
	else if (y>SCREEN_HEIGHT*.95f-10.f && y<SCREEN_HEIGHT*.95f + 10.f && x>SCREEN_WIDTH*.95f-10.f && x < SCREEN_WIDTH*.95f + 10.f){
		if (SPEEDMODE == Pause || SPEEDMODE == Warp)
			SPEEDMODE = Normal;
		else SPEEDMODE = Warp;
	}
	else testEnvironment->ClickSet(x, y);
}
