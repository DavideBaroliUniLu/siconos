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

#ifndef SICONOS_SETS_H
#define SICONOS_SETS_H

/*!\file SiconosSets.h
 * \brief Sets structures used in Siconos:
 * - box constraints \f$ K = \{ x \in \mathbb{R}^n | lb_i \leq x_i \leq ub_i\quad i = 1 .. n\}\f$
 * - polytopes and polyhedra \f$ \{ x \in \mathbb{R}^n | Hx\leq K\}\f$
 * - sets defined by a set of inequalities \f$\{g_i(x)\leq 0\}\f$ ( work in progress)
 *
 * \author Olivier Huber
*/

#include "NumericsFwd.h"

/** \struct generic_set SiconosSets.h
 * Generic set (can be seen as a kind of ``base class''). Mainly used to infer
 * the type of set (box, polytope, ...) to properly operate on it
 */
typedef struct {
  int id; /**< type of the set */
} generic_set;

/** The positive orthant does not need to contain much info */
typedef generic_set positive_orthant;

/** \struct box_constraints SiconosSets.h
 * Definition of a rectangular set, also known as box 
 */
typedef struct
{
  int id; /**< id of the structure, usually solver specific */
  double* lb; /**< lower bounds */
  double* ub; /**< upper bounds */
} box_constraints;

/** \struct Polyhedron SiconosSets.h
 * Definition of a polytope in terms of (H,K) representation
 */
struct polyhedron
{
  int id; /**< id of the structure, usually solver specific */
  unsigned size_ineq; /**< number of inequalities */
  unsigned size_eq; /**< number of equalities */
  double* H; /**< H matrix in an (H,K) representation of a polytope H x <= K */
  double* K; /**< K vector in an (H,K) representation of a polytope H x <= K */
  double* Heq; /**< Heq matrix for the equality constraints Heq x = Keq */
  double* Keq; /**< Keq vector for the equality constraints Heq x = Keq */
};

enum SICONOS_SET_ID { SICONOS_SET_POSITIVE_ORTHANT, SICONOS_SET_BOX, SICONOS_SET_POLYHEDRON };

#include "SiconosConfig.h"

#if defined(__cplusplus) && !defined(BUILD_AS_CPP)
extern "C"
{
#endif

  /** set set id
   * \param set the set which id should be set
   * \param id the id of the set
   */
  static inline void set_set_id(void* set, unsigned id)
  {
    ((generic_set*)set)->id = id;
  }

  /** project the point x on a set
   * \param n the size of x
   * \param x the point to project
   * \param set the set on which we project x
   */
  void project_on_set(int n, double* x, void* set);

  /** free a set
   * \param set struct to be freed
   */
  void free_siconos_set(void* set);

  /** free a box struct
  * \param b the box struct to free
  */
  void free_box(box_constraints* b);

  /** free a Polyhedron struct
  * \param poly the Polyhedron struct to free
  */
  void free_polyhedron(polyhedron* poly);


#if defined(__cplusplus) && !defined(BUILD_AS_CPP)
}
#endif

#endif


