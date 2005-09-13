/*!\file solve_rp.c

  This subroutine allows the primal resolution of relay problems.

  Try \f$(z,w)\f$ such that:
\f$
\left\lbrace
\begin{array}{l}
M z- w=q\\
-w \in \partial\psi_{[-a, a]}(z)\\
\end{array}
\right.
\f$

 here M is an n by n  matrix, q an n-dimensional vector, z an n-dimensional  vector and w an n-dimensional vector.
 This system of equations and inequalities is solved thanks to @ref pr solvers.
 The routine's call is due to the function solve_rp.c.
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef MEXFLAG
#include "solverpack.h"
#endif
#include <time.h>

/*!\fn int solve_rp (double vec[],double *q,int *nn, method *pt,double z[],double w[])

   solve_rp is a generic interface allowing the call of one of the PR solvers.

   \param double* : vec On enter double vector containing the components of the double matrix with a fortran90 allocation.
   \param double* : q On enter a pointer over doubles containing the components of the second member of the system.
   \param int* : nn On enter a pointer over integers, the dimension of the second member.
   \param method* : pt On enter a pointer other a structure (::method).
   \param double[] : z On return real vector, the solution of the problem.
   \param double[] : w On return real vector, the solution of the problem.

  \return  On return int, the termination reason (0 is successful otherwise 1).

   \author Nineb Sheherazade.
 */
int solve_rp(double *vec, double *q, int *nn, method *pt, double z[], double w[])
{
  int info = -1, choix, it_end, fail;
  char mot1[10] = "Gsnl", mot2[10] = "Gcp", mot3[10] = "Latin";
  double res;
  int n = *nn, i;

  clock_t t1 = clock();


  if (strcmp(pt->rp.nom_method, mot1) == 0)
    rp_nlgs(vec, q, &n, pt->rp.a, pt->rp.b, & pt->rp.itermax, & pt->rp.tol, z, w, &it_end, &res, &info);
  else if (strcmp(pt->rp.nom_method, mot2) == 0)
  {
    /*    cfp_gcp_(vec,q,&n,& pt->cfp.mu,& pt->cfp.itermax,& pt->cfp.tol,z,w,&it_end,&res,&info);*/
  }
  else if (strcmp(pt->rp.nom_method, mot3) == 0)
    rp_latin(vec, q, &n, &pt->rp.k_latin, pt->rp.a, pt->rp.b, &pt->rp.itermax, &pt->rp.tol, z, w, &it_end, &res, &info);
  else printf("Warning : Unknown solving method : %s\n", pt->rp.nom_method);

  clock_t t2 = clock();
  printf("%.4lf seconds of processing\n", (t2 - t1) / (double)CLOCKS_PER_SEC);

  return info;
}
