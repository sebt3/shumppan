/**
 *
 *  mesh.c
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
#include "mesh.h"
#include "storage.h"


static unsigned short squareIndex[6] = { 0, 1, 2, 2, 3, 0 };

EGE_Mesh*	EGE_Mesh_new(const char *m_name, const uint32_t d, const uint32_t f, const GLenum m) {
	EGE_Mesh* ret		= calloc(1, sizeof(EGE_Mesh));
	ret->name		= eina_stringshare_add(m_name);
	ret->mode		= m;
	ret->dots_max		= d*3;
	ret->faces_max		= f;
	ret->dots		= NULL;
	ret->cols		= NULL;
	ret->tex		= NULL;
	ret->norm		= NULL;
	ret->fdots		= NULL;
	ret->fcols		= NULL;
	ret->ftex		= NULL;
	ret->fnorm		= NULL;
	ret->faces		= NULL;
	ret->t			= NULL;
	ret->dots_count		= 0;
	ret->cols_count		= 0;
	ret->tex_count		= 0;
	ret->norm_count		= 0;
	ret->fdots_count	= 0;
	ret->fcols_count	= 0;
	ret->ftex_count		= 0;
	ret->fnorm_count	= 0;
	ret->faces_count	= 0;
	ret->alpha		= EINA_FALSE;
	ret->use_sub		= EINA_FALSE;
	ret->is_tile		= EINA_FALSE;
	ret->loaded		= EINA_FALSE;
	ret->tilemap_w		= 0;
	ret->tilemap_h		= 0;
	ret->col[0]		= fp_1;
	ret->col[1]		= fp_1;
	ret->col[2]		= fp_1;
	ret->col[3]		= fp_1;
	// lighting info :
	ret->ambiant[0]		= fp_1;
	ret->ambiant[1]		= fp_1;
	ret->ambiant[2]		= fp_1;
	ret->ambiant[3]		= fp_1;
	ret->difuse[0]		= fp_1;
	ret->difuse[1]		= fp_1;
	ret->difuse[2]		= fp_1;
	ret->difuse[3]		= fp_1;
	ret->specular[0]	= fp_1;
	ret->specular[1]	= fp_1;
	ret->specular[2]	= fp_1;
	ret->specular[3]	= fp_1;
	ret->transfilter[0]	= fp_1;
	ret->transfilter[1]	= fp_1;
	ret->transfilter[2]	= fp_1;
	ret->transfilter[3]	= fp_1;
	return ret;
}

void		EGE_Mesh_free(EGE_Mesh* s) {
	EGE_String_Free(s->name);
	if (s->dots != NULL)
		free(s->dots);
	if (!s->loaded) {
		if (s->cols != NULL)
			free(s->cols);
		if (s->tex != NULL)
			free(s->tex);
		if (s->norm != NULL)
			free(s->norm);
		if (s->fdots != NULL)
			free(s->fdots);
		if (s->fcols != NULL)
			free(s->fcols);
		if (s->ftex != NULL)
			free(s->ftex);
		if (s->fnorm != NULL)
			free(s->fnorm);
		if (s->faces != NULL)
			free(s->faces);
	}
	// Dont free the texture here as they are shared
	free(s);
}

void		EGE_Mesh_finalize(EGE_Mesh* s) {
	int i;
	// hide last useless break.
	if(s->faces_count>3 && s->use_sub)
		s->faces_count-=3;

	// duplicate dots
	if (s->dots != NULL) {
		s->fdots = calloc(s->dots_count, sizeof(float));
		for(i=0;i<s->dots_count;i++)
			s->fdots[i] = fp_to_float(s->dots[i]);
		s->fdots_count	= s->dots_count;
	}
	// duplicate colors
	if (s->cols != NULL) {
		s->fcols = calloc(s->dots_count*4/3, sizeof(float));
		for(i=0;i<s->dots_count*4/3;i++)
			s->fcols[i] = fp_to_float(s->cols[i]);
		s->cols_count	= s->fcols_count	= s->dots_count*4/3;
	}
	// duplicate texture coodinates
	if (s->tex != NULL) {
		s->ftex = calloc(s->dots_count*2/3, sizeof(float));
		for(i=0;i<s->dots_count*2/3;i++)
			s->ftex[i] = fp_to_float(s->tex[i]);
		s->tex_count	= s->ftex_count		= s->dots_count*2/3;
	}
	// duplicate normals
	if (s->norm != NULL) {
		s->fnorm = calloc(s->dots_count, sizeof(float));
		for(i=0;i<s->dots_count;i++)
			s->fnorm[i] = fp_to_float(s->norm[i]);
		s->norm_count	= s->fnorm_count	= s->dots_count;
	}
/*	printf("%d\nindices = [",s->dots_count);
	for(i=0;i<s->faces_count;) {
		printf(" %d",s->faces[i]);
		i++;
		if (i%24==0) printf(" ]\n[");
		else if (i%3==0) printf(" ][");
		else printf(",");
	}*/
}

