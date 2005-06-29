#ifndef NSDS_H
#define NSDS_H

#include "SiconosConst.h"

#include "DynamicalSystem.h"
#include "Interaction.h"
#include "EqualityConstraint.h"
#include "NSDSXML.h"

#include "check.h"
#include <iostream>
#include <vector>
#include <string>

enum dynamicalsystem {LAGRANGIANNLDS, LAGRANGIANTIDS, LINEARTIDS};

class Interaction;
class DynamicalSystem;
class EqualityConstraint;
class NSDSXML;

/** \class NonSmoothDynamicalSystem
 *  \brief This class describes the Non Smooth Dynamical System (NonSmoothDynamicalSystem)
 * composed with some dynamical systems, some algebraic constraints and some interactions between those systems
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 0.1
 *  \date (Creation) Apr 23, 2004
 *
 */

class NonSmoothDynamicalSystem
{
public:

  // --- CONSTRUCTORS/DESTRUCTOR ---

  /** \fn NonSmoothDynamicalSystem()
   *  \brief default constructor
   *  \param NSDSXML* : the XML object corresponding to the NonSmoothDynamicalSystem
   */
  NonSmoothDynamicalSystem();

  /** \fn NonSmoothDynamicalSystem(const NonSmoothDynamicalSystem&)
   *  \brief copy constructor
   *  \param  a ref to the NonSmoothDynamicalSystem to be copied
   */
  NonSmoothDynamicalSystem(const NonSmoothDynamicalSystem&);

  /** \fn NonSmoothDynamicalSystem(NSDSXML*)
   *  \brief xml constructor
   *  \param: the XML object corresponding to the NonSmoothDynamicalSystem
   */
  NonSmoothDynamicalSystem(NSDSXML*);

  /** \fn NonSmoothDynamicalSystem(bool)
   *  \brief constructor with indication concerning boundary conditions
   *  \param const bool : if true, the systems has boundary conditions
   */
  NonSmoothDynamicalSystem(const bool&);


  /** \fn NonSmoothDynamicalSystem(const string&)
   *  \brief constructs the object with his type (IVP or BVP)
   *  \param a string value which determines if the problem is BVP or IVP
   */
  NonSmoothDynamicalSystem(const std::string&);

  ~NonSmoothDynamicalSystem();

  // --- GETTERS/SETTERS ---

  /** \fn const bool isBVP(void)
   *  \brief get problem type (true if BVP)
   *  \return a bool
   */
  inline const bool isBVP() const
  {
    return BVP;
  }

  /** \fn const bool isIVP(void)
   *  \brief get problem type (true if IVP)
   *  \return a bool
   */
  inline const bool isIVP() const
  {
    return !BVP;
  }

  /** \fn void setBVP(const bool&)
   *  \brief set the NonSmoothDynamicalSystem to BVP, else it is IVP
   *  \param bool : true if BVP, false otherwise
   */
  inline void setBVP(const bool& newBvp)
  {
    BVP = newBvp;
  }

  /** \fn inline const int getDSVectorSize() const
   *  \brief get the size of the vector of Dynamical Systems
   *  \return int : the size of DSVector
   */
  inline const int getDSVectorSize() const
  {
    return DSVector.size();
  }

  /** \fn const vector<DynamicalSystem*> getDynamicalSystems()
   *  \brief get all the DynamicalSystem of the NonSmoothDynamicalSystem problem
   *  \return the vector of DS
   */
  inline const std::vector<DynamicalSystem*> getDynamicalSystems() const
  {
    return DSVector;
  }

  /** \fn DynamicalSystem* getDynamicalSystemPtr(const int& position)
   *  \brief get DynamicalSystem at indix position in vector
   *  \param an int
   *  \return a pointer on DynamicalSystem
   */
  DynamicalSystem* getDynamicalSystemPtr(const int&) const ;

  /** \fn DynamicalSystem* getDynamicalSystemPtrNumber(const int& I)
   *  \brief get Dynamical system number I
   *  \param the identifier of the DynamicalSystem to get
   *  \return a pointer on DynamicalSystem
   */
  DynamicalSystem* getDynamicalSystemPtrNumber(const int&) const ;

  /** \fn void setDynamicalSystems(const vector<DynamicalSystem*>&)
   *  \brief set the vector of DynamicalSystems
   *  \param vector<DynamicalSystem> : the new value for the vector
   */
  void setDynamicalSystems(const std::vector<DynamicalSystem*>&) ;

  /** \fn inline const int getInteractionVectorSize()  const
   *  \brief get the size of the vector of Interactions
   *  \return int
   */
  inline const int getInteractionVectorSize() const
  {
    return interactionVector.size();
  }

  /** \fn const vector<Interaction*> getInteractions() const
   *  \brief get the vector of interactions
   *  \return a vector
   */
  inline std::vector<Interaction*> getInteractions() const
  {
    return interactionVector;
  }

  /** \fn Interaction* getInteractionPtr(const int& N) const
   *  \brief get interaction at position N in vectorInteraction
   *  \param the position of the Interaction to get
   *  \return a pointer on interaction
   */
  Interaction* getInteractionPtr(const int&) const ;

