/**
 *
 *  pl_texture.h
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
#ifndef __EGE_PL_TEXTURE_H__
#define __EGE_PL_TEXTURE_H__
#include "texture.h"
#include "game.h"

void		EGE_Texture_bind(EGE_Texture *t);
void		EGE_Texture_load(EGE_Texture *t);
void		EGE_Texture_unload(EGE_Texture *t);
void		EGE_Texture_Player_init(EGE_Games *g);
void		EGE_Texture_Player_shutdown(EGE_Games *g);
EGE_Texture*    EGE_Texture_newAlpha(const char *p_name, const unsigned int w, const unsigned int h, unsigned char *bmp);

#endif
