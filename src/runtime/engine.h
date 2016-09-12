/**
 *
 *  engine.h
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
#ifndef __EGE_ENGINE_H__
#define __EGE_ENGINE_H__
#include <SDL/SDL.h>
#include "fp.h"

typedef Eina_Bool (*EGE_Event_CB)(SDL_Event *e);

void		EGE_init(void);
void		EGE_finish(void);
void		EGE_main_Loop(void);
inline Uint32	EGE_GetTicks(void);
#include "scene.h"
void		EGE_setScene(EGE_Scene *game, EGE_Scene *ui);
void		EGE_setUIScene(EGE_Scene*ui);
void		EGE_setUISceneAnime(EGE_Scene*ui, int type);
#endif
