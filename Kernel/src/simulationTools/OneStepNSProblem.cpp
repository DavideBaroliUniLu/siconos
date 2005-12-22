/* Siconos version 1.0, Copyright INRIA 2005.
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
#include "OneStepNSProblem.h"
// to be removed thanks to factories:
#include "LemkeSolver.h"
#include "LexicoLemkeSolver.h"
#include "QPSolver.h"
#include "NSQPSolver.h"
#include "NLGSSolver.h"
#include "CPGSolver.h"
#include "LatinSolver.h"

using namespace std;


// --- CONSTRUCTORS/DESTRUCTOR ---

// Default constructor
OneStepNSProblem::OneStepNSProblem():
  nspbType("none"), dim(0), solver(NULL), isSolverAllocatedIn(false),  strategy(NULL), onestepnspbxml(NULL)
{}

// xml constructor
OneStepNSProblem::OneStepNSProblem(OneStepNSProblemXML* osnspbxml, Strategy* newStrat):
  nspbType("none"), dim(0), solver(NULL), isSolverAllocatedIn(false), strategy(newStrat), onestepnspbxml(osnspbxml)
{
  if (onestepnspbxml != NULL)
  {
    // get dimension of the problem ...
    if (onestepnspbxml->hasDim()) dim = onestepnspbxml->getDimNSProblem();

    // === read solver related data ===
    if (onestepnspbxml->hasSolver())
    {
      // get the name of the solver algorithm used
      string solverAlgorithmType = onestepnspbxml->getSolverXMLPtr()->getSolverAlgorithmName();
      if (solverAlgorithmType == "Lemke")
        solver = new LemkeSolver(onestepnspbxml->getSolverXMLPtr());
      else if (solverAlgorithmType == "LexicoLemke")
        solver =  new LexicoLemkeSolver(onestepnspbxml->getSolverXMLPtr());
      else if (solverAlgorithmType == "QP")
        solver =  new QPSolver(onestepnspbxml->getSolverXMLPtr());
      else if (solverAlgorithmType == "NSQP")
        solver =  new NSQPSolver(onestepnspbxml->getSolverXMLPtr());
      else if (solverAlgorithmType == "NLGS")
        solver =  new NLGSSolver(onestepnspbxml->getSolverXMLPtr());
      else if (solverAlgorithmType == "CPG")
        solver =  new CPGSolver(onestepnspbxml->getSolverXMLPtr());
      else if (solverAlgorithmType == "Latin")
        solver =  new LatinSolver(onestepnspbxml->getSolverXMLPtr());
      else
        RuntimeException::selfThrow("OneStepNSProblem::xml constructor, unknown solver algorithm type: " + solverAlgorithmType);
      isSolverAllocatedIn = true;
    }
    else
      RuntimeException::selfThrow("OneStepNSProblem::xml constructor, no solver found!");
  }
  else RuntimeException::selfThrow("OneStepNSProblem::xml constructor, xml file=NULL");

  // read list of interactions and equality constraints concerned
  if (strategy != NULL)
  {
    interactionVector = strategy->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getInteractions();
    ecVector = strategy->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getEqualityConstraints();
    // Default value for dim. Warning: default size is the size of the first interaction in the vector
    if (interactionVector.size() != 0) dim = interactionVector[0]->getNInteraction();
  }
  else cout << "OneStepNSPb xml-constructor - Warning: no strategy linked to OneStepPb" << endl;
}

// Constructor with given strategy and solving method parameters (optional)
OneStepNSProblem::OneStepNSProblem(Strategy * newStrat, const string& newSolvingFormalisation, const string& solverAlgorithmType, const int& MaxIter,
                                   const double & Tolerance, const string & NormType, const double & SearchDirection):
  nspbType("none"), dim(0), solver(NULL), isSolverAllocatedIn(false), strategy(newStrat), onestepnspbxml(NULL)
{
  if (strategy != NULL)
  {
    interactionVector = strategy->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getInteractions();
    ecVector = strategy->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getEqualityConstraints();
    // Default value for n  \warning: default size is the size of the first interaction in the vector if it exists
    // else, n=0
    if (interactionVector.size() != 0) dim = interactionVector[0]->getNInteraction();
    if (newSolvingFormalisation != "none")
    {
      if (solverAlgorithmType == " Lemke")
        solver = new LemkeSolver(newSolvingFormalisation, MaxIter);
      else if (solverAlgorithmType == "LexicoLemke")
        solver =  new LexicoLemkeSolver(newSolvingFormalisation, MaxIter);
      else if (solverAlgorithmType == "QP")
        solver =  new QPSolver(newSolvingFormalisation, Tolerance);
      else if (solverAlgorithmType == "NSQP")
        solver =  new NSQPSolver(newSolvingFormalisation, Tolerance);
      else if (solverAlgorithmType == "NLGS")
        solver =  new NLGSSolver(newSolvingFormalisation, MaxIter, Tolerance);
      else if (solverAlgorithmType == "CPG")
        solver =  new CPGSolver(newSolvingFormalisation, MaxIter, Tolerance);
      else if (solverAlgorithmType == "Latin")
        solver =  new LatinSolver(newSolvingFormalisation, MaxIter, Tolerance, SearchDirection);
      else
        RuntimeException::selfThrow("OneStepNSProblem::constructor, unknown solver algorithm type: " + solverAlgorithmType);
      isSolverAllocatedIn = true;
    }
    else RuntimeException::selfThrow("OneStepNSProblem:: constructor from data, wrong solving method type");
  }
  else
    RuntimeException::selfThrow("OneStepNSProblem:: constructor from strategy, given strategy == NULL");
}

// Constructor with given strategy and a pointer on Solver
OneStepNSProblem::OneStepNSProblem(Strategy * newStrat, Solver* newSolver):
  nspbType("none"), dim(0), solver(newSolver), isSolverAllocatedIn(false), strategy(newStrat), onestepnspbxml(NULL)
{
  if (strategy != NULL)
  {
    interactionVector = strategy->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getInteractions();
    ecVector = strategy->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getEqualityConstraints();
    // Default value for n  \warning: default size is the size of the first interaction in the vector if it exists
    // else, n=0
    if (interactionVector.size() != 0) dim = interactionVector[0]->getNInteraction();
  }
  else
    RuntimeException::selfThrow("OneStepNSProblem:: constructor from strategy, given strategy == NULL");
}

OneStepNSProblem::~OneStepNSProblem()
{
  map< Interaction* , SiconosMatrix*>::iterator it;
  for (it = diagonalBlocksMap.begin(); it != diagonalBlocksMap.end(); it++)
  {
    SiconosMatrix * tmp = (*it).second;
    if (tmp != NULL)  delete tmp ;
    tmp = NULL;
  }

  map< Interaction* , map<Interaction *, SiconosMatrix*> >::iterator it2;
  map<Interaction *, SiconosMatrix*>::iterator it3;
  for (it2 = extraDiagonalBlocksMap.begin(); it2 != extraDiagonalBlocksMap.end(); it2++)
  {

    for (it3 = ((*it2).second).begin(); it3 != ((*it2).second).end(); it3++)
    {
      SiconosMatrix * tmp = (*it3).second;
      if (tmp != NULL)  delete tmp ;
      tmp = NULL;
    }
  }
  if (isSolverAllocatedIn) delete solver;
  solver = NULL;
}

Interaction* OneStepNSProblem::getInteractionPtr(const unsigned int& nb)
{
  if (nb >= interactionVector.size())
    RuntimeException::selfThrow("OneStepNSProblem::getInteractionPtr(const int& nb) - number greater than size of interaction vector");
  return interactionVector[nb];
}

void OneStepNSProblem::addInteraction(Interaction *interaction)
{
  interactionVector.push_back(interaction);
}

void OneStepNSProblem::initialize()
{
  // update topology if necessary (ie take into account modifications in the NonSmoothDynamicalSystem)
  Topology * topology = strategy->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getTopologyPtr();
  if (!(topology->isUpToDate()))
    topology->updateTopology();

  updateOutput();
  updateInput();
}

void OneStepNSProblem::computeEffectiveOutput()
{

  // 3 steps to update the effective output, this for each interaction:
  //  - compute prediction for y ( -> yp), this for the r-1 first derivatives, r being
  //    the relative degree
  //  - compute indexMax using this prediction
  //  - compute effectiveIndexes, a list of the indexes for which constraints will be applied
  //
  //

  // get topology of the NonSmooth Dynamical System
  Topology * topology = strategy->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getTopologyPtr();
  // get time step
  double pasH = strategy->getTimeDiscretisationPtr()->getH();

  unsigned int i ; // index of derivation
  unsigned int j ; // relation number
  unsigned int sizeOutput; // effective size of vector y for a specific interaction
  unsigned int globalSizeOutput = 0; // effective size of global vector y (ie including all interactions) = sum sizeOutput over all interactions
  unsigned int k;
  // === loop over the interactions ===
  vector<Interaction*>::iterator it;
  for (it = interactionVector.begin(); it != interactionVector.end(); it++)
  {
    // get the output vector (values for previous time step)
    vector<SimpleVector *> yOld = (*it)->getYOld();

    // get relative degrees vector of this interaction (one relative degree for each relation!)
    vector<unsigned int> relativeDegree = topology->getRelativeDegrees(*it);
    unsigned int numberOfRelations = relativeDegree.size(); // this size corresponds to the interaction size, ie the number of relations

    // --- prediction vector ---

    // we compute yp[i], i =0..r-2. r is equal to the maximum value of all the relative degrees.
    // For the moment we consider that the interaction is homegeneous, ie all the relations have the same degree.
    // if r<2, no prediction, all relations are effective.
    unsigned int sizeYp;

    if (relativeDegree[0] != 0)  sizeYp = relativeDegree[0] - 1;
    else sizeYp = 0;

    if (sizeYp > 0)
    {
      // --- prediction vector ---

      vector<SimpleVector *> yp;
      yp.resize(sizeYp, NULL);
      // allocate and initialize yp with yOld.
      for (i = 0; i < sizeYp ; i++)
        yp[i] = new SimpleVector(*yOld[i]);
      // \todo the way prediction is calculated should be defined by user elsewhere
      *(yp[0]) = *(yOld[0]) +  0.5 * pasH * *(yOld[1]) ;

      // --- indexMax ---

      // loop from 0 to relative degree to find the first yp>0
      vector<unsigned int> indexMax;
      indexMax.resize(numberOfRelations, 0);
      for (j = 0; j < numberOfRelations; j++)
      {
        for (i = 0; i < sizeYp; i++)
        {
          if ((*(yp[i]))(j) <= 0)
            indexMax[j]++;
          else
            break;
        }
      }
      topology->setIndexMax(*it, indexMax);

      for (i = 0; i < sizeYp ; i++)
        delete yp[i];

      // --- effective indexes ---

      // compute sizeOutput for the current interaction
      sizeOutput = topology->computeEffectiveSizeOutput(*it);

      vector<unsigned int> effectiveIndexes, blockIndexes, indexMin;
      indexMin = topology->getIndexMin(*it);

      effectiveIndexes.resize(sizeOutput, 0);
      blockIndexes.resize(sizeOutput, 0);

      k = 0;

      for (j = 0; j < numberOfRelations; j++)
      {
        for (i = indexMin[j]; i < indexMax[j] + 1; i++)
        {
          effectiveIndexes[k] = i + j * (relativeDegree[j]);
          blockIndexes[k] = i - indexMin[j] + j * (relativeDegree[j] - indexMin[j]);
          k++;
        }
      }
      topology->setEffectiveIndexes(*it, effectiveIndexes);
      blockIndexesMap[*it] = blockIndexes;

    }
    else
    {
      // compute sizeOutput for the current interaction
      sizeOutput = topology->computeEffectiveSizeOutput(*it);
    }
    globalSizeOutput   += sizeOutput;


  }// == end of interactions loop ==

  topology->setEffectiveSizeOutput(globalSizeOutput);

  // compute effective positions map
  topology->computeInteractionEffectivePositionMap();
}
void OneStepNSProblem::nextStep()
{
  vector<Interaction*>::iterator it;
  for (it = interactionVector.begin(); it != interactionVector.end(); it++)
    (*it)->swapInMemory();
}

void OneStepNSProblem::updateInput()
{
  vector<Interaction*>::iterator it;
  double currentTime = strategy->getModelPtr()->getCurrentT();

  for (it = interactionVector.begin(); it != interactionVector.end(); it++)
    (*it)->getRelationPtr() -> computeInput(currentTime);
}

void OneStepNSProblem::updateOutput()
{
  vector<Interaction*>::iterator it;
  double currentTime = strategy->getModelPtr()->getCurrentT();
  for (it = interactionVector.begin(); it != interactionVector.end(); it++)
    (*it)->getRelationPtr()->computeOutput(currentTime);
}

void OneStepNSProblem::compute(const double& time)
{
  RuntimeException::selfThrow("OneStepNSProblem::compute - not yet implemented for problem type =" + getType());
}

void OneStepNSProblem::saveNSProblemToXML()
{
  IN("OneStepNSProblem::saveNSProblemToXML\n");
  if (onestepnspbxml != NULL)
  {
    onestepnspbxml->setDimNSProblem(dim);
    vector<int> v;
    for (unsigned int i = 0; i < interactionVector.size(); i++)
      v.push_back(interactionVector[i]->getNumber());
    //onestepnspbxml->setInteractionConcerned( v, allInteractionConcerned() );

    /*
     * save of the solving method to XML
     */

    //    onestepnspbxml->setSolver(solvingFormalisation, methodName, normType, tolerance, maxIter, searchDirection );
  }
  else RuntimeException::selfThrow("OneStepNSProblem::saveNSProblemToXML - OneStepNSProblemXML object not exists");
  OUT("OneStepNSProblem::saveNSProblemToXML\n");
}

