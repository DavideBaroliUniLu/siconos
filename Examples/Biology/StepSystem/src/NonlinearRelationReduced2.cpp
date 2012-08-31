#ifndef NONLINEARRELATIONREDUCED2_CPP
#define NONLINEARRELATIONREDUCED2_CPP

#include "NonlinearRelationReduced2.h"

//#include "const.h"
#define SICONOS_DEBUG

NonlinearRelationReduced2::NonlinearRelationReduced2():
  FirstOrderType2R()
{
}

void NonlinearRelationReduced2::initialize(Interaction& inter)
{
  FirstOrderType2R::initialize(inter);
  unsigned int sizeY = inter.getSizeOfY();
  unsigned int sizeDS = inter.getSizeOfDS();
  //SiconosVector& y = *inter.y(0);
  //SiconosVector& lambda = *inter.lambda(0);

  double t0 = 0;

  _jachx->resize(sizeY, sizeDS);
  _jachlambda->resize(sizeY, sizeY);

  _jacgx->resize(sizeDS, sizeDS);
  _jacglambda->resize(sizeDS, sizeY);


  //  SiconosVector workX = *inter.data(x);
  //_workX->setValue(0,0);
  // y.setValue(0,0);
  // y.setValue(1,0);
  // y.setValue(2,0);
  // y.setValue(3,0);
  // lambda.setValue(0,0);
  // lambda.setValue(1,0);
  // lambda.setValue(2,0);
  // lambda.setValue(3,0);


  computeh(t0, inter);
  computeg(t0, inter);
  computeJach(t0, inter);
  computeJacg(t0, inter);
  //*inter.data(r)=*inter.data(g_alpha);

  /*
  #ifdef SICONOS_DEBUG
    std::cout<<"data[r (g_alpha)] init\n";
    data[r]->display();
  #endif
  */

}

/*y = h(X)*/
void NonlinearRelationReduced2::computeh(double t, Interaction& inter)
{

  SiconosVector workX = *inter.data(x);
  //SiconosVector& lambda = *inter.lambda(0);

  /*
  #ifdef SICONOS_DEBUG
    std::cout<<"******** NonlinearRelationReduced2::computeh computeh at "<<t<<std::endl;
  #endif
  */
  SP::SiconosVector Heval = inter.Halpha();

  Heval->setValue(0, 8.0 - workX(0));
  Heval->setValue(1, 8.0 - workX(1));

}

/*g=g(lambda)*/
void NonlinearRelationReduced2::computeg(double t, Interaction& inter)
{
  SiconosVector& lambda = *inter.lambda(0);
  /*
  #ifdef SICONOS_DEBUG
    std::cout<<"*** NonlinearRelationReduced2::computeg     computeg at: "<<t<<std::endl;
  #endif
  */

  inter.data(g_alpha)->setValue(0, 40.0 * (1 - lambda(0)));
  inter.data(g_alpha)->setValue(1, 40.0 * (1 - lambda(1)));
  /*
  #ifdef SICONOS_DEBUG
    std::cout<<"NonlinearRelationReduced2::computeg with lambda="<<std::endl;
    lambda.display();
    std::cout<<std::endl;
    std::cout<<"NonlinearRelationReduced2::computeg modif g_alpha : \n";
    inter.data(g_alpha)->display();
    std::cout<<std::endl;
  #endif
  */

}

void NonlinearRelationReduced2::computeJachlambda(double t, Interaction& inter)
{
  //    double *h = &(*_jachlambda)(0,0);
  /*
    #ifdef SICONOS_DEBUG
    std::cout<<"NonlinearRelationReduced2::computeJachlambda " <<" at " <<" "<<t<<std::endl;
    #endif
  */
  _jachlambda->zero();
}

void NonlinearRelationReduced2::computeJachx(double t, Interaction& inter)
{

  _jachx->setValue(0, 0, -1);
  _jachx->setValue(0, 1, 0);
  _jachx->setValue(1, 0, 0);
  _jachx->setValue(1, 1, -1);

  /*
  #ifdef SICONOS_DEBUG
    std::cout<<"NonlinearRelationReduced2::computeJachx computeJachx " <<" at " <<" "<<t<<":"<<std::endl;
    Jachx->display();
    std::cout<<std::endl;
  #endif
  */
}

void NonlinearRelationReduced2::computeJacgx(double t, Interaction& inter)
{
  //   double *g = &(*jacgx)(0,0);
  _jacgx->zero();

  /*
  #ifdef SICONOS_DEBUG
    std::cout<<"NonlinearRelationReduced2::computeJacgx " <<" at " <<" "<<t<<std::endl;
  #endif
  */

}

void NonlinearRelationReduced2::computeJacglambda(double t, Interaction& inter)
{

  //SiconosVector& lambda = *inter.lambda(0);

  //  double *g = &(*Jacglambda)(0,0);
  _jacglambda->setValue(0, 0, -40.0);
  _jacglambda->setValue(1, 0,  0.0);
  _jacglambda->setValue(0, 1,  0.0);
  _jacglambda->setValue(1, 1, -40.0);
}
#endif

