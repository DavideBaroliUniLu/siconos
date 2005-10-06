#ifndef LAGRANGIANNLDS_H
#define LAGRANGIANNLDS_H
#include "DynamicalSystem.h"
#include "LagrangianDSXML.h"

class LagrangianDSXML;

/** \class LagrangianDS
 *  \brief main class of Lagrangian dynamic systems
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 1.0
 *  \date (Creation) Apr 29, 2004
 *
 *
 * The class LagrangianDS  defines  and computes a generic ndof-dimensional
 * Lagrangian Non Linear Dynamical System of the form :
 * \f[
 * M(q) \ddot q + NNL(\dot q, q) = F_{Int}(\dot q , q , t)+F_{Ext}(t) + p,
 * \f]
 * where
 *    - \f$q \in R^{ndof} \f$ is the set of the generalized coordinates,
 *    - \f$ \dot q  \in R^{ndof} \f$  the velocity, i. e. the time derivative of the  generalized coordinates.
 *    - \f$ \ddot q  \in R^{ndof} \f$  the acceleration, i. e. the second time derivative of the  generalized coordinates.
 *    - \f$ p  \in R^{ndof} \f$  the forces due to the Non Smooth Interaction. In the particular case of Non Smooth evolution, the variable p contains the impulse and not the force.
 *    -  \f$ M(q)  \in  R^{ndof \times ndof}  \f$ is the inertia term saved in the SiconosMatrix mass.
 *    -  \f$ NNL(\dot q, q)  \in R^{ndof}\f$ is the non linear inertia term saved in the SiconosVector NNL.
 *    -  \f$ F_{Int}(\dot q , q , t)  \in R^{ndof} \f$ are the internal forces saved in the SiconosVector fInt.
 *    -  \f$ F_{Ext}(t)  \in R^{ndof}  \f$ are the external forces saved in the SiconosVector fExt.
 *
 *
 * One word on the initial condition.
 *
 * One word on the bilateral constraint
 *
 * The state of the master class DynamicalSystem is defined by \f$ x = \left[\begin{array}{c}q \\ \dot q\end{array}\right]\f$ and then \f$ n= 2 ndof \f$ and the VectorField
 * is specified as :
 * \f[
 * f(x,t) = \left[\begin{array}{cc}
 *  0_{ndof \times ndof} & I_{ndof \times ndof} \\
 * M^{-1}(q)\left[   F_{Int}(\dot q , q , t)+F_{Ext}( q , t) -  Q(\dot q, q) \right]\\
 * \end{array}\right]
 * \f]
 *  and the input due to the non smooth law by
 * \f[
 * r = \left[\begin{array}{c}0 \\ p \end{array}\right]
 * \f]
 *
 *
 * \todo Automatically, specify the function of DynamicalSystem such as
 *          VectorField.
 * \todo The specification of the master class DynamicalSystem must be made :
 *     -  Automatically,  in order to specify the function of DynamicalSystem such as
 *          VectorField.
 *     -  On the control of the OneStep Integrator and/or the user for the variable x, xDot and r and the memories.
 * we should find a right way ti do that without systematically copying the value of the state.
 * \todo Default plugin mechanism (for example for fExt FInt, M, ....) has to be enhanced following these steps :
 *   -# A Plugin is defined (in the XML or via the API) : Plug this Plugin. If a value is also given  (if it is possible),
 * this value is set as a current value in this object.
 *   -# A contant vector is given : Create a function which returns the constant value and plug it.
 *   -# Nothing is given : Plug the Basic Plugin and print a help message.
 *
 */
class LagrangianDS : public DynamicalSystem
{
public:

  // === CONSTRUCTORS - DESTRUCTOR ===

  /** \fn LagrangianDS(DynamicalSystemXML * nsdsXML)
   *  \brief constructor from an xml file
   *  \param DynamicalSystemXML * : the XML object for this DynamicalSystem
   *  \param NonSmoothDynamicalSystem* (optional): the NSDS that owns this ds
   *  \exception RuntimeException
   */
  LagrangianDS(DynamicalSystemXML * dsXML, NonSmoothDynamicalSystem* = NULL);

  /** \fn LagrangianDS(int number, int ndof,
      const SimpleVector& q0, const SimpleVector& velocity0,
      const SiconosMatrix& Mass)
      *  \brief constructor from a minimum set of data
      *  \param int : the number for this DynamicalSystem
      *  \param int : the dimension of this DynamicalSystem
      *  \param SimpleVector : initial coordinates of this DynamicalSystem
      *  \param SimpleVector : initial velocity of this DynamicalSystem
      *  \param SiconosMatrix : mass matrix
      *  \exception RuntimeException
      */
  LagrangianDS(const int&, const unsigned int& ,
               const SimpleVector& , const SimpleVector& ,
               const SiconosMatrix&);

  /** \fn LagrangianDS(int number, int ndof,
      const SimpleVector& q0, const SimpleVector& velocity0,
      const std::string& massPluginName)
      *  \brief constructor from a minimum set of data
      *  \param int : the number for this DynamicalSystem
      *  \param int : the dimension of this DynamicalSystem
      *  \param SimpleVector : initial coordinates of this DynamicalSystem
      *  \param SimpleVector : initial velocity of this DynamicalSystem
      *  \param string: plugin path to compute mass matrix
      *  \exception RuntimeException
      */
  LagrangianDS(const int&, const unsigned int& ,
               const SimpleVector& , const SimpleVector& ,
               const std::string&  = "BasicPlugin:computeMass");

