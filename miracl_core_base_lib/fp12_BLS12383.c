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

/* CORE Fp^12 functions */
/* SU=m, m is Stack Usage (no lazy )*/
/* FP12 elements are of the form a+i.b+i^2.c */

#include "fp12_BLS12383.h"
#include "config_curve_BLS12383.h"

/* return 1 if b==c, no branching */
static int teq(sign32 b, sign32 c)
{
    sign32 x = b ^ c;
    x -= 1; // if x=0, x now -1
    return (int)((x >> 31) & 1);
}


/* Constant time select from pre-computed table */
static void FP12_BLS12383_select(FP12_BLS12383 *f, FP12_BLS12383 g[], sign32 b)
{
    FP12_BLS12383 invf;
    sign32 m = b >> 31;
    sign32 babs = (b ^ m) - m;

    babs = (babs - 1) / 2;

    FP12_BLS12383_cmove(f, &g[0], teq(babs, 0)); // conditional move
    FP12_BLS12383_cmove(f, &g[1], teq(babs, 1));
    FP12_BLS12383_cmove(f, &g[2], teq(babs, 2));
    FP12_BLS12383_cmove(f, &g[3], teq(babs, 3));
    FP12_BLS12383_cmove(f, &g[4], teq(babs, 4));
    FP12_BLS12383_cmove(f, &g[5], teq(babs, 5));
    FP12_BLS12383_cmove(f, &g[6], teq(babs, 6));
    FP12_BLS12383_cmove(f, &g[7], teq(babs, 7));

    FP12_BLS12383_copy(&invf, f);
    FP12_BLS12383_conj(&invf, &invf); // 1/f
    FP12_BLS12383_cmove(f, &invf, (int)(m & 1));
}



/* test x==0 ? */
/* SU= 8 */
int FP12_BLS12383_iszilch(FP12_BLS12383 *x)
{
    if (FP4_BLS12383_iszilch(&(x->a)) && FP4_BLS12383_iszilch(&(x->b)) && FP4_BLS12383_iszilch(&(x->c))) return 1;
    return 0;
}

/* test x==1 ? */
/* SU= 8 */
int FP12_BLS12383_isunity(FP12_BLS12383 *x)
{
    if (FP4_BLS12383_isunity(&(x->a)) && FP4_BLS12383_iszilch(&(x->b)) && FP4_BLS12383_iszilch(&(x->c))) return 1;
    return 0;
}

/* FP12 copy w=x */
/* SU= 16 */
void FP12_BLS12383_copy(FP12_BLS12383 *w, FP12_BLS12383 *x)
{
    if (x == w) return;
    FP4_BLS12383_copy(&(w->a), &(x->a));
    FP4_BLS12383_copy(&(w->b), &(x->b));
    FP4_BLS12383_copy(&(w->c), &(x->c));
    w->type = x->type;
}

/* FP12 w=1 */
/* SU= 8 */
void FP12_BLS12383_one(FP12_BLS12383 *w)
{
    FP4_BLS12383_one(&(w->a));
    FP4_BLS12383_zero(&(w->b));
    FP4_BLS12383_zero(&(w->c));
    w->type = FP_UNITY;
}

void FP12_BLS12383_zero(FP12_BLS12383 *w)
{
    FP4_BLS12383_zero(&(w->a));
    FP4_BLS12383_zero(&(w->b));
    FP4_BLS12383_zero(&(w->c));
    w->type = FP_ZILCH;
}

/* return 1 if x==y, else 0 */
/* SU= 16 */
int FP12_BLS12383_equals(FP12_BLS12383 *x, FP12_BLS12383 *y)
{
    if (FP4_BLS12383_equals(&(x->a), &(y->a)) && FP4_BLS12383_equals(&(x->b), &(y->b)) && FP4_BLS12383_equals(&(x->c), &(y->c)))
        return 1;
    return 0;
}

/* Set w=conj(x) */
/* SU= 8 */
void FP12_BLS12383_conj(FP12_BLS12383 *w, FP12_BLS12383 *x)
{
    FP12_BLS12383_copy(w, x);
    FP4_BLS12383_conj(&(w->a), &(w->a));
    FP4_BLS12383_nconj(&(w->b), &(w->b));
    FP4_BLS12383_conj(&(w->c), &(w->c));
}

/* Create FP12 from FP4 */
/* SU= 8 */
void FP12_BLS12383_from_FP4(FP12_BLS12383 *w, FP4_BLS12383 *a)
{
    FP4_BLS12383_copy(&(w->a), a);
    FP4_BLS12383_zero(&(w->b));
    FP4_BLS12383_zero(&(w->c));
    w->type = FP_SPARSEST;
}

/* Create FP12 from 3 FP4's */
/* SU= 16 */
void FP12_BLS12383_from_FP4s(FP12_BLS12383 *w, FP4_BLS12383 *a, FP4_BLS12383 *b, FP4_BLS12383 *c)
{
    FP4_BLS12383_copy(&(w->a), a);
    FP4_BLS12383_copy(&(w->b), b);
    FP4_BLS12383_copy(&(w->c), c);
    w->type = FP_DENSE;
}

