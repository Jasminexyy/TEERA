/*
 * Copyright (c) 2012-2020 MIRACL UK Ltd.
 *
 * This file is part of MIRACL Core
 * (see https://github.com/miracl/core).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file fp4.h
 * @author Mike Scott
 * @brief FP4 Header File
 *
 */

#ifndef FP4_BLS12383_H
#define FP4_BLS12383_H

#include "fp2_BLS12383.h"
#include "config_curve_BLS12383.h"

/**
	@brief FP4 Structure - towered over two FP2
*/

typedef struct
{
    FP2_BLS12383 a; /**< real part of FP4 */
    FP2_BLS12383 b; /**< imaginary part of FP4 */
} FP4_BLS12383;


/* FP4 prototypes */
/**	@brief Tests for FP4 equal to zero
 *
	@param x FP4 number to be tested
	@return 1 if zero, else returns 0
 */
extern int FP4_BLS12383_iszilch(FP4_BLS12383 *x);

/**	@brief Tests for lexically larger 
 *
	@param x FP4 number to be tested if larger than -x
	@return 1 if larger, else returns 0
 */
extern int FP4_BLS12383_islarger(FP4_BLS12383 *x);

/**	@brief Serialize out FP4  
 *
    @param b buffer for output
	@param x FP4 number to be serialized
 */
extern void FP4_BLS12383_toBytes(char *b,FP4_BLS12383 *x);

/**	@brief Serialize in FP4  
 *
	@param x FP4 number to be serialized
    @param b buffer for input
 */
extern void FP4_BLS12383_fromBytes(FP4_BLS12383 *x,char *b);


/**	@brief Tests for FP4 equal to unity
 *
	@param x FP4 number to be tested
	@return 1 if unity, else returns 0
 */
extern int FP4_BLS12383_isunity(FP4_BLS12383 *x);
/**	@brief Tests for equality of two FP4s
 *
	@param x FP4 instance to be compared
	@param y FP4 instance to be compared
	@return 1 if x=y, else returns 0
 */
extern int FP4_BLS12383_equals(FP4_BLS12383 *x, FP4_BLS12383 *y);
/**	@brief Tests for FP4 having only a real part and no imaginary part
 *
	@param x FP4 number to be tested
	@return 1 if real, else returns 0
 */
extern int FP4_BLS12383_isreal(FP4_BLS12383 *x);
/**	@brief Initialise FP4 from two FP2s
 *
	@param x FP4 instance to be initialised
	@param a FP2 to form real part of FP4
	@param b FP2 to form imaginary part of FP4
 */
extern void FP4_BLS12383_from_FP2s(FP4_BLS12383 *x, FP2_BLS12383 *a, FP2_BLS12383 *b);
/**	@brief Initialise FP4 from single FP2
 *
	Imaginary part is set to zero
	@param x FP4 instance to be initialised
	@param a FP2 to form real part of FP4
 */
extern void FP4_BLS12383_from_FP2(FP4_BLS12383 *x, FP2_BLS12383 *a);

/**	@brief Initialise FP4 from single FP2
 *
	real part is set to zero
	@param x FP4 instance to be initialised
	@param a FP2 to form imaginary part of FP4
 */
extern void FP4_BLS12383_from_FP2H(FP4_BLS12383 *x, FP2_BLS12383 *a);

/**	@brief Initialise FP4 from single FP
 *
	@param x FP4 instance to be initialised
	@param a FP to form real part of FP4
 */
extern void FP4_BLS12383_from_FP(FP4_BLS12383 *x, FP_BLS12383 *a);

/**	@brief Copy FP4 to another FP4
 *
	@param x FP4 instance, on exit = y
	@param y FP4 instance to be copied
 */
extern void FP4_BLS12383_copy(FP4_BLS12383 *x, FP4_BLS12383 *y);
/**	@brief Set FP4 to zero
 *
	@param x FP4 instance to be set to zero
 */
extern void FP4_BLS12383_zero(FP4_BLS12383 *x);
/**	@brief Set FP4 to unity
 *
	@param x FP4 instance to be set to one
 */
extern void FP4_BLS12383_one(FP4_BLS12383 *x);

/**	@brief Sign of FP4
 *
	@param x FP4 instance
	@return "sign" of FP4
 */
extern int FP4_BLS12383_sign(FP4_BLS12383 *x);

