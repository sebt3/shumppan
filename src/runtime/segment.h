/**
 *
 *  segment.c
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
#ifndef __EGE_PL_SEGMENT_H__
#define __EGE_PL_SEGMENT_H__
#include "texture.h"
#include "fp.h"

#ifdef HAVE_GLES
#define fp2real(n) n
#define real2fp(n) n
typedef GLfixed EGE_real;
#else
#define fp2real(n) fp_to_float(n)
#define real2fp(n) fp_from_float(n)
typedef GLfloat EGE_real;
#endif

typedef struct {
	GLenum		mode;
	Eina_Bool	alpha;
	Eina_Bool	shouldfree;

	EGE_real*	dots;
	EGE_real*	cols;
	EGE_real	col[4];
	EGE_real*	tex;
	uint16_t	dotsCnt;

	unsigned short*	indexData;
	uint16_t	indexLen;

	EGE_Texture*	t;

	EGE_real	min[3], max[3];
} EGE_Segment;

/***********************************************************************************
 *				Setup segments
 ***********************************************************************************/

EGE_Segment*	EGE_Segment_new(const uint16_t d, const GLenum m, const int cnt, const unsigned short* c);
void		EGE_Segment_free(EGE_Segment* s);
void		EGE_Segment_draw(EGE_Segment* s);
void		EGE_Segment_Texture_set(EGE_Segment* s, EGE_Texture* t);
void		EGE_Segment_uniq_col_set(EGE_Segment* s, const EGE_quater c);
void		EGE_Segment_col_set(EGE_Segment* s, const int id, const EGE_quater c);
void		EGE_Segment_tex_set(EGE_Segment* s, const int id, const EGE_texC t);
void		EGE_Segment_dot_set(EGE_Segment* s, const int id, const EGE_vector p);

/***********************************************************************************
 *				runtime functions
 ***********************************************************************************/

void		EGE_Segment_move_by(EGE_Segment* s, const EGE_vector p);
void		EGE_Segment_scale_by(EGE_Segment* s, EGE_real b);
void		EGE_Segment_rotate_by(EGE_Segment* s, const EGE_vector a);
void		EGE_Segment_rotate_sprite(EGE_Segment* s, const EGE_fp a);
void		EGE_Segment_set_sprite(EGE_Segment* s, EGE_vector p, const char *name);
void		EGE_Segment_set_sprite_tex(EGE_Segment* s, const char *name);
void		EGE_Segment_sprite_hide(EGE_Segment* s);
#endif
