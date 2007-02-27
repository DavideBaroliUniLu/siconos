/* Siconos-sample version 2.0.1, Copyright INRIA 2005-2006.
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

#include <stdio.h>
#include <math.h>

extern double gravity;
extern double m1;
extern double l1;





//extern "C" void mass(unsigned int sizeOfq, const double *q, double *mass, double* param)
//{
//  mass[0]= (m1*l1);
//}

extern "C" void FInt(unsigned int sizeOfq, double time, const double *q, const double *velocity, double *fInt, double* param)
{
  fInt[0] =  m1 * sin(q[0]) * gravity;
}
extern "C" void jacobianQFInt(unsigned int sizeOfq, double time, const double *q, const double *velocity, double *jacob, double* param)
{
  jacob[0] =  cos(q[0]) * gravity * (m1);
}

extern "C" void jacobianVFInt(unsigned int sizeOfq, double time, const double *q, const double *velocity, double *jacob, double* param)
{
  jacob[0] = 0.0;
}

// extern "C" void FExt(unsigned int  sizeOfq, double time, double *fExt, double* param)
// {
//   unsigned int i;
//   unsigned int n = sizeOfq;
//   for(i = 0; i<n ; i++)
//     fExt[i] = 0.0;

// }


extern "C" void h0(unsigned int sizeOfq, const double* q, unsigned int sizeOfY, double* y, double* param)
{
  y[0] = l1 * sin(q[0]);
}

extern "C" void G0(unsigned int sizeOfq, const double* q, unsigned int sizeOfY, double* G, double* param)
{
  G[0] = l1 * cos(q[0]);
  G[1] = 0.0;
}
