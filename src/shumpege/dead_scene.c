/**
 *
 *  dead_scene.c
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
#include "score.h"
#include "scenes.h"
#include "player.h"

static EGE_Scene*	uiSc;
EGE_Anime*	anime;
Eina_Bool	EGE_Dead_Event_Handle(SDL_Event *e);
EGE_RGroup*	grpCD;
EGE_RGroup*	grpMN;
int life_left;
Eina_Bool	isFinished;

void		EGE_Dead_Scene_init(void) {
	EGE_RGroup*	grp	= EGE_RGroupSprite_new(  1, EINA_FALSE, EINA_FALSE);
	EGE_RGroup*	grpTT	= EGE_RGroupSprite_new( 20, EINA_FALSE, EINA_TRUE);
	EGE_RGroup*	grpTC	= EGE_RGroupSprite_new(200, EINA_FALSE, EINA_TRUE);
			grpMN	= EGE_RGroupSprite_new( 20, EINA_FALSE, EINA_TRUE);
			grpCD	= EGE_RGroupSprite_new(200, EINA_FALSE, EINA_TRUE);
	EGE_vector p		= {fp_1*120,fp_1*80,fp_05};
	EGE_vector q		= {fp_1*680,fp_1*400,fp_05};
	EGE_quater grpc 	= {fp_from_float(0.2f), fp_from_float(0.2f),fp_from_float(0.2f),fp_from_float(0.7f)};
	EGE_quater grpTc 	= {fp_from_float(0.6f), fp_from_float(0.6f),fp_from_float(0.6f),fp_1};
	EGE_quater grpMc 	= {fp_from_float(0.7f), fp_from_float(0.7f),fp_from_float(0.7f),fp_1};
	grp->alpha		= EINA_TRUE;
	uiSc			= EGE_Scene_new();
	uiSc->onEvent		= EGE_Dead_Event_Handle;
	anime			= NULL;

	EGE_RGroup_Rect_add(grp,p,q);
	EGE_Scene_add_rgroup(uiSc, grp);

	EGE_RGroup_set_font(grpTT, EGE_Game_find_Font(g, "Title"));
	EGE_RGroup_set_font(grpMN, EGE_Game_find_Font(g, "subTit"));
	EGE_RGroup_set_font(grpCD, EGE_Game_find_Font(g, "Finish"));
	EGE_RGroup_set_font(grpTC, EGE_Game_find_Font(g, "Finish"));
	EGE_RGroup_uniq_col_set(grp, grpc);
	EGE_RGroup_uniq_col_set(grpTC, grpTc);
	EGE_RGroup_uniq_col_set(grpMN, grpMc);
	p[0]+=fp_from_int(100);
	p[1]+=fp_from_int(30);
	EGE_RGroup_Text_add(grpTT, GAME_NAME,p);
	p[0]=fp_from_int(480);
	p[1]+=fp_from_int(80);
	EGE_RGroup_Text_add(grpTC, "Total",p);
	p[0]=fp_from_int(190);
	p[1]+=fp_from_int(40);
	EGE_RGroup_Text_add(grpTC, "Life Left:",p);
	p[1]+=fp_from_int(35);
	EGE_RGroup_Text_add(grpTC, "Kills:",p);
	p[1]+=fp_from_int(35);
	EGE_RGroup_Text_add(grpTC, "Score:",p);
	p[1]+=fp_from_int(35);
	EGE_RGroup_Text_add(grpTC, "Max combo:",p);

	EGE_Scene_add_rgroup(uiSc, grpTT);
	EGE_Scene_add_rgroup(uiSc, grpMN);
	EGE_Scene_add_rgroup(uiSc, grpTC);
	EGE_Scene_add_rgroup(uiSc, grpCD);
	isFinished = EINA_FALSE;
}

void		EGE_Dead_Scene_setScore(void) {
	char	buf[256];
	EGE_vector p		= {fp_1*400,fp_1*225,fp_05};
	EGE_RGroup_empty(grpCD);
	sprintf(buf, "%d", EGE_Game_Scene_get_player()->life);
	EGE_RGroup_Text_add(grpCD, buf, p);
	p[0]+=fp_from_int(120);
	sprintf(buf, "%d", life_left);
	EGE_RGroup_Text_add(grpCD, buf, p);
	p[1]+=fp_from_int(35);p[0]-=fp_from_int(120);
	sprintf(buf, "%d", score.currentLevelKills);
	EGE_RGroup_Text_add(grpCD, buf, p);
	p[0]+=fp_from_int(120);
	sprintf(buf, "%d", score.totalKills);
	EGE_RGroup_Text_add(grpCD, buf, p);
	p[1]+=fp_from_int(35);p[0]-=fp_from_int(120);
	sprintf(buf, "%d", score.currentLevelScore);
	EGE_RGroup_Text_add(grpCD, buf, p);
	p[0]+=fp_from_int(120);
	sprintf(buf, "%d", score.totalScore);
	EGE_RGroup_Text_add(grpCD, buf, p);
	p[1]+=fp_from_int(35);p[0]-=fp_from_int(120);
	sprintf(buf, "%d", score.currentMaxCombo);
	EGE_RGroup_Text_add(grpCD, buf, p);
	p[0]+=fp_from_int(120);
	sprintf(buf, "%d", score.maxCombo);
	EGE_RGroup_Text_add(grpCD, buf, p);
}

void		runTickDeadSceneAnim(EGE_Anime* a) {
	switch ((int)a->tickLeft/30) {
	case 2:
		if(a->tickLeft==60) break;
		score.currentLevelScore += 5*EGE_Game_Scene_get_player()->life/(a->tickLeft-60);
		EGE_Game_Scene_get_player()->life -= EGE_Game_Scene_get_player()->life/(a->tickLeft-60);
	break;
	case 1:
		if(a->tickLeft==30) break;
		score.currentLevelScore += 5*score.currentLevelKills/(a->tickLeft-30);
		score.totalKills	+= score.currentLevelKills/(a->tickLeft-30);
		score.currentLevelKills -= score.currentLevelKills/(a->tickLeft-30);
	break;
	case 0:
		if (score.currentMaxCombo>score.maxCombo)
			score.maxCombo = score.currentMaxCombo;
		if(a->tickLeft==1) {
			anime=NULL;
			if (score.currentMaxCombo>score.maxCombo)
				score.maxCombo = score.currentMaxCombo;
		}
		score.totalScore	+= score.currentLevelScore/(a->tickLeft);
		score.currentLevelScore -= score.currentLevelScore/(a->tickLeft);
	break;
	default:break;
	}
	EGE_Dead_Scene_setScore();
}

void		EGE_Dead_Scene_startAnim(void) {
	if(anime==NULL)
		anime	= EGE_Scene_add_anime(uiSc, NULL);
	anime->custom	= runTickDeadSceneAnim;
	anime->tickLeft	= 240;
}

void		EGE_Finish_Scene_set(void) {
	EGE_vector p		= {fp_1*350,fp_1*150,fp_05};
	EGE_RGroup_empty(grpMN);
	EGE_RGroup_Text_add(grpMN, "Finished!",p);
	life_left  = EGE_Game_Scene_get_player()->life;
	isFinished = EINA_TRUE;
	EGE_Dead_Scene_setScore();
	EGE_Dead_Scene_startAnim();
	EGE_setUIScene(uiSc);
}


void		EGE_Dead_Scene_set(void) {
	EGE_vector p		= {fp_1*250,fp_1*150,fp_05};
	EGE_RGroup_empty(grpMN);
	EGE_RGroup_Text_add(grpMN, "You're dead, loser!",p);
	life_left = 0;
	isFinished = EINA_FALSE;
	EGE_Dead_Scene_setScore();
	EGE_Dead_Scene_startAnim();
	EGE_setUISceneAnime(uiSc,1);
}

void		EGE_Dead_Scene_free(void) {
	EGE_Scene_free(uiSc);	
}

void		EGE_Dead_Scene_next(void) {
	if(anime!=NULL && anime->tickLeft>1) {
		if (anime->tickLeft>62) anime->tickLeft = 62;
		else if (anime->tickLeft>32) anime->tickLeft = 32;
		else if (anime->tickLeft>2) anime->tickLeft = 2;
	} else if (isFinished)
		EGE_Game_Scene_set_next();
	else
		EGE_enterScore_Scene_set();
}

Eina_Bool	EGE_Dead_Event_Handle(SDL_Event *e) {
	Eina_Bool quit = EINA_FALSE;
	switch(e->type) {
	case SDL_QUIT:
		quit = EINA_TRUE;
	break;
	case SDL_KEYUP:
		switch(e->key.keysym.sym) {
#ifdef PANDORA
		case SDLK_HOME:
		case SDLK_END:
		case SDLK_PAGEDOWN:
		case SDLK_PAGEUP:
#endif
		case SDLK_q:
		case SDLK_ESCAPE:
		case SDLK_RETURN:
			EGE_Dead_Scene_next();
		break;
		default:
		break;
		}
	break;
	case SDL_MOUSEBUTTONUP:
		EGE_Dead_Scene_next();
	break;
#ifdef CAANOO
	case SDL_JOYBUTTONUP:
		if (e->jbutton.button == 6) {
			EGE_Dead_Scene_next();
		}
	break;
#endif
	default:
	break;
	}
	return quit;
}
