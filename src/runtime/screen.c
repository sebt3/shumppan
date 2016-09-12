/**
 *
 *  screen.c
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
#include <SDL/SDL_image.h>

#ifdef HAVE_GLES
# include <GLES/gl.h>
# include "eglport.h"
# define glClearDepth glClearDepthx
# define glOrtho glOrthox
#else
# include <GL/gl.h>
#endif

#include <stdlib.h>
#include <math.h>

// 
#include "pl_texture.h"
#include "game.h"
#include "screen.h"
#include "scene.h"


static SDL_Surface*	sdl_screen;
Viewport		EGE_viewport;
EGE_Font*		dejavu;
EGE_RGroup*		fps;
static Eina_Bool	showFPS;


static void _EGE_MakeIdentityf(GLfloat m[16]) {
	memset(m,0,16);
	m[0] = m[5] = m[10] = m[15] = 1;
}

#define __glPi 3.14159265358979323846

void EGE_Perspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar) {
    GLfloat m[4][4];
    float sine, cotangent, deltaZ;
    float radians = fovy / 2 * __glPi / 180;

    deltaZ = zFar - zNear;
    sine = sinf(radians);
    if ((deltaZ == 0) || (sine == 0) || (aspect == 0)) {
	return;
    }
    cotangent = cosf(radians) / sine;

    _EGE_MakeIdentityf(&m[0][0]);
    m[0][0] = cotangent / aspect;
    m[1][1] = cotangent;
    m[2][2] = -(zFar + zNear) / deltaZ;
    m[2][3] = -1;
    m[3][2] = -2 * zNear * zFar / deltaZ;
    m[3][3] = 0;
    GL_CHECK(glMultMatrixf(&m[0][0]));
}

static void _EGE_normalize(float v[3]) {
    float r;

    r = sqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
    if (r == 0.0) return;

    v[0] /= r;
    v[1] /= r;
    v[2] /= r;
}

static void _EGE_cross(float v1[3], float v2[3], float result[3]) {
    result[0] = v1[1]*v2[2] - v1[2]*v2[1];
    result[1] = v1[2]*v2[0] - v1[0]*v2[2];
    result[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

void EGE_LookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez, GLfloat centerx,
	  GLfloat centery, GLfloat centerz, GLfloat upx, GLfloat upy,
	  GLfloat upz) {
    float forward[3], side[3], up[3];
    GLfloat m[4][4];

    forward[0] = centerx - eyex;
    forward[1] = centery - eyey;
    forward[2] = centerz - eyez;

    up[0] = upx;
    up[1] = upy;
    up[2] = upz;

    _EGE_normalize(forward);

    /* Side = forward x up */
    _EGE_cross(forward, up, side);
    _EGE_normalize(side);

    /* Recompute up as: up = side x forward */
    _EGE_cross(side, forward, up);

    _EGE_MakeIdentityf(&m[0][0]);
    m[0][0] = side[0];
    m[1][0] = side[1];
    m[2][0] = side[2];

    m[0][1] = up[0];
    m[1][1] = up[1];
    m[2][1] = up[2];

    m[0][2] = -forward[0];
    m[1][2] = -forward[1];
    m[2][2] = -forward[2];

    glMultMatrixf(&m[0][0]);
    GL_CHECK(glTranslatef(-eyex, -eyey, -eyez));
}


void		EGE_Screen_Init(void) {
	EGE_vector mn, mx, p;

	EGE_viewport.scrn[0] = EGE_viewport.orig[0] = fp_from_int(800);
	EGE_viewport.scrn[1] = EGE_viewport.orig[1] = fp_from_int(480);
	EGE_viewport.is3D = EINA_FALSE;
	EGE_viewport.scaleFactor = 1;
/*#ifndef PANDORA
	EGE_viewport.scrn[0] = fp_from_int(1600);
	EGE_viewport.scrn[1] = fp_from_int(960);
	EGE_viewport.scaleFactor = 2;
#endif//*/
	EGE_viewport.eye[0]  = fp_from_int(200);
	EGE_viewport.eye[1]  = fp_from_int(0);
	EGE_viewport.eye[2]  = fp_from_int(240);
	EGE_viewport.at[0]   = fp_from_int(200);
	EGE_viewport.at[1]   = fp_from_int(140);
	EGE_viewport.at[2]   = fp_from_int(0);
	EGE_viewport.up[0]   = fp_from_int(0);
	EGE_viewport.up[1]   = fp_from_int(1);
	EGE_viewport.up[2]   = fp_from_int(0);
	EGE_viewport.scrn[2] = EGE_viewport.orig[2] = fp_0;
	showFPS		     = EINA_FALSE;
	p[0] = p[1] = p[2]   = fp_0;


	// Init SDL
	SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK );
	IMG_Init(IMG_INIT_PNG|IMG_INIT_JPG);

	// Init the GLScreen
#if defined(HAVE_GLES)
	sdl_screen = SDL_SetVideoMode(fp_to_int(EGE_viewport.scrn[0]), fp_to_int(EGE_viewport.scrn[1]), 0, SDL_SWSURFACE | SDL_FULLSCREEN );
	if (EGL_Open( 800, 480 )) {
		fprintf(stderr,"ERROR: Cannot open EGL interface\n");
		SDL_Quit();
		exit(1);
	}

#else
	sdl_screen = SDL_SetVideoMode(fp_to_int(EGE_viewport.scrn[0]), fp_to_int(EGE_viewport.scrn[1]), 0, SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER | SDL_OPENGL );
#endif
	if (!sdl_screen) {
		fprintf(stderr,"ERROR: Cannot allocate screen\n");
		SDL_Quit();
		exit(1);
	}

