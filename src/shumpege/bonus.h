/**
 *
 *  bonus.h
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
#ifndef __EGE_BONUS_H__
#define __EGE_BONUS_H__
#include "scene.h"

typedef struct _EGE_bonus_list EGE_bonus_list;
typedef struct {
	EGE_Anime*	anime;
	EGE_Segment*	seg;
	EGE_bonus_list	*parent;
	uint16_t	type;
} EGE_bonus;

EGE_bonus*	EGE_bonus_new(EGE_Anime* a, EGE_Segment* s);
void		EGE_bonus_free(EGE_bonus* b);
Eina_Bool	EGE_bonus_Colide(EGE_bonus* pl, EGE_real* tl, EGE_real* br);

struct _EGE_bonus_list {
	Eina_List*      bonus;
	Eina_List*      reuse;
	EGE_RGroup*	rgroup;
	EGE_Scene*	scene;
};
EGE_bonus_list*	EGE_bonus_get_bl(void);
EGE_bonus_list* EGE_bonus_list_new(EGE_RGroup* rg, EGE_Scene* sc);
void		EGE_bonus_list_free(EGE_bonus_list* bl);
void		EGE_bonus_list_reset(EGE_bonus_list* bl);
void		EGE_bonus_list_remove_bonus(EGE_bonus_list* bl, EGE_bonus* b);
EGE_bonus*	EGE_bonus_list_add_bonus(EGE_bonus_list* bl, EGE_vector p);
EGE_bonus*	EGE_bonus_list_Colide(EGE_bonus_list* pl, EGE_real* tl, EGE_real* br);


#endif
