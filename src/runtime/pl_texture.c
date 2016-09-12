/**
 *
 *  pl_texture.c
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
#include "pl_texture.h"
#include "screen.h"
#include <stdint.h>

EGE_Texture *lastBindedTex = NULL;


#define B sp[i - b]
#define D sp[i - (i>0?1:0)]
#define F sp[i + (i<w?1:0)]
#define H sp[i + h0]
#define E  sp[i]
#define E0 tp[i*2] 
#define E1 tp[i*2 + 1]
#define E2 tp[i*2 + tpitch]
#define E3 tp[i*2 + 1 + tpitch]

static uint16_t* scale2x(uint16_t* src, const int w, const int h) {
	register int i, j;
	int b, h0;

	uint16_t* ret = calloc(2*w*2*h, sizeof(uint16_t));

	int tpitch = 2*w;
	int spitch = w;
	uint16_t* tp = ret;
	uint16_t* sp = src;
 
	for (j = 0; j < h; ++j) {
		b = j>0?spitch:0;
		h0 = j<h?spitch:0;
		for (i = 0; i < w; ++i) {
			if (B != H && D != F) {
				E0 = D == B ? D : E;
				E1 = B == F ? F : E;
				E2 = D == H ? D : E;
				E3 = H == F ? F : E;
			} else {
				E0 = E;
				E1 = E;
				E2 = E;
				E3 = E;
			}
		}
		tp += 2*tpitch;
		sp += spitch;
	}
	return ret;
}
static unsigned char* scale2xa(unsigned char* src, const int w, const int h) {
	register int i, j;
	int b, h0;

	unsigned char* ret = calloc(2*w*2*h, sizeof(unsigned char));

	int tpitch = 2*w;
	int spitch = w;
	unsigned char* tp = ret;
	unsigned char* sp = src;
 
	for (j = 0; j < h; ++j) {
		b = j>0?spitch:0;
		h0 = j<h?spitch:0;
		for (i = 0; i < w; ++i) {
			if (B != H && D != F) {
				E0 = D == B ? D : E;
				E1 = B == F ? F : E;
				E2 = D == H ? D : E;
				E3 = H == F ? F : E;
			} else {
				E0 = E;
				E1 = E;
				E2 = E;
				E3 = E;
			}
		}
		tp += 2*tpitch;
		sp += spitch;
	}
	return ret;
}

#undef B
#undef D
#undef F
#undef H
#undef E
#undef E0
#undef E1
#undef E2
#undef E3


void		EGE_Texture_bind(EGE_Texture *t) {
	if (t!=lastBindedTex) {
		glBindTexture(GL_TEXTURE_2D, t->id);
		//printf("binding %s\n", t->name);
		lastBindedTex = t;
	}
}

EGE_Texture*    EGE_Texture_newAlpha(const char *p_name, const unsigned int w, const unsigned int h, unsigned char *bmp) {
	GLuint i;
	EGE_Texture* ret = EGE_Texture_new(p_name, w, h);
	GL_CHECK(glGenTextures(1, &i));
	ret->id		= i;
	ret->format	= EGE_TEXTURE_FORMAT_ALPHA;
	EGE_Texture_bind(ret);
	GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512,512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, bmp));
	// can free temp_bitmap at this point
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	EGE_Game_add_Texture(g, ret);
	return ret;
}

void		EGE_Texture_load(EGE_Texture *t) {
	GLuint i;
	int scale = 1;
	if (t==NULL) return;
	void *buf = t->data;
	
	if (EGE_viewport.scaleFactor==2 || t->scale) {
		scale = 2;
		if (t->format!=EGE_TEXTURE_FORMAT_ALPHA)
			buf = scale2x(t->data, t->w, t->h);
		else
			buf = scale2xa((unsigned char *)t->data, t->w, t->h);
	}
	glGenTextures(1, &i);
	t->id = i;
	EGE_Texture_bind(t);
	if (t->format == EGE_TEXTURE_FORMAT_656) {
		GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t->w*scale, t->h*scale, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, buf));
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	} else if (t->format == EGE_TEXTURE_FORMAT_4444) {
		GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t->w*scale, t->h*scale, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, buf));
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	} else {
		GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, t->w*scale, t->h*scale, 0, GL_ALPHA, GL_UNSIGNED_BYTE, buf));
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	}
	if (EGE_viewport.scaleFactor==2 || t->scale)
		free(buf);
}

void		EGE_Texture_unload(EGE_Texture *t) {
	GL_CHECK(glDeleteTextures(1, &(t->id)));
}

void		EGE_Texture_Player_init(EGE_Games *g) {
	Eina_List*	l;
	EGE_Texture*	t;
	EGE_Font*	f;
	EINA_LIST_FOREACH(g->textures, l, t) {
		EGE_Texture_load(t);
	}
	EINA_LIST_FOREACH(g->fonts, l, f) {
		f->texture  = EGE_Game_find_Texture(g, f->tex_name);
		f->texture2 = EGE_Game_find_Texture(g, f->tex_name2);
	}
}

void		EGE_Texture_Player_shutdown(EGE_Games *g) {
	Eina_List*	l;
	EGE_Texture*	t;
	EINA_LIST_FOREACH(g->textures, l, t) {
		EGE_Texture_unload(t);
	}
}
