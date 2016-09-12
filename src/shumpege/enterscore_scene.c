/**
 *
 *  enterscore_scene.c
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
#include "score.h"
#include "player.h"

static EGE_Scene*	uiSc;
static EGE_Cursor*	keyboard;
EGE_RGroup*		grpVAL;
EGE_RGroup*		grpl2;
Eina_Bool	EGE_enterScore_Event_Handle(SDL_Event *e);

void		EGE_enterScore_submit(void) {
	EGE_HighScore_Scene_Submit(keyboard->value, score.totalScore, score.currentLevel, score.totalKills);
	EGE_HighScore_Scene_set();
}

void		EGE_enterScore_Scene_init(void) {
	EGE_RGroup*	grp	= EGE_RGroupSprite_new(  3, EINA_FALSE, EINA_FALSE);
	EGE_RGroup*	grpTT	= EGE_RGroupSprite_new( 20, EINA_FALSE, EINA_TRUE);
	EGE_RGroup*	grpCUR	= EGE_RGroupSprite_new(  1, EINA_FALSE, EINA_FALSE);
	EGE_RGroup*	grpMN	= EGE_RGroupSprite_new( 80, EINA_FALSE, EINA_TRUE);
	EGE_RGroup*	grpTXT	= EGE_RGroupSprite_new( 80, EINA_FALSE, EINA_TRUE);
			grpl2	= EGE_RGroupSprite_new( 80, EINA_FALSE, EINA_TRUE);
			grpVAL	= EGE_RGroupSprite_new( 80, EINA_FALSE, EINA_TRUE);
	EGE_vector p		= {fp_1*120,fp_1*80, fp_05};
	EGE_vector q		= {fp_1*680,fp_1*300,fp_05};
	EGE_vector p2		= {fp_1*180,fp_1*330,fp_05};
	EGE_vector q2		= {fp_1*600,fp_1*470,fp_05};
	EGE_vector p3		= {fp_1*280,fp_1*240,fp_05};
	EGE_vector q3		= {fp_1*500,fp_1*270,fp_05};
	EGE_vector t1		= {fp_1*170,fp_1*180,fp_05};
	EGE_vector t2		= {fp_1*210,fp_1*250,fp_05};
	EGE_quater grpc 	= {fp_from_float(0.2f), fp_from_float(0.2f),fp_from_float(0.2f),fp_from_float(0.7f)};
	EGE_quater grpMc 	= {fp_from_float(0.1f), fp_from_float(0.1f),fp_from_float(0.1f),fp_1};

	grp->alpha		= EINA_TRUE;
	uiSc			= EGE_Scene_new();
	uiSc->onEvent		= EGE_enterScore_Event_Handle;

	EGE_RGroup_Rect_add(grp,p,q);
	EGE_RGroup_Rect_add(grp,p2,q2);
	EGE_RGroup_Rect_add(grp,p3,q3);
	EGE_Scene_add_rgroup(uiSc, grp);

	EGE_RGroup_set_font(grpVAL,EGE_Game_find_Font(g, "Credit"));
	EGE_RGroup_set_font(grpTT, EGE_Game_find_Font(g, "Title"));
 	EGE_RGroup_set_font(grpMN, EGE_Game_find_Font(g, "Credit"));
 	EGE_RGroup_set_font(grpTXT,EGE_Game_find_Font(g, "Credit"));
 	EGE_RGroup_set_font(grpl2, EGE_Game_find_Font(g, "subTit"));
	EGE_RGroup_uniq_col_set(grp,   grpc);
	EGE_RGroup_uniq_col_set(grpMN, grpMc);

	p[0]+=fp_from_int(100);
	p[1]+=fp_from_int(30);
	EGE_RGroup_Text_add(grpTT,  GAME_NAME, p);
	EGE_RGroup_Text_add(grpTXT, "Congratulation you made a high score !", t1);
	EGE_RGroup_Text_add(grpTXT, "Name:", t2);
	
	keyboard	= EGE_Keyboard_new(grpCUR, grpMN, uiSc, EGE_enterScore_submit);

	EGE_Scene_add_rgroup(uiSc, grpCUR);
	EGE_Scene_add_rgroup(uiSc, grpTT);
	EGE_Scene_add_rgroup(uiSc, grpMN);
	EGE_Scene_add_rgroup(uiSc, grpTXT);
	EGE_Scene_add_rgroup(uiSc, grpl2);
	EGE_Scene_add_rgroup(uiSc, grpVAL);
}

void		EGE_enterScore_Scene_set(void) {
	int pos		= EGE_HighScore_getPos(score.totalScore);
	EGE_vector t3	= {fp_1*370,fp_1*210,fp_05};
	char u[4];
	EGE_RGroup_empty(grpl2);
	snprintf(u,4,"#%d!", pos);
	EGE_RGroup_Text_add(grpl2,  u,   t3);

	EGE_Menu_Scene_set();
	if (pos==0)	EGE_HighScore_Scene_set();
	else		EGE_setUISceneAnime(uiSc, 0);
}

void		EGE_enterScore_Scene_free(void) {
	EGE_Scene_free(uiSc);	
}

Eina_Bool	EGE_enterScore_Event_Handle(SDL_Event *e) {
	int len		= strlen(keyboard->value);
	EGE_vector p3	= {fp_1*288,fp_1*248, fp_05};
	Eina_Bool quit = EGE_Cursor_Event_Handle(keyboard, e);
	if (len != strlen(keyboard->value)) {
		EGE_RGroup_empty(grpVAL);
		EGE_RGroup_Text_add(grpVAL, keyboard->value, p3);
	}
	switch(e->type) {
	case SDL_QUIT:
		quit = EINA_TRUE;
	break;
	case SDL_KEYUP:
		switch(e->key.keysym.sym) {
		case SDLK_ESCAPE:
			EGE_Menu_Scene_set_ui();
		break;
		default:
		break;
		}

	break;
#ifdef CAANOO
	case SDL_JOYBUTTONUP:
		if (e->jbutton.button == 6)
			EGE_Menu_Scene_set_ui();
	break;
#endif
	default:
	break;
	}
	return quit;
}
