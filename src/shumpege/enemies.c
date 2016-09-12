/**
 *
 *  enemies.c
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
#include "enemies.h"
#include "bullets.h"
#include "player.h"
#include "bonus.h"

int maxEnnemies;
EGE_bullet_list*	ebl;
EGE_vector		shadOffset;
EGE_vector 		bossPos;
EGE_bullet_list*	EGE_ennemy_get_bl(void) {return ebl;}


EGE_Enemy*		EGE_Enemy_new(EGE_Anime* a, EGE_Segment* s, EGE_Segment* shad, int life, uint16_t strength, uint16_t type) {
	EGE_Enemy*	ret	= calloc(1, sizeof(EGE_Enemy));
	ret->anime		= a;
	ret->seg		= s;
	ret->shadow		= shad;
	ret->life		= life;
	ret->maxLife		= life;
	ret->startTick		= EGE_GetTicks();
	ret->altMove		= 0;
	ret->strength		= strength;
	ret->type		= type;
	ret->lastShoot		= 0;
	return ret;
}

void			EGE_Enemy_free(EGE_Enemy* e) {
	free(e);
}

void			runTickEnemyHide(EGE_Anime* a) {
	int i, j;
	EGE_real p[3];
	EGE_Enemy* e = (EGE_Enemy*)a->obj;
	for(j=0;j<3;j++)
		p[j] = fp2real(fp_0);
	for(i=0;i<a->seg->dotsCnt;i++)
		for(j=0;j<3;j++)
			p[j] += a->seg->dots[3*i+j];
	for(j=0;j<3;j++)
		p[j] = fp2real(fp_div(real2fp(p[j]),fp_from_int(a->seg->dotsCnt)));

	if (a->tickLeft>1) {
		a->seg->dots[9]	+= fp2real((real2fp(p[0]) - real2fp(a->seg->dots[9]))/a->tickLeft);
		a->seg->dots[4]	+= fp2real((real2fp(p[1]) - real2fp(a->seg->dots[4]))/a->tickLeft);
		a->seg->dots[6]	+= fp2real((real2fp(p[0]) - real2fp(a->seg->dots[6]))/a->tickLeft);
		a->seg->dots[7]	+= fp2real((real2fp(p[1]) - real2fp(a->seg->dots[7]))/a->tickLeft);
	} else {
		EGE_Segment_sprite_hide(a->seg);
		e->parent->reuse = eina_list_append(e->parent->reuse, e);
	}
}

void			runTickBossDeath(EGE_Anime* a) {
	EGE_vector p	= {bossPos[0],bossPos[1],bossPos[2]};
	if (a->tickLeft<2) {
		EGE_Finish_Scene_set();
	} else if (a->tickLeft%6==0 && a->tickLeft>15) {
		int	c	= a->tickLeft/6;
		p[0]	-= fp_1*2*c;
		p[1]	-= fp_1*2*c;
		EGE_bullet_list_remove_bullet(ebl, EGE_bullet_list_add_bullet(ebl, p, "explo1", 0));
		p[0]	+= fp_1*5*c;
		EGE_bullet_list_remove_bullet(ebl, EGE_bullet_list_add_bullet(ebl, p, "explo1", 0));
		p[1]	+= fp_1*5*c;
		EGE_bullet_list_remove_bullet(ebl, EGE_bullet_list_add_bullet(ebl, p, "explo1", 0));
		p[0]	-= fp_1*5*c;
		EGE_bullet_list_remove_bullet(ebl, EGE_bullet_list_add_bullet(ebl, p, "explo1", 0));
	}
}

void			EGE_Enemy_startBossDeathAnime(EGE_Enemy* e) {
	EGE_Anime  *a;
	bossPos[0]	= fp_div(real2fp(e->seg->min[0]+e->seg->max[0]),fp_1*2);
	bossPos[1]	= fp_div(real2fp(e->seg->min[1]+e->seg->max[1]),fp_1*2);
	bossPos[2]	= fp_05;
	a		= EGE_Scene_add_anime(e->parent->scene, NULL);
	a->custom	= runTickBossDeath;
	a->tickLeft	= 40;
}


void			runTickEnemy(EGE_Anime* a) {
	int i;
	int tick	= EGE_GetTicks();
	a->tickLeft++;
	EGE_vector v	= {fp_0,fp_0,fp_05};
	EGE_Enemy* e	= (EGE_Enemy*)a->obj;
	EGE_fp	ang	= (tick - e->startTick)%60;
	EGE_fp	ang2	= (tick - e->startTick)%120;
	EGE_fp	ang3	= (tick - e->startTick)%180;
	EGE_fp	ang8	= (tick - e->startTick)%480;
	EGE_bullet* b;
	EGE_player* pl	= EGE_Game_Scene_get_player();
	int	freq	= 90;
	char*	name	= NULL;
	// off screen detection
	for(i=0;i<2;i++) {
		if (a->seg->max[i]<=fp2real(EGE_viewport.cur[i])) {
			EGE_Segment_sprite_hide(e->shadow);
			EGE_Segment_sprite_hide(e->seg);
			EGE_Enemies_list_remove_Enemy(e->parent, e);
			return;
		}
		if (a->seg->min[i]>fp2real(EGE_viewport.br[i])) {
			EGE_Segment_sprite_hide(e->shadow);
			EGE_Segment_sprite_hide(e->seg);
			EGE_Enemies_list_remove_Enemy(e->parent, e);
			return;
		}
	}
	// Collide detection with player bullets
	b = EGE_bullet_list_Colide(EGE_player_get_bl(),a->seg->min,a->seg->max);
	if (b!=NULL) {
		i = e->life - b->life;
		if (i<0) {
			b->life = -i;
			if (e->type == ENNEMY_BOSS) {
				EGE_Score_addKill(e->strength*120, e->maxLife*10);
				EGE_Enemy_startBossDeathAnime(e);
			} else
				EGE_Score_addKill(e->strength, e->maxLife);
			EGE_Enemies_list_remove_Enemy(e->parent, e);
			return;
		}else if (i>0){
			e->life = i;
			EGE_bullet_list_remove_bullet(b->parent, b);
		}else {
			if (e->type == ENNEMY_BOSS) {
				EGE_Score_addKill(e->strength*120, e->maxLife*10);
				EGE_Enemy_startBossDeathAnime(e);
			} else
				EGE_Score_addKill(e->strength, e->maxLife);
			EGE_Enemies_list_remove_Enemy(e->parent, e);
			EGE_bullet_list_remove_bullet(b->parent, b);
			return;
		}
	}
	// Collide detection with player
	if(EGE_player_Colide(pl, a->seg->min,a->seg->max)) {
		if (e->life<pl->life) {
			EGE_player_setLife(pl, pl->life - e->life);
			EGE_Score_addKill(e->strength, e->maxLife);
			EGE_Enemies_list_remove_Enemy(e->parent, e);
			if (e->type == ENNEMY_BOSS)
				EGE_Enemy_startBossDeathAnime(e);
			return;
		} else {
			EGE_player_setLife(pl, 0);
			EGE_Dead_Scene_set();
			return;
		}
	}

	// set shoot frequency
	switch(e->type){
	case ENNEMY_B:	  freq=64;break;
	case ENNEMY_C:	  freq=110;break;
	case ENNEMY_D:	  freq=198;break;
	case ENNEMY_TOWER:freq=28;break;
	case ENNEMY_BOSS: freq=30*(4-e->strength);break;
	default:	break;
	}
	// set next movement
	/*
	 * Movement famillies :
	 *   ? : sinuzoids
	 *  1? : Lines forward (?=speed)
	 *  2? : Lines to bottom
	 *  3? : Lines to top
	 *  4? : Keeps on screen (loops, stick to egde...)
	 *  5? : 2 steps attacks
	 *  6? : Target attacks
	 */
	switch(e->altMove){
	// sinuzoids ----------------------------------------
	case 1:
		a->move[0]	= fp_from_float(-2);
		a->move[1]	= 10*cosx(ang2*ANG_360s/2);
		break;
	case 2:	// is static
		a->move[0]	= -3*sinx(ang*ANG_360s);
		a->move[1]	= 3*cosx(ang*ANG_360s);
		break;
	case 3:
		a->move[0]	= fp_from_float(-2);
		a->move[1]	= 5*cosx(ang2*ANG_360s/2);
		break;
	case 4:
		a->move[0]	= fp_from_float(-2.6);
		a->move[1]	= 4*cosx(ang8*ANG_360s/8);
		break;
	case 5:
		a->move[0]	= fp_from_float(-2.6);
		a->move[1]	= -4*cosx(ang8*ANG_360s/8);
		break;
	case 6:
		a->move[0]	= fp_from_float(-2);
		a->move[1]	= 3*cosx(ang2*ANG_360s/2);
		break;
	case 7: // is static
		a->move[0]	= 0;
		a->move[1]	= 0;
		break;
	// Lines forward ------------------------------------
	case 10:
		a->move[0]	= -fp_1;
		a->move[1]	= 0;
		break;
	case 11:
		a->move[0]	= -fp_1*2;
		a->move[1]	= 0;
		break;
	case 12:
		a->move[0]	= -fp_1*4;
		a->move[1]	= 0;
		break;
	case 13:
		a->move[0]	= -fp_1*6;
		a->move[1]	= 0;
		break;
	case 14:
		a->move[0]	= -fp_div(fp_1,fp_1*2);
		a->move[1]	= 0;
		break;
	case 15:
		a->move[0]	= -fp_div(fp_1,fp_1*3);
		a->move[1]	= 0;
		break;
	case 16:
		a->move[0]	= -fp_div(fp_1,fp_1*4);
		a->move[1]	= 0;
		break;
	// Lines to bottom ----------------------------------
	case 20:
		a->move[0]	= -fp_1;
		a->move[1]	= fp_1;
		break;
	case 21:
		a->move[0]	= -fp_1*2;
		a->move[1]	= fp_1;
		break;
	case 22:
		a->move[0]	= -fp_1*4;
		a->move[1]	= fp_1;
		break;
	case 23:
		a->move[0]	= -fp_1*6;
		a->move[1]	= fp_1;
		break;
	case 24:
		a->move[0]	= -fp_div(fp_1,fp_1*2);
		a->move[1]	= fp_1;
		break;
	case 25:
		a->move[0]	= -fp_div(fp_1,fp_1*3);
		a->move[1]	= fp_1;
		break;
	case 26:
		a->move[0]	= -fp_div(fp_1,fp_1*4);
		a->move[1]	= fp_1;
		break;
	case 27:
		a->move[0]	= -fp_div(fp_1,fp_1*2);
		a->move[1]	= fp_1*2;
		break;
	case 28:
		a->move[0]	= -fp_1;
		a->move[1]	= fp_1*3;
		break;
	case 29:
		a->move[0]	= -fp_div(fp_1,fp_1*2);
		a->move[1]	= fp_1*3;
		break;
	// Lines to top -------------------------------------
	case 30:
		a->move[0]	= -fp_1;
		a->move[1]	= -fp_1;
		break;
	case 31:
		a->move[0]	= -fp_1*2;
		a->move[1]	= -fp_1;
		break;
	case 32:
		a->move[0]	= -fp_1*4;
		a->move[1]	= -fp_1;
		break;
	case 33:
		a->move[0]	= -fp_1*6;
		a->move[1]	= -fp_1;
		break;
	case 34:
		a->move[0]	= -fp_div(fp_1,fp_1*2);
		a->move[1]	= -fp_1;
		break;
	case 35:
		a->move[0]	= -fp_div(fp_1,fp_1*3);
		a->move[1]	= -fp_1;
		break;
	case 36:
		a->move[0]	= -fp_div(fp_1,fp_1*4);
		a->move[1]	= -fp_1;
		break;
	case 37:
		a->move[0]	= -fp_div(fp_1,fp_1*2);
		a->move[1]	= -fp_1*2;
		break;
	case 38:
		a->move[0]	= -fp_1;
		a->move[1]	= -fp_1*3;
		break;
	case 39:
		a->move[0]	= -fp_div(fp_1,fp_1*2);
		a->move[1]	= -fp_1*3;
		break;
	// Keeps on screen ----------------------------------
	case 40: // small loops
		a->move[0]	= -2*sinx(ang*ANG_360s)+fp_05*10/5;
		a->move[1]	= 2*cosx(ang*ANG_360s);
		break;
	case 41: // large loops
		a->move[0]	= -4*sinx(ang*ANG_360s)+fp_05*10/5;
		a->move[1]	= 4*cosx(ang*ANG_360s);
		break;
	case 42:
		if (tick - e->startTick>15)
			a->move[0]	= fp_05*10/5;
		if (a->move[1]== 0)
			a->move[1]	= -fp_1;
		if (a->seg->min[1]<=fp2real(EGE_viewport.cur[1])) {
			a->move[1]	= fp_1;
		}
		if (a->seg->max[1]>fp2real(EGE_viewport.br[1])) {
			a->move[1]	= -fp_1;
		}
		break;
	case 43:
		if (tick - e->startTick>15)
			a->move[0]	= fp_05*10/5;
		if (a->move[1]== 0)
			a->move[1]	= -fp_1*2;
		if (a->seg->min[1]<=fp2real(EGE_viewport.cur[1]))
			a->move[1]	= fp_1*2;
		else if (a->seg->max[1]>fp2real(EGE_viewport.br[1]))
			a->move[1]	= -fp_1*2;
		break;
	case 44:
		if (tick - e->startTick>15)
			a->move[0]	= fp_05*10/5;
		if (a->move[1]== 0)
			a->move[1]	= -fp_1*3;
		if (a->seg->min[1]<=fp2real(EGE_viewport.cur[1]))
			a->move[1]	= fp_1*3;
		else if (a->seg->max[1]>fp2real(EGE_viewport.br[1]))
			a->move[1]	= -fp_1*3;
		break;
	case 45:
		if (tick - e->startTick>15)
			a->move[0]	= fp_05*10/5;
		if (a->move[1]== 0)
			a->move[1]	= -fp_1*4;
		if (a->seg->min[1]<=fp2real(EGE_viewport.cur[1]))
			a->move[1]	= fp_1*4;
		else if (a->seg->max[1]>fp2real(EGE_viewport.br[1]))
			a->move[1]	= -fp_1*4;
		break;
	case 46:
		if (tick - e->startTick>15)
			a->move[0]	= fp_05*10/5;
		break;
	// 2 steps attacks ----------------------------------
	case 50: // target in front after a few
		if (tick - e->startTick>15 && tick - e->startTick<180)
			a->move[0]	= fp_05*10/5;
		else if (tick - e->startTick>180)
			a->move[0]	= -fp_1*3;
		break;
	case 51: // target center after a few
		if (tick - e->startTick>15 && tick - e->startTick<180)
			a->move[0]	= fp_05*10/5;
		else if (tick - e->startTick>180) {
			a->move[0]	= -fp_1*3;
			a->move[1]	= -fp_div(real2fp(a->seg->min[1])-EGE_viewport.cur[1]-220*fp_1, 70*fp_1);
		}
		break;
	case 52: // target top after a few
		if (tick - e->startTick>15 && tick - e->startTick<180)
			a->move[0]	= fp_05*10/5;
		else if (tick - e->startTick>180) {
			a->move[0]	= -fp_1*3;
			a->move[1]	= -fp_div(real2fp(a->seg->min[1])-EGE_viewport.cur[1], 70*fp_1);
		}
		break;
	case 53: // target bottom after a few
		if (tick - e->startTick>15 && tick - e->startTick<180)
			a->move[0]	= fp_05*10/5;
		else if (tick - e->startTick>180) {
			a->move[0]	= -fp_1*3;
			a->move[1]	= -fp_div(real2fp(a->seg->min[1])-EGE_viewport.cur[1]-440*fp_1, 70*fp_1);
		}
		break;
	// Target attacks -----------------------------------
	case 60: // target in front
		a->move[0]	= -fp_1*3;
		break;
	case 61: // target center
		a->move[0]	= -fp_1*3;
		a->move[1]	= -fp_div(real2fp(a->seg->min[1])-EGE_viewport.cur[1]-220*fp_1, 70*fp_1);
		break;
	case 62: // target top
		a->move[0]	= -fp_1*3;
		a->move[1]	= -fp_div(real2fp(a->seg->min[1])-EGE_viewport.cur[1], 70*fp_1);
		break;
	case 63: // target bottom
		a->move[0]	= -fp_1*3;
		a->move[1]	= -fp_div(real2fp(a->seg->min[1])-EGE_viewport.cur[1]-440*fp_1, 70*fp_1);
		break;
	// Default ------------------------------------------
	case ENNEMY_MV_DEFAULT:
	default:
		switch(e->type){
		case ENNEMY_BOSS:
			a->move[1]		= fp_div(fp_from_int(fp_to_int(EGE_viewport.br[1])+fp_to_int(EGE_viewport.cur[1]) - fp_to_int(real2fp(a->seg->max[1]))-fp_to_int(real2fp(a->seg->min[1]))), 30*fp_1);
			if (tick - e->startTick>e->strength*20)
				a->move[0]	= (EGE_viewport.br[0]-fp_1*10-real2fp(a->seg->max[0]))/15;
			break;
		case ENNEMY_B:	a->move[0]	= fp_from_float(-1);freq=64;
				a->move[1]	= 9*cosx(ang3*ANG_720s/3);
				break;
		case ENNEMY_TOWER:
		case ENNEMY_C:	a->move[0]	= fp_from_float(-0.5);freq=110;
				break;
		case 6:
		case ENNEMY_D:	a->move[0]	= -3*sinx(ang*ANG_720s);freq=198;
				a->move[1]	= 3*cosx(ang*ANG_720s);
				break;
		case ENNEMY_E:	a->move[0]	= fp_from_float(-2);break;
		case ENNEMY_A:	a->move[0]	= fp_from_float(-2);
				a->move[1]	= 8*cosx(ang2*ANG_360s/2);
		default:	break;
		}
		break;
	}

	// move the enemy
	if (a->move[0] != fp_0 || a->move[1] != fp_0 || a->move[2] != fp_0) {
		EGE_Segment_move_by(e->seg, a->move);
		EGE_Segment_move_by(e->shadow, a->move);
	}
	if (e->type == ENNEMY_ASTERB || e->type == ENNEMY_ASTERM) {
		EGE_Segment_rotate_sprite(e->seg, a->rot[2]);
		EGE_Segment_rotate_sprite(e->shadow, a->rot[2]);
		return; // no shot for asteroids
	}

	// shooting
	v[0] = real2fp(e->seg->min[0]);
	v[1] = real2fp((e->seg->min[1]+e->seg->max[1])/2) - fp_1*8;
	if (e->type==ENNEMY_BOSS) {
		if (tick - e->lastShoot == freq/3 || tick - e->lastShoot == 2*freq/3) {
			name		=	"shootR02";
			if (e->strength>1)
				name	=	"shootV02";
			b = EGE_bullet_list_add_bullet(ebl, v, name, e->strength);
			EGE_Segment_rotate_sprite(b->seg, -ANG_90/6);
			b->anime->move[0]	= -18*sinx(ANG_90/6);
			b->anime->move[1]	= 6*sinx(ANG_90/6);
			b = EGE_bullet_list_add_bullet(ebl, v, name, e->strength);
			EGE_Segment_rotate_sprite(b->seg, -ANG_90/4);
			b->anime->move[0]	= -12*sinx(ANG_90/4);
			b->anime->move[1]	= 6*sinx(ANG_90/4);
			b = EGE_bullet_list_add_bullet(ebl, v, name, e->strength);
			EGE_Segment_rotate_sprite(b->seg, -ANG_90/2);
			b->anime->move[0]	= -6*sinx(ANG_90/2);
			b->anime->move[1]	= 6*sinx(ANG_90/2);
			b = EGE_bullet_list_add_bullet(ebl, v, name, e->strength);
			EGE_Segment_rotate_sprite(b->seg, ANG_90/6);
			b->anime->move[0]	= -18*sinx(ANG_90/6);
			b->anime->move[1]	= -6*sinx(ANG_90/6);
			b = EGE_bullet_list_add_bullet(ebl, v, name, e->strength);
			EGE_Segment_rotate_sprite(b->seg, ANG_90/4);
			b->anime->move[0]	= -12*sinx(ANG_90/4);
			b->anime->move[1]	= -6*sinx(ANG_90/4);
			b = EGE_bullet_list_add_bullet(ebl, v, name, e->strength);
			EGE_Segment_rotate_sprite(b->seg, ANG_90/2);
			b->anime->move[0]	= -6*sinx(ANG_90/2);
			b->anime->move[1]	= -6*sinx(ANG_90/2);
		} else if (tick - e->lastShoot == freq/2 || tick - e->lastShoot == freq/4 || tick - e->lastShoot == 3*freq/4) {
			name		=	"shootV04";
			if (e->strength>1)
				name	=	"shootB04";
			b = EGE_bullet_list_add_bullet(ebl, v, name, e->strength*2);
			EGE_Segment_rotate_sprite(b->seg, -ANG_90/4);
			b->anime->move[0]	= -24*sinx(ANG_90/4);
			b->anime->move[1]	= 12*sinx(ANG_90/4);
			b = EGE_bullet_list_add_bullet(ebl, v, name, e->strength*2);
			EGE_Segment_rotate_sprite(b->seg, ANG_90/4);
			b->anime->move[0]	= -24*sinx(ANG_90/4);
			b->anime->move[1]	= -12*sinx(ANG_90/4);
			b = EGE_bullet_list_add_bullet(ebl, v, name, e->strength*2);
			b->anime->move[0]	= -24*sinx(ANG_90/4);
			b->anime->move[1]	= 0;
		} else if (tick - e->lastShoot>freq) {
			e->lastShoot = EGE_GetTicks();
			name		=	"shootB11";
			if (e->strength>1)
				name	=	"shootR11";
			b = EGE_bullet_list_add_bullet(ebl, v, name, e->strength*3);
			v[1] -= fp_1*10;
			b = EGE_bullet_list_add_bullet(ebl, v, name, e->strength*3);
		}
	} else if (tick - e->lastShoot>freq) {
		e->lastShoot = EGE_GetTicks();
		// add a shoot
		switch(e->type){
		case ENNEMY_B:	name = "shootR02";break;
		case ENNEMY_C:	name = "shootV02";break;
		case ENNEMY_D:	name = "shootR11";break;
		case ENNEMY_E:	name = "shootV11";break;
		case ENNEMY_TOWER:
				name = "shootR04";break;
		default:	name = "shootR05";break;
		}
		b = EGE_bullet_list_add_bullet(ebl, v, name, e->strength);
		switch(e->type){
		case ENNEMY_TOWER:
			EGE_Segment_rotate_sprite(b->seg, -ANG_90/4);
			b->anime->move[0]	= -12*sinx(ANG_90/4);
			b->anime->move[1]	= 6*sinx(ANG_90/4);
			b = EGE_bullet_list_add_bullet(ebl, v, name, e->strength);
			EGE_Segment_rotate_sprite(b->seg, ANG_90/4);
			b->anime->move[0]	= -12*sinx(ANG_90/4);
			b->anime->move[1]	= -6*sinx(ANG_90/4);
		break;
		case ENNEMY_B:
			v[1]	+= fp_1*8;
			b->anime->move[0] = -fp_from_int(6);
			b = EGE_bullet_list_add_bullet(ebl, v, name, e->strength);
			b->anime->move[0] = -fp_from_int(6);
		break;
		case ENNEMY_C:
			b = EGE_bullet_list_add_bullet(ebl, v, name, e->strength);
			EGE_Segment_rotate_sprite(b->seg, -ANG_90/2);
			b->anime->move[0]	= -6*sinx(ANG_90/2);
			b->anime->move[1]	= 6*sinx(ANG_90/2);
			b = EGE_bullet_list_add_bullet(ebl, v, name, e->strength);
			EGE_Segment_rotate_sprite(b->seg, ANG_90/2);
			b->anime->move[0]	= -6*sinx(ANG_90/2);
			b->anime->move[1]	= -6*sinx(ANG_90/2);
		break;
		case ENNEMY_D:
			b->anime->move[0] = -fp_from_int(7);
			v[1]	-= fp_1*10;
			b = EGE_bullet_list_add_bullet(ebl, v, name, e->strength);
			b->anime->move[0] = -fp_from_int(7);
			v[1]	-= fp_1*10;
			b = EGE_bullet_list_add_bullet(ebl, v, name, e->strength);
			b->anime->move[0] = -fp_from_int(7);
		break;
		case ENNEMY_E:
		break;
		default:break;
		}
	}
}