void		EGE_Mesh_Texture_set(EGE_Mesh* s, EGE_Texture* t) {
	if(t==NULL)	return;
	s->t		= t;
	s->texture_name	= eina_stringshare_add(t->name);
	if (t->format  == EGE_TEXTURE_FORMAT_4444 || t->format == EGE_TEXTURE_FORMAT_ALPHA)
		s->alpha= EINA_TRUE;
}

void		EGE_Mesh_uniq_col_set(EGE_Mesh* s, const EGE_quater c) {
	s->col[0] = c[0];
	s->col[1] = c[1];
	s->col[2] = c[2];
	s->col[3] = c[3];
}

void		EGE_Mesh_dot_set(EGE_Mesh* s, const int id, const EGE_vector p) {
	int i;
	if (s->dots == NULL)
		s->dots = calloc(s->dots_max, sizeof(EGE_fp));
	for(i=0;i<3;i++)
		s->dots[3*id+i] = p[i];
}

void		EGE_Mesh_col_set(EGE_Mesh* s, const int id, const EGE_quater c) {
	if (s->cols == NULL)
		s->cols = calloc(s->dots_max/3*4, sizeof(EGE_fp));
	s->cols[4*id]   = c[0];
	s->cols[4*id+1] = c[1];
	s->cols[4*id+2] = c[2];
	s->cols[4*id+3] = c[3];
	if (c[3]<fp_1)
		s->alpha	= EINA_TRUE;
}

void		EGE_Mesh_tex_set(EGE_Mesh* s, const int id, const EGE_texC t) {
	if (s->tex == NULL)
		s->tex = calloc(s->dots_max/3*2, sizeof(EGE_fp));
	s->tex[2*id]   = t[0];
	s->tex[2*id+1] = t[1];
}

void		EGE_Mesh_norm_set(EGE_Mesh* s, const int id, const EGE_vector p) {
	if (s->norm == NULL)
		s->norm = calloc(s->dots_max, sizeof(EGE_fp));
	s->norm[3*id+0]	= p[0];
	s->norm[3*id+1]	= p[1];
	s->norm[3*id+2]	= p[2];
}

void		EGE_Mesh_finish_sub(EGE_Mesh* s, const uint16_t d, const uint16_t f, const unsigned short* idx) {
	int i;
	//if(s->faces_count>s->faces_max) printf("FAILURE:%d>%d\n",s->faces_count,s->faces_max);
	if (idx==NULL) return;
	// copy index and offset by the sub position
	if (s->faces == NULL)
		s->faces = calloc(s->faces_max, sizeof(unsigned short));
	for(i=0;i<f;i++)
		s->faces[i+s->faces_count] = idx[i] + s->dots_count/3;
	// add an invalid tri in the index to create a break
	s->faces[f+s->faces_count+0] = idx[f-1] + s->dots_count/3;
	s->faces[f+s->faces_count+1] = idx[f-1] + s->dots_count/3;
	s->faces[f+s->faces_count+2] = idx[f-1] + s->dots_count/3;
	// improve previous break
	if(s->faces_count>3)
		s->faces[s->faces_count-1] = s->faces[s->faces_count];

	s->dots_count += d*3;
	s->faces_count += f+3;
	s->use_sub = EINA_TRUE;
}

void		EGE_Mesh_dot_sub(EGE_Mesh* s, const int id, const EGE_vector p) {
	int i;
	if (s->dots == NULL)
		s->dots = calloc(s->dots_max, sizeof(EGE_fp));
	for(i=0;i<3;i++)
		s->dots[3*id+s->dots_count+i] = p[i];
}

void		EGE_Mesh_col_sub(EGE_Mesh* s, const int id, const EGE_quater c) {
	int idx = 4*(id+s->dots_count/3);
	if (s->cols == NULL)
		s->cols = calloc(s->dots_max/3*4, sizeof(EGE_fp));
	s->cols[idx]   = c[0];
	s->cols[idx+1] = c[1];
	s->cols[idx+2] = c[2];
	s->cols[idx+3] = c[3];
	if (c[3]<fp_1)
		s->alpha	= EINA_TRUE;
}

void		EGE_Mesh_tex_sub(EGE_Mesh* s, const int id, const EGE_texC t) {
	int idx = 2*(id+s->dots_count/3);
	if (s->tex == NULL)
		s->tex = calloc(s->dots_max/3*2, sizeof(EGE_fp));
	s->tex[idx]   = t[0];
	s->tex[idx+1] = t[1];
}

void		EGE_Mesh_norm_sub(EGE_Mesh* s, const int id, const EGE_vector p) {
	if (s->norm == NULL)
		s->norm = calloc(s->dots_max, sizeof(EGE_fp));
	s->norm[3*id+s->dots_count+0]	= p[0];
	s->norm[3*id+s->dots_count+1]	= p[1];
	s->norm[3*id+s->dots_count+2]	= p[2];
}

