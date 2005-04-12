#ifndef NLINEARBC_H
#define NLINEARBC_H

#include "BoundaryCondition.h"

/** \class NLinearBC
 *  \brief kind of BoundaryCondition
*  \author SICONOS Development Team - copyright INRIA
 *  \version 0.1
 *  \date (Creation) May 6, 2004
 *
 *
 *
 */
class NLinearBC : public BoundaryCondition
{
public:

  /** \fn NLinearBC()
   *  \brief default constructor
   */
  NLinearBC();

  /** \fn NLinearBC(BoundaryConditionXML*)
   *  \brief constructor with XML object of the boundary condition
   *  \param BoundaryConditionXML* : the XML object corresponding
   */
  NLinearBC(BoundaryConditionXML*);

  ~NLinearBC();


  /////////////////////

  /** \fn void saveBCToXML()
   *  \brief copy the data of the BoundaryCondition to the XML tree
   */
  void saveBCToXML();

  /** \fn void createBoundaryCondition(BoundaryConditionXML * bcXML)
   *  \brief allows to create the BoundaryCondition with an xml file, or the needed data
   *  \param BoundaryConditionXML* : the XML object for this BoundaryCondition
   *  \exception RuntimeException
   */
  void createBoundaryCondition(BoundaryConditionXML * bcXML);

  /** \fn NLinearBC* convert (BoundaryCondition* bc)
   *  \brief encapsulates an operation of dynamic casting. Needed by Python interface.
   *  \param BoundaryCondition* : the boundary condition which must be converted
   * \return a pointer on the boundary condition if it is of the right type, NULL otherwise
   */
  static NLinearBC* convert(BoundaryCondition* bc);

protected:
  /** \fn void fillBCWithBCXML()
   *  \brief uses the BoundaryConditionXML of the BoundaryCondition to fill the fields of this BoundaryCondition
   */
  void fillBCWithBCXML();
};

#endif // NLINEARBC_H


