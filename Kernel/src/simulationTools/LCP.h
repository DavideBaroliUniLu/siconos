/* Siconos-Kernel version 1.1.4, Copyright INRIA 2005-2006.
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

/** \class LCP
 *  \brief This class is devoted to the formalization and the resolution of the
 * Linear Complementarity Problem (LCP)
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 1.1.4.
 *  \date (Creation) Apr 26, 2004
 *
 *
 * This class is devoted to the formalization and the resolution of the
 * Linear Complementarity Problem (LCP) defined by :
 *  * \f[
 * w =  q + M z
 * \f]
 * \f[
 * w \geq 0, z \geq 0,  z^{T} w =0
 * \f]
 * where
 *    - \f$w \in R^{n} \f$  and \f$z \in R^{n} \f$ are the unknown,
 *    - \f$A \in R^{n \times n } \f$  and \f$q \in R^{n} \f$
 *
 * \todo Correct the computation of M with a correct concatenation process
 * \todo : add "recover" function to start from old values of z and w.
 *
 */
#ifndef LCP_H
#define LCP_H

#include "OneStepNSProblem.h"
#include "LCPXML.h"

class OneStepNSProblem;

class LCP : public OneStepNSProblem
{
private:

  /** Size of the LCP */
  unsigned int nLcp;

  /** contains the vector w of a LCP system */
  SimpleVector *w;

  /** contains the vector z of a LCP system */
  SimpleVector *z;

  /** contains the matrix M of a LCP system */
  SiconosMatrix *M;

  /** contains the vector q of a LCP system */
  SimpleVector *q;

  /** Flags to check wheter pointers were allocated in class constructors or not */
  bool isWAllocatedIn;
  bool isZAllocatedIn;
  bool isMAllocatedIn;
  bool isQAllocatedIn;

public:

  /** \fn LCP()
   *  \brief default constructor
   */
  LCP();

  /** \fn LCP(OneStepNSProblemXML*, Strategy*=NULL)
   *  \brief xml constructor
   *  \param OneStepNSProblemXML* : the XML linked-object
   *  \param Strategy *: the strategy that owns the problem (optional)
   */
  LCP(OneStepNSProblemXML*, Strategy* = NULL);

  /** \fn LCP(Strategy * , const std::string& =DEFAULT_SOLVER, const unsigned int& =DEFAULT_ITER, const double& =DEFAULT_TOL,
   *          const std::string & = DEFAULT_NORMTYPE, const double & =DEFAULT_SEARCHDIR);
   *  \brief constructor from data
   *  \param Strategy *: the strategy that owns this problem
   *  \param string: solver name (optional)
   *  \param unsigned int   : MaxIter (optional) required if a solver is given
   *  \param double : Tolerance (optional) -> for NLGS, Gcp, Latin
   *  \param string : NormType (optional) -> never used at the time
   *  \param double : SearchDirection (optional) -> for Latin
   */
  LCP(Strategy * , const std::string& = DEFAULT_SOLVER, const unsigned int& = DEFAULT_ITER, const double& = DEFAULT_TOL,
      const std::string& = DEFAULT_NORMTYPE, const double& = DEFAULT_SEARCHDIR);

  // --- Destructror ---
  ~LCP();

  // GETTERS/SETTERS

  /** \fn int getNLcp(void)
   *  \brief get the size nLcp of the LCP
   *  \return an int
   */
  inline const unsigned int getNLcp() const
  {
    return nLcp;
  }

  /** \fn void setNLcp(const int&)
   *  \brief set the size of the LCP
   *  \param the size
   */
  inline void setNLcp(const unsigned int& newValue)
  {
    nLcp = newValue;
  }

  // --- W ---
  /** \fn  const SimpleVector getW(void) const
   *  \brief get the value of w, the initial state of the DynamicalSystem
   *  \return SimpleVector
   *  \warning: SiconosVector is an abstract class => can not be an lvalue => return SimpleVector
   */
  inline const SimpleVector getW() const
  {
    return *w;
  }

  /** \fn SimpleVector* getWPtr(void) const
   *  \brief get w, the initial state of the DynamicalSystem
   *  \return pointer on a SimpleVector
   */
  inline SimpleVector* getWPtr() const
  {
    return w;
  }

  /** \fn void setW(const SimpleVector& newValue)
   *  \brief set the value of w to newValue
   *  \param SimpleVector newValue
   */
  void setW(const SimpleVector&);

  /** \fn void setWPtr(SimpleVector* newPtr)
   *  \brief set w to pointer newPtr
   *  \param SimpleVector * newPtr
   */
  void setWPtr(SimpleVector*);

  // --- Z ---
  /** \fn  const SimpleVector getZ(void) const
   *  \brief get the value of z, the initial state of the DynamicalSystem
   *  \return SimpleVector
   *  \warning: SimpleVector is an abstract class => can not be an lvalue => return SimpleVector
   */
  inline const SimpleVector getZ() const
  {
    return *z;
  }

  /** \fn SimpleVector* getZPtr(void) const
   *  \brief get z, the initial state of the DynamicalSystem
   *  \return pointer on a SimpleVector
   */
  inline SimpleVector* getZPtr() const
  {
    return z;
  }

  /** \fn void setZ(const SimpleVector& newValue)
   *  \brief set the value of z to newValue
   *  \param SimpleVector newValue
   */
  void setZ(const SimpleVector&);

  /** \fn void setZPtr(SimpleVector* newPtr)
   *  \brief set z to pointer newPtr
   *  \param SimpleVector * newPtr
   */
  void setZPtr(SimpleVector*) ;

