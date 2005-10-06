/* Siconos version 1.0, Copyright INRIA 2005.
 * Siconos is a program dedicated to modeling, simulation and control
 * of non smooth dynamical systems.
 * Siconos is a free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * Siconos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Siconos; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Contact: Vincent ACARY vincent.acary@inrialpes.fr
*/
/*!\file lcp_latin.c
 *
 * This subroutine allows the resolution of LCP (Linear Complementary Problem).\n
 * Try \f$(z,w)\f$ such that:\n
 * \f$
 *  \left\lbrace
 *   \begin{array}{l}
 *    M z + q= w\\
 *    0 \le z \perp w \ge 0\\
 *   \end{array}
 *  \right.
 * \f$
 *
 * where M is an (n x n)-matrix, q , w and z n-vectors.
 *
 * \fn  lcp_latin( int *nn , double *vec , double *q , double *z , int *info ,
 *                int *iparamLCP , double *dparamLCP )
 *
 * lcp_latin (LArge Time INcrements) is a solver for LCP based on the principle of splitting method\n
 *
 * Generic lcp parameters:\n
 *
 * \param nn      Unchanged parameter which represents the dimension of the system.
 * \param vec     Unchanged parameter which contains the components of the matrix with a fortran storage.
 * \param q       Unchanged parameter which contains the components of the right hand side vector.
 * \param z       Modified parameter which contains the initial solution and returns the solution of the problem.
 * \param w       Modified parameter which returns the solution of the problem.
 * \param info    Modified parameter which returns the termination value\n
 *                0 - convergence\n
 *                1 - iter = itermax\n
 *                2 - Cholesky Factorization failed \n
 *                3 - nul diagonal term\n
 *
 * Specific LATIN parameters:\n
 *
 * \param iparamLCP[0] = itermax  Input unchanged parameter which represents the maximum number of iterations allowed.
 * \param iparamLCP[1] = iout     Input unchanged parameter which represents the output log identifiant\n
 *                       0 - no output\n
 *                       0 < active screen output\n
 * \param iparamLCP[2] = it_end   Output modified parameter which returns the number of iterations performed by the algorithm.
 *
 * \param dparamLCP[0] = tol      Input unchanged parameter which represents the tolerance required.
 * \param dparamLCP[1] = k_latin  Input unchanged parameter which represents the latin parameter.
 * \param dparamLCP[2] = res      Output modified parameter which returns the final error value.
 * \param dparamLCP[3] = res      Input unchanged parameter which represents the relax coefficient.
 *
 * \author Nineb Sheherazade.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "blaslapack.h"




void lcp_latin_w(int *nn, double *vec, double *qq,  double *z, double *w, int *info, int *iparamLCP, double *dparamLCP)
{


  int      i, j,  iter1, info2, nrhs, iout;
  int      n, incx, incy, n2;
  int      itermax, itt, it_end;


  double   alpha, beta;
  double   err1, num11, err0;
  double   k_latin, res, tol, errmax;
  double   den11, den22, omega;
  double   *wc, *zc, *kinvden1, *kinvden2, *wt;
  double   *maxwt, *wnum1, *znum1, *ww, *zz;
  double   *num1, *kinvnum1, *den1, *den2, *wden1, *zden1;
  double   *kinvwden1, *kzden1;
  double   *k, *kinv, *DPO;
  double   *zn, *wn;

  char trans = 'T', notrans = 'N', uplo = 'U', diag = 'N';



  n = *nn;
  n2 = n * n;
  incx = 1;
  incy = 1;

  /* Recup input */

  itermax = iparamLCP[0];
  iout    = iparamLCP[1];

  k_latin = dparamLCP[1];
  tol     = dparamLCP[0];
  omega   = dparamLCP[3];


  errmax = tol;
  itt = itermax;


  /* Initialize output */

  iparamLCP[2] = 0;
  dparamLCP[2] = 0.0;

  /* Allocations */

  ww        = (double*) malloc(n * sizeof(double));
  zz        = (double*) malloc(n * sizeof(double));
  wc        = (double*) malloc(n * sizeof(double));
  zc        = (double*) malloc(n * sizeof(double));
  znum1     = (double*) malloc(n * sizeof(double));
  wnum1     = (double*) malloc(n * sizeof(double));
  kinvden1  = (double*) malloc(n * sizeof(double));
  kinvden2  = (double*) malloc(n * sizeof(double));
  wt        = (double*) malloc(n * sizeof(double));
  maxwt     = (double*) malloc(n * sizeof(double));
  num1      = (double*) malloc(n * sizeof(double));
  kinvnum1  = (double*) malloc(n * sizeof(double));
  den1      = (double*) malloc(n * sizeof(double));
  den2      = (double*) malloc(n * sizeof(double));
  wden1     = (double*) malloc(n * sizeof(double));
  zden1     = (double*) malloc(n * sizeof(double));
  kinvwden1 = (double*) malloc(n * sizeof(double));
  kzden1    = (double*) malloc(n * sizeof(double));
  zn        = (double*) malloc(n * sizeof(double));
  wn        = (double*) malloc(n * sizeof(double));


  DPO       = (double*) malloc(n2 * sizeof(double));
  k         = (double*) malloc(n2 * sizeof(double));
  kinv      = (double*) malloc(n2 * sizeof(double));




  /* Initialization */

  for (i = 0; i < n2; i++)
  {


    if (i < n)
    {

      wc[i]       = 0.0;
      zc[i]       = 0.0;
      z[i]        = 0.0;
      zn[i]       = 0.0;
      wn[i]       = 0.0;
      w[i]        = 0.0;
      znum1[i]    = 0.0;
      wnum1[i]    = 0.0;
      kinvden1[i] = 0.0;
      kinvden2[i] = 0.0;
      wt[i]       = 0.0;
      maxwt[i]    = 0.0;
      num1[i]     = 0.0;
      kinvnum1[i] = 0.0;
      den1[i]     = 0.0;
      den2[i]     = 0.0;
    }

    k[i]          = 0.0;
    kinv[i]       = 0.0;
    DPO[i]        = 0.0;


  }





  for (i = 0 ; i < n ; i++)
  {

    k[i * n + i] =  k_latin * vec[i * n + i];

    if (fabs(k[i * n + i]) < 1e-16)
    {

      if (iout > 0)
      {
        printf(" Warning nul diagonal term in k matrix \n");
      }

      free(ww);

      free(zz);
      free(wn);
      free(zn);
      free(wc);
      free(zc);
      free(znum1);
      free(wnum1);
      free(kinvden1);
      free(kinvden2);
      free(wt);
      free(maxwt);
      free(num1);
      free(kinvnum1);
      free(den1);
      free(den2);
      free(wden1);
      free(zden1);
      free(kinvwden1);
      free(kzden1);
      free(DPO);
      free(k);
      free(kinv);

      *info = 3;

      return;

    }
    else

      kinv[i + n * i] = 1.0 / k[i + n * i];

  }



  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      DPO[i + n * j] = vec[j * n + i] + k[i + n * j];







  /*            Cholesky              */


  dpotrf_(&uplo, &n, DPO , &n, &info2);


  if (info2 != 0)
  {
    printf(" Matter with Cholesky Factorization \n ");

    free(ww);
    free(zz);
    free(wc);
    free(zc);
    free(znum1);
    free(wnum1);
    free(kinvden1);
    free(kinvden2);
    free(wt);
    free(maxwt);
    free(num1);
    free(kinvnum1);
    free(den1);
    free(den2);
    free(wden1);
    free(zden1);
    free(kinvwden1);
    free(kzden1);
    free(DPO);
    free(k);
    free(kinv);
    free(zn);
    free(wn);

    *info = 2;
    return;
  }

  /*            End of Cholesky          */




  /*            Iteration loops  */

  iter1 = 0;
  err1 = 1.;


  while ((iter1 < itt) && (err1 > errmax))
  {

    /*        Linear stage (zc,wc) -> (z,w)*/


    alpha = 1.;
    beta  = 1.;
    dgemv_(&trans, &n, &n, &alpha, k, &n, zc, &incx, &beta, wc, &incy);


    dcopy_(&n, qq, &incx, znum1, &incy);


    alpha = -1.;
    dscal_(&n , &alpha , znum1 , &incx);

    alpha = 1.;
    daxpy_(&n, &alpha, wc, &incx, znum1, &incy);
    nrhs = 1;


    dtrtrs_(&uplo, &trans, &diag, &n, &nrhs, DPO, &n, znum1, &n, &info2);

    dtrtrs_(&uplo, &notrans, &diag, &n, &nrhs, DPO, &n, znum1, &n, &info2);

    dcopy_(&n, znum1, &incx, z, &incy);




    alpha = -1.;
    beta = 1.;
    dgemv_(&trans, &n, &n, &alpha, k, &n, z, &incx, &beta, wc, &incy);

    dcopy_(&n, wc, &incx, w, &incy);

    alpha = omega;
    dscal_(&n , &alpha , z , &incx);

    alpha = 1.0 - omega;
    daxpy_(&n, &alpha, zn, &incx, z, &incy);


    alpha = omega;
    dscal_(&n , &alpha , w , &incx);

    alpha = 1.0 - omega;
    daxpy_(&n, &alpha, wn, &incx, w, &incy);


    dcopy_(&n, w, &incx, wn, &incy);
    dcopy_(&n, z, &incx, zn, &incy);



    /*         Local Stage                  */

    dcopy_(&n, w, &incx, wt, &incy);
    alpha = -1.;
    beta = 1.;
    dgemv_(&trans, &n, &n, &alpha, k, &n, z, &incx, &beta, wt, &incy);




    for (i = 0; i < n; i++)
    {
      if (wt[i] > 0.0)
      {
        wc[i] = wt[i];
        zc[i] = 0.0;
      }
      else
      {
        wc[i] = 0.0;
        zc[i] =  -wt[i] / k[i + n * i];
      }
    }



    /*         Convergence criterium                */


    dcopy_(&n, w, &incx, wnum1, &incy);
    alpha = -1.;
    daxpy_(&n, &alpha, wc, &incx, wnum1, &incy);


    dcopy_(&n, z, &incx, znum1, &incy);
    daxpy_(&n, &alpha, zc, &incx, znum1, &incy);



    alpha = 1.;
    beta = 1.;
    dgemv_(&trans, &n, &n, &alpha, k, &n, znum1, &incx, &beta, wnum1, &incy);


    /*    wnum1(:) =(w(:)-wc(:))+matmul( k(:,:),(z(:)-zc(:)))   */



    alpha = 1.;
    beta = 0.;
    dgemv_(&trans, &n, &n, &alpha, kinv, &n, wnum1, &incx, &beta, kinvnum1, &incy);


    num11 = ddot_(&n, wnum1, &incx, kinvnum1, &incy);




    dcopy_(&n, z, &incx, zz, &incy);
    dcopy_(&n, w, &incx, ww, &incy);

    alpha = 1.;
    daxpy_(&n, &alpha, wc, &incx, ww, &incy);

    daxpy_(&n, &alpha, zc, &incx, zz, &incy);

    beta = 0.;
    alpha = 1.;
    dgemv_(&trans, &n, &n, &alpha, k, &n, zz, &incx, &beta, kzden1, &incy);


    den22 = ddot_(&n, zz, &incx, kzden1, &incy);

    beta = 0.;
    alpha = 1.;
    dgemv_(&trans, &n, &n, &alpha, kinv, &n, ww, &incx, &beta, kinvwden1, &incy);

    den11 = ddot_(&n, ww, &incx, kinvwden1, &incy);





    err0   = num11 / (den11 + den22);
    err1   = sqrt(err0);

    it_end = iter1;
    res    = err1;

    iter1  = iter1 + 1;

    iparamLCP[2] = it_end;
    dparamLCP[2] = res;




  }




  if (err1 > errmax)
  {
    if (iout > 0)
      printf("No convergence after %d iterations, the residue is %g\n", iter1, err1);

    *info = 1;
  }
  else
  {
    if (iout > 0)
      printf("Convergence after %d iterations, the residue is %g \n", iter1, err1);

    *info = 0;
  }




  free(wc);

  free(DPO);
  free(k);
  free(kinv);

  free(zz);
  free(zn);
  free(wn);
  free(ww);
  free(zc);
  free(znum1);
  free(wnum1);
  free(kinvden1);
  free(kinvden2);
  free(wt);
  free(maxwt);
  free(num1);
  free(kinvnum1);
  free(den1);
  free(den2);
  free(wden1);
  free(zden1);
  free(kinvwden1);
  free(kzden1);




}
