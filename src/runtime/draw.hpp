/**
 *
 * panmines
 * Copyright (C) 2011 Sébastien Huss (sebt3 on boards.openpandora.org)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __screen_h__
#define __screen_h__

#include <SDL/SDL.h>
#ifdef HAVE_GLES
#include <GLES/gl.h>
#else
#include <GL/gl.h>
#endif


#if defined(CAANOO)
#  define SCREEN_WIDTH	320
#  define SCREEN_HEIGHT 240
#else
#  define SCREEN_WIDTH	800
#  define SCREEN_HEIGHT 480
#endif
#define DEGRAD 57.29577957795135

typedef struct{
	GLfloat x;
	GLfloat y;
	GLfloat z;
} GLpoint;

typedef struct{
	GLfloat r;
	GLfloat g;
	GLfloat b;
	GLfloat a;
} GL_Color;

class Texture {
public:
	Texture(SDL_Surface *s);
	~Texture();
	void bind();
	int w;
	int h;
private:
	void fromTexture(SDL_Surface *s);
	GL_Color c;
	GLuint texID;
};
typedef map<string,Texture*> TexMap;

class GLObject;
class Segment {
public:
	Segment(int d=0, GLenum m = GL_TRIANGLE_STRIP);
	~Segment();
	void setDotVtx(int id, GLfloat x, GLfloat y, GLfloat z);
	void setDotVtx(int id, GLpoint p);
	void setDotCol(int id, GLfloat r, GLfloat g, GLfloat b, GLfloat a);
	void setDotCol(int id, GL_Color c);
	void setDotTex(int id, GLfloat x, GLfloat y);
	void setAlpha(bool alpha = true);
	void setTexture(Texture *t);
	void setMode(GLenum m);
	void setColor(GL_Color c);
	void setThickness(int t);
	void moveBy(GLfloat x, GLfloat y, GLfloat z = 0);
	void draw();
private:
	GLenum	mode;
	int	dots;
	GLfloat *vtx;
	GLfloat *col;
	GLfloat *tex;
	GL_Color c;
	int	thickness;
	bool	haveAlpha;
	Texture *t;
};
typedef vector<Segment*> SegmentList;

class GLObject {
public:
	GLObject();
	GLObject(GLpoint p);
	GLObject(GLfloat x, GLfloat y, GLfloat z=0);
	~GLObject();
	void clear();
	void draw();
	void moveBy(GLfloat x, GLfloat y, GLfloat z = 0);
	void moveTo(GLfloat x, GLfloat y, GLfloat z = 0);
	void moveTo(GLpoint p);
	Segment *addTexture(GLpoint a, Texture* t);
	void addTextureCenter(GLpoint a, Texture* t);
	void addTexture(GLpoint a, Texture* t, GL_Color c);
	void addTextureCenter(GLpoint a, Texture* t, GL_Color c);
	void addTexture90(GLpoint a, Texture* t, GL_Color c);
	void addLine(GLpoint a, GLpoint b, GL_Color c, int thickness = 1);
	void addRect(GLpoint a, int w, int h, GL_Color c);
	void addDisc(GLpoint a, int r, GL_Color ci, GL_Color co, int from = 0, int to = 360, int prec = 2, int c_x = 0, int c_y = 0);
	void addArc(GLpoint a, int r, GL_Color c, int thickness=2, int from = 0, int to = 360, int prec = 2);
	void addBox(GLpoint a, int w, int h, GL_Color c);
	void addButton(GLpoint a, int w, int h, GL_Color ci, GL_Color co, int radiusPct = 20);
protected:
	SegmentList segs;
	GLpoint pos;
};
typedef map<string,GLObject*> ObjectMap;


class SpriteSet: public Texture {
public:
	SpriteSet(SDL_Surface *s, int sw, int sh);
	void setSprite(Segment *s, int sprite_id = 0);

	int sw;
	int sh;
private:
	int spl;
	float tsw;
	float tsh;
};

class TilesLayer: public GLObject {
public:
	TilesLayer(SpriteSet *ss, bool alpha, int w, int h);
private:
	SpriteSet *sheet;
	int tlw;
	int tlh;
	bool alpha;
	vector<SegmentList*> tiles;
};

class GLScreen {
public:
	GLScreen();
	~GLScreen();
	void		addTexture(string id, SDL_Surface *s);
	void		addTexture(string id, Texture *t);
 	Texture*	getTexture(string id);
	void		addObject(string id, GLObject *o);
	GLObject*	getObject(string id);
	void		flip();

	void drawFaceButton(bool happy);

private:
	SDL_Surface*	sdl_screen;
	TexMap		tex;
	ObjectMap	obj;
	Uint32		t;
};

#endif