  // --- M ---

  /** \fn  const SimpleMatrix getM(void) const
   *  \brief get the value of M
   *  \return SimpleMatrix
   */
  inline const SimpleMatrix getM() const
  {
    return *M;
  }

  /** \fn SiconosMatrix* getMPtr(void) const
   *  \brief get M
   *  \return pointer on a SiconosMatrix
   */
  inline SiconosMatrix* getMPtr() const
  {
    return M;
  }

  /** \fn void setM (const SiconosMatrix& newValue)
   *  \brief set the value of M to newValue
   *  \param SiconosMatrix newValue
   */
  void setM(const SiconosMatrix&);

  /** \fn void setMPtr(SiconosMatrix* newPtr)
   *  \brief set M to pointer newPtr
   *  \param SiconosMatrix * newPtr
   */
  void setMPtr(SiconosMatrix *);

  // --- Q ---
  /** \fn  const SimpleVector getQ(void) const
   *  \brief get the value of q, the initial state of the DynamicalSystem
   *  \return SimpleVector
   *  \warning: SimpleVector is an abstract class => can not be an lvalue => return SimpleVector
   */
  inline const SimpleVector getQ() const
  {
    return *q;
  }

  /** \fn SimpleVector* getQPtr(void) const
   *  \brief get q, the initial state of the DynamicalSystem
   *  \return pointer on a SimpleVector
   */
  inline SimpleVector* getQPtr() const
  {
    return q;
  }

  /** \fn void setQ(const SimpleVector& newValue)
   *  \brief set the value of q to newValue
   *  \param SimpleVector newValue
   */
  void setQ(const SimpleVector&);

  /** \fn void setQPtr(SimpleVector* newPtr)
   *  \brief set q to pointer newPtr
   *  \param SimpleVector * newPtr
   */
  void setQPtr(SimpleVector*);

  // --- Others functions ---

  /** \fn void initialize()
   *  \brief initialize the LCP problem(compute topology ...)
   */
  void initialize();

  /** \fn void computeAllBlocks()
   * \brief compute all the blocks matrices necessary to assemble Mlcp
   */
  void computeAllBlocks();

  /** \fn void computeDiagonalBlocksLinearTIR(Relation *, unsigned int sizeInteraction, vector<DynamicalSystem*>, SiconosMatrix*)
   * \brief compute diagonal block-matrices for a LinearTIR
   */
  void computeDiagonalBlocksLinearTIR(Relation *, const unsigned int&, std::vector<DynamicalSystem*>, std::map<DynamicalSystem*, SiconosMatrix*>, const double&, SiconosMatrix*);


  void computeExtraDiagonalBlocksLinearTIR(Relation *, Relation*, const unsigned int&, const unsigned int&, std::vector<DynamicalSystem*>,
      std::map<DynamicalSystem*, SiconosMatrix*>, const double&, SiconosMatrix*);

  void computeDiagonalBlocksLagrangianR(Relation *, const unsigned int&, std::vector<DynamicalSystem*>, std::map < DynamicalSystem*,
                                        SiconosMatrix* > , const double&, SiconosMatrix*);


  void computeExtraDiagonalBlocksLagrangianR(Relation *, Relation*, const unsigned int&, const unsigned int&, std::vector<DynamicalSystem*>,
      std::map<DynamicalSystem*, SiconosMatrix*>, const double&, SiconosMatrix*);

  /** \fn void updateBlocks();
   *  \brief update blocks value for nonlinear relations
   *  \return void
   */
  void updateBlocks();

  /** \fn void preLCP(const double& time)
   *  \brief pre-treatment for LCP
   *  \param double : current time
   *  \return void
   */
  void preLCP(const double& time);

  /** \fn void assembleM()
   *  \brief built matrix M using already computed blocks
   */
  void assembleM();

  /** \fn void computeQ (void)
   *  \brief compute vector q
   *  \param double : current time
   */
  void computeQ(const double& time);

  /** \fn void compute(const double& time)
   *  \brief Compute the unknown z and w and update the Interaction (y and lambda )
   *  \param double : current time
   *  \return void
   */
  void compute(const double& time);

  /** \fn void postLCP(const SimpleVector& w, SimpleVector& z)
   *  \brief post-treatment for LCP
   *  \param 2 simple vectors: output of LCP solver
   *  \return void
   */
  void postLCP(const SimpleVector&, const SimpleVector&);

  /** \fn void display()
   *  \brief print the data to the screen
   */
  void display() const;

  /** \fn void saveRelationToXML()
   *  \brief copy the data of the OneStepNSProblem to the XML tree
   *  \exception RuntimeException
   */
  void saveNSProblemToXML();

  /** \fn void saveMToXML()
   *  \brief copy the matrix M of the OneStepNSProblem to the XML tree
   *  \exception RuntimeException
   */
  void saveMToXML();

  /** \fn void saveQToXML()
   *  \brief copy the vector q of the OneStepNSProblem to the XML tree
   *  \exception RuntimeException
   */
  void saveQToXML();

  /** \fn LCP* convert (OneStepNSProblem* ds)
   *  \brief encapsulates an operation of dynamic casting. Needed by Python interface.
   *  \param OneStepNSProblem* : the one step problem which must be converted
   * \return a pointer on the problem if it is of the right type, NULL otherwise
   */
  static LCP* convert(OneStepNSProblem* osnsp);


};

#endif // LCP_H
