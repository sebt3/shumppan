/**
 *
 *  rgroup.c
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
#include "rgroup.h"
#include "pl_texture.h"

static unsigned short squareIndex[6] = { 0, 1, 2, 2, 3, 0 };

// private functions
EGE_Segment*	EGE_RGroup_Segment_new(EGE_RGroup* m, const uint16_t d, const int cnt, const unsigned short* c);
void		EGE_RGroup_Segment_free(EGE_RGroup* m, EGE_Segment* s);


EGE_RGroup*	EGE_RGroup_new(const uint16_t d, const GLenum m, const int cnt, const unsigned short* c, const Eina_Bool cols, const Eina_Bool tex) {
	EGE_RGroup* ret		= calloc(1, sizeof(EGE_RGroup));
	ret->mode		= m;
	ret->dotsCnt		= d;
	ret->dotsCur		= 0;
	ret->tilemap_w		= 0;
	ret->tilemap_h		= 0;
	ret->dots		= calloc(d*3, sizeof(EGE_real));
	ret->cols		= NULL;
	ret->tex		= NULL;
	ret->t			= NULL;
	ret->segs		= NULL;
	ret->alpha		= EINA_FALSE;
	ret->tilehack		= EINA_FALSE;
	ret->doScissor		= EINA_FALSE;
	ret->col[0]		= fp2real(fp_1);
	ret->col[1]		= fp2real(fp_1);
	ret->col[2]		= fp2real(fp_1);
	ret->col[3]		= fp2real(fp_1);
	ret->indexLen		= cnt;
	ret->indexCur		= 0;
	ret->indexData		= calloc(cnt, sizeof(unsigned short));
	if (c!=NULL)	memcpy(ret->indexData, c, cnt*sizeof(unsigned short));
	else		memset(ret->indexData, 0, cnt*sizeof(unsigned short));
	if (cols) ret->cols	= calloc(d, sizeof(EGE_quater));
	if (tex) ret->tex	= calloc(d, sizeof(EGE_texC));
	memset(ret->scissor, 0, 4*sizeof(int32_t));
	return ret;
}

EGE_RGroup*	EGE_RGroupSprite_new(const int cnt, const Eina_Bool cols, const Eina_Bool tex) {
	return EGE_RGroup_new(4*cnt, GL_TRIANGLE_STRIP, 9*cnt, NULL, cols, tex);
}


EGE_RGroup*	EGE_RGroup_from_Mesh(EGE_Mesh* m) {
	if (m==NULL) return NULL;
	EGE_RGroup* ret		= calloc(1, sizeof(EGE_RGroup));
	ret->mode		= m->mode;
	ret->dotsCnt		= m->dots_count/3;
	ret->dotsCur		= m->dots_count/3;
	ret->dots		= NULL;
	ret->cols		= NULL;
	ret->tex		= NULL;
	ret->t			= NULL;
	ret->segs		= NULL;
	ret->alpha		= m->alpha;
	ret->tilehack		= m->is_tile;
	ret->doScissor		= EINA_FALSE;
	ret->tilemap_w		= m->tilemap_w;
	ret->tilemap_h		= m->tilemap_h;
	ret->col[0]		= m->col[0];
	ret->col[1]		= m->col[1];
	ret->col[2]		= m->col[2];
	ret->col[3]		= m->col[3];
	ret->indexLen		= m->faces_count;
	ret->indexCur		= m->faces_count;
	ret->indexData		= calloc(m->faces_count, sizeof(unsigned short));
	memset(ret->scissor, 0, 4*sizeof(int32_t));
	memcpy(ret->indexData, m->faces, m->faces_count*sizeof(unsigned short));
	if(m->dots_count>0) {
		EGE_RGroup_Texture_set(ret, EGE_Game_find_Texture(g, m->texture_name));
		ret->dots = calloc(m->dots_count, sizeof(EGE_real));
		if (m->cols_count>0)
			ret->cols = calloc(m->cols_count, sizeof(EGE_real));
		if (m->tex_count>0)
			ret->tex = calloc(m->tex_count, sizeof(EGE_real));
#ifdef HAVE_GLES
		memcpy(ret->dots, m->dots, m->dots_count*sizeof(EGE_fp));
		if (m->cols_count>0)
			memcpy(ret->cols, m->cols, m->cols_count*sizeof(EGE_fp));
		if (m->tex_count>0)
			memcpy(ret->tex, m->tex, m->tex_count*sizeof(EGE_fp));
#else
		memcpy(ret->dots, m->fdots, m->dots_count*sizeof(float));
		if (m->cols_count>0)
			memcpy(ret->cols, m->fcols, m->cols_count*sizeof(float));
		if (m->tex_count>0)
			memcpy(ret->tex, m->ftex, m->tex_count*sizeof(float));
#endif
	}
	return ret;
}

void		EGE_RGroup_free(EGE_RGroup* m) {
	EGE_Segment *s;

	if (m->dots != NULL)
		free(m->dots);
	if (m->cols != NULL)
		free(m->cols);
	if (m->tex != NULL)
		free(m->tex);
	if (m->indexData != NULL)
		free(m->indexData);

	EINA_LIST_FREE(m->segs, s) {
		EGE_RGroup_Segment_free(m, s);
	}

	free(m);

}

void		EGE_RGroup_empty(EGE_RGroup* m) {
	EGE_Segment *s;

	m->dotsCur		= 0;
	m->indexCur		= 0;
	EINA_LIST_FREE(m->segs, s) {
		EGE_RGroup_Segment_free(m, s);
	}
}

void		EGE_RGroup_draw(EGE_RGroup* m) {
	if (m->dots == NULL || m->dotsCur<1 || m->indexCur<1 )
		return;

	if (m->t != NULL) {
		GL_CHECK(glEnable(GL_TEXTURE_2D));
		EGE_Texture_bind(m->t);
	}

	if (m->doScissor) {
		glScissor(m->scissor[0], m->scissor[1], m->scissor[2], m->scissor[3]);
		GL_CHECK(glEnable(GL_SCISSOR_TEST));
	}

	if (m->alpha)
		GL_CHECK(glEnable(GL_BLEND));

	if (m->cols != NULL)
		glEnableClientState(GL_COLOR_ARRAY);
	else
		glColor(m->col[0], m->col[1], m->col[2], m->col[3]);

	if (m->tex != NULL)
		GL_CHECK(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
	if (m->cols != NULL)
		GL_CHECK(glColorPointer(4, EGE_POINTER_TYPE, 0, m->cols));
	if (m->tex != NULL)
		GL_CHECK(glTexCoordPointer(2, EGE_POINTER_TYPE, 0, m->tex));
	GL_CHECK(glVertexPointer(3, EGE_POINTER_TYPE, 0, m->dots));
	if (m->tilehack) {
		int w = m->tilemap_h*9*41;
		int x = 9*(fp_to_int(EGE_viewport.cur[0])/m->t->tile_w)*m->tilemap_h;
		if (w+x>m->indexCur)
			w=m->indexCur-x;
		GL_CHECK(glDrawElements(m->mode, w, GL_UNSIGNED_SHORT, m->indexData+x));
	} else {
		GL_CHECK(glDrawElements(m->mode, m->indexCur, GL_UNSIGNED_SHORT, m->indexData));
	}

	if (m->tex != NULL)
		GL_CHECK(glDisableClientState(GL_TEXTURE_COORD_ARRAY));
	if (m->cols != NULL)
		GL_CHECK(glDisableClientState(GL_COLOR_ARRAY));
	if (m->t != NULL)
		GL_CHECK(glDisable(GL_TEXTURE_2D));

	if (m->alpha)
		GL_CHECK(glDisable(GL_BLEND));

	if (m->doScissor)
		GL_CHECK(glDisable(GL_SCISSOR_TEST));
}

void		EGE_RGroup_Texture_set(EGE_RGroup* m, EGE_Texture* t) {
	m->t  = t;
	if (t->format == EGE_TEXTURE_FORMAT_4444 || t->format == EGE_TEXTURE_FORMAT_ALPHA)
		m->alpha	= EINA_TRUE;
}

EGE_Segment*	EGE_RGroup_Segment_new(EGE_RGroup* m, const uint16_t d, const int cnt, const unsigned short* c) {
	int i;
	EGE_Segment* ret	= EGE_Segment_new(d, m->mode,0, NULL); //cnt,c);

	ret->dots		= m->dots+m->dotsCur*3;
	ret->t			= m->t;
	ret->alpha		= m->alpha;
	ret->shouldfree		= EINA_FALSE;
	ret->indexLen		= cnt;
	ret->indexData		= m->indexData+sizeof(unsigned short)*(m->indexCur+1);
	if (m->tex) ret->tex	= m->tex+m->dotsCur*2;
	if (m->cols) ret->cols	= m->cols+m->dotsCur*4;

	if (c!=NULL) {
		if (m->indexCur>0) {
			m->indexData[m->indexCur+0] = m->indexData[m->indexCur-1];
			m->indexData[m->indexCur+1] = m->indexData[m->indexCur-1];
			m->indexData[m->indexCur+2] = c[0]+m->dotsCur;
			m->indexCur+=3;
		}
		for (i=0;i<cnt;i++)
			m->indexData[m->indexCur+i] = c[i]+m->dotsCur;
	} else
		memset(ret->indexData, 0, cnt);
	m->segs			= eina_list_append(m->segs, ret);
	m->dotsCur		+= d;
	m->indexCur		+= cnt;
	return ret;
}

void		EGE_RGroup_Segment_free(EGE_RGroup* m, EGE_Segment* s) {
	m->segs			= eina_list_remove(m->segs, s);
	EGE_Segment_free(s);
}

EGE_Segment*	EGE_RGroup_Mesh_add(EGE_RGroup* rg, EGE_Mesh* m) {
	if (m==NULL || m->dots_count<1) return NULL;
	EGE_Segment *s = EGE_RGroup_Segment_new(rg, m->dots_count/3, m->faces_count, m->faces);
	EGE_RGroup_Texture_set(rg, EGE_Game_find_Texture(g, m->texture_name));
#ifdef HAVE_GLES
	memcpy(s->dots, m->dots, m->dots_count*sizeof(EGE_fp));
	if (m->cols_count>0)
		memcpy(s->cols, m->cols, m->cols_count*sizeof(EGE_fp));
	if (m->tex_count>0)
		memcpy(s->tex, m->tex, m->tex_count*sizeof(EGE_fp));
#else
	memcpy(s->dots, m->fdots, m->dots_count*sizeof(float));
	if (m->cols_count>0)
		memcpy(s->cols, m->fcols, m->cols_count*sizeof(float));
	if (m->tex_count>0)
		memcpy(s->tex, m->ftex, m->tex_count*sizeof(float));
#endif
	return s;
}

EGE_Segment*	EGE_RGroup_Rect_add(EGE_RGroup* m, EGE_vector tl, EGE_vector br) {
	EGE_vector p0;
	EGE_Segment *s = EGE_RGroup_Segment_new(m, 4, 6, squareIndex);
	p0[0] = tl[0];
	p0[1] = tl[1];
	p0[2] = tl[2];
	EGE_Segment_dot_set(s, 0, p0);
	p0[0] = br[0];
	EGE_Segment_dot_set(s, 1, p0);
	p0[1] = br[1];
	EGE_Segment_dot_set(s, 2, p0);
	p0[0] = tl[0];
	EGE_Segment_dot_set(s, 3, p0);
	return s;
}

EGE_Segment*	EGE_RGroup_Tile_add(EGE_RGroup* m, EGE_vector p, int32_t id) {
	int maxts, ix, iy;
	EGE_vector tl = {fp_0, fp_0, fp_0};
	EGE_vector br = {fp_0, fp_0, fp_0};

	if (! m->t->tilesheet) return NULL;
	if (id == 0) return NULL;
	maxts = m->t->orig_w/m->t->tile_w;
	iy = (id-1)/maxts;
	ix = (id-1) - iy*maxts;
	tl[0]= fp_mul(fp_from_int(ix), fp_from_int(m->t->tile_w));
	tl[1]= fp_mul(fp_from_int(iy), fp_from_int(m->t->tile_h));
	br[0]= fp_mul(fp_from_int(ix+1), fp_from_int(m->t->tile_w));
	br[1]= fp_mul(fp_from_int(iy+1), fp_from_int(m->t->tile_h));

	return EGE_RGroup_TexturePart_add(m, p, tl, br);
}

EGE_Segment*	EGE_RGroup_Sprite_add(EGE_RGroup* m, EGE_vector p, const char *name) {
	EGE_Segment *s;
	EGE_vector br = {fp_0, fp_0, fp_0};
	EGE_sprite_coord* sc = EGE_Texture_get_sprite(m->t, name);
	if (sc==NULL) return NULL;
	br[0] = p[0] + fp_from_int(sc->sw);
	br[1] = p[1] + fp_from_int(sc->sh);
	s = EGE_RGroup_Rect_add(m, p, br);
#ifdef HAVE_GLES
	memcpy(s->tex, sc->tex, 8*sizeof(EGE_fp));
#else
	memcpy(s->tex, sc->ftex, 8*sizeof(float));
#endif
	return s;
}


EGE_Segment*	EGE_RGroup_TexturePart_add(EGE_RGroup* m, EGE_vector p, EGE_vector tl, EGE_vector br) {
	EGE_texC  t0;
	EGE_vector p0;
	EGE_Segment *s = EGE_RGroup_Segment_new(m, 4, 6, squareIndex);
	t0[0] = fp_div(tl[0]+fp_from_float(0.1), fp_from_int(m->t->w));
	t0[1] = fp_div(tl[1]+fp_from_float(0.1), fp_from_int(m->t->h));
	p0[0] = p[0];
	p0[1] = p[1];
	p0[2] = p[2];
	EGE_Segment_dot_set(s, 0, p0);	EGE_Segment_tex_set(s, 0, t0);
	t0[0] = fp_div(br[0]-fp_from_float(0.1), fp_from_int(m->t->w));
	p0[0] = p[0] + br[0] - tl[0];
	EGE_Segment_dot_set(s, 1, p0);	EGE_Segment_tex_set(s, 1, t0);
	t0[1] = fp_div(br[1]-fp_from_float(0.1), fp_from_int(m->t->h));
	p0[1] = p[1] + br[1] - tl[1];
	EGE_Segment_dot_set(s, 2, p0);	EGE_Segment_tex_set(s, 2, t0);
	t0[0] = fp_div(tl[0]+fp_from_float(0.1), fp_from_int(m->t->w));
	p0[0] = p[0];
	EGE_Segment_dot_set(s, 3, p0);	EGE_Segment_tex_set(s, 3, t0);
	return s;
}

EGE_Segment*	EGE_RGroup_Tilemap_Tile_add(EGE_RGroup* m, int32_t x, int32_t y, int32_t id)  {
	int maxts, ix, iy;
	EGE_vector p  = {fp_0, fp_0, fp_0};
	EGE_vector tl = {fp_0, fp_0, fp_0};
	EGE_vector br = {fp_0, fp_0, fp_0};

	if (! m->t->tilesheet) return NULL;
	if (id == 0) return NULL;
	maxts = m->t->orig_w/m->t->tile_w;
	iy = (id-1)/maxts;
	ix = (id-1) - iy*maxts;
	p[0] = fp_mul(fp_from_int(x), fp_from_int(m->t->tile_w));
	p[1] = fp_mul(fp_from_int(y), fp_from_int(m->t->tile_h));
	tl[0]= fp_mul(fp_from_int(ix), fp_from_int(m->t->tile_w));
	tl[1]= fp_mul(fp_from_int(iy), fp_from_int(m->t->tile_h));
	br[0]= fp_mul(fp_from_int(ix+1), fp_from_int(m->t->tile_w));
	br[1]= fp_mul(fp_from_int(iy+1), fp_from_int(m->t->tile_h));

	return EGE_RGroup_TexturePart_add(m, p, tl, br);
}


void		EGE_RGroup_set_font(EGE_RGroup* m, EGE_Font *f) {
	m->f = f;
	if(EGE_viewport.scaleFactor>1)
		EGE_RGroup_Texture_set(m,f->texture2);
	else
		EGE_RGroup_Texture_set(m,f->texture);
	
}

void		EGE_RGroup_Text_add(EGE_RGroup* m, char* text, EGE_vector p) {
	float 			x = fp_to_float(p[0]);
	float 			y = fp_to_float(p[1]);
	stbtt_bakedchar*	cdata;
	stbtt_aligned_quad	q;

	if(EGE_viewport.scaleFactor>1)
		cdata = m->f->cdata2;
	else
		cdata = m->f->cdata;
	stbtt_GetBakedQuad(cdata, 512,512, 1, &x,&y,&q,1);
	x = fp_to_float(p[0]);
	y+= q.y1-q.y0;
	EGE_texC  t0;
	EGE_vector p0;

	while (*text) {
		if (*text >= 32 && *text < 128) {
			stbtt_GetBakedQuad(cdata, 512,512, *text-32, &x,&y,&q,1);//1=opengl,0=old d3d
			EGE_Segment *s = EGE_RGroup_Segment_new(m, 4, 6, squareIndex);
			t0[0] = fp_from_float(q.s0);
			t0[1] = fp_from_float(q.t0);
			p0[0] = fp_from_float(q.x0);
			p0[1] = fp_from_float(q.y0);
			p0[2] = p[2];
			EGE_Segment_dot_set(s, 0, p0);	EGE_Segment_tex_set(s, 0, t0);
			t0[0] = fp_from_float(q.s1);
			p0[0] = fp_from_float(q.x1);
			EGE_Segment_dot_set(s, 1, p0);	EGE_Segment_tex_set(s, 1, t0);
			t0[1] = fp_from_float(q.t1);
			p0[1] = fp_from_float(q.y1);
			EGE_Segment_dot_set(s, 2, p0);	EGE_Segment_tex_set(s, 2, t0);
			t0[0] = fp_from_float(q.s0);
			p0[0] = fp_from_float(q.x0);
			EGE_Segment_dot_set(s, 3, p0);	EGE_Segment_tex_set(s, 3, t0);
		}
		++text;
	}
}

void		EGE_RGroup_uniq_col_set(EGE_RGroup* s, const EGE_quater c) {
	s->col[0] = fp2real(c[0]);
	s->col[1] = fp2real(c[1]);
	s->col[2] = fp2real(c[2]);
	s->col[3] = fp2real(c[3]);
}

void		EGE_RGroup_move_by(EGE_RGroup* rg, const EGE_vector p) {
	int i, j;
	for(i=0;i<rg->dotsCur;i++)
		for(j=0;j<3;j++) {
 			rg->dots[3*i+j] += fp2real(p[j]);
		}
}