/*
 *  Benchmark matrices for LCP (Linear Complementary Problem)
 *  This file generate the reading of data file contains in the folder MATRIX
 *  and try to found (z,w) such that:
 *
 *                        M z + q = w        (1)
 *                        0 <= z , 0 <= w    (2)
 *                        z.w= O             (3)
 *
 *  where M is an (n x n)-matrix, q an n-vector, z an n-vector and w an n-vector.
 *
 *  This system of equations and inequalities is solved thanks to subroutine_lcp:
 *
 *        lexicolemke_lcp (M,q,n,itermax,z,w,it_end,res,info)
 *        gsnl_lcp(M,q,n,itermax,tol,z,w,it_end,res,info)
 *        gcp_lcp(M,q,n,itermax,tol,z,w,it_end,res,info)
 *        qp_lcp(M,q,n,res,z,w,fail)
 *
 *  where _ itermax is the maximum iterations required, it's an integer
 *        _ res is the residue, it's a float
 *        _ it_end is the number of iterations carried out, it's an integer
 *        _ tol is the tolerance value desired, it's a float
 *        _ k_latin is the parameter research of the latin, it's a float
 *        _ fail shows the termination reason, it's an integer
 *        _ info shows the trmination reason (0 successful otherwise 1), it's an integer.
 *
 *  The subroutine's call is due to the function solve_lcp:
 *
 *  int solve_lcp (float (*M)[maxcols],float * q, int n, methode *pt, float *z, float * w)
 *
 *  where M is an n by n matrix, q an n-dimensional vector, n is the row dimension of M,
 *  and pt a pointer other a structure (methode), z and w are n-dimensional vector, the solutions of the lcp.
 *  methode is a variable with a structure type; this structure gives to the function solve_lcp,
 *  the name and the parameters (itermax, tol, k_latin) of the method we want to use.
 *  This function return an interger:  0 successful return otherwise 1.
 *
 * author : Mathieu Renouf
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "solverpack.h"
#include "blaslapack.h"

#define BAVARD

int main(void)
{

  FILE *LCPfile;

  int i, j;
  int incx = 1 , incy = 1;
  int info ;
  int dim , dim2;

  double a1;

  double RES;
  double *q , *z , *w , *sol;
  double *vecM;

  char val[20];

  int iter;
  double criteria;

  /*
   * methode declaration :
   *
   * methode 1 for Gsnl
   * methode 2 for Cpg
   * methode 3 for lexicolemke
   *
   */

  static methode_lcp meth_lcp1 = { "Gsnl"        , 1000 , 1e-8 , 0.6 , 1.0 , 0 , "N2" };
  static methode_lcp meth_lcp2 = { "Gcp"         , 1000 , 1e-8 , 0.6 , 1.0 , 0 , "N2" };
  static methode_lcp meth_lcp3 = { "LexicoLemke" , 1000 , 1e-8 , 0.6 , 1.0 , 0 , "N2" };

  /****************************************************************/
  /****************************************************************/

  if ((LCPfile = fopen("MATRIX/deudeu.dat", "r")) == NULL)
  {
    perror("fopen LCPfile: deudeu.dat");
    exit(1);
  }

  fscanf(LCPfile , "%d" , &dim);

  dim2 = dim * dim;

  vecM = (double*)malloc(dim2 * sizeof(double));
  q    = (double*)malloc(dim * sizeof(double));
  sol  = (double*)malloc(dim * sizeof(double));

  for (i = 0 ; i < dim ; ++i)  q[i]    = 0.0;
  for (i = 0 ; i < dim ; ++i)  sol[i]  = 0.0;
  for (i = 0 ; i < dim2 ; ++i) vecM[i] = 0.0;

  for (i = 0 ; i < dim ; ++i)
  {
    for (j = 0 ; j < dim ; ++j)
    {
      fscanf(LCPfile, "%s", val);
      vecM[ dim * j + i ] = atof(val);
    }
  }

  for (i = 0 ; i < dim ; ++i)
  {
    fscanf(LCPfile , "%s" , val);
    q[i] = atof(val);
  }

  fscanf(LCPfile , "%s" , val);

  if (!feof(LCPfile))
  {

    sol[0] = atof(val);

    for (i = 1 ; i < dim ; ++i)
    {
      fscanf(LCPfile , "%s" , val);
      sol[i] = atof(val);
    }
  }
  else
  {
    for (i = 0 ; i < dim ; ++i) sol[i] = 0.0;
  }

  fclose(LCPfile);