/**	@brief Negation of FP4
 *
	@param x FP4 instance, on exit = -y
	@param y FP4 instance
 */
extern void FP4_BLS12383_neg(FP4_BLS12383 *x, FP4_BLS12383 *y);
/**	@brief Conjugation of FP4
 *
	If y=(a,b) on exit x=(a,-b)
	@param x FP4 instance, on exit = conj(y)
	@param y FP4 instance
 */
extern void FP4_BLS12383_conj(FP4_BLS12383 *x, FP4_BLS12383 *y);
/**	@brief Negative conjugation of FP4
 *
	If y=(a,b) on exit x=(-a,b)
	@param x FP4 instance, on exit = -conj(y)
	@param y FP4 instance
 */
extern void FP4_BLS12383_nconj(FP4_BLS12383 *x, FP4_BLS12383 *y);
/**	@brief addition of two FP4s
 *
	@param x FP4 instance, on exit = y+z
	@param y FP4 instance
	@param z FP4 instance
 */
extern void FP4_BLS12383_add(FP4_BLS12383 *x, FP4_BLS12383 *y, FP4_BLS12383 *z);
/**	@brief subtraction of two FP4s
 *
	@param x FP4 instance, on exit = y-z
	@param y FP4 instance
	@param z FP4 instance
 */
extern void FP4_BLS12383_sub(FP4_BLS12383 *x, FP4_BLS12383 *y, FP4_BLS12383 *z);
/**	@brief Multiplication of an FP4 by an FP2
 *
	@param x FP4 instance, on exit = y*a
	@param y FP4 instance
	@param a FP2 multiplier
 */
extern void FP4_BLS12383_pmul(FP4_BLS12383 *x, FP4_BLS12383 *y, FP2_BLS12383 *a);

/**	@brief Multiplication of an FP4 by an FP
 *
	@param x FP4 instance, on exit = y*a
	@param y FP4 instance
	@param a FP multiplier
 */
extern void FP4_BLS12383_qmul(FP4_BLS12383 *x, FP4_BLS12383 *y, FP_BLS12383 *a);

/**	@brief Multiplication of an FP4 by a small integer
 *
	@param x FP4 instance, on exit = y*i
	@param y FP4 instance
	@param i an integer
 */
extern void FP4_BLS12383_imul(FP4_BLS12383 *x, FP4_BLS12383 *y, int i);
/**	@brief Squaring an FP4
 *
	@param x FP4 instance, on exit = y^2
	@param y FP4 instance
 */
extern void FP4_BLS12383_sqr(FP4_BLS12383 *x, FP4_BLS12383 *y);
/**	@brief Multiplication of two FP4s
 *
	@param x FP4 instance, on exit = y*z
	@param y FP4 instance
	@param z FP4 instance
 */
extern void FP4_BLS12383_mul(FP4_BLS12383 *x, FP4_BLS12383 *y, FP4_BLS12383 *z);
/**	@brief Inverting an FP4
 *
	@param x FP4 instance, on exit = 1/y
	@param y FP4 instance
    @param h optional input hint
 */
extern void FP4_BLS12383_inv(FP4_BLS12383 *x, FP4_BLS12383 *y, FP_BLS12383 *h);
/**	@brief Formats and outputs an FP4 to the console
 *
	@param x FP4 instance to be printed
 */
extern void FP4_BLS12383_output(FP4_BLS12383 *x);
/**	@brief Formats and outputs an FP4 to the console in raw form (for debugging)
 *
	@param x FP4 instance to be printed
 */
extern void FP4_BLS12383_rawoutput(FP4_BLS12383 *x);
/**	@brief multiplies an FP4 instance by irreducible polynomial sqrt(1+sqrt(-1))
 *
	@param x FP4 instance, on exit = sqrt(1+sqrt(-1)*x
 */
extern void FP4_BLS12383_times_i(FP4_BLS12383 *x);
/**	@brief Normalises the components of an FP4
 *
	@param x FP4 instance to be normalised
 */
extern void FP4_BLS12383_norm(FP4_BLS12383 *x);
/**	@brief Reduces all components of possibly unreduced FP4 mod Modulus
 *
	@param x FP4 instance, on exit reduced mod Modulus
 */
extern void FP4_BLS12383_reduce(FP4_BLS12383 *x);
/**	@brief Raises an FP4 to the power of a BIG
 *
	@param x FP4 instance, on exit = y^b
	@param y FP4 instance
	@param b BIG number
 */
