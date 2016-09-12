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
#ifndef __EGE_GAME_H__
#define __EGE_GAME_H__

#include <Eina.h>
#include "texture.h"
#include "mesh.h"
#include "sound.h"

typedef struct {
	unsigned int	engine_version;
	unsigned int	version;
  	const char*	name;
	Eina_List*	textures;
	Eina_List*	fonts;
	Eina_List*	meshs;
	Eina_List*	sounds;
	Eina_List*	musics;
} EGE_Games;

EGE_Games*	EGE_Game_new(const char *p_name);
void		EGE_Game_free(EGE_Games *g);

void		EGE_Game_add_Texture(EGE_Games *game, const EGE_Texture *texture);
EGE_Texture*	EGE_Game_find_Texture(const EGE_Games *game, const char *name);
void		EGE_Game_remove_Texture(EGE_Games *game, const char *name);

void		EGE_Game_add_Font(EGE_Games *game, const EGE_Font *font);
EGE_Font*	EGE_Game_find_Font(const EGE_Games *game, const char *name);
void		EGE_Game_remove_Font(EGE_Games *game, const char *name);

void		EGE_Game_add_Mesh(EGE_Games *game, const EGE_Mesh *m);
EGE_Mesh*	EGE_Game_find_Mesh(const EGE_Games *game, const char *name);
void		EGE_Game_remove_Mesh(EGE_Games *game, const char *name);

void		EGE_Game_add_Sound(EGE_Games *game, const EGE_Sound *s);
EGE_Sound*	EGE_Game_find_Sound(const EGE_Games *game, const char *name);
void		EGE_Game_remove_Sound(EGE_Games *game, const char *name);

void		EGE_Game_add_Music(EGE_Games *game, const EGE_Music *m);
EGE_Music*	EGE_Game_find_Music(const EGE_Games *game, const char *name);
void		EGE_Game_remove_Music(EGE_Games *game, const char *name);


extern EGE_Games *g;


#endif
