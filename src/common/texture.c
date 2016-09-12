/**
 *
 *  texture.c
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
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <Eina.h>
#include <Eet.h>
#include <stdlib.h>

#include "texture.h"
#include "storage.h"

EGE_Texture*    EGE_Texture_new(const char *p_name, const unsigned int w, const unsigned int h) {
	EGE_Texture* ret = calloc(1, sizeof(EGE_Texture));

	ret->name	= eina_stringshare_add(p_name);
	ret->w		= w;
	ret->h		= h;
	ret->orig_w	= 0;
	ret->orig_h	= 0;
	ret->data	= calloc(w*h, sizeof(uint16_t));
	ret->format	= EGE_TEXTURE_FORMAT_4444;
	ret->spritesheet= EINA_FALSE;
	ret->tilesheet	= EINA_FALSE;
	ret->scale	= EINA_FALSE;
	ret->tile_w	= 0;
	ret->tile_h	= 0;
	ret->sprites	= NULL;

	memset(ret->data, 0, w*h*sizeof(uint16_t));

	return ret;
}

void		EGE_Texture_free(EGE_Texture *t) {
	EGE_sprite_coord* sc;
	if (!t)		return;
	if (t->data)	free(t->data);
	EINA_LIST_FREE(t->sprites, sc) {
		EGE_String_Free(sc->name);
		free(sc);
	}
	EGE_String_Free(t->name);
	free(t);
}
void		EGE_Texture_add_sprite(EGE_Texture* t, const char *s_name, EGE_texC tl, EGE_texC br) {
	int i;
	EGE_sprite_coord* sc = calloc(1, sizeof(EGE_sprite_coord));
	sc->name	= eina_stringshare_add(s_name);
	sc->sw		= fp_to_int(br[0])-fp_to_int(tl[0]);
	sc->sh		= fp_to_int(br[1])-fp_to_int(tl[1]);
	sc->tex[0]	= fp_div(tl[0]+fp_from_float(0.1), fp_from_int(t->w));
	sc->tex[1]	= fp_div(tl[1]+fp_from_float(0.1), fp_from_int(t->h));
	sc->tex[2]	= fp_div(br[0]-fp_from_float(0.1), fp_from_int(t->w));
	sc->tex[3]	= sc->tex[1];
	sc->tex[4]	= sc->tex[2];
	sc->tex[5]	= fp_div(br[1]-fp_from_float(0.1), fp_from_int(t->h));
	sc->tex[6]	= fp_div(tl[0]+fp_from_float(0.1), fp_from_int(t->w));
	sc->tex[7]	= sc->tex[5];
	for(i=0;i<8;i++)
		sc->ftex[i] = fp_to_float(sc->tex[i]);
	t->sprites	= eina_list_append(t->sprites, sc);
}

EGE_sprite_coord* EGE_Texture_get_sprite(EGE_Texture* t, const char *s_name) {
	Eina_List*	l;
	EGE_sprite_coord* sc;

	EINA_LIST_FOREACH(t->sprites, l, sc) {
		if (!strcmp(sc->name, s_name))
			return sc;
	}
	return NULL;
}

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

EGE_Font*	EGE_Font_new(const char *ttfFile, const unsigned int size, const char *name) {
	unsigned char ttf_buffer[1<<20];

	EGE_Font* ret	= calloc(1, sizeof(EGE_Font));
	FILE* f		= fopen(ttfFile, "rb");

	fread(ttf_buffer, 1, 1<<20, f);
	fclose(f);

	ret->name	= name;

	ret->tex_name	= calloc(strlen(name)+5,sizeof(char));
	sprintf(ret->tex_name, "fnt-%s", name);
	ret->texture	= EGE_Texture_new(ret->tex_name, 512, 512);
	if (ret->texture != NULL) {
		ret->texture->format	= EGE_TEXTURE_FORMAT_ALPHA;
		free(ret->texture->data);
		ret->texture->data = calloc(512*512, sizeof(unsigned char));
		stbtt_BakeFontBitmap(ttf_buffer,0, (float)size,   (unsigned char *)ret->texture->data, 512,512, 32,96, ret->cdata); // no guarantee this fits!
	}

	ret->tex_name2	= calloc(strlen(name)+7,sizeof(char));
	sprintf(ret->tex_name2, "fnt-%s-2", name);
	ret->texture2	= EGE_Texture_new(ret->tex_name2, 512, 512);
	if (ret->texture2 != NULL) {
		ret->texture2->format	= EGE_TEXTURE_FORMAT_ALPHA;
		free(ret->texture2->data);
		ret->texture2->data = calloc(512*512, sizeof(unsigned char));
		stbtt_BakeFontBitmap(ttf_buffer,0, (float)size*2, (unsigned char *)ret->texture2->data,512,512, 32,96, ret->cdata2); // no guarantee this fits!
	}
	return ret;
}

void		EGE_Font_free(EGE_Font* f) {
	free(f);
}

static __inline__ int p2(int o) {	// build next power of 2 number
	int v = 1;
	while (v < o)
		v <<= 1;
	return v;
}

EGE_Texture*	EGE_Texture_load_from(const char *p_name, const char *p_filename) {
	SDL_Surface *dest	= NULL;
	SDL_Surface *sur	= IMG_Load(p_filename);
	EGE_Texture *ret	= NULL;
	uint16_t *a;
	int i;
	if (!sur) {
		fprintf(stderr, "Could not load image '%s'.\n", p_filename);
		return NULL;
	}
	int w = p2(sur->w);
	int h = p2(sur->h);
	ret = EGE_Texture_new(p_name, w, h);
	if (!ret) {
		fprintf(stderr, "Could not create texture for '%s'.\n", p_filename);
		SDL_FreeSurface(sur);
		return NULL;
	}
	if (sur->format->Amask>0) {
		// use 4444 texture format
		dest = SDL_CreateRGBSurface(0, w, h, 16, 0xf000, 0x0f00, 0x00f0, 0x000f);
		a = (uint16_t *)dest->pixels;
		for (i=0;i<w*h;i++)
			a[i]=0x555f;
		ret->format = EGE_TEXTURE_FORMAT_4444;
	} else {
		// use 565 texture format
		dest = SDL_CreateRGBSurface(0, w, h, 16, 0x0000f800, 0x000007e0, 0x0000001f, 0x00000000);
		ret->format = EGE_TEXTURE_FORMAT_656;
	}
	SDL_BlitSurface(sur, 0, dest, 0);
	if (ret->format == EGE_TEXTURE_FORMAT_4444) {
		a = (uint16_t *)dest->pixels;
		for (i=0;i<w*h;i++)
			if (a[i]==0x555f)
 				a[i]=0;
	}

	ret->orig_w	= sur->w;
	ret->orig_h	= sur->h;
	SDL_FreeSurface(sur);
	memcpy(ret->data, dest->pixels, w*h*sizeof(uint16_t));
	SDL_FreeSurface(dest);
	return ret;
}

Eina_Bool	EGE_Texture_save_to(const char *p_name, const char *p_filename) {
	return EINA_FALSE;
}

