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
#ifndef TIMESTEPPING_H
#define TIMESTEPPING_H

#include "Strategy.h"
/** \class TimeStepping
 *  \brief It's a way to drive a simulation, where the resolution is only managed by the time
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 1.0
 *  \date (Creation) Apr 26, 2004
 *
 */
class TimeStepping : public Strategy
{
public:

  /** \fn TimeStepping()
   *  \brief Default constructor
   */
  TimeStepping();

  /** \fn TimeStepping(StrategyXML*, Model*)
   *  \brief constructor with XML object for TimeStepping
   *  \param StrategyXML* : the XML object corresponding
   *  \param Model* : the Model which contains the Strategy
   */
  TimeStepping(StrategyXML*, Model*);

  ~TimeStepping();

  /** \fn void createStrategy(StrategyXML * strategyXML, Model * model)
   *  \brief create the Strategy with an xml file, or the needed data
   *  \param StrategyXML* : the StrategyXML linked to this Strategy
   *  \param Model* : the Model which contains this Strategy
   *  \exception RuntimeException
   */
  void createStrategy(StrategyXML * newStrategyXML, Model * newModel);

  /** \fn TimeStepping* convert (Strategy* str)
   *  \brief encapsulates an operation of dynamic casting. Needed by Python interface.
   *  \param Strategy* : the Strategy which must be converted
   * \return a pointer on the Strategy if it is of the right type, NULL otherwise
   */
  static TimeStepping* convert(Strategy* str);
};

#endif // TIMESTEPPING_H
