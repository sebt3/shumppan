/**
 *
 *  score.c
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
#include "score.h"
#include "game.h"
#include "scenes.h"

EGE_Score score;


void		EGE_Score_show(void) {
	char		buf[256];
	EGE_vector	p	= {fp_from_int(125),  fp_from_int(5),  fp_05};
	EGE_RGroup_empty(score.rg);
	sprintf(buf, "score: % 5d", score.currentLevelScore);
	EGE_RGroup_Text_add(score.rg, buf, p);
}

void		EGE_Score_reset(void) {
	score.totalScore	= 0;
	score.totalKills	= 0;
	score.maxCombo		= 0;
	score.currentLevelScore	= 0;
	score.currentLevelKills	= 0;
	score.lastKill		= 0;
	score.currentLevel	= 1;
	score.currentCombo	= 1;
	score.currentMaxCombo	= 1;
	if (score.animeCombo != NULL) 
		score.animeCombo->tickLeft = 1;
	if (score.rg != NULL) 
		EGE_Score_show();
}

void		EGE_Score_setScene(EGE_Scene* scene, EGE_RGroup* rg) {
	score.scene		= scene;
	score.rg		= rg;
	EGE_Score_show();
}

void		EGE_Score_addKill(int strength, int maxLife) {
		EGE_Sound_play(EGE_Game_find_Sound(g, "explode.wav"));
	if (EGE_GetTicks()-score.lastKill<60)
		EGE_Score_addCombo();
	score.lastKill		 = EGE_GetTicks();
	score.currentLevelScore	+= score.currentCombo * (strength * 5 + maxLife);
	score.currentLevelKills++;
	EGE_Score_show();
}

void		runTickScoreCombo(EGE_Anime *a) {
	if (score.animeCombo->tickLeft == 1) {
		score.currentCombo	= 1;
		score.animeCombo	= NULL;
	}
}

void		EGE_Score_addCombo(void) {
	score.currentCombo++;
	if (score.currentCombo>score.currentMaxCombo)
		score.currentMaxCombo	= score.currentCombo;
	if (score.animeCombo == NULL || score.currentCombo==2) {
		score.animeCombo	= EGE_Scene_add_anime(score.scene, NULL);
	}
	score.animeCombo->tickLeft	= 120;
	score.animeCombo->custom	= runTickScoreCombo;
}

void		EGE_Score_nextLevel(void) {
	score.lastKill			= 0;
	score.currentLevel++;
	score.totalScore		+= score.currentLevelScore;
	score.currentLevelScore		 = 0;
	score.totalKills		+= score.currentLevelKills;
	score.currentLevelKills		 = 0;
	if (score.currentMaxCombo>score.maxCombo)
		score.maxCombo		 = score.currentMaxCombo;
	score.currentMaxCombo		 = 1;
	if (score.animeCombo != NULL)
		score.animeCombo->tickLeft = 1;
	
}
