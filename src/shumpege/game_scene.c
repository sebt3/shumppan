/**
 *
 *  game_scene.c
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
#include "bonus.h"
#include "score.h"
#include "enemies.h"
#include "ui.h"

#define START_LEVEL 1
#define MAX_LEVEL 10

static EGE_Scene*	gameSc;
static EGE_Scene*	uiSc;
static EGE_Enemies_list* enemies;
static EGE_player*	player;
static EGE_Enemy*	lastEnnemy;
static EGE_bonus_list*	bonus;
static EGE_bullet_list*	pl_bul;
static EGE_bullet_list*	en_bul;
static EGE_RGroup*	tilemap;
static EGE_Anime*	animeTick;
static EGE_RGroup*	grpClo;
static EGE_RGroup*	grpText;


EGE_progressBar*	lb;
EGE_progressBar*	pg;
EGE_Mesh*		emap;
int 			currentE;
int 			currentLevel;
Eina_Bool		bossMode;

EGE_player*	EGE_Game_Scene_get_player(void){return player;}
EGE_bonus_list*	EGE_Game_Scene_get_bonus(void){return bonus;}
Eina_Bool	EGE_Game_Scene_Event_Handle(SDL_Event *e);

void	runTickView(EGE_Anime* a) {
	int i;
	EGE_vector p={EGE_viewport.br[0]-fp_1*20,(EGE_viewport.br[1]-EGE_viewport.cur[1])/2,fp_from_float(0.1)};
	a->tickLeft++;
	

	// Move the viewPort
	if (bossMode && currentE>0) {
		switch(((EGE_GetTicks()-lastEnnemy->startTick)/180)%6) {
		case 0:
			a->move[0] = fp_from_float(1.2);
			a->move[1] = fp_from_float(1.8);
			break;
		case 4:
			a->move[0] = -fp_from_float(1.5);
			a->move[1] = -fp_from_float(1.5);
			break;
		case 3:
			a->move[0] = fp_from_float(1.5);
			a->move[1] = -fp_from_float(1.5);
			break;
		case 2:
			a->move[0] = -fp_1;
			a->move[1] = 0;
			break;
		case 1:
			a->move[0] = -fp_from_float(1.2);
			a->move[1] = fp_from_float(1.8);
			break;
		case 5:
			a->move[0] = fp_1;
			a->move[1] = 0;
			break;
		}
		EGE_Screen_View_move(a->move);
		// update the progress bar
		EGE_progressBar_setMax(pg, lastEnnemy->maxLife);
		EGE_progressBar_set(pg, lastEnnemy->maxLife-lastEnnemy->life);
	} else {
		a->move[0]	= fp_05*10/5;
		a->move[1]	= 0;
		EGE_Screen_View_move(a->move);
		for(i=0;i<2;i++) {
			if (EGE_viewport.br[i]>=EGE_viewport.max[i])
				EGE_Finish_Scene_set(); // end of the level reached
		}
		// update the progress bar
		EGE_progressBar_set(pg, fp_to_int(EGE_viewport.br[0]));
	}

	// add ennemies
	while (emap->dots[currentE*3]<EGE_viewport.cur[0]+fp_from_int(800) && currentE<emap->dots_count/3) {
		p[1] = emap->dots[currentE*3+1];
		if (fp_to_int(emap->norm[currentE*3])<10)
			lastEnnemy = EGE_Enemies_list_add_Enemy(enemies, p, fp_to_int(emap->norm[currentE*3]), fp_to_int(emap->norm[currentE*3+1]), fp_to_int(emap->norm[currentE*3+2]));
		else
			EGE_bonus_list_add_bonus(EGE_Game_Scene_get_bonus(), p);
		currentE++;
	}
}

void	runTickViewCloud(EGE_Anime* a) {
	EGE_vector c={fp_1,fp_1,0};
	a->tickLeft++;
	// move the clouds
	c[0]	= -animeTick->move[0]*2;
	c[1]	= -animeTick->move[1]*2;
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

static char *bonusText;
static EGE_vector btPos;
void	runTickViewBonus(EGE_Anime* a) {
	char 			buf[2];
	stbtt_aligned_quad	q;
	float 			x = fp_to_float(btPos[0]);
	float 			y = fp_to_float(btPos[1]);

	buf[1]		= 0;
	if (*bonusText!=0) {
		switch (a->tickLeft%4) {
		case 0:
			// add a character every 4 ticks
			buf[0] = *bonusText;
			EGE_RGroup_Text_add(grpText, buf, btPos);
			stbtt_GetBakedQuad(grpText->f->cdata, 512,512, *bonusText-32, &x,&y,&q,1);
			btPos[0] = fp_from_float(x+3);
			bonusText++;
		default:break;
		}
	} else if (a->tickLeft>40) a->tickLeft = 30;
	if (a->tickLeft<5) {
		EGE_RGroup_empty(grpText);
	}
}

void		EGE_Game_Scene_startAnime(char *text) {
	btPos[0]	= 320*fp_1;
	btPos[1]	= 220*fp_1;
	btPos[2]	= fp_05;
	bonusText	= text;
	EGE_Anime* a	= EGE_Scene_add_anime(uiSc, NULL);
	a->tickLeft	= 100;
	a->custom	= runTickViewBonus;
}

void		EGE_Game_Scene_init(void) {
	EGE_Texture*	tex		= EGE_Game_find_Texture(g, "sprites.png");
	EGE_Texture*	tshad		= EGE_Game_find_Texture(g, "shadows.png");
	EGE_Texture*	tclo		= EGE_Game_find_Texture(g, "clouds.png");
	EGE_RGroup*	grp		= EGE_RGroupSprite_new(  1, EINA_FALSE, EINA_TRUE);
	EGE_RGroup*	bonu		= EGE_RGroupSprite_new( 20, EINA_FALSE, EINA_TRUE);
	EGE_RGroup*	ene		= EGE_RGroupSprite_new(200, EINA_FALSE, EINA_TRUE);
	EGE_RGroup*	shad		= EGE_RGroupSprite_new(201, EINA_FALSE, EINA_TRUE);
	EGE_RGroup*	bul		= EGE_RGroupSprite_new(500, EINA_FALSE, EINA_TRUE);
	EGE_RGroup*	bule		= EGE_RGroupSprite_new(500, EINA_FALSE, EINA_TRUE);
	EGE_RGroup*	ui		= EGE_RGroupSprite_new(  6, EINA_TRUE,  EINA_FALSE);
	EGE_RGroup*	sco		= EGE_RGroupSprite_new( 50, EINA_FALSE, EINA_TRUE);
			grpClo		= EGE_RGroupSprite_new(  6, EINA_FALSE, EINA_TRUE);
			grpText		= EGE_RGroupSprite_new( 24, EINA_FALSE, EINA_TRUE);
	EGE_vector	tl		= {fp_from_int(3),   fp_from_int(3),  fp_05};
	EGE_vector	br		= {fp_from_int(120), fp_from_int(12), fp_05};
	EGE_vector	tlp		= {fp_from_int(3),   fp_from_int(12), fp_05};
	EGE_vector	brp		= {fp_from_int(120), fp_from_int(17), fp_05};
	EGE_vector 	clt		= {0,0,0};
	EGE_vector 	clb		= {fp_1*512,fp_1*512,0};
	EGE_vector	p		= {0,0,fp_05};
	EGE_quater 	grpcloud 	= {fp_from_float(0.5f), fp_from_float(0.5f),fp_from_float(0.5f),fp_from_float(0.4f)};
	EGE_quater 	shadowColor 	= {fp_from_float(0.2f), fp_from_float(0.2f),fp_from_float(0.2f),fp_from_float(0.7f)};
	EGE_Anime* 		a;

	EGE_player_init();
	uiSc		= EGE_Scene_new();
	gameSc		= EGE_Scene_new();

	// init the view animator
	tilemap		= EGE_RGroupSprite_new(350*24, EINA_FALSE, EINA_TRUE);//EGE_RGroup_from_Mesh(EGE_Game_find_Mesh(g, "level2.map"));
	emap		= EGE_Game_find_Mesh(g, "level1.emap");
	a		= EGE_Scene_add_anime(uiSc, NULL);
	a->tickLeft	= 100;
 	a->move[0]	= fp_05*10/5;
	a->custom	= runTickView;
	animeTick	= a;

	// attach the cloud animation to the game view, so they are animated even if the game isnt running
	a		= EGE_Scene_add_anime(gameSc, NULL);
	a->tickLeft	= 100;
 	a->move[0]	= fp_05*10/5;
	a->custom	= runTickViewCloud;

	// set the background
	EGE_Scene_add_rgroup(gameSc,tilemap);

	// shadows
	EGE_RGroup_Texture_set(shad,  tshad);
	EGE_RGroup_uniq_col_set(shad, shadowColor);
	EGE_Scene_add_rgroup(gameSc, shad);

	// ennemies
	EGE_RGroup_Texture_set(bule, tex);
	EGE_RGroup_Texture_set(ene,  tex);
	EGE_Scene_add_rgroup(gameSc, ene);
	EGE_Scene_add_rgroup(gameSc, bule);
	en_bul		= EGE_bullet_list_new(bule, uiSc, EGE_bullet_ENNEMY);
	enemies		= EGE_Enemies_list_new(ene, shad, uiSc, en_bul);

	// init bonus
	EGE_RGroup_Texture_set(bonu,  tex);
	EGE_Scene_add_rgroup(gameSc, bonu);
	bonus		= EGE_bonus_list_new(bonu, uiSc);

	// init the player
	EGE_RGroup_Texture_set(grp,  tex);
	EGE_RGroup_Texture_set(bul,  tex);
	EGE_Scene_add_rgroup(gameSc, grp);
	EGE_Scene_add_rgroup(gameSc, bul);
	lb		= EGE_progressBar_new(tl, br, ui, uiSc);
	pl_bul		= EGE_bullet_list_new(bul,  uiSc, EGE_bullet_PLAYER);
	player		= EGE_player_new(uiSc, grp, shad, pl_bul, lb);

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

	// Bonus messages
	EGE_RGroup_set_font(grpText, EGE_Game_find_Font(g, "Credit"));
	EGE_Scene_add_rgroup(uiSc, grpText);

	// init the UI (lifebar and co)
	uiSc->onEvent	= EGE_Game_Scene_Event_Handle;
	uiSc->joystickR = EGE_player_joystick;
	pg		= EGE_progressBar_new(tlp, brp, ui, uiSc);
	EGE_Scene_add_rgroup(uiSc, ui);
	EGE_Scene_add_rgroup(uiSc, sco);
	EGE_RGroup_set_font(sco, EGE_Game_find_Font(g, "Score"));
	EGE_progressBar_setLBColors(lb);
	EGE_progressBar_setSimpleColors(pg);
	EGE_progressBar_setMax(lb, player->maxLife);
	EGE_Score_setScene(uiSc, sco);
	currentLevel	= 1;
}

void		EGE_Game_Scene_set_ui(void) {
	EGE_setUIScene(uiSc);
}
void		EGE_Game_Scene_set_level(int level) {
	EGE_vector c={real2fp(-grpClo->dots[0]),real2fp(-grpClo->dots[1]),0};
	EGE_vector p, m;
	EGE_Mesh *tm;
	EGE_Music *u;
	char buf[128];
	if (level>MAX_LEVEL) {
		score.currentLevel=MAX_LEVEL;
		EGE_End_Scene_set();
		currentLevel = 1;
		return;
	}
	currentLevel = level;
	p[0] = p[1] = p[2] = m[2] = fp_0;
 	animeTick->move[0]	= fp_05*10/5;

	// load the ennemy map
	sprintf(buf, "level%d.emap",level);
	emap = EGE_Game_find_Mesh(g, buf);
	bossMode = (emap->dots_count==3); //only one ennemy to activate bossMode

	// load the background tileset
	sprintf(buf, "level%d.map",level);
	tm = EGE_Game_find_Mesh(g, buf);

	// load the music
	sprintf(buf, "level%d.it",level);
	u = EGE_Game_find_Music(g, buf);
	if (u==NULL) {
		sprintf(buf, "level%d.xm",level);
		u = EGE_Game_find_Music(g, buf);
	}
	if (u==NULL && level>10) {
		sprintf(buf, "level%d.it",level%10);
		u = EGE_Game_find_Music(g, buf);
		if (u==NULL) {
			sprintf(buf, "level%d.xm",level%10);
			u = EGE_Game_find_Music(g, buf);
		}
	}
	EGE_Music_play(u);

	// reset from previous play
	EGE_RGroup_empty(grpText);
	EGE_RGroup_empty(tilemap);
	EGE_RGroup_Mesh_add(tilemap, tm);
	EGE_bullet_list_reset(pl_bul);
	EGE_bullet_list_reset(en_bul);
	EGE_Enemies_list_reset(enemies);
	EGE_player_reset(player, level==1);
	currentE = 0;
	EGE_RGroup_move_by(grpClo, c);
	m[0] = fp_from_int(tilemap->t->tile_w*(tm->tilemap_w));
	m[1] = fp_from_int(tilemap->t->tile_h*(tm->tilemap_h+1));
	EGE_Screen_Area_set(p,m);
	EGE_Screen_View_move(p);
	EGE_progressBar_set(lb, player->life);
	EGE_progressBar_setMax(lb, player->maxLife);
	EGE_progressBar_setMax(pg, fp_to_int(EGE_viewport.max[0]));
	EGE_setScene(gameSc, uiSc);
	EGE_Intro_Scene_set(level);
}

void		EGE_Game_Scene_set(void) {
	EGE_Score_reset();
 	EGE_Game_Scene_set_level(START_LEVEL);
}
void		EGE_Game_Scene_set_next(void) {
	EGE_Score_nextLevel();
	EGE_Game_Scene_set_level(++currentLevel);
}

void		EGE_Game_Scene_free(void) {
	EGE_setScene(NULL,NULL);
	EGE_Scene_free(gameSc);
	EGE_Scene_free(uiSc);
}

Eina_Bool	EGE_Game_Scene_Event_Handle(SDL_Event *e) {
	Eina_Bool quit = EGE_player_Event_Handle(player, e);
	switch(e->type) {
	case SDL_QUIT:
		quit = EINA_TRUE;
	break;
	case SDL_KEYUP:
		switch(e->key.keysym.sym) {
		case SDLK_q:
		case SDLK_ESCAPE:
			EGE_Menu_Scene_set();
		break;
#ifdef PANDORA
		case SDLK_LALT:
#endif
		case SDLK_p:
			EGE_Pause_Scene_set();
		break;
		default:
		break;
		}
	break;
#ifdef CAANOO
	case SDL_JOYBUTTONUP:
		if (e->jbutton.button == 6)
			EGE_Menu_Scene_set();
	break;
#endif
	default:
	break;
	}
	return quit;
}
