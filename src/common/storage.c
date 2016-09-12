/**
 *
 *  storage.c
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
#include <stdio.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "game.h"
#include "texture.h"
#include "mesh.h"
#include "storage.h"

#define EGE_STORAGE_USE_IMAGE_DATA

static Eet_Data_Descriptor *	_EGE_Sound_Descriptor;
static Eet_Data_Descriptor *	_EGE_Music_Descriptor;
static Eet_Data_Descriptor *	_EGE_Texture_Descriptor;
static Eet_Data_Descriptor *	_EGE_Sprite_Descriptor;
static Eet_Data_Descriptor *	_EGE_Font_stbtt_Descriptor;
static Eet_Data_Descriptor *	_EGE_Font_Descriptor;
static Eet_Data_Descriptor *	_EGE_Mesh_Descriptor;
static Eet_Data_Descriptor *	_EGE_Game_Descriptor;
static const char		_EGE_Game_Store[] = "default";

static Eet_Dictionary *		_EGE_Dict = NULL;

void		EGE_String_Free(const char *str) {
	if (!str)
		return;

	if ( _EGE_Dict && eet_dictionary_string_check(_EGE_Dict, str))
		return;

	eina_stringshare_del(str);
}

EGE_Games*	EGE_Game_load(const char *filename) {
#ifdef EGE_STORAGE_USE_IMAGE_DATA
	EGE_Mesh*	m;
#endif
	EGE_Sound*	s;
	EGE_Music*	u;
	Eina_List*	l;
	EGE_Texture*	t;
	EGE_Games	*ret = NULL;//calloc(1, sizeof(EGE_Games));
	Eet_File	*ef  = eet_open(filename, EET_FILE_MODE_READ);
	unsigned int	w,h;
	int		a,c,q,lo;
	if (!ef) {
		fprintf(stderr, "ERROR: could not open '%s' for read\n", filename);
		return NULL;
	}

	ret	= eet_data_read(ef, _EGE_Game_Descriptor, _EGE_Game_Store);

	if (ret) {
		if (!_EGE_Dict)
			_EGE_Dict = eet_dictionary_get(ef);
		/* Once the datatype evolve
		if (ret->version < 2) {
			// do something to update the data ;)
		}
		*/

		EINA_LIST_FOREACH(ret->textures,l,t) {
			t->data = eet_data_image_read(ef, t->name, &w, &h, &a, &c, &q, &lo);
		}

#ifdef EGE_STORAGE_USE_IMAGE_DATA
		EINA_LIST_FOREACH(ret->meshs,l,m) {
			int i=m->dots_count;
			int j=m->dots_count;
			m->dots = (EGE_fp*)eet_read(ef, m->name, &c);
			m->loaded = EINA_TRUE;
			if (m->dots == NULL) {
				printf("Load mesh %s FAILED\n", m->name);
				break;
			}
			if (m->cols_count>0) {
				m->cols 	= m->dots + i;
				i+=m->cols_count;
			}
			if (m->tex_count>0) {
				m->tex		= m->dots + i;
				i+=m->tex_count;
			}
			if (m->norm_count>0) {
				m->norm		= m->dots + i;
				i+=m->norm_count;
			}
			m->fdots		= (float*) (m->dots + i);
			if (m->cols_count>0) {
				m->fcols	= (float*) (m->dots + i + j);
				j+=m->cols_count;
			}
			if (m->tex_count>0) {
				m->ftex		= (float*) (m->dots + i + j);
				j+=m->tex_count;
			}
			if (m->norm_count>0) {
				m->fnorm	= (float*) (m->dots + i + j);
				j+=m->norm_count;
			}
			m->faces		= (unsigned short*) (m->dots + i + j);
		}
#endif
		EINA_LIST_FOREACH(ret->sounds,l,s) {
			s->data = (EGE_fp*)eet_read(ef, s->name, &c);
			EGE_Sound_init(s);
		}

		EINA_LIST_FOREACH(ret->musics,l,u) {
			u->data = (EGE_fp*)eet_read(ef, u->name, &c);
			EGE_Music_init(u);
		}

	}

	eet_close(ef);
	return ret;
}

