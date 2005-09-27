/*!\file pfc_2D_latin.c

  This subroutine allows the primal resolution of contact problems with friction.

   Try \f$(z,w)\f$ such that:
\f$
\left\lbrace
\begin{array}{l}
M z + q = w\\
0 \le z_n \perp w_n \ge 0\\
-w_t \in \partial\psi_{[-\mu z_n, \mu z_n]}(z_t)\\
\end{array}
\right.
\f$

 here M is an n by n  matrix, q an n-dimensional vector, z an n-dimensional  vector and w an n-dimensional vector.

*/
/*!\fn pfc_2D_latin(int *nn, double *vec, double *qq, double *z, double *w, int *info, int *iparamPFC, double *dparamPFC)



   pfc_2D_latin  is a specific latin solver for primal contact problem with friction.

   \param vec         On enter a double vector containing the components of the double matrix with a fortran90 allocation.
   \param qq          On enter a pointer over doubles containing the components of the double vector.
   \param nn          On enter a pointer over integers, the dimension of the second member.
   \param iparamPFC   On enter/return a vector of integers,

                       _ iparamPFC[0] = on enter, the maximum number of iterations allowed,
                       _ iparamPFC[1] = on enter, the parameter which represents the output log identifiant
                             0 - no output\n
           0 < active screen output\n
           _ iparamPFC[2] =  on return, the number of iterations performed by the algorithm

  \param dparamPFC   On enter/return a vector of doubles,

                       _ dparamPFC[0] = on enter, a positive double which represents the friction coefficient,
                       _ dparamPFC[1] = on enter, a positive double which represents the tolerance required,
                       _ dparamPFC[2] =  on return, a positive double which represents the residu.

   \param z           On return double vector, the solution of the problem.
   \param w           On return double vector, the solution of the problem.
   \param info        On return a pointer over integers, the termination reason
                       0 = Convergence,
           1 = no convergence,
           2 = Cholesky factorizayion failed,
           3 = Nul term in diagonal of M.



   \author Nineb Sheherazade.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "blaslapack.h"




void pfc_2D_latin(int *nn, double *vec, double *qq, double *z, double *w, int *info, int *iparamPFC, double *dparamPFC)
{



  int    i, j, kk, iter1, ino, ddl, info2, info77, nrhs, ispeak;
  int    n = *nn, incx = 1, incy = 1, nc = n / 2, idim, jdim, nbno, it_end;
  int    taille, taillet, taillen, itt;
  int    *ddln;
  int    *ddlt, *vectnt;

  double  errmax, alpha, beta, maxa, k_latin, res;
  double  aa, nt, wn, tc, zc0, mu;
  double  err1, num11, err0;
  double  den11, den22, knz0, ktz0, *ktz, *wf;
  double  *wc, *zc, *wt, *maxwt, *wnum1, *znum1;
  double  *zt, *maxzt;

  double  *kn, *kt;

  char    trans = 'T', diag = 'N';
  char    uplo = 'U', notrans = 'N';



  double  *k, *DPO, *kf, *kninv;
  double  *kinvwden1, *kzden1, *kfinv, *knz, *wtnc;



  /*                Recup input                    */


  itt     = iparamPFC[0];
  ispeak  = iparamPFC[1];

  mu      = dparamPFC[0];
  errmax  = dparamPFC[1];
  k_latin = dparamPFC[2];




  /*               Initialize output                */


  iparamPFC[2] = 0;
  dparamPFC[3] = 0.0;


  /*               Allocations                      */



  k         = (double*) malloc(n * n * sizeof(double));
  DPO       = (double*) malloc(n * n * sizeof(double));
  kf        = (double*) malloc(n * n * sizeof(double));
  kfinv     = (double*) malloc(n * n * sizeof(double));

  kninv     = (double*) malloc(nc * nc * sizeof(double));
  kn        = (double*) malloc(nc * nc * sizeof(double));
  kt        = (double*) malloc(nc * nc * sizeof(double));




  kinvwden1 = (double*) malloc(n  * sizeof(double));
  kzden1    = (double*) malloc(n  * sizeof(double));
  wc        = (double*) malloc(n  * sizeof(double));
  zc        = (double*) malloc(n  * sizeof(double));
  znum1     = (double*) malloc(n  * sizeof(double));
  wnum1     = (double*) malloc(n  * sizeof(double));
  wt        = (double*) malloc(n  * sizeof(double));
  maxzt     = (double*) malloc(n  * sizeof(double));



  knz       = (double*) malloc(nc * sizeof(double));
  wtnc      = (double*) malloc(nc * sizeof(double));
  ktz       = (double*) malloc(nc * sizeof(double));
  wf        = (double*) malloc(nc * sizeof(double));
  maxwt     = (double*) malloc(nc * sizeof(double));
  zt        = (double*) malloc(nc * sizeof(double));


  vectnt    = (int*) malloc(n * sizeof(int));

  ddln      = (int*) malloc(nc * sizeof(int));
  ddlt      = (int*) malloc(nc * sizeof(int));



  /*                    Initialization                   */



  for (i = 0; i < n * n; i++)
  {
    k[i]     = 0.;
    kf[i]    = 0.;
    kfinv[i] = 0.;

    if (i < nc * nc)
    {

      kn[i]    = 0.0;
      kt[i]    = 0.0;
      kninv[i] = 0.0;


      if (i < n)
      {
        wc[i]    = 0.0;
        zc[i]    = 0.;
        z[i]     = 0.;
        w[i]     = 0.;
        znum1[i] = 0.;
        wnum1[i] = 0.;
        wt[i]    = 0.;
        maxzt[i] = 0.;

        if (i < nc)
        {
          maxwt[i] = 0.;
          zt[i]    = 0.;
          knz[i]   = 0.;
          ktz[i]   = 0.;
          wf[i]    = 0.;
          wtnc[i]  = 0.;
        }

      }

    }
  }







  for (i = 0; i < n; i++)
  {

    if (fabs(vec[i * n + i]) < 1e-16)
    {

      if (ispeak > 0)
        printf("\n Warning nul diagonal term in M matrix \n");

      free(k);
      free(DPO);
      free(kf);
      free(kfinv);
      free(kninv);
      free(kn);
      free(kt);
      free(kinvwden1);
      free(kzden1);
      free(wc);
      free(zc);
      free(znum1);
      free(wnum1);
      free(wt);
      free(maxzt);
      free(knz);
      free(wtnc);
      free(ktz);
      free(wf);
      free(maxwt);
      free(zt);
      free(vectnt);
      free(ddln);
      free(ddlt);

      *info = 3;

      return;


    }
    else
    {

      k[i + n * i] = k_latin / vec[i * n + i];
      vectnt[i] = i + 1;

    }

  }





  for (i = 0; i < nc; i++)
  {
    ddln[i] = vectnt[2 * i];
    if (i != 0) ddlt[i] = vectnt[2 * i - 1];
    else ddlt[i] = 0;

  }






  for (i = 0; i < nc; i++)
  {
    kn[i + nc * i] = k[ddln[i] + n * ddln[i]];
    kt[i + nc * i] = k[ddlt[i] + n * ddlt[i]];
  }




  taillen = sizeof(ddln) / sizeof(ddln[0]);
  taillet = sizeof(ddlt) / sizeof(ddlt[0]);

  idim = 1 +  taillen / taillet;
  jdim = idim - 1;



  taille = 0;
  for (i = 0; i < n; i++)
    taille = sizeof(qq[i]) + taille;

  taille = taille / sizeof(qq[0]);
  nbno = taille / idim;




  for (i = 0; i < nc; i++)
  {
    kf[ddln[i] + n * ddln[i]] = kn[i + nc * i];
    kf[ddlt[i] + n * ddlt[i]] = kt[i + nc * i];
  }





  for (i = 0; i < n; i++)
  {
    kfinv[i + n * i] = 1. / kf[i + n * i];

    if (i < nc)
      kninv[i + nc * i] = 1. / kt[i + nc * i];

  }



  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      DPO[i + n * j] = vec[j * n + i] + kfinv[i + n * j];



  dpotrf_(&uplo, &n, DPO , &n, &info2);



  if (info2 != 0)
  {
    if (ispeak > 0)
      printf("\n Matter with Cholesky factorization \n");

    free(k);
    free(DPO);
    free(kf);
    free(kfinv);
    free(kninv);
    free(kn);
    free(kt);
    free(kinvwden1);
    free(kzden1);
    free(wc);
    free(zc);
    free(znum1);
    free(wnum1);
    free(wt);
    free(maxzt);
    free(knz);
    free(wtnc);
    free(ktz);
    free(wf);
    free(maxwt);
    free(zt);
    free(vectnt);
    free(ddln);
    free(ddlt);


    *info = 2;
    return;
  }



  /*                Iteration loops                  */


  iter1 = 0;
  err1  = 1.;



  while ((iter1 < itt) && (err1 > errmax))
  {

    /*               Linear stage (zc,wc) -> (z,w)         */


    alpha  = 1.;
    beta   = 1.;
    dgemv_(&notrans, &n, &n, &alpha, kfinv, &n, zc, &incx, &beta, wc, &incy);

    dcopy_(&n, qq, &incx, znum1, &incy);

    alpha = -1.;//-
    dscal_(&n , &alpha , znum1 , &incx);

    alpha = 1.;
    daxpy_(&n, &alpha, wc, &incx, znum1, &incy);

    nrhs = 1;
    dtrtrs_(&uplo, &trans, &diag, &n, &nrhs, DPO, &n, znum1, &n, &info77);

    dtrtrs_(&uplo, &notrans, &diag, &n, &nrhs, DPO, &n, znum1, &n, &info77);

    dcopy_(&n, znum1, &incx, z, &incy);

    alpha = -1.;
    beta = 1.;
    dgemv_(&notrans, &n, &n, &alpha, kfinv, &n, z, &incx, &beta, wc, &incy);

    dcopy_(&n, wc, &incx, w, &incy);



    /*               Local stage (z,w)->(zc,wc)          */



    for (i = 0; i < n; i++)
    {
      zc[i] = 0.;
      wc[i] = 0.0;
    }



    /*           Normal party                           */



    for (i = 0; i < nc; i++)
    {
      knz0 = 0.;
      for (kk = 0; kk < nc; kk++)
      {
        knz[i] = kt[i + nc * kk] * w[ddlt[kk]] + knz0;
        knz0 = knz[i];
      }

      zt[i] = z[ddlt[i]] - knz[i];

      if (zt[i] > 0.0)
      {
        zc[ddlt[i]] = zt[i];
        maxzt[i] = 0.0;
      }
      else
      {
        zc[ddlt[i]] = 0.0;
        maxzt[i] = -zt[i];
      }
    }


    for (i = 0; i < nc; i++)
    {
      zc0 = 0.;
      ktz0 = 0.;
      for (j = 0; j < nc; j++)
      {
        wc[ddlt[i]] = kninv[i + nc * j] * maxzt[j] + zc0;
        zc0 = wc[ddlt[i]];
        ktz[i] = kn[i + nc * j] * w[ddln[j]] + ktz0;
        ktz0 =  ktz[i];
      }
      wf[i] = z[ddln[i]] - ktz[i];
    }


    /*             Loop other nodes              */



    for (ino = 0; ino < nbno; ino++)
    {
      ddl  = ddln[ino];
      nt   = fabs(wf[ino]);


      /*          Tangential vector              */



      if (nt < 1.e-8)  tc = 0.;
      else tc = wf[ino] / nt;



      /*               Tangentiel component             */


      wn = zc[ddlt[ino]];

      aa = nt - mu * wn;

      if (aa > 0.0)
      {
        maxa = aa;
      }
      else
      {
        maxa = 0.0;
      }

      wc[ddl] = (maxa / (-1 * kn[ino + nc * ino])) * tc;

      aa = -nt + mu * wn;

      if (aa > 0.0)
      {
        maxa = aa;
      }
      else
      {
        maxa = 0.0;
      }

      zc[ddl] = (mu * wn - maxa) * tc;

    }



    /*               Convergence criterium                */



    dcopy_(&n, z, &incx, znum1, &incy);

    alpha = -1.;
    daxpy_(&n, &alpha, zc, &incx, znum1, &incy);

    dcopy_(&n, w, &incx, wnum1, &incy);

    daxpy_(&n, &alpha, wc, &incx, wnum1, &incy);

    alpha  = 1.;
    beta   = 1.;
    dgemv_(&notrans, &n, &n, &alpha, kf, &n, wnum1, &incx, &beta, znum1, &incy);

    num11  = 0.;
    alpha  = 1.;
    beta = 0.;
    dgemv_(&notrans, &n, &n, &alpha, kfinv, &n, znum1, &incx, &beta, wnum1, &incy);

    num11 = ddot_(&n, wnum1, &incx, znum1, &incy);

    dcopy_(&n, z, &incx, znum1, &incy);

    alpha  = 1.;
    beta   = 1.;
    dgemv_(&notrans, &n, &n, &alpha, kf, &n, w, &incx, &beta, znum1, &incy);

    alpha  = 1.;
    beta   = 0.;
    dgemv_(&notrans, &n, &n, &alpha, kfinv, &n, znum1, &incx, &beta, wnum1, &incy);

    den11  = ddot_(&n, wnum1, &incx, znum1, &incy);

    dcopy_(&n, zc, &incx, znum1, &incy);

    alpha  = 1.;
    beta   = 1.;
    dgemv_(&notrans, &n, &n, &alpha, kf, &n, wc, &incx, &beta, znum1, &incy);

    alpha  = 1.;
    beta   = 0.;
    dgemv_(&notrans, &n, &n, &alpha, kfinv, &n, znum1, &incx, &beta, wnum1, &incy);

    den22  = ddot_(&n, znum1, &incx, wnum1, &incy);

    err0   = num11 / (den11 + den22);

    err1   = sqrt(err0);



    it_end = iter1;
    res    = err1;

    iparamPFC[2] = iter1;
    dparamPFC[3] = err1;

    iter1   = iter1 + 1;


  }


  if (err1 > errmax)
  {

    if (ispeak > 0)
      printf("No convergence after %d iterations, the residue is %g\n", iter1, err1);

    *info = 1;
  }
  else
  {

    if (ispeak > 0)
      printf("Convergence after %d iterations, the residue is %g \n", iter1, err1);

    *info = 0;
  }


  free(k);
  free(DPO);
  free(kf);
  free(kfinv);
  free(kninv);
  free(kn);
  free(kt);
  free(kinvwden1);
  free(kzden1);
  free(wc);
  free(zc);
  free(znum1);
  free(wnum1);
  free(wt);
  free(maxzt);
  free(knz);
  free(wtnc);
  free(ktz);
  free(wf);
  free(maxwt);
  free(zt);
  free(vectnt);
  free(ddln);
  free(ddlt);








}
