/**
 *
 *  intro_scene.c
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
#include "pl_texture.h"
#include "game.h"
#include "screen.h"
#include "scenes.h"
#include "player.h"

static EGE_Scene*	uiSc;
static EGE_Anime*	sceneAnime;
EGE_RGroup*		grp;

Eina_Bool	EGE_Intro_Event_Handle(SDL_Event *e);
char * text1[] = {
	"For wimp !",
	"Getting started",
	"Okay for a warior",
	"short and sweet",
	"Mumff?",
	"Getting harder",
	"Getting even",
	"Let's clean",
	"Now this is it",
	"Final"
};
char * text2[] = {
	"Warming up",
	"Feeling nice",
	"We're climbing it",
	"Mumff?",
	"First Boss",
	"Yeap yeap yeap",
	"Harder !",
	"This mess",
	"The real deal",
	"Last Boss"
};
char text0[80];
char text3[] = "Get READY !!!";
char *t0, *t1, *t2, *t3;
EGE_vector pos;

void		runTickIntroSceneAnim(EGE_Anime* a) {
	char 			buf[2];
	stbtt_aligned_quad	q;
	float 			x = fp_to_float(pos[0]);
	float 			y = fp_to_float(pos[1]);

	buf[1]		= 0;
	switch (a->tickLeft%4) {
	case 0:
		// add a character every 4 ticks
		if (*t0!=0) {
			buf[0] = *t0;
			EGE_RGroup_Text_add(grp, buf, pos);
			stbtt_GetBakedQuad(grp->f->cdata, 512,512, *t0-32, &x,&y,&q,1);
			pos[0] = fp_from_float(x+3);
			t0++;
			if (*t0==0) {pos[0] = fp_from_int(400-8*strlen(t1));pos[1]+=fp_from_int(30);}
		} else if (*t1!=0) {
			buf[0] = *t1;
			EGE_RGroup_Text_add(grp, buf, pos);
			stbtt_GetBakedQuad(grp->f->cdata, 512,512, *t1-32, &x,&y,&q,1);
			pos[0] = fp_from_float(x+3);
			t1++;
			if (*t1==0) {pos[0] = fp_from_int(400-8*strlen(t2));pos[1]+=fp_from_int(30);}
		} else if (*t2!=0) {
			buf[0] = *t2;
			EGE_RGroup_Text_add(grp, buf, pos);
			stbtt_GetBakedQuad(grp->f->cdata, 512,512, *t2-32, &x,&y,&q,1);
			pos[0] = fp_from_float(x+3);
			t2++;
			if (*t2==0) {pos[0] = fp_from_int(250);pos[1]+=fp_from_int(80);}
		} else if (*t3!=0) {
			buf[0] = *t3;
			EGE_RGroup_Text_add(grp, buf, pos);
			stbtt_GetBakedQuad(grp->f->cdata, 512,512, *t3-32, &x,&y,&q,1);
			pos[0] = fp_from_float(x+10);
			t3++;
		} else if (a->tickLeft>40) a->tickLeft = 30;
	break;
	default:break;
	}
	if (a->tickLeft<5) {
		EGE_Game_Scene_set_ui();
	}
}

void		EGE_Intro_Scene_init(void) {
		grp		= EGE_RGroupSprite_new(500, EINA_FALSE, EINA_TRUE);

	grp->alpha		= EINA_TRUE;
	uiSc			= EGE_Scene_new();
	uiSc->onEvent		= EGE_Intro_Event_Handle;
	sceneAnime		= NULL;
	t0			= NULL;
	t1			= NULL;
	t2			= NULL;
	t3			= NULL;


 	EGE_RGroup_set_font(grp, EGE_Game_find_Font(g, "subTit"));
	EGE_Scene_add_rgroup(uiSc, grp);
}

void		EGE_Intro_Scene_set(int level) {
	if (sceneAnime==NULL) {
		sceneAnime	= EGE_Scene_add_anime(uiSc, NULL);
	}
	sprintf(text0, "Level %d :",level);
	sceneAnime->custom	= runTickIntroSceneAnim;
	sceneAnime->tickLeft	= 300;
	t0			= text0;
	t1			= text1[level-1];
	t2			= text2[level-1];
	t3			= text3;
	pos[0]			= fp_from_int(400-8*strlen(text0));
	pos[1]			= fp_from_int(40);
	pos[2]			= fp_05;
	EGE_setUIScene(uiSc);
	EGE_RGroup_empty(grp);
}

void		EGE_Intro_Scene_free(void) {
	EGE_Scene_free(uiSc);	
}

Eina_Bool	EGE_Intro_Event_Handle(SDL_Event *e) {
	Eina_Bool quit = EINA_FALSE;
	switch(e->type) {
	case SDL_QUIT:
		quit = EINA_TRUE;
	break;
	case SDL_KEYUP:
	case SDL_MOUSEBUTTONUP:
		EGE_Game_Scene_set_ui();
	break;
#ifdef CAANOO
	case SDL_JOYBUTTONUP:
		if (e->jbutton.button == 6)
			EGE_Game_Scene_set_ui();
	break;
#endif
	default:
	break;
	}
	return quit;
}
