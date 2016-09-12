/**
 *
 *  bullets.h
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
#ifndef __EGE_BULLETS_H__
#define __EGE_BULLETS_H__
#include "scene.h"
#define EGE_bullet_PLAYER 0
#define EGE_bullet_ENNEMY 1

typedef struct _EGE_bullet_list EGE_bullet_list;
typedef struct {
	EGE_Anime*	anime;
	EGE_Segment*	seg;
	EGE_bullet_list	*parent;
	int		life;
} EGE_bullet;

EGE_bullet*	EGE_bullet_new(EGE_Anime* a, EGE_Segment* s, int life);
void		EGE_bullet_free(EGE_bullet* b);
Eina_Bool	EGE_bullet_Colide(EGE_bullet* pl, EGE_real* tl, EGE_real* br);

struct _EGE_bullet_list {
	Eina_List*      bullets;
	Eina_List*      reuse;
	EGE_RGroup*	rgroup;
	EGE_Scene*	scene;
	uint16_t	type;
};

EGE_bullet_list* EGE_bullet_list_new(EGE_RGroup* rg, EGE_Scene* sc, uint16_t type);
void		EGE_bullet_list_free(EGE_bullet_list* bl);
void		EGE_bullet_list_reset(EGE_bullet_list* bl);
void		EGE_bullet_list_remove_bullet(EGE_bullet_list* bl, EGE_bullet* b);
EGE_bullet*	EGE_bullet_list_add_bullet(EGE_bullet_list* bl, EGE_vector p, const char *name, int life);
EGE_bullet*	EGE_bullet_list_Colide(EGE_bullet_list* pl, EGE_real* tl, EGE_real* br);


#endif
