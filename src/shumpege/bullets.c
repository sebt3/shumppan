/**
 *
 *  bullets.c
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
#include "scene.h"
#include "bullets.h"
void		EGE_bullet_list_remove_bullet_na(EGE_bullet_list* bl, EGE_bullet* b);

EGE_bullet*	EGE_bullet_new(EGE_Anime* a, EGE_Segment* s, int life) {
	EGE_bullet*	ret	= calloc(1, sizeof(EGE_bullet));
	ret->anime		= a;
	ret->seg		= s;
	ret->life		= life;
	return ret;
}

void		EGE_bullet_free(EGE_bullet* b) {
	free(b);
}

void		runTickBulletColide(EGE_Anime* a) {
	EGE_bullet* b	= (EGE_bullet*)a->obj;
	EGE_vector  v	= {real2fp(a->seg->min[0]), real2fp(a->seg->min[1]),fp_05};
	
	switch(a->tickLeft) {
	case 9:	EGE_Segment_set_sprite(a->seg,v,"explo1");break;
	case 8:	EGE_Segment_set_sprite(a->seg,v,"explo2");break;
	case 7:	EGE_Segment_set_sprite(a->seg,v,"explo3");break;
	case 6:	EGE_Segment_set_sprite(a->seg,v,"explo4");break;
	case 5:	EGE_Segment_set_sprite(a->seg,v,"explo5");break;
	case 4:	EGE_Segment_set_sprite(a->seg,v,"explo6");break;
	case 3:	EGE_Segment_set_sprite(a->seg,v,"explo7");break;
	case 2:	EGE_Segment_set_sprite(a->seg,v,"explo8");break;
	case 1:	EGE_Segment_sprite_hide(b->seg);
		b->parent->reuse = eina_list_append(b->parent->reuse, b);
	default:break;
	}
}

void		runTickBullet(EGE_Anime* a) {
	int i;
	a->tickLeft++;
	EGE_bullet* b = (EGE_bullet*)a->obj;
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
	if (a->tickLeft<=0) EGE_bullet_list_remove_bullet_na(b->parent, b);
	if (a->move[0] != fp_0 || a->move[1] != fp_0 || a->move[2] != fp_0)
		EGE_Segment_move_by(a->seg, a->move);
}

Eina_Bool	EGE_bullet_Colide(EGE_bullet* pl, EGE_real* tl, EGE_real* br) {
	EGE_real x=(pl->seg->min[0]+pl->seg->max[0])/2,y=(pl->seg->min[1]+pl->seg->max[1])/2;
	if (tl[0]<x && x<br[0] && tl[1]<y && y<br[1]) return EINA_TRUE;
	return EINA_FALSE;
}

EGE_bullet*	EGE_bullet_list_Colide(EGE_bullet_list* bl, EGE_real* tl, EGE_real* br) {
	Eina_List*	l;
	EGE_bullet*	b;

	EINA_LIST_FOREACH(bl->bullets, l, b) {
		if (EGE_bullet_Colide(b,tl,br))
			return b;
	}
	return NULL;
}

EGE_bullet_list* EGE_bullet_list_new(EGE_RGroup* rg, EGE_Scene* sc, uint16_t type) {
	EGE_bullet_list* ret	= calloc(1, sizeof(EGE_bullet_list));
	ret->bullets		= NULL;
	ret->reuse		= NULL;
	ret->scene		= sc;
	ret->rgroup		= rg;
	ret->type		= type;
	return ret;
}
void		EGE_bullet_list_reset(EGE_bullet_list* bl) {
	EGE_bullet	*t;
	Eina_List	*l;
	Eina_List	*l2;

	EINA_LIST_FOREACH_SAFE(bl->bullets, l, l2, t) {
		EGE_bullet_list_remove_bullet_na(bl,t);
	}
}

void		EGE_bullet_list_free(EGE_bullet_list* bl) {
	EGE_bullet* b;
	EINA_LIST_FREE(bl->bullets, b) {
		EGE_bullet_free(b);
	}
	EINA_LIST_FREE(bl->reuse, b) {
		EGE_bullet_free(b);
	}
	free(bl);
}

void		EGE_bullet_list_remove_bullet_na(EGE_bullet_list* bl, EGE_bullet* b) {
	EGE_bullet 	*t, *f=NULL;
	Eina_List*	l;

	EINA_LIST_FOREACH(bl->bullets, l, t) {
		if (t==b) {
			f=t;
			break;
		}
	}
	if (f!=NULL) {
		bl->bullets = eina_list_remove(bl->bullets, f);
		bl->reuse = eina_list_append(bl->reuse, f);
		f->anime->tickLeft = 0;
		f->anime = NULL;
		EGE_Segment_sprite_hide(f->seg);
	}
}

void		EGE_bullet_list_remove_bullet(EGE_bullet_list* bl, EGE_bullet* b) {
	EGE_bullet *t, *f=NULL;
	Eina_List *l;
	EGE_Anime*	a;
	EINA_LIST_FOREACH(bl->bullets, l, t) {
		if (t==b) {
			f=t;
			break;
		}
	}
	if (f!=NULL) {
		bl->bullets = eina_list_remove(bl->bullets, f);
		f->anime->tickLeft = 0;
		f->anime = NULL;
		// add animation for this hide
		a		= EGE_Scene_add_anime(bl->scene, f->seg);
		a->custom	= runTickBulletColide;
		a->tickLeft	= 9;
		a->obj		= (void*)f;
	}
}

EGE_bullet*	EGE_bullet_list_add_bullet(EGE_bullet_list* bl, EGE_vector p, const char *name, int life) {
	EGE_bullet* 	ret	= NULL;
	EGE_Segment*	s	= NULL;
	EGE_vector	a	= {0,0,ANG_90};
	Eina_List*	l	= eina_list_last(bl->reuse);
	if (l==NULL) {
		s		= EGE_RGroup_Sprite_add(bl->rgroup,p, name);
		ret		= EGE_bullet_new(NULL, s, life);
	} else {
		ret		= (EGE_bullet*)l->data;
		bl->reuse	= eina_list_remove(bl->reuse, ret);
	}
	// set the animator
	ret->parent		= bl;
	ret->life		= life;
	ret->anime		= EGE_Scene_add_anime(bl->scene,ret->seg);
	ret->anime->move[0]	= fp_from_int((bl->type == EGE_bullet_ENNEMY)?-5:7);
	ret->anime->tickLeft	= 10;
	ret->anime->custom	= runTickBullet;
	ret->anime->obj		= (void*)ret;
	// add positionning and set sprite
	EGE_Segment_set_sprite(ret->seg, p, name);
	// rotating in the right direction
	if(bl->type == EGE_bullet_ENNEMY) a[2] = -a[2];
	EGE_Segment_rotate_by(ret->seg, a);
	bl->bullets = eina_list_append(bl->bullets, ret);
	return ret;
}
