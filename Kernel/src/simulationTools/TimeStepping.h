/* Siconos-Kernel version 1.2.0, Copyright INRIA 2005-2006.
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
#ifndef TIMESTEPPING_H
#define TIMESTEPPING_H

#include "Simulation.h"
#include "Moreau.h"
#include "LCP.h"

/** \class TimeStepping
 *  \brief Specific simulation, using time stepping schemes.
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 1.2.0.
 *  \date (Creation) Apr 26, 2004
 *
 */
class TimeStepping : public Simulation
{
public:

  /** \fn TimeStepping(Model * = NULL)
   *  \brief Default constructor
   *  \param a pointer to the model that owns this simulation. NULL Model leads to exception
   */
  TimeStepping(Model* = NULL);

  /** \fn TimeStepping(SimulationXML*, Model*)
   *  \brief constructor with XML object for TimeStepping
   *  \param SimulationXML* : the XML object corresponding
   *  \param Model* : the Model which contains the Simulation
   */
  TimeStepping(SimulationXML*, Model*);

  ~TimeStepping();

  /** \fn void addOneStepNSProblemPtr(OneStepNSProblem*)
   *  \brief add a OneStepNSProblem of the Simulation (if its not the first, it needs to have an id clearly defined)
   *  \param a pointer to OneStepNSProblem
   */
  void addOneStepNSProblemPtr(OneStepNSProblem*);

  /** \fn virtual void updateIndexSet(const unsigned int i) = 0;
   *  \brief update indexSets[i] of the topology, using current y and lambda values of Interactions.
   *  \param unsigned int: the number of the set to be updated
   */
  void updateIndexSet(const unsigned int);

  /** \fn void initialize()
   *  \brief executes the complete initialisation of Simulation (OneStepIntegrators, OneStepNSProblem, TImediscretisation) with the XML Object
   */
  void initialize();

  /** \fn void nextStep()
   *  \brief increments all the Integrators to next step of the simulation
   */
  void nextStep();

  /** \fn void update(const unsigned int)
   *  \brief update input, state of each dynamical system and output
   *  \param lambda order used to compute input
   */
  void update(const unsigned int);

  /** \fn void run()
   *  \brief run the simulation, from t0 to T
   */
  void run();

  /** \fn void computeOneStep()
   *  \brief run one step of the simulation
   */
  void computeOneStep();

  /** \fn TimeStepping* convert (Simulation* str)
   *  \brief encapsulates an operation of dynamic casting. Needed by Python interface.
   *  \param Simulation* : the Simulation which must be converted
   * \return a pointer on the Simulation if it is of the right type, NULL otherwise
   */
  static TimeStepping* convert(Simulation* str);
};

#endif // TIMESTEPPING_H