  /** \fn LagrangianDS(const DynamicalSystem &)
   *  \brief copy constructor
   *  \param a Dynamical system to copy
   */
  LagrangianDS(const DynamicalSystem &);

  /** \fn ~LagrangianDS()
   *  \brief destructor */
  virtual ~LagrangianDS();

  // === GETTERS AND SETTERS ===

  /** \fn const int getNdof() const
   *  \brief allows to get the value of ndof
   *  \return the value of ndof
   */
  inline const unsigned int getNdof() const
  {
    return ndof;
  };

  /** \fn void setNdof(const unsigned int&)
   *  \brief allows to set ndof
   *  \param unsigned int ndof : the value to set ndof
   */
  inline void setNdof(const unsigned int& newNdof)
  {
    ndof = newNdof;
  };

  // -- q --

  /** \fn  const SimpleVector getQ() const
   *  \brief get the value of q
   *  \return SimpleVector
   */
  inline const SimpleVector getQ() const
  {
    return *q;
  }

  /** \fn SimpleVector* getQPtr() const
   *  \brief get q
   *  \return pointer on a SimpleVector
   */
  inline SimpleVector* getQPtr() const
  {
    return q;
  }

  /** \fn void setQ (const SimpleVector& newValue)
   *  \brief set the value of q to newValue
   *  \param SimpleVector newValue
   */
  void setQ(const SimpleVector&);

  /** \fn void setQPtr(SimpleVector* newPtr)
   *  \brief set Q to pointer newPtr
   *  \param SimpleVector * newPtr
   */
  void setQPtr(SimpleVector *newPtr);

  // -- q0 --

  /** \fn  const SimpleVector getQ0() const
   *  \brief get the value of q0
   *  \return SimpleVector
   */
  inline const SimpleVector getQ0() const
  {
    return *q0;
  }

  /** \fn SimpleVector* getQ0Ptr() const
   *  \brief get q0
   *  \return pointer on a SimpleVector
   */
  inline SimpleVector* getQ0Ptr() const
  {
    return q0;
  }

  /** \fn void setQ0 (const SimpleVector& newValue)
   *  \brief set the value of q0 to newValue
   *  \param SimpleVector newValue
   */
  void setQ0(const SimpleVector&);

  /** \fn void setQ0Ptr(SimpleVector* newPtr)
   *  \brief set Q0 to pointer newPtr
   *  \param SimpleVector * newPtr
   */
  void setQ0Ptr(SimpleVector *newPtr);

  // -- qFree --

  /** \fn  const SimpleVector getQFree() const
   *  \brief get the value of qFree
   *  \return SimpleVector
   */
  inline const SimpleVector getQFree() const
  {
    return *qFree;
  }

  /** \fn SimpleVector* getQFreePtr() const
   *  \brief get qFree
   *  \return pointer on a SimpleVector
   */
  inline SimpleVector* getQFreePtr() const
  {
    return qFree;
  }

  /** \fn void setQFree (const SimpleVector& newValue)
   *  \brief set the value of qFree to newValue
   *  \param SimpleVector newValue
   */
  void setQFree(const SimpleVector&);

  /** \fn void setQFreePtr(SimpleVector* newPtr)
   *  \brief set QFree to pointer newPtr
   *  \param SimpleVector * newPtr
   */
  void setQFreePtr(SimpleVector *newPtr);

  // Q memory

  /** \fn  const SiconosMemory getQMemory(void) const
   *  \brief get the value of qMemory
   *  \return a SiconosMemory
   */
  inline const SiconosMemory getQMemory() const
  {
    return *qMemory;
  }

  /** \fn SiconosMemory getQMemoryPtr(void) const
   *  \brief get all the values of the state vector q stored in memory
   *  \return a memory
   */
  inline SiconosMemory* getQMemoryPtr() const
  {
    return qMemory;
  }

  /** \fn void setQMemory(const SiconosMemory &)
   *  \brief set the value of qMemory
   *  \param a ref on a SiconosMemory
   */
  void setQMemory(const SiconosMemory&);

  /** \fn void setQMemory(SiconosMemory * newPtr)
   *  \brief set qMemory to pointer newPtr
   *  \param a ref on a SiconosMemory
   */
  void setQMemoryPtr(SiconosMemory *);

  // -- velocity --

  /** \fn  const SimpleVector getVelocity() const
   *  \brief get the value of velocity
   *  \return SimpleVector
   */
  inline const SimpleVector getVelocity() const
  {
    return *velocity;
  }

  /** \fn SimpleVector* getVelocityPtr() const
   *  \brief get velocity
   *  \return pointer on a SimpleVector
   */
  inline SimpleVector* getVelocityPtr() const
  {
    return velocity;
  }

  /** \fn void setVelocity (const SimpleVector& newValue)
   *  \brief set the value of velocity to newValue
   *  \param SimpleVector newValue
   */
  void setVelocity(const SimpleVector&);

  /** \fn void setVelocityPtr(SimpleVector* newPtr)
   *  \brief set Velocity to pointer newPtr
   *  \param SimpleVector * newPtr
   */
  void setVelocityPtr(SimpleVector *newPtr);

  // -- velocity0 --

  /** \fn  const SimpleVector getVelocity0() const
   *  \brief get the value of velocity0
   *  \return SimpleVector
   */
  inline const SimpleVector getVelocity0() const
  {
    return *velocity0;
  }