Eina_Bool	EGE_Game_save(const EGE_Games *game, const char *filename) {
	char		tmp[PATH_MAX];
	Eet_File *	ef;
	Eina_Bool	ret;
	unsigned int	i, len;
	struct stat	st;
	EGE_Sound*	s;
	EGE_Music*	u;
	Eina_List*	l;
	EGE_Texture*	t;
#ifdef EGE_STORAGE_USE_IMAGE_DATA
	EGE_Mesh*	m;
#endif

	len = eina_strlcpy(tmp, filename, sizeof(tmp));
	if (len + 12 >= (int)sizeof(tmp)) {
		fprintf(stderr, "ERROR: file name is too big: %s\n", filename);
		return EINA_FALSE;
	}

	// looking for a temporary filename
	i = 0;
	do {
		snprintf(tmp + len, 12, ".%u", i);
		i++;
	} while (stat(tmp, &st) == 0);

	ef = eet_open(tmp, EET_FILE_MODE_WRITE);
	if (!ef) {
		fprintf(stderr, "ERROR: could not open '%s' for write\n", tmp);
		return EINA_FALSE;
	}

	ret = eet_data_write(ef, _EGE_Game_Descriptor, _EGE_Game_Store, game, EET_COMPRESSION_VERYFAST);
	if (ret==0) {
		eet_close(ef);unlink(tmp);
		fprintf(stderr, "ERROR: could not write main data\n");
		return EINA_FALSE;
	}

	EINA_LIST_FOREACH(game->textures,l,t) {
 		//printf("Saving texture %s\n", t->name);
		if (t->format == EGE_TEXTURE_FORMAT_ALPHA)
			ret = eet_data_image_write (ef, t->name, t->data, t->w*sizeof(unsigned char)/sizeof(uint32_t), t->h, 1, 3, 100, 0); //eet expect 32b data but we have something smaller
		else
			ret = eet_data_image_write (ef, t->name, t->data, t->w/2, t->h, 1, 3, 100, 0); //eet expect 32b data but we have 16b hence the "w/2"
		if (!ret) {
			eet_close(ef);unlink(tmp);
			fprintf(stderr, "ERROR: could not write texture data : %s\n", t->name);
			return EINA_FALSE;
		}
	}
#ifdef EGE_STORAGE_USE_IMAGE_DATA
	EINA_LIST_FOREACH(game->meshs,l,m) {
 		int x		 = 3;
		int y;
		int i		 = m->dots_count;
		int j		 = m->dots_count;
		EGE_fp *mem	 = NULL;
		if (m->cols!=NULL)	x+=4;
		if (m->tex!=NULL)	x+=2;
		if (m->norm!=NULL)	x+=3;
		x		*= 2;
		y		 = m->dots_count/3 + m->faces_count/(2*x)+1;
		mem		 = calloc(x*y,sizeof(EGE_fp));
		
		
		memcpy(mem, m->dots, m->dots_count*sizeof(EGE_fp));
		if (m->cols_count>0) {
			memcpy(mem+i, m->cols, m->cols_count*sizeof(EGE_fp));
			i+=m->cols_count;
		}
		if (m->tex_count>0) {
			memcpy(mem+i, m->tex, m->tex_count*sizeof(EGE_fp));
			i+=m->tex_count;
		}
		if (m->norm_count>0) {
			memcpy(mem+i, m->norm, m->norm_count*sizeof(EGE_fp));
			i+=m->norm_count;
		}
		if (m->fdots_count>0) {
			memcpy(mem+i, m->fdots, m->dots_count*sizeof(float));
			if (m->cols_count>0) {
				memcpy(mem+i+j, m->fcols, m->cols_count*sizeof(float));
				j+=m->cols_count;
			}
			if (m->tex_count>0) {
				memcpy(mem+i+j, m->ftex, m->tex_count*sizeof(float));
				j+=m->tex_count;
			}
			if (m->norm_count>0) {
				memcpy(mem+i+j, m->fnorm, m->norm_count*sizeof(float));
				j+=m->norm_count;
			}
			memcpy(mem+i+j, m->faces, m->faces_count*sizeof(unsigned short));
		}
		ret = eet_write(ef, m->name, mem, x*y*sizeof(EGE_fp), 1);
		free(mem);
		if (!ret) {
			eet_close(ef);unlink(tmp);
			fprintf(stderr, "ERROR: could not write mesh data : %s\n", m->name);
			return EINA_FALSE;
		}
	}
#endif

	EINA_LIST_FOREACH(game->sounds,l,s) {
		ret = eet_write(ef, s->name, s->data, s->data_count*sizeof(char), 1);
	}

	EINA_LIST_FOREACH(game->musics,l,u) {
		ret = eet_write(ef, u->name, u->data, u->data_count*sizeof(char), 1);
	}

	eet_close(ef);
	if (ret) {
		// Save succeded, moving to the real file
		unlink(filename);
		rename(tmp, filename);
	} else {
		// Save failed, removing garbage file
		unlink(tmp);
	}
	return ret;
}