/* Granger-Scott Unitary Squaring. This does not benefit from lazy reduction */
/* SU= 600 */
void FP12_BLS12383_usqr(FP12_BLS12383 *w, FP12_BLS12383 *x)
{
    FP4_BLS12383 A, B, C, D;

    FP4_BLS12383_copy(&A, &(x->a));

    FP4_BLS12383_sqr(&(w->a), &(x->a));
    FP4_BLS12383_add(&D, &(w->a), &(w->a));
    FP4_BLS12383_add(&(w->a), &D, &(w->a));

    FP4_BLS12383_norm(&(w->a));
    FP4_BLS12383_nconj(&A, &A);

    FP4_BLS12383_add(&A, &A, &A);
    FP4_BLS12383_add(&(w->a), &(w->a), &A);
    FP4_BLS12383_sqr(&B, &(x->c));
    FP4_BLS12383_times_i(&B);

    FP4_BLS12383_add(&D, &B, &B);
    FP4_BLS12383_add(&B, &B, &D);
    FP4_BLS12383_norm(&B);

    FP4_BLS12383_sqr(&C, &(x->b));

    FP4_BLS12383_add(&D, &C, &C);
    FP4_BLS12383_add(&C, &C, &D);

    FP4_BLS12383_norm(&C);
    FP4_BLS12383_conj(&(w->b), &(x->b));
    FP4_BLS12383_add(&(w->b), &(w->b), &(w->b));
    FP4_BLS12383_nconj(&(w->c), &(x->c));

    FP4_BLS12383_add(&(w->c), &(w->c), &(w->c));
    FP4_BLS12383_add(&(w->b), &B, &(w->b));
    FP4_BLS12383_add(&(w->c), &C, &(w->c));

    w->type = FP_DENSE;
    FP12_BLS12383_reduce(w);     /* reduce here as in pow function repeated squarings would trigger multiple reductions */
}

/* FP12 squaring w=x^2 */
/* SU= 600 */
void FP12_BLS12383_sqr(FP12_BLS12383 *w, FP12_BLS12383 *x)
{
    /* Use Chung-Hasan SQR2 method from http://cacr.uwaterloo.ca/techreports/2006/cacr2006-24.pdf */

    FP4_BLS12383 A, B, C, D;

    if (x->type <= FP_UNITY)
    {
        FP12_BLS12383_copy(w, x);
        return;
    }

    FP4_BLS12383_sqr(&A, &(x->a));
    FP4_BLS12383_mul(&B, &(x->b), &(x->c));
    FP4_BLS12383_add(&B, &B, &B);
    FP4_BLS12383_norm(&B);
    FP4_BLS12383_sqr(&C, &(x->c));

    FP4_BLS12383_mul(&D, &(x->a), &(x->b));
    FP4_BLS12383_add(&D, &D, &D);
    FP4_BLS12383_add(&(w->c), &(x->a), &(x->c));
    FP4_BLS12383_add(&(w->c), &(x->b), &(w->c));
    FP4_BLS12383_norm(&(w->c));

    FP4_BLS12383_sqr(&(w->c), &(w->c));

    FP4_BLS12383_copy(&(w->a), &A);
    FP4_BLS12383_add(&A, &A, &B);

    FP4_BLS12383_norm(&A);

    FP4_BLS12383_add(&A, &A, &C);
    FP4_BLS12383_add(&A, &A, &D);

    FP4_BLS12383_norm(&A);
    FP4_BLS12383_neg(&A, &A);
    FP4_BLS12383_times_i(&B);
    FP4_BLS12383_times_i(&C);

    FP4_BLS12383_add(&(w->a), &(w->a), &B);
    FP4_BLS12383_add(&(w->b), &C, &D);
    FP4_BLS12383_add(&(w->c), &(w->c), &A);

    if (x->type == FP_SPARSER || x->type == FP_SPARSEST)
        w->type = FP_SPARSE;
    else
        w->type = FP_DENSE;

    FP12_BLS12383_norm(w);
}

/* FP12 full multiplication w=w*y */


/* SU= 896 */
/* FP12 full multiplication w=w*y */
void FP12_BLS12383_mul(FP12_BLS12383 *w, FP12_BLS12383 *y)
{
    FP4_BLS12383 z0, z1, z2, z3, t0, t1;

    FP4_BLS12383_mul(&z0, &(w->a), &(y->a));
    FP4_BLS12383_mul(&z2, &(w->b), &(y->b)); //

    FP4_BLS12383_add(&t0, &(w->a), &(w->b));
    FP4_BLS12383_add(&t1, &(y->a), &(y->b)); //

    FP4_BLS12383_norm(&t0);
    FP4_BLS12383_norm(&t1);

    FP4_BLS12383_mul(&z1, &t0, &t1);
    FP4_BLS12383_add(&t0, &(w->b), &(w->c));
    FP4_BLS12383_add(&t1, &(y->b), &(y->c)); //

    FP4_BLS12383_norm(&t0);
    FP4_BLS12383_norm(&t1);

    FP4_BLS12383_mul(&z3, &t0, &t1);

    FP4_BLS12383_neg(&t0, &z0);
    FP4_BLS12383_neg(&t1, &z2);

    FP4_BLS12383_add(&z1, &z1, &t0); // z1=z1-z0
    FP4_BLS12383_add(&(w->b), &z1, &t1); // z1=z1-z2
    FP4_BLS12383_add(&z3, &z3, &t1);      // z3=z3-z2
    FP4_BLS12383_add(&z2, &z2, &t0);      // z2=z2-z0

    FP4_BLS12383_add(&t0, &(w->a), &(w->c));
    FP4_BLS12383_add(&t1, &(y->a), &(y->c));

    FP4_BLS12383_norm(&t0);
    FP4_BLS12383_norm(&t1);

    FP4_BLS12383_mul(&t0, &t1, &t0);
    FP4_BLS12383_add(&z2, &z2, &t0);

    FP4_BLS12383_mul(&t0, &(w->c), &(y->c));
    FP4_BLS12383_neg(&t1, &t0);

    FP4_BLS12383_add(&(w->c), &z2, &t1);
    FP4_BLS12383_add(&z3, &z3, &t1);
    FP4_BLS12383_times_i(&t0);
    FP4_BLS12383_add(&(w->b), &(w->b), &t0);
    FP4_BLS12383_norm(&z3);
    FP4_BLS12383_times_i(&z3);
    FP4_BLS12383_add(&(w->a), &z0, &z3);

    w->type = FP_DENSE;
    FP12_BLS12383_norm(w);
}

