#include "Moreau.h"
// includes to be deleted thanks to factories:
#include "LagrangianLinearTIDS.h"
#include "LagrangianDS.h"

using namespace std;


// --- xml constructor ---
Moreau::Moreau(OneStepIntegratorXML *osiXML, TimeDiscretisation* td, DynamicalSystem* ds) :
  OneStepIntegrator(td, ds), W(NULL), isWAllocatedIn(true), theta(0.1)
{

  integratorXml = osiXML;
  integratorType = MOREAU_INTEGRATOR;
  // Memory allocation for W
  int sizeW = (static_cast<LagrangianDS*>(ds))->getQPtr()->size();
  W = new SiconosMatrix(sizeW, sizeW);

  // xml loading
  if (osiXML != NULL)
  {
    if (integratorXml->hasR()) sizeMem = integratorXml->getR();
    if ((static_cast<MoreauXML*>(integratorXml))->hasW() == true)
    {
      *W = (static_cast<MoreauXML*>(integratorXml))->getW();
    }
    if ((static_cast<MoreauXML*>(integratorXml))->hasTheta() == true)
    {
      theta = (static_cast<MoreauXML*>(integratorXml))->getTheta();
    }
  }
  else RuntimeException::selfThrow("Moreau::Moreau() - xml constructor - IntegratorXML object not exists");
}

// --- constructor from a minimum set of data ---
Moreau::Moreau(TimeDiscretisation* td, DynamicalSystem* ds, const double& newTheta):
  OneStepIntegrator(td, ds), W(NULL), isWAllocatedIn(true), theta(newTheta)
{
  integratorType = MOREAU_INTEGRATOR;
  // Memory allocation for W
  int sizeW = (static_cast<LagrangianDS*>(ds))->getQPtr()->size();
  W = new SiconosMatrix(sizeW, sizeW);
}

Moreau::~Moreau()
{
  if (isWAllocatedIn)
  {
    delete W;
    W = NULL;
  }
  if (ds->getType() == LNLDS) ds->freeTmpWorkVector("LagNLDSMoreau");
}

void Moreau::initialize()
{
  IN("Moreau::initialize\n");
  OneStepIntegrator::initialize();
  // Get initial time
  double t0 = timeDiscretisation->getT0();
  // Compute W
  computeW(t0);
  if (ds->getType() == LNLDS)
    ds->allocateTmpWorkVector("LagNLDSMoreau", W->size(0));
  OUT("Moreau::initialize\n");
}

void Moreau::computeW(const double& t)
{
  SiconosMatrix *M;      // mass matrix
  SiconosMatrix *K, *C ;
  double h = timeDiscretisation->getH(); // time step

  // Memory allocation
  LagrangianDS* d = static_cast<LagrangianDS*>(ds);
  int size = d->getQPtr()->size();
  K = new SiconosMatrix(size, size);
  C = new SiconosMatrix(size, size);
  // Check if W is allocated
  if (W == NULL)
  {
    W = new SiconosMatrix(size, size);
    isWAllocatedIn = true;
  }
  // === Lagrangian dynamical system ===
  if (ds->getType() == LNLDS)
  {
    // Compute Mass matrix
    d->computeMass(t);
    // Compute and get Jacobian:
    d->computeJacobianQFInt(t);
    d->computeJacobianVelocityFInt(t);
    d->computeJacobianQQNLInertia(t);
    d->computeJacobianVelocityQNLInertia(t);
    *K = *(d->getJacobianQFIntPtr()) + *(d->getJacobianQQNLInertiaPtr());
    *C = *(d->getJacobianVelocityFIntPtr()) + *(d->getJacobianVelocityQNLInertiaPtr());
  }
  // === Lagrangian linear time invariant system ===
  else if (ds->getType() == LTIDS)
  {
    // Get K and C
    *K = *((static_cast<LagrangianLinearTIDS*>(d))->getKPtr());
    *C = *((static_cast<LagrangianLinearTIDS*>(d))->getCPtr());
  }
  // === ===
  else RuntimeException::selfThrow("Moreau::computeW - not yet implemented for Dynamical system type :" + ds->getType());
  // Get Mass matrix
  M = d->getMassPtr();
  // Compute W
  *W = *M + h * theta * (*C + h * theta* *K);
  // LU factorization of W
  W->PLUFactorizationInPlace();
  delete K;
  delete C;
}


