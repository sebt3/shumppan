/**
 *
 *  ui.c
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
#include "game.h"
#include "ui.h"

/***********************************************************************************
 *				ProgressBar
 ***********************************************************************************/

EGE_progressBar* EGE_progressBar_new(EGE_vector tl, EGE_vector br, EGE_RGroup* rg, EGE_Scene* scene) {
	EGE_vector tlb		= {tl[0]+fp_1,tl[1]+fp_1, tl[2]};
	EGE_vector brb		= {br[0]-fp_1,br[1]-fp_1, tl[2]};
	EGE_progressBar* ret	= calloc(1, sizeof(EGE_progressBar));
	ret->value		= 0;
	ret->max		= 0;
	ret->rg			= rg;
	ret->scene		= scene;
	ret->box		= EGE_RGroup_Rect_add(rg, tl, br);
	ret->lseg		= EGE_RGroup_Rect_add(rg, tlb, brb);
	ret->rseg		= EGE_RGroup_Rect_add(rg, tlb, brb);
	return ret;
}

void		 EGE_progressBar_setMax(EGE_progressBar* p, int max) {
	p->max			= max;
	EGE_progressBar_set(p, max);
}

void		 EGE_progressBar_set(EGE_progressBar* p, int value) {
	EGE_vector ts		= {real2fp(p->lseg->min[0])+fp_mul(real2fp(p->rseg->max[0]-p->lseg->min[0]),fp_div(fp_from_int(value),fp_from_int(p->max))),
				   real2fp(p->lseg->min[1]),
				   real2fp(p->rseg->dots[2])};
	EGE_vector bs		= {real2fp(p->lseg->min[0])+fp_mul(real2fp(p->rseg->max[0]-p->lseg->min[0]),fp_div(fp_from_int(value),fp_from_int(p->max))),
				   real2fp(p->rseg->max[1]), 
				   real2fp(p->rseg->dots[2])};
	p->value		= value;
	EGE_Segment_dot_set(p->lseg,1,ts);
	EGE_Segment_dot_set(p->lseg,2,bs);
	EGE_Segment_dot_set(p->rseg,0,ts);
	EGE_Segment_dot_set(p->rseg,3,bs);
}

void		 EGE_progressBar_setLBColors(EGE_progressBar* p) {
	EGE_quater black	= {fp_0, fp_0, fp_0, fp_1};
	EGE_quater red		= {fp_1, fp_0, fp_0, fp_1};
	EGE_quater green	= {fp_0, fp_1, fp_0, fp_1};
	EGE_Segment_col_set(p->box, 0, black);
	EGE_Segment_col_set(p->box, 1, black);
	EGE_Segment_col_set(p->box, 2, black);
	EGE_Segment_col_set(p->box, 3, black);
	EGE_Segment_col_set(p->lseg, 0, green);
	EGE_Segment_col_set(p->lseg, 1, green);
	EGE_Segment_col_set(p->lseg, 2, green);
	EGE_Segment_col_set(p->lseg, 3, green);
	EGE_Segment_col_set(p->rseg, 0, red);
	EGE_Segment_col_set(p->rseg, 1, red);
	EGE_Segment_col_set(p->rseg, 2, red);
	EGE_Segment_col_set(p->rseg, 3, red);
}

void		 EGE_progressBar_setSimpleColors(EGE_progressBar* p) {
	EGE_quater black	= {fp_0, fp_0, fp_0, fp_1};
	EGE_quater white	= {fp_1, fp_1, fp_1, fp_1};
	EGE_Segment_col_set(p->box, 0, black);
	EGE_Segment_col_set(p->box, 1, black);
	EGE_Segment_col_set(p->box, 2, black);
	EGE_Segment_col_set(p->box, 3, black);
	EGE_Segment_col_set(p->lseg, 0, white);
	EGE_Segment_col_set(p->lseg, 1, white);
	EGE_Segment_col_set(p->lseg, 2, white);
	EGE_Segment_col_set(p->lseg, 3, white);
	EGE_Segment_col_set(p->rseg, 0, black);
	EGE_Segment_col_set(p->rseg, 1, black);
	EGE_Segment_col_set(p->rseg, 2, black);
	EGE_Segment_col_set(p->rseg, 3, black);
}