  /** \fn Interaction* getInteractionPtrNumber(const int& N) const
   *  \brief get interaction number N
   *  \param the identifier of the Interaction to get
   *  \return a pointer on interaction
   */
  Interaction* getInteractionPtrNumber(const int&) const ;

  /** \fn void setInteractions(const vector<Interaction*>&)
   *  \brief set the vector of interactions
   *  \param vector<Interaction> : the new value for the vector
   */
  void setInteractions(const std::vector<Interaction*>&) ;

  /** \fn vector<EqualityConstraint*> getEqualityConstraints(void)
   *  \brief get the vector of algebraic constraints
   *  \return vector of EqualityConstraint
   */
  inline const std::vector<EqualityConstraint*> getEqualityConstraints(void) const
  {
    return ecVector;
  }

  /** \fn EqualityConstraint* getEqualityConstraintPtr(const int& N) const
   *  \brief get algebraic constraint at position N in vectorEc
   *  \param int : the position of the ec to get
   *  \return a pointer on EC
   */
  EqualityConstraint* getEqualityConstraintPtr(const int&) const;

  /** \fn void setEqualityConstraints(const vector<EqualityConstraint*>& )
   *  \brief set the vector of algebraic constraints
   *  \param vector<EqualityConstraint*> : new value for the vector
   */
  void setEqualityConstraints(const std::vector<EqualityConstraint*>& newEcVect) ;

  /** \fn inline NSDSXML* getNSDSXMLPtr()
   *  \brief get the xml linked object
   *  \return a pointer on NSDSXML
   */
  inline NSDSXML* getNSDSXMLPtr() const
  {
    return nsdsxml;
  }

  /** \fn inline void setNSDSXMLPtr( NSDSXML *nsdsxml )
   *  \brief set the xml linked object
   *  \param NSDSXML* : a pointer on NSDSXML* to link
   */
  inline void setNSDSXMLPtr(NSDSXML *newNsdsxml)
  {
    nsdsxml = newNsdsxml;
  }

  // --- OTHER FUNCTIONS ---

  /** \fn bool hasDynamicalSystemNumber(const int& N)  const
   *  \brief check if DynamicalSystem number N exists
   *  \param the identifier of the DynamicalSystem to get
   *  \return bool
   */
  bool hasDynamicalSystemNumber(const int&) const ;
  /** \fn bool hasInteractionNumber(const int& N)
   *  \brief check if Interaction number N exists
   *  \param the identifier of the Interaction to get
   *  \return bool
   */
  bool hasInteractionNumber(const int&) const;

  /** \fn void saveNSDSToXML()
   *  \brief copy the data of the NonSmoothDynamicalSystem to the XML tree
   *  \exception RuntimeException
   */
  void saveNSDSToXML();

  /** \fn void display()
   *  \brief display the data of the Non Smooth Dynamical System
   */
  void display() const;

  /** \fn void addDynamicalSystem(DynamicalSystem*)
   *  \brief add a DynamicalSystem to the NonSmoothDynamicalSystem
   *  \param DynamicalSystem* : the DynamicalSystem to add
   */
  void addDynamicalSystem(DynamicalSystem*);

  /** \fn void addInteraction(Interaction*)
   *  \brief add an Interaction to the NonSmoothDynamicalSystem
   *  \param Interaction : the Interaction to add
   */
  void addInteraction(Interaction*);

  /** \fn Interaction* addInteraction(const int& number, const int& nInter, vector<int>* status, vector<DynamicalSystem*>*)
   *  \brief add an Interaction to the NonSmoothDynamicalSystem
   *  \param int : the number of the Interaction
   *  \param int : the size of the y vector of the Interaction
   *  \param vector<int> : the status of the interaction
   */
  Interaction* addInteraction(const int& = -1, const int& = -1, std::vector<int>* = NULL, std::vector<DynamicalSystem*>* = NULL);

  /** \fn void addEqualityConstraint(EqualityConstraint*)
   *  \brief add an EqualityConstraint to the NonSmoothDynamicalSystem
   *  \param EqualityConstraint* : the EqualityConstraint to add
   */
  void addEqualityConstraint(EqualityConstraint*);

  /** \fn double nsdsConvergenceIndicator() const
   *  \brief calculate an indicator that gives convergence information for the DSs
   *  \return a double
   */
  double nsdsConvergenceIndicator() ;

private:
  /** TRUE if the NonSmoothDynamicalSystem is a boundary value problem*/
  bool BVP;

  /** contains Dynamic systems of the simulation */
  std::vector<DynamicalSystem*> DSVector;

  /** contains the Interactions */
  std::vector<Interaction*> interactionVector;

  /** contains the EqualityConstraints */
  std::vector<EqualityConstraint*> ecVector;

  /** the XML object linked to the NonSmoothDynamicalSystem to read XML data */
  NSDSXML *nsdsxml;

  /** Flags to check wheter pointers were allocated in class constructors or not */
  std::vector<bool> isDSVectorAllocatedIn;
  std::vector<bool> isInteractionVectorAllocatedIn;
  std::vector<bool> isEcVectorAllocatedIn;
};

#endif
