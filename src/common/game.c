/**
 *
 *  game.c
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
#include <Eina.h>
#include <Eet.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "storage.h"
#include "texture.h"

EGE_Games *g = NULL;


EGE_Games*	EGE_Game_new(const char *p_name) {
	EGE_Games * ret = calloc(1, sizeof(EGE_Games));

	ret->version	= 1;
	ret->meshs	= NULL;
	ret->sounds	= NULL;
	ret->musics	= NULL;
	ret->name	= eina_stringshare_add(p_name);

	return ret;
}

void		EGE_Game_free(EGE_Games *g) {
	EGE_String_Free(g->name);

	EGE_Texture *t;
	EGE_Font  *f;
	EGE_Mesh  *m;
	EGE_Music *u;
	EGE_Sound *s;
	EINA_LIST_FREE(g->textures, t) {
		EGE_Texture_free(t);
	}
	EINA_LIST_FREE(g->fonts, f) {
		EGE_Font_free(f);
	}
	EINA_LIST_FREE(g->meshs, m) {
		EGE_Mesh_free(m);
	}
	EINA_LIST_FREE(g->sounds, s) {
		EGE_Sound_free(s);
	}
	EINA_LIST_FREE(g->musics, u) {
		EGE_Music_free(u);
	}
	free(g);
}

void		EGE_Game_add_Texture(EGE_Games *game, const EGE_Texture *texture) {
	if(texture!=NULL) game->textures = eina_list_append(game->textures, texture);
}

EGE_Texture*	EGE_Game_find_Texture(const EGE_Games *game, const char *name) {
	Eina_List*	l;
	EGE_Texture*	t;

	EINA_LIST_FOREACH(game->textures, l, t) {
		if (!strcmp(t->name, name))
			return t;
	}
	printf("Texture not found : %s\n", name);
	return NULL;
}

void		EGE_Game_remove_Texture(EGE_Games *game, const char *name) {
	EGE_Texture*	t = EGE_Game_find_Texture(game, name);
	game->textures = eina_list_remove(game->textures, t);
	EGE_Texture_free(t);
}



void		EGE_Game_add_Font(EGE_Games *game, const EGE_Font *font) {
	if(font!=NULL) game->fonts = eina_list_append(game->fonts, font);
}

EGE_Font*	EGE_Game_find_Font(const EGE_Games *game, const char *name) {
	Eina_List*	l;
	EGE_Font*	f;

	EINA_LIST_FOREACH(game->fonts, l, f) {
		if (!strcmp(f->name, name))
			return f;
	}
	return NULL;
}

void		EGE_Game_remove_Font(EGE_Games *game, const char *name) {
	EGE_Font*	t = EGE_Game_find_Font(game, name);
	game->fonts = eina_list_remove(game->fonts, t);
	EGE_Font_free(t);
}



void		EGE_Game_add_Mesh(EGE_Games *game, const EGE_Mesh *m) {
	if(m!=NULL) game->meshs = eina_list_append(game->meshs, m);
}

EGE_Mesh*	EGE_Game_find_Mesh(const EGE_Games *game, const char *name) {
	Eina_List*	l;
	EGE_Mesh*	m;

	EINA_LIST_FOREACH(game->meshs, l, m) {
		if (!strcmp(m->name, name))
			return m;
	}
	return NULL;
}

void		EGE_Game_remove_Mesh(EGE_Games *game, const char *name) {
	EGE_Mesh*	t = EGE_Game_find_Mesh(game, name);
	game->meshs = eina_list_remove(game->meshs, t);
	EGE_Mesh_free(t);
}

void		EGE_Game_add_Sound(EGE_Games *game, const EGE_Sound *s) {
	if(s!=NULL) game->sounds = eina_list_append(game->sounds, s);
}

EGE_Sound*	EGE_Game_find_Sound(const EGE_Games *game, const char *name) {
	Eina_List*	l;
	EGE_Sound*	s;

	EINA_LIST_FOREACH(game->sounds, l, s) {
		if (!strcmp(s->name, name))
			return s;
	}
	return NULL;
}

void		EGE_Game_remove_Sound(EGE_Games *game, const char *name) {
	EGE_Sound*	t = EGE_Game_find_Sound(game, name);
	game->sounds = eina_list_remove(game->sounds, t);
	EGE_Sound_free(t);
}

void		EGE_Game_add_Music(EGE_Games *game, const EGE_Music *m) {
	if(m!=NULL) game->musics = eina_list_append(game->musics, m);
}

EGE_Music*	EGE_Game_find_Music(const EGE_Games *game, const char *name) {
	Eina_List*	l;
	EGE_Music*	m;

	EINA_LIST_FOREACH(game->musics, l, m) {
		if (!strcmp(m->name, name))
			return m;
	}
	return NULL;
}

void		EGE_Game_remove_Music(EGE_Games *game, const char *name) {
	EGE_Music*	t = EGE_Game_find_Music(game, name);
	game->musics = eina_list_remove(game->musics, t);
	EGE_Music_free(t);
}