void		 EGE_progressBar_free(EGE_progressBar* p) {
	free(p);
}


/***********************************************************************************
 *				Buttons-Cursor
 ***********************************************************************************/

EGE_Button*	EGE_Button_new(EGE_vector tl, EGE_vector br, char* label, EGE_Joy_CB onClick) {
	EGE_Button* ret	= calloc(1, sizeof(EGE_Button));
	ret->tl[0]	= tl[0];
	ret->tl[1]	= tl[1];
	ret->tl[2]	= tl[2];
	ret->br[0]	= br[0];
	ret->br[1]	= br[1];
	ret->br[2]	= br[2];
	ret->label	= label;
	ret->onClick	= onClick;
	ret->quit	= EINA_FALSE;
	return ret;
}

void		EGE_Button_free(EGE_Button* b) {
	free(b);
}

EGE_Cursor*	 EGE_Cursor_new(int maxButton, int width, EGE_RGroup* rg, EGE_RGroup* txt, EGE_Scene* scene) {
	EGE_Cursor* ret	= calloc(1, sizeof(EGE_Cursor));
	ret->width	= width;
	ret->curButton	= 0;
	ret->maxButton	= maxButton;
	ret->buttons	= calloc(maxButton, sizeof(EGE_Button*));
	ret->rg		= rg;
	ret->txt	= txt;
	ret->scene	= scene;
	ret->value	= NULL;
	ret->seg	= NULL;
	return ret;
}

void		 EGE_Cursor_free(EGE_Cursor* c) {
	int i;
	for(i=0;i<c->maxButton;i++)
		EGE_Button_free(c->buttons[i]);
	if(c->value!=NULL)
		free(c->value);
	free(c->buttons);
	free(c);
}

void		 EGE_Cursor_set_button(EGE_Cursor* c, int id, EGE_Button* b) {
	EGE_vector v = {b->tl[0]+fp_1*5, b->tl[1]+fp_1*5, b->tl[2]};
	EGE_RGroup_Text_add(c->txt, b->label, v);
	c->buttons[id]	= b;
	if(id==c->curButton && c->seg == NULL) {
		c->seg	= EGE_RGroup_Rect_add(c->rg,c->buttons[id]->tl,c->buttons[id]->br);
	}
}

void		 EGE_Cursor_set_Quitbutton(EGE_Cursor* c, int id) {
	c->buttons[id]->quit	= EINA_TRUE;
}

void		runTickCursor(EGE_Anime* a) {
	EGE_Cursor* 	c	 = (EGE_Cursor*)a->obj;
	if (a->tickLeft>1) {
		c->seg->dots[9]	+= fp2real((c->buttons[a->objid]->tl[0] - real2fp(c->seg->dots[9]))/a->tickLeft);
		c->seg->dots[4]	+= fp2real((c->buttons[a->objid]->tl[1] - real2fp(c->seg->dots[4]))/a->tickLeft);
		c->seg->dots[6]	+= fp2real((c->buttons[a->objid]->br[0] - real2fp(c->seg->dots[6]))/a->tickLeft);
		c->seg->dots[7]	+= fp2real((c->buttons[a->objid]->br[1] - real2fp(c->seg->dots[7]))/a->tickLeft);
	} else {
		c->seg->dots[9]	 = fp2real(c->buttons[a->objid]->tl[0]);
		c->seg->dots[4]	 = fp2real(c->buttons[a->objid]->tl[1]);
		c->seg->dots[6]	 = fp2real(c->buttons[a->objid]->br[0]);
		c->seg->dots[7]	 = fp2real(c->buttons[a->objid]->br[1]);
	}
	c->seg->dots[0]		 = c->seg->dots[9];
	c->seg->dots[1]		 = c->seg->dots[4];
	c->seg->dots[3]		 = c->seg->dots[6];
	c->seg->dots[10]	 = c->seg->dots[7];
}