  /** \fn SimpleVector* getVelocity0Ptr() const
   *  \brief get velocity0
   *  \return pointer on a SimpleVector
   */
  inline SimpleVector* getVelocity0Ptr() const
  {
    return velocity0;
  }

  /** \fn void setVelocity0 (const SimpleVector& newValue)
   *  \brief set the value of velocity0 to newValue
   *  \param SimpleVector newValue
   */
  void setVelocity0(const SimpleVector&);

  /** \fn void setVelocity0Ptr(SimpleVector* newPtr)
   *  \brief set Velocity0 to pointer newPtr
   *  \param SimpleVector * newPtr
   */
  void setVelocity0Ptr(SimpleVector *newPtr) ;

  // -- velocityFree --

  /** \fn  const SimpleVector getVelocityFree() const
   *  \brief get the value of velocityFree
   *  \return SimpleVector
   */
  inline const SimpleVector getVelocityFree() const
  {
    return *velocityFree;
  }

  /** \fn SimpleVector* getVelocityFreePtr() const
   *  \brief get velocityFree
   *  \return pointer on a SimpleVector
   */
  inline SimpleVector* getVelocityFreePtr() const
  {
    return velocityFree;
  }

  /** \fn void setVelocityFree (const SimpleVector& newValue)
   *  \brief set the value of velocityFree to newValue
   *  \param SimpleVector newValue
   */
  void setVelocityFree(const SimpleVector&);

  /** \fn void setVelocityFreePtr(SimpleVector* newPtr)
   *  \brief set VelocityFree to pointer newPtr
   *  \param SimpleVector * newPtr
   */
  void setVelocityFreePtr(SimpleVector *newPtr);

  // Velocity memory

  /** \fn  const SiconosMemory getVelocityMemory(void) const
   *  \brief get the value of velocityMemory
   *  \return a SiconosMemory
   */
  inline const SiconosMemory getVelocityMemory() const
  {
    return *velocityMemory;
  }

  /** \fn SiconosMemory getVelocityMemoryPtr(void) const
   *  \brief get all the values of the state vector velocity stored in memory
   *  \return a memory
   */
  inline SiconosMemory* getVelocityMemoryPtr() const
  {
    return velocityMemory;
  }

  /** \fn void setVelocityMemory(const SiconosMemory &)
   *  \brief set the value of velocityMemory
   *  \param a ref on a SiconosMemory
   */
  void setVelocityMemory(const SiconosMemory&);

  /** \fn void setVelocityMemory(SiconosMemory * newPtr)
   *  \brief set velocityMemory to pointer newPtr
   *  \param a ref on a SiconosMemory
   */
  void setVelocityMemoryPtr(SiconosMemory *);

  // -- p --

  /** \fn  const SimpleVector getP() const
   *  \brief get the value of p
   *  \return SimpleVector
   */
  inline const SimpleVector getP() const
  {
    return *p;
  }

  /** \fn SimpleVector* getPPtr() const
   *  \brief get p
   *  \return pointer on a SimpleVector
   */
  inline SimpleVector* getPPtr() const
  {
    return p;
  }

  /** \fn void setP (const SimpleVector& newValue)
   *  \brief set the value of p to newValue
   *  \param SimpleVector newValue
   */
  void setP(const SimpleVector&);

  /** \fn void setPPtr(SimpleVector* newPtr)
   *  \brief set P to pointer newPtr
   *  \param SimpleVector * newPtr
   */
  void setPPtr(SimpleVector *newPtr);

  // -- Mass --

  /** \fn  const SiconosMatrix getMass() const
   *  \brief get the value of Mass
   *  \return SiconosMatrix
   */
  inline const SiconosMatrix getMass() const
  {
    return *mass;
  }

  /** \fn SiconosMatrix* getMassPtr() const
   *  \brief get Mass
   *  \return pointer on a SiconosMatrix
   */
  inline SiconosMatrix* getMassPtr() const
  {
    return mass;
  }

  /** \fn void setMass (const SiconosMatrix& newValue)
   *  \brief set the value of Mass to newValue
   *  \param SiconosMatrix newValue
   */
  void setMass(const SiconosMatrix&);

  /** \fn void setMassPtr(SiconosMatrix* newPtr)
   *  \brief set Mass to pointer newPtr
   *  \param SiconosMatrix * newPtr
   */
  void setMassPtr(SiconosMatrix *newPtr);

  // -- FInt --

  /** \fn  const SimpleVector getFInt() const
   *  \brief get the value of fInt
   *  \return SimpleVector
   */
  inline const SimpleVector getFInt() const
  {
    return *fInt;
  }

  /** \fn SimpleVector* getFIntPtr() const
   *  \brief get fInt
   *  \return pointer on a SimpleVector
   */
  inline SimpleVector* getFIntPtr() const
  {
    return fInt;
  }

  /** \fn void setFInt (const SimpleVector& newValue)
   *  \brief set the value of fInt to newValue
   *  \param SimpleVector newValue
   */
  void setFInt(const SimpleVector&);

  /** \fn void setFIntPtr(SimpleVector* newPtr)
   *  \brief set FInt to pointer newPtr
   *  \param SimpleVector * newPtr
   */
  void setFIntPtr(SimpleVector *newPtr);

  // -- Fext --

  /** \fn  const SimpleVector getFExt() const
   *  \brief get the value of fExt
   *  \return SimpleVector
   */
  inline const SimpleVector getFExt() const
  {
    return *fExt;
  }

  /** \fn SimpleVector* getFExtPtr() const
   *  \brief get fExt
   *  \return pointer on a SimpleVector
   */
  inline SimpleVector* getFExtPtr() const
  {
    return fExt;
  }

