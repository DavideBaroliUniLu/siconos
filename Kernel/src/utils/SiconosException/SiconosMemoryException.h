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
#ifndef SICONOSMEMORYEXCEPTION_H
#define SICONOSMEMORYEXCEPTION_H

#include "SiconosException.h"

// --------------------------------------------------------------------------
class SiconosMemoryException : public SiconosException
{
public:

  /**
   * constructor
   */
  SiconosMemoryException();

  /**
   * constructor
   * @param std::string which describe the exception
   */
  SiconosMemoryException(const std::string& report);

  /**
   * destructor
   */
  ~SiconosMemoryException();

  static void selfThrow() ;

  static void selfThrow(const std::string& report) ;

};

#endif //SICONOSMEMORYEXCEPTION_H