/* FP12 full multiplication w=w*y */
/* Supports sparse multiplicands */
/* Usually w is denser than y */
void FP12_BLS12383_ssmul(FP12_BLS12383 *w, FP12_BLS12383 *y)
{
    FP4_BLS12383 z0, z1, z2, z3, t0, t1;
    if (w->type == FP_UNITY)
    {
        FP12_BLS12383_copy(w, y);
        return;
    }
    if (y->type == FP_UNITY)
        return;

    if (y->type >= FP_SPARSE)
    {
        FP4_BLS12383_mul(&z0, &(w->a), &(y->a)); // xa.ya   always 11x11

#if SEXTIC_TWIST_BLS12383 == M_TYPE
        if (y->type == FP_SPARSE || w->type == FP_SPARSE)
        {
            FP2_BLS12383_mul(&z2.b, &(w->b).b, &(y->b).b);
            FP2_BLS12383_zero(&z2.a);
            if (y->type != FP_SPARSE)
                FP2_BLS12383_mul(&z2.a, &(w->b).b, &(y->b).a);
            if (w->type != FP_SPARSE)
                FP2_BLS12383_mul(&z2.a, &(w->b).a, &(y->b).b);
            FP4_BLS12383_times_i(&z2);
        }
        else
#endif
            FP4_BLS12383_mul(&z2, &(w->b), &(y->b)); // xb.yb  could be 00x00 or 01x01 or or 10x10 or 11x00 or 11x10 or 11x01 or 11x11

        FP4_BLS12383_add(&t0, &(w->a), &(w->b)); // (xa+xb)
        FP4_BLS12383_add(&t1, &(y->a), &(y->b)); // (ya+yb)

        FP4_BLS12383_norm(&t0);
        FP4_BLS12383_norm(&t1);

        FP4_BLS12383_mul(&z1, &t0, &t1); // (xa+xb)(ya+yb)  always 11x11
        FP4_BLS12383_add(&t0, &(w->b), &(w->c)); // (xb+xc)
        FP4_BLS12383_add(&t1, &(y->b), &(y->c)); // (yb+yc)

        FP4_BLS12383_norm(&t0);
        FP4_BLS12383_norm(&t1);

        FP4_BLS12383_mul(&z3, &t0, &t1); // (xb+xc)(yb+yc)   could be anything...
        FP4_BLS12383_neg(&t0, &z0);      // -(xa.ya)
        FP4_BLS12383_neg(&t1, &z2);      // -(xb.yb)

        FP4_BLS12383_add(&z1, &z1, &t0);
        FP4_BLS12383_add(&(w->b), &z1, &t1); // /wb = (xa+xb)(ya+yb) -(xa.ya) -(xb.yb)                     = xa.yb + xb.ya

        FP4_BLS12383_add(&z3, &z3, &t1);      // (xb+xc)(yb+yc) -(xb.yb)
        FP4_BLS12383_add(&z2, &z2, &t0);      // (xb.yb) - (xa.ya)

        FP4_BLS12383_add(&t0, &(w->a), &(w->c)); // (xa+xc)
        FP4_BLS12383_add(&t1, &(y->a), &(y->c)); // (ya+yc)

        FP4_BLS12383_norm(&t0);
        FP4_BLS12383_norm(&t1);

        FP4_BLS12383_mul(&t0, &t1, &t0); // (xa+xc)(ya+yc)    always 11x11
        FP4_BLS12383_add(&z2, &z2, &t0); // (xb.yb) - (xa.ya) + (xa+xc)(ya+yc)

#if SEXTIC_TWIST_BLS12383 == D_TYPE
        if (y->type == FP_SPARSE || w->type == FP_SPARSE)
        {
            FP2_BLS12383_mul(&t0.a, &(w->c).a, &(y->c).a);
            FP2_BLS12383_zero(&t0.b);
            if (y->type != FP_SPARSE)
                FP2_BLS12383_mul(&t0.b, &(w->c).a, &(y->c).b);
            if (w->type != FP_SPARSE)
                FP2_BLS12383_mul(&t0.b, &(w->c).b, &(y->c).a);
        }
        else
#endif
            FP4_BLS12383_mul(&t0, &(w->c), &(y->c)); // (xc.yc)  could be anything

        FP4_BLS12383_neg(&t1, &t0);            // -(xc.yc)

        FP4_BLS12383_add(&(w->c), &z2, &t1);     // wc = (xb.yb) - (xa.ya) + (xa+xc)(ya+yc) - (xc.yc)    =  xb.yb + xc.ya + xa.yc
        FP4_BLS12383_add(&z3, &z3, &t1);         // (xb+xc)(yb+yc) -(xb.yb) - (xc.yc)                    =  xb.yc + xc.yb
        FP4_BLS12383_times_i(&t0);               // i.(xc.yc)
        FP4_BLS12383_add(&(w->b), &(w->b), &t0); // wb = (xa+xb)(ya+yb) -(xa.ya) -(xb.yb) +i(xc.yc)
        FP4_BLS12383_norm(&z3);
        FP4_BLS12383_times_i(&z3);               // i[(xb+xc)(yb+yc) -(xb.yb) - (xc.yc)]                 = i(xb.yc + xc.yb)
        FP4_BLS12383_add(&(w->a), &z0, &z3);     // wa = xa.ya + i(xb.yc + xc.yb)
    } else {
        if (w->type == FP_SPARSER || w->type == FP_SPARSEST)
        {
            FP12_BLS12383_smul(w, y);
            return;
        }
// dense by sparser - 13m
#if SEXTIC_TWIST_BLS12383 == D_TYPE
        FP4_BLS12383_copy(&z3, &(w->b));
        FP4_BLS12383_mul(&z0, &(w->a), &(y->a));

        if (y->type == FP_SPARSEST)
            FP4_BLS12383_qmul(&z2, &(w->b), &(y->b).a.a);
        else
            FP4_BLS12383_pmul(&z2, &(w->b), &(y->b).a);

        FP4_BLS12383_add(&(w->b), &(w->a), &(w->b));
        FP4_BLS12383_copy(&t1, &(y->a));
        FP2_BLS12383_add(&t1.a, &t1.a, &(y->b).a);

        FP4_BLS12383_norm(&t1);
        FP4_BLS12383_norm(&(w->b));

        FP4_BLS12383_mul(&(w->b), &(w->b), &t1);
        FP4_BLS12383_add(&z3, &z3, &(w->c));
        FP4_BLS12383_norm(&z3);
        if (y->type == FP_SPARSEST)
            FP4_BLS12383_qmul(&z3, &z3, &(y->b).a.a);
        else
            FP4_BLS12383_pmul(&z3, &z3, &(y->b).a);

        FP4_BLS12383_neg(&t0, &z0);
        FP4_BLS12383_neg(&t1, &z2);

        FP4_BLS12383_add(&(w->b), &(w->b), &t0); // z1=z1-z0
        FP4_BLS12383_add(&(w->b), &(w->b), &t1); // z1=z1-z2

        FP4_BLS12383_add(&z3, &z3, &t1);      // z3=z3-z2
        FP4_BLS12383_add(&z2, &z2, &t0);      // z2=z2-z0

        FP4_BLS12383_add(&t0, &(w->a), &(w->c));
        FP4_BLS12383_norm(&t0);
        FP4_BLS12383_norm(&z3);

        FP4_BLS12383_mul(&t0, &(y->a), &t0);
        FP4_BLS12383_add(&(w->c), &z2, &t0);

        FP4_BLS12383_times_i(&z3);
        FP4_BLS12383_add(&(w->a), &z0, &z3);
#endif
#if SEXTIC_TWIST_BLS12383 == M_TYPE
        FP4_BLS12383_mul(&z0, &(w->a), &(y->a));
        FP4_BLS12383_add(&t0, &(w->a), &(w->b));
        FP4_BLS12383_norm(&t0);

        FP4_BLS12383_mul(&z1, &t0, &(y->a));
        FP4_BLS12383_add(&t0, &(w->b), &(w->c));
        FP4_BLS12383_norm(&t0);

        if (y->type == FP_SPARSEST)
            FP4_BLS12383_qmul(&z3, &t0, &(y->c).b.a);
        else
            FP4_BLS12383_pmul(&z3, &t0, &(y->c).b);

        FP4_BLS12383_times_i(&z3);

        FP4_BLS12383_neg(&t0, &z0);
        FP4_BLS12383_add(&z1, &z1, &t0); // z1=z1-z0

        FP4_BLS12383_copy(&(w->b), &z1);
        FP4_BLS12383_copy(&z2, &t0);

        FP4_BLS12383_add(&t0, &(w->a), &(w->c));
        FP4_BLS12383_add(&t1, &(y->a), &(y->c));

        FP4_BLS12383_norm(&t0);
        FP4_BLS12383_norm(&t1);

        FP4_BLS12383_mul(&t0, &t1, &t0);
        FP4_BLS12383_add(&z2, &z2, &t0);

        if (y->type == FP_SPARSEST)
            FP4_BLS12383_qmul(&t0, &(w->c), &(y->c).b.a);
        else
            FP4_BLS12383_pmul(&t0, &(w->c), &(y->c).b);

        FP4_BLS12383_times_i(&t0);
        FP4_BLS12383_neg(&t1, &t0);
        FP4_BLS12383_times_i(&t0);

        FP4_BLS12383_add(&(w->c), &z2, &t1);
        FP4_BLS12383_add(&z3, &z3, &t1);

        FP4_BLS12383_add(&(w->b), &(w->b), &t0);
        FP4_BLS12383_norm(&z3);
        FP4_BLS12383_times_i(&z3);
        FP4_BLS12383_add(&(w->a), &z0, &z3);

#endif
    }
    w->type = FP_DENSE;
    FP12_BLS12383_norm(w);
}