Eina_Bool		EGE_Enemy_Colide(EGE_Enemy* pl, EGE_real* tl, EGE_real* br) {
	EGE_real x=(br[0]+tl[0])/2,y=(br[1]+tl[1])/2;
	if (pl->seg->min[0]<x && x<pl->seg->max[0] && pl->seg->min[1]<y && y<pl->seg->max[1]) return EINA_TRUE;
	return EINA_FALSE;
}

EGE_Enemy*		EGE_Enemies_list_Colide(EGE_Enemies_list* el, EGE_real* tl, EGE_real* br) {
	Eina_List*	l;
	EGE_Enemy*	e;

	EINA_LIST_FOREACH(el->enemies, l, e) {
		if (EGE_Enemy_Colide(e,tl,br))
			return e;
	}
	return NULL;
}


EGE_Enemies_list*	EGE_Enemies_list_new(EGE_RGroup* rg, EGE_RGroup* rgshad, EGE_Scene* sc, EGE_bullet_list* a_bl) {
	EGE_Enemies_list* ret	= calloc(1, sizeof(EGE_Enemies_list));
	ret->enemies		= NULL;
	ret->reuse		= NULL;
	ret->scene		= sc;
	ret->rgroup		= rg;
	ret->rgroupShadow	= rgshad;
	ebl = a_bl;
	shadOffset[0]		= fp_1*SHADOW_OFFX;
	shadOffset[1]		= fp_1*SHADOW_OFFY;
	shadOffset[2]		= 0;
	maxEnnemies		= 0;
	return ret;
}

