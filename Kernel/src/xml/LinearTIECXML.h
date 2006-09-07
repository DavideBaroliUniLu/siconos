/* Siconos-Kernel version 1.3.0, Copyright INRIA 2005-2006.
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
#ifndef LINEARTIECXML_H
#define LINEARTIECXML_H

#include "LinearECXML.h"

/** \class LinearTIECXML
 *  \brief object to manage XML data of an Linear Time Invariant EqualityConstraint
*  \author SICONOS Development Team - copyright INRIA
 *  \version 1.3.0.
 *  \date 17/01/2005
 *
 *
 */

class LinearTIECXML : public LinearECXML
{
public:

  LinearTIECXML();

  /** \fn LinearTIECXML(xmlNode * , vector<int> )
  *   \brief Build a EqualityConstraintXML object from a DOM tree describing a EqualityConstraint
  *   \param xmlNode* : the EqualityConstraint DOM tree
  *   \param vector<int>  : vector of DSXML numbers to verify DS concerned by the EqualityConstraint (identified by number) exists
  */
  LinearTIECXML(xmlNode*, std::vector<int>);
  ~LinearTIECXML();
};

#endif // LINEARTIECXML_H

