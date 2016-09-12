/**
 *
 *  scene.c
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
#include "screen.h"
#include "scene.h"

EGE_Anime*	EGE_Anime_new(EGE_Segment* s) {
	EGE_Anime* ret	= calloc(1, sizeof(EGE_Anime));
	ret->seg	= s;
	ret->onEnd	= NULL;
	ret->custom	= NULL;
	ret->tickLeft	= 0;
	ret->move[0]	= fp_0;
	ret->move[1]	= fp_0;
	ret->move[2]	= fp_0;
	ret->rot[0]	= fp_0;
	ret->rot[1]	= fp_0;
	ret->rot[2]	= fp_0;
	ret->scale	= fp2real(fp_0);
	ret->obj	= NULL;
	ret->objid	= 0;
	return ret;
}

EGE_Scene*	EGE_Scene_new() {
	EGE_Scene* ret	= calloc(1, sizeof(EGE_Scene));
	ret->rgroups	= NULL;
	ret->animes	= NULL;
	ret->joystickR	= NULL;
	ret->onEvent	= NULL;
	return ret;
}

void		EGE_Scene_free(EGE_Scene* s) {
	EGE_RGroup *m;
	EGE_Anime   *a;
	EINA_LIST_FREE(s->animes, a) {
		free(a);
	}
	EINA_LIST_FREE(s->rgroups, m) {
		EGE_RGroup_free(m);
	}
}

void		EGE_Scene_empty(EGE_Scene* s) {
	EGE_RGroup *m;
	EGE_Anime   *a;
	EINA_LIST_FREE(s->animes, a) {
		free(a);
	}
	EINA_LIST_FREE(s->rgroups, m) {
		EGE_RGroup_free(m);
	}
	s->rgroups	= NULL;
}


void		EGE_Scene_draw(EGE_Scene* s) {
	Eina_List*	l1;
	EGE_RGroup	*m;

	EINA_LIST_FOREACH(s->rgroups, l1, m) {
		EGE_RGroup_draw(m);
	}
}

void		EGE_Scene_runTick(EGE_Scene* s) {
	Eina_List*	l1;
	Eina_List*	l2 = NULL;
	EGE_Anime	*a;
	EINA_LIST_FOREACH(s->animes, l1, a) {
		if (a!=NULL && a->tickLeft>0) {
			if (a->custom != NULL) {
				a->custom(a);
			} else if (a->seg == NULL) {
				EGE_Screen_View_move(a->move);
			} else {
				if (a->scale != fp2real(fp_0))
					EGE_Segment_scale_by(a->seg, a->scale);
				if (a->move[0] != fp_0 || a->move[1] != fp_0 || a->move[2] != fp_0)
					EGE_Segment_move_by(a->seg, a->move);
				if (a->rot[0] != fp_0 || a->rot[1] != fp_0 || a->rot[2] != fp_0)
					EGE_Segment_rotate_by(a->seg, a->rot);
			}
			a->tickLeft--;
			if (a->tickLeft == 0)
				l2 = eina_list_append(l2, a);
		} else if (a!=NULL)
			l2 = eina_list_append(l2, a);
	}
	EINA_LIST_FREE(l2, a) {
		if (a->onEnd != NULL)
			a->onEnd(a);
		if (a->tickLeft<=0) {
			s->animes  = eina_list_remove(s->animes, a);
			free(a);
		}
	}
}

EGE_Anime*	EGE_Scene_add_anime(EGE_Scene* s, EGE_Segment* seg) {
	EGE_Anime* ret = EGE_Anime_new(seg);
	s->animes = eina_list_append(s->animes, ret);
	return ret;
}

void		EGE_Scene_add_rgroup(EGE_Scene* s, EGE_RGroup *m) {
	s->rgroups = eina_list_append(s->rgroups, m);
}