/* FP12 multiplication w=w*y */
/* catering for special case that arises from special form of ATE pairing line function */
/* w and y are both sparser line functions - cost = 6m */
void FP12_BLS12383_smul(FP12_BLS12383 *w, FP12_BLS12383 *y)
{
    FP2_BLS12383 w1, w2, w3, ta, tb, tc, td, te, t;

//  if (type==D_TYPE)
//  {
#if SEXTIC_TWIST_BLS12383 == D_TYPE
    FP2_BLS12383_mul(&w1, &(w->a).a, &(y->a).a); // A1.A2
    FP2_BLS12383_mul(&w2, &(w->a).b, &(y->a).b); // B1.B2

    if (y->type == FP_SPARSEST || w->type == FP_SPARSEST)
    {
        if (y->type == FP_SPARSEST && w->type == FP_SPARSEST)
        {
            FP_BLS12383_mul(&w3.a, &(w->b).a.a, &(y->b).a.a);
            FP_BLS12383_zero(&w3.b);
        } else {
            if (y->type != FP_SPARSEST)
                FP2_BLS12383_pmul(&w3, &(y->b).a, &(w->b).a.a);
            if (w->type != FP_SPARSEST)
                FP2_BLS12383_pmul(&w3, &(w->b).a, &(y->b).a.a);
        }
    }
    else
        FP2_BLS12383_mul(&w3, &(w->b).a, &(y->b).a); // C1.C2

    FP2_BLS12383_add(&ta, &(w->a).a, &(w->a).b); // A1+B1
    FP2_BLS12383_add(&tb, &(y->a).a, &(y->a).b); // A2+B2
    FP2_BLS12383_norm(&ta);
    FP2_BLS12383_norm(&tb);
    FP2_BLS12383_mul(&tc, &ta, &tb);         // (A1+B1)(A2+B2)
    FP2_BLS12383_add(&t, &w1, &w2);
    FP2_BLS12383_neg(&t, &t);
    FP2_BLS12383_add(&tc, &tc, &t);          // (A1+B1)(A2+B2)-A1.A2-B1*B2 =  (A1.B2+A2.B1)

    FP2_BLS12383_add(&ta, &(w->a).a, &(w->b).a); // A1+C1
    FP2_BLS12383_add(&tb, &(y->a).a, &(y->b).a); // A2+C2
    FP2_BLS12383_norm(&ta);
    FP2_BLS12383_norm(&tb);
    FP2_BLS12383_mul(&td, &ta, &tb);         // (A1+C1)(A2+C2)
    FP2_BLS12383_add(&t, &w1, &w3);
    FP2_BLS12383_neg(&t, &t);
    FP2_BLS12383_add(&td, &td, &t);          // (A1+C1)(A2+C2)-A1.A2-C1*C2 =  (A1.C2+A2.C1)

    FP2_BLS12383_add(&ta, &(w->a).b, &(w->b).a); // B1+C1
    FP2_BLS12383_add(&tb, &(y->a).b, &(y->b).a); // B2+C2
    FP2_BLS12383_norm(&ta);
    FP2_BLS12383_norm(&tb);
    FP2_BLS12383_mul(&te, &ta, &tb);         // (B1+C1)(B2+C2)
    FP2_BLS12383_add(&t, &w2, &w3);
    FP2_BLS12383_neg(&t, &t);
    FP2_BLS12383_add(&te, &te, &t);          // (B1+C1)(B2+C2)-B1.B2-C1*C2 =  (B1.C2+B2.C1)

    FP2_BLS12383_mul_ip(&w2);
    FP2_BLS12383_add(&w1, &w1, &w2);
    FP4_BLS12383_from_FP2s(&(w->a), &w1, &tc);
    FP4_BLS12383_from_FP2s(&(w->b), &td, &te); // only norm these 2
    FP4_BLS12383_from_FP2(&(w->c), &w3);

    FP4_BLS12383_norm(&(w->a));
    FP4_BLS12383_norm(&(w->b));
#endif
//  } else {
#if SEXTIC_TWIST_BLS12383 == M_TYPE
    FP2_BLS12383_mul(&w1, &(w->a).a, &(y->a).a); // A1.A2
    FP2_BLS12383_mul(&w2, &(w->a).b, &(y->a).b); // B1.B2
    if (y->type == FP_SPARSEST || w->type == FP_SPARSEST)
    {
        if (y->type == FP_SPARSEST && w->type == FP_SPARSEST)
        {
            FP_BLS12383_mul(&w3.a, &(w->c).b.a, &(y->c).b.a);
            FP_BLS12383_zero(&w3.b);
        } else {
            if (y->type != FP_SPARSEST)
                FP2_BLS12383_pmul(&w3, &(y->c).b, &(w->c).b.a);
            if (w->type != FP_SPARSEST)
                FP2_BLS12383_pmul(&w3, &(w->c).b, &(y->c).b.a);
        }
    }
    else
        FP2_BLS12383_mul(&w3, &(w->c).b, &(y->c).b); // F1.F2

    FP2_BLS12383_add(&ta, &(w->a).a, &(w->a).b); // A1+B1
    FP2_BLS12383_add(&tb, &(y->a).a, &(y->a).b); // A2+B2
    FP2_BLS12383_norm(&ta);
    FP2_BLS12383_norm(&tb);
    FP2_BLS12383_mul(&tc, &ta, &tb);         // (A1+B1)(A2+B2)
    FP2_BLS12383_add(&t, &w1, &w2);
    FP2_BLS12383_neg(&t, &t);
    FP2_BLS12383_add(&tc, &tc, &t);          // (A1+B1)(A2+B2)-A1.A2-B1*B2 =  (A1.B2+A2.B1)

    FP2_BLS12383_add(&ta, &(w->a).a, &(w->c).b); // A1+F1
    FP2_BLS12383_add(&tb, &(y->a).a, &(y->c).b); // A2+F2
    FP2_BLS12383_norm(&ta);
    FP2_BLS12383_norm(&tb);
    FP2_BLS12383_mul(&td, &ta, &tb);         // (A1+F1)(A2+F2)
    FP2_BLS12383_add(&t, &w1, &w3);
    FP2_BLS12383_neg(&t, &t);
    FP2_BLS12383_add(&td, &td, &t);          // (A1+F1)(A2+F2)-A1.A2-F1*F2 =  (A1.F2+A2.F1)

    FP2_BLS12383_add(&ta, &(w->a).b, &(w->c).b); // B1+F1
    FP2_BLS12383_add(&tb, &(y->a).b, &(y->c).b); // B2+F2
    FP2_BLS12383_norm(&ta);
    FP2_BLS12383_norm(&tb);
    FP2_BLS12383_mul(&te, &ta, &tb);         // (B1+F1)(B2+F2)
    FP2_BLS12383_add(&t, &w2, &w3);
    FP2_BLS12383_neg(&t, &t);
    FP2_BLS12383_add(&te, &te, &t);          // (B1+F1)(B2+F2)-B1.B2-F1*F2 =  (B1.F2+B2.F1)

    FP2_BLS12383_mul_ip(&w2);
    FP2_BLS12383_add(&w1, &w1, &w2);
    FP4_BLS12383_from_FP2s(&(w->a), &w1, &tc);

    FP2_BLS12383_mul_ip(&w3);
    FP2_BLS12383_norm(&w3);
    FP4_BLS12383_from_FP2H(&(w->b), &w3);

    FP2_BLS12383_norm(&te);
    FP2_BLS12383_mul_ip(&te);
    FP4_BLS12383_from_FP2s(&(w->c), &te, &td);

    FP4_BLS12383_norm(&(w->a));
    FP4_BLS12383_norm(&(w->c));
#endif

//  }
    w->type = FP_SPARSE;
}

