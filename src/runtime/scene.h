/**
 *
 *  scene.h
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
#ifndef __EGE_SCENE_H__
#define __EGE_SCENE_H__

typedef struct EGE_Anime_ EGE_Anime;
typedef void (*EGE_Anime_CB)(EGE_Anime* a);
typedef void (*EGE_Joy_CB)(void);
typedef struct EGE_Scene_ EGE_Scene;
#include "engine.h"
#include "rgroup.h"

struct EGE_Anime_ {
	EGE_Segment*	seg;
	int		tickLeft;
	EGE_Anime_CB	onEnd;
	EGE_Anime_CB	custom;
	EGE_vector	move;
	EGE_vector	rot;
	EGE_real	scale;
	void*		obj;
	int		objid;
};

EGE_Anime*	EGE_Anime_new(EGE_Segment* s);

struct  EGE_Scene_ {
	Eina_List*	rgroups;
	Eina_List*	animes;
	EGE_Event_CB	onEvent;
	EGE_Joy_CB	joystickR;
};

extern EGE_Scene*	gameScene;
extern EGE_Scene*	uiScene;

EGE_Scene*	EGE_Scene_new();
void		EGE_Scene_free(EGE_Scene* s);
void		EGE_Scene_empty(EGE_Scene* s);
void		EGE_Scene_draw(EGE_Scene* s);
void		EGE_Scene_runTick(EGE_Scene* s);
void		EGE_Scene_add_rgroup(EGE_Scene* s, EGE_RGroup *m);
EGE_Anime*	EGE_Scene_add_anime(EGE_Scene* s, EGE_Segment* seg);
#endif
