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

#include "TimeStepping.h"
#include "LCP.h"
#include "FrictionContact2D.h"
#include "FrictionContact3D.h"

using namespace std;

// --- Default constructor ---
TimeStepping::TimeStepping(Model * newModel): Simulation(newModel, "TimeStepping")
{}

// --- XML constructor ---
TimeStepping::TimeStepping(SimulationXML* strxml, Model *newModel): Simulation(strxml, newModel, "TimeStepping")
{
  // === One Step NS Problem ===
  // For time stepping, only one non smooth problem is built.
  if (simulationxml->hasOneStepNSProblemXML())
  {
    // OneStepNSProblem - Memory allocation/construction
    string type = simulationxml->getOneStepNSProblemXMLPtr()->getNSProblemType();
    if (type == LCP_TAG)  // LCP
    {
      allNSProblems["timeStepping"] = new LCP(simulationxml->getOneStepNSProblemXMLPtr(), this);
      isNSProblemAllocatedIn[ allNSProblems["timeStepping"] ] = true;
    }
    else if (type == FrictionContact2D_TAG) // Friction 2D
    {
      allNSProblems["timeStepping"] = new FrictionContact2D(simulationxml->getOneStepNSProblemXMLPtr(), this);
      isNSProblemAllocatedIn[ allNSProblems["timeStepping"] ] = true;
    }
    else if (type == FrictionContact3D_TAG) // Friction 3D
    {
      allNSProblems["timeStepping"] = new FrictionContact3D(simulationxml->getOneStepNSProblemXMLPtr(), this);
      isNSProblemAllocatedIn[ allNSProblems["timeStepping"] ] = true;
    }
    else RuntimeException::selfThrow("Simulation::xml constructor - wrong type of NSProblem: inexistant or not yet implemented");

    allNSProblems["timeStepping"]->setId("timeStepping");

    // Add QP and Relay cases when these classes will be fully implemented.
  }
}

// --- Destructor ---
TimeStepping::~TimeStepping()
{}

TimeStepping* TimeStepping::convert(Simulation *str)
{
  TimeStepping* ts = dynamic_cast<TimeStepping*>(str);
  return ts;
}

void TimeStepping::updateIndexSet(const unsigned int i)
{
  if (i > indexSets.size())
    RuntimeException::selfThrow("TimeStepping::updateIndexSet(i), indexSets[i] does not exist.");

  if (i == 0) // IndexSets[0] must not be updated by this function.
    RuntimeException::selfThrow("TimeStepping::updateIndexSet(i=0), indexSets[0] can not be updated.");

  // for all Unitary Relations in indexSet[i-1], compute y[i-1] and update the indexSet[i]
  UnitaryRelationIterator it, itForFind;

  double y;

  if (i == 1) // special case for Moreau time-stepping
  {
    double yp;
    double yDot;
    for (it = indexSets[0].begin(); it != indexSets[0].end(); ++it)
    {
      double h = timeDiscretisation->getH();
      // checks if current Unitary Relation (ie *it) is already in indexSets[1]
      // (if not itForFind will be equal to indexSets.end())
      itForFind = indexSets[1].find(*it);
      y = (*it)->getYRef(0);
      yDot = (*it)->getYRef(1);
      yp = y + 0.5 * h * yDot;

      // if yp <=0, then the unitary relation is added in indexSets[1] (if it was not already there)
      // else if yp > 0 and if the unitary relation was in the set, it is removed.
      if (yp <= 0 && itForFind == indexSets[1].end())
        indexSets[1].insert(*it);

      else if (yp > 0 && itForFind != indexSets[1].end())
        indexSets[1].erase(*it);
    }
  }
  else
  {
    for (it = indexSets[i - 1].begin(); it != indexSets[i - 1].end(); ++it)
    {
      // check if current Unitary Relation (ie *it) is in indexSets[i]
      // (if not itForFind will be equal to indexSets.end())
      itForFind = indexSets[i].find(*it);

      // Get y[i-1] double value
      y = (*it)->getYRef(i - 1);

      // if y[i-1] <=0, then the unitary relation is added in indexSets[i] (if it was not already there)
      // else if y[i-1] > 0 and if the unitary relation was in the set, it is removed.
      if (y <= 0 && itForFind == indexSets[i].end())
        indexSets[i].insert(*it);
      else if (y > 0 && itForFind != indexSets[i].end())
        indexSets[i].erase(*it);
    }
  }
}

void TimeStepping::addOneStepNSProblemPtr(OneStepNSProblem* osns)
{
  // A the time, a time stepping simulation can only have one non smooth problem.
  if (!allNSProblems.empty())
    RuntimeException::selfThrow("TimeStepping, addOneStepNSProblemPtr - A non smooth problem already exist. You can not have more than one.");

  string name = "timeStepping"; // osns->getId();
  osns->setId(name);
  allNSProblems[name] = osns;
  isNSProblemAllocatedIn[osns] = false;
}

void TimeStepping::initialize()
{
  if (model == NULL)
    RuntimeException::selfThrow("Simulation initialization - model = NULL.");

  // initialization of the OneStepIntegrators
  OSIIterator itOsi;
  for (itOsi = allOSI.begin(); itOsi != allOSI.end(); ++itOsi)
    (*itOsi)->initialize();

  if (allNSProblems.size() > 1)
    RuntimeException::selfThrow("TimeStepping::initialize, at the time, a time stepping simulation can not have more than one non smooth problem.");

  unsigned int levelMax = 0 ;

  if (model->getNonSmoothDynamicalSystemPtr()->getTopologyPtr()->getMaxRelativeDegree() != 0)
    levelMax =  model->getNonSmoothDynamicalSystemPtr()->getTopologyPtr()->getMaxRelativeDegree() - 1;

  unsigned int levelMin;

  // At the time, we consider that for all systems, levelMin is equal to the minimum value of the relative degree
  // except for Lagrangian (Mechanical) Systems, where levelMin = 1.
  // We get one Dynamical System of the OSI and check its type.
  string DStype = (*(((*allOSI.begin())->getDynamicalSystems()).begin()))->getType();
  if (DStype == LNLDS || DStype == LTIDS)
    levelMin = 1;

  else
    levelMin = model->getNonSmoothDynamicalSystemPtr()->getTopologyPtr()->getMinRelativeDegree();

  // initialization of  OneStepNonSmoothProblem
  OSNSIterator itOsns;
  for (itOsns = allNSProblems.begin(); itOsns != allNSProblems.end(); ++itOsns)
  {
    (itOsns->second)->setLevels(levelMin, levelMax);
    (itOsns->second)->initialize();
  }

  updateIndexSets();
}

void TimeStepping::run()
{
  // Current Step
  unsigned int k = timeDiscretisation->getK();
  // Number of time steps
  unsigned int nSteps = timeDiscretisation->getNSteps();
  while (k < nSteps)
  {
    // transfer of state i+1 into state i and time incrementation
    nextStep();
    // update current time step
    k = timeDiscretisation->getK();

    computeOneStep();

  }
}

// compute simulation between ti and ti+1, ti+1 being currentTime (?)
// Initial DS/interaction state is given by memory vectors (check that?)
// and final state is the one saved in DS/Interaction at the end of this function
void TimeStepping::computeOneStep()
{
  // solve ...
  computeFreeState();
  computeOneStepNSProblem("timeStepping");
  // update
  update();
}