/* Set w=1/x */
/* SU= 600 */
void FP12_BLS12383_inv(FP12_BLS12383 *w, FP12_BLS12383 *x)
{
    FP4_BLS12383 f0, f1, f2, f3;

    FP4_BLS12383_sqr(&f0, &(x->a));
    FP4_BLS12383_mul(&f1, &(x->b), &(x->c));
    FP4_BLS12383_times_i(&f1);
    FP4_BLS12383_sub(&f0, &f0, &f1); /* y.a */
    FP4_BLS12383_norm(&f0);

    FP4_BLS12383_sqr(&f1, &(x->c));
    FP4_BLS12383_times_i(&f1);
    FP4_BLS12383_mul(&f2, &(x->a), &(x->b));
    FP4_BLS12383_sub(&f1, &f1, &f2); /* y.b */
    FP4_BLS12383_norm(&f1);

    FP4_BLS12383_sqr(&f2, &(x->b));
    FP4_BLS12383_mul(&f3, &(x->a), &(x->c));
    FP4_BLS12383_sub(&f2, &f2, &f3); /* y.c */
    FP4_BLS12383_norm(&f2);

    FP4_BLS12383_mul(&f3, &(x->b), &f2);
    FP4_BLS12383_times_i(&f3);
    FP4_BLS12383_mul(&(w->a), &f0, &(x->a));
    FP4_BLS12383_add(&f3, &(w->a), &f3);
    FP4_BLS12383_mul(&(w->c), &f1, &(x->c));
    FP4_BLS12383_times_i(&(w->c));

    FP4_BLS12383_add(&f3, &(w->c), &f3);
    FP4_BLS12383_norm(&f3);

    FP4_BLS12383_inv(&f3, &f3, NULL);

    FP4_BLS12383_mul(&(w->a), &f0, &f3);
    FP4_BLS12383_mul(&(w->b), &f1, &f3);
    FP4_BLS12383_mul(&(w->c), &f2, &f3);
    w->type = FP_DENSE;
}

