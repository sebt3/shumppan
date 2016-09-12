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
#include "screen.h"
#include "segment.h"
#include "pl_texture.h"
#include "engine.h"

static void _EGE_Segment_resetMm(EGE_Segment* ret) {
	uint16_t mxv  = (1<<15)-1;
	ret->min[0]		= fp2real(fp_from_int(mxv));
	ret->min[1]		= fp2real(fp_from_int(mxv));
	ret->min[2]		= fp2real(fp_from_int(mxv));
	ret->max[0]		= fp2real(fp_from_int(-mxv));
	ret->max[1]		= fp2real(fp_from_int(-mxv));
	ret->max[2]		= fp2real(fp_from_int(-mxv));
}

EGE_Segment*	EGE_Segment_new(const uint16_t d, const GLenum m, const int cnt, const unsigned short* c) {
	EGE_Segment* ret	= calloc(1, sizeof(EGE_Segment));
	ret->mode		= m;
	ret->dotsCnt		= d;
	ret->dots		= NULL;
	ret->cols		= NULL;
	ret->tex		= NULL;
	ret->t			= NULL;
	ret->alpha		= EINA_FALSE;
	ret->shouldfree		= EINA_TRUE;
	ret->indexLen		= cnt;
	ret->indexData		= NULL;
	if (cnt>0) {
		ret->indexData	= calloc(cnt, sizeof(unsigned short));
		if (c!=NULL)	memcpy(ret->indexData, c, cnt*sizeof(unsigned short));
		else		memset(ret->indexData, 0, cnt*sizeof(unsigned short));
	}
	ret->col[0]		= fp2real(fp_1);
	ret->col[1]		= fp2real(fp_1);
	ret->col[2]		= fp2real(fp_1);
	ret->col[3]		= fp2real(fp_1);
	_EGE_Segment_resetMm(ret);
	return ret;
}

void		EGE_Segment_free(EGE_Segment* s) {
	if (s->dots != NULL && s->shouldfree)
		free(s->dots);
	if (s->cols != NULL && s->shouldfree)
		free(s->cols);
	if (s->tex != NULL && s->shouldfree)
		free(s->tex);
	if (s->indexData != NULL && s->shouldfree)
		free(s->indexData);
	// Dont free the texture here as they are shared
	free(s);
}

