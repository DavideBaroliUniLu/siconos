/* Siconos-Kernel version 2.0.1, Copyright INRIA 2005-2006.
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

/*! \file FirstOrderLinearTIRXML.h
  \brief xml management for first order linear time invariant relations.
*/

#ifndef FirstOrderLinearTIRXML_H
#define FirstOrderLinearTIRXML_H

#include "FirstOrderRXML.h"
#include "SimpleMatrix.h"
#include "SimpleVector.h"

/** XML management for FirstOrderLinearTIR
 *
 *  \author SICONOS Development Team - copyright INRIA
 *   \version 2.0.1.
 *   \date 05/13/2004
 *
 */
class FirstOrderLinearTIRXML : public FirstOrderRXML
{

private:

  /** C matrix node */
  xmlNodePtr CNode;
  /** D matrix node */
  xmlNodePtr DNode;
  /** F matrix node */
  xmlNodePtr FNode;
  /** e vector node */
  xmlNodePtr eNode;
  /** B matrix node */
  xmlNodePtr BNode;

  /** Default constructor */
  FirstOrderLinearTIRXML();

public:

  /** Build a FirstOrderLinearTIRXML object from a DOM tree describing a Relation with LTI type
   *   \param xml pointer to relation data.
   */
  FirstOrderLinearTIRXML(xmlNodePtr);

  /** Destructor*/
  ~FirstOrderLinearTIRXML();

  /** Return the C of the LTIRelationXML
   *   \return The C SimpleMatrix of the LTIRelationXML
   */
  inline SimpleMatrix getC()
  {
    return SiconosDOMTreeTools::getSiconosMatrixValue(CNode);
  }

  /** Return the D of the LTIRelationXML
   *   \return The D SimpleMatrix of the LTIRelationXML
   */
  inline SimpleMatrix getD()
  {
    return SiconosDOMTreeTools::getSiconosMatrixValue(DNode);
  }

  /** Return the F of the LTIRelationXML
   *   \return The F SimpleMatrix of the LTIRelationXML
   */
  inline SimpleMatrix getF()
  {
    return SiconosDOMTreeTools::getSiconosMatrixValue(FNode);
  }

  /** Return e of the LTIRelationXML
   *   \return SimpleVector : e of LTIRelationXML
   */
  inline SimpleVector getE()
  {
    return SiconosDOMTreeTools::getSiconosVectorValue(eNode);
  }

  /** Return the B of the LTIRelationXML
   *   \return The B SimpleMatrix of the LTIRelationXML
   */
  inline SimpleMatrix getB()
  {
    return SiconosDOMTreeTools::getSiconosMatrixValue(BNode);
  }

  /** Change the C matrix values (in xml file or external data file switch his origin position)
   *   \param SiconosMatrix matrix : the new value for C matrix
   */
  void setC(const SiconosMatrix&);

  /** Change the D matrix values (in xml file or external data file switch his origin position)
   *   \param SiconosMatrix matrix : the new value for D matrix
   */
  void setD(const SiconosMatrix&);

  /** Change the F matrix values (in xml file or external data file switch his origin position)
   *   \param SiconosMatrix matrix : the new value for F matrix
   */
  void setF(const SiconosMatrix&);

  /** Change the e Vector values (in xml file or external data file switch his origin position)
   *   \param SiconosVector *vector : new value of e
   */
  void setE(const SiconosVector&);

  /** Change the B matrix values (in xml file or external data file switch his origin position)
   *   \param SiconosMatrix matrix : the new value for B matrix
   */
  void setB(const SiconosMatrix&);

  /** return true if CNode exists */
  inline bool hasC() const
  {
    return (CNode != NULL);
  }
  /** return true if DNode exists */
  inline bool hasD() const
  {
    return (DNode != NULL);
  }
  /** return true if FNode exists */
  inline bool hasF() const
  {
    return (FNode != NULL);
  }
  /** return true if eNode exists */
  inline bool hasE() const
  {
    return (eNode != NULL);
  }
  /** return true if BNode exists */
  inline bool hasB() const
  {
    return (BNode != NULL);
  }

};


#endif
