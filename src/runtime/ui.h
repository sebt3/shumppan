/**
 *
 *  ui.h
 *  Copyright (C) 2013-2014 Sebastien Huss
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *  THE SOFTWARE.
 *
 */
#ifndef __EGE_UI_H__
#define __EGE_UI_H__
#include "scene.h"

/***********************************************************************************
 *				ProgressBar
 ***********************************************************************************/


typedef struct {
	int		value;
	int		max;
	EGE_Segment*	box;
	EGE_Segment*	lseg;
	EGE_Segment*	rseg;
	EGE_RGroup*	rg;
	EGE_Scene*	scene;
} EGE_progressBar;

EGE_progressBar* EGE_progressBar_new(EGE_vector tl, EGE_vector br, EGE_RGroup* rg, EGE_Scene* scene);
void		 EGE_progressBar_set(EGE_progressBar* p, int value);
void		 EGE_progressBar_setMax(EGE_progressBar* p, int max);
void		 EGE_progressBar_setLBColors(EGE_progressBar* p);
void		 EGE_progressBar_setSimpleColors(EGE_progressBar* p);
void		 EGE_progressBar_free(EGE_progressBar* p);

/***********************************************************************************
 *				Butttons-Cursor
 ***********************************************************************************/


typedef void (*EGE_Button_CB)();
typedef struct {
	EGE_vector	tl, br;
	EGE_Joy_CB	onClick;
	char*		label;
	Eina_Bool	quit;
} EGE_Button;

EGE_Button*	EGE_Button_new(EGE_vector tl, EGE_vector br, char* label, EGE_Joy_CB onClick);
void		EGE_Button_free(EGE_Button* b);

typedef struct {
	int		curButton;
	int		maxButton;
	int		width;
	char*		value; // only for keyboard
	EGE_Button**	buttons;
	EGE_Segment*	seg;
	EGE_RGroup*	rg;
	EGE_RGroup*	txt;
	EGE_Scene*	scene;
} EGE_Cursor;

EGE_Cursor*	 EGE_Cursor_new(int maxButton, int width, EGE_RGroup* rg, EGE_RGroup* txt, EGE_Scene* scene);
void		 EGE_Cursor_free(EGE_Cursor* c);
void		 EGE_Cursor_set_button(EGE_Cursor* c, int id, EGE_Button* b);
void		 EGE_Cursor_set_Quitbutton(EGE_Cursor* c, int id);
Eina_Bool	 EGE_Cursor_Event_Handle(EGE_Cursor* c, SDL_Event *e);

// Keyboard is a specific cursor
EGE_Cursor*	 EGE_Keyboard_new(EGE_RGroup* rg, EGE_RGroup* txt, EGE_Scene* scene, EGE_Joy_CB onSubmit);

#endif
