#ifndef LAGRANGIANRELATION_H
#define LAGRANGIANRELATION_H

#include "Relation.h"
#include "LagrangianRXML.h"

/** \class LagrangianR
 *  \brief Lagrangian (Non Linear) Relation
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 0.1
 *  \date Apr 27, 2004
 *
 *
 */
class LagrangianRXML;

class LagrangianR : public Relation
{
public:

  /** \fn LagrangianR(Interaction* =NULL);
   * \brief default constructor
   *  \param Interaction*: a pointer to the interaction that owns this relation (optional)
   */
  LagrangianR(Interaction* = NULL);

  /** \fn void LagrangianR(RelationXML*, Interaction* =NULL)
   *  \brief constructor from xml file
   *  \param relationXML
   *  \param Interaction*: a pointer to the interaction that owns this relation (optional)
   *  \exception RuntimeException
   */
  LagrangianR(RelationXML*, Interaction* = NULL);

  /** \fn void LagrangianR(const string& computeInput,const string& computeOutput, Interaction* =NULL)
   *  \brief constructor from a set of data
   *  \param string : the name of the plugin for computeInput
   *  \param string : the name of the plugin for computeOutput
   *  \param Interaction*: a pointer to the interaction that owns this relation (optional)
   *  \exception RuntimeException
   */
  LagrangianR(const std::string&, const std::string&, Interaction* = NULL);

  /** \fn LagrangianR(const Relation&)
   *  \brief copy constructor
   *  \param a relation to copy
   *  \param Interaction*: a pointer to the interaction that owns this relation (optional)
   *  warning: the interaction link is not copied, set a new one!
   */
  LagrangianR(const Relation &, Interaction* = NULL);

  ~LagrangianR();

  /** \fn void computeJacobian(void);
   * \brief default function to compute Jacobian
   */
  void computeJacobian(void);


  /** \fn void saveRelationToXML()
   *  \brief copy the data of the Relation to the XML tree
   */
  void saveRelationToXML();


  /** \fn LagrangianR* convert (Relation *r)
   *  \brief encapsulates an operation of dynamic casting. Needed by Python interface.
   *  \param Relation* : the relation which must be converted
   * \return a pointer on the relation if it is of the right type, NULL otherwise
   */
  static LagrangianR* convert(Relation *r);


private:

  /** class for manage plugin (open, close librairy...) */
  SiconosSharedLibrary cShared;

  /** \fn void (*computeJacobianPtr)(void);
   * \brief to be defined
   */

  void (*computeJacobianPtr)(int* sizeOfQ, double* qPtr, int* sizeOfY, double* jacobPtr);

  void (*computeHPtr)(int* sizeOfQ, double* qPtr, int* sizeOfY, double* yPtr);

};

#endif // LAGRANGIANRELATION_H