void		 EGE_Cursor_moveTo(EGE_Cursor* c, int id) {
	EGE_Anime* a	= EGE_Scene_add_anime(c->scene, c->seg);
	a->tickLeft	= 8;
	a->custom	= runTickCursor;
	a->obj		= (void*)c;
	a->objid	= id;
}

#define KEYB_VAL_LEN 15
Eina_Bool	 EGE_Cursor_Event_Handle(EGE_Cursor* c, SDL_Event *e) {
	Eina_Bool quit = EINA_FALSE;
	int i, len=0;
	if (c->value != NULL)
		len	= strlen(c->value);
	switch(e->type) {
	case SDL_KEYUP:
		switch(e->key.keysym.sym) {
		case SDLK_LEFT:
			c->curButton-=2;
			if (c->curButton<0)c->curButton+=c->maxButton;
		case SDLK_RIGHT:
			c->curButton=(c->curButton+1)%c->maxButton;
			EGE_Cursor_moveTo(c, c->curButton);
			EGE_Sound_play(EGE_Game_find_Sound(g, "slide.wav"));
		break;
		case SDLK_UP:
			c->curButton-=2*c->width;
			if (c->curButton<0)c->curButton+=c->maxButton;
		case SDLK_DOWN:
			c->curButton=(c->curButton+c->width)%c->maxButton;
			EGE_Cursor_moveTo(c, c->curButton);
			EGE_Sound_play(EGE_Game_find_Sound(g, "slide.wav"));
		break;
#ifdef PANDORA
		case SDLK_HOME:
		case SDLK_END:
		case SDLK_PAGEDOWN:
		case SDLK_PAGEUP:
#endif
		case SDLK_RETURN:
			if (c->buttons[c->curButton]->quit) quit = EINA_TRUE;
			else if (c->buttons[c->curButton]->onClick!=NULL)
				c->buttons[c->curButton]->onClick();
#ifdef PANDORA
			else if (c->value != NULL) {
				if (c->buttons[38]->onClick!=NULL)
					c->buttons[38]->onClick();
			}
#endif
		break;
		case SDLK_SPACE:
			if (c->buttons[c->curButton]->quit) quit = EINA_TRUE;
			else if (c->buttons[c->curButton]->onClick!=NULL)
				c->buttons[c->curButton]->onClick();
			else if (c->value != NULL) {
				// Keyboard mode
				if (c->curButton>9 && c->curButton<36)
					snprintf(c->value+len, KEYB_VAL_LEN-len, "%c", 'a'-10+c->curButton);
				else if (c->curButton<9)
					snprintf(c->value+len, KEYB_VAL_LEN-len, "%c", '1'+c->curButton);
				else if (c->curButton==9)
					snprintf(c->value+len, KEYB_VAL_LEN-len, "0");
				else if (c->curButton==36)
					snprintf(c->value+len, KEYB_VAL_LEN-len, " ");
				else if (c->curButton==37)
					c->value[len-1]	= 0;
			}
		break;
		default:
			if (c->value !=NULL) { // Keyboard mode
				if (e->key.keysym.sym>=SDLK_a&&e->key.keysym.sym<=SDLK_z)
					snprintf(c->value+len, KEYB_VAL_LEN-len, "%c", 'a'+e->key.keysym.sym-SDLK_a);
				else if (e->key.keysym.sym>=SDLK_0&&e->key.keysym.sym<=SDLK_9)
					snprintf(c->value+len, KEYB_VAL_LEN-len, "%c", '0'+e->key.keysym.sym-SDLK_0);
				else if (e->key.keysym.sym==SDLK_SPACE)
					snprintf(c->value+len, KEYB_VAL_LEN-len, " ");
				if (e->key.keysym.sym==SDLK_BACKSPACE)
					c->value[len-1]	= 0;
			}
		break;
		}
	break;
	case SDL_MOUSEBUTTONUP:
		for(i=0;i<c->maxButton;i++) {
 			if (fp_to_int(c->buttons[i]->tl[0])<e->button.x
			 && fp_to_int(c->buttons[i]->br[0])>e->button.x
			 && fp_to_int(c->buttons[i]->tl[1])<e->button.y
			 && fp_to_int(c->buttons[i]->br[1])>e->button.y
			) {
				c->curButton = i;
				EGE_Cursor_moveTo(c, c->curButton);
				if (c->buttons[i]->onClick != NULL)
					c->buttons[i]->onClick();
				else if (c->buttons[i]->quit) 
					quit = EINA_TRUE;
				else if (c->value != NULL) {
					// Keyboard mode
					if (c->curButton>9 && c->curButton<36)
						snprintf(c->value+len, KEYB_VAL_LEN-len, "%c", 'a'-10+c->curButton);
					else if (c->curButton<9)
						snprintf(c->value+len, KEYB_VAL_LEN-len, "%c", '1'+c->curButton);
					else if (c->curButton==9)
						snprintf(c->value+len, KEYB_VAL_LEN-len, "0");
					else if (c->curButton==36)
						snprintf(c->value+len, KEYB_VAL_LEN-len, " ");
					else if (c->curButton==37)
						c->value[len-1]	= 0;
				}
			}
		}
	break;
	default:
	break;
	}
	return quit;
}

