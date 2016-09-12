/**
 *
 *  menu_scene.c
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
#include "ui.h"

static EGE_Scene*	gameSc;
static EGE_Scene*	uiSc;
static EGE_Cursor*	cur;
static EGE_RGroup*	grpClo;


Eina_Bool	EGE_Menu_Event_Handle(SDL_Event *e);
void		runTickViewMenu(EGE_Anime* a) {
	int i;
	EGE_vector c={fp_1,fp_1,0};
	a->tickLeft++;
	for(i=0;i<2;i++) {
		if (EGE_viewport.br[i]>=EGE_viewport.max[i])
			a->move[i] = -abs(a->move[i]);
		if (EGE_viewport.cur[i]<=EGE_viewport.min[i])
			a->move[i] = abs(a->move[i]);
	}
	EGE_Screen_View_move(a->move);

	// move the clouds
	c[0]	= -a->move[0];
	c[1]	= -a->move[1];
	EGE_RGroup_move_by(grpClo, c);
	if (grpClo->dots[0]>fp2real(EGE_viewport.cur[0])) {
		c[0]	= -fp_1*512;
		c[1]	= 0;
		EGE_RGroup_move_by(grpClo, c);
	} else if (grpClo->dots[0]<fp2real(EGE_viewport.cur[0]-512*fp_1)) {
		c[0]	= fp_1*512;
		c[1]	= 0;
		EGE_RGroup_move_by(grpClo, c);
	}
	if (grpClo->dots[1]<fp2real(EGE_viewport.cur[1]-512*fp_1)) {
		c[0]	= 0;
		c[1]	= fp_1*512;
		EGE_RGroup_move_by(grpClo, c);
	}
	if (grpClo->dots[1]>fp2real(EGE_viewport.cur[1])) {
		c[0]	= 0;
		c[1]	= -fp_1*512;
		EGE_RGroup_move_by(grpClo, c);
	}
}

void		EGE_Menu_Scene_init(void) {
	EGE_RGroup*	tilemap = EGE_RGroup_from_Mesh(EGE_Game_find_Mesh(g, "menu.map"));
	EGE_RGroup*	grp	= EGE_RGroupSprite_new( 1, EINA_FALSE, EINA_FALSE);
	EGE_RGroup*	grpCUR	= EGE_RGroupSprite_new( 1, EINA_FALSE, EINA_FALSE);
	EGE_RGroup*	grpTT	= EGE_RGroupSprite_new(20, EINA_FALSE, EINA_TRUE);
	EGE_RGroup*	grpMN	= EGE_RGroupSprite_new(50, EINA_FALSE, EINA_TRUE);
			grpClo	= EGE_RGroupSprite_new(  6, EINA_FALSE, EINA_TRUE);
	EGE_Texture*	tclo	= EGE_Game_find_Texture(g, "clouds.png");
	EGE_Anime*	a;

	EGE_vector p		= {fp_1*120,fp_1*120,fp_05};
	EGE_vector q		= {fp_1*680,fp_1*360,fp_05};
	EGE_vector t0		= {fp_1*125,fp_1*285,fp_05};
	EGE_vector b0		= {fp_1*255,fp_1*309,fp_05};
	EGE_vector t1		= {fp_1*255,fp_1*285,fp_05};
	EGE_vector b1		= {fp_1*410,fp_1*309,fp_05};
	EGE_vector t2		= {fp_1*410,fp_1*285,fp_05};
	EGE_vector b2		= {fp_1*570,fp_1*309,fp_05};
	EGE_vector t3		= {fp_1*570,fp_1*285,fp_05};
	EGE_vector b3		= {fp_1*675,fp_1*309,fp_05};
	EGE_vector clt		= {0,0,0};
	EGE_vector clb		= {fp_1*512,fp_1*512,0};
	EGE_quater grpcloud 	= {fp_from_float(0.6f), fp_from_float(0.6f),fp_from_float(0.6f),fp_from_float(0.4f)};
	EGE_quater grpc 	= {fp_from_float(0.2f), fp_from_float(0.2f),fp_from_float(0.2f),fp_from_float(0.7f)};
	EGE_quater grpCc 	= {fp_from_float(1.0f), fp_from_float(1.0f),fp_from_float(1.0f),fp_1};
	EGE_quater grpMc 	= {fp_from_float(0.1f), fp_from_float(0.1f),fp_from_float(0.1f),fp_1};

	grp->alpha		= EINA_TRUE;
	uiSc			= EGE_Scene_new();
	gameSc			= EGE_Scene_new();
	cur		 	= EGE_Cursor_new(4, 4, grpCUR, grpMN, uiSc);

	// set event management
	uiSc->onEvent		= EGE_Menu_Event_Handle;
	// set background animation
	a			= EGE_Scene_add_anime(gameSc, NULL);
	a->move[0]		= fp_1;
	a->move[1]		= fp_1;
	a->tickLeft		= 100;
	a->custom		= runTickViewMenu;
	EGE_Scene_add_rgroup(gameSc,tilemap);
	
	// draw the menu
	EGE_RGroup_set_font(grpTT, EGE_Game_find_Font(g, "Title"));
	EGE_RGroup_set_font(grpMN, EGE_Game_find_Font(g, "Menu"));
	EGE_RGroup_uniq_col_set(grp, grpc);
	EGE_RGroup_uniq_col_set(grpCUR, grpCc);
	EGE_RGroup_uniq_col_set(grpMN, grpMc);
	EGE_Cursor_set_button(cur, 0, EGE_Button_new(t0, b0, "[ Start ]", EGE_Game_Scene_set));
	EGE_Cursor_set_button(cur, 1, EGE_Button_new(t1, b1, "[ Scores ]",EGE_HighScore_Scene_set));
	EGE_Cursor_set_button(cur, 2, EGE_Button_new(t2, b2,"[ Credits ]",EGE_Credit_Scene_set));
	EGE_Cursor_set_button(cur, 3, EGE_Button_new(t3, b3, "[ Quit ]",  NULL));
	EGE_Cursor_set_Quitbutton(cur, 3);

	EGE_RGroup_Rect_add(grp,p,q);
	p[0]+=fp_from_int(100);
	p[1]+=fp_from_int(30);
	EGE_RGroup_Text_add(grpTT, GAME_NAME,p);
	EGE_Scene_add_rgroup(uiSc, grp);
	EGE_Scene_add_rgroup(uiSc, grpCUR);
	EGE_Scene_add_rgroup(uiSc, grpTT);
	EGE_Scene_add_rgroup(uiSc, grpMN);

	// clouds
	EGE_Scene_add_rgroup(gameSc, grpClo);
	EGE_RGroup_uniq_col_set(grpClo, grpcloud);
	EGE_RGroup_Texture_set(grpClo, tclo);
	EGE_RGroup_TexturePart_add(grpClo,p,clt,clb);
	p[0]+=fp_from_int(512);
	EGE_RGroup_TexturePart_add(grpClo,p,clt,clb);
	p[0]+=fp_from_int(512);
	EGE_RGroup_TexturePart_add(grpClo,p,clt,clb);
	p[0]-=fp_from_int(1024);
	p[1]+=fp_from_int(512);
	EGE_RGroup_TexturePart_add(grpClo,p,clt,clb);
	p[0]+=fp_from_int(512);
	EGE_RGroup_TexturePart_add(grpClo,p,clt,clb);
	p[0]+=fp_from_int(512);
	EGE_RGroup_TexturePart_add(grpClo,p,clt,clb);
}

void		EGE_Menu_Scene_set(void) {
	EGE_setScene(gameSc, uiSc);
	EGE_vector p, m;
	p[0] = p[1] = p[2] = m[2] = fp_0;
	m[0] = fp_from_int(20*80);
	m[1] = fp_from_int(20*80);
	EGE_Screen_Area_set(p,m);
	EGE_Music_play(EGE_Game_find_Music(g, "menu.it"));
}

void		EGE_Menu_Scene_set_ui(void) {
	EGE_setUISceneAnime(uiSc, 2);
}

void		EGE_Menu_Scene_free(void) {
	EGE_setScene(NULL,NULL);
	EGE_Scene_free(gameSc);
	EGE_Scene_free(uiSc);	
}

Eina_Bool	EGE_Menu_Event_Handle(SDL_Event *e) {
	Eina_Bool quit = EGE_Cursor_Event_Handle(cur, e);
	switch(e->type) {
	case SDL_QUIT:
		quit = EINA_TRUE;
	break;
	case SDL_KEYUP:
		switch(e->key.keysym.sym) {
		case SDLK_q:
		case SDLK_ESCAPE:
			quit = EINA_TRUE;
		break;
		default:
		break;
		}
	break;
#ifdef CAANOO
	case SDL_JOYBUTTONUP:
		if (e->jbutton.button == 6)
			quit = EINA_TRUE;
	break;
#endif
	default:
	break;
	}
	return quit;
}
