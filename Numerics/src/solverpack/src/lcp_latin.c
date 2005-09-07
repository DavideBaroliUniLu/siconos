#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


/*!\file latin_lcp.c


   This subroutine allows the resolution of LCP (Linear Complementary Problem).
   Try \f$(z,w)\f$ such that:

\f$
\left\lbrace
\begin{array}{l}
M z- w=q\\
0 \le z \perp w \ge 0\\
\end{array}
\right.
\f$

  here M is an n by n  matrix, q an n-dimensional vector, w an n-dimensional  vector and z an n-dimensional vector.
*/


/*!\fn  latin_lcp(double vec[],double *qq,int *nn, double * k_latin,int * itermax, double * tol,double z[],double w[],int *it_end,double * res,int *info)

   latin_lcp is a basic latin solver for LCP.


   \param vec On enter a double vector containing the components of the double matrix with a fortran90 allocation.
   \param qq On enter a pointer over doubles containing the components of the double vector.
   \param nn On enter a pointer over integers, the dimension of the second member.
   \param k_latin On enter a pointer over doubles, the k_latin coefficient (positive).
   \param itermax On enter a pointer over integers, the maximum iterations required.
   \param tol On enter a pointer over doubles, the tolerance required.
   \param it_end On enter a pointer over integers, the number of iterations carried out.
   \param res On return a pointer over doubles, the error value.
   \param z On return double vector, the solution of the problem.
   \param w On return double vector, the solution of the problem.
   \param info On return a pointer over integers, the termination reason (0 is successful otherwise 1).
   \author Nineb Sheherazade.
*/



