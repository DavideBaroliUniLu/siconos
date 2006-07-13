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
#ifndef ONESTEPINTEGRATOR_H
#define ONESTEPINTEGRATOR_H

#include "RuntimeCmp.h"
#include "TimeDiscretisation.h"
#include "DynamicalSystem.h"
#include "SiconosMatrix.h"
#include "SiconosVector.h"
#include "OneStepIntegratorXML.h"
#include "Simulation.h"
#include "SiconosConst.h"
#include "SiconosNumerics.h"
#include "DynamicalSystemsSet.h"
#include "check.h"
#include <iostream>
#include <vector>
#include <set>

class DynamicalSystem;
class TimeDiscretisation;
class OneStepIntegratorXML;
class Simulation;

/** \class OneStepIntegrator
 *  \brief Generic object to manage DynamicalSystem(s) time-integration
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 1.2.0.
 *  \date (Creation) Apr 26, 2004
 *
 * !!! This is a virtual class, interface for some specific integrators !!!
 *
 * At the time, available integrators are: Moreau and Lsodar
 *
 */


class OneStepIntegrator
{
protected:

  /** type/name of the Integrator */
  std::string integratorType;

  /** a set of DynamicalSystem to integrate */
  DynamicalSystemsSet OSIDynamicalSystems;

  /** a set of Interactions to define a list of *
   * dynamical systems to be integrated, with some *
   * constraints to be taken into account */
  InteractionsSet OSIInteractions;

  /** size of the memory for the integrator */
  unsigned int sizeMem;

  /** A link to the simulation that owns this OSI */
  Simulation * simulationLink;

  /** the corresponding XML object */
  OneStepIntegratorXML *integratorXml;

  /** \fn OneStepIntegrator()
   *  \brief default constructor
   */
  OneStepIntegrator();

  /** \fn OneStepIntegrator(const std::string, Simulation*)
   *  \brief basic constructor with Id and simulation
   *  \param string, integrator type/name
   *  \param Simulation * : the simulation that owns the osi
   */
  OneStepIntegrator(const std::string, Simulation*);

  /** \fn OneStepIntegrator(const std::string, OneStepIntegratorXML*, Simulation*)
   *  \brief constructor from xml file
   *  \param string, integrator type/name
   *  \param OneStepIntegratorXML* : the corresponding XML object
   *  \param Simulation * : the simulation that owns the osi
   */
  OneStepIntegrator(const std::string, OneStepIntegratorXML*, Simulation*);

  /** \fn OneStepIntegrator(const std::string, const DynamicalSystemsSet& , Simulation*)
   *  \brief constructor from a minimum set of data
   *  \param string, integrator type/name
   *  \param DynamicalSystem* : the DynamicalSystem to be integrated
   *  \param Simulation * : the simulation that owns the osi
   */
  OneStepIntegrator(const std::string, const DynamicalSystemsSet&, Simulation*);

public:

  /** \fn ~OneStepIntegrator()
   *  \brief destructor
   */
  virtual ~OneStepIntegrator();

  // --- GETTERS/SETTERS ---

  /** \fn inline const string getType() const
   *  \brief get the type of the OneStepIntegrator
   *  \return string : the type of the OneStepIntegrator
   */
  inline const std::string getType() const
  {
    return integratorType;
  }

  /** \fn inline void setType(const string)
   *  \brief set the type of the OneStepIntegrator
   *  \return string : the type of the OneStepIntegrator
   */
  inline void setType(const std::string newType)
  {
    integratorType = newType;
  }

  /** \fn const DynamicalSystemsSet getDynamicalSystems()
    *  \brief get the set of DynamicalSystem associated with the Integrator
    *  \return a DynamicalSystemsSet
    */
  inline DynamicalSystemsSet getDynamicalSystems() const
  {
    return OSIDynamicalSystems;
  };

  /** \fn void setDynamicalSystems(const DynamicalSystemsSet&)
   *  \brief set the DynamicalSystem list of this Integrator
   *  \param a DynamicalSystemsSet
   */
  void setDynamicalSystems(const DynamicalSystemsSet&);

