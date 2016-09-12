/**
 *
 *  engine.c
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
#include <stdio.h>
#include <Eina.h>
#include <Eet.h>
#include <unistd.h>
#include "scene.h"
#include "game.h"
#include "texture.h"
#include "pl_texture.h"
#include "storage.h"

#include "screen.h"

EGE_Scene*	gameScene;
EGE_Scene*	uiScene;
EGE_Scene*	uiScene2;
int		timer;
int		animeType;

#define EGE_TICK2MS 16
#define ANIMLEN 30
inline Uint32	EGE_GetTicks(void) {
	return SDL_GetTicks()/EGE_TICK2MS;
}

void	EGE_init(void) {
	EGE_Game_Descriptor_Init();
	EGE_Screen_Init();
	EGE_Sound_Init();
	timer		= 0;
	gameScene	= NULL;
	uiScene2	= NULL;
	uiScene		= NULL;
}

void	EGE_finish(void) {
	if (g!=NULL)EGE_Texture_Player_shutdown(g);
	EGE_Sound_Finish();
	EGE_Screen_Shutdown();
	EGE_Game_Descriptor_Shutdown();
	exit(0);
}

Eina_Bool	EGE_Event_Handle(SDL_Event *e) {
	Eina_Bool quit = EINA_FALSE;
	switch(e->type) {
	case SDL_QUIT:
		quit = EINA_TRUE;
	break;
	case SDL_KEYUP:
		switch(e->key.keysym.sym) {
		case SDLK_q:
		case SDLK_ESCAPE:
			quit = EINA_TRUE;
		break;
		default:
		break;
		}
	break;
#ifdef CAANOO
	case SDL_JOYBUTTONUP:
		if (e->jbutton.button == 6)
			quit = EINA_TRUE;
	break;
#endif
	default:
	break;
	}
	return quit;
}

Eina_Bool	EGE_Update_Ticks(void) {
	// pool Input Event
	Eina_Bool quit = EINA_FALSE;
	SDL_Event event;
	while ( SDL_PollEvent(&event) && !quit) {
		if (timer>0) 
			EGE_Event_Handle(&event);
		else if (uiScene->onEvent != NULL) 
			quit = uiScene->onEvent(&event);
		else if (gameScene->onEvent != NULL) 
			quit = gameScene->onEvent(&event);
		else
			quit = EGE_Event_Handle(&event);
	}
	if (gameScene!=NULL)
 		EGE_Scene_runTick(gameScene);
	if (timer>0) {
		if(timer==1) {
			uiScene  = uiScene2;
			uiScene2 = NULL;
		}
		timer--;
		return quit;
	}
	if (uiScene!=NULL)
 		EGE_Scene_runTick(uiScene);
	if (uiScene!=NULL && uiScene->joystickR != NULL)
		uiScene->joystickR();
	
	return quit;
}

void	EGE_main_Loop(void) {
	Uint32 next_game_tick = EGE_GetTicks();
	Eina_Bool quit = EINA_FALSE;
	int i =0;

	while( quit == EINA_FALSE ) {
		i=0;
//		printf("[- (%d) %d -", EGE_GetTicks(), SDL_GetTicks());
		while( EGE_GetTicks() > next_game_tick && !quit) {
			i++;
			quit = EGE_Update_Ticks();
			next_game_tick ++;
//			if (i>1) printf("Running more ticks! %d\n", i);
		}
//		printf("- %d -", SDL_GetTicks());
		EGE_Music_update();

		if (gameScene!=NULL) {
			EGE_Screen_setViewport();
			EGE_Scene_draw(gameScene);
		}
		if (timer>0) {
			EGE_Screen_setInterface();
			switch(animeType) {
			case 0:
				glTranslate(fp2real(fp_from_int(-(800/ANIMLEN)*(ANIMLEN-timer))),0,0);
				break;
			case 1:
				glTranslate(0, fp2real(fp_from_int(-(480/ANIMLEN)*(ANIMLEN-timer))),0);
				break;
			case 2:
				glTranslate(fp2real(fp_from_int((800/ANIMLEN)*(ANIMLEN-timer))),0,0);
				break;
			case 3:
				glTranslate(0, fp2real(fp_from_int((480/ANIMLEN)*(ANIMLEN-timer))),0);
				break;
			}
			EGE_Scene_draw(uiScene);
			EGE_Screen_setInterface();
			switch(animeType) {
			case 0:
				glTranslate(fp2real(fp_from_int((800/ANIMLEN)*timer)),0,0);
				break;
			case 1:
				glTranslate(0, fp2real(fp_from_int((480/ANIMLEN)*timer)),0);
				break;
			case 2:
				glTranslate(fp2real(fp_from_int(-(800/ANIMLEN)*timer)),0,0);
				break;
			case 3:
				glTranslate(0, fp2real(fp_from_int(-(480/ANIMLEN)*timer)),0);
				break;
			}
			EGE_Scene_draw(uiScene2);
			EGE_Screen_setInterface();
		} else if (uiScene!=NULL) {
			EGE_Screen_setInterface();
			EGE_Scene_draw(uiScene);
		}

		EGE_Screen_Flip();
//		printf("- %d (%d) -]\n", SDL_GetTicks(), EGE_GetTicks());
		// frame limiter
		if (next_game_tick>EGE_GetTicks()+1) {
//			printf("Limiting frames!\n");
			SDL_Delay( (next_game_tick-EGE_GetTicks())*EGE_TICK2MS-5);
		}
			
	}
}


void		EGE_setScene(EGE_Scene* game, EGE_Scene* ui) {
	gameScene = game;
	uiScene = ui;
}
void		EGE_setUISceneAnime(EGE_Scene* ui, int type) {
	timer		= ANIMLEN;
	animeType	= type;
	uiScene2	= ui;
}

void		EGE_setUIScene(EGE_Scene* ui) {
	uiScene		= ui;
}
