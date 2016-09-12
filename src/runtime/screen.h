/**
 *
 *  screen.h
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
#ifndef __EGE_SCREEN_H__
#define __EGE_SCREEN_H__

#include "fp.h"


#if defined(DEBUG)
#ifdef HAVE_GLES
#define GL_CHECK(x) \
        x; \
        { \
                GLenum glError = glGetError(); \
                switch(glError) { \
                        case GL_INVALID_OPERATION:      fprintf(stderr, "glGetError(INVALID_OPERATION) = %i (0x%.8x) at %s:%i\n", glError, glError, __FILE__, __LINE__);      break; \
                        case GL_INVALID_ENUM:           fprintf(stderr, "glGetError(INVALID_ENUM) = %i (0x%.8x) at %s:%i\n", glError, glError, __FILE__, __LINE__);      break; \
                        case GL_INVALID_VALUE:          fprintf(stderr, "glGetError(INVALID_VALUE) = %i (0x%.8x) at %s:%i\n", glError, glError, __FILE__, __LINE__);      break; \
                        case GL_OUT_OF_MEMORY:          fprintf(stderr, "glGetError(OUT_OF_MEMORY) = %i (0x%.8x) at %s:%i\n", glError, glError, __FILE__, __LINE__);      break; \
                } \
        }
#else
#define GL_CHECK(x) \
        x; \
        { \
                GLenum glError = glGetError(); \
                switch(glError) { \
                        case GL_INVALID_OPERATION:      fprintf(stderr, "glGetError(INVALID_OPERATION) = %i (0x%.8x) at %s:%i\n", glError, glError, __FILE__, __LINE__);      break; \
                        case GL_INVALID_ENUM:           fprintf(stderr, "glGetError(INVALID_ENUM) = %i (0x%.8x) at %s:%i\n", glError, glError, __FILE__, __LINE__);      break; \
                        case GL_INVALID_VALUE:          fprintf(stderr, "glGetError(INVALID_VALUE) = %i (0x%.8x) at %s:%i\n", glError, glError, __FILE__, __LINE__);      break; \
                        case GL_OUT_OF_MEMORY:          fprintf(stderr, "glGetError(OUT_OF_MEMORY) = %i (0x%.8x) at %s:%i\n", glError, glError, __FILE__, __LINE__);      break; \
                        case GL_INVALID_FRAMEBUFFER_OPERATION:  fprintf(stderr, "glGetError(INVALID_FRAMEBUFFER) = %i (0x%.8x) at %s:%i\n", glError, glError, __FILE__, __LINE__);      break; \
                } \
        }
#endif
#else
#define GL_CHECK(x) x;
#endif

#ifdef HAVE_GLES
#define EGE_POINTER_TYPE GL_FIXED
#define glColor glColor4x
#define glTranslate glTranslatex
#else
#define EGE_POINTER_TYPE GL_FLOAT
#define glTranslate glTranslatef
#define glColor glColor4f
#endif


void		EGE_Screen_Init(void);
void		EGE_Screen_Flip(void);
void		EGE_Screen_Shutdown(void);

typedef struct {
	EGE_vector	min, max, cur, orig, scrn, br;
	EGE_vector	eye, at,  up;
	int		scaleFactor;
	Eina_Bool	isInterface;
	Eina_Bool	is3D;
} Viewport;
extern Viewport EGE_viewport;
void		EGE_Screen_setViewport(void);
void		EGE_Screen_setInterface(void);
void		EGE_Screen_Area_set(EGE_vector mn, EGE_vector mx);
void		EGE_Screen_View_move(EGE_vector v);
void		EGE_Screen_Eye_move(EGE_vector v);
void		EGE_Screen_LookAt(EGE_vector eye, EGE_vector at, EGE_vector up);
#endif