void		EGE_Segment_draw(EGE_Segment* s) {
	int i;
	if (s->dots == NULL)
		return;

	for(i=0;i<2 && EGE_viewport.isInterface != EINA_TRUE;i++) {
		if(s->min[i]>fp2real(EGE_viewport.br[i])) return;
		if(s->max[i]<fp2real(EGE_viewport.cur[i]))return;
	}

	if (s->alpha)
		GL_CHECK(glEnable(GL_BLEND));

	if (s->t != NULL) {
		GL_CHECK(glEnable(GL_TEXTURE_2D));
		EGE_Texture_bind(s->t);
	}
	if (s->cols != NULL)
		glEnableClientState(GL_COLOR_ARRAY);
	else
		glColor(s->col[0], s->col[1], s->col[2], s->col[3]);

	if (s->tex != NULL)
		GL_CHECK(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
	if (s->cols != NULL)
		GL_CHECK(glColorPointer(4, EGE_POINTER_TYPE, 0, s->cols));
	if (s->tex != NULL)
		GL_CHECK(glTexCoordPointer(2, EGE_POINTER_TYPE, 0, s->tex));
	GL_CHECK(glVertexPointer(3, EGE_POINTER_TYPE, 0, s->dots));
	GL_CHECK(glDrawElements(s->mode, s->indexLen, GL_UNSIGNED_SHORT, s->indexData));

	if (s->tex != NULL)
		GL_CHECK(glDisableClientState(GL_TEXTURE_COORD_ARRAY));
	if (s->cols != NULL)
		GL_CHECK(glDisableClientState(GL_COLOR_ARRAY));
	if (s->t != NULL)
		GL_CHECK(glDisable(GL_TEXTURE_2D));

	if (s->alpha)
		GL_CHECK(glDisable(GL_BLEND));
}


void		EGE_Segment_Texture_set(EGE_Segment* s, EGE_Texture* t) {
	s->t  = t;
	if (t->format == EGE_TEXTURE_FORMAT_4444 || t->format == EGE_TEXTURE_FORMAT_ALPHA)
		s->alpha	= EINA_TRUE;
}

void		EGE_Segment_dot_set(EGE_Segment* s, const int id, const EGE_vector p) {
	int i;
	if (s->dots == NULL)
		s->dots = calloc(s->dotsCnt, sizeof(EGE_vector));
	for(i=0;i<3;i++) {
		s->dots[3*id+i] = fp2real(p[i]);
		if(s->min[i]>fp2real(p[i])) s->min[i]=fp2real(p[i]);
		if(s->max[i]<fp2real(p[i])) s->max[i]=fp2real(p[i]);
	}
}

void		EGE_Segment_uniq_col_set(EGE_Segment* s, const EGE_quater c) {
	s->col[0] = fp2real(c[0]);
	s->col[1] = fp2real(c[1]);
	s->col[2] = fp2real(c[2]);
	s->col[3] = fp2real(c[3]);
}

void		EGE_Segment_col_set(EGE_Segment* s, const int id, const EGE_quater c) {
	if (s->cols == NULL)
		s->cols = calloc(s->dotsCnt, sizeof(EGE_quater));
	s->cols[4*id]   = fp2real(c[0]);
	s->cols[4*id+1] = fp2real(c[1]);
	s->cols[4*id+2] = fp2real(c[2]);
	s->cols[4*id+3] = fp2real(c[3]);
	if (c[3]<fp_1)
		s->alpha	= EINA_TRUE;
}

void		EGE_Segment_tex_set(EGE_Segment* s, const int id, const EGE_texC t) {
	if (s->tex == NULL)
		s->tex = calloc(s->dotsCnt, sizeof(EGE_texC));
	s->tex[2*id]   = fp2real(t[0]);
	s->tex[2*id+1] = fp2real(t[1]);
}


void		EGE_Segment_set_sprite(EGE_Segment* s, EGE_vector p, const char *name) {
	EGE_sprite_coord* sc		= EGE_Texture_get_sprite(s->t, name);
	s->dots[9]	= s->dots[0]	= fp2real(p[0]);
	s->dots[4]	= s->dots[1]	= fp2real(p[1]);
	s->dots[6]	= s->dots[3]	= fp2real(p[0]+fp_from_int(sc->sw));
	s->dots[7]	= s->dots[10]	= fp2real(p[1]+fp_from_int(sc->sh));
#ifdef HAVE_GLES
	memcpy(s->tex, sc->tex, 8*sizeof(EGE_fp));
#else
	memcpy(s->tex, sc->ftex, 8*sizeof(float));
#endif
}

void		EGE_Segment_set_sprite_tex(EGE_Segment* s, const char *name) {
	EGE_sprite_coord* sc		= EGE_Texture_get_sprite(s->t, name);
#ifdef HAVE_GLES
	memcpy(s->tex, sc->tex, 8*sizeof(EGE_fp));
#else
	memcpy(s->tex, sc->ftex, 8*sizeof(float));
#endif
}

void		EGE_Segment_move_by(EGE_Segment* s, const EGE_vector p) {
	int i, j;
	for(j=0;j<3;j++) {
		s->min[j] += fp2real(p[j]);
		s->max[j] += fp2real(p[j]);
	}
	for(i=0;i<s->dotsCnt;i++)
		for(j=0;j<3;j++) {
			s->dots[3*i+j] += fp2real(p[j]);
		}
}

void		EGE_Segment_scale_by(EGE_Segment* s, EGE_real b) {
	int i, j;
	EGE_real p[3];
	for(j=0;j<3;j++)
		p[j] = fp2real(fp_0);
	for(i=0;i<s->dotsCnt;i++)
		for(j=0;j<3;j++)
			p[j] += s->dots[3*i+j];
	for(j=0;j<3;j++)
		p[j] = fp2real(fp_div(real2fp(p[j]),fp_from_int(s->dotsCnt)));
	// now P is the iso-center of the object
	_EGE_Segment_resetMm(s);

	for(i=0;i<s->dotsCnt;i++)
		for(j=0;j<3;j++) {
			s->dots[3*i+j] = p[j] - fp2real(
				fp_mul(real2fp(p[j])-real2fp(s->dots[3*i+j]),real2fp(b))
			);
			if(s->min[j]>s->dots[3*i+j]) s->min[j]=s->dots[3*i+j];
			if(s->max[j]<s->dots[3*i+j]) s->max[j]=s->dots[3*i+j];
		}
}

void		EGE_Segment_rotate_by(EGE_Segment* s, const EGE_vector a) {
	int i, j;
	EGE_real p[3];
	EGE_real d[3];
	for(j=0;j<3;j++)
		p[j] = fp2real(fp_0);
	for(i=0;i<s->dotsCnt;i++)
		for(j=0;j<3;j++)
			p[j] += s->dots[3*i+j];
	for(j=0;j<3;j++)
		p[j] = fp2real(fp_div(real2fp(p[j]),fp_from_int(s->dotsCnt)));
	// now P is the iso-center of the object
	_EGE_Segment_resetMm(s);

	for(i=0;i<s->dotsCnt;i++) {
		for(j=0;j<3;j++)
			d[j] = fp2real(fp_0);
		for(j=0;j<3;j++) {
			// doing x rotation
			if (a[0]!=fp2real(fp_0)){
			if (j==1) {
				d[1] += fp2real(fp_mul(fp_sub(real2fp(s->dots[3*i+1]),real2fp(p[1])),cosx(a[0])));
				d[1] -= fp2real(fp_mul(fp_sub(real2fp(s->dots[3*i+2]),real2fp(p[2])),sinx(a[0])));
			} else if (j==2) {
				d[2] -= fp2real(fp_mul(fp_sub(real2fp(s->dots[3*i+1]),real2fp(p[1])),sinx(a[0])));
				d[2] += fp2real(fp_mul(fp_sub(real2fp(s->dots[3*i+2]),real2fp(p[2])),cosx(a[0])));
			}}
			// doing y rotation
			if (a[1]!=fp2real(fp_0)){
			if (j==0) {
				d[0] += fp2real(fp_mul(fp_sub(real2fp(s->dots[3*i+0]),real2fp(p[0])),cosx(a[1])));
				d[0] += fp2real(fp_mul(fp_sub(real2fp(s->dots[3*i+2]),real2fp(p[2])),sinx(a[1])));
			} else if (j==2) {
				d[2] -= fp2real(fp_mul(fp_sub(real2fp(s->dots[3*i+0]),real2fp(p[0])),sinx(a[1])));
				d[2] += fp2real(fp_mul(fp_sub(real2fp(s->dots[3*i+2]),real2fp(p[2])),cosx(a[1])));
			}}
			// doing z rotation
			if (a[2]!=fp2real(fp_0)){
			if (j==0) {
				d[0] += fp2real(fp_mul(fp_sub(real2fp(s->dots[3*i+0]),real2fp(p[0])),cosx(a[2])));
				d[0] -= fp2real(fp_mul(fp_sub(real2fp(s->dots[3*i+1]),real2fp(p[1])),sinx(a[2])));
			} else if (j==1) {
				d[1] += fp2real(fp_mul(fp_sub(real2fp(s->dots[3*i+0]),real2fp(p[0])),sinx(a[2])));
				d[1] += fp2real(fp_mul(fp_sub(real2fp(s->dots[3*i+1]),real2fp(p[1])),cosx(a[2])));
			}}
		}
		for(j=0;j<3;j++) {
 			s->dots[3*i+j] = d[j] + p[j];
			if(s->min[j]>s->dots[3*i+j]) s->min[j]=s->dots[3*i+j];
			if(s->max[j]<s->dots[3*i+j]) s->max[j]=s->dots[3*i+j];
		}
	}
}
void		EGE_Segment_rotate_sprite(EGE_Segment* s, const EGE_fp a) {
	int i, j;
	EGE_real p[3];
	EGE_real d[3];
	for(j=0;j<3;j++)
		p[j] = fp2real(fp_0);
	for(i=0;i<s->dotsCnt;i++)
		for(j=0;j<3;j++)
			p[j] += s->dots[3*i+j];
	for(j=0;j<3;j++)
		p[j] = fp2real(fp_div(real2fp(p[j]),fp_from_int(s->dotsCnt)));
	// now P is the iso-center of the object
	_EGE_Segment_resetMm(s);

	for(i=0;i<s->dotsCnt;i++) {
		for(j=0;j<3;j++)
			d[j] = fp2real(fp_0);
		for(j=0;j<3;j++) {
			// doing z rotation
			if (a!=fp2real(fp_0)){
			if (j==0) {
				d[0] += fp2real(fp_mul(fp_sub(real2fp(s->dots[3*i+0]),real2fp(p[0])),cosx(a)));
				d[0] -= fp2real(fp_mul(fp_sub(real2fp(s->dots[3*i+1]),real2fp(p[1])),sinx(a)));
			} else if (j==1) {
				d[1] += fp2real(fp_mul(fp_sub(real2fp(s->dots[3*i+0]),real2fp(p[0])),sinx(a)));
				d[1] += fp2real(fp_mul(fp_sub(real2fp(s->dots[3*i+1]),real2fp(p[1])),cosx(a)));
			}}
		}
		for(j=0;j<3;j++) {
 			s->dots[3*i+j] = d[j] + p[j];
			if(s->min[j]>s->dots[3*i+j]) s->min[j]=s->dots[3*i+j];
			if(s->max[j]<s->dots[3*i+j]) s->max[j]=s->dots[3*i+j];
		}
	}
}

void		EGE_Segment_sprite_hide(EGE_Segment* s) {
	memset(s->dots, 0, s->dotsCnt*3*sizeof(EGE_real));
}
