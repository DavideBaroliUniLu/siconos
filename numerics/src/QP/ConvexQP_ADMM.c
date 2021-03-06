/* Siconos is a program dedicated to modeling, simulation and control
 * of non smooth dynamical systems.
 *
 * Copyright 2016 INRIA.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/


#include "ConvexQP_Solvers.h"
#include "ConvexQP_computeError.h"
#include "NumericsMatrix.h"
#include "SparseBlockMatrix.h"
#include "NumericsMatrix.h"
#include "NumericsVector.h"
#include "NumericsSparseMatrix.h"
#include "SiconosBlas.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "ConvexQP_cst.h"
#include "numerics_verbose.h"

/* #define DEBUG_NOCOLOR */
/* #define DEBUG_MESSAGES */
/* #define DEBUG_STDOUT */
#include "debug.h"

const char* const   SICONOS_CONVEXQP_ADMM_STR = "CONVEXQP ADMM";


void convexQP_ADMM(ConvexQP* problem,
                   double *z, double * w,
                   double * xi, double *u,
                   int* info, SolverOptions* options)
{
  /* int and double parameters */
  int* iparam = options->iparam;
  double* dparam = options->dparam;



  double* q = problem->q;
  NumericsMatrix* M = problem->M;

  NumericsMatrix* A = problem->A;
  double * b = problem->b;

  /* Dimension of the problem */
  int n =  problem->size;
  int AisIdentity = 0;

  if (!A) /* A is considered to be the identity and b =0 */
  {
    DEBUG_PRINTF("A is not given and then considered as Identity and b =0\n");
    problem->m = n;
    problem->A= NM_create(NM_SPARSE,n,n);
    NM_triplet_alloc(problem->A,0);
    problem->A->matrix2->origin= NS_TRIPLET;

    for (int k =0; k< n; k++)
    {
      NM_zentry(problem->A, k, k, 1);
    }
    DEBUG_EXPR(NM_display(problem->A));

    if (!b)
    {
      problem->b = (double * ) calloc(n,sizeof(double));
    }
    A = problem->A;
    b = problem->b;
    AisIdentity = 1;
  }


  int m =  problem->m;

  /* Maximum number of iterations */
  int itermax = iparam[SICONOS_IPARAM_MAX_ITER];
  /* Tolerance */
  double tolerance = dparam[SICONOS_DPARAM_TOL];

  /*****  ADMM iterations *****/
  int iter = 0; /* Current iteration number */
  double error = 1.; /* Current error */
  int hasNotConverged = 1;

  double rho = 0.0;
  rho = dparam[SICONOS_CONVEXQP_ADMM_RHO];
  if (rho == 0.0)
    numerics_error("ConvexQP_ADMM", "dparam[SICONOS_CONVEXQP_PGOC_RHO] must be nonzero");

  double tau=1;


  /* double * z_k; */
  /* double * w_k; */
  /* double * u_k; */
  double * u_tmp;
  /* double * xi_k; */
  /* double * xi_tmp; */

  int accelerated = iparam[SICONOS_CONVEXQP_ADMM_IPARAM_ACCELERATION];
  if (accelerated == iparam[SICONOS_CONVEXQP_ADMM_NO_ACCELERATION])
  {

  }


  /* z_k = (double *)malloc(n * sizeof(double)); */
  /* w_k = (double *)malloc(n * sizeof(double)); */
  /* u_k = (double *)calloc(m,sizeof(double)); */
  u_tmp = (double *)calloc(m,sizeof(double));
  /* xi_k = (double *)calloc(m,sizeof(double)); */
  /* xi_tmp = (double *)calloc(m,sizeof(double)); */


  /* Compute M + rho A^T A (storage in M)*/
  NumericsMatrix *Atrans;
  if (!A)
  {
    if (M->storageType != A->storageType)
    {
      numerics_error("ConvexQP_ADMM", "M and A must have the same storage");
    }
  }


  NumericsMatrix *W = NM_new();

  NM_copy(M, W);

  if (AisIdentity)
  {
    NM_add_to_diag3(W, rho);
  }
  else
  {

    if (M->storageType == NM_SPARSE_BLOCK)
    {
      Atrans = NM_create(NM_SPARSE_BLOCK, A->size1, A->size0);
      SBM_transpose(A->matrix1, Atrans->matrix1);
    }
    else if (A->storageType == NM_SPARSE)
    {
      NM_csc_trans(A);

      Atrans = NM_new();
      Atrans->storageType = NM_SPARSE;
      Atrans-> size0 = m;
      Atrans-> size1 = n;
      NM_csc_alloc(Atrans, 0);
      Atrans->matrix2->origin = NS_CSC;
      Atrans->matrix2->csc = NM_csc_trans(A);
      DEBUG_EXPR(NM_display(Atrans));
    }
    else
    {
      printf("gfc3d_reformulation_local_problem :: unknown matrix storage");
      exit(EXIT_FAILURE);
    }
    NM_gemm(rho, Atrans, A, 1.0, W);
  }




  while ((iter < itermax) && (hasNotConverged > 0))
  {
    ++iter;

    /********************/
    /*  1 - Compute z */
    /********************/

    /* compute the rhs */
    /* q --> z */
    cblas_dcopy(n , q , 1 , z, 1);
    cblas_dscal(n, -1, z,1);

    /*  u -b + xi_k --> u */
    cblas_daxpy(m, -1.0, b, 1, u , 1);
    cblas_daxpy(m, 1.0, xi, 1, u , 1);

    /* rho A^T (u_k +xi_k -b  ) - q --> w_k */
    /* for (int k =0; k< n; k++) w_k[k] = -w_k[k]; */

    if (AisIdentity)
    {
      cblas_daxpy(m, rho, u, 1, z, 1);
    }
    else
    {
      NM_gemv(rho, Atrans, u, 1.0, z);
    }
    DEBUG_EXPR(NV_display(z,n));

    /* Linear system solver */
    /* cblas_dcopy(n , w_k , 1 , z, 1); */
    NM_gesv_expert(W,z,NM_KEEP_FACTORS);
    DEBUG_EXPR(NV_display(z,n));

    /********************/
    /*  2 - Compute u */
    /********************/

    /* A z_k - xi_k + b */
    cblas_dcopy(m , b , 1 , u_tmp, 1);
    cblas_daxpy(m, -1, xi, 1, u_tmp , 1);
    if (AisIdentity)
    {
      cblas_daxpy(m, 1.0, z, 1, u_tmp , 1);
    }
    else
    {
      NM_gemv(1.0, A, z, 1.0, u_tmp);
    }
    problem->ProjectionOnC(problem,u_tmp,u);
    DEBUG_EXPR(NV_display(u,m));

    /**********************/
    /*  3 - Compute xi */
    /**********************/

    /* xi_k + A z_k -u_k +b -> xi_k */
    cblas_daxpy(m, -1, b, 1, xi , 1);
    
    if (AisIdentity)
    {
      cblas_daxpy(m, -1.0, z, 1, xi , 1);
    }
    else
    {
      NM_gemv(-1.0, A, z, 1.0, xi);
    }

    cblas_daxpy(m, 1, u, 1, xi , 1);
    DEBUG_EXPR(NV_display(xi,m));

    //cblas_daxpy(m,rho, u_k, 0 , w_k, 1);
    //DEBUG_EXPR(NV_display(w_k,n));

    /* **** Criterium convergence **** */
    //convexQP_computeError(problem, z , w_k, tolerance, options, &error);
    convexQP_computeError_full(problem, z , xi, w, u, tolerance, options, &error);
    DEBUG_EXPR(NV_display(w_k,n));
    DEBUG_EXPR(NV_display(q,n));
    if (verbose > 0)
      printf("--------------- ConvexQP - ADMM  - Iteration %i rho = %14.7e \tError = %14.7e\n", iter, rho, error);

    if (error < tolerance) hasNotConverged = 0;
    *info = hasNotConverged;
  }

  //cblas_dscal(m, -1, xi, 1);


  //verbose=1;

  if (verbose > 0)
  printf("---------------  ConvexQP - ADMM - #Iteration %i Final Residual = %14.7e\n", iter, error);
  dparam[SICONOS_DPARAM_RESIDU] = error;
  iparam[SICONOS_IPARAM_ITER_DONE] = iter;

  /* free(z_k); */
  /* free(w_k); */
  free(u_tmp);
  /* free(xi_k); */

}


int convexQP_ADMM_setDefaultSolverOptions(SolverOptions* options)
{
  if (verbose > 0)
  {
    printf("Set the Default SolverOptions for the ADMM Solver\n");
  }

  options->solverId = SICONOS_CONVEXQP_ADMM;

  options->numberOfInternalSolvers = 0;
  options->isSet = 1;
  options->filterOn = 1;
  options->iSize = 20;
  options->dSize = 20;

  options->iparam = (int *)calloc(options->iSize, sizeof(int));
  options->dparam = (double *)calloc(options->dSize, sizeof(double));
  options->dWork = NULL;
  solver_options_nullify(options);

  options->iparam[SICONOS_IPARAM_MAX_ITER] = 20000;
  options->iparam[SICONOS_CONVEXQP_ADMM_ACCELERATION] = 0; /* 0 Acceleration */


  options->dparam[SICONOS_DPARAM_TOL] = 1e-6;

  options->dparam[SICONOS_CONVEXQP_ADMM_RHO] = 1.e+2; 

  options->internalSolvers = NULL;


  return 0;
}