  /** \fn void setFExt (const SimpleVector& newValue)
   *  \brief set the value of fExt to newValue
   *  \param SimpleVector newValue
   */
  void setFExt(const SimpleVector&);

  /** \fn void setFExtPtr(SimpleVector* newPtr)
   *  \brief set FExt to pointer newPtr
   *  \param SimpleVector * newPtr
   */
  void setFExtPtr(SimpleVector *newPtr);

  // -- paramFExt --

  /** \fn  const SimpleVector getParamFExt() const
   *  \brief get the value of fExt
   *  \return SimpleVector
   */
  inline const SimpleVector getParamFExt() const
  {
    return *paramFExt;
  }

  /** \fn SimpleVector* getParamFExtPtr() const
   *  \brief get fExt
   *  \return pointer on a SimpleVector
   */
  inline SimpleVector* getParamFExtPtr() const
  {
    return paramFExt;
  }

  /** \fn void setParamFExt (const SimpleVector& newValue)
   *  \brief set the value of fExt to newValue
   *  \param SimpleVector newValue
   */
  void setParamFExt(const SimpleVector&);

  /** \fn void setParamFExtPtr(SimpleVector* newPtr)
   *  \brief set paramFExt to pointer newPtr
   *  \param SimpleVector * newPtr
   */
  void setParamFExtPtr(SimpleVector *newPtr);

  // -- NNL --

  /** \fn  const SimpleVector getNNL() const
   *  \brief get the value of NNL
   *  \return SimpleVector
   */
  inline const SimpleVector getNNL() const
  {
    return *NNL;
  }

  /** \fn SimpleVector* getNNLPtr() const
   *  \brief get NNL
   *  \return pointer on a SimpleVector
   */
  inline SimpleVector* getNNLPtr() const
  {
    return NNL;
  }

  /** \fn void setNNL (const SimpleVector& newValue)
   *  \brief set the value of NNL to newValue
   *  \param SimpleVector newValue
   */
  void setNNL(const SimpleVector&);

  /** \fn void setNNLPtr(SimpleVector* newPtr)
   *  \brief set NNL to pointer newPtr
   *  \param SimpleVector * newPtr
   */
  void setNNLPtr(SimpleVector *newPtr);

  // -- Jacobian Q Fint --

  /** \fn  const SiconosMatrix getJacobianQFInt() const
   *  \brief get the value of JacobianQFInt
   *  \return SiconosMatrix
   */
  inline const SiconosMatrix getJacobianQFInt() const
  {
    return *jacobianQFInt;
  }

  /** \fn SiconosMatrix* getJacobianQFIntPtr() const
   *  \brief get JacobianQFInt
   *  \return pointer on a SiconosMatrix
   */
  inline SiconosMatrix* getJacobianQFIntPtr() const
  {
    return jacobianQFInt;
  }

  /** \fn void setJacobianQFInt (const SiconosMatrix& newValue)
   *  \brief set the value of JacobianQFInt to newValue
   *  \param SiconosMatrix newValue
   */
  void setJacobianQFInt(const SiconosMatrix&);

  /** \fn void setJacobianQFIntPtr(SiconosMatrix* newPtr)
   *  \brief set JacobianQFInt to pointer newPtr
   *  \param SiconosMatrix * newPtr
   */
  void setJacobianQFIntPtr(SiconosMatrix *newPtr);

  // -- Jacobian velocity Fint --

  /** \fn  const SiconosMatrix getJacobianVelocityFInt() const
   *  \brief get the value of JacobianVelocityFInt
   *  \return SiconosMatrix
   */
  inline const SiconosMatrix getJacobianVelocityFInt() const
  {
    return *jacobianVelocityFInt;
  }

  /** \fn SiconosMatrix* getJacobianVelocityFIntPtr() const
   *  \brief get JacobianVelocityFInt
   *  \return pointer on a SiconosMatrix
   */
  inline SiconosMatrix* getJacobianVelocityFIntPtr() const
  {
    return jacobianVelocityFInt;
  }

  /** \fn void setJacobianVelocityFInt (const SiconosMatrix& newValue)
   *  \brief set the value of JacobianVelocityFInt to newValue
   *  \param SiconosMatrix newValue
   */
  void setJacobianVelocityFInt(const SiconosMatrix&);

  /** \fn void setJacobianVelocityFIntPtr(SiconosMatrix* newPtr)
   *  \brief set JacobianVelocityFInt to pointer newPtr
   *  \param SiconosMatrix * newPtr
   */
  void setJacobianVelocityFIntPtr(SiconosMatrix *newPtr);

  // -- Jacobian Q NNL --

  /** \fn  const SiconosMatrix getJacobianQNNL() const
   *  \brief get the value of JacobianQNNL
   *  \return SiconosMatrix
   */
  inline const SiconosMatrix getJacobianQNNL() const
  {
    return *jacobianQNNL;
  }

  /** \fn SiconosMatrix* getJacobianQNNLPtr() const
   *  \brief get JacobianQNNL
   *  \return pointer on a SiconosMatrix
   */
  inline SiconosMatrix* getJacobianQNNLPtr() const
  {
    return jacobianQNNL;
  }

  /** \fn void setJacobianQNNL (const SiconosMatrix& newValue)
   *  \brief set the value of JacobianQNNL to newValue
   *  \param SiconosMatrix newValue
   */
  void setJacobianQNNL(const SiconosMatrix&);

