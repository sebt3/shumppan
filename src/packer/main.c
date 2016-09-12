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
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <Eina.h>
#include <Eet.h>
#include <unistd.h>
#include "game.h"
#include "texture.h"
#include "mesh.h"
#include "storage.h"
#include "readobj.h"

EGE_Mesh *	EGE_Mesh_load_from_emap(const char *p_name, const char *p_filename) {
	FILE *file = NULL;
	EGE_Mesh *ret=NULL;
	char line[1024];
	int lineno=0, dotno=0;
	int a,b,c,d, e, size;
	EGE_vector p={fp_0, fp_0, fp_05};
	EGE_vector t={fp_0, fp_0, fp_0};
	sprintf(line, "cat %s|wc -l", p_filename);
	file = popen(line, "r");
	fgets(line, sizeof line, file);
	sscanf(line, "%d", &size);
	fclose(file);
	ret = EGE_Mesh_new(p_name, size, 0, GL_TRIANGLE_STRIP);
	file = fopen(p_filename, "r");
	if (file == NULL) return NULL;
	while ( fgets ( line, sizeof line, file ) != NULL && dotno<size) {
		e=0;
		if (sscanf(line, "%d %d %d %d %d", &a, &b,&c,&d,&e)>3) {
			p[0] = fp_from_int(a);
			p[1] = fp_from_int(b);
			t[0] = fp_from_int(c);
			t[1] = fp_from_int(d);
			t[2] = fp_from_int(e);
			EGE_Mesh_dot_set(ret, dotno, p);ret->dots_count+=3;
			EGE_Mesh_norm_set(ret, dotno, t);ret->norm_count+=3;
			dotno++;
		} else {
			printf("Malformed file(%s), ignoring\n", p_filename);
			break;
		}
		lineno++;
	}	
	fclose(file);
	/*if (ret!=NULL)
		EGE_Mesh_finalize(ret);*/
	return ret;
}

EGE_Mesh *	EGE_Mesh_load_from_map(const char *p_name, const char *p_filename) {
	FILE *file = fopen ( p_filename, "r" );
	EGE_Mesh *ret=NULL;
	char line[40960];
	char token[128];
	char *l;
	int lineno=0;
	int x, y, z, i, j;
	int *map = NULL;
	EGE_Texture *t;
	EGE_vector p={fp_0, fp_0, fp_0};
	if (file == NULL) return NULL;
	while ( fgets ( line, sizeof line, file ) != NULL ) {
		if (lineno==0 && sscanf(line, "%s %d:%d", token, &x, &y)>2) {
			t = EGE_Game_find_Texture(g, token);
			if (t==NULL) {
				printf("Error while reading mapfile(%s), texture(%s) not found. ignoring\n", p_filename, token);
				break;
			}
			ret = EGE_Mesh_new(p_name, 4*x*y, 9*x*y, GL_TRIANGLE_STRIP);
			EGE_Mesh_Texture_set(ret, t);
			ret->is_tile   = EINA_TRUE;
			ret->tilemap_w = x;
			ret->tilemap_h = y;
			map = calloc(x*y, sizeof(int));
		} else if (lineno>0&&lineno<y+1) {
			l=line;i=0;
			while(sscanf(l, "%d", &z)>0) {
				//EGE_Mesh_add_tilemap_tile(ret, p, i, lineno-1, z);
				map[(lineno-1)*x+i] = z;
				l+=2;
				if (z>=10)l++;
				if (z>=100)l++;
				if (z>=1000)l++;
				i++;if(i>x-1) break;
			}
		} else {
			printf("Malformed file(%s), ignoring\n", p_filename);
			break;
		}
		lineno++;
	}	
	fclose(file);
	if (map!=NULL && ret!=NULL) {
		for(i=0;i<x;i++) {
			for(j=0;j<y;j++) {
				EGE_Mesh_add_tilemap_tile(ret, p, i, j, map[j*x+i]);
			}
		}
	}
	if (map!=NULL)
		free(map);
	if (ret!=NULL)
		EGE_Mesh_finalize(ret);
	return ret;
}

void		EGE_Texture_Properties_From(EGE_Texture *texture, const char *parfile) {
	FILE *file = fopen ( parfile, "r" );
	char line[1024];
	char token[128];
	int x, y, a, b;
	EGE_texC tl;
	EGE_texC br;
	if (file == NULL) return;
	while ( fgets ( line, sizeof line, file ) != NULL ) {
		if (sscanf(line, "%s %d:%d %d:%d", token, &x, &y,&a,&b)<5) {
			b = sscanf(line, "%s %d:%d %d", token, &x, &y, &a);
			if (b>2) {
				if(!strcmp(token, "tilemap")) {
					texture->tilesheet = EINA_TRUE;
					texture->tile_w = x;
					texture->tile_h = y;
					if(b>3 && a==2)
						texture->scale = EINA_TRUE;
				}
				else
					printf("Malformed file(%s), ignoring\n", parfile);
				break;
			}
			break;
		}
		tl[0] = fp_from_int(x);
		tl[1] = fp_from_int(y);
		br[0] = fp_from_int(a);
		br[1] = fp_from_int(b);
		EGE_Texture_add_sprite(texture,token,tl,br); 
	}	
	fclose(file);
}
void		EGE_Music_Properties_From(EGE_Music *m, const char *parfile) {
	FILE *file = fopen ( parfile, "r" );
	char line[1024];
	int x;
	if (file == NULL) return;
	while ( fgets ( line, sizeof line, file ) != NULL ) {
		if (sscanf(line, "%d", &x)==1) {
			m->start_at = x;
		}
	}	
	fclose(file);
}

