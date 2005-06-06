#ifndef STRATEGY_H
#define STRATEGY_H

#include "OneStepIntegrator.h"
#include "OneStepNSProblem.h"
#include "TimeDiscretisation.h"
#include "SiconosMatrix.h"
#include "NewSiconosVector.h"
#include "StrategyXML.h"
#include "SiconosConst.h"
#include "Model.h"

#include "check.h"
#include <iostream>
#include <vector>


class Model;
class OneStepIntegrator;
class OneStepNSProblem;
class TimeDiscretisation;
class StrategyXML;

/** \class Strategy
 *  \brief It regroups all the elements to lead the resolution of the simulation
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 1.0
 *  \date (Crestion) Apr 26, 2004
 *
 *
 */
class Strategy
{
public:

  /** \fn Strategy()
   *  \brief default constructor
   */
  Strategy();

  /** \fn Strategy(TimeDiscretisation*, vector<OneStepIntegrator*>, OneStepNSProblem*, Model*= NULL)
   *  \brief constructor from a given set of data (1)
   *  \param pointer on the time discretisation
   *  \param the vector of osi
   *  \param pointer on a OneStepNSProblem
   *  \param the model which owns this strategy  (optional parameter)
   */
  Strategy(TimeDiscretisation*, std::vector<OneStepIntegrator*>, OneStepNSProblem *, Model* = NULL);

  /** \fn Strategy(TimeDiscretisation*, vector<OneStepIntegrator*>, Model*= NULL)
   *  \brief constructor from a given set of data (2)
   *  \param pointer on the time discretisation
   *  \param the vector of osi
   *  \param the model which owns this strategy  (optional parameter)
   *   with previous constructor)
   */
  Strategy(TimeDiscretisation*, std::vector<OneStepIntegrator*>, Model* = NULL);

  /** \fn Strategy(TimeDiscretisation*, OneStepNSProblem *, Model*= NULL )
   *  \brief constructor from a given set of data (3)
   *  \param pointer on the time discretisation
   *  \param pointer on a OneStepNSProblem
   *  \param the model which owns this strategy  (optional parameter)
   */
  Strategy(TimeDiscretisation*, OneStepNSProblem *, Model* = NULL);

  /** \fn Strategy(TimeDiscretisation*, Model*)
   *  \brief constructor from a given set of data (4)
   *  \param pointer on the time discretisation
   *  \param the model which owns this strategy  (optional parameter)
   */
  Strategy(TimeDiscretisation*, Model* = NULL);

  /** \fn Strategy(vector<OneStepIntegrator*>, OneStepNSProblem*, Model*= NULL)
   *  \brief constructor from a given set of data (5)
   *  \param the vector of osi
   *  \param pointer on a OneStepNSProblem
   *  \param the model which owns this strategy  (optional parameter)
   */
  Strategy(std::vector<OneStepIntegrator*>, OneStepNSProblem *, Model* = NULL);

  /** \fn Strategy(vector<OneStepIntegrator*> , Model*= NULL)
   *  \brief constructor from a given set of data (6)
   *  \param the vector of osi
   *  \param the model which owns this strategy  (optional parameter)
   */
  Strategy(std::vector<OneStepIntegrator*>, Model* = NULL);

  /** \fn Strategy(vector<OneStepIntegrator*> , Model*= NULL)
   *  \brief constructor from a given set of data (7)
   *  \param pointer on a OneStepNSProblem
   *  \param the model which owns this strategy  (optional parameter)
   */
  Strategy(OneStepNSProblem *, Model* = NULL);

  /** \fn Strategy(StrategyXML*, Model*)
   *  \brief constructor with XML object of the Strategy
   *  \param StrategyXML* : the XML object corresponding
   *  \param the model which owns this strategy  (optional parameter)
   */
  Strategy(StrategyXML*, Model* = NULL);

  /** \fn ~Strategy()
   *  \brief destructor
   */
  virtual ~Strategy();

  bool isStrategyComplete() const;

  // GETTERS/SETTERS

  /** \fn inline string getType() const
   *  \brief get the type of the Strategy
   *  \return string : the type of the Strategy
   */
  inline const std::string getType() const
  {
    return strategyType;
  }

  /** \fn inline string setType(const string&)
   *  \brief set the type of the Strategy
   */
  inline void setType(const std::string& newType)
  {
    strategyType = newType;
  }

  /** \fn TimeDiscretisation* getTimeDiscretisationPtr()
   *  \brief get the TimeDiscretisation of the Strategy
   *  \return the TimeDiscretisation
   */
  inline TimeDiscretisation* getTimeDiscretisationPtr() const
  {
    return timeDiscretisation;
  };

  /** \fn void setTimeDiscretisationPtr(TimeDiscretisation*)
   *  \brief set timeDiscretisation
   *  \param the TimeDiscretisation to set
   */
  void setTimeDiscretisationPtr(TimeDiscretisation* td);

