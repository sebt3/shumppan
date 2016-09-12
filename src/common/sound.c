/**
 *
 *  sound.c
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
#include "sound.h"
#include "storage.h"

#ifdef USE_OPENAL
static ALCdevice* device;
static ALCcontext* context;
static inline ALenum GetFormatFromInfo(short channels, short bitsPerSample) {
	if (channels == 1)
		return AL_FORMAT_MONO16;
	return AL_FORMAT_STEREO16;
}
EGE_Music* current_music;


void	decode_it(EGE_Music *s, ALuint destination_buffer, int nb_samples) {
	ALushort* raw_data = calloc(nb_samples, sizeof(ALushort));
	// decode from the IT data the specified number of PCM samples
	int bytes_read = ModPlug_Read(s->mpf, raw_data, nb_samples);
	if (bytes_read == 0) {
		// on some systems, we have to make the music loop manually
		ModPlug_Seek(s->mpf, 0);
	}
	alBufferData(destination_buffer, AL_FORMAT_STEREO16, raw_data, nb_samples, 44100);
	free(raw_data);
}

#endif

Eina_Bool isLoaded;

void		EGE_Sound_Init(void) {
	isLoaded = EINA_FALSE;
#ifdef USE_OPENAL
	// init ModPlug
	ModPlug_Settings settings;
	ModPlug_GetSettings(&settings);
	settings.mChannels = 2;     // stereo
	settings.mBits = 16;        // 16 bits
	settings.mLoopCount = -1;   // loop forever
	ModPlug_SetSettings(&settings);

	// init openAl
	device = alcOpenDevice(NULL);
	context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);
	// define the listener
	alListener3f(AL_POSITION, 0, 0, 0);
	alListener3f(AL_VELOCITY, 0, 0, 0);
	alListener3f(AL_ORIENTATION, 0, 0, -1);

	// init the music system
	current_music = NULL;
#else
	// open the Sound Device
	if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 ) {
		printf("Audio init Failed !\n");
		return;
	}
	// load support for the OGG and MOD sample/music formats
	//int flags=MIX_INIT_OGG|MIX_INIT_MOD;
	int flags=MIX_INIT_MOD;
	int initted=Mix_Init(flags);
	if( (initted&flags) != flags) {
		printf("Mix_Init: Failed to init required ogg and mod support!\n");
		printf("Mix_Init: %s\n", Mix_GetError());
		return;
	}
#endif
	isLoaded = EINA_TRUE;
}
        

void		EGE_Sound_Finish(void) {
#ifdef USE_OPENAL
	alcDestroyContext(context);
	alcCloseDevice(device);
#else
	Mix_Quit();
	Mix_CloseAudio();
#endif
}

EGE_Sound*	EGE_Sound_new(const char * m_name) {
	EGE_Sound* ret		= calloc(1, sizeof(EGE_Sound));
	ret->name		= eina_stringshare_add(m_name);
	ret->data		= NULL;
	ret->data_count		= 0;
#ifdef USE_OPENAL
	ret->source		= AL_NONE;
	ret->buffer		= AL_NONE;
#else
	ret->sdl		= NULL;
#endif
	return ret;
}

void		EGE_Sound_free(EGE_Sound *s) {
	EGE_String_Free(s->name);
	if(s->data != NULL) free(s->data);
#ifdef USE_OPENAL
	if(s->source!=AL_NONE)
		alDeleteSources(1, &s->source);
	if(s->buffer!=AL_NONE)
		alDeleteBuffers(1, &s->buffer);

#else
	if(s->sdl  != NULL) Mix_FreeChunk(s->sdl);
#endif
	free(s);
}

EGE_Sound*	EGE_Sound_from_file(const char * m_name, const char * f_name) {
	EGE_Sound* ret	= NULL;
	long	size 	= 0;
	FILE	*f	= fopen(f_name, "rb");
	if (!f)	return NULL;

	fseek(f, 0, SEEK_END);size = ftell(f);fseek(f, 0, SEEK_SET);

	ret		= EGE_Sound_new(m_name);
	ret->data	= calloc(size,sizeof(char));
	ret->data_count	= (uint32_t)size;
	if (size != fread(ret->data, sizeof(char), size, f)) {
		// read error
		EGE_Sound_free(ret);
		return NULL;
	}
	fclose(f);

	//ret->sdl	= Mix_LoadWAV(f_name);
	return ret;
}

void		EGE_Sound_init(EGE_Sound *s) {
#ifdef USE_OPENAL
	// read the wav header
	int16_t	*buf16	= (int16_t*)s->data;
	int32_t	*buf32	= (int32_t*)s->data;
	int32_t	*ADATA	= (int32_t*)s->data + 11;
	int16_t numChan	= buf16[11];
	int32_t	rate	= buf32[6];
	int32_t	dcs	= buf32[1];
	int16_t bps	= buf16[18];

	// define the source
	alGenSources(1, &s->source);
	alSourcef(s->source, AL_PITCH, 1);
	alSourcef(s->source, AL_GAIN, 0.25f); // sound volume at 25%
	alSource3f(s->source, AL_POSITION, 0, 0, 0);
	alSource3f(s->source, AL_VELOCITY, 0, 0, 0);
	alSourcei(s->source, AL_LOOPING, AL_FALSE);
	
	//load the buffer
	alGenBuffers(1, &s->buffer);
	alBufferData(s->buffer, GetFormatFromInfo(numChan, bps), ADATA, dcs, rate);
	alSourcei(s->source, AL_BUFFER, s->buffer);
#else
	s->sdl = Mix_LoadWAV_RW(SDL_RWFromMem(s->data, s->data_count), 1);
	Mix_VolumeChunk(s->sdl, MIX_MAX_VOLUME/4);  // sound volume at 25%
#endif
}

void		EGE_Sound_play(EGE_Sound *s) {
	if (s==NULL)
		return;
#ifdef USE_OPENAL
	alSourcePlay(s->source);
#else
	if(s->sdl  != NULL) Mix_PlayChannel( -1, s->sdl, 0 );
#endif
}

EGE_Music*	EGE_Music_new(const char * m_name) {
	EGE_Music* ret		= calloc(1, sizeof(EGE_Music));
	ret->name		= eina_stringshare_add(m_name);
	ret->data		= NULL;
	ret->data_count		= 0;
	ret->start_at		= 0;
#ifdef USE_OPENAL
	int i;
	ret->source		= AL_NONE;
	for (i = 0; i < NB_BUFFERS; i++) {
		ret->buffers[i]	= AL_NONE;
	}
	ret->mpf		= NULL;
#else
	ret->sdl		= NULL;
#endif
	return ret;
}

void		EGE_Music_free(EGE_Music *m) {
	if (m==NULL)
		return;
	EGE_String_Free(m->name);
	if(m->data != NULL) free(m->data);
#ifdef USE_OPENAL
	int i;
	if(m->mpf  != NULL) {
		ModPlug_Unload(m->mpf);
	}
	if(m->source!=AL_NONE)
		alDeleteSources(1, &m->source);
	for(i=0; i < NB_BUFFERS; i++) {
		if(m->buffers[i]!=AL_NONE)
			alDeleteBuffers(1, &m->buffers[i]);
	}
#else
	if(m->sdl  != NULL) Mix_FreeMusic(m->sdl);
#endif
	free(m);
}

void		EGE_Music_play(EGE_Music *m) {
	if (m==NULL)
		return;
#ifdef USE_OPENAL
	int i;
	if (current_music!=NULL) {
		ALint nb_queued;
		ALuint buffer;
		alSourceStop(current_music->source);

		alGetSourcei(current_music->source, AL_BUFFERS_QUEUED, &nb_queued);
		for (i = 0; i < nb_queued; i++) {
			alSourceUnqueueBuffers(current_music->source, 1, &buffer);
		}
		alSourcei(current_music->source, AL_BUFFER, 0);
	}
	current_music	= m;
	ModPlug_Seek(current_music->mpf, current_music->start_at);
	for (i = 0; i < NB_BUFFERS; i++) {
		decode_it(current_music, current_music->buffers[i], 4096);
	}

	alSourceQueueBuffers(current_music->source, NB_BUFFERS, current_music->buffers);
	alSourcePlay(current_music->source);
#else
	if( Mix_PlayingMusic() == 0 )
		Mix_HaltMusic();
	if(m->sdl  != NULL) Mix_PlayMusic(m->sdl, -1);
#endif
}

/*SDL: //If the music is paused
if( Mix_PausedMusic() == 1 ) {
	//Resume the music
	Mix_ResumeMusic();
} //If the music is playing
else {
	//Pause the music
	Mix_PauseMusic();
}*/
EGE_Music*	EGE_Music_from_file(const char * m_name, const char * f_name) {
	EGE_Music* ret	= NULL;
	long	size 	= 0;
	FILE	*f	= fopen(f_name, "rb");
	if (!f)	return NULL;

	fseek(f, 0, SEEK_END);size = ftell(f);fseek(f, 0, SEEK_SET);

	ret		= EGE_Music_new(m_name);
	ret->data	= calloc(size,sizeof(char));
	ret->data_count	= (uint32_t)size;
	if (size != fread(ret->data, sizeof(char), size, f)) {
		// read error
		ret->data = NULL;
		EGE_Music_free(ret);
		ret=NULL;
	}
	fclose(f);
	return ret;
}

