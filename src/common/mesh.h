/**
 *
 *  mesh.h
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
#ifndef __EGE_MESH_H__
#define __EGE_MESH_H__
#include "texture.h"
#include "fp.h"


typedef struct {
	const char*	name;
	GLenum		mode;
	Eina_Bool	alpha;
	Eina_Bool	use_sub;
	Eina_Bool	is_tile;
	Eina_Bool	loaded;
	uint16_t	tilemap_w;
	uint16_t	tilemap_h;
	

	// Materia data
	EGE_quater	col;	// physical color (used when not using "cols")
	EGE_quater	ambiant;// Ka
	EGE_quater	difuse;	// Kd
	EGE_quater	specular; // Ks
	EGE_quater	transfilter; // Tf
	uint16_t	illum;	// illumination mode

	// standard version (using fixed data)
	EGE_fp*		dots;
	int32_t		dots_count;
	uint32_t	dots_max;
	EGE_fp*		cols;
	int32_t		cols_count;
	EGE_fp*		tex;
	int32_t		tex_count;
	EGE_fp*		norm;
	int32_t		norm_count;

	// desktop version (using floats, created using the method finalize)
	float*		fdots;
	int32_t		fdots_count;
	float*		fcols;
	int32_t		fcols_count;
	float*		ftex;
	int32_t		ftex_count;
	float*		fnorm;
	int32_t		fnorm_count;

	unsigned short*	faces;
	int32_t		faces_count;
	uint32_t	faces_max;

	EGE_Texture*	t;
	const char*	texture_name;
} EGE_Mesh;

EGE_Mesh*	EGE_Mesh_new(const char *m_name, const uint32_t d, const uint32_t f, const GLenum m);
void		EGE_Mesh_free(EGE_Mesh* s);
void		EGE_Mesh_finalize(EGE_Mesh* s);
void		EGE_Mesh_finish_sub_mesh(EGE_Mesh* s, const uint16_t d);
void		EGE_Mesh_Texture_set(EGE_Mesh* s, EGE_Texture* t);
void		EGE_Mesh_uniq_col_set(EGE_Mesh* s, const EGE_quater c);

// sub-mesh management
void		EGE_Mesh_finish_sub(EGE_Mesh* s, const uint16_t d, const uint16_t f, const unsigned short* idx);
void		EGE_Mesh_col_sub(EGE_Mesh* s, const int id, const EGE_quater c);
void		EGE_Mesh_tex_sub(EGE_Mesh* s, const int id, const EGE_texC t);
void		EGE_Mesh_dot_sub(EGE_Mesh* s, const int id, const EGE_vector p);
void		EGE_Mesh_norm_sub(EGE_Mesh* s, const int id, const EGE_vector p);

// if no sub mesh are used (but remember to set the indexes using EGE_Mesh_finish_sub
void		EGE_Mesh_col_set(EGE_Mesh* s, const int id, const EGE_quater c);
void		EGE_Mesh_tex_set(EGE_Mesh* s, const int id, const EGE_texC t);
void		EGE_Mesh_dot_set(EGE_Mesh* s, const int id, const EGE_vector p);
void		EGE_Mesh_norm_set(EGE_Mesh* s, const int id, const EGE_vector p);

// Adding prebuild stuff
void		EGE_Mesh_add_tilemap_tile(EGE_Mesh* m, EGE_vector o, int32_t x, int32_t y, int32_t id);
void		EGE_Mesh_add_tile(EGE_Mesh* m, EGE_vector p, int32_t id);
void		EGE_Mesh_add_texture_part(EGE_Mesh* m, EGE_vector vtl, EGE_texC ttl, EGE_texC tbr);
void		EGE_Mesh_add_rect(EGE_Mesh* s, EGE_vector vtl, EGE_vector vbr);
#endif