void		EGE_Game_add_Font_From(EGE_Games *game, const char *filename, const char*parfile) {
	FILE *file = fopen ( parfile, "r" );
	char line[1024];
	char *token;
	int s;
	EGE_Font *f;
	if (file == NULL) return;
	while ( fgets ( line, sizeof line, file ) != NULL ) {
		token = calloc(128, sizeof(char));
		if (sscanf(line, "%s %d", token, &s)>0) {
			f = EGE_Font_new(filename, s, token);
			EGE_Game_add_Texture(game, f->texture);
			EGE_Game_add_Texture(game, f->texture2);
			EGE_Game_add_Font(game, f);
		}else free(token);
	}
	fclose(file);
}

void EGE_Game_add_From_Dir(EGE_Games *game, const char *dirname) {
	DIR *dp;
	struct dirent *ep;
	struct stat buffer;
	EGE_Texture *t;
	EGE_Mesh *m = NULL;
	EGE_Sound *s = NULL;
	EGE_Music *u = NULL;
	char *pnt, *parFile, *parF, *fullFile;

	dp = opendir (dirname);
	if (dp == NULL) return;
	// Load textures and fonts
	while ((ep = readdir (dp)))
		if (strcmp(ep->d_name, ".") && strcmp(ep->d_name, "..") && (pnt = strrchr(ep->d_name, '.'))) {
			fullFile = calloc(strlen(ep->d_name)+strlen(dirname)+2, sizeof(char));
			sprintf(fullFile, "%s/%s", dirname, ep->d_name);
			parFile = strdup(ep->d_name);memcpy(parFile + (int)pnt-(int)ep->d_name+1, "txt", 4);
			parF = calloc(strlen(parFile)+strlen(dirname)+2, sizeof(char));
			sprintf(parF, "%s/%s", dirname, parFile);free(parFile);
			if (stat(parF, &buffer)) {
				free(parF);parF=NULL;
			}

			if (!strncmp(pnt, ".bmp", 4)||!strncmp(pnt, ".png", 4)||!strncmp(pnt, ".jpg", 4)) {
				t = EGE_Texture_load_from(ep->d_name, fullFile);
				if (parF != NULL) 
					EGE_Texture_Properties_From(t, parF);
				EGE_Game_add_Texture(game, t);
				printf("added %s texture.\n", ep->d_name);
			} else if (!strncmp(pnt, ".it",3)||!strncmp(pnt, ".xm",3)) {
				u = EGE_Music_from_file(ep->d_name, fullFile);
				if(u!=NULL) {
					if (parF != NULL) 
						EGE_Music_Properties_From(u, parF);
					EGE_Game_add_Music(game, u);
					printf("added %s Music.\n", ep->d_name);
				}
			} else if (!strcmp(pnt, ".wav")) {
				s = EGE_Sound_from_file(ep->d_name, fullFile);
				if(s!=NULL) {
					EGE_Game_add_Sound(game, s);
					printf("added %s sound.\n", ep->d_name);
				}
			} else if (!strcmp(pnt, ".ttf")) {
				if (parF != NULL){
 					EGE_Game_add_Font_From(game, fullFile, parF);
					printf("added %s font.\n", ep->d_name);
				}else {
					printf("Font : %s (NoParFile)\n", fullFile);
				}
			}
			if (parF!=NULL)
				free(parF);
			free(fullFile);
		}
	rewinddir(dp);
	// Load tilemaps and meshs
	while ((ep = readdir (dp)))
		if (strcmp(ep->d_name, ".") && strcmp(ep->d_name, "..") && (pnt = strrchr(ep->d_name, '.'))) {
			fullFile = calloc(strlen(ep->d_name)+strlen(dirname)+2, sizeof(char));
			sprintf(fullFile, "%s/%s", dirname, ep->d_name);

			if (!strcmp(pnt, ".map")) {
				m = EGE_Mesh_load_from_map(ep->d_name, fullFile);
				EGE_Game_add_Mesh(game, m);
				printf("added %s tilemap.\n", ep->d_name);
			}else if (!strcmp(pnt, ".emap")) {
				m = EGE_Mesh_load_from_emap(ep->d_name, fullFile);
				EGE_Game_add_Mesh(game, m);
				printf("added %s emap.\n", ep->d_name);
			}else if (!strcmp(pnt, ".obj")) {
				m = EGE_Mesh_load_from_obj(ep->d_name, fullFile);
				EGE_Game_add_Mesh(game, m);
				printf("added %s mesh.\n", ep->d_name);
			}
			free(fullFile);
		}
	closedir (dp);
}


int main(int argc, char *argv[]) {
	//printf("0x%08x\n",fp_from_float(0.01));
	EGE_Game_Descriptor_Init();
	if (argc<2) {
		fprintf(stderr, "no file to pack\n");
		exit(1);
	}
	g = EGE_Game_new("game");
	EGE_Game_add_From_Dir(g, argv[2]);	
	EGE_Game_save(g, argv[1]);
	EGE_Game_Descriptor_Shutdown();
	return 0;
}
