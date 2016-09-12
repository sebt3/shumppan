/**
 *
 *  rgroup.h
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
#ifndef __EGE_RGROUP_H__
#define __EGE_RGROUP_H__
#include "segment.h"
#include "mesh.h"

typedef struct {
	GLenum		mode;
	Eina_Bool	alpha;
	Eina_Bool	tilehack;
	uint16_t	tilemap_w;
	uint16_t	tilemap_h;
	
	Eina_Bool	doScissor;
	int32_t		scissor[4];
	
	EGE_real*	dots;
	EGE_real*	cols;
	EGE_real	col[4];
	EGE_real*	tex;
	uint16_t	dotsCur;
	uint16_t	dotsCnt;

	unsigned short*	indexData;
	uint16_t	indexCur;
	uint16_t	indexLen;

	EGE_Texture*	t;
	EGE_Font*	f;
	
	Eina_List*	segs;
} EGE_RGroup;

/***********************************************************************************
 *				Setup render groups
 ***********************************************************************************/

EGE_RGroup*	EGE_RGroup_new(const uint16_t d, const GLenum m, const int cnt, const unsigned short* c, const Eina_Bool cols, const Eina_Bool tex);
EGE_RGroup*	EGE_RGroupSprite_new(const int cnt, const Eina_Bool cols, const Eina_Bool tex);
EGE_RGroup*	EGE_RGroup_from_Mesh(EGE_Mesh* m);
void		EGE_RGroup_free(EGE_RGroup* m);

void		EGE_RGroup_set_font(EGE_RGroup* m, EGE_Font *f);
void		EGE_RGroup_Texture_set(EGE_RGroup* m, EGE_Texture* t);
void		EGE_RGroup_uniq_col_set(EGE_RGroup* s, const EGE_quater c);

/***********************************************************************************
 *				Adding objects
 ***********************************************************************************/

void		EGE_RGroup_Text_add(EGE_RGroup* m, char* text, EGE_vector p);
EGE_Segment*	EGE_RGroup_Rect_add(EGE_RGroup* m, EGE_vector tl, EGE_vector br);
EGE_Segment*	EGE_RGroup_TexturePart_add(EGE_RGroup* m, EGE_vector p, EGE_vector tl, EGE_vector br);
EGE_Segment*	EGE_RGroup_Tilemap_Tile_add(EGE_RGroup* m, int32_t x, int32_t y, int32_t id);
EGE_Segment*	EGE_RGroup_Tile_add(EGE_RGroup* m, EGE_vector p, int32_t id);
EGE_Segment*	EGE_RGroup_Sprite_add(EGE_RGroup* m, EGE_vector p, const char *name);
EGE_Segment*	EGE_RGroup_Mesh_add(EGE_RGroup* rg, EGE_Mesh* m);

/***********************************************************************************
 *				runtime functions
 ***********************************************************************************/
void		EGE_RGroup_empty(EGE_RGroup* m);
void		EGE_RGroup_draw(EGE_RGroup* m);
void		EGE_RGroup_move_by(EGE_RGroup* rg, const EGE_vector p);

#endif