void		_EGE_Mesh_add_rect_vtx(EGE_Mesh* s, EGE_vector vtl, EGE_vector vbr) {
	EGE_vector p0;
	int scale = 1;
	if (s->t->scale) scale = 2;

	p0[1] = vtl[1]*scale;	p0[2] = vtl[2];
	p0[0] = vtl[0]*scale;	EGE_Mesh_dot_sub(s, 0, p0);
	p0[0] = vbr[0]*scale;	EGE_Mesh_dot_sub(s, 1, p0);
	p0[1] = vbr[1]*scale;	EGE_Mesh_dot_sub(s, 2, p0);
	p0[0] = vtl[0]*scale;	EGE_Mesh_dot_sub(s, 3, p0);
}

void		EGE_Mesh_add_rect(EGE_Mesh* s, EGE_vector vtl, EGE_vector vbr) {
	_EGE_Mesh_add_rect_vtx(s,vtl,vbr);
	EGE_Mesh_finish_sub(s, 4, 6, squareIndex);
}

void		EGE_Mesh_add_texture_part(EGE_Mesh* m, EGE_vector vtl, EGE_texC ttl, EGE_texC tbr) {
	EGE_vector vbr = {fp_0, fp_0, fp_0};
	EGE_texC   t0;
	vbr[0] = vtl[0] + tbr[0] - ttl[0];
	vbr[1] = vtl[1] + tbr[1] - ttl[1];
	vbr[2] = vtl[2];
	_EGE_Mesh_add_rect_vtx(m,vtl,vbr);
	t0[0] = fp_div(ttl[0]+fp_from_float(0.1), fp_from_int(m->t->w));
	t0[1] = fp_div(ttl[1]+fp_from_float(0.1), fp_from_int(m->t->h));EGE_Mesh_tex_sub(m, 0, t0);
	t0[0] = fp_div(tbr[0]-fp_from_float(0.1), fp_from_int(m->t->w));EGE_Mesh_tex_sub(m, 1, t0);
	t0[1] = fp_div(tbr[1]-fp_from_float(0.1), fp_from_int(m->t->h));EGE_Mesh_tex_sub(m, 2, t0);
	t0[0] = fp_div(ttl[0]+fp_from_float(0.1), fp_from_int(m->t->w));EGE_Mesh_tex_sub(m, 3, t0);
	EGE_Mesh_finish_sub(m, 4, 6, squareIndex);
}

void		EGE_Mesh_add_tile(EGE_Mesh* m, EGE_vector p, int32_t id) {
	int maxts, ix, iy;
	EGE_texC   tl = {fp_0, fp_0};
	EGE_texC   br = {fp_0, fp_0};

	if (! m->t->tilesheet)	return;
	if (id == 0)		return;
	maxts	= m->t->orig_w/m->t->tile_w;
	iy	= (id-1)/maxts;
	ix	= (id-1) - iy*maxts;
	tl[0]	= fp_mul(fp_from_int(ix),   fp_from_int(m->t->tile_w));
	tl[1]	= fp_mul(fp_from_int(iy),   fp_from_int(m->t->tile_h));
	br[0]	= fp_mul(fp_from_int(ix+1), fp_from_int(m->t->tile_w));
	br[1]	= fp_mul(fp_from_int(iy+1), fp_from_int(m->t->tile_h));
	EGE_Mesh_add_texture_part(m, p, tl, br);
}

void		EGE_Mesh_add_tilemap_tile(EGE_Mesh* m, EGE_vector o, int32_t x, int32_t y, int32_t id) {
	int maxts, ix, iy;
	EGE_vector p  = {fp_0, fp_0, fp_0};
	EGE_texC   tl = {fp_0, fp_0};
	EGE_texC   br = {fp_0, fp_0};

	if (! m->t->tilesheet) 	return;
	if (id == 0)		return;
	maxts = m->t->orig_w/m->t->tile_w;
	iy = (id-1)/maxts;
	ix = (id-1) - iy*maxts;
	p[2] = o[2];
	p[0] = fp_mul(fp_from_int(x),    fp_from_int(m->t->tile_w)) + o[0];
	p[1] = fp_mul(fp_from_int(y),    fp_from_int(m->t->tile_h)) + o[1];
	tl[0]= fp_mul(fp_from_int(ix),   fp_from_int(m->t->tile_w));
	tl[1]= fp_mul(fp_from_int(iy),   fp_from_int(m->t->tile_h));
	br[0]= fp_mul(fp_from_int(ix+1), fp_from_int(m->t->tile_w));
	br[1]= fp_mul(fp_from_int(iy+1), fp_from_int(m->t->tile_h));
	EGE_Mesh_add_texture_part(m, p, tl, br);
}