  /** \fn vector<OneStepIntegrator*> getOneStepIntegrators(void)
   *  \brief get all the Integrators of the Strategy
   *  \return a vector of OneStepIntegrator*
   *  \exception RuntimeException
   */
  inline std::vector<OneStepIntegrator*> getOneStepIntegrators() const
  {
    return integratorVector;
  };

  /** \fn void setOneStepIntegrators(vector<OneStepIntegrator*>)
   *  \brief set the vector of Integrators
   *  \param a vector of OneStepIntegrator to set
   */
  void setOneStepIntegrators(const std::vector<OneStepIntegrator*> vOSI);

  /** \fn OneStepIntegrator* getOneStepIntegrator(const int&)
   *  \brief get one Integrator of the Strategy
   *  \return one OneStepIntegrator* if it exists else return exception
   */
  OneStepIntegrator* getOneStepIntegrator(const int&) const;

  /** \fn inline int getOneStepIntegratorVectorSize()
   *  \brief get the size of the vector of Integrators
   *  \return int : the size of integratorVector
   */
  inline const int getOneStepIntegratorVectorSize() const
  {
    return integratorVector.size();
  }

  /** \fn OneStepNSProblem* getOneStepNSProblemPtr(void)
   *  \brief get the OneStepNSProblem of the Strategy
   *  \return the OneStepNSProblem of the Strategy if it exists
   */
  inline OneStepNSProblem* getOneStepNSProblemPtr() const
  {
    return nsProblem;
  };

  /** \fn void setOneStepNSProblemPtr(OneStepNSProblem*)
   *  \brief set the OneStepNSProblem of the Strategy
   *  \param the OneStepNSProblem to set
   */
  void setOneStepNSProblemPtr(OneStepNSProblem* nspb);

  /** \fn inline StrategyXML* getStrategyXMLPtr() const
   *  \brief get the StrategyXML* of the Strategy
   *  \return a pointer on the StrategyXML of the Strategy
   */
  inline StrategyXML* getStrategyXMLPtr() const
  {
    return strategyxml;
  }

  /** \fn inline setStrategyXMLPtr(StrategyXML* strxml)
   *  \brief set the StrategyXML of the Strategy
   *  \param StrategyXML* : the pointer to set the StrategyXML
   */
  inline void setStrategyXMLPtr(StrategyXML* strxml)
  {
    strategyxml = strxml;
  }

  /** \fn Model* getModelPtr()
   *  \brief get the Model which contains the Strategy
   *  \return Model* : the Model which the Strategy
   */
  inline Model* getModelPtr() const
  {
    return model;
  }

  /** \fn void setModelPtr(Model* m)
   *  \brief set the Model which contains the Strategy
   *  \param Model* : the Model to set
   */
  inline void setModelPtr(Model* m)
  {
    model = m;
  }

  /* \fn OneStepIntegrator* getIntegratorOfDSptr(const int& numberDS);
   * \brief searchs the integrator of the DS number "numberDS"
   *
   */
  OneStepIntegrator* getIntegratorOfDSPtr(const int&  numberDS);

  // --- OTHER FUNCTIONS ---

  /** \fn void addOneStepIntegrator(OneStepIntegrator*)
   *  \brief add an Integrator into the vector of Integrators
   *  \param the OneStepIntegrator to add
   */
  void addOneStepIntegrator(OneStepIntegrator *osi)
  {
    integratorVector.push_back(osi);
  };

  /** \fn void computeFreeState()
   *  \brief integrates all the DynamicalSystem taking not into account nslaw, reactions ...
   */
  virtual void computeFreeState();

  /** \fn void nextStep()
   *  \brief increments all the Integrators to next step of the simulation
   */
  virtual void nextStep();

  /** \fn void formaliseOneStepNSProblem()
   *  \brief formalizes the non smooth problem
   */
  virtual void formaliseOneStepNSProblem();

  /** \fn void computeOneStepNSProblem()
   *  \brief computes the one step NS problem
   */
  virtual void computeOneStepNSProblem();

  /** \fn voir updateState()
   *  \brief updates the state of each DynamicalSystem
   */
  virtual void updateState();

  /** \fn void initialize()
   *  \brief executes the complete initialisation of Strategy (OneStepIntegrators, OneStepNSProblem, TImediscretisation) with the XML Object
   */
  virtual void initialize();

  /** \fn void newtonSolve(const double& criterion , const int& maxStep)
   *  \brief newton algorithm
   *  \param double criterion: convergence criterion, int maxStep: maximum number of Newton steps
   */
  void newtonSolve(const double& criterion , const int& maxStep);

  /** \fn newtonUpdateState()
   *  \brief update the state of the dynamical system at the end of Newton step
   */
  void newtonUpdateState();

