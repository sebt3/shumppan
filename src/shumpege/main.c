/**
 *
 *  main.c
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
#include "game.h"
#include "texture.h"
#include "pl_texture.h"
#include "storage.h"

#include "screen.h"
#include "engine.h"
#include "scenes.h"

/*
 * TODO: 
 * - Meilleur son pour les bonus (+ fort et + long)
 */

int main(int argc, char *argv[]) {
	EGE_init();
	if (argc>1) 
		g = EGE_Game_load(argv[1]);
	else {
		g = EGE_Game_load("data_shump.eet");
	}
	if(!g) {
		printf("Failed to load data file\n");
		g=NULL;
		EGE_finish();
		return 1;
	}

	EGE_Texture_Player_init(g);
	EGE_enterScore_Scene_init();
	EGE_Menu_Scene_init();
	EGE_Dead_Scene_init();
	EGE_End_Scene_init();
	EGE_Pause_Scene_init();
	EGE_Intro_Scene_init();
	EGE_HighScore_Scene_init();
	EGE_Game_Scene_init();
	EGE_Credit_Scene_init();

	EGE_Menu_Scene_set();

	EGE_main_Loop();

	EGE_Game_Scene_free();
	EGE_Menu_Scene_free();
	EGE_Credit_Scene_free();
	EGE_Dead_Scene_free();
	EGE_End_Scene_free();
	EGE_Pause_Scene_free();
	EGE_HighScore_Scene_free();
	EGE_Intro_Scene_free();
	EGE_enterScore_Scene_free();

	EGE_Game_free(g);
	EGE_finish();
	return 0;
}
