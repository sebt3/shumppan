/**
 *
 *  player.h
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
#ifndef __EGE_PLAYER_H__
#define __EGE_PLAYER_H__
#include "scene.h"
#include "bullets.h"
#include "ui.h"

#define SHADOW_OFFX 7
#define SHADOW_OFFY 15
#define SHADOW_SCALE  fp2real(fp_from_float(0.8))
typedef struct {
	EGE_Anime*	anime;
	EGE_Segment*	seg;
	EGE_Segment*	shadow;
	int		life;
	int		maxLife;
	int		strength;
	Eina_Bool	shooting;
	Uint32		lastShoot;
	EGE_progressBar* lb;
} EGE_player;

void		 EGE_player_init(void);
EGE_bullet_list* EGE_player_get_bl(void);

EGE_player*	 EGE_player_new(EGE_Scene* scene, EGE_RGroup* grp, EGE_RGroup* shad, EGE_bullet_list* a_bl, EGE_progressBar* lb);
void		 EGE_player_reset(EGE_player* pl, Eina_Bool full);
void		 EGE_player_setLife(EGE_player* pl, int life);
Eina_Bool	 EGE_player_Colide(EGE_player* pl, EGE_real* tl, EGE_real* br);
Eina_Bool	 EGE_player_Event_Handle(EGE_player *player, SDL_Event *e);
void		 EGE_player_joystick(void);
#endif
