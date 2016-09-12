/**
 *
 *  player.c
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
#include "enemies.h"
#include "bullets.h"
#include "bonus.h"

#ifdef PANDORA
static SDL_Joystick *left_nub = NULL;
static SDL_Joystick *right_nub = NULL;
Eina_Bool joyUse;
#endif

static EGE_Anime*	viewAnim;
EGE_bullet_list*	bl;
EGE_bullet_list* EGE_player_get_bl(void) {return bl;}
void		 runTickPlayer(EGE_Anime* a);
EGE_Sound* power;
EGE_Sound* touched;
EGE_Sound* shoot;

void		EGE_player_init(void) {
	power	= EGE_Game_find_Sound(g, "power.wav");
	touched	= EGE_Game_find_Sound(g, "touched.wav");
	shoot	= EGE_Game_find_Sound(g, "shoot2.wav");
#ifdef PANDORA
	int i  = 0;
	for( i = 0; i < SDL_NumJoysticks(); i++) {
		if (!strcmp(SDL_JoystickName(i), "nub0"))
			left_nub = SDL_JoystickOpen(i);

		else if (!strcmp(SDL_JoystickName(i), "nub1"))
			right_nub = SDL_JoystickOpen(i);
	}
#endif
}

EGE_player*	EGE_player_new(EGE_Scene* scene, EGE_RGroup* grp, EGE_RGroup* shad, EGE_bullet_list* a_bl, EGE_progressBar* lb) {
	EGE_vector	b	= {0,0,ANG_90};
	EGE_vector	p	= {fp_from_int(50),fp_from_int(50),fp_from_float(0.1)};
	EGE_vector shadOffset	= {fp_1*SHADOW_OFFX,fp_1*SHADOW_OFFY,0};
	EGE_Segment*	s	= EGE_RGroup_Sprite_add(grp, p, "playerR1");
	EGE_Segment*	sh	= EGE_RGroup_Sprite_add(shad, p, "playerR1");
	EGE_Segment_move_by(sh, shadOffset);
	EGE_Segment_scale_by(sh, SHADOW_SCALE);

	if(s==NULL) return NULL;
	EGE_Segment_rotate_by(s, b);
	EGE_Segment_rotate_by(sh, b);

	EGE_player*	ret	= calloc(1, sizeof(EGE_player));
	ret->seg		= s;
	ret->shadow		= sh;
	ret->anime		= EGE_Scene_add_anime(scene, s);
	ret->anime->tickLeft	= 100;
	ret->anime->custom	= runTickPlayer;
	ret->anime->obj		= (void*)ret;
	ret->maxLife		= 50;
	ret->lb			= lb;
	viewAnim		= ret->anime;
	bl			= a_bl;
	EGE_player_reset(ret, EINA_TRUE);

	return ret;
}

void	EGE_player_setSprite(EGE_player* pl) {
	int s	= 1;
	char name[16];
	char q;
	if (pl->strength>18)		q='B';
	else if (pl->strength>12)	q='V';
	else				q='R';
	
	if (pl->maxLife>90)		s=3;
	else if (pl->maxLife>=70)	s=2;
	sprintf(name, "player%c%d", q, s);
	EGE_Segment_set_sprite_tex(pl->seg, name);
	EGE_Segment_set_sprite_tex(pl->shadow, name);
}

void		EGE_player_reset(EGE_player* pl, Eina_Bool full) {
	EGE_vector	v	= {fp_from_int(10)-real2fp(pl->seg->min[0]), fp_from_int(210)-real2fp(pl->seg->min[1]),fp_0};
	if (full) {
		pl->strength	= 10;
		pl->maxLife	= 50;
	}
	pl->life		= pl->maxLife;
	pl->shooting		= EINA_FALSE;
	pl->lastShoot		= 0;
	pl->anime->move[0]	= fp_0;
	pl->anime->move[1]	= fp_0;
	EGE_player_setSprite(pl);
	EGE_Segment_move_by(pl->seg,v);
	EGE_Segment_move_by(pl->shadow,v);
}

void		 EGE_player_setLife(EGE_player* pl, int life) {
	pl->life		= life;
	if (life>pl->maxLife)
		pl->life	= pl->maxLife;
	if (life<0)
		pl->life	= 0;
	EGE_progressBar_setMax(pl->lb, pl->maxLife);
	EGE_progressBar_set(pl->lb, pl->life);
}


Eina_Bool	EGE_player_Colide(EGE_player* pl, EGE_real* tl, EGE_real* br) {
	EGE_real x=(br[0]+tl[0])/2,y=(br[1]+tl[1])/2;
	if (pl->seg->min[0]<x && x<pl->seg->max[0] && pl->seg->min[1]<y && y<pl->seg->max[1]) return EINA_TRUE;
	return EINA_FALSE;
}

void	runTickPlayer(EGE_Anime* a) {
	int i;
	EGE_bullet* b;
	EGE_bonus* o;
	EGE_vector v={fp_0,fp_0,fp_0};
	EGE_player* player = (EGE_player*)a->obj;
	for(i=0;i<2;i++) {
		if (a->seg->min[i]<=fp2real(EGE_viewport.cur[i]))
			v[i]=EGE_viewport.cur[i]-real2fp(a->seg->min[i]);
		if (a->seg->max[i]>fp2real(EGE_viewport.br[i]))
			v[i]=EGE_viewport.br[i]-real2fp(a->seg->max[i]);
			
	}
	if (v[0] != fp_0 || v[1] != fp_0 || v[2] != fp_0) {
		EGE_Segment_move_by(player->seg, v);
		EGE_Segment_move_by(player->shadow, v);
	}
	if (a->move[0] != fp_0 || a->move[1] != fp_0 || a->move[2] != fp_0) {
		EGE_Segment_move_by(player->seg, a->move);
		EGE_Segment_move_by(player->shadow, a->move);
	}
	a->tickLeft++;

	// collide with bonus
	o = EGE_bonus_list_Colide(EGE_bonus_get_bl(),a->seg->min,a->seg->max);
	if (o!=NULL) {
		EGE_Sound_play(power);
		
		switch(o->type) {
		case 2:
			EGE_Game_Scene_startAnime("Power Up!");
			player->strength+=2;
			EGE_player_setSprite(player);
			break;
		case 1: 
			EGE_Game_Scene_startAnime("Life Up!");
			EGE_player_setLife(player, player->life+10);
			break;
		default: 
			player->maxLife+=10;
			EGE_Game_Scene_startAnime("Shield Up!");
			EGE_player_setLife(player, player->life+10);
			EGE_player_setSprite(player);
			break;
		}
		
		EGE_bonus_list_remove_bonus(o->parent, o);
	}
	
	// Collide detection with ennemies bullets
	b = EGE_bullet_list_Colide(EGE_ennemy_get_bl(),a->seg->min,a->seg->max);
	if (b!=NULL) {
		EGE_Sound_play(touched);

		i = player->life - b->life;
		if (i>0){
			EGE_player_setLife(player, i);
			EGE_bullet_list_remove_bullet(b->parent, b);
		}else {
			EGE_Dead_Scene_set();
			return;
		}
	}


	if (player->shooting && EGE_GetTicks()-player->lastShoot>10) {
		EGE_Sound_play(shoot);
		v[0] = real2fp(viewAnim->seg->max[0]);
		v[1] = real2fp((viewAnim->seg->min[1]+viewAnim->seg->max[1])/2) - fp_1*8;
		if (player->strength>16) {
			b = EGE_bullet_list_add_bullet(bl, v, "shootB03", player->strength/2);
			EGE_Segment_rotate_sprite(b->seg, ANG_90/4);
			b->anime->move[0]	= 24*sinx(ANG_90/4);
			b->anime->move[1]	= 12*sinx(ANG_90/4);
			b = EGE_bullet_list_add_bullet(bl, v, "shootB03", player->strength/2);
			EGE_Segment_rotate_sprite(b->seg, -ANG_90/4);
			b->anime->move[0]	= 24*sinx(ANG_90/4);
			b->anime->move[1]	= -12*sinx(ANG_90/4);
			b = EGE_bullet_list_add_bullet(bl, v, "shootB03", player->strength/2);
			b->anime->move[0]	= 24*sinx(ANG_90/4);
			b->anime->move[1]	= 0;
		}else if (player->strength>12) {
			v[1] -= 4*fp_1;
			EGE_bullet_list_add_bullet(bl, v, "shootB02", player->strength/2);
			v[1] += 8*fp_1;
			EGE_bullet_list_add_bullet(bl, v, "shootB02", player->strength/2);
		}else{
			EGE_bullet_list_add_bullet(bl, v, "shootB06", player->strength);
		}
		player->lastShoot = EGE_GetTicks();
	}	
}


Eina_Bool	EGE_player_Event_Handle(EGE_player *player, SDL_Event *e) {
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
		case SDLK_SPACE:
		case SDLK_z: player->shooting= EINA_FALSE;
		break;
		case SDLK_UP:
		case SDLK_DOWN:
			viewAnim->move[1] = fp_0;
		break;
		case SDLK_LEFT:
		case SDLK_RIGHT:
			viewAnim->move[0] = fp_0;
		break;
		default:
		break;
		}
	break;
 	case SDL_KEYDOWN:
		switch(e->key.keysym.sym) {
#ifdef PANDORA
		case SDLK_HOME:
		case SDLK_END:
		case SDLK_PAGEDOWN:
		case SDLK_PAGEUP:
#endif
		case SDLK_SPACE:
		case SDLK_z: player->shooting= EINA_TRUE;
		break;
		case SDLK_UP:
			viewAnim->move[1] = fp_from_int(-4);
		break;
		case SDLK_DOWN:
			viewAnim->move[1] = fp_from_int(4);
		break;
		case SDLK_LEFT:
			viewAnim->move[0] = fp_from_int(-4);
		break;
		case SDLK_RIGHT:
			viewAnim->move[0] = fp_from_int(4);
		break;
		default:
		break;
		}
	break;
	default:
	break;
	}
	return quit;
}


void		 EGE_player_joystick(void) {
#ifdef PANDORA
	Sint16 x, y;
	SDL_JoystickUpdate();
	x = SDL_JoystickGetAxis(left_nub, 0)/6500;
	y = SDL_JoystickGetAxis(left_nub, 1)/6500;
	if (x!=0) { viewAnim->move[0] = fp_from_int(x); joyUse = EINA_TRUE; } 
	if (y!=0) { viewAnim->move[1] = fp_from_int(y); joyUse = EINA_TRUE; }
	if (x==0 && y==0 && joyUse) { viewAnim->move[0] = fp_0; viewAnim->move[1] = fp_0; joyUse = EINA_FALSE; }
#endif
}