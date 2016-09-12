/**
 *
 *  scenes.h
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
#ifndef __EGE_SCENES_H__
#define __EGE_SCENES_H__
#include "scene.h"

#define GAME_NAME "S.H.M.U.P.P.A.N."

extern int currentLevel;


void		EGE_Game_Scene_init(void);
void		EGE_Game_Scene_set(void);
void		EGE_Game_Scene_set_ui(void);
void		EGE_Game_Scene_set_level(int level);
void		EGE_Game_Scene_set_next(void);
void		EGE_Game_Scene_free(void);
void		EGE_Menu_Scene_init(void);
void		EGE_Menu_Scene_free(void);
void		EGE_Menu_Scene_set(void);
void		EGE_Menu_Scene_set_ui(void);
void		EGE_Credit_Scene_init(void);
void		EGE_Credit_Scene_free(void);
void		EGE_Credit_Scene_set(void);
void		EGE_Finish_Scene_set(void);
void		EGE_Dead_Scene_init(void);
void		EGE_Dead_Scene_free(void);
void		EGE_Dead_Scene_set(void);
void		EGE_Pause_Scene_init(void);
void		EGE_Pause_Scene_free(void);
void		EGE_Pause_Scene_set(void);
void		EGE_End_Scene_init(void);
void		EGE_End_Scene_free(void);
void		EGE_End_Scene_set(void);
void		EGE_Intro_Scene_init(void);
void		EGE_Intro_Scene_free(void);
void		EGE_Intro_Scene_set(int level);
void		EGE_HighScore_Scene_init(void);
void		EGE_HighScore_Scene_free(void);
void		EGE_HighScore_Scene_set(void);
void		EGE_enterScore_Scene_init(void);
void		EGE_enterScore_Scene_free(void);
void		EGE_enterScore_Scene_set(void);
#include "player.h"
#include "bonus.h"
int		EGE_HighScore_getPos(int score);
void		EGE_HighScore_Scene_Submit(char *name, int score, int level, int kills);
void		EGE_Game_Scene_startAnime(char *text);
EGE_player*	EGE_Game_Scene_get_player(void);
EGE_bonus_list*	EGE_Game_Scene_get_bonus(void);
#endif