lcp_latin(double *vec, double *qq, int *nn, double * k_latin, int *itermax, double * tol, double z[], double w[], int *it_end, double * res, int *info)
{
  int i, j, kk, iter1;
  int n = *nn, incx = 1, incy = 1, itt = *itermax;
  double errmax = *tol, alpha, beta;
  double rr, rrr, r1, r2, r3, invR0, invRT0, err1, z0, num11, err0, invRTinvR0;
  double den11, den22, vv;
  double  *wc, *zc, *kinvden1, *kinvden2, *wt, *maxwt, *wnum1, *znum1, *ww, *zz;
  double *num1, *kinvnum1, *den1, *den2, *wden1, *zden1;
  char trans = 'T';
  double k[n][n], A[n][n], R[n][n], RT[n][n], invRT[n][n], invR[n][n];
  double invRTinvR[n][n], kinv[n][n], xx[n][itt], kinvwden1[n], kzden1[n];





  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
    {
      k[i][j] = 0.;
    }

  for (i = 0; i < n; i++)
    k[i][i] = *k_latin * vec[i * n + i];

  /*// get C storage format for matrix*/

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
    {
      A[i][j] = vec[i * n + j] + k[i][j];
      R[i][j] = 0.;
    }


  /*    // !!!!!!!!!!!!!!!!!!!!!Cholesky!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

  R[0][0] = sqrt(A[0][0]);


  for (i = 1; i < n; i++)
  {
    rr = 0.0;
    rrr = 0.0;
    for (j = 0; j <= i; j++)
    {
      r1 = 0.0;
      r2 = 0.0;
      for (kk = 0; kk <= j - 1; kk++)
      {
        rr = R[i][kk] * R[j][kk] + r1;
        r1 = rr;
      }

      if (fabs(R[j][j]) <= 1.e-10)
      {
        printf("nul pivot %d ,and R(%d,%d) %g/n", j, j, j, R[j][j]);
        break;
      }
      R[i][j] = (1 / R[j][j]) * (A[i][j] - rr);

      r3 = 0.0;
      for (kk = 0; kk <= i - 1; kk++)
      {
        rrr = R[i][kk] * R[i][kk] + r3;
        r3 = rrr;
      }

      R[i][i] = sqrt(A[i][i] - rrr);
    }
  }

  /*    // !!!!!end of cholesky!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/


  /*   //  !determination of the R tranposeted*/

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      RT[i][j] = R[j][i];

  /*   // !inverse of R and RT*/

  for (i = 0; i < n; i++)
  {
    for (j = 0; j < n; j++)
    {
      invRT[i][j] = 0.;
      invR[i][j] = 0.;
    }
  }

  /*   // !!!!!!!!!inversion of the inf triangular matrix!!!!!!!!!!!!!*/

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
    {
      if (i == j) invR[i][j] = 1 / R[i][j];
      else
      {
        invR0 = 0.;
        for (kk = j; kk <= (i - 1); kk++)
        {
          invR[i][j] = (-1 / R[i][i]) * R[i][kk] * invR[kk][j] + invR0;
          invR0 = invR[i][j];
        }
      }
    }

  /*   // !!!!!!!!!!!!!!!!!!!end of inversion!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

  /*  // !!!!!!!!!!!!!!!!!!!!!!inversion of the sup triangular matrix!!!!!!!*/

  for (i = 0; i < n; i++)
  {
    invRT[i][i] = 1 / RT[i][i];
  }


  for (i = n - 2; i >= 0; i--)
  {
    for (j = n - 1; j >= 0; j--)
    {
      invRT0 = 0.;
      for (kk = i + 1; kk <= j; kk++)
      {
        invRT[i][j] = (-1 / RT[i][i]) * RT[i][kk] * invRT[kk][j] + invRT0;
        invRT0 = invRT[i][j];
      }
    }
  }

  /*  // !!!!!!!!!!!!!!!!!!!end of inversion!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/




  ww = (double*) malloc(n * sizeof(double));
  zz = (double*) malloc(n * sizeof(double));
  wc = (double*) malloc(n * sizeof(double));
  zc = (double*) malloc(n * sizeof(double));
  znum1 = (double*) malloc(n * sizeof(double));
  wnum1 = (double*) malloc(n * sizeof(double));
  kinvden1 = (double*) malloc(n * sizeof(double));
  kinvden2 = (double*) malloc(n * sizeof(double));
  wt = (double*) malloc(n * sizeof(double));
  maxwt = (double*) malloc(n * sizeof(double));
  num1 = (double*) malloc(n * sizeof(double));
  kinvnum1 = (double*) malloc(n * sizeof(double));
  den1 = (double*) malloc(n * sizeof(double));
  den2 = (double*) malloc(n * sizeof(double));
  wden1 = (double*) malloc(n * sizeof(double));
  zden1 = (double*) malloc(n * sizeof(double));


  for (i = 0; i < n; i++)
  {

    wc[i] = 0.0;
    zc[i] = 0.;
    z[i] = 0.;
    w[i] = 0.;
    znum1[i] = 0.;
    wnum1[i] = 0.;
    kinvden1[i] = 0.;
    kinvden2[i] = 0.;
    wt[i] = 0.;
    maxwt[i] = 0.;
    num1[i] = 0.;
    kinvnum1[i] = 0.;
    den1[i] = 0.;
    den2[i] = 0.;
  }

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
    {
      invRTinvR[i][j] = 0.;
      kinv[i][j] = 0.;
    }



  for (i = 0; i < n; i++)
    kinv[i][i] = 1 / k[i][i];


  for (i = 0; i < n; i++)
  {
    for (j = 0; j < n; j++)
    {
      invRTinvR0 = 0.0;
      for (kk = 0; kk < n; kk++)
      {
        invRTinvR[i][j] = invRT[i][kk] * invR[kk][j] + invRTinvR0;
        invRTinvR0 = invRTinvR[i][j];
      }
    }
  }


  /*    //  !iteration loops*/

  iter1 = 0;
  err1 = 1.;




  while ((iter1 < itt) && (err1 > errmax))
  {

    /*      //   !linear stage (zc,wc) -> (z,w)*/



    alpha = 1.;
    beta = 1.;


    dgemv_(&trans, &n, &n, &alpha, k, &n, zc, &incx, &beta, /*wt*/ wc, &incy);


    for (i = 0; i < n; i++)
    {
      z0 = 0.;
      vv = 0.;
      for (j = 0; j < n; j++)
      {
        vv = wc[j] - (-qq[j]);
        z[i] = invRTinvR[i][j] * vv + z0;
        z0 = z[i];
      }
    }

    for (i = 0; i < n; i++)
      xx[i][iter1] = z[i];



    alpha = -1.;
    beta = 1.;
    dgemv_(&trans, &n, &n, &alpha, k, &n, z, &incx, &beta, wc, &incy);

    dcopy_(&n, wc, &incx, w, &incy);




    /*Local Stage*/

    dcopy_(&n, w, &incx, wt, &incy);
    alpha = -1.;
    beta = 1.;
    dgemv_(&trans, &n, &n, &alpha, k, &n, z, &incx, &beta, wt, &incy);


    for (i = 0; i < n; i++)
    {
      if (wt[i] > 0.0) wc[i] = wt[i];
      else wc[i] = 0.0;
    }


    for (i = 0; i < n; i++)
    {
      if (-wt[i] < 0.0) zc[i] = 0.;
      else zc[i] = -wt[i] / k[i][i];
    }



    /*      // convergence criterium */


    dcopy_(&n, w, &incx, wnum1, &incy);
    alpha = -1.;
    daxpy_(&n, &alpha, wc, &incx, wnum1, &incy);


    dcopy_(&n, z, &incx, znum1, &incy);
    daxpy_(&n, &alpha, zc, &incx, znum1, &incy);



    alpha = 1.;
    beta = 1.;
    dgemv_(&trans, &n, &n, &alpha, k, &n, znum1, &incx, &beta, wnum1, &incy);


    /*      //    wnum1(:) =(w(:)-wc(:))+matmul( k(:,:),(z(:)-zc(:)))*/



    alpha = 1.;
    beta = 0.;
    dgemv_(&trans, &n, &n, &alpha, kinv, &n, wnum1, &incx, &beta, kinvnum1, &incy);


    num11 = ddot_(&n, wnum1, &incx, kinvnum1, &incy);


    /*     rectif ici*/


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


    /*     end rectif ici */


    err0 = num11 / (den11 + den22);
    err1 = sqrt(err0);
    iter1 = iter1 + 1;
    *it_end = iter1;
    *res = err1;

    /*printf("iter %d err %g\n",iter1,err1);*/
  }

  if (err1 > errmax)
  {
    printf("no convergence after %d iterations, the residue is %g\n", iter1, err1);
    *info = 1;
  }
  else
  {
    printf("there is convergence after %d iterations, the residue is %g \n", iter1, err1);
    *info = 0;
  }





  free(wc);
  free(zz);
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

}