  /** \fn void setJacobianQNNLPtr(SiconosMatrix* newPtr)
   *  \brief set JacobianQNNL to pointer newPtr
   *  \param SiconosMatrix * newPtr
   */
  void setJacobianQNNLPtr(SiconosMatrix *newPtr);

  // -- Jacobian velocity NNL --

  /** \fn  const SiconosMatrix getJacobianVelocityNNL() const
   *  \brief get the value of JacobianVelocityNNL
   *  \return SiconosMatrix
   */
  inline const SiconosMatrix getJacobianVelocityNNL() const
  {
    return *jacobianVelocityNNL;
  }

  /** \fn SiconosMatrix* getJacobianVelocityNNLPtr() const
   *  \brief get JacobianVelocityNNL
   *  \return pointer on a SiconosMatrix
   */
  inline SiconosMatrix* getJacobianVelocityNNLPtr() const
  {
    return jacobianVelocityNNL;
  }

  /** \fn void setJacobianVelocityNNL (const SiconosMatrix& newValue)
   *  \brief set the value of JacobianVelocityNNL to newValue
   *  \param SiconosMatrix newValue
   */
  void setJacobianVelocityNNL(const SiconosMatrix&);

  /** \fn void setJacobianVelocityNNLPtr(SiconosMatrix* newPtr)
   *  \brief set JacobianVelocityNNL to pointer newPtr
   *  \param SiconosMatrix * newPtr
   */
  void setJacobianVelocityNNLPtr(SiconosMatrix *newPtr);

  /** \fn const bool getIsLDSPlugin(unsigned int& n) const
   *  \brief get a bool to check if member number n is loaded from a plugin or not
   *  to know which member corresponds to n see private member list below.
   *  \return a bool
   */
  inline bool getIsLDSPlugin(const unsigned int& n) const
  {
    return isLDSPlugin[n];
  };

  /** \fn const bool getIsLDSPlugin(unsigned int& n) const
   *  \brief get the full vector isLDSPlugin, to check if member number n is loaded from a plugin or not
   *  to know which member corresponds to n see private member list below.
   *  \return a vector of bool
   */
  inline std::vector<bool> getIsLDSPlugin() const
  {
    return isLDSPlugin;
  };

  /** \fn  std::string getMassFunctionName() const
   *  \brief get name of function that computes mass (if mass from plugin)
   *  \return a string
   */
  inline const std::string getMassFunctionName() const
  {
    return massFunctionName;
  }

  /** \fn  std::string getFIntFunctionName() const
   *  \brief get name of function that computes fInt (if fInt from plugin)
   *  \return a string
   */
  inline const std::string getFIntFunctionName() const
  {
    return fIntFunctionName;
  }

  /** \fn  std::string getFExtFunctionName() const
   *  \brief get name of function that computes fExt (if fExt from plugin)
   *  \return a string
   */
  inline const std::string getFExtFunctionName() const
  {
    return fExtFunctionName;
  }

  /** \fn  std::string getNNLFunctionName() const
   *  \brief get name of function that computes NNL (if NNL from plugin)
   *  \return a string
   */
  inline const std::string getNNLFunctionName() const
  {
    return NNLFunctionName;
  }

  /** \fn  std::string getJacobianQFIntFunctionName() const
   *  \brief get name of function that computes jacobianQFInt (if jacobianQFInt from plugin)
   *  \return a string
   */
  inline const std::string getJacobianQFIntFunctionName() const
  {
    return jacobianQFIntFunctionName;
  }

  /** \fn  std::string getJacobianVelocityFIntFunctionName() const
   *  \brief get name of function that computes jacobianVelocityFInt (if jacobianVelocityFInt from plugin)
   *  \return a string
   */
  inline const std::string getJacobianVelocityFIntFunctionName() const
  {
    return jacobianVelocityFIntFunctionName;
  }

  /** \fn  std::string getJacobianQNNLFunctionName() const
   *  \brief get name of function that computes jacobianQNNL (if jacobianQNNL from plugin)
   *  \return a string
   */
  inline const std::string getJacobianQNNLFunctionName() const
  {
    return jacobianQNNLFunctionName;
  }

  /** \fn  std::string getJacobianVelocityNNLFunctionName() const
   *  \brief get name of function that computes jacobianVelocityNNL (if jacobianVelocityNNL from plugin)
   *  \return a string
   */
  inline const std::string getJacobianVelocityNNLFunctionName() const
  {
    return jacobianVelocityNNLFunctionName;
  }

  // --- PLUGINS RELATED FUNCTIONS ---

  /** \fn void computeMass(const double &)
   *  \brief default function to compute the mass
   *  \param double time : the current time
   *  \exception RuntimeException
   */
  void computeMass(const double &);

  /** \fn void computeMass(const double &, SimpleVector *q)
   *  \brief function to compute the mass
   *  \param double time : the current time, SimpleVector*: pointer on the state vector q
   *  \exception RuntimeException
   */
  void computeMass(const double &, SimpleVector *);

  /** \fn void computeFInt(const double &)
   *  \brief default function to compute the internal strengths
   *  \param double time : the current time
   *  \exception RuntimeException
   */
  void computeFInt(const double &);

  /** \fn void computeFInt(const double &, SimpleVector q, SimpleVector velocity)
   *  \brief function to compute the internal strengths
   *  \param double time : the current time, SimpleVector*: pointers on the state vectors q and velocity (\dot q)
   *  \exception RuntimeException
   */
  void computeFInt(const double &, SimpleVector *, SimpleVector *);

