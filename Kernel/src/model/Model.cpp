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

#include "Model.h"

// includes to be deleted thanks to factories ...
#include "TimeStepping.h"
#include "EventDriven.h"

using namespace std;

// --- CONSTRUCTORS ---

// --- Default (private) constructor ---
Model::Model(): t(0.0), t0(0.0), T(0.0), strat(NULL), nsds(NULL),
  modelxml(NULL), title("none"), author("nobody"), description("none"),
  date("none"), xmlSchema("none")
{}
// -> xml
Model::Model(char *xmlFile):
  t(0.0), t0(0.0), T(-1.0), strat(NULL), nsds(NULL),
  modelxml(NULL), title("none"), author("nobody"), description("none"),
  date("none"), xmlSchema(XML_SCHEMA),
  isNsdsAllocatedIn(true), isSimulationAllocatedIn(false), isModelXmlAllocatedIn(true)
{
  if (xmlFile != NULL)
  {
    // Built DOMtree
    modelxml = new SiconosModelXML(xmlFile);

    // Load data (default value for T is -1)
    if (modelxml->hasT()) T = modelxml->getT();
    t0 = modelxml->getT0();
    if (modelxml->hasTCurrent()) t = modelxml->getTCurrent();
    else t = t0;
    title = modelxml->getTitle();
    author = modelxml->getAuthor();
    description = modelxml->getDescription();
    date = modelxml->getDate();
    if (modelxml->hasXMLSchema())
      xmlSchema = modelxml->getXMLSchema();

    // Memory allocation for nsds and simulation
    nsds = new NonSmoothDynamicalSystem(modelxml->getNonSmoothDynamicalSystemXML());
    if (modelxml->hasSimulation())
    {
      isSimulationAllocatedIn = true;
      if (modelxml->getSimulationXML()->getSimulationXMLType() == TIMESTEPPING_TAG)
        strat = new TimeStepping(modelxml->getSimulationXML(), this);
      else if (modelxml->getSimulationXML()->getSimulationXMLType() == EVENTDRIVEN_TAG)
        strat = new EventDriven(modelxml->getSimulationXML(), this);
      else RuntimeException::selfThrow("Model: xml constructor, wrong type of simulation" + (modelxml->getSimulationXML()->getSimulationXMLType()));
    }
  }
  else RuntimeException::selfThrow("Model: xml constructor, xmlfile = NULL");
}

// --- From a minimum set of data ---
Model::Model(double newT0, double newT, const string& newTitle, const string& newAuthor,
             const string& newDescription, const string& newDate, const string& newSchema):
  t(newT0), t0(newT0), T(-1), strat(NULL), nsds(NULL), modelxml(NULL), title(newTitle),
  author(newAuthor), description(newDescription), date(newDate), xmlSchema(newSchema),
  isNsdsAllocatedIn(false), isSimulationAllocatedIn(false), isModelXmlAllocatedIn(false)
{
  if (newT > t0) T = newT;
  else if (newT > 0 && newT <= t0)
    RuntimeException::selfThrow("Model::constructor from min data: Warning, final T lower than t0");
  // else no T in the model!
}

Model::~Model()
{
  if (isNsdsAllocatedIn) delete nsds;
  nsds = NULL;
  if (isSimulationAllocatedIn) delete strat;
  strat = NULL;
  if (isModelXmlAllocatedIn) delete modelxml;
  modelxml = NULL;
}

// --- SETTERS ---
void Model::setT0(const double& newT0)
{
  if (strat != NULL && strat->getTimeDiscretisationPtr() != NULL)
    RuntimeException::selfThrow("Model::setT0 - To set t0 value, use rather setT0 of the corresponding TimeDiscretisation.");
  else
    t0 = newT0;
}

void Model::setSimulationPtr(Simulation *newPtr)
{
  // Warning: this function may be used carefully because of the links between Model and TimeDiscretisation
  // The model of the simulation input MUST be the current model.
  if (isSimulationAllocatedIn) delete strat;
  strat = newPtr;
  isSimulationAllocatedIn = false;
}


void Model::setNonSmoothDynamicalSystemPtr(NonSmoothDynamicalSystem *newPtr)
{
  if (isNsdsAllocatedIn) delete nsds;
  nsds = newPtr;
  isNsdsAllocatedIn = false;
}

void Model::setSiconosModelXMLPtr(SiconosModelXML *newPtr)
{
  if (isModelXmlAllocatedIn) delete modelxml;
  modelxml = newPtr;
  isModelXmlAllocatedIn = false;
}