void		EGE_Game_Descriptor_Shutdown(void) {
	eet_data_descriptor_free(_EGE_Mesh_Descriptor);
	eet_data_descriptor_free(_EGE_Font_stbtt_Descriptor);
	eet_data_descriptor_free(_EGE_Font_Descriptor);
	eet_data_descriptor_free(_EGE_Sprite_Descriptor);
	eet_data_descriptor_free(_EGE_Texture_Descriptor);
	eet_data_descriptor_free(_EGE_Game_Descriptor);

	eet_shutdown();
	eina_shutdown();
}

#define EGE_GAME_ADD_BASIC(member, eet_type)		EET_DATA_DESCRIPTOR_ADD_BASIC(_EGE_Game_Descriptor, EGE_Games, # member, member, eet_type)
#define EGE_GAME_ADD_LIST(member, eet_desc)		EET_DATA_DESCRIPTOR_ADD_LIST(_EGE_Game_Descriptor, EGE_Games, # member, member, eet_desc)
#define TEXTURE_ADD_BASIC(member, eet_type)		EET_DATA_DESCRIPTOR_ADD_BASIC(_EGE_Texture_Descriptor, EGE_Texture, # member, member, eet_type)
#define TEXTURE_ADD_LIST(member, eet_desc)		EET_DATA_DESCRIPTOR_ADD_LIST(_EGE_Texture_Descriptor, EGE_Texture, # member, member, eet_desc)
#define SPRITE_ADD_BASIC(member, eet_type)		EET_DATA_DESCRIPTOR_ADD_BASIC(_EGE_Sprite_Descriptor, EGE_sprite_coord, # member, member, eet_type)
#define SPRITE_ADD_ARRAY(member, eet_type)		EET_DATA_DESCRIPTOR_ADD_BASIC_ARRAY(_EGE_Sprite_Descriptor, EGE_sprite_coord, # member, member, eet_type)
#define STBTT_ADD_BASIC(member, eet_type)		EET_DATA_DESCRIPTOR_ADD_BASIC(_EGE_Font_stbtt_Descriptor, stbtt_bakedchar, # member, member, eet_type)
#define FONT_ADD_BASIC(member, eet_type)		EET_DATA_DESCRIPTOR_ADD_BASIC(_EGE_Font_Descriptor, EGE_Font, # member, member, eet_type)
#define FONT_ADD_ARRAY(member, eet_desc)		EET_DATA_DESCRIPTOR_ADD_ARRAY(_EGE_Font_Descriptor, EGE_Font, # member, member, eet_desc)
#define MESH_ADD_BASIC(member, eet_type)		EET_DATA_DESCRIPTOR_ADD_BASIC(_EGE_Mesh_Descriptor, EGE_Mesh, # member, member, eet_type)
#define MESH_ADD_BASIC_VAR_ARRAY(member, eet_type)	EET_DATA_DESCRIPTOR_ADD_BASIC_VAR_ARRAY(_EGE_Mesh_Descriptor, EGE_Mesh, # member, member, eet_type)
#define MESH_ADD_ARRAY(member, eet_type)		EET_DATA_DESCRIPTOR_ADD_BASIC_ARRAY(_EGE_Mesh_Descriptor, EGE_Mesh, # member, member, eet_type)

