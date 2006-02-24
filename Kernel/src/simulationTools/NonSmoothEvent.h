/* Siconos-Kernel version 1.1.2, Copyright INRIA 2005-2006.
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
#ifndef NONSMOOTHEVENT_H
#define NONSMOOTHEVENT_H

/** \class NonSmoothEvent
 *  \brief class derived from Event one: events due to non smooth behavior (contact occurence ...)
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 1.1.2.
 *  \date (Creation) February 21, 2006
 *
 */

#include "Event.h"

class NonSmoothEvent : public Event
{

private:

  /** Default constructor */
  NonSmoothEvent();

public:

  /** \fn NonSmoothEvent(const NonSmoothEvent&)
   *  \brief copy constructor
   *  \param the timeDiscrEvent to be copied
   */
  NonSmoothEvent(const NonSmoothEvent&);

  /** \fn NonSmoothEvent(const unsigned long int&)
   *  \brief constructor with time value as a parameter
   *  \param an unsigned long int
   */
  NonSmoothEvent(const unsigned long int&);

  /** \fn ~NonSmoothEvent()
   *  \brief destructor
   */
  ~NonSmoothEvent();

  /** \fn void process()
   *  \brief
   */
  void process();
};

#endif // NonSmoothEvent_H