// --- OTHER FUNCTIONS ---


// --- XML RELATED FUNCTIONS ---
void Model::saveToXMLFile(char* xmlFile)
{
  cout << "## Model->checkXMLPlatform()" << endl;
  //   the first operation to do is to check the XML objects
  checkXMLPlatform();

  cout << "## Model->savePlatformToXML()" << endl;
  //   copy the values of the platform to the DOM tree
  savePlatformToXML();

  cout << "## Model->checkXMLDOMTree()" << endl;
  //   verifies that the DOM tree respects the XML schema
  checkXMLDOMTree();

  cout << "## Model->saveSiconosModelInXMLFile()" << endl;
  //   saves in a file the DOM tree
  modelxml->saveSiconosModelInXMLFile(xmlFile);
}

void Model::saveToDOMTree()
{
  checkXMLPlatform();
  savePlatformToXML();
  checkXMLDOMTree();
}

void Model::savePlatformToXML()
{
  // update of the data of the Model
  modelxml->setT0(t0);
  modelxml->setT(T);
  modelxml->setTCurrent(t);
  modelxml->setTitle(title);
  modelxml->setAuthor(author);
  modelxml->setDescription(description);
  modelxml->setDate(date);
  modelxml->setXMLSchema(xmlSchema);

  // save of the NonSmoothDynamicalSystem
  nsds->saveNSDSToXML();

  // save of the Simulation

  if (strat != NULL)
  {
    strat->getTimeDiscretisationPtr()->saveTimeDiscretisationToXML();

    if (strat->getType() == "TimeStepping")
      (static_cast<TimeStepping*>(strat))->saveSimulationToXML();
    else if (strat->getType() == "EventDriven")
      (static_cast<EventDriven*>(strat))->saveSimulationToXML();
    else RuntimeException::selfThrow("Model::savePlatformToXML - bad kind of Simulation");
  }
  else //RuntimeException::selfThrow("Model::saveToXML - object SimulationXML does not exist");
    cout << "Model::saveToXML - Warning : No Simulation is defined" << endl;
}

bool Model::checkXMLDOMTree()
{
  bool res = false;
  if (modelxml != NULL)
    res = modelxml->checkSiconosDOMTree();

  cout << " # checkModelCoherency()" << endl;
  checkModelCoherency();
  return res;
}

void Model::checkXMLPlatform()
{
  if (modelxml != NULL)
  {
    if (modelxml->getNonSmoothDynamicalSystemXML() != NULL)
    {
      // we must create/update the DynamicalSystemXMLs
      nsds->getNonSmoothDynamicalSystemXMLPtr()->updateNonSmoothDynamicalSystemXML(modelxml->getNonSmoothDynamicalSystemXML()->getRootNode(), nsds);
    }
    else if (nsds != NULL)
    {
      // creation of the NonSmoothDynamicalSystemXML and of all the DynamicalSystemXML and InteractionXML
      modelxml->loadModel(this);
      // \todo to be tested !!
    }
    else RuntimeException::selfThrow("Model::checkXMLPlatform - There's no NonSmoothDynamicalSystem in the Platform, the XML platform can't be built");

    if ((strat != NULL))
    {
      if (modelxml->getSimulationXML() == NULL)
      {
        //
        // no SimulationXML already exists, so no TimeDiscretisationXML, OneStepIntegratorXML and OneStepNSProblemXML are existing
        // because these objects are required when a Simulation is defined in the XML input file

        // we must update all the Model to do
        // the creation of the SimulationXML and of all the OneStepIntegratorXML and OneStepNSProblemXML
        //
        modelxml->loadModel(this);
        // \todo to be tested !!
      }
      else
      {
        strat->getSimulationXMLPtr()->saveSimulation2XML(modelxml->getSimulationXML()->getRootNode(), strat);
      }
    }
  }
  else
  {
    // in this case, we must create all the XML objects
    // SiconosModelXML, NonSmoothDynamicalSystemXML, SimulationXML, ...

    // to build all the XML objects, we must fold all the objects of the platform

    modelxml = new SiconosModelXML();
    modelxml->loadModel(this);
  }
}


