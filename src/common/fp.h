/**
 *
 *  fp.h
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
#ifndef __EGE_FP_H__
#define __EGE_FP_H__

#include <eina/eina_fp.h>

/***********************************************************************************
 *				    Fixed Numbers
 ***********************************************************************************/

// type definition
#define EGE_fp Eina_F16p16

// Usefull numbers
#define fp_0          0x0
#define fp_1          0x10000
#define fp_05	      0x00008000
#define fp_01	      0x0000028f
#define fp_sq2        0x00016a0a
#define PI            0x3243f
#define ANG_90        0x19220
#define ANG_90s       0x6b4
#define ANG_360s      0x1acf
#define ANG_720s      0x359e

extern const EGE_fp	pi;

// Conversion
#define fp_from_int(x) ((x)<<16)
#define fp_from_float eina_f16p16_float_from
#define fp_to_int     eina_f16p16_int_to
#define fp_to_float   eina_f16p16_float_to
#define fp_get_fract  eina_f16p16_fracc_get

// Simple Maths
#define fp_add        eina_f16p16_add
#define fp_sub        eina_f16p16_sub
#define fp_mul        eina_f16p16_mul
#define fp_div        eina_f16p16_div
#define fp_scale      eina_f16p16_scale
#define fp_sqrt       eina_f16p16_sqrt

// Trigo Function
const EGE_fp		cosx(const EGE_fp ri);
const EGE_fp		sinx(const EGE_fp ri);
const EGE_fp		tanx(const EGE_fp ri);
const EGE_fp		acosx(const EGE_fp ri);

typedef EGE_fp		EGE_texC[2];

/***********************************************************************************
 *					Vectors
 ***********************************************************************************/
typedef EGE_fp		EGE_vector[3];

void			EGE_vector_zero(EGE_vector v);
EGE_vector *		EGE_vector_new(void);
EGE_vector *		EGE_vector_copy(const EGE_vector v);
void			EGE_vector_negate(EGE_vector v);
void			EGE_vector_add_to(EGE_vector dest, const EGE_vector a);
void			EGE_vector_del_from(EGE_vector dest, const EGE_vector a);
void			EGE_vector_multiply(EGE_vector dest, const EGE_fp s);
EGE_fp			EGE_vector_dot(const EGE_vector v, const EGE_vector a);
__inline__ void		EGE_vector_divide(EGE_vector dest, const EGE_fp s);
EGE_fp			EGE_vector_len2(const EGE_vector v);
__inline__ EGE_fp	EGE_vector_length(const EGE_vector v);
void			EGE_vector_normalize(EGE_vector v);
__inline__ Eina_Bool	EGE_vector_eq(const EGE_vector a, const EGE_vector b);

/***********************************************************************************
 *					Matrix
 ***********************************************************************************/
typedef EGE_fp EGE_matrix[4][4];



/***********************************************************************************
 *				      Quaternion
 ***********************************************************************************/
typedef EGE_fp EGE_quater[4];


#endif