void Moreau::computeFreeState()
{
  IN("Moreau::computeFreeState\n");
  // get current time, theta and time step
  double t = timeDiscretisation->getStrategyPtr()->getModelPtr()->getCurrentT();
  double h = timeDiscretisation->getH();

  // -- Get the DS --
  LagrangianDS* d = static_cast<LagrangianDS*>(ds);

  // --- RESfree calculus ---
  //
  // Get state i (previous time step)
  SimpleVector* vold, *qold;
  qold = static_cast<SimpleVector*>(d->getQMemoryPtr()->getSiconosVector(0));
  vold = static_cast<SimpleVector*>(d->getVelocityMemoryPtr()->getSiconosVector(0));

  // Previous time step (i)
  double told = t - h;

  // Computation of the external forces
  d->computeFExt(told);
  SimpleVector FExt0 = d->getFExt();
  d->computeFExt(t);
  SimpleVector FExt1 = d->getFExt();
  // RESfree ...
  SimpleVector *v = d->getVelocityPtr();
  SimpleVector *RESfree = new SimpleVector(FExt1.size());
  // Velocity free
  SimpleVector *vfree = d->getVelocityFreePtr();

  // --- Compute Velocity Free ---
  // For general Lagrangian system:
  if (ds->getType() == LNLDS)
  {
    // Get Mass (remark: M is computed for present state during computeW(t) )
    SiconosMatrix *M = d -> getMassPtr();
    // Compute Qint and Fint
    // for state i
    // warning: get values and not pointers
    d->computeQNLInertia(qold, vold);
    d->computeFInt(told, qold, vold);
    SimpleVector QNL0 = d->getQNLInertia();
    SimpleVector FInt0 = d->getFInt();
    // for present state
    // warning: get values and not pointers
    d->computeQNLInertia();
    d->computeFInt(t);
    SimpleVector QNL1 = d->getQNLInertia();
    SimpleVector FInt1 = d->getFInt();
    // Compute ResFree and vfree solution of Wk(v-vfree)=RESfree
    *RESfree = *M * (*v - *vold) + h * ((1.0 - theta) * (QNL0 + FInt0 - FExt0) + theta * (QNL1 + FInt1 - FExt1));
    *vfree = *v - W->PLUForwardBackward((*RESfree));
  }
  // --- For linear Lagrangian:
  else if (ds->getType() == LTIDS)
  {
    // get K, M and C mass pointers
    SiconosMatrix * K = static_cast<LagrangianLinearTIDS*>(d)->getKPtr();
    SiconosMatrix * C = static_cast<LagrangianLinearTIDS*>(d)->getCPtr();
    // Compute ResFree and vfree
    *RESfree = -h * (theta * FExt1 + (1.0 - theta) * FExt0 - (*C * *vold) - (*K * *qold) - h * theta * (*K * *vold));
    *vfree =  *vold - W->PLUForwardBackward((*RESfree));
  }
  else RuntimeException::selfThrow("Moreau::computeFreeState - not yet implemented for Dynamical system type :" + ds->getType());
  // calculate qfree (whereas it is useless for future computation)
  SimpleVector *qfree = d->getQFreePtr();
  *qfree = (*qold) + h * (theta * (*vfree) + (1.0 - theta) * (*vold));
  delete RESfree;
  OUT("Moreau::computeFreeState\n");
}


void Moreau::integrate()
{
  IN("Moreau::integrate()\n");

  double h = timeDiscretisation->getH();
  double t = timeDiscretisation->getStrategyPtr()->getModelPtr()->getCurrentT();
  double told = t - h;

  if (ds->getType() == LNLDS)
  {
    //VL(("Moreau::integrate -- LNLDS\n"));
    // We do not use integrate() for LNDS
  }
  else if (ds->getType() == LTIDS)
  {
    VL(("Moreau::integrate -- LTIDS\n"));
    // get the ds
    LagrangianLinearTIDS* d = static_cast<LagrangianLinearTIDS*>(ds);
    // get q and velocity pointers for current time step
    SimpleVector *v, *q, *vold, *qold;
    q = d->getQPtr();
    v = d->getVelocityPtr();
    // get q and velocity pointers for previous time step
    qold = static_cast<SimpleVector*>(d->getQMemoryPtr()->getSiconosVector(0));
    vold = static_cast<SimpleVector*>(d->getVelocityMemoryPtr()->getSiconosVector(0));
    // get mass, K and C pointers
    SiconosMatrix *M, *K, *C;
    M = d->getMassPtr();
    K = d->getKPtr();
    C = d->getCPtr();
    // get p pointer
    SimpleVector  *p;
    p = d->getPPtr();
    // Inline Version
    // The method computeFExt does not allow to compute directly
    // as a function.  To do that, you have to call directly the function of the plugin
    // or call the F77 function  MoreauLTIDS
    // Computation of the external forces
    d->computeFExt(told);
    SimpleVector FExt0 = d->getFExt();
    d->computeFExt(t);
    SimpleVector FExt1 = d->getFExt();
    // velocity computation
    *v = h * (theta * FExt1 + (1.0 - theta) * FExt0 - (*C * *vold) - (*K * *qold) - h * theta * (*K * *vold)) + *p;
    W->PLUForwardBackwardInPlace((*v));
    *v +=  *vold;
    // q computation
    *q = (*qold) + h * ((theta * (*v)) + (1.0 - theta) * (*vold));
    // Right Way  : Fortran 77 version with BLAS call
    // F77NAME(MoreauLTIDS)(told,t,theta
    //                      ndof, &qold(0),&vold(0),
    //                      &W(0,0),&K(0,0),&C(0,0),fext,
    //                      &v(0),&q(0))

  }
  else RuntimeException::selfThrow("Moreau::integrate - not yet implemented for Dynamical system type :" + ds->getType());
  OUT("Moreau::integrate()\n");
}