void Model::checkModelCoherency()
{
  int number;
  unsigned int i, j, k, cpt;
  char num[32];
  string error;


  // at first, checking the XML
  // if matrix and vector are well defined by example
  // if DynamicalSystems have BoundaryConditions when the NonSmoothDynamicalSystem is BVP for example

  if (modelxml->checkSiconosDOMTreeCoherency() == true) cout << "Data of the XML DOM tree are coherent." << endl;
  else cout << "Warning : Data of the XML DOM tree are not coherent." << endl;

  // we can check here other properties that the platform must have
  // the number of each EqualityConstraint must be unique
  for (i = 0; i < nsds->getEqualityConstraints().size(); i++)
  {
    for (j = i + 1; j < nsds->getEqualityConstraints().size(); j++)
    {
      if (nsds->getEqualityConstraintPtr(i)->getNumber() == nsds->getEqualityConstraintPtr(j)->getNumber())
      {
        number = nsds->getEqualityConstraintPtr(j)->getNumber();
        sprintf(num, "%d", number);
        error = "/!\\ Error, 2 EqualityConstraints have the same number : ";
        error += num;
        error += " \n";
        RuntimeException::selfThrow(error);
      }
    }
  }

  // the number of each DSInputOutput must be unique

  // we get all the DSInputOutput numbers from all the DynamicalSystems
  // and we can check if there are redundant numbers

  cpt = 0; // cpt corresponds to the size of 'vec'
  vector<int> vec;
  for (i = 0; i < nsds->getDynamicalSystems().size(); i++)
  {
    for (j = 0; j < nsds->getDynamicalSystemPtr(i)->getDSInputOutputs().size(); j++)
    {
      if (cpt == 0)
      {
        vec.push_back(nsds->getDynamicalSystemPtr(i)->getDSInputOutput(0)->getNumber());
        cpt++;
      }
      else
      {
        for (k = 0; k < cpt; k++)
          if (vec[k] != nsds->getDynamicalSystemPtr(i)->getDSInputOutput(j)->getNumber())
          {
            vec.push_back(nsds->getDynamicalSystemPtr(i)->getDSInputOutput(j)->getNumber());
            cpt++;
            break;
          }
          else
          {
            number = vec[k];
            sprintf(num, "%d", number);
            error = "/!\\ Error, 2 DSInputOutputs have the same number : ";
            error += num;
            error += " \n";
            RuntimeException::selfThrow(error);
          }
      }
    }
  }
}

int Model::xmlSchemaValidated(string xmlFile, string xmlSchema)
{
  int res;
  cout << "int Model::xmlSchemaValidated(string xmlFile, string xmlSchema)" << endl;
  res = modelxml->validateXmlFile(xmlFile, xmlSchema);
  return res;
}


// --- OTHER FUNCTIONS ---

void Model::display() const
{
  cout << " ===== Model display =====" << endl;
  cout << "| current time = " << t << endl;
  cout << "| t0 (initial time) = " << t0 << endl;
  cout << "| T (final time) = " << T << endl;
  cout << "| &simulation = " << endl;
  if (strat != NULL) cout << strat << endl;
  else cout << "-> NULL" << endl;
  cout << "| &nsds = " << endl;
  if (nsds != NULL) cout << nsds << endl;
  else cout << "-> NULL" << endl;
  cout << "| &modelxml = " << endl;
  if (modelxml != NULL) cout << modelxml << endl;
  else cout << "-> NULL" << endl;
  cout << "| author = " << author << endl;
  cout << "| description = " << description << endl;
  cout << "| date = " << date << endl;
  cout << "| title = " << title << endl;
  cout << "| xmlSchema = " << xmlSchema << endl;
  cout << "============================" << endl;
}

//=======================================================
//
// function to create the platform from a C++ programm
//
//=======================================================

Simulation* Model::createSimulation(std::string type)
{
  if (isSimulationAllocatedIn) delete strat;
  isSimulationAllocatedIn = false;
  strat = NULL;
  if (type == "TimeStepping")
  {
    strat = new TimeStepping(NULL, this);
    isSimulationAllocatedIn = true ;
  }
  else if (type == "EventDriven")
  {
    strat = new EventDriven(NULL, this);
    isSimulationAllocatedIn = true ;
  }
  else RuntimeException::selfThrow("Model::create Simulation:wrong type of simulation:" + type);
  return strat;
}

Simulation* Model::createTimeStepping()
{
  if (isSimulationAllocatedIn) delete strat;
  strat = new TimeStepping(NULL, this);
  isSimulationAllocatedIn = true ;
  return strat;
}

Simulation* Model::createTimeEventDriven()
{
  if (isSimulationAllocatedIn) delete strat;
  strat = new EventDriven(NULL, this);
  isSimulationAllocatedIn = true ;
  return strat;
}