void OneStepNSProblem::setSolverPtr(Solver * newSolv)
{
  if (isSolverAllocatedIn) delete solver;
  solver = newSolv;
  isSolverAllocatedIn = false;
}

bool OneStepNSProblem::isOneStepNsProblemComplete()
{
  bool isComplete = true;

  if (nspbType != "LCP" || nspbType != "FrictionContact2D" || nspbType != "FrictionContact3D" || nspbType != "QP" || nspbType != "Relay")
  {
    cout << "OneStepNSProblem is not complete: unknown problem type " << nspbType << endl;
    isComplete = false;
  }

  if (dim == 0)
  {
    cout << "OneStepNSProblem warning: problem size == 0" << endl;
    isComplete = false;
  }

  if (!(interactionVector.size() > 0))
  {
    cout << "OneStepNSProblem warning: interaction vector is empty" << endl;
    isComplete = false;
  }
  else
  {
    vector< Interaction* >::iterator it;
    for (it = interactionVector.begin(); it != interactionVector.end(); it++)
      if (*it == NULL)
        cout << "OneStepNSProblem warning: an interaction points to NULL" << endl;
  }

  if (!(ecVector.size() > 0))
  {
    cout << "OneStepNSProblem warning: equality constraints vector is empty" << endl;
    isComplete = false;
  }
  else
  {
    vector< EqualityConstraint* >::iterator it;
    for (it = ecVector.begin(); it != ecVector.end(); it++)
      if (*it == NULL)
        cout << "OneStepNSProblem warning: an equalityConstraint of the problem points to NULL" << endl;
  }

  if (strategy == NULL)
  {
    cout << "OneStepNSProblem warning: no strategy linked with the problem" << endl;
    isComplete = false;
  }

  if (solver == NULL)
  {
    cout << "OneStepNSProblem warning: no solver defined in the problem" << endl;
    isComplete = false;
  }

  return isComplete;
}

