/**
 *
 *  texture.h
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
#ifndef __EGE_TEXTURE_H__
#define __EGE_TEXTURE_H__

#include <Eina.h>
#include <stdint.h>
#include "stb_truetype.h"
#include "fp.h"
#ifdef HAVE_GLES
#include <GLES/gl.h>
#else
#include <GL/gl.h>
#endif

#define EGE_TEXTURE_FORMAT_656   0
#define EGE_TEXTURE_FORMAT_4444  1
#define EGE_TEXTURE_FORMAT_ALPHA 2

typedef struct EGE_sprite_coord_ EGE_sprite_coord;
typedef struct EGE_Texture_ EGE_Texture;


struct EGE_sprite_coord_ {
	const char*	name;
	EGE_fp		tex[8]; // 4 dots, 2 coords/dot
	float		ftex[8];
	uint16_t	sw, sh;
};

struct EGE_Texture_ {
	const char*	name;
 	uint16_t	w, h;
 	uint16_t	orig_w, orig_h;
	uint16_t	tile_w, tile_h;
	uint16_t*	data;
	Eina_List*	sprites;
	GLuint		id;
	uint8_t		format;
	Eina_Bool	spritesheet;
	Eina_Bool	tilesheet;
	Eina_Bool	scale;
};

void		EGE_Texture_free(EGE_Texture *t);
EGE_Texture*	EGE_Texture_new(const char *p_name, const unsigned int w, const unsigned int h);
EGE_Texture*	EGE_Texture_load_from(const char *p_name, const char *p_filename);
void		EGE_Texture_add_sprite(EGE_Texture* t, const char *s_name, EGE_texC tl, EGE_texC br);
EGE_sprite_coord* EGE_Texture_get_sprite(EGE_Texture* t, const char *s_name);

typedef struct {
	const char*	name;
	char*		tex_name;
	EGE_Texture *	texture;
	stbtt_bakedchar	cdata[96];
	char*		tex_name2;
	EGE_Texture *	texture2;
	stbtt_bakedchar	cdata2[96];
} EGE_Font;

EGE_Font*	EGE_Font_new(const char *ttfFile, const unsigned int size, const char *name);
void		EGE_Font_free(EGE_Font* f);



#endif
