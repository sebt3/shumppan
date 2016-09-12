/**
 *
 *  end_scene.c
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
EGE_Anime*		anime;
EGE_RGroup*		grpFINAL;

Eina_Bool	EGE_End_Event_Handle(SDL_Event *e);

#define ADD_LINE(txt) \
	EGE_RGroup_Text_add(grpFINAL, txt,p); \
	p[1]+=fp_from_int(20);q[1]+=fp_from_int(20);
#define ADD_LINEQ(txt) \
	EGE_RGroup_Text_add(grpFINAL, txt,q); \
	p[1]+=fp_from_int(20);q[1]+=fp_from_int(20);
#define ADD_T(txt) \
	EGE_RGroup_Text_add(grpFINAL, txt,p); 

void		EGE_End_Scene_init(void) {
	EGE_RGroup*	grp	= EGE_RGroupSprite_new(  1, EINA_FALSE, EINA_FALSE);
	EGE_RGroup*	grpTT	= EGE_RGroupSprite_new( 20, EINA_FALSE, EINA_TRUE);
			grpFINAL	= EGE_RGroupSprite_new(950, EINA_FALSE, EINA_TRUE);
	EGE_vector p		= {fp_1*130,fp_1*80,fp_05};
	EGE_vector q		= {fp_1*680,fp_1*420,fp_05};
	EGE_quater grpc 	= {fp_from_float(0.2f), fp_from_float(0.2f),fp_from_float(0.2f),fp_from_float(0.7f)};

	grp->alpha		= EINA_TRUE;
	uiSc			= EGE_Scene_new();
	uiSc->onEvent		= EGE_End_Event_Handle;
	anime			= NULL;

	EGE_RGroup_set_font(grpTT, EGE_Game_find_Font(g, "Title"));
 	EGE_RGroup_set_font(grpFINAL, EGE_Game_find_Font(g, "TextC"));
	EGE_RGroup_uniq_col_set(grp, grpc);
	EGE_Scene_add_rgroup(uiSc, grp);
	EGE_Scene_add_rgroup(uiSc, grpTT);
	EGE_Scene_add_rgroup(uiSc, grpFINAL);
	EGE_RGroup_Rect_add(grp,p,q);

	p[0]+=fp_from_int(100);	p[1]+=fp_from_int(40);
	EGE_RGroup_Text_add(grpTT, GAME_NAME,p);

	p[0]=0;	p[1]=0;q[0]=fp_from_int(150);	q[1]=0;
	ADD_LINE("Congratulation you have finished this game.");
	ADD_LINE("");
	ADD_LINE("");
	ADD_LINEQ("Special thanks to :");
	ADD_LINE("");
	ADD_T("My wife");		ADD_LINEQ("For being patient about this");
					ADD_LINEQ("and for being that lovely one");
	ADD_LINE("");
	ADD_LINEQ("Credits :");
	ADD_LINE("");
	ADD_LINE("Background tiles:");
					ADD_LINEQ("Daniel Cook - Hard Vacuum");
					ADD_LINEQ("http://www.lostgarden.com/");
	ADD_T("Sprites:");		ADD_LINEQ("Kenney (www.kenney.nl)");
	ADD_T("Font:");			ADD_LINEQ("Kenney (www.kenney.nl)");
	ADD_T("Code:");			ADD_LINEQ("sebt3 (sebt3.openpandora.org)");
	ADD_T("Level design:");		ADD_LINEQ("sebt3 (sebt3.openpandora.org)");
	ADD_T("Sounds:");		ADD_LINEQ("sebt3 (sebt3.openpandora.org)");
	ADD_LINE("");
	ADD_LINEQ("Songs :");
	ADD_LINE("");
	ADD_T("Menu:");			ADD_LINEQ("Trance2N (A-Lin)");
					ADD_LINEQ("jamendo.com/artist/A-Lin");
	ADD_T("level 1:");		ADD_LINEQ("Plasma Weapon (Chromex)");
					ADD_LINEQ("ekergoat.chez.com/Frame_Chromex.htm");
	ADD_T("level 2:");		ADD_LINEQ("Shiva 2.0.4 (Jonemaan)");
	ADD_T("level 3:");		ADD_LINEQ("Circuit System (Biotic)");
	ADD_T("level 4:");		ADD_LINEQ("Hyperspaced (maverick)");
	ADD_T("level 5:");		ADD_LINEQ("Ethereallity (Ivory)");
	ADD_T("level 6:");		ADD_LINEQ("Cosmosis (matti kari - maza)");
	ADD_T("level 7:");		ADD_LINEQ("GoAhead (viOLAtor)");
	ADD_T("level 8:");		ADD_LINEQ("Movement (madhatter/chill)");
	ADD_T("level 9:");		ADD_LINEQ("Silent Infinity");
					ADD_LINEQ("(iNFRA of SiLiCON Records)");
	ADD_T("level 10:");		ADD_LINEQ("Jungle temple");
					ADD_LINEQ("(Ancient Amazon Artifact)");
	ADD_LINE("");
	ADD_LINE("My beta testers:");
					ADD_LINEQ("Cloudef");
					ADD_LINEQ("B-ZaR");
					ADD_LINEQ("ptitSeb");
					ADD_LINEQ("Alexandre");
					ADD_LINEQ("skeezix");
					ADD_LINEQ("EvilDragon");
					ADD_LINEQ("Wally");
					ADD_LINEQ("ekianjo");
	ADD_LINE("");
	ADD_LINE("     Well Done !");
	ADD_LINE("");
	ADD_LINE("You made it...");
	ADD_LINE("");
	ADD_LINE("");
	ADD_LINE("");
	ADD_LINE("");
	ADD_LINE("");
	ADD_LINE("As if it was that hard :D");

	q[1]=0;q[2]=0;
	EGE_RGroup_move_by(grpFINAL, q);
	grpFINAL->doScissor	= EINA_TRUE;
	grpFINAL->scissor[0]	= 150; // left
	grpFINAL->scissor[1]	= 80;  // bottom
	grpFINAL->scissor[2]	= 510; // width
	grpFINAL->scissor[3]	= 240; // height
}

void		runTickEndSceneAnim(EGE_Anime* a) {
	EGE_vector q	= {0,fp_from_float(-0.4f),0};
	EGE_RGroup_move_by(grpFINAL, q);
	if(a->tickLeft==1) {
		anime=NULL;
		EGE_Menu_Scene_set();
		EGE_HighScore_Scene_set();
	}

}

void		EGE_End_Scene_set(void) {
	EGE_vector q	= {0,0,0};
	q[1]		= fp_1*400-real2fp(grpFINAL->dots[1]);
	EGE_RGroup_move_by(grpFINAL, q);
	if(anime==NULL)
		anime	= EGE_Scene_add_anime(uiSc, NULL);
	anime->custom	= runTickEndSceneAnim;
	anime->tickLeft	= 3000;
	EGE_setUIScene(uiSc);
}

void		EGE_End_Scene_free(void) {
	EGE_Scene_free(uiSc);	
}

Eina_Bool	EGE_End_Event_Handle(SDL_Event *e) {
	Eina_Bool quit = EINA_FALSE;
	switch(e->type) {
	case SDL_QUIT:
		quit = EINA_TRUE;
	break;
	case SDL_KEYUP:
	case SDL_MOUSEBUTTONUP:
		EGE_enterScore_Scene_set();
	break;
#ifdef CAANOO
	case SDL_JOYBUTTONUP:
		if (e->jbutton.button == 6)
			EGE_enterScore_Scene_set();
	break;
#endif
	default:
	break;
	}
	return quit;
}