#define SOUND_ADD_BASIC(member, eet_type)		EET_DATA_DESCRIPTOR_ADD_BASIC(_EGE_Sound_Descriptor, EGE_Sound, # member, member, eet_type)
#define MUSIC_ADD_BASIC(member, eet_type)		EET_DATA_DESCRIPTOR_ADD_BASIC(_EGE_Music_Descriptor, EGE_Music, # member, member, eet_type)

void		EGE_Game_Descriptor_Init(void) {
	Eet_Data_Descriptor_Class eddc;

	eina_init();
	eet_init();
	EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, EGE_Games);
	_EGE_Game_Descriptor = eet_data_descriptor_stream_new(&eddc);

	// Sprite
	EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, EGE_sprite_coord);
	_EGE_Sprite_Descriptor = eet_data_descriptor_stream_new(&eddc);

	SPRITE_ADD_BASIC(name,		EET_T_STRING);
	SPRITE_ADD_BASIC(sw,		EET_T_USHORT);
 	SPRITE_ADD_BASIC(sh,		EET_T_USHORT);
	SPRITE_ADD_ARRAY(tex,		EET_T_F16P16);
	SPRITE_ADD_ARRAY(ftex,		EET_T_FLOAT);

	// Texture
	EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, EGE_Texture);
	_EGE_Texture_Descriptor = eet_data_descriptor_stream_new(&eddc);

	TEXTURE_ADD_BASIC(name,		EET_T_STRING);
	TEXTURE_ADD_BASIC(w,		EET_T_USHORT);
	TEXTURE_ADD_BASIC(h,		EET_T_USHORT);
	TEXTURE_ADD_BASIC(orig_w,	EET_T_USHORT);
	TEXTURE_ADD_BASIC(orig_h,	EET_T_USHORT);
	TEXTURE_ADD_BASIC(format,	EET_T_UCHAR);
	TEXTURE_ADD_BASIC(scale,	EET_T_CHAR);
	TEXTURE_ADD_BASIC(spritesheet,	EET_T_CHAR);
	TEXTURE_ADD_BASIC(tilesheet,	EET_T_CHAR);
	TEXTURE_ADD_BASIC(tile_w,	EET_T_USHORT);
 	TEXTURE_ADD_BASIC(tile_h,	EET_T_USHORT);
	TEXTURE_ADD_LIST(sprites,	_EGE_Sprite_Descriptor);


	// Font
	EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, EGE_Font);
	_EGE_Font_Descriptor = eet_data_descriptor_stream_new(&eddc);
	EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, stbtt_bakedchar);
	_EGE_Font_stbtt_Descriptor = eet_data_descriptor_stream_new(&eddc);
	STBTT_ADD_BASIC(x0,		EET_T_UINT);
	STBTT_ADD_BASIC(y0,		EET_T_UINT);
	STBTT_ADD_BASIC(x1,		EET_T_UINT);
	STBTT_ADD_BASIC(y1,		EET_T_UINT);
	STBTT_ADD_BASIC(xoff,		EET_T_FLOAT);
	STBTT_ADD_BASIC(yoff,		EET_T_FLOAT);
	STBTT_ADD_BASIC(xadvance,	EET_T_FLOAT);
	FONT_ADD_BASIC(name,		EET_T_STRING);
	FONT_ADD_BASIC(tex_name,	EET_T_STRING);
	FONT_ADD_ARRAY(cdata,		_EGE_Font_stbtt_Descriptor);
	FONT_ADD_BASIC(tex_name2,	EET_T_STRING);
	FONT_ADD_ARRAY(cdata2,		_EGE_Font_stbtt_Descriptor);

	// Sound
	EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, EGE_Sound);
	_EGE_Sound_Descriptor = eet_data_descriptor_stream_new(&eddc);
	SOUND_ADD_BASIC(name,		EET_T_STRING);
	SOUND_ADD_BASIC(data_count,	EET_T_UINT);

	// Music
	EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, EGE_Music);
	_EGE_Music_Descriptor = eet_data_descriptor_stream_new(&eddc);
	MUSIC_ADD_BASIC(name,		EET_T_STRING);
	MUSIC_ADD_BASIC(data_count,	EET_T_UINT);
	MUSIC_ADD_BASIC(start_at,	EET_T_UINT);

	// Mesh
	EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, EGE_Mesh);
	_EGE_Mesh_Descriptor = eet_data_descriptor_stream_new(&eddc);
	MESH_ADD_BASIC(name,		EET_T_STRING);
	MESH_ADD_BASIC(texture_name,	EET_T_STRING);
	MESH_ADD_BASIC(mode,		EET_T_UCHAR);
	MESH_ADD_BASIC(alpha,		EET_T_UCHAR);
	MESH_ADD_BASIC(is_tile,		EET_T_UCHAR);
	MESH_ADD_BASIC(tilemap_w,	EET_T_USHORT);
	MESH_ADD_BASIC(tilemap_h,	EET_T_USHORT);
	MESH_ADD_ARRAY(col,		EET_T_F16P16);
	MESH_ADD_ARRAY(ambiant,		EET_T_F16P16);
	MESH_ADD_ARRAY(difuse,		EET_T_F16P16);
	MESH_ADD_ARRAY(specular,	EET_T_F16P16);
	MESH_ADD_ARRAY(transfilter,	EET_T_F16P16);
	MESH_ADD_BASIC(illum,		EET_T_USHORT);
	MESH_ADD_BASIC(dots_count,	EET_T_INT);
	MESH_ADD_BASIC(cols_count,	EET_T_INT);
	MESH_ADD_BASIC(tex_count,	EET_T_INT);
	MESH_ADD_BASIC(norm_count,	EET_T_INT);
	MESH_ADD_BASIC(fdots_count,	EET_T_INT);
	MESH_ADD_BASIC(fcols_count,	EET_T_INT);
	MESH_ADD_BASIC(ftex_count,	EET_T_INT);
	MESH_ADD_BASIC(fnorm_count,	EET_T_INT);
	MESH_ADD_BASIC(faces_count,	EET_T_INT);