  /** \fn void computeFExt(const double &)
   *  \brief default function to compute the external strengths
   *  \param double time : the current time
   *  \exception RuntimeException
   */
  void computeFExt(const double &);

  /** \fn void computeNNL();
   *  \brief default function to compute the inertia
   *  \exception RuntimeException
   */
  void computeNNL();

  /** \fn void computeNNL(SimpleVector q, SimpleVector velocity);
   *  \brief function to compute the inertia
   *  \param SimpleVector*: pointers on the state vectors q and velocity (\dot q)
   *  \exception RuntimeException
   */
  void computeNNL(SimpleVector *q, SimpleVector *velocity);

  /** \fn void computeJacobianQFInt(const double &)
   *  \brief default function to compute the gradient of the internal strengths compared to the state
   *  \param double time : the current time
   *  \exception RuntimeException
   */
  void computeJacobianQFInt(const double &);

  /** \fn void computeJacobianQFInt(const double &,SimpleVector q, SimpleVector velocity)
   *  \brief function to compute the gradient of the internal strengths compared to state q
   *  \param double time : the current time, SimpleVector*: pointers on the state vectors q and velocity (\dot q)
   *  \exception RuntimeException
   */
  void computeJacobianQFInt(const double &, SimpleVector *q, SimpleVector *velocity);

  /** \fn void computeJacobianVelocityFInt(double time)
   *  \brief function to compute the gradient of the internal strengths compared to velocity
   *  \param double time : the current time
   *  \exception RuntimeException
   */
  void computeJacobianVelocityFInt(const double &);

  /** \fn void computeJacobianVelocityFInt(const double &, SimpleVector q, SimpleVector velocity)
   *  \brief function to compute the gradient of the internal strengths compared to velocity
   *  \param double time : the current time, SimpleVector*: pointers on the state vectors q and velocity (\dot q)
   *  \exception RuntimeException
   */
  void computeJacobianVelocityFInt(const double &, SimpleVector *q, SimpleVector *velocity);

  /** \fn void computeJacobianQNNL(const double &)
   *  \brief function to compute the gradient of the inertia strengths compared to the state q
   *  \exception RuntimeException
   */
  void computeJacobianQNNL();

  /** \fn void computeJacobianQNNL(const double &,SimpleVector q, SimpleVector velocity)
   *  \brief function to compute the gradient of the inertia strengths compared to the state q
   *  \param SimpleVector*: pointers on the state vectors q and velocity (\dot q)
   *  \exception RuntimeException
   */
  void computeJacobianQNNL(SimpleVector *q, SimpleVector *velocity);

  /** \fn void computeJacobianVelocityNNL(const double & )
   *  \brief function to compute the gradient of the inertia strengths compared to velocity
   *  \exception RuntimeException
   */
  void computeJacobianVelocityNNL();

  /** \fn void computeJacobianVelocityNNL(double time, SimpleVector q, SimpleVector velocity )
   *  \brief function to compute the gradient of the inertia strengths compared to velocity
   *  \param SimpleVector*: pointers on the state vectors q and velocity (\dot q)
   *  \exception RuntimeException
   */
  void computeJacobianVelocityNNL(SimpleVector *q, SimpleVector *velocity);

  /** \fn void setComputeMassFunction(const string pluginPath, const string functionName&)
   *  \brief allow to set a specified function to compute the mass
   *  \param string : the complete path to the plugin
   *  \param string : the name of the function to use in this plugin
   *  \exception SiconosSharedLibraryException
   */
  void setComputeMassFunction(const std::string & pluginPath, const std::string & functionName);

  /** \fn void setComputeFIntFunction(const string& pluginPath, const string& functionName)
   *  \brief allow to set a specified function to compute Fint
   *  \param string : the complete path to the plugin
   *  \param string : the name of the function to use in this plugin
   *  \exception SiconosSharedLibraryException
   */
  void setComputeFIntFunction(const std::string & pluginPath, const std::string & functionName);

  /** \fn void setComputeFExtFunction(const string& pluginPath, const string& functionName)
   *  \brief allow to set a specified function to compute Fext
   *  \param string : the complete path to the plugin
   *  \param string : the name of the function to use in this plugin
   *  \exception ICDLL_CSharedLibraryException
   */
  void setComputeFExtFunction(const std::string & pluginPath, const std::string & functionName);

  /** \fn void setComputeNNLFunction(const string& pluginPath, const string& functionName)
   *  \brief allow to set a specified function to compute the inertia
   *  \param string : the complete path to the plugin
   *  \param string : the name of the function to use in this plugin
   *  \exception SiconosCSharedLibraryException
   */
  void setComputeNNLFunction(const std::string & pluginPath, const std::string & functionName);

  /** \fn void setComputeJacobianQFIntFunction(const string& pluginPath, const string& functionName)
   *  \brief allow to set a specified function to compute the gradient of the internal strength compared to the state
   *  \param string : the complete path to the plugin
   *  \param string : the name of the function to use in this plugin
   *  \exception SiconosSharedLibraryException
   */
  void setComputeJacobianQFIntFunction(const std::string & pluginPath, const std::string & functionName);

  /** \fn void setComputeJacobianVelocityFIntFunction(const string& pluginPath, const string& functionName)
   *  \brief allow to set a specified function to compute the internal strength compared to the velocity
   *  \param string : the complete path to the plugin
   *  \param string : the name of the function to use in this plugin
   *  \exception SiconosSharedLibraryException
   */
  void setComputeJacobianVelocityFIntFunction(const std::string & pluginPath, const std::string & functionName);

