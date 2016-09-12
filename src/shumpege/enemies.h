/**
 *
 *  enemies.h
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
#ifndef __EGE_ENEMIES_H__
#define __EGE_ENEMIES_H__
#include "scene.h"

#define ENNEMY_A 1
#define ENNEMY_B 2
#define ENNEMY_C 3
#define ENNEMY_D 4
#define ENNEMY_E 5
#define ENNEMY_ASTERM 6
#define ENNEMY_ASTERB 7
#define ENNEMY_TOWER 8
#define ENNEMY_BOSS 9
#define ENNEMY_STR_1 1
#define ENNEMY_STR_2 2
#define ENNEMY_STR_3 3
#define ENNEMY_STR_4 4
#define ENNEMY_MV_DEFAULT 0

typedef struct _EGE_Enemies_list EGE_Enemies_list;
typedef struct {
	EGE_Anime*		anime;
	EGE_Segment*		seg;
	EGE_Segment*		shadow;
	EGE_Enemies_list*	parent;
	int			life;
	int			maxLife;
	int			altMove;
	Uint32			startTick;
	uint16_t		strength;
	uint16_t		type;
	Uint32			lastShoot;
} EGE_Enemy;

EGE_Enemy*		EGE_Enemy_new(EGE_Anime* a, EGE_Segment* s, EGE_Segment* shad, int life, uint16_t strength, uint16_t type);
void			EGE_Enemy_free(EGE_Enemy* b);
Eina_Bool		EGE_Enemy_Colide(EGE_Enemy* pl, EGE_real* tl, EGE_real* br);

struct _EGE_Enemies_list {
	Eina_List*		enemies;
	Eina_List*		reuse;
	EGE_RGroup*		rgroup;
	EGE_RGroup*		rgroupShadow;
	EGE_Scene*		scene;
};

EGE_Enemies_list*	EGE_Enemies_list_new(EGE_RGroup* rg, EGE_RGroup* rgshad, EGE_Scene* sc, EGE_bullet_list* a_bl);
void			EGE_Enemies_list_free(EGE_Enemies_list* bl);
void			EGE_Enemies_list_reset(EGE_Enemies_list* bl);
void			EGE_Enemies_list_remove_Enemy(EGE_Enemies_list* bl, EGE_Enemy* b);
EGE_Enemy*		EGE_Enemies_list_add_Enemy(EGE_Enemies_list* bl, EGE_vector p, uint16_t type, int str, int altmove);
EGE_Enemy*		EGE_Enemies_list_Colide(EGE_Enemies_list* el, EGE_real* tl, EGE_real* br);
EGE_bullet_list*	EGE_ennemy_get_bl(void);
#endif

