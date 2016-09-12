/**
 *
 *  sound.h
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
#ifndef __EGE_SOUND_H__
#define __EGE_SOUND_H__
#ifdef USE_OPENAL
# include <libmodplug/modplug.h>
# include <AL/al.h>
# include <AL/alc.h>
# define NB_BUFFERS 8
#else
# include <SDL/SDL_mixer.h>
#endif
#include "fp.h"

void		EGE_Sound_Init(void);
void		EGE_Sound_Finish(void);

typedef struct {
	const char*	name;
	void*		data;
	uint32_t	data_count;
#ifdef USE_OPENAL
	ALuint		source;
	ALuint		buffer;
#else
	Mix_Chunk*	sdl;
#endif
} EGE_Sound;

EGE_Sound*	EGE_Sound_new(const char * m_name);
EGE_Sound*	EGE_Sound_from_file(const char * m_name, const char * f_name);
void		EGE_Sound_init(EGE_Sound *s);
void		EGE_Sound_free(EGE_Sound *s);
void		EGE_Sound_play(EGE_Sound *s);

typedef struct {
	const char*	name;
	void*		data;
	uint32_t	data_count;
	uint32_t	start_at;
#ifdef USE_OPENAL
	ALuint		buffers[NB_BUFFERS];
	ALuint		source;
	ModPlugFile*	mpf;
#else
	Mix_Music*	sdl;
#endif
} EGE_Music;

EGE_Music*	EGE_Music_new(const char * m_name);
EGE_Music*	EGE_Music_from_file(const char * m_name, const char * f_name);
void		EGE_Music_init(EGE_Music *s);
void		EGE_Music_free(EGE_Music *m);
void		EGE_Music_play(EGE_Music *m);
void		EGE_Music_update(void);

#endif