void			EGE_Enemies_list_free(EGE_Enemies_list* bl) {
	EGE_Enemy* b;
	EINA_LIST_FREE(bl->enemies, b) {
		EGE_Enemy_free(b);
	}
	EINA_LIST_FREE(bl->reuse, b) {
		EGE_Enemy_free(b);
	}
	free(bl);
}

void			EGE_Enemies_list_remove_Enemy_na(EGE_Enemies_list* bl, EGE_Enemy* b) {
	EGE_Enemy *t, *f=NULL;
	Eina_List *l;

	EINA_LIST_FOREACH(bl->enemies, l, t) {
		if (t==b) {
			f=t;
			break;
		}
	}
	if (f!=NULL) {
		bl->enemies	= eina_list_remove(bl->enemies, f);
		f->anime->tickLeft = 0;
		f->anime	= NULL;
		EGE_Segment_sprite_hide(f->seg);
		EGE_Segment_sprite_hide(f->shadow);
	}
}

void			EGE_Enemies_list_reset(EGE_Enemies_list* bl) {
	EGE_Enemy	*t;
	Eina_List	*l;
	Eina_List	*l2;

	maxEnnemies	= 0;
	EINA_LIST_FOREACH_SAFE(bl->enemies, l, l2, t) {
		EGE_Enemies_list_remove_Enemy_na(bl,t);
	}
}