  /** \fn void setComputeJacobianQNNLFunction(const string& pluginPath, const string& functionName)
   *  \brief allow to set a specified function to compute the gradient of the the external strength compared to the state
   *  \param string : the complete path to the plugin
   *  \param string : the name of the function to use in this plugin
   *  \exception SiconosSharedLibraryException
   */
  void setComputeJacobianQNNLFunction(const std::string & pluginPath, const std::string & functionName);

  /** \fn void setComputeJacobianVelocityNNLFunction(const string& pluginPath, const string& functionName)
   *  \brief allow to set a specified function to compute the external strength compared to the velocity
   *  \param string : the complete path to the plugin
   *  \param string : the name of the function to use in this plugin
   *  \exception SiconosSharedLibraryException
   */
  void setComputeJacobianVelocityNNLFunction(const std::string & pluginPath, const std::string & functionName);

  // --- miscellaneous ---

  /** \fn void saveDSToXML()
   *  \brief copy the data of the DS to the XML tree
   *  \exception RuntimeException
   */
  virtual void saveDSToXML();

  /** \fn void display()
   *  \brief print the data to the screen
   */
  virtual void display() const;

  /** \fn void initMemory(const unsigned int& steps) ;
   *  \brief initialize the SiconosMemory objects with a positive size.
   *  \param the size of the SiconosMemory. must be >= 0
   */
  void initMemory(const unsigned int& steps);

  /** \fn virtual void swapInMemory(void);
   * \brief push the current values of x, q and r in the stored previous values
   *  xMemory, qMemory, rMemory,
   * \todo Modify the function swapIn Memory with the new Object Memory
   */
  void swapInMemory();

  /** \fn LagrangianDS* convert (DynamicalSystem* ds)
   *  \brief encapsulates an operation of dynamic casting. Needed by Python interface.
   *  \param DynamicalSystem* : the system which must be converted
   * \return a pointer on the system if it is of the right type, NULL otherwise
   */
  static LagrangianDS* convert(DynamicalSystem* ds);

  /** \fn double dsConvergenceIndicator()
   *  \brief compute $\frac{|\dot q_{i+1} - \dot qi|}{|\dot q_i|}$ where $\dot q_{i+1}$ represents the present state and $\dot q_i$ the previous one
   * \return a double
   */
  virtual double dsConvergenceIndicator();

protected:

  // -- DEFAULT CONSTRUCTOR --
  /** \fn LagrangianDS()
   *  \brief Default constructor
   */
  LagrangianDS();

  // -- MEMBERS --

  /** number of degrees of freedom of the system */
  unsigned int ndof;
  /** coordinates of the system */
  SimpleVector *q;
  /** initial coordinates of the system */
  SimpleVector *q0;
  /** free coordinate */
  SimpleVector *qFree;
  /** memory of previous coordinates of the system */
  SiconosMemory *qMemory;
  /** velocity of the system */
  SimpleVector *velocity;
  /** initial velocity of the system */
  SimpleVector *velocity0;
  /** free Velocity */
  SimpleVector *velocityFree;
  /** memory of previous velocity of the system */
  SiconosMemory *velocityMemory;
  /** Reaction due to the non smooth law */
  SimpleVector *p;

  /** mass of the system */
  SiconosMatrix *mass;
  /** internal strength of the system */
  SimpleVector *fInt;
  /** external strength of the system */
  SimpleVector *fExt;
  /** list of (user-defined) parameters to customize fExt (example, omega in fExt = cos(Omega t) ) **/
  SimpleVector *paramFExt;
  /** non-linear inertia term of the system */
  SimpleVector *NNL;
  /** jacobian/coordinates of internal strength */
  SiconosMatrix *jacobianQFInt;
  /** jacobian/velocity of internal strength */
  SiconosMatrix *jacobianVelocityFInt;
  /** jacobian/coordinates of inertia */
  SiconosMatrix *jacobianQNNL;
  /** jacobian/velocity of inertie */
  SiconosMatrix *jacobianVelocityNNL;

  /* contains the name of the plugin used to compute the mass */
  std::string  massFunctionName;
  /* contains the name of the plugin used to compute fInt */
  std::string  fIntFunctionName;
  /* contains the name of the plugin used to compute fExt */
  std::string  fExtFunctionName;
  /* contains the name of the plugin used to compute jacobianQFInt */
  std::string  jacobianQFIntFunctionName;
  /* contains the name of the plugin used to compute jacobianQNNL */
  std::string  jacobianQNNLFunctionName;
  /* contains the name of the plugin used to compute jacobianVelocityFInt */
  std::string  jacobianVelocityFIntFunctionName;
  /* contains the name of the plugin used to compute jacobianVelocityNNL */
  std::string  jacobianVelocityNNLFunctionName;
  /* contains the name of the plugin used to compute NNL */
  std::string  NNLFunctionName;

  /** vector of bool to check if mass, fInt, fExt, paramFExt, NNL and the 4 jacobian are loaded from a plugin or not
   The vector order is the one of members list (see above)*/
  std::vector<bool> isLDSPlugin;

  /** class for manage plugin (open, close librairy...) */
  SiconosSharedLibrary cShared;

  /** Flags to know if pointers have been allocated inside constructors or not */