/* constant time powering by small integer of max length bts */

void FP12_BLS12383_pinpow(FP12_BLS12383 *r, int e, int bts)
{
    int i, b;
    FP12_BLS12383 R[2];

    FP12_BLS12383_one(&R[0]);
    FP12_BLS12383_copy(&R[1], r);

    for (i = bts - 1; i >= 0; i--)
    {
        b = (e >> i) & 1;
        FP12_BLS12383_mul(&R[1 - b], &R[b]);
        FP12_BLS12383_usqr(&R[b], &R[b]);
    }
    FP12_BLS12383_copy(r, &R[0]);
}

/* Compressed powering of unitary elements y=x^(e mod r) */

void FP12_BLS12383_compow(FP4_BLS12383 *c, FP12_BLS12383 *x, BIG_384_58 e, BIG_384_58 r)
{
    FP12_BLS12383 g1, g2;
    FP4_BLS12383 cp, cpm1, cpm2;
    FP2_BLS12383 f;
    BIG_384_58 q, a, b, m;

    BIG_384_58_rcopy(a, Fra_BLS12383);
    BIG_384_58_rcopy(b, Frb_BLS12383);
    FP2_BLS12383_from_BIGs(&f, a, b);

    BIG_384_58_rcopy(q, Modulus_BLS12383);

    FP12_BLS12383_copy(&g1, x);
    FP12_BLS12383_copy(&g2, x);

    BIG_384_58_copy(m, q);
    BIG_384_58_mod(m, r);

    BIG_384_58_copy(a, e);
    BIG_384_58_mod(a, m);

    BIG_384_58_copy(b, e);
    BIG_384_58_sdiv(b, m);

    FP12_BLS12383_trace(c, &g1);

    if (BIG_384_58_iszilch(b))
    {
        FP4_BLS12383_xtr_pow(c, c, e);
        return;
    }

    FP12_BLS12383_frob(&g2, &f);
    FP12_BLS12383_trace(&cp, &g2);

    FP12_BLS12383_conj(&g1, &g1);
    FP12_BLS12383_mul(&g2, &g1);
    FP12_BLS12383_trace(&cpm1, &g2);
    FP12_BLS12383_mul(&g2, &g1);
    FP12_BLS12383_trace(&cpm2, &g2);

    FP4_BLS12383_xtr_pow2(c, &cp, c, &cpm1, &cpm2, a, b);
}