#ifdef BAVARD
  printf("\n solution deudeu  : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , sol[i]);
  printf("\n");
#endif

  z = malloc(dim * sizeof(double));
  w = malloc(dim * sizeof(double));

  /* NLGS */

  for (i = 0 ; i < dim ; ++i) z[i] = 0.0;

  info = solve_lcp(vecM , q , &dim , &meth_lcp1 , z , w , &iter , &criteria);

#ifdef BAVARD
  printf("\n           NLGS   : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , z[i]);
  printf(" ( %6d ) " , iter);
#endif

  /* CPG */

  for (i = 0 ; i < dim ; ++i) z[i] = 0.0;

  info = solve_lcp(vecM , q , &dim , &meth_lcp2 , z , w , &iter , &criteria);

#ifdef BAVARD
  printf("\n           CPG    : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , z[i]);
  printf(" ( %6d ) " , iter);
#endif

  /* Lemke */

  for (i = 0 ; i < dim ; ++i) z[i] = 0.0;

  info = solve_lcp(vecM , q , &dim , &meth_lcp3 , z , w , &iter , &criteria);

#ifdef BAVARD
  printf("\n           Lemke  : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , z[i]);
  printf(" ( %6d ) " , iter);
#endif

  free(q);
  free(sol);
  free(vecM);

  /****************************************************************/
  /****************************************************************/

  if ((LCPfile = fopen("MATRIX/trivial.dat", "r")) == NULL)
  {
    perror("fopen LCPfile: trivial.dat");
    exit(1);
  }

  fscanf(LCPfile , "%d" , &dim);

  dim2 = dim * dim;

  vecM = (double*)malloc(dim2 * sizeof(double));
  q    = (double*)malloc(dim * sizeof(double));
  sol  = (double*)malloc(dim * sizeof(double));

  for (i = 0 ; i < dim ; ++i)  q[i]    = 0.0;
  for (i = 0 ; i < dim ; ++i)  sol[i]  = 0.0;
  for (i = 0 ; i < dim2 ; ++i) vecM[i] = 0.0;

  for (i = 0 ; i < dim ; ++i)
  {
    for (j = 0 ; j < dim ; ++j)
    {
      fscanf(LCPfile, "%s", val);
      vecM[ dim * j + i ] = atof(val);
    }
  }

  for (i = 0 ; i < dim ; ++i)
  {
    fscanf(LCPfile , "%s" , val);
    q[i] = atof(val);
  }

  fscanf(LCPfile , "%s" , val);

  if (!feof(LCPfile))
  {

    sol[0] = atof(val);

    for (i = 1 ; i < dim ; ++i)
    {
      fscanf(LCPfile , "%s" , val);
      sol[i] = atof(val);
    }
  }
  else
  {
    for (i = 0 ; i < dim ; ++i) sol[i] = 0.0;
  }

  fclose(LCPfile);

#ifdef BAVARD
  printf("\n");
  printf("\n solution trivial : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , sol[i]);
  printf("\n");
#endif

  z = malloc(dim * sizeof(double));
  w = malloc(dim * sizeof(double));

  /* NLGS */

  for (i = 0 ; i < dim ; ++i) z[i] = 0.0;

  info = solve_lcp(vecM , q , &dim , &meth_lcp1 , z , w , &iter , &criteria);

#ifdef BAVARD
  printf("\n           NLGS   : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , z[i]);
  printf(" ( %6d ) " , iter);
#endif

  /* CPG */

  for (i = 0 ; i < dim ; ++i) z[i] = 0.0;

  info = solve_lcp(vecM , q , &dim , &meth_lcp2 , z , w , &iter , &criteria);

#ifdef BAVARD
  printf("\n           CPG    : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , z[i]);
  printf(" ( %6d ) " , iter);
#endif

  /* Lemke */

  for (i = 0 ; i < dim ; ++i) z[i] = 0.0;

  info = solve_lcp(vecM , q , &dim , &meth_lcp3 , z , w , &iter , &criteria);

#ifdef BAVARD
  printf("\n           Lemke  : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , z[i]);
  printf(" ( %6d ) " , iter);
#endif

  free(q);
  free(sol);
  free(vecM);

  /****************************************************************/
  /****************************************************************/

  if ((LCPfile = fopen("MATRIX/ortiz.dat", "r")) == NULL)
  {
    perror("fopen LCPfile: ortiz.dat");
    exit(1);
  }

  fscanf(LCPfile , "%d" , &dim);

  dim2 = dim * dim;

  vecM = (double*)malloc(dim2 * sizeof(double));
  q    = (double*)malloc(dim * sizeof(double));
  sol  = (double*)malloc(dim * sizeof(double));

  for (i = 0 ; i < dim ; ++i)  q[i]    = 0.0;
  for (i = 0 ; i < dim ; ++i)  sol[i]  = 0.0;
  for (i = 0 ; i < dim2 ; ++i) vecM[i] = 0.0;

  for (i = 0 ; i < dim ; ++i)
  {
    for (j = 0 ; j < dim ; ++j)
    {
      fscanf(LCPfile, "%s", val);
      vecM[ dim * j + i ] = atof(val);
    }
  }

  for (i = 0 ; i < dim ; ++i)
  {
    fscanf(LCPfile , "%s" , val);
    q[i] = atof(val);
  }

  fscanf(LCPfile , "%s" , val);

  if (!feof(LCPfile))
  {

    sol[0] = atof(val);

    for (i = 1 ; i < dim ; ++i)
    {
      fscanf(LCPfile , "%s" , val);
      sol[i] = atof(val);
    }
  }
  else
  {
    for (i = 0 ; i < dim ; ++i) sol[i] = 0.0;
  }

  fclose(LCPfile);

#ifdef BAVARD
  printf("\n");
  printf("\n solution ortiz   : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , sol[i]);
  printf("\n");
#endif

  z = malloc(dim * sizeof(double));
  w = malloc(dim * sizeof(double));

  /* NLGS */

  for (i = 0 ; i < dim ; ++i) z[i] = 0.0;

  info = solve_lcp(vecM , q , &dim , &meth_lcp1 , z , w , &iter , &criteria);

#ifdef BAVARD
  printf("\n           NLGS   : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , z[i]);
  printf(" ( %6d ) " , iter);
#endif

  /* CPG */

  for (i = 0 ; i < dim ; ++i) z[i] = 0.0;

  info = solve_lcp(vecM , q , &dim , &meth_lcp2 , z , w , &iter , &criteria);

#ifdef BAVARD
  printf("\n           CPG    : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , z[i]);
  printf(" ( %6d ) " , iter);
#endif

  /* Lemke */

  for (i = 0 ; i < dim ; ++i) z[i] = 0.0;

  info = solve_lcp(vecM , q , &dim , &meth_lcp3 , z , w , &iter , &criteria);

#ifdef BAVARD
  printf("\n           Lemke  : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , z[i]);
  printf(" ( %6d ) " , iter);
#endif

  free(q);
  free(sol);
  free(vecM);

  /****************************************************************/
  /****************************************************************/

  if ((LCPfile = fopen("MATRIX/pang.dat", "r")) == NULL)
  {
    perror("fopen LCPfile: pang.dat");
    exit(1);
  }

  fscanf(LCPfile , "%d" , &dim);

  dim2 = dim * dim;

  vecM = (double*)malloc(dim2 * sizeof(double));
  q    = (double*)malloc(dim * sizeof(double));
  sol  = (double*)malloc(dim * sizeof(double));

  for (i = 0 ; i < dim ; ++i)  q[i]    = 0.0;
  for (i = 0 ; i < dim ; ++i)  sol[i]  = 0.0;
  for (i = 0 ; i < dim2 ; ++i) vecM[i] = 0.0;

  for (i = 0 ; i < dim ; ++i)
  {
    for (j = 0 ; j < dim ; ++j)
    {
      fscanf(LCPfile, "%s", val);
      vecM[ dim * j + i ] = atof(val);
    }
  }

  for (i = 0 ; i < dim ; ++i)
  {
    fscanf(LCPfile , "%s" , val);
    q[i] = atof(val);
  }

  fscanf(LCPfile , "%s" , val);

  if (!feof(LCPfile))
  {

    sol[0] = atof(val);

    for (i = 1 ; i < dim ; ++i)
    {
      fscanf(LCPfile , "%s" , val);
      sol[i] = atof(val);
    }
  }
  else
  {
    for (i = 0 ; i < dim ; ++i) sol[i] = 0.0;
  }

  fclose(LCPfile);

#ifdef BAVARD
  printf("\n");
  printf("\n solution pang    : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , sol[i]);
  printf("\n");
#endif

  z = malloc(dim * sizeof(double));
  w = malloc(dim * sizeof(double));

  /* NLGS */

  for (i = 0 ; i < dim ; ++i) z[i] = 0.0;

  info = solve_lcp(vecM , q , &dim , &meth_lcp1 , z , w , &iter , &criteria);

#ifdef BAVARD
  printf("\n           NLGS   : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , z[i]);
  printf(" ( %6d ) " , iter);
#endif

  /* CPG */

  for (i = 0 ; i < dim ; ++i) z[i] = 0.0;

  info = solve_lcp(vecM , q , &dim , &meth_lcp2 , z , w , &iter , &criteria);

#ifdef BAVARD
  printf("\n           CPG    : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , z[i]);
  printf(" ( %6d ) " , iter);
#endif

  /* Lemke */

  for (i = 0 ; i < dim ; ++i) z[i] = 0.0;

  info = solve_lcp(vecM , q , &dim , &meth_lcp3 , z , w , &iter , &criteria);

#ifdef BAVARD
  printf("\n           Lemke  : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , z[i]);
  printf(" ( %6d ) " , iter);
#endif

  free(q);
  free(sol);
  free(vecM);

  /****************************************************************/
  /****************************************************************/

  if ((LCPfile = fopen("MATRIX/diodes.dat", "r")) == NULL)
  {
    perror("fopen LCPfile: diodes.dat");
    exit(1);
  }

  fscanf(LCPfile , "%d" , &dim);

  dim2 = dim * dim;

  vecM = (double*)malloc(dim2 * sizeof(double));
  q    = (double*)malloc(dim * sizeof(double));
  sol  = (double*)malloc(dim * sizeof(double));

  for (i = 0 ; i < dim ; ++i)  q[i]    = 0.0;
  for (i = 0 ; i < dim ; ++i)  sol[i]  = 0.0;
  for (i = 0 ; i < dim2 ; ++i) vecM[i] = 0.0;

  for (i = 0 ; i < dim ; ++i)
  {
    for (j = 0 ; j < dim ; ++j)
    {
      fscanf(LCPfile, "%s", val);
      vecM[ dim * j + i ] = atof(val);
    }
  }

  for (i = 0 ; i < dim ; ++i)
  {
    fscanf(LCPfile , "%s" , val);
    q[i] = atof(val);
  }

  fscanf(LCPfile , "%s" , val);

  if (!feof(LCPfile))
  {

    sol[0] = atof(val);

    for (i = 1 ; i < dim ; ++i)
    {
      fscanf(LCPfile , "%s" , val);
      sol[i] = atof(val);
    }
  }
  else
  {
    for (i = 0 ; i < dim ; ++i) sol[i] = 0.0;
  }

  fclose(LCPfile);

#ifdef BAVARD
  printf("\n");
  printf("\n solution diodes  : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , sol[i]);
  printf("\n");
#endif

  z = malloc(dim * sizeof(double));
  w = malloc(dim * sizeof(double));

  /* NLGS */

  for (i = 0 ; i < dim ; ++i) z[i] = 0.0;

  info = solve_lcp(vecM , q , &dim , &meth_lcp1 , z , w , &iter , &criteria);

#ifdef BAVARD
  printf("\n           NLGS   : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , z[i]);
  printf(" ( %6d ) " , iter);
#endif

  /* CPG */

  for (i = 0 ; i < dim ; ++i) z[i] = 0.0;

  info = solve_lcp(vecM , q , &dim , &meth_lcp2 , z , w , &iter , &criteria);

#ifdef BAVARD
  printf("\n           CPG    : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , z[i]);
  printf(" ( %6d ) " , iter);
#endif

  /* Lemke */

  for (i = 0 ; i < dim ; ++i) z[i] = 0.0;

  info = solve_lcp(vecM , q , &dim , &meth_lcp3 , z , w , &iter , &criteria);

#ifdef BAVARD
  printf("\n           Lemke  : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , z[i]);
  printf(" ( %6d ) " , iter);
#endif

  free(q);
  free(sol);
  free(vecM);

  /****************************************************************/
  /****************************************************************/

  if ((LCPfile = fopen("MATRIX/murty1.dat", "r")) == NULL)
  {
    perror("fopen LCPfile: murty1.dat");
    exit(1);
  }

  fscanf(LCPfile , "%d" , &dim);

  dim2 = dim * dim;

  vecM = (double*)malloc(dim2 * sizeof(double));
  q    = (double*)malloc(dim * sizeof(double));
  sol  = (double*)malloc(dim * sizeof(double));

  for (i = 0 ; i < dim ; ++i)  q[i]    = 0.0;
  for (i = 0 ; i < dim ; ++i)  sol[i]  = 0.0;
  for (i = 0 ; i < dim2 ; ++i) vecM[i] = 0.0;

  for (i = 0 ; i < dim ; ++i)
  {
    for (j = 0 ; j < dim ; ++j)
    {
      fscanf(LCPfile, "%s", val);
      vecM[ dim * j + i ] = atof(val);
    }
  }

  for (i = 0 ; i < dim ; ++i)
  {
    fscanf(LCPfile , "%s" , val);
    q[i] = atof(val);
  }

  fscanf(LCPfile , "%s" , val);

  if (!feof(LCPfile))
  {

    sol[0] = atof(val);

    for (i = 1 ; i < dim ; ++i)
    {
      fscanf(LCPfile , "%s" , val);
      sol[i] = atof(val);
    }
  }
  else
  {
    for (i = 0 ; i < dim ; ++i) sol[i] = 0.0;
  }

  fclose(LCPfile);

#ifdef BAVARD
  printf("\n");
  printf("\n solution murty1  : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , sol[i]);
  printf("\n");
#endif

  z = malloc(dim * sizeof(double));
  w = malloc(dim * sizeof(double));

  /* NLGS */

  for (i = 0 ; i < dim ; ++i) z[i] = 0.0;

  info = solve_lcp(vecM , q , &dim , &meth_lcp1 , z , w , &iter , &criteria);

#ifdef BAVARD
  printf("\n           NLGS   : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , z[i]);
  printf(" ( %6d ) " , iter);
#endif

  /* CPG */

  for (i = 0 ; i < dim ; ++i) z[i] = 0.0;

  info = solve_lcp(vecM , q , &dim , &meth_lcp2 , z , w , &iter , &criteria);

#ifdef BAVARD
  printf("\n           CPG    : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , z[i]);
  printf(" ( %6d ) " , iter);
#endif

  /* Lemke */

  for (i = 0 ; i < dim ; ++i) z[i] = 0.0;

  info = solve_lcp(vecM , q , &dim , &meth_lcp3 , z , w , &iter , &criteria);

#ifdef BAVARD
  printf("\n           Lemke  : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , z[i]);
  printf(" ( %6d ) " , iter);
#endif

  free(q);
  free(sol);
  free(vecM);

  /****************************************************************/
  /****************************************************************/

  if ((LCPfile = fopen("MATRIX/confeti.dat", "r")) == NULL)
  {
    perror("fopen LCPfile: confeti.dat");
    exit(1);
  }

  fscanf(LCPfile , "%d" , &dim);

  dim2 = dim * dim;

  vecM = (double*)malloc(dim2 * sizeof(double));
  q    = (double*)malloc(dim * sizeof(double));
  sol  = (double*)malloc(dim * sizeof(double));

  for (i = 0 ; i < dim ; ++i)  q[i]    = 0.0;
  for (i = 0 ; i < dim ; ++i)  sol[i]  = 0.0;
  for (i = 0 ; i < dim2 ; ++i) vecM[i] = 0.0;

  for (i = 0 ; i < dim ; ++i)
  {
    for (j = 0 ; j < dim ; ++j)
    {
      fscanf(LCPfile, "%s", val);
      vecM[ dim * j + i ] = atof(val);
    }
  }

  for (i = 0 ; i < dim ; ++i)
  {
    fscanf(LCPfile , "%s" , val);
    q[i] = atof(val);
  }

  fscanf(LCPfile , "%s" , val);

  if (!feof(LCPfile))
  {

    sol[0] = atof(val);

    for (i = 1 ; i < dim ; ++i)
    {
      fscanf(LCPfile , "%s" , val);
      sol[i] = atof(val);
    }
  }
  else
  {
    for (i = 0 ; i < dim ; ++i) sol[i] = 0.0;
  }

  fclose(LCPfile);

#ifdef BAVARD
  printf("\n");
  printf("\n solution confeti : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , sol[i]);
  printf("\n");
#endif

  z = malloc(dim * sizeof(double));
  w = malloc(dim * sizeof(double));

  /* NLGS */

  for (i = 0 ; i < dim ; ++i) z[i] = 0.0;

  info = solve_lcp(vecM , q , &dim , &meth_lcp1 , z , w , &iter , &criteria);

#ifdef BAVARD
  printf("\n           NLGS   : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , z[i]);
  printf(" ( %6d ) " , iter);
#endif

  /* CPG */

  for (i = 0 ; i < dim ; ++i) z[i] = 0.0;

  info = solve_lcp(vecM , q , &dim , &meth_lcp2 , z , w , &iter , &criteria);

#ifdef BAVARD
  printf("\n           CPG    : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , z[i]);
  printf(" ( %6d ) " , iter);
#endif

  /* Lemke */

  for (i = 0 ; i < dim ; ++i) z[i] = 0.0;

  info = solve_lcp(vecM , q , &dim , &meth_lcp3 , z , w , &iter , &criteria);

#ifdef BAVARD
  printf("\n           Lemke  : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , z[i]);
  printf(" ( %6d ) " , iter);
#endif

  free(q);
  free(sol);
  free(vecM);

  /***/

  printf(" \n");

  return 0;
}




