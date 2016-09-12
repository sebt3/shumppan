/**
 *
 *  highscore_scene.c
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
Eina_Bool	EGE_HighScore_Event_Handle(SDL_Event *e);

typedef struct {
	char	name[80];
	int	score;
	int	level;
	int	kills;
} EGE_HScore_entry;

EGE_HScore_entry ScoreTable[10];
EGE_RGroup*	grpE;

void		EGE_HighScore_Scene_Submit(char *name, int score, int level, int kills) {
	int i,j;
	for(i=0;i<10;i++) {
		if(score>ScoreTable[i].score) {
			for(j=9;j>i;j--) {
				strcpy(ScoreTable[j].name, ScoreTable[j-1].name);
				ScoreTable[j].score = ScoreTable[j-1].score;
				ScoreTable[j].level = ScoreTable[j-1].level;
				ScoreTable[j].kills = ScoreTable[j-1].kills;
			}
			sprintf(ScoreTable[i].name, name);
			ScoreTable[i].score = score;
			ScoreTable[i].level = level;
			ScoreTable[i].kills = kills;
			return;
		}
	}
}
int		EGE_HighScore_getPos(int score) {
	int i;
	for(i=0;i<10;i++)
		if(score>ScoreTable[i].score) return i+1;
	return 0;
}

void		EGE_HighScore_Scene_init(void) {
	int i;
	FILE*		file	= NULL;
	char line[1024];
	EGE_RGroup*	grp	= EGE_RGroupSprite_new(  1, EINA_FALSE, EINA_FALSE);
	EGE_RGroup*	grpTT	= EGE_RGroupSprite_new( 20, EINA_FALSE, EINA_TRUE);
	EGE_RGroup*	grpMN	= EGE_RGroupSprite_new( 50, EINA_FALSE, EINA_TRUE);
			grpE	= EGE_RGroupSprite_new(200, EINA_FALSE, EINA_TRUE);
	EGE_vector p		= {fp_1*120,fp_1*80,fp_05};
	EGE_vector q		= {fp_1*680,fp_1*400,fp_05};
	EGE_quater grpc 	= {fp_from_float(0.2f), fp_from_float(0.2f),fp_from_float(0.2f),fp_from_float(0.7f)};
	EGE_quater grpt 	= {fp_from_float(0.6f), fp_from_float(0.6f),fp_from_float(0.6f),fp_1};

	grp->alpha		= EINA_TRUE;
	uiSc			= EGE_Scene_new();
	uiSc->onEvent		= EGE_HighScore_Event_Handle;

	EGE_RGroup_Rect_add(grp,p,q);
	EGE_Scene_add_rgroup(uiSc, grp);

	EGE_RGroup_set_font(grpTT, EGE_Game_find_Font(g, "Title"));
 	EGE_RGroup_set_font(grpMN, EGE_Game_find_Font(g, "Credit"));
 	EGE_RGroup_set_font(grpE, EGE_Game_find_Font(g, "Credit"));
	EGE_RGroup_uniq_col_set(grp, grpc);
	EGE_RGroup_uniq_col_set(grpMN, grpt);

	p[0]+=fp_from_int(100);
	p[1]+=fp_from_int(30);
	EGE_RGroup_Text_add(grpTT, GAME_NAME,p);
	p[0] =fp_from_int(140);
	p[1] =fp_from_int(160);
	EGE_RGroup_Text_add(grpMN, "Name",p);
	p[0]+=fp_from_int(200);
	EGE_RGroup_Text_add(grpMN, "kills",p);
	p[0]+=fp_from_int(100);
	EGE_RGroup_Text_add(grpMN, "Level",p);
	p[0]+=fp_from_int(100);
	EGE_RGroup_Text_add(grpMN, "Score",p);
	EGE_Scene_add_rgroup(uiSc, grpTT);
	EGE_Scene_add_rgroup(uiSc, grpMN);
	EGE_Scene_add_rgroup(uiSc, grpE);

	// reading the score table
	for(i=0;i<10;i++) {
		ScoreTable[i].name[0]	= 0;
		ScoreTable[i].score	= 0;
		ScoreTable[i].level	= 0;
		ScoreTable[i].kills	= 0;
	}
	file = fopen ("scores.txt", "r" );
	i=0;
	while (file!=NULL && fgets ( line, sizeof line, file ) != NULL ) {
		if (sscanf(line, "%s %d %d %d", ScoreTable[i].name, &ScoreTable[i].kills, &ScoreTable[i].level,&ScoreTable[i].score)>3)
			i++;
		if (i>9) break;
	}
	if (file!=NULL)fclose(file);
}

void		EGE_HighScore_Scene_set(void) {
	int i;
	char buf[12];
	EGE_vector p		= {fp_1*140,fp_1*180,fp_05};
	EGE_RGroup_empty(grpE);
	for(i=0;i<10;i++) {
		if (ScoreTable[i].name[0] == 0)
			EGE_RGroup_Text_add(grpE, "unknown",p);
		else
			EGE_RGroup_Text_add(grpE, ScoreTable[i].name,p);
		sprintf(buf,"%d", ScoreTable[i].kills);p[0]+=fp_from_int(200);
		EGE_RGroup_Text_add(grpE, buf,p);
		sprintf(buf,"%d", ScoreTable[i].level);p[0]+=fp_from_int(100);
		EGE_RGroup_Text_add(grpE, buf,p);
		sprintf(buf,"%d", ScoreTable[i].score);p[0]+=fp_from_int(100);
		EGE_RGroup_Text_add(grpE, buf,p);
		p[0] = fp_1*140;
		p[1] += fp_1*20;
	}
	EGE_setUISceneAnime(uiSc, 0);
}

void		EGE_HighScore_Scene_free(void) {
	// Saving High Score table
	//char buf[1024];
	int i;
	FILE *file = fopen ("scores.txt", "w");
	for (i=0;i<10;i++) {
		if (ScoreTable[i].name[0] == 0)
			fprintf(file, "unknown %d %d %d\n", ScoreTable[i].kills, ScoreTable[i].level, ScoreTable[i].score);
		else
			fprintf(file, "%s %d %d %d\n", ScoreTable[i].name, ScoreTable[i].kills, ScoreTable[i].level, ScoreTable[i].score);
	}
	fclose(file);
	EGE_Scene_free(uiSc);	
}

Eina_Bool	EGE_HighScore_Event_Handle(SDL_Event *e) {
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