  /** \fn const InteractionsSet getInteractions()
    *  \brief get the set of Interactions associated with the Integrator
    *  \return an InteractionsSet
    */
  inline const InteractionsSet getInteractions() const
  {
    return OSIInteractions;
  };

  /** \fn void setInteractionst(const InteractionsSet&)
   *  \brief set the Interaction list of this Integrator
   *  \param an InteractionsSet
   */
  void setInteractions(const InteractionsSet&);

  /** \fn * getDynamicalSystemPtr()
    *  \brief get the DynamicalSystem associated with the Integrator
    *  \return a DS
    */
  //  inline DynamicalSystem* getDynamicalSystemPtr() const {return ds;};

  /** \fn void setDynamicalSystemPtr(DynamicalSystem*)
   *  \brief set the DynamicalSystem of this Integrator
   *  \param a pointer to a DynamicalSystem
   */
  //inline void setDynamicalSystemPtr(DynamicalSystem* newDs) {ds = newDs;};

  /** \fn const unsigned int getSizeMem() const
   *  \brief get sizeMem value
   *  \return an unsigned int
   */
  inline const unsigned int getSizeMem() const
  {
    return sizeMem;
  };

  /** \fn void setSizeMem(const unsigned int)
   *  \brief set sizeMem
   *  \param an unsigned int
   */
  inline void setSizeMem(const unsigned int newValue)
  {
    sizeMem = newValue;
  };

  /** \fn inline Simulation* getSimulationPtr()
    *  \brief get the Simulation that owns the OneStepIntegrator
    *  \return a pointer to Simulation
    */
  inline Simulation* getSimulationPtr() const
  {
    return simulationLink;
  }

  /** \fn inline void setSimulationPtr(Simulation*)
   *  \brief set the Simulation of the OneStepIntegrator
   *  \param a pointer to Simulation
   */
  inline void setSimulationPtr(Simulation* newS)
  {
    simulationLink = newS;
  }

  /** \fn inline OneStepIntegratorXML* getOneStepIntegratorXMLPtr()
    *  \brief get the OneStepIntegratorXML of the OneStepIntegrator
    *  \return a pointer on the OneStepIntegratorXML of the OneStepIntegrator
    */
  inline OneStepIntegratorXML* getOneStepIntegratorXMLPtr() const
  {
    return integratorXml;
  }

  /** \fn inline void setOneStepIntegratorXMLPtr(OneStepIntegratorXML* integratorXml)
   *  \brief set the OneStepIntegratorXML of the OneStepIntegrator
   *  \param OneStepIntegratorXML* : the pointer to set the OneStepIntegratorXML
   */
  inline void setOneStepIntegratorXMLPtr(OneStepIntegratorXML* newIntegratorXml)
  {
    integratorXml = newIntegratorXml;
  }

  // --- OTHERS ... ---

  /** \fn void initialize()
   *  \brief initialise the integrator
   */
  virtual void initialize() = 0;

  /** \fn void nextStep()
   *  \brief prepares the DynamicalSystem for the next time step, push x in Memory
   *  \param to be defined
   *  \exception to be defined
   *  \return void
   */
  virtual void nextStep();

  /** \fn void computeFreeState()
   *  \brief integrates the Dynamical System linked to this integrator without constraints
   */
  virtual void computeFreeState() = 0;

  /** \fn void integrate(const double, const double, double, bool)
   *  \brief integrate the system, between tinit and tend (->iout=true), with possible stop at tout (->iout=false)
   *  \param double: tinit, initial time
   *  \param double: tend, end time
   *  \param double: tout, real end time
   *  \param bool: true if tend is reached, else false.
   */
  virtual void integrate(const double, const double, double, bool) = 0;

  /** \fn void updateState(const double time)
   *  \brief update the state of the DynamicalSystem attached to this Integrator
   *  \param double: current time
   */
  virtual void updateState(const double) = 0;

  /** \fn void display()
   *  \brief print the data to the screen
   */
  virtual  void display() const;

  /** \fn void saveIntegratorToXML()
   *  \brief copy the data of the OneStepNSProblem to the XML tree
   *  \exception RuntimeException
   */
  virtual void saveIntegratorToXML();

};

#endif // ONESTEPINTEGRATOR_H
