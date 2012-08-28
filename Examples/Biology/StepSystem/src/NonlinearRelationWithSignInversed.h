#ifndef NONLINEARRELATIONWITHSIGNINVERSED_H
#define NONLINEARRELATIONWITHSIGNINVERSED_H

#include "SiconosKernel.hpp"

class NonlinearRelationWithSignInversed : public FirstOrderType2R
{
protected:
public:
  NonlinearRelationWithSignInversed();
  virtual ~NonlinearRelationWithSignInversed() {};

  virtual void initialize(Interaction& inter);

  /** default function to compute h
   *  \param double : current time
   */
  virtual void computeh(double, Interaction& inter) ;

  /** default function to compute g
   *  \param double : current time
   */
  virtual void computeg(double, Interaction& inter) ;

  /** default function to compute jacobianH
   *  \param double : current time
   *  \param index for jacobian (0: jacobian according to x, 1 according to lambda)
   */
  virtual void computeJachx(double, Interaction& inter);
  virtual void computeJachlambda(double, Interaction& inter);

  /** default function to compute jacobianG according to lambda
   *  \param double : current time
   *  \param index for jacobian: at the time only one possible jacobian => i = 0 is the default value .
   */
  virtual void computeJacgx(double, Interaction& inter);
  virtual void computeJacglambda(double, Interaction& inter);

};

TYPEDEF_SPTR(NonlinearRelationWithSignInversed);

#endif
