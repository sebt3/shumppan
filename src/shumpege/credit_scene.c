/**
 *
 *  credit_scene.c
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
Eina_Bool	EGE_Credit_Event_Handle(SDL_Event *e);

void		EGE_Credit_Scene_init(void) {
	EGE_RGroup*	grp	= EGE_RGroupSprite_new(  1, EINA_FALSE, EINA_FALSE);
	EGE_RGroup*	grpTT	= EGE_RGroupSprite_new( 20, EINA_FALSE, EINA_TRUE);
	EGE_RGroup*	grpMN	= EGE_RGroupSprite_new( 20, EINA_FALSE, EINA_TRUE);
	EGE_RGroup*	grpCD	= EGE_RGroupSprite_new(200, EINA_FALSE, EINA_TRUE);
	EGE_RGroup*	grpTC	= EGE_RGroupSprite_new(200, EINA_FALSE, EINA_TRUE);
	EGE_vector p		= {fp_1*120,fp_1*80,fp_05};
	EGE_vector q		= {fp_1*680,fp_1*400,fp_05};
	EGE_quater grpc 	= {fp_from_float(0.2f), fp_from_float(0.2f),fp_from_float(0.2f),fp_from_float(0.7f)};
	EGE_quater grpTc 	= {fp_from_float(0.9f), fp_from_float(0.9f),fp_from_float(0.9f),fp_1};
	EGE_quater grpMc 	= {fp_from_float(0.7f), fp_from_float(0.7f),fp_from_float(0.7f),fp_1};

	grp->alpha		= EINA_TRUE;
	uiSc			= EGE_Scene_new();
	uiSc->onEvent		= EGE_Credit_Event_Handle;

	EGE_RGroup_Rect_add(grp,p,q);
	EGE_Scene_add_rgroup(uiSc, grp);

	EGE_RGroup_set_font(grpTT, EGE_Game_find_Font(g, "Title"));
	EGE_RGroup_set_font(grpMN, EGE_Game_find_Font(g, "subTit"));
	EGE_RGroup_set_font(grpCD, EGE_Game_find_Font(g, "Credit"));
	EGE_RGroup_set_font(grpTC, EGE_Game_find_Font(g, "TextC"));
	EGE_RGroup_uniq_col_set(grp, grpc);
	EGE_RGroup_uniq_col_set(grpTT, grpTc);
	EGE_RGroup_uniq_col_set(grpMN, grpMc);

	p[0]+=fp_from_int(100);
	p[1]+=fp_from_int(30);
	EGE_RGroup_Text_add(grpTT, GAME_NAME,p);
	p[0]+=fp_from_int(130);
	p[1]+=fp_from_int(30);
	EGE_RGroup_Text_add(grpMN, "Credits",p);
	p[0]=fp_from_int(180);
	p[1]+=fp_from_int(35);
	EGE_RGroup_Text_add(grpTC, "Code :",p);
	p[1]+=fp_from_int(25);p[0]+=fp_from_int(30);
	EGE_RGroup_Text_add(grpCD, "sebt3 (sebt3.openpandora.org)",p);
	p[1]+=fp_from_int(35);p[0]-=fp_from_int(30);
	EGE_RGroup_Text_add(grpTC, "Sprites & font :",p);
	p[1]+=fp_from_int(25);p[0]+=fp_from_int(30);
	EGE_RGroup_Text_add(grpCD, "Kenney (www.kenney.nl)",p);
	p[1]+=fp_from_int(35);p[0]-=fp_from_int(30);
	EGE_RGroup_Text_add(grpTC, "Background tiles :",p);
	p[1]+=fp_from_int(25);p[0]+=fp_from_int(30);
	EGE_RGroup_Text_add(grpCD, "Daniel Cook - Hard Vacuum",p);
	p[1]+=fp_from_int(20);
	EGE_RGroup_Text_add(grpCD, "http://www.lostgarden.com/",p);
	EGE_Scene_add_rgroup(uiSc, grpTT);
	EGE_Scene_add_rgroup(uiSc, grpMN);
	EGE_Scene_add_rgroup(uiSc, grpTC);
	EGE_Scene_add_rgroup(uiSc, grpCD);
}

void		EGE_Credit_Scene_set(void) {
	EGE_setUISceneAnime(uiSc, 0);
}

void		EGE_Credit_Scene_free(void) {
	EGE_Scene_free(uiSc);	
}

Eina_Bool	EGE_Credit_Event_Handle(SDL_Event *e) {
	Eina_Bool quit = EINA_FALSE;
	switch(e->type) {
	case SDL_QUIT:
		quit = EINA_TRUE;
	break;
	case SDL_KEYUP:
	case SDL_MOUSEBUTTONUP:
		EGE_Menu_Scene_set_ui();
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