void Moreau::updateState()
{
  IN("Moreau::updateState\n");
  // get dynamical system
  LagrangianDS* d = static_cast<LagrangianDS*>(ds);
  // get velocity free, p, velocity and q pointers
  SimpleVector *vfree = d->getVelocityFreePtr();
  SimpleVector *p = d->getPPtr();
  SimpleVector *v = d->getVelocityPtr();
  SimpleVector *q = d->getQPtr();
  // Save value of q and v in stateTmp for future convergence computation
  if (ds->getType() == LNLDS)
    ds->addTmpWorkVector(v, "LagNLDSMoreau");
  // Compute velocity
  double h = timeDiscretisation->getH();
  *v = *vfree +  W->PLUForwardBackward((*p));
  // Compute q
  //  -> get previous time step state
  SimpleVector *vold = static_cast<SimpleVector*>(d->getVelocityMemoryPtr()->getSiconosVector(0));
  SimpleVector *qold = static_cast<SimpleVector*>(d->getQMemoryPtr()->getSiconosVector(0));
  *q = *qold + h * (theta * *v + (1.0 - theta)* *vold);
  // set reaction to zero
  p->zero();
  // --- Update W for general Lagrangian system
  if (ds->getType() == LNLDS)
  {
    double t = timeDiscretisation->getStrategyPtr()->getModelPtr()->getCurrentT();
    computeW(t);
  }
  // Remark: for Linear system, W is already saved in object member w
  OUT("Moreau::updateState\n");
}


void Moreau::display() const
{
  OneStepIntegrator::display();

  cout << "====== Moreau OSI display ======" << endl;
  cout << "____ data of the Moreau Integrator " << endl;
  cout << "| W " << endl;
  if (W != NULL) W->display();
  else cout << "-> NULL" << endl;
  cout << "| theta : " << theta << endl;
  cout << "================================" << endl;
}

void Moreau::saveIntegratorToXML()
{
  IN("Moreau::saveIntegratorToXML\n");
  OneStepIntegrator::saveIntegratorToXML();
  if (integratorXml != NULL)
  {
    (static_cast<MoreauXML*>(integratorXml))->setTheta(theta);
    (static_cast<MoreauXML*>(integratorXml))->setW(W);
  }
  else RuntimeException::selfThrow("Moreau::saveIntegratorToXML - IntegratorXML object not exists");
  OUT("Moreau::saveIntegratorToXML\n");
}

void Moreau::saveWToXML()
{
  IN("Moreau::saveWToXML\n");
  if (integratorXml != NULL)
  {
    (static_cast<MoreauXML*>(integratorXml))->setW(W);
  }
  else RuntimeException::selfThrow("Moreau::saveIntegratorToXML - IntegratorXML object not exists");
  OUT("Moreau::saveWToXML\n");
}

Moreau* Moreau::convert(OneStepIntegrator* osi)
{
  cout << "Moreau::convert (OneStepIntegrator* osi)" << endl;
  Moreau* moreau = dynamic_cast<Moreau*>(osi);
  return moreau;
}

// --- Default constructor ---
Moreau::Moreau(): OneStepIntegrator(), W(NULL), isWAllocatedIn(false), theta(0.1)
{
  integratorType = MOREAU_INTEGRATOR;
}