#if !defined(HAVE_GLES)
	SDL_WM_SetCaption( "Test", NULL );

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,		4);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,		4);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,		4);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,		4);

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,		16);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE,		16);

	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL,	1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,	1);
#endif

	// set the view port
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//glClearDepth(1.0f);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_ALPHA_TEST);
	glShadeModel(GL_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

	
	//Set the viewport
	mn[0] = mn[1] = mn[2] = mx[2]  = fp_0;
	mx[0] = EGE_viewport.scrn[0];
	mx[1] = EGE_viewport.scrn[1];
	glViewport(0, 0, fp_to_float(EGE_viewport.scrn[0]), fp_to_float(EGE_viewport.scrn[1]));
	EGE_Screen_Area_set(mn,mx);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnableClientState(GL_VERTEX_ARRAY);

	gameScene = NULL;
	uiScene = NULL;
	dejavu = NULL;
	fps = EGE_RGroupSprite_new(10, EINA_FALSE, EINA_TRUE);

	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClearDepth(fp2real(fp_1));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void		EGE_Screen_setViewport(void) {
	EGE_viewport.isInterface = EINA_FALSE;
	if (EGE_viewport.cur[0]==EGE_viewport.br[0] || EGE_viewport.cur[1] == EGE_viewport.br[1])
		return;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(EGE_viewport.is3D)
		EGE_Perspective(70,80/48,1,1000);
	else
		glOrtho(fp2real(EGE_viewport.cur[0]), fp2real(EGE_viewport.br[0]), fp2real(EGE_viewport.br[1]), fp2real(EGE_viewport.cur[1]), fp2real(fp_from_int(-1)), fp2real(fp_from_int(1)));
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	if(EGE_viewport.is3D == EINA_TRUE)
		EGE_LookAt(fp_to_float(EGE_viewport.eye[0]), fp_to_float(EGE_viewport.eye[1]), fp_to_float(EGE_viewport.eye[2]), 
			   fp_to_float(EGE_viewport.at[0]),  fp_to_float(EGE_viewport.at[1]),  fp_to_float(EGE_viewport.at[2]), 
			   fp_to_float(EGE_viewport.up[0]),  fp_to_float(EGE_viewport.up[1]),  fp_to_float(EGE_viewport.up[2]));
		
}

void		EGE_Screen_setInterface(void) {
	EGE_viewport.isInterface = EINA_TRUE;
	if (EGE_viewport.cur[0]==EGE_viewport.br[0] || EGE_viewport.cur[1] == EGE_viewport.br[1])
		return;
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, fp2real(EGE_viewport.scrn[0])/EGE_viewport.scaleFactor, fp2real(EGE_viewport.scrn[1])/EGE_viewport.scaleFactor, 0, fp2real(fp_from_int(-1)), fp2real(fp_from_int(1)));
}

Uint32 t=0;
Uint32 c=0;

void		EGE_Screen_Flip(void) {
	if (showFPS) EGE_RGroup_draw(fps);
#if !defined(HAVE_GLES)
	SDL_GL_SwapBuffers();
#else
	EGL_SwapBuffers();
#endif
	c++;
	if (SDL_GetTicks()>=t+1000 && showFPS) {
		if (dejavu==NULL) {
			EGE_RGroup_set_font(fps, EGE_Game_find_Font(g, "fps"));
		}
		char *txt  = calloc(32, sizeof(char));
		EGE_vector p;
		p[0] = p[1] = p[2] = fp_0;
		p[0] = EGE_viewport.scrn[0]/EGE_viewport.scaleFactor-fp_1*50*EGE_viewport.scaleFactor;
		sprintf(txt, "%d fps", c);
		EGE_RGroup_empty(fps);
		EGE_RGroup_Text_add(fps, txt, p);
		free(txt);

		t=SDL_GetTicks();
		c=0;
	}
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClearDepth(fp2real(fp_1));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void		EGE_Screen_Shutdown(void) {
	glDisableClientState(GL_VERTEX_ARRAY);
#if defined(HAVE_GLES)
	EGL_Close();
#endif
	SDL_Quit();
}


void		EGE_Screen_Area_set(EGE_vector mn, EGE_vector mx) {
	int i;
//	EGE_vector v = {fp_0,fp_0,fp_0};
	for(i=0;i<2;i++) {
		EGE_viewport.cur[i] = EGE_viewport.min[i] = mn[i];EGE_viewport.max[i] = mx[i];
		if(EGE_viewport.max[i]-EGE_viewport.min[i]<EGE_viewport.orig[i]) EGE_viewport.max[i]=EGE_viewport.orig[i]+EGE_viewport.min[i];
	}
}

void		EGE_Screen_View_move(EGE_vector v) {
	int i;

	if(EGE_viewport.is3D)
		for(i=0;i<3;i++) {
			EGE_viewport.eye[i] += v[i];
			EGE_viewport.at[i]  += v[i];
		}
	else
		for(i=0;i<2;i++) {
			EGE_viewport.cur[i] += v[i];
			if(EGE_viewport.cur[i]<EGE_viewport.min[i]) EGE_viewport.cur[i]=EGE_viewport.min[i];
			if(EGE_viewport.cur[i]>EGE_viewport.max[i]-EGE_viewport.orig[i]) EGE_viewport.cur[i]=EGE_viewport.max[i]-EGE_viewport.orig[i];
			EGE_viewport.br[i] = EGE_viewport.orig[i] + EGE_viewport.cur[i];
		}
}

void		EGE_Screen_LookAt(EGE_vector eye, EGE_vector at, EGE_vector up) {
	int i;
	for(i=0;i<3;i++) {
		EGE_viewport.eye[i]= eye[i];
		EGE_viewport.at[i] = at[i];
		EGE_viewport.up[i] = up[i];
	}
}
