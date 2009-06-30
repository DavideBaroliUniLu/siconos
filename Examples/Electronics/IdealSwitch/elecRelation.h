#ifndef ELECRELATION_H
#define ELECRELATION_H

#include "SiconosKernel.hpp"

class elecRelation : public FirstOrderType2R
{
public:
  elecRelation();
  virtual ~elecRelation() {};


  virtual void initialize(SP::Interaction inter);


  /** default function to compute h
   *  \param double : current time
   */
  virtual void computeH(double) ;

  /** default function to compute g
   *  \param double : current time
   */
  virtual void computeG(double) ;

  /** default function to compute jacobianH
   *  \param double : current time
   *  \param index for jacobian (0: jacobian according to x, 1 according to lambda)
   */
  virtual void computeJacH(double, unsigned int);

  /** default function to compute jacobianG according to lambda
   *  \param double : current time
   *  \param index for jacobian: at the time only one possible jacobian => i = 0 is the default value .
   */
  virtual void computeJacG(double, unsigned int);



  double source(double t);

};

TYPEDEF_SPTR(elecRelation);

#endif