void		EGE_Music_init(EGE_Music *s) {
	if (s==NULL)
		return;
#ifdef USE_OPENAL
	// define the source
	alGenSources(1, &s->source);
	alSourcef(s->source, AL_PITCH, 1);
	alSourcef(s->source, AL_GAIN, 1.0f);
	alSource3f(s->source, AL_POSITION, 0, 0, 0);
	alSource3f(s->source, AL_VELOCITY, 0, 0, 0);

	//init modplug
	s->mpf = ModPlug_Load((const void*) s->data, s->data_count);

	//init the buffers
	alGenBuffers(NB_BUFFERS, s->buffers);

#else
	s->sdl = Mix_LoadMUS_RW(SDL_RWFromMem(s->data, s->data_count));
#endif
}


void		EGE_Music_update(void) {
#ifdef USE_OPENAL
	int i;
	ALint nb_empty;
	ALint status;
	if (current_music!=NULL) {
		alGetSourcei(current_music->source, AL_BUFFERS_PROCESSED, &nb_empty);
		for (i = 0; i < nb_empty; i++) {
			ALuint buffer;
			alSourceUnqueueBuffers(current_music->source, 1, &buffer); // unqueue the buffer
			decode_it(current_music, buffer, 4096);
			alSourceQueueBuffers(current_music->source, 1, &buffer);   // queue it agai
		}
	}
	alGetSourcei(current_music->source, AL_SOURCE_STATE, &status);

	if (status != AL_PLAYING) {
		alSourcePlay(current_music->source);
	}
#endif
}