/* SU= 528 */
/* set r=a^b */
/* Note this is simple square and multiply, so not side-channel safe */

void FP12_BLS12383_pow(FP12_BLS12383 *r, FP12_BLS12383 *a, BIG_384_58 b)
{
    FP12_BLS12383 w, sf;
    BIG_384_58 b1, b3;
    int i, nb, bt;
    BIG_384_58_copy(b1, b);
    BIG_384_58_norm(b1);
    BIG_384_58_pmul(b3, b1, 3);
    BIG_384_58_norm(b3);

    FP12_BLS12383_copy(&sf, a);
    FP12_BLS12383_norm(&sf);
    FP12_BLS12383_copy(&w, &sf);

    if (BIG_384_58_iszilch(b3))
    {
        FP12_BLS12383_one(r);
        return;
    }

    nb = BIG_384_58_nbits(b3);
    for (i = nb - 2; i >= 1; i--)
    {
        FP12_BLS12383_usqr(&w, &w);
        bt = BIG_384_58_bit(b3, i) - BIG_384_58_bit(b1, i);
        if (bt == 1)
            FP12_BLS12383_mul(&w, &sf);
        if (bt == -1)
        {
            FP12_BLS12383_conj(&sf, &sf);
            FP12_BLS12383_mul(&w, &sf);
            FP12_BLS12383_conj(&sf, &sf);
        }
    }

    FP12_BLS12383_copy(r, &w);
    FP12_BLS12383_reduce(r);
}

/* p=q0^u0.q1^u1.q2^u2.q3^u3 */
/* Side channel attack secure */
// Bos & Costello https://eprint.iacr.org/2013/458.pdf
// Faz-Hernandez & Longa & Sanchez  https://eprint.iacr.org/2013/158.pdf