#define KEYB_LEFT 200*fp_1
#define KEYB_TOP  350*fp_1
#define KEYB_CELW 23*fp_1
#define KEYB_STEPW fp_1*40
#define KEYB_STEPH fp_1*30


EGE_Cursor*	 EGE_Keyboard_new(EGE_RGroup* rg, EGE_RGroup* txt, EGE_Scene* scene, EGE_Joy_CB onSubmit) {
	int		i;
	char		label[8];
	EGE_vector	tl	= {KEYB_LEFT,KEYB_TOP, fp_05};
	EGE_vector	br	= {KEYB_LEFT+KEYB_CELW,KEYB_TOP+KEYB_CELW, fp_05};
	EGE_Cursor*	ret	= EGE_Cursor_new(26+10+3, 10, rg, txt, scene);
	ret->value		= calloc(KEYB_VAL_LEN, sizeof(char));
	
	for(i=0;i<10;i++) {
		sprintf(label, "%d", i+1);
		if (i==9) sprintf(label, "%d", 0);
		EGE_Cursor_set_button(ret, i, EGE_Button_new(tl,br, label, NULL));
		tl[0]	+= KEYB_STEPW;
		br[0]	+= KEYB_STEPW;
	}
	tl[0]	 = KEYB_LEFT;
	br[0]	 = KEYB_LEFT+KEYB_CELW;
	tl[1]	+= KEYB_STEPH;
	br[1]	+= KEYB_STEPH;
	for(i=0;i<26;i++) {
		sprintf(label, "%c", i+'a');
		EGE_Cursor_set_button(ret, i+10, EGE_Button_new(tl,br, label, NULL));
		tl[0]	+= KEYB_STEPW;
		br[0]	+= KEYB_STEPW;
		if (i==9 || i==19) {
			tl[0]	 = KEYB_LEFT;
			br[0]	 = KEYB_LEFT+KEYB_CELW;
			tl[1]	+= KEYB_STEPH;
			br[1]	+= KEYB_STEPH;
		}
	}
	sprintf(label, "_");
	EGE_Cursor_set_button(ret, 36, EGE_Button_new(tl,br, label, NULL));
	tl[0]	+= KEYB_STEPW;
	br[0]	+= KEYB_STEPW;
	sprintf(label, "~");
	EGE_Cursor_set_button(ret, 37, EGE_Button_new(tl,br, label, NULL));
	tl[0]	+= KEYB_STEPW;
	br[0]	+= KEYB_STEPW*2;
	sprintf(label, "[ OK ]");
	EGE_Cursor_set_button(ret, 38, EGE_Button_new(tl,br, label, onSubmit));
	return ret;
}
#undef KEYB_LEFT
#undef KEYB_TOP