extern void FP4_BLS12383_pow(FP4_BLS12383 *x, FP4_BLS12383 *y, BIG_384_58 b);
/**	@brief Raises an FP4 to the power of the internal modulus p, using the Frobenius
 *
	@param x FP4 instance, on exit = x^p
	@param f FP2 precalculated Frobenius constant
 */
extern void FP4_BLS12383_frob(FP4_BLS12383 *x, FP2_BLS12383 *f);
/**	@brief Calculates the XTR addition function r=w*x-conj(x)*y+z
 *
	@param r FP4 instance, on exit = w*x-conj(x)*y+z
	@param w FP4 instance
	@param x FP4 instance
	@param y FP4 instance
	@param z FP4 instance
 */
extern void FP4_BLS12383_xtr_A(FP4_BLS12383 *r, FP4_BLS12383 *w, FP4_BLS12383 *x, FP4_BLS12383 *y, FP4_BLS12383 *z);
/**	@brief Calculates the XTR doubling function r=x^2-2*conj(x)
 *
	@param r FP4 instance, on exit = x^2-2*conj(x)
	@param x FP4 instance
 */
extern void FP4_BLS12383_xtr_D(FP4_BLS12383 *r, FP4_BLS12383 *x);
/**	@brief Calculates FP4 trace of an FP12 raised to the power of a BIG number
 *
	XTR single exponentiation
	@param r FP4 instance, on exit = trace(w^b)
	@param x FP4 instance, trace of an FP12 w
	@param b BIG number
 */
extern void FP4_BLS12383_xtr_pow(FP4_BLS12383 *r, FP4_BLS12383 *x, BIG_384_58 b);
/**	@brief Calculates FP4 trace of c^a.d^b, where c and d are derived from FP4 traces of FP12s
 *
	XTR double exponentiation
	Assumes c=tr(x^m), d=tr(x^n), e=tr(x^(m-n)), f=tr(x^(m-2n))
	@param r FP4 instance, on exit = trace(c^a.d^b)
	@param c FP4 instance, trace of an FP12
	@param d FP4 instance, trace of an FP12
	@param e FP4 instance, trace of an FP12
	@param f FP4 instance, trace of an FP12
	@param a BIG number
	@param b BIG number
 */
extern void FP4_BLS12383_xtr_pow2(FP4_BLS12383 *r, FP4_BLS12383 *c, FP4_BLS12383 *d, FP4_BLS12383 *e, FP4_BLS12383 *f, BIG_384_58 a, BIG_384_58 b);

/**	@brief Conditional copy of FP4 number
 *
	Conditionally copies second parameter to the first (without branching)
	@param x FP4 instance, set to y if s!=0
	@param y another FP4 instance
	@param s copy only takes place if not equal to 0
 */
extern void FP4_BLS12383_cmove(FP4_BLS12383 *x, FP4_BLS12383 *y, int s);

/**	@brief Test FP4 for QR
 * 
	@param r FP4 instance
    @param h optional generated hint
	@return 1 x is a QR, otherwise 0
 */
extern int  FP4_BLS12383_qr(FP4_BLS12383 *r, FP_BLS12383 *h);

/**	@brief Calculate square root of an FP4
 *
	Square root
	@param r FP4 instance, on exit = sqrt(x)
	@param x FP4 instance
	@param h optional input hint
 */
extern void  FP4_BLS12383_sqrt(FP4_BLS12383 *r, FP4_BLS12383 *x, FP_BLS12383 *h);


/**	@brief Divide FP4 number by QNR
 *
	Divide FP4 by the QNR
	@param x FP4 instance
 */
extern void FP4_BLS12383_div_i(FP4_BLS12383 *x);

/**	@brief Divide an FP4 by QNR/2
 *
	Divide FP4 by the QNR/2
	@param x FP4 instance
 */
extern void FP4_BLS12383_div_2i(FP4_BLS12383 *x);



/**	@brief Divide an FP4 by 2
 *
	@param x FP4 instance, on exit = y/2
	@param y FP4 instance
 */
extern void FP4_BLS12383_div2(FP4_BLS12383 *x, FP4_BLS12383 *y);

/**	@brief Generate random FP4
 *
	@param x random FP4 number
	@param rng random number generator
 */
extern void FP4_BLS12383_rand(FP4_BLS12383 *x, csprng *rng);

#endif