void FP12_BLS12383_pow4(FP12_BLS12383 *p, FP12_BLS12383 *q, BIG_384_58 u[4])
{
    int i, j, k, nb, pb, bt;
    FP12_BLS12383 g[8], r;
    BIG_384_58 t[4], mt;
    sign8 w[NLEN_384_58 * BASEBITS_384_58 + 1];
    sign8 s[NLEN_384_58 * BASEBITS_384_58 + 1];

    for (i = 0; i < 4; i++)
        BIG_384_58_copy(t[i], u[i]);

    FP12_BLS12383_zero(&r);
// Precomputed table
    FP12_BLS12383_copy(&g[0], &q[0]); // q[0]
    FP12_BLS12383_copy(&g[1], &g[0]);
    FP12_BLS12383_mul(&g[1], &q[1]); // q[0].q[1]
    FP12_BLS12383_copy(&g[2], &g[0]);
    FP12_BLS12383_mul(&g[2], &q[2]); // q[0].q[2]
    FP12_BLS12383_copy(&g[3], &g[1]);
    FP12_BLS12383_mul(&g[3], &q[2]); // q[0].q[1].q[2]
    FP12_BLS12383_copy(&g[4], &g[0]);
    FP12_BLS12383_mul(&g[4], &q[3]); // q[0].q[3]
    FP12_BLS12383_copy(&g[5], &g[1]);
    FP12_BLS12383_mul(&g[5], &q[3]); // q[0].q[1].q[3]
    FP12_BLS12383_copy(&g[6], &g[2]);
    FP12_BLS12383_mul(&g[6], &q[3]); // q[0].q[2].q[3]
    FP12_BLS12383_copy(&g[7], &g[3]);
    FP12_BLS12383_mul(&g[7], &q[3]); // q[0].q[1].q[2].q[3]

// Make it odd
    pb = 1 - BIG_384_58_parity(t[0]);
    BIG_384_58_inc(t[0], pb);
    BIG_384_58_norm(t[0]);

// Number of bits
    BIG_384_58_zero(mt);
    for (i = 0; i < 4; i++)
    {
        BIG_384_58_or(mt, mt, t[i]);
    }
    nb = 1 + BIG_384_58_nbits(mt);

// Sign pivot
    s[nb - 1] = 1;
    for (i = 0; i < nb - 1; i++)
    {
        BIG_384_58_fshr(t[0], 1);
        s[i] = 2 * BIG_384_58_parity(t[0]) - 1;
    }

// Recoded exponent
    for (i = 0; i < nb; i++)
    {
        w[i] = 0;
        k = 1;
        for (j = 1; j < 4; j++)
        {
            bt = s[i] * BIG_384_58_parity(t[j]);
            BIG_384_58_fshr(t[j], 1);

            BIG_384_58_dec(t[j], (bt >> 1));
            BIG_384_58_norm(t[j]);
            w[i] += bt * k;
            k *= 2;
        }
    }

// Main loop
    FP12_BLS12383_select(p, g, 2 * w[nb - 1] + 1);
    for (i = nb - 2; i >= 0; i--)
    {
        FP12_BLS12383_select(&r, g, 2 * w[i] + s[i]);
        FP12_BLS12383_usqr(p, p);
        FP12_BLS12383_mul(p, &r);
    }
// apply correction
    FP12_BLS12383_conj(&r, &q[0]);
    FP12_BLS12383_mul(&r, p);
    FP12_BLS12383_cmove(p, &r, pb);

    FP12_BLS12383_reduce(p);
}

/* Set w=w^p using Frobenius */
/* SU= 160 */
void FP12_BLS12383_frob(FP12_BLS12383 *w, FP2_BLS12383 *f)
{
    FP2_BLS12383 f2, f3;
    FP2_BLS12383_sqr(&f2, f);    /* f2=f^2 */
    FP2_BLS12383_mul(&f3, &f2, f); /* f3=f^3 */

    FP4_BLS12383_frob(&(w->a), &f3);
    FP4_BLS12383_frob(&(w->b), &f3);
    FP4_BLS12383_frob(&(w->c), &f3);

    FP4_BLS12383_pmul(&(w->b), &(w->b), f);
    FP4_BLS12383_pmul(&(w->c), &(w->c), &f2);
    w->type = FP_DENSE;
}

/* SU= 8 */
/* normalise all components of w */
void FP12_BLS12383_norm(FP12_BLS12383 *w)
{
    FP4_BLS12383_norm(&(w->a));
    FP4_BLS12383_norm(&(w->b));
    FP4_BLS12383_norm(&(w->c));
}

/* SU= 8 */
/* reduce all components of w */
void FP12_BLS12383_reduce(FP12_BLS12383 *w)
{
    FP4_BLS12383_reduce(&(w->a));
    FP4_BLS12383_reduce(&(w->b));
    FP4_BLS12383_reduce(&(w->c));
}

/* trace function w=trace(x) */
/* SU= 8 */
void FP12_BLS12383_trace(FP4_BLS12383 *w, FP12_BLS12383 *x)
{
    FP4_BLS12383_imul(w, &(x->a), 3);
    FP4_BLS12383_reduce(w);
}

/* SU= 8 */
/* Output w in hex */
void FP12_BLS12383_output(FP12_BLS12383 *w)
{
    printf("[");
    FP4_BLS12383_output(&(w->a));
    printf(",");
    FP4_BLS12383_output(&(w->b));
    printf(",");
    FP4_BLS12383_output(&(w->c));
    printf("]");
}

/* SU= 64 */
/* Convert g to octet string w */
void FP12_BLS12383_toOctet(octet *W, FP12_BLS12383 *g)
{
    W->len = 12 * MODBYTES_384_58;
    FP4_BLS12383_toBytes(&(W->val[0]),&(g->c));
    FP4_BLS12383_toBytes(&(W->val[4 * MODBYTES_384_58]),&(g->b));
    FP4_BLS12383_toBytes(&(W->val[8 * MODBYTES_384_58]),&(g->a));
}

/* SU= 24 */
/* Restore g from octet string w */
void FP12_BLS12383_fromOctet(FP12_BLS12383 *g, octet *W)
{
    FP4_BLS12383_fromBytes(&(g->c),&(W->val[0]));
    FP4_BLS12383_fromBytes(&(g->b),&(W->val[4 * MODBYTES_384_58]));
    FP4_BLS12383_fromBytes(&(g->a),&(W->val[8 * MODBYTES_384_58]));
}

/* Move b to a if d=1 */
void FP12_BLS12383_cmove(FP12_BLS12383 *f, FP12_BLS12383 *g, int d)
{
    FP4_BLS12383_cmove(&(f->a), &(g->a), d);
    FP4_BLS12383_cmove(&(f->b), &(g->b), d);
    FP4_BLS12383_cmove(&(f->c), &(g->c), d);
    d = ~(d - 1);
    f->type ^= (f->type ^ g->type)&d;
}

