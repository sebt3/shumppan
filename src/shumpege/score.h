/**
 *
 *  score.h
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
#ifndef __EGE_SCORE_H__
#define __EGE_SCORE_H__
#include "scene.h"
#include "ui.h"

typedef struct {
	uint32_t	totalScore;
	uint32_t	totalKills;
	uint8_t		maxCombo;
	uint32_t	currentLevelScore;
	uint32_t	currentLevelKills;
	uint8_t		currentLevel;
	uint8_t		currentCombo;
	uint8_t		currentMaxCombo;
	uint32_t	lastKill;
	EGE_Anime*	animeCombo;
	EGE_Scene*	scene;
	EGE_RGroup*	rg;
} EGE_Score;

extern EGE_Score score;


void		EGE_Score_reset(void);
void		EGE_Score_setScene(EGE_Scene* scene, EGE_RGroup* rg);
void		EGE_Score_addKill(int strength, int maxLife);
void		EGE_Score_addCombo(void);
void		EGE_Score_nextLevel(void);

#endif
