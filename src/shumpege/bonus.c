/**
 *
 *  bonus.c
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
#include <stdlib.h>
#include "pl_texture.h"
#include "game.h"
#include "screen.h"
#include "scene.h"
#include "bonus.h"

EGE_bonus_list*	bonusl;
EGE_bonus_list*	EGE_bonus_get_bl(void) {return bonusl;}


EGE_bonus*	EGE_bonus_new(EGE_Anime* a, EGE_Segment* s) {
	EGE_bonus*	ret	= calloc(1, sizeof(EGE_bonus));
	ret->anime		= a;
	ret->seg		= s;
	return ret;
}

void		EGE_bonus_free(EGE_bonus* b) {
	free(b);
}

void		runTickBonus(EGE_Anime* a) {
	int i;
	a->tickLeft++;
	EGE_bonus* b = (EGE_bonus*)a->obj;
	for(i=0;i<2;i++) {
		if (a->seg->min[i]<=fp2real(EGE_viewport.cur[i])) {
			a->tickLeft=0;
			EGE_Segment_sprite_hide(a->seg);
		}
		if (a->seg->max[i]>fp2real(EGE_viewport.br[i])) {
			a->tickLeft=0;
			EGE_Segment_sprite_hide(a->seg);
		}
	}
	
	// Add rotation here
	
	if (a->tickLeft<=0) EGE_bonus_list_remove_bonus(b->parent, b);
	if (a->move[0] != fp_0 || a->move[1] != fp_0 || a->move[2] != fp_0)
		EGE_Segment_move_by(a->seg, a->move);
}

Eina_Bool	EGE_bonus_Colide(EGE_bonus* pl, EGE_real* tl, EGE_real* br) {
	EGE_real x=(pl->seg->min[0]+pl->seg->max[0])/2,y=(pl->seg->min[1]+pl->seg->max[1])/2;
	if (tl[0]<x && x<br[0] && tl[1]<y && y<br[1]) return EINA_TRUE;
	return EINA_FALSE;
}

EGE_bonus*	EGE_bonus_list_Colide(EGE_bonus_list* bl, EGE_real* tl, EGE_real* br) {
	Eina_List*	l;
	EGE_bonus*	b;

	EINA_LIST_FOREACH(bl->bonus, l, b) {
		if (EGE_bonus_Colide(b,tl,br))
			return b;
	}
	return NULL;
}

EGE_bonus_list* EGE_bonus_list_new(EGE_RGroup* rg, EGE_Scene* sc) {
	EGE_bonus_list* ret	= calloc(1, sizeof(EGE_bonus_list));
	ret->bonus		= NULL;
	ret->reuse		= NULL;
	ret->scene		= sc;
	ret->rgroup		= rg;
	bonusl			= ret;
	return ret;
}
void		EGE_bonus_list_reset(EGE_bonus_list* bl) {
	EGE_bonus *t;
	Eina_List *l;
	EINA_LIST_FOREACH(bl->bonus, l, t) {
		EGE_bonus_list_remove_bonus(bl,t);
	}
}

void		EGE_bonus_list_free(EGE_bonus_list* bl) {
	EGE_bonus* b;
	EINA_LIST_FREE(bl->bonus, b) {
		EGE_bonus_free(b);
	}
	EINA_LIST_FREE(bl->reuse, b) {
		EGE_bonus_free(b);
	}
	free(bl);
}

void		EGE_bonus_list_remove_bonus(EGE_bonus_list* bl, EGE_bonus* b) {
	EGE_bonus 	*t, *f=NULL;
	Eina_List*	l;

	EINA_LIST_FOREACH(bl->bonus, l, t) {
		if (t==b) {
			f=t;
			break;
		}
	}
	if (f!=NULL) {
		bl->bonus = eina_list_remove(bl->bonus, f);
		bl->reuse = eina_list_append(bl->reuse, f);
		f->anime->tickLeft = 0;
		f->anime = NULL;
		EGE_Segment_sprite_hide(f->seg);
	}
}

EGE_bonus*	EGE_bonus_list_add_bonus(EGE_bonus_list* bl, EGE_vector p) {
	EGE_bonus* 	ret	= NULL;
	EGE_Segment*	s	= NULL;
	EGE_vector	a	= {0,0,ANG_90};
	Eina_List*	l	= eina_list_last(bl->reuse);
	uint16_t	type	= (int)random()%3;
	char		name[80];
	switch(type) {
	case 2: sprintf(name, "boxVs");break;
	case 1: sprintf(name, "boxRe");break;
	default: sprintf(name, "boxBb");break;
	}
	if (l==NULL) {
		s		= EGE_RGroup_Sprite_add(bl->rgroup,p, name);
		ret		= EGE_bonus_new(NULL, s);
	} else {
		ret		= (EGE_bonus*)l->data;
		bl->reuse	= eina_list_remove(bl->reuse, ret);
	}
	ret->type		= type;
	// set the animator
	ret->parent		= bl;
	ret->anime		= EGE_Scene_add_anime(bl->scene,ret->seg);
	ret->anime->move[0]	= -fp_1;
	ret->anime->tickLeft	= 10;
	ret->anime->custom	= runTickBonus;
	ret->anime->obj		= (void*)ret;
	// add positionning and set sprite
	EGE_Segment_set_sprite(ret->seg, p, name);
	// rotating in the right direction
	EGE_Segment_rotate_by(ret->seg, a);
	bl->bonus = eina_list_append(bl->bonus, ret);
	return ret;
}
