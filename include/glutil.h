//###########################################################################################################//
//
//  Copyright(c) 2012, Algorithmic Ecology / Anthony Rodriguez (anth.jrod@gmail.com)
//  Copyright not applicable in segments where specific credit is given (ex: printw())
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
//    glutil.h
//    General UI utilities
//
//############################################################################################################//

#ifndef GLUTIL_H
#define GLUTIL_H

#include "glref.h"
//#include "atoms.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
//#include "linear.h"


const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT =  900;
const int SCREEN_FPS = 60;

//Warp: Don't render, skip forward
typedef enum SPEED_MODE{ Pause, Normal, Warp } SPEED_MODE;

//Initialize
bool GL_Init();

//Per-frame operations
void GL_Update();

//Renders and presents buffer
void GL_Render();

//Print text to screen
//Credit: http://mycodelog.com/2010/03/23/printw/
void printw(float x, float y, float z, char* format, ...);

//Mouse movement
void handleMotion(int x, int y);

//handle keyboard inputs
void handleKeys(unsigned char key, int x, int y);

//handle mouse clicks
void handleClicks(int button, int state, int x, int y);

//set mode to normal when window is being closed (avoids crashes)
void safeClose();

#endif