  /** \fn newtonCheckConvergence(const double& criterion);
   *  \brief check the convergence of Newton algorithm
   */
  bool Strategy::newtonCheckConvergence(const double& criterion);

  // --- XML RELATED FUNCTIONS ---

  /** \fn void saveStrategyToXML()
   *  \brief copys the data of the Strategy to the XML tree
   *  \exception RuntimeException
   */
  virtual void saveStrategyToXML();

  // --- FUNCTIONS TO CREATE OR ADD VARIOUS OBJECTS ---

  /** \fn void createTimeDiscretisationPtr(const double& h, const int& N, SimpleVector * tk,
      const double& hMin, const double& hMax, const bool& constant
      *  \brief allows to create the TimeDiscretisation of the Strategy
      *  \param double : the h value
      *  \param int : the N value
      *  \param SiconosVector* : the tk vector
      *  \param double : the hMin value
      *  \param double : the hMax value
      *  \param bool : the boolean which determines if the TimeDiscretisation is constant
      *  \return TimeDiscretisation* : the TimeDiscretisation created
      *  \exception RuntimeException
      */
  TimeDiscretisation* createTimeDiscretisationPtr(const double& h, const int& N, SimpleVector * tk,
      const double& hMin, const double& hMax, const bool& constant);

  //===========================================
  /** \fn OneStepNSProblem* createLCP()
   *  \brief allows to create a LCP
   *  \return OneStepNSProblem* : the OneStepNSProblem created
   */
  OneStepNSProblem* createLCP();

  /** \fn OneStepNSProblem* createQP()
   *  \brief allows to create a QP
   *  \return OneStepNSProblem* : the OneStepNSProblem created
   */
  OneStepNSProblem* createQP();

  /** \fn OneStepNSProblem* createRelay()
   *  \brief allows to create a Relay NSProblem
   *  \return OneStepNSProblem* : the OneStepNSProblem created
   */
  OneStepNSProblem* createRelay();

  /** \fn OneStepIntegrator* addAdams(TimeDiscretisation* td, DynamicalSystem* ds)
   *  \brief allows to add an Adams integrator to the Strategy
   *  \param TimeDiscretisation* : the TimeDiscretisation of the OneStepIntegrator
   *  \param DynamicalSystem* : the DynamicalSystem that OneStepIntegrator must integrate
   *  \return OneStepIntegrator* : the OneStepIntegrator created
   */
  OneStepIntegrator* addAdams(TimeDiscretisation* td, DynamicalSystem* ds);

  /** \fn OneStepIntegrator* addMoreau(TimeDiscretisation* td, DynamicalSystem* ds,
                                       const double& theta)
   *  \brief allows to add an Moreau integrator to the Strategy
   *  \param TimeDiscretisation* : the TimeDiscretisation of the OneStepIntegrator
   *  \param DynamicalSystem* : the DynamicalSystem that OneStepIntegrator must integrate
   *  \param double : the theta value
   *  \return OneStepIntegrator* : the OneStepIntegrator created
   */
  OneStepIntegrator* addMoreau(TimeDiscretisation* td, DynamicalSystem* ds, const double& theta);

  /** \fn OneStepIntegrator* addLsodar(TimeDiscretisation* td, DynamicalSystem* ds)
   *  \brief allows to add an Lsodar integrator to the Strategy
   *  \param TimeDiscretisation* : the TimeDiscretisation of the OneStepIntegrator
   *  \param DynamicalSystem* : the DynamicalSystem that OneStepIntegrator must integrate
   *  \return OneStepIntegrator* : the OneStepIntegrator created
   */
  OneStepIntegrator* addLsodar(TimeDiscretisation* td, DynamicalSystem* ds);

  /** \fn bool hasDynamicalSystemIntegrator( DynamicalSystem* ds) const
   *  \brief checks if a DynamicalSystem owns already an OneStepIntegrator
   *  \return bool : false if the DynamicalSystem has no OneStepIntegrator, else true
   */
  bool hasDynamicalSystemIntegrator(DynamicalSystem* ds) const ;

protected:

  /** the name of the type of the Strategy */
  std::string strategyType;

  /** contains the data of the time discretisation */
  TimeDiscretisation *timeDiscretisation;

  /** contains the integrators to integre the DS */
  std::vector<OneStepIntegrator*> integratorVector;

  /** contains the type of resolution */
  OneStepNSProblem *nsProblem;

  /** the XML object linked to the Strategy to read XML data */
  StrategyXML *strategyxml;

  /** A link to the Model which contains the Strategy */
  Model *model;

  /** Flags to check wheter pointers were allocated in class constructors or not */
  bool isTimeDiscrAllocatedIn;
  bool isNsPbAllocatedIn;
  std::vector<bool> isIntegratorVectorAllocatedIn;
};

#endif // STRATEGY_H