void			EGE_Enemies_list_remove_Enemy(EGE_Enemies_list* bl, EGE_Enemy* b) {
	EGE_Enemy *t, *f=NULL;
	Eina_List *l;
	EGE_Anime*	a;
	EGE_vector	p = {real2fp(b->seg->min[0]),real2fp(b->seg->min[1]),real2fp(b->seg->min[2])};

	EINA_LIST_FOREACH(bl->enemies, l, t) {
		if (t==b) {
			f=t;
			break;
		}
	}
	if (f!=NULL) {
		EGE_Segment_sprite_hide(f->shadow);
		bl->enemies	= eina_list_remove(bl->enemies, f);
		if (eina_list_count(bl->enemies)==0) {
			if (maxEnnemies>3+currentLevel)
				EGE_bonus_list_add_bonus(EGE_Game_Scene_get_bonus(), p);
			maxEnnemies	= 0;
		}
		f->anime->tickLeft = 0;
		f->anime	= NULL;
		// add animation for ennemy kill
		a		= EGE_Scene_add_anime(bl->scene, f->seg);
		a->custom	= runTickEnemyHide;
		a->tickLeft	= 6;
		a->obj		= (void*)f;
	}
}

EGE_Enemy*		EGE_Enemies_list_add_Enemy(EGE_Enemies_list* bl, EGE_vector p, uint16_t type, int str, int altmove) {
	EGE_Enemy* 	ret	= NULL;
	EGE_Segment*	s	= NULL;
	EGE_Segment*	shad	= NULL;
	char		name[128];
	int		n	= 1;
	int 		life	= 0;
	uint16_t	strength= 0;
	EGE_vector	a	= {0,0,ANG_90};
	Eina_List*	l	= eina_list_last(bl->reuse);
	if (type<=ENNEMY_E) {
		switch(type){
		case ENNEMY_B:	n	= 2;life=20+(str-1)*5;strength=2+(str-1);break;
		case ENNEMY_C:	n	= 3;life=30+(str-1)*5;strength=4+(str-1);break;
		case ENNEMY_D:	n	= 4;life=40+(str-1)*5;strength=5+(str-1);break;
		case ENNEMY_E:	n	= 5;life=25+(str-1)*5;strength=3+(str-1);break;
		default:	n	= 1;life=10+(str-1)*5;strength=1+(str-1);break;
		}
		switch(str) {
		case ENNEMY_STR_2:	sprintf(name, "enemyB%d", n);break;
		case ENNEMY_STR_3:	sprintf(name, "enemyE%d", n);break;
		case ENNEMY_STR_4:	sprintf(name, "enemyD%d", n);break;
		default:		sprintf(name, "enemyC%d", n);break;
		}
	} else if (type==ENNEMY_ASTERM) {
		sprintf(name, "asterM%d", str); life=20+(str-1)*4;
	} else if (type==ENNEMY_ASTERB) {
		sprintf(name, "asterB%d", str); life=10+(str-1)*3;
	} else if (type==ENNEMY_TOWER) {
		switch (str) {
			case 2:  sprintf(name, "towerV"); break;
			case 3:  sprintf(name, "towerB"); break;
			case 4:  sprintf(name, "towerJ"); break;
			default: sprintf(name, "towerR"); break;
		}
		strength = str;
		life=50+(str-1)*10;
	} else if (type==ENNEMY_BOSS) {
		sprintf(name, "boss%d", str); life=40+(str-1)*10;
		strength = str;
		life=200+str*200;
	}
	if (l==NULL) {
		s		= EGE_RGroup_Sprite_add(bl->rgroup,p, name);
		shad		= EGE_RGroup_Sprite_add(bl->rgroupShadow, p, name);
		EGE_Segment_move_by(shad,shadOffset);
		ret		= EGE_Enemy_new(NULL, s, shad, life, strength, type);
	} else {
		ret		= (EGE_Enemy*)l->data;
		bl->reuse	= eina_list_remove(bl->reuse, ret);
	}

	// set the animator
	ret->parent		= bl;
	ret->life		= life;
	ret->maxLife		= life;
	ret->altMove		= altmove;
	ret->strength		= strength;
	ret->startTick		= EGE_GetTicks();
	ret->type		= type;
	ret->anime		= EGE_Scene_add_anime(bl->scene,ret->seg);
	ret->anime->move[0]	= fp_from_int(-1);
	ret->anime->tickLeft	= 10;
	ret->anime->custom	= runTickEnemy;
	ret->anime->obj		= (void*)ret;
	// add positionning and set sprite
	EGE_Segment_set_sprite(ret->seg, p, name);
	EGE_Segment_set_sprite(ret->shadow, p, name);
	EGE_Segment_move_by(ret->shadow, shadOffset);
	EGE_Segment_scale_by(ret->shadow, SHADOW_SCALE);
	// rotating in the right direction
	EGE_Segment_rotate_by(ret->seg, a);
	EGE_Segment_rotate_by(ret->shadow, a);
	if (ret->type == ENNEMY_ASTERM || ret->type == ENNEMY_ASTERB) {
		ret->anime->rot[2] = ANG_90s*(10-str);
	}
	bl->enemies = eina_list_append(bl->enemies, ret);
	maxEnnemies = (eina_list_count(bl->enemies)>maxEnnemies)?eina_list_count(bl->enemies):maxEnnemies;
	return ret;
}