#ifndef EGE_STORAGE_USE_IMAGE_DATA
	MESH_ADD_BASIC_VAR_ARRAY(dots,	EET_T_F16P16);
	MESH_ADD_BASIC_VAR_ARRAY(cols,	EET_T_F16P16);
	MESH_ADD_BASIC_VAR_ARRAY(tex,	EET_T_F16P16);
	MESH_ADD_BASIC_VAR_ARRAY(norm,	EET_T_F16P16);
	MESH_ADD_BASIC_VAR_ARRAY(fdots,	EET_T_FLOAT);
	MESH_ADD_BASIC_VAR_ARRAY(fcols,	EET_T_FLOAT);
	MESH_ADD_BASIC_VAR_ARRAY(ftex,	EET_T_FLOAT);
	MESH_ADD_BASIC_VAR_ARRAY(fnorm,	EET_T_FLOAT);
	MESH_ADD_BASIC_VAR_ARRAY(faces,	EET_T_USHORT);
#endif
	// Game
	EGE_GAME_ADD_BASIC(version,	EET_T_UINT);
	EGE_GAME_ADD_BASIC(name,	EET_T_STRING);
	EGE_GAME_ADD_LIST(textures,	_EGE_Texture_Descriptor);
	EGE_GAME_ADD_LIST(fonts,	_EGE_Font_Descriptor);
	EGE_GAME_ADD_LIST(meshs,	_EGE_Mesh_Descriptor);
	EGE_GAME_ADD_LIST(sounds,	_EGE_Sound_Descriptor);
	EGE_GAME_ADD_LIST(musics,	_EGE_Music_Descriptor);
}

