/* dlasq2.f -- translated by f2c (version 20061008) */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <assert.h>

/* Table of constant values */
static int c__1 = 1;
static int c__2 = 2;
static int c__10 = 10;
static int c__3 = 3;
static int c__4 = 4;
static int c__11 = 11;

/* Subroutine */ 
int odsq2(int *n, double *z__, int *info)
{
    /* System generated locals */
    int i__1, i__2, i__3;
    double d__1, d__2;

    /* Builtin functions */
    // double sqrt(double);

    /* Local variables */
    double d__, e, g;
    int k;
    double s, t;
    int i0, i4, n0;
    double dn;
    int pp;
    double dn1, dn2, dee, eps, tau, tol;
    int ipn4;
    double tol2;
    int ieee;
    int nbig;
    double dmin__, emin, emax;
    int kmin, ndiv, iter;
    double qmin, temp, qmax, zmax;
    int splt;
    double dmin1, dmin2;
    int nfail;
    double desig, trace, sigma;
    int iinfo, ttype;
    extern /* Subroutine */ int odsq3(int *, int *, double *, 
	    int *, double *, double *, double *, double *, 
	     int *, int *, int *, int *, int *, 
	    double *, double *, double *, double *, 
	    double *, double *, double *);
    // extern double odmch(char *);
    double deemin;
    int iwhila, iwhilb;
    double oldemn, safmin;
    extern /* Subroutine */ int oerbla(char *, int *);
    /* extern int oienv(int *, char *, char *, int *, int *,  */
    /* 	    int *, int *); */
    extern /* Subroutine */ int odsrt(char *, int *, double *, 
	    int *);


/*  -- LAPACK routine (version 3.2)                                    -- */

/*  -- Contributed by Osni Marques of the Lawrence Berkeley National   -- */
/*  -- Laboratory and Beresford Parlett of the Univ. of California at  -- */
/*  -- Berkeley                                                        -- */
/*  -- November 2008                                                   -- */

/*  -- LAPACK is a software package provided by Univ. of Tennessee,    -- */
/*  -- Univ. of California Berkeley, Univ. of Colorado Denver and NAG Ltd..-- */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ODSQ2 computes all the eigenvalues of the symmetric positive */
/*  definite tridiagonal matrix associated with the qd array Z to high */
/*  relative accuracy are computed to high relative accuracy, in the */
/*  absence of denormalization, underflow and overflow. */

/*  To see the relation of Z to the tridiagonal matrix, let L be a */
/*  unit lower bidiagonal matrix with subdiagonals Z(2,4,6,,..) and */
/*  let U be an upper bidiagonal matrix with 1's above and diagonal */
/*  Z(1,3,5,,..). The tridiagonal is L*U or, if you prefer, the */
/*  symmetric tridiagonal to which it is similar. */

/*  Note : ODSQ2 defines a int variable, IEEE, which is true */
/*  on machines which follow ieee-754 floating-point standard in their */
/*  handling of infinities and NaNs, and false otherwise. This variable */
/*  is passed to ODSQ3. */

/*  Arguments */
/*  ========= */

/*  N     (input) INT */
/*        The number of rows and columns in the matrix. N >= 0. */

/*  Z     (input/output) DOUBLE PRECISION array, dimension ( 4*N ) */
/*        On entry Z holds the qd array. On exit, entries 1 to N hold */
/*        the eigenvalues in decreasing order, Z( 2*N+1 ) holds the */
/*        trace, and Z( 2*N+2 ) holds the sum of the eigenvalues. If */
/*        N > 2, then Z( 2*N+3 ) holds the iteration count, Z( 2*N+4 ) */
/*        holds NDIVS/NIN^2, and Z( 2*N+5 ) holds the percentage of */
/*        shifts that failed. */

/*  INFO  (output) INT */
/*        = 0: successful exit */
/*        < 0: if the i-th argument is a scalar and had an illegal */
/*             value, then INFO = -i, if the i-th argument is an */
/*             array and the j-entry had an illegal value, then */
/*             INFO = -(i*100+j) */
/*        > 0: the algorithm failed */
/*              = 1, a split was marked by a positive value in E */
/*              = 2, current block of Z not diagonalized after 30*N */
/*                   iterations (in inner while loop) */
/*              = 3, termination criterion of outer while loop not met */
/*                   (program created more than N unreduced blocks) */

/*  Further Details */
/*  =============== */
/*  Local Variables: I0:N0 defines a current unreduced segment of Z. */
/*  The shifts are accumulated in SIGMA. Iteration count is in ITER. */
/*  Ping-pong is controlled by PP (alternates between 0 and 1). */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input arguments. */
/*     (in case ODSQ2 is not called by DLASQ1) */

    /* Parameter adjustments */
    --z__;

    /* Function Body */
    *info = 0;
    eps = DBL_EPSILON; // odmch("Precision");
    safmin = DBL_MIN; // odmch("Safe minimum");
    tol = eps * 100.;
/* Computing 2nd power */
    d__1 = tol;
    tol2 = d__1 * d__1;

    if (*n < 0) {
	*info = -1;
	oerbla("ODSQ2", &c__1);
	return 0;
    } else if (*n == 0) {
	return 0;
    } else if (*n == 1) {

/*        1-by-1 case. */

	if (z__[1] < 0.) {
	    *info = -201;
	    oerbla("ODSQ2", &c__2);
	}
	return 0;
    } else if (*n == 2) {

/*        2-by-2 case. */

	if (z__[2] < 0. || z__[3] < 0.) {
	    *info = -2;
	    oerbla("ODSQ2", &c__2);
	    return 0;
	} else if (z__[3] > z__[1]) {
	    d__ = z__[3];
	    z__[3] = z__[1];
	    z__[1] = d__;
	}
	z__[5] = z__[1] + z__[2] + z__[3];
	if (z__[2] > z__[3] * tol2) {
	    t = (z__[1] - z__[3] + z__[2]) * .5;
	    s = z__[3] * (z__[2] / t);
	    if (s <= t) {
		s = z__[3] * (z__[2] / (t * (sqrt(s / t + 1.) + 1.)));
	    } else {
		s = z__[3] * (z__[2] / (t + sqrt(t) * sqrt(t + s)));
	    }
	    t = z__[1] + (s + z__[2]);
	    z__[3] *= z__[1] / t;
	    z__[1] = t;
	}
	z__[2] = z__[3];
	z__[6] = z__[2] + z__[1];
	return 0;
    }

/*     Check for negative data and compute sums of q's and e's. */

    z__[*n * 2] = 0.;
    emin = z__[2];
    qmax = 0.;
    zmax = 0.;
    d__ = 0.;
    e = 0.;

    i__1 = *n - 1 << 1;
    for (k = 1; k <= i__1; k += 2) {
	if (z__[k] < 0.) {
	    *info = -(k + 200);
	    oerbla("ODSQ2", &c__2);
	    return 0;
	} else if (z__[k + 1] < 0.) {
	    *info = -(k + 201);
	    oerbla("ODSQ2", &c__2);
	    return 0;
	}
	d__ += z__[k];
	e += z__[k + 1];
/* Computing MAX */
	d__1 = qmax, d__2 = z__[k];
	qmax = fmax(d__1,d__2);
/* Computing MIN */
	d__1 = emin, d__2 = z__[k + 1];
	emin = fmin(d__1,d__2);
/* Computing MAX */
	d__1 = fmax(qmax,zmax), d__2 = z__[k + 1];
	zmax = fmax(d__1,d__2);
/* L10: */
    }
    if (z__[(*n << 1) - 1] < 0.) {
	*info = -((*n << 1) + 199);
	oerbla("ODSQ2", &c__2);
	return 0;
    }
    d__ += z__[(*n << 1) - 1];
/* Computing MAX */
    d__1 = qmax, d__2 = z__[(*n << 1) - 1];
    qmax = fmax(d__1,d__2);
    zmax = fmax(qmax,zmax);

/*     Check for diagonality. */

    if (e == 0.) {
	i__1 = *n;
	for (k = 2; k <= i__1; ++k) {
	    z__[k] = z__[(k << 1) - 1];
/* L20: */
	}
	odsrt("D", n, &z__[1], &iinfo);
	z__[(*n << 1) - 1] = d__;
	return 0;
    }

    trace = d__ + e;

/*     Check for zero data. */

    if (trace == 0.) {
	z__[(*n << 1) - 1] = 0.;
	return 0;
    }

/*     Check whether the machine is IEEE conformable. */
    /* ieee = oienv(&c__10, "ODSQ2", "N", &c__1, &c__2, &c__3, &c__4) == 1 && oienv(&c__11, "ODSQ2", "N", &c__1, &c__2,  */
    /* 	     &c__3, &c__4) == 1; */
    ieee = 1;

/*     Rearrange data for locality: Z=(q1,qq1,e1,ee1,q2,qq2,e2,ee2,...). */

    for (k = *n << 1; k >= 2; k += -2) {
	z__[k * 2] = 0.;
	z__[(k << 1) - 1] = z__[k];
	z__[(k << 1) - 2] = 0.;
	z__[(k << 1) - 3] = z__[k - 1];
/* L30: */
    }

    i0 = 1;
    n0 = *n;

/*     Reverse the qd-array, if warranted. */

    if (z__[(i0 << 2) - 3] * 1.5 < z__[(n0 << 2) - 3]) {
	ipn4 = i0 + n0 << 2;
	i__1 = i0 + n0 - 1 << 1;
	for (i4 = i0 << 2; i4 <= i__1; i4 += 4) {
	    temp = z__[i4 - 3];
	    z__[i4 - 3] = z__[ipn4 - i4 - 3];
	    z__[ipn4 - i4 - 3] = temp;
	    temp = z__[i4 - 1];
	    z__[i4 - 1] = z__[ipn4 - i4 - 5];
	    z__[ipn4 - i4 - 5] = temp;
/* L40: */
	}
    }

/*     Initial split checking via dqd and Li's test. */

    pp = 0;

    for (k = 1; k <= 2; ++k) {

	d__ = z__[(n0 << 2) + pp - 3];
	i__1 = (i0 << 2) + pp;
	for (i4 = (n0 - 1 << 2) + pp; i4 >= i__1; i4 += -4) {
	    if (z__[i4 - 1] <= tol2 * d__) {
		z__[i4 - 1] = -0.;
		d__ = z__[i4 - 3];
	    } else {
		d__ = z__[i4 - 3] * (d__ / (d__ + z__[i4 - 1]));
	    }
/* L50: */
	}

/*        dqd maps Z to ZZ plus Li's test. */

	emin = z__[(i0 << 2) + pp + 1];
	d__ = z__[(i0 << 2) + pp - 3];
	i__1 = (n0 - 1 << 2) + pp;
	for (i4 = (i0 << 2) + pp; i4 <= i__1; i4 += 4) {
	    z__[i4 - (pp << 1) - 2] = d__ + z__[i4 - 1];
	    if (z__[i4 - 1] <= tol2 * d__) {
		z__[i4 - 1] = -0.;
		z__[i4 - (pp << 1) - 2] = d__;
		z__[i4 - (pp << 1)] = 0.;
		d__ = z__[i4 + 1];
	    } else if (safmin * z__[i4 + 1] < z__[i4 - (pp << 1) - 2] && 
		    safmin * z__[i4 - (pp << 1) - 2] < z__[i4 + 1]) {
		temp = z__[i4 + 1] / z__[i4 - (pp << 1) - 2];
		z__[i4 - (pp << 1)] = z__[i4 - 1] * temp;
		d__ *= temp;
	    } else {
		z__[i4 - (pp << 1)] = z__[i4 + 1] * (z__[i4 - 1] / z__[i4 - (
			pp << 1) - 2]);
		d__ = z__[i4 + 1] * (d__ / z__[i4 - (pp << 1) - 2]);
	    }
/* Computing MIN */
	    d__1 = emin, d__2 = z__[i4 - (pp << 1)];
	    emin = fmin(d__1,d__2);
/* L60: */
	}
	z__[(n0 << 2) - pp - 2] = d__;

/*        Now find qmax. */

	qmax = z__[(i0 << 2) - pp - 2];
	i__1 = (n0 << 2) - pp - 2;
	for (i4 = (i0 << 2) - pp + 2; i4 <= i__1; i4 += 4) {
/* Computing MAX */
	    d__1 = qmax, d__2 = z__[i4];
	    qmax = fmax(d__1,d__2);
/* L70: */
	}

/*        Prepare for the next iteration on K. */

	pp = 1 - pp;
/* L80: */
    }

/*     Initialise variables to pass to ODSQ3. */

    ttype = 0;
    dmin1 = 0.;
    dmin2 = 0.;
    dn = 0.;
    dn1 = 0.;
    dn2 = 0.;
    g = 0.;
    tau = 0.;

    iter = 2;
    nfail = 0;
    ndiv = n0 - i0 << 1;

    i__1 = *n + 1;
    for (iwhila = 1; iwhila <= i__1; ++iwhila) {
	if (n0 < 1) {
	    goto L170;
	}

/*        While array unfinished do */

/*        E(N0) holds the value of SIGMA when submatrix in I0:N0 */
/*        splits from the rest of the array, but is negated. */

	desig = 0.;
	if (n0 == *n) {
	    sigma = 0.;
	} else {
	    sigma = -z__[(n0 << 2) - 1];
	}
	if (sigma < 0.) {
	    *info = 1;
	    return 0;
	}

/*        Find last unreduced submatrix's top index I0, find QMAX and */
/*        EMIN. Find Gershgorin-type bound if Q's much greater than E's. */

	emax = 0.;
	if (n0 > i0) {
	    emin = (d__1 = z__[(n0 << 2) - 5], fabs(d__1));
	} else {
	    emin = 0.;
	}
	qmin = z__[(n0 << 2) - 3];
	qmax = qmin;
	for (i4 = n0 << 2; i4 >= 8; i4 += -4) {
	    if (z__[i4 - 5] <= 0.) {
		goto L100;
	    }
	    if (qmin >= emax * 4.) {
/* Computing MIN */
		d__1 = qmin, d__2 = z__[i4 - 3];
		qmin = fmin(d__1,d__2);
/* Computing MAX */
		d__1 = emax, d__2 = z__[i4 - 5];
		emax = fmax(d__1,d__2);
	    }
/* Computing MAX */
	    d__1 = qmax, d__2 = z__[i4 - 7] + z__[i4 - 5];
	    qmax = fmax(d__1,d__2);
/* Computing MIN */
	    d__1 = emin, d__2 = z__[i4 - 5];
	    emin = fmin(d__1,d__2);
/* L90: */
	}
	i4 = 4;

L100:
	i0 = i4 / 4;
	pp = 0;

	if (n0 - i0 > 1) {
	    dee = z__[(i0 << 2) - 3];
	    deemin = dee;
	    kmin = i0;
	    i__2 = (n0 << 2) - 3;
	    for (i4 = (i0 << 2) + 1; i4 <= i__2; i4 += 4) {
		dee = z__[i4] * (dee / (dee + z__[i4 - 2]));
		if (dee <= deemin) {
		    deemin = dee;
		    kmin = (i4 + 3) / 4;
		}
/* L110: */
	    }
	    if (kmin - i0 << 1 < n0 - kmin && deemin <= z__[(n0 << 2) - 3] * 
		    .5) {
		ipn4 = i0 + n0 << 2;
		pp = 2;
		i__2 = i0 + n0 - 1 << 1;
		for (i4 = i0 << 2; i4 <= i__2; i4 += 4) {
		    temp = z__[i4 - 3];
		    z__[i4 - 3] = z__[ipn4 - i4 - 3];
		    z__[ipn4 - i4 - 3] = temp;
		    temp = z__[i4 - 2];
		    z__[i4 - 2] = z__[ipn4 - i4 - 2];
		    z__[ipn4 - i4 - 2] = temp;
		    temp = z__[i4 - 1];
		    z__[i4 - 1] = z__[ipn4 - i4 - 5];
		    z__[ipn4 - i4 - 5] = temp;
		    temp = z__[i4];
		    z__[i4] = z__[ipn4 - i4 - 4];
		    z__[ipn4 - i4 - 4] = temp;
/* L120: */
		}
	    }
	}

/*        Put -(initial shift) into DMIN. */

/* Computing MAX */
	d__1 = 0., d__2 = qmin - sqrt(qmin) * 2. * sqrt(emax);
	dmin__ = -fmax(d__1,d__2);

/*        Now I0:N0 is unreduced. */
/*        PP = 0 for ping, PP = 1 for pong. */
/*        PP = 2 indicates that flipping was applied to the Z array and */
/*               and that the tests for deflation upon entry in ODSQ3 */
/*               should not be performed. */

	nbig = (n0 - i0 + 1) * 100;
	i__2 = nbig;
	for (iwhilb = 1; iwhilb <= i__2; ++iwhilb) {
	    if (i0 > n0) {
		goto L150;
	    }

/*           While submatrix unfinished take a good dqds step. */

	    odsq3(&i0, &n0, &z__[1], &pp, &dmin__, &sigma, &desig, &qmax, &
		    nfail, &iter, &ndiv, &ieee, &ttype, &dmin1, &dmin2, &dn, &
		    dn1, &dn2, &g, &tau);

	    pp = 1 - pp;

/*           When EMIN is very small check for splits. */

	    if (pp == 0 && n0 - i0 >= 3) {
		if (z__[n0 * 4] <= tol2 * qmax || z__[(n0 << 2) - 1] <= tol2 *
			 sigma) {
		    splt = i0 - 1;
		    qmax = z__[(i0 << 2) - 3];
		    emin = z__[(i0 << 2) - 1];
		    oldemn = z__[i0 * 4];
		    i__3 = n0 - 3 << 2;
		    for (i4 = i0 << 2; i4 <= i__3; i4 += 4) {
			if (z__[i4] <= tol2 * z__[i4 - 3] || z__[i4 - 1] <= 
				tol2 * sigma) {
			    z__[i4 - 1] = -sigma;
			    splt = i4 / 4;
			    qmax = 0.;
			    emin = z__[i4 + 3];
			    oldemn = z__[i4 + 4];
			} else {
/* Computing MAX */
			    d__1 = qmax, d__2 = z__[i4 + 1];
			    qmax = fmax(d__1,d__2);
/* Computing MIN */
			    d__1 = emin, d__2 = z__[i4 - 1];
			    emin = fmin(d__1,d__2);
/* Computing MIN */
			    d__1 = oldemn, d__2 = z__[i4];
			    oldemn = fmin(d__1,d__2);
			}
/* L130: */
		    }
		    z__[(n0 << 2) - 1] = emin;
		    z__[n0 * 4] = oldemn;
		    i0 = splt + 1;
		}
	    }

/* L140: */
	}

	*info = 2;
	return 0;

/*        end IWHILB */

L150:

/* L160: */
	;
    }

    *info = 3;
    return 0;

/*     end IWHILA */

L170:

/*     Move q's to the front. */
    i__1 = *n;
    for (k = 2; k <= i__1; ++k) {
	z__[k] = z__[(k << 2) - 3];
/* L180: */
    }

/*     Sort and compute sum of eigenvalues. */
    odsrt("D", n, &z__[1], &iinfo);

    e = 0.;
    for (k = *n; k >= 1; --k) {
	e += z__[k];
/* L190: */
    }

/*     Store trace, sum(eigenvalues) and information on performance. */
    z__[(*n << 1) + 1] = trace;
    z__[(*n << 1) + 2] = e;
    z__[(*n << 1) + 3] = (double) iter;
/* Computing 2nd power */
    i__1 = *n;
    z__[(*n << 1) + 4] = (double) ndiv / (double) (i__1 * i__1);
    z__[(*n << 1) + 5] = nfail * 100. / (double) iter;
    return 0;

/*     End of ODSQ2 */

} /* odsq2_ */
