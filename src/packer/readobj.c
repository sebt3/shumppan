/**
 *
 *  readobj.c
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
#include <libgen.h>
#include "game.h"
#include "texture.h"
#include "readobj.h"
#include "storage.h"

#define EGE_MAX_MESH 5000

void		EGE_Mesh_set_materia_prop(EGE_Mesh *m, const char *p_filename) {
	FILE *		file	= fopen (p_filename, "r" );
	char		line[1024];
	char		token[128];
	char		arg[1024];
	float		x,y,z;
	int		i;
	while ( fgets ( line, sizeof line, file ) != NULL ) {
		if (sscanf(line, "%s ", token)<1)			continue;
		if (!strcmp(token, "#"))				continue;
		if (!strcmp(token, "Ka")) {
			if (sscanf(line, "%s %f %f %f", token, &x, &y, &z)<3)   continue;
			m->ambiant[0] = fp_from_float(x);	
			m->ambiant[1] = fp_from_float(y);	
			m->ambiant[2] = fp_from_float(z);	
		} else if (!strcmp(token, "Kd")) {
			if (sscanf(line, "%s %f %f %f", token, &x, &y, &z)<3)   continue;
			m->difuse[0] = fp_from_float(x);	
			m->difuse[1] = fp_from_float(y);	
			m->difuse[2] = fp_from_float(z);	
		} else if (!strcmp(token, "Ks")) {
			if (sscanf(line, "%s %f %f %f", token, &x, &y, &z)<3)   continue;
			m->specular[0] = fp_from_float(x);	
			m->specular[1] = fp_from_float(y);	
			m->specular[2] = fp_from_float(z);	
		} else if (!strcmp(token, "Tf")) {
			if (sscanf(line, "%s %f %f %f", token, &x, &y, &z)<3)   continue;
			m->transfilter[0] = fp_from_float(x);	
			m->transfilter[1] = fp_from_float(y);	
			m->transfilter[2] = fp_from_float(z);	
		} else if (!strcmp(token, "map_Kd")) {
			if (sscanf(line, "%s %s", token, arg)<2)		continue;
			for (i=0;i<strlen(arg);i++)
				if (arg[i] == '\\')
					arg[i] = '/';
			EGE_Mesh_Texture_set(m, EGE_Game_find_Texture(g, basename(arg)));
		}
	}
	fclose(file);
}

EGE_Mesh *      EGE_Mesh_load_from_obj(const char *p_name, const char *p_filename) {
	EGE_Mesh*	tmp	= EGE_Mesh_new(p_name, 4*EGE_MAX_MESH, 9*EGE_MAX_MESH, GL_TRIANGLE_STRIP);
	EGE_Mesh*	ret	= EGE_Mesh_new(p_name, 4*EGE_MAX_MESH, 9*EGE_MAX_MESH, GL_TRIANGLE_STRIP);
	FILE *		file	= fopen (p_filename, "r" );
	char		line[1024];
	char		token[128];
	char		arg[1024];
	char		mtl_file[1024];
	float		x,y,z;
	int		av,at,an,bv,bt,bn,cv,ct,cn,dv,dt,dn;
	EGE_texC	t;
	EGE_vector	v;
	int		iv=0,it=0,in=0,r;
	ret->faces = calloc(ret->faces_max, sizeof(unsigned short));
	while ( fgets ( line, sizeof line, file ) != NULL ) {
		if (sscanf(line, "%s ", token)<1)				continue;
		if (!strcmp(token, "#"))					continue;
		if (!strcmp(token, "mtllib")) {
			if (sscanf(line, "%s %s", token, arg)<2)		continue;
			sprintf(mtl_file,"%s/%s",dirname((char*)p_filename), arg);
		} else if (!strcmp(token, "usemtl")) {
			if (sscanf(line, "%s %s", token, arg)<2)		continue;
		} else if (!strcmp(token, "vt")) {
			if (sscanf(line, "%s %f %f %f", token, &x, &y, &z)<2)	continue;
			t[0] = fp_from_float(x);
			t[1] = fp_from_float(y);
			EGE_Mesh_tex_set(tmp,it,t);
			it++;
		} else if (!strcmp(token, "vn")) {
			if (sscanf(line, "%s %f %f %f", token, &x, &y, &z)<3)	continue;
			v[0] = fp_from_float(x);
			v[1] = fp_from_float(y);
			v[2] = fp_from_float(z);
			EGE_Mesh_norm_set(tmp,in,v);
			in++;
		} else if (!strcmp(token, "v")) {
			if (sscanf(line, "%s %f %f %f", token, &x, &y, &z)<3)	continue;
			v[0] = fp_from_float(x);
			v[1] = fp_from_float(y);
			v[2] = fp_from_float(z);
			EGE_Mesh_dot_set(tmp,iv,v);
			iv++;
		} else if (!strcmp(token, "f")) {
			r = sscanf(line, "%s %d//%d %d//%d %d//%d %d//%d", token, &av,&an, &bv,&bn, &cv,&cn, &dv,&dn);
			if (r>7) {
				printf("found a quad without tex coord ignoring\n");
			} else if (r>5) {
				printf("found a tri without tex coord ignoring\n");
			} else {
				r = sscanf(line, "%s %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", token, &av,&at,&an, &bv,&bt,&bn, &cv,&ct,&cn, &dv,&dt,&dn);
				if(r>9) {
					EGE_Mesh_dot_set(ret,  ret->dots_count/3 + 0, tmp->dots+3*av);
					EGE_Mesh_tex_set(ret,  ret->tex_count/2  + 0, tmp->tex+2*av);
					EGE_Mesh_norm_set(ret, ret->norm_count/3 + 0, tmp->norm+3*an);
					EGE_Mesh_dot_set(ret,  ret->dots_count/3 + 1, tmp->dots+3*bv);
					EGE_Mesh_tex_set(ret,  ret->tex_count/2  + 1, tmp->tex+2*bv);
					EGE_Mesh_norm_set(ret, ret->norm_count/3 + 1, tmp->norm+3*bn);
					EGE_Mesh_dot_set(ret,  ret->dots_count/3 + 2, tmp->dots+3*cv);
					EGE_Mesh_tex_set(ret,  ret->tex_count/2  + 2, tmp->tex+2*cv);
					EGE_Mesh_norm_set(ret, ret->norm_count/3 + 2, tmp->norm+3*cn);
					ret->dots_count += 9;
					ret->tex_count  += 6;
					ret->norm_count += 9;
					ret->faces_count+= 3;
					if (r>12) {
						// handling the second tri
						EGE_Mesh_dot_set(ret,  ret->dots_count/3 + 0, tmp->dots+3*dv);
						EGE_Mesh_tex_set(ret,  ret->tex_count/2  + 0, tmp->tex+2*dt);
						EGE_Mesh_norm_set(ret, ret->norm_count/3 + 0, tmp->norm+3*dn);
						ret->faces[ret->faces_count+0]	= ret->dots_count/3;
						ret->faces[ret->faces_count+0]	= ret->dots_count/3+1;
						ret->faces[ret->faces_count+0]	= ret->dots_count/3-3;
						ret->dots_count	+= 3;
						ret->tex_count  += 2;
						ret->norm_count += 3;
						ret->faces_count+= 3;
					}
				} else {
					printf("face parse failed, ignoring\n");
				}
			}

		} else if (!strcmp(token, "g")) {
			// object group ignoring
		} else if (!strcmp(token, "s")) {
			// shading model ignoring too
		} else {
			printf("unknown token %s\n", token);
		}

	}
	fclose(file);
	EGE_Mesh_free(tmp);
	EGE_Mesh_set_materia_prop(ret, mtl_file);
	EGE_Mesh_finalize(ret);
	return ret;
}