  // for vectors related to q (q, q0, qFree, qMemory)
  std::vector<bool> isQAllocatedIn;
  // the same for velocity
  std::vector<bool> isVelocityAllocatedIn;
  // p
  bool isPAllocatedIn;
  // Mass
  bool isMassAllocatedIn;
  // Forces: fInt, fExt, NNL, paramFExt
  std::vector<bool> areForcesAllocatedIn;
  // Jacobian: jacobianQFInt, jacobianVelocityFInt, jacobianQNNL, jacobianVelocityNNL
  std::vector<bool> isJacobianAllocatedIn;

  // pointers to functions member to compute plug-in functions

  /** \fn void (*computeMassPtr)(unsigned int* sizeOfq,const double * time, double* qPtr, double* massPtr)
   *  \brief compute the mass
   *  \param unsigned int* sizeOfq : the size of the vector q
   *  \param double* time : the time for the computation
   *  \param double* qPtr : the pointer to the first element of the vector q
   *  \param double* massPtr : the pointer to the first element of the matrix mass (in-out parameter)
   */
  void (*computeMassPtr)(const unsigned int*, const double*, const double*, double*);

  /** \fn void (*computeFIntPtr)(int* sizeOfq, double* time, double* qPtr, double* velocityPtr, double* fIntPtr)
   *  \brief computes the internal strengths
   *  \param int* sizeOfq : the size of the vector q
   *    \param int* time : the current time
   *  \param double* qPtr : the pointer to the first element of the vector q
   *  \param double* velocityPtr : the pointer to the first element of the vector velocity
   *  \param double* fIntPtr : the pointer to the first element of the vector FInt (in-out parameter)
   */
  void (*computeFIntPtr)(const unsigned int*, const double*, const double*, const double*, double*);

  /** \fn void (*computeFExtPtr)(unsigned int* sizeOfq, double* time, double* paramPtr, double* fExtPtr)
   *  \brief computes the external strengths
   *  \param unsigned int* sizeOfq : the size of the vector q
   *    \param double* time : the current time
   *    \param double* paramPtr: a list of parameters to customize FExt
   *  \param double* fExtPtr : the pointer to the first element of the vector FExt (in-out parameter)
   */
  void (*computeFExtPtr)(const unsigned int*, const double*, const double*, double* fExtPtr);

  /** \fn void (*computeNNLPtr)(unsigned int* sizeOfq, double* qPtr, double* velocityPtr, double* NNLPtr)
   *  \brief computes the inertia
   *  \param unsigned int* sizeOfq : the size of the vector q
   *  \param double* qPtr : the pointer to the first element of the vector q
   *  \param double* velocityPtr : the pointer to the first element of the vector velocity
   *  \param double* NNLPtr : the pointer to the first element of the vector NNL (in-out parameter)
   */
  void (*computeNNLPtr)(const unsigned int*, const double*, const double*, double*);

  /** \fn void (*computeJacobianQFIntPtr)(int* sizeOfq, double* time, double* qPtr, double* velocityPtr, double* jacobPtr)
   *  \brief computes the gradient of the the internal strength compared to the state
   *  \param unsigned int* sizeOfq : the size of the vector q
   *    \param double* time : the current time
   *  \param double* qPtr : the pointer to the first element of the vector q
   *  \param double* velocityPtr : the pointer to the first element of the vector velocity
   *  \param double* jacobPtr : the pointer to the first element of the matrix JacobianQFInt (in-out parameter)
   */
  void (*computeJacobianQFIntPtr)(const unsigned int*, const double*, const double*, const double*, double*);

  /** \fn void (*computeJacobianVelocityFIntPtr)(unsigned int* sizeOfq, const double* time, double* qPtr, double* velocityPtr, double* jacobPtr);
   *  \brief computes the gradient of the the internal strength compared to the velocity
   *  \param unsigned int* sizeOfq : the size of the vector q
   *    \param double* time : the current time
   *  \param double* qPtr : the pointer to the first element of the vector q
   *  \param double* velocityPtr : the pointer to the first element of the vector velocity
   *  \param double* jacobPtr : the pointer to the first element of the matrix JacobianQFInt (in-out parameter)
   */
  void (*computeJacobianVelocityFIntPtr)(const unsigned int*, const double*, const double*, const double*, double*);

  /** \fn void (*computeJacobianQNNL)(unsigned int* sizeOfq, const double* qPtr, double* velocityPtr, double* jacobPtr)
   *  \brief computes the gradient of the the external strength compared to the state
   *  \param unsigned int* sizeOfq : the size of the vector q
   *  \param double* qPtr : the pointer to the first element of the vector q
   *  \param double* velocityPtr : the pointer to the first element of the vector velocity
   *  \param double* jacobPtr : the pointer to the first element of the matrix JacobianQFInt (in-out parameter)
   */
  void (*computeJacobianQNNLPtr)(const unsigned int*, const double*, const double*, double*);

  /** \fn void (*computeJacobianVelocityNNLPtr)(unsigned int* sizeOfq, const double* qPtr, double* velocityPtr, double* jacobPtr)
   *  \brief computes the gradient of the the external strength compared to the velocity
   *  \param unsigned int* sizeOfq : the size of the vector q
   *  \param double* qPtr : the pointer to the first element of the vector q
   *  \param double* velocityPtr : the pointer to the first element of the vector velocity
   *  \param double* jacobPtr : the pointer to the first element of the matrix JacobianQFInt (in-out parameter)
   */
  void (*computeJacobianVelocityNNLPtr)(const unsigned int*, const double*, const double*, double*);

};

#endif // LAGRANGIANNLDS_H
