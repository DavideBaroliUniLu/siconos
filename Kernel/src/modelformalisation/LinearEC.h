//$Id: LinearEC.h,v 1.5 2005/03/15 09:57:47 jbarbier Exp $
#ifndef LINEAREC_H
#define LINEAREC_H

#include "EqualityConstraint.h"

/** \class LinearEC
 *  \brief Linear Equality Constraint
 *  \author Jean-Michel Barbier
 *  \version 0.1
 *  \date 17/01/2005
 *
 * $Date: 2005/03/15 09:57:47 $
 * $Revision: 1.5 $
 * $Author: jbarbier $
 * $Source: /CVS/Siconos/SICONOS/src/modelformalisation/LinearEC.h,v $
 *
 */

class LinearEC : public EqualityConstraint
{
public:

  /** \fn LinearEC(void);
   * \brief default constructor
   */
  LinearEC();
  virtual ~LinearEC();

  LinearEC(EqualityConstraintXML*);

  /** \fn void createEqualityConstraint(LagrangianECXML * ecXML)
   *  \brief allows to create the EqualityConstraint with an xml file, or the needed data
   *  \param LagrangianECXML * : the XML object for this EqualityConstraint
   *  \exception RuntimeException
   */
  void createEqualityConstraint(EqualityConstraintXML * ecXML , int number = -1,
                                SiconosMatrix *G = NULL, vector<DSInputOutput*> *dsioVector = NULL);
};

#endif // LINEAREC_H

//$Log: LinearEC.h,v $
//Revision 1.5  2005/03/15 09:57:47  jbarbier
//- EqualityConstraint save OK
//
//Revision 1.4  2005/03/14 16:05:27  jbarbier
//- manual creation of DSInputOutput saving OK
//
//- in progress for EqualityConstraint
//
//Revision 1.3  2005/03/11 15:06:20  jbarbier
//- save to XML methods of EqualityConstraint and DSInputOutput added
//
//- XML loading process modified : Model loads NSDS, then NSDS loads the DynamicalSystems, EqualityConstraints, Interactions; Modle loads Strategy, then Strategy loads TimeDiscretisation, then the Integrators, then the OneStepNSProblem
//
//Revision 1.2  2005/03/09 15:30:32  jbarbier
//- add of LagrangianEC class
//
//- in progress : implementation of the EqualityConstraint and DSInputOutput - create methods
//
//Revision 1.1  2005/01/17 10:56:25  jbarbier
//- classes EqualityConstraint and DSInputOutput added with inherited classes
//
//- classes EqualityConstraintXML and DSInputOutputXML added with inherited classes
//