#include "LinearTIR.h"
using namespace std;


// xml constructor
LinearTIR::LinearTIR(RelationXML* relxml, Interaction * inter):
  Relation(relxml, inter), C(NULL), D(NULL), F(NULL), e(NULL), B(NULL), a(NULL)
{
  relationType = LINEARTIRELATION;
  if (relationxml != NULL)
  {
    LinearTIRXML * lTIRxml = static_cast<LinearTIRXML *>(relationxml);
    isAllocatedIn.resize(6, false);
    unsigned int sizeY, sizeX; // size of vector y and of vector x
    if (lTIRxml->hasC())
    {
      sizeY = lTIRxml->getC().size(0);
      sizeX = lTIRxml->getC().size(1);
    }
    else
      RuntimeException::selfThrow("LinearTIR:: xml constructor: input matrix C is missing in xml file ");

    if (inter != NULL)
    {
      // get size of vector y from linked interaction
      unsigned int size = interaction->getNInteraction();
      if (size != sizeY)
        RuntimeException::selfThrow("LinearTIR:: xml constructor, inconsistent size between C and y vector");
    }

    C = new SiconosMatrix(sizeY, sizeX);
    isAllocatedIn[0] = true;
    *C = lTIRxml->getC();

    if (lTIRxml->hasD())
    {
      if (lTIRxml->getD().size(0) != sizeY || lTIRxml->getD().size(0) != sizeY)
        RuntimeException::selfThrow("LinearTIR:: xml constructor, inconsistent size between D and C");
      D = new SiconosMatrix(sizeY, sizeY);
      isAllocatedIn[1] = true;
      *D = lTIRxml->getD();
    }
    if (lTIRxml->hasF())
    {
      unsigned int uSize = lTIRxml->getF().size(1);
      if (lTIRxml->getF().size(0) != sizeY)
        RuntimeException::selfThrow("LinearTIR:: xml constructor, inconsistent size between F and C");
      F = new SiconosMatrix(sizeY, uSize);
      isAllocatedIn[2] = true;
      *F = lTIRxml->getF();
    }
    if (lTIRxml->hasE())

    {
      if (lTIRxml->getE().size() != sizeY)
        RuntimeException::selfThrow("LinearTIR:: xml constructor, inconsistent size between e and C");
      e = new SimpleVector(sizeY);
      isAllocatedIn[3] = true;
      *e = lTIRxml->getE();
    }
    if (lTIRxml->hasB())
    {
      if (lTIRxml->getB().size(0) != sizeX || lTIRxml->getB().size(1) != sizeY)
        RuntimeException::selfThrow("LinearTIR:: xml constructor, inconsistent size between B and ds vector");
      B = new SiconosMatrix(sizeX, sizeY);
      isAllocatedIn[4] = true;
      *B = lTIRxml->getB();
    }
    else
      RuntimeException::selfThrow("LinearTIR:: xml constructor: input matrix B is missing in xml file ");

    if (lTIRxml->hasA())
    {
      if (lTIRxml->getA().size() != sizeX)
        RuntimeException::selfThrow("LinearTIR:: xml constructor, inconsistent size between a and B");
      a = new SimpleVector(sizeX);
      isAllocatedIn[5] = true;
      *a = lTIRxml->getA();
    }
  }
  else RuntimeException::selfThrow("LinearTIR::xml constructor, xml file=NULL");
}

// Minimum data (C, B) constructor
LinearTIR::LinearTIR(const SiconosMatrix& newC, const SiconosMatrix& newB, Interaction* inter):
  Relation(inter), C(NULL), D(NULL), F(NULL), e(NULL), B(NULL), a(NULL)
{
  relationType = LINEARTIRELATION;
  unsigned int sizeY, sizeX;
  sizeY = newC.size(0);
  sizeX = newC.size(1);
  if (newB.size(0) != sizeX || newB.size(1) != sizeY)
    RuntimeException::selfThrow("LinearTIR:: constructor from data, inconsistent size between C and B");

  if (inter != NULL)
  {
    // get size of vector y
    unsigned int size = interaction->getNInteraction();
    if (sizeY != size)
      RuntimeException::selfThrow("LinearTIR:: constructor from data, inconsistent size with y vector for input vector or matrix");
  }

  C = new SiconosMatrix(sizeY, sizeX);
  B = new SiconosMatrix(sizeX, sizeY);
  *C = newC;
  *B = newB;
  isAllocatedIn.resize(6, false);
  isAllocatedIn[0] = true;
  isAllocatedIn[4] = true;
}

// Constructor from a complete set of data
LinearTIR::LinearTIR(const SiconosMatrix& newC, const SiconosMatrix& newD,
                     const SiconosMatrix& newF, const SimpleVector& newE,
                     const SiconosMatrix& newB, const SimpleVector& newA, Interaction* inter):
  Relation(inter), C(NULL), D(NULL), F(NULL), e(NULL), B(NULL), a(NULL)
{
  relationType = LINEARTIRELATION;
  unsigned int sizeY, sizeX; // size of vector y and of vector x

  sizeY = newC.size(0);
  sizeX = newC.size(1);

  if (inter != NULL)
  {
    unsigned int size = interaction->getNInteraction();
    if (size != sizeY)
      RuntimeException::selfThrow("LinearTIR:: constructor from data, inconsistent size between C and y vector");
  }

  C = new SiconosMatrix(sizeY, sizeX);
  *C = newC;

  if (newD.size(0) != sizeY || newD.size(1) != sizeY)
    RuntimeException::selfThrow("LinearTIR:: constructor from data, inconsistent size between C and D");
  D = new SiconosMatrix(sizeY, sizeY);
  *D = newD;

  // \todo check newF.size(1) = size of u and that the ds type is well adapted to this kind of relation
  if (newF.size(0) != sizeY)
    RuntimeException::selfThrow("LinearTIR:: constructor from data, inconsistent size between F and C");
  F = new SiconosMatrix(sizeY, newF.size(1));
  *F = newF;

  if (newE.size(0) != sizeY)
    RuntimeException::selfThrow("LinearTIR:: constructor from data, inconsistent size between e and C");
  e = new SimpleVector(sizeY);
  *e = newE;

  if (newB.size(0) != sizeX || newB.size(1) != sizeY)
    RuntimeException::selfThrow("LinearTIR:: constructor from data, inconsistent size between C and B");
  B = new SiconosMatrix(sizeX, sizeY);
  *B = newB;

  if (newA.size(0) != sizeX)
    RuntimeException::selfThrow("LinearTIR:: constructor from data, inconsistent size between a and B");
  a = new SimpleVector(sizeX);
  *a = newA;

  isAllocatedIn.resize(6, true);
}

// Copy constructor (inter is optional)
LinearTIR::LinearTIR(const Relation & newLTIR, Interaction* inter):
  Relation(newLTIR, inter), C(NULL), D(NULL), F(NULL), e(NULL), B(NULL), a(NULL)
{
  if (relationType != LINEARTIRELATION)
    RuntimeException::selfThrow("LinearTIR:: copy constructor, inconsistent relation types for copy");

  // Warning: the interaction link is not copyed!!!

  const LinearTIR * ltir = static_cast<const LinearTIR*>(&newLTIR);
  isAllocatedIn.resize(6, false);

  // Since this is a copy, we suppose that various sizes of members of newLTIR are consistent alltogether
  // -> no more tests on that subject.

  C = new SiconosMatrix(ltir->getC());
  B = new SiconosMatrix(ltir->getB());

  isAllocatedIn[0] = true; // C
  isAllocatedIn[4] = true; // B

  if (ltir->getDPtr() != NULL)
  {
    D = new SiconosMatrix(ltir->getD());
    isAllocatedIn[1] = true;

  }
  if (ltir->getFPtr() != NULL)
  {
    F = new SiconosMatrix(ltir->getF());
    isAllocatedIn[2] = true;

  }
  if (ltir->getEPtr() != NULL)
  {
    e = new SimpleVector(ltir->getE());
    isAllocatedIn[3] = true;

  }
  if (ltir->getAPtr() != NULL)
  {
    a = new SimpleVector(ltir->getA());
    isAllocatedIn[5] = true;
  }
}


LinearTIR::~LinearTIR()
{
  if (isAllocatedIn[0])
  {
    delete C;
    C = NULL;
  }
  if (isAllocatedIn[1])
  {
    delete D;
    D = NULL;
  }
  if (isAllocatedIn[2])
  {
    delete F;
    F = NULL;
  }
  if (isAllocatedIn[3])
  {
    delete e;
    e = NULL;
  }
  if (isAllocatedIn[4])
  {
    delete B;
    B = NULL;
  }
  if (isAllocatedIn[5])
  {
    delete a;
    a = NULL;
  }
}

// setters

void LinearTIR::setC(const SiconosMatrix& newValue)
{
  unsigned int sizeY;
  if (interaction != NULL)
  {
    sizeY = interaction->getNInteraction();
    if (newValue.size(0) != sizeY)
      RuntimeException::selfThrow("LinearTIR - setC: inconsistent dimensions with problem size for input matrix C");
  }

  if (C == NULL)
  {
    C = new SiconosMatrix(newValue);
    isAllocatedIn[0] = true;
  }
  else
  {
    if (newValue.size(1) == C->size(1))
      *C = newValue;
    else
      RuntimeException::selfThrow("LinearTIR - setC: inconsistent dimensions with problem size for input matrix C");
  }
}

void LinearTIR::setCPtr(SiconosMatrix *newPtr)
{
  if (isAllocatedIn[0]) delete C;
  if (interaction != NULL)
  {
    unsigned int sizeY = interaction->getNInteraction();
    if (newPtr->size(0) != sizeY)
      RuntimeException::selfThrow("LinearTIR - setCPtr: inconsistent dimensions with problem size for input matrix C");
  }

  C = newPtr;
  isAllocatedIn[0] = false;
}

void LinearTIR::setD(const SiconosMatrix& newValue)
{
  unsigned int sizeY = newValue.size(0);
  if (sizeY != newValue.size(1))
    RuntimeException::selfThrow("LinearTIR - setD:  D is not square!!");

  if (interaction != NULL)
  {
    unsigned int size = interaction->getNInteraction();
    if (size != sizeY)
      RuntimeException::selfThrow("LinearTIR - setD: inconsistent dimensions with problem size for input matrix D");
  }

  if (D == NULL)
  {
    D = new SiconosMatrix(newValue);
    isAllocatedIn[1] = true;
  }
  else
  {
    if (sizeY == D->size(0))
      *D = newValue;
    else
      RuntimeException::selfThrow("LinearTIR - setD: inconsistent dimensions with problem size for input matrix D");
  }
}

void LinearTIR::setDPtr(SiconosMatrix *newPtr)
{
  if (isAllocatedIn[1])  delete D;
  if (interaction != NULL)
  {
    unsigned int sizeY = interaction->getNInteraction();
    if (newPtr->size(0) != sizeY || newPtr->size(1) != sizeY)
      RuntimeException::selfThrow("LinearTIR - setDPtr: inconsistent dimensions with problem size for input matrix D");
  }
  D = newPtr;
  isAllocatedIn[1] = false;
}

void LinearTIR::setF(const SiconosMatrix& newValue)
{
  unsigned int sizeY = newValue.size(0);
  if (interaction != NULL)
  {
    unsigned int size = interaction->getNInteraction();
    if (size != sizeY)
      RuntimeException::selfThrow("LinearTIR - setF: inconsistent dimensions with problem size for input matrix F");
  }

  if (F == NULL)
  {
    F = new SiconosMatrix(newValue);
    isAllocatedIn[2] = true;
  }
  else
  {
    if (newValue.size(1) == F->size(1))
      *F = newValue;
    else
      RuntimeException::selfThrow("LinearTIR - setF: inconsistent dimensions with problem size for input matrix F");
  }
}

void LinearTIR::setFPtr(SiconosMatrix *newPtr)
{
  if (isAllocatedIn[2]) delete F;
  if (interaction != NULL)
  {
    unsigned int sizeY = interaction->getNInteraction();
    if (newPtr->size(0) != sizeY)
      RuntimeException::selfThrow("LinearTIR - setFPtr: inconsistent dimensions with problem size for input matrix F");
  }
  F = newPtr;
  isAllocatedIn[2] = false;
}

void LinearTIR::setE(const SimpleVector& newValue)
{
  unsigned int sizeY = newValue.size();
  if (interaction != NULL)
  {
    unsigned int size = interaction->getNInteraction();
    if (size != sizeY)
      RuntimeException::selfThrow("LinearTIR - setE: inconsistent dimensions with problem size for input vector e");
  }

  if (e == NULL)
  {
    e = new SimpleVector(newValue);
    isAllocatedIn[3] = true;
  }
  else
  {
    if (sizeY == e->size())
      *e = newValue;
    else
      RuntimeException::selfThrow("LinearTIR - setE: inconsistent dimensions with problem size for input vector e");
  }
}

void LinearTIR::setEPtr(SimpleVector *newPtr)
{
  if (isAllocatedIn[3]) delete e;
  if (interaction != NULL)
  {
    unsigned int sizeY = interaction->getNInteraction();
    if (newPtr->size() != sizeY)
      RuntimeException::selfThrow("LinearTIR - setEPtr: inconsistent dimensions with problem size for input matrix E");
  }
  e = newPtr;
  isAllocatedIn[3] = false;
}

void LinearTIR::setB(const SiconosMatrix& newValue)
{
  unsigned int sizeY = newValue.size(1);
  unsigned int sizeX = newValue.size(0);

  if (interaction != NULL)
  {
    unsigned int size = interaction->getNInteraction();
    if (size != sizeY)
      RuntimeException::selfThrow("LinearTIR - setB: inconsistent dimensions with problem size for input matrix B");
  }

  if (B == NULL)
  {
    B = new SiconosMatrix(newValue);
    isAllocatedIn[4] = true;
  }
  else
  {
    if (sizeX == B->size(0))
      *B = newValue;
    else
      RuntimeException::selfThrow("LinearTIR - setB: inconsistent dimensions with problem size for input matrix B");
  }
}

void LinearTIR::setBPtr(SiconosMatrix *newPtr)
{
  if (isAllocatedIn[4]) delete B;
  if (interaction != NULL)
  {
    unsigned int sizeY = interaction->getNInteraction();
    if (newPtr->size(1) != sizeY)
      RuntimeException::selfThrow("LinearTIR - setBPtr: inconsistent dimensions with problem size for input matrix B");
  }
  B = newPtr;
  isAllocatedIn[4] = false;
}

void LinearTIR::setA(const SimpleVector& newValue)
{
  unsigned int sizeY = newValue.size();
  if (interaction != NULL)
  {
    unsigned int size = interaction->getNInteraction();
    if (size != sizeY)
      RuntimeException::selfThrow("LinearTIR - setA: inconsistent dimensions with problem size for input vector a");
  }

  if (a == NULL)
  {
    a = new SimpleVector(newValue);
    isAllocatedIn[5] = true;
  }
  else
  {
    if (sizeY == a->size())
      *a = newValue;
    else
      RuntimeException::selfThrow("LinearTIR - setA: inconsistent dimensions with problem size for input vector a");
  }
}

void LinearTIR::setAPtr(SimpleVector *newPtr)
{
  if (isAllocatedIn[5]) delete a;
  a = newPtr;
  isAllocatedIn[5] = false;
}

void LinearTIR::getCBlockDSPtr(DynamicalSystem * ds, SiconosMatrix& CBlock) const
{
  unsigned int k = 0;

  vector<DynamicalSystem*> vDS = interaction ->getDynamicalSystems();

  vector<DynamicalSystem*>::iterator itDS;
  itDS = vDS.begin();

  // look for ds
  while (*itDS != ds && itDS != vDS.end())
  {
    k += (*itDS)->getN();
    itDS++;
  }

  // check dimension
  if ((*itDS)->getN() != CBlock.size(1))
    RuntimeException::selfThrow("LinearTIR - getCBlockDSPtr: inconsistent sizes between CBlock and DS");

  // get block
  unsigned int l = k + (*itDS)->getN() - 1;
  vector<unsigned int> index_list(4);
  index_list[0] = 0;
  index_list[1] = C->size(0) - 1;
  index_list[2] = k;
  index_list[3] = l;
  C->getBlock(index_list, CBlock);
}

void LinearTIR::getCBlockDSPtr(const int& DSNumber, SiconosMatrix& CBlock) const
{
  unsigned int k = 0;

  vector<DynamicalSystem*> vDS = interaction ->getDynamicalSystems();

  vector<DynamicalSystem*>::iterator itDS;
  itDS = vDS.begin();

  // look for DS number DSNumber ...
  while ((*itDS)->getNumber() != DSNumber && itDS != vDS.end())
  {
    k += (*itDS)->getN();
    itDS++;
  }

  // check dimension
  if ((*itDS)->getN() != CBlock.size(1))
    RuntimeException::selfThrow("LinearTIR - getCBlockDSPtr: inconsistent sizes between CBlock and DS");

  // get block
  unsigned int l = k + (*itDS)->getN() - 1;
  vector<unsigned int> index_list(4);
  index_list[0] = 0;
  index_list[1] = C->size(0) - 1;
  index_list[2] = k;
  index_list[3] = l;
  C->getBlock(index_list, CBlock);
}

void LinearTIR::getBBlockDSPtr(DynamicalSystem* ds, SiconosMatrix& BBlock) const
{
  unsigned int k = 0;

  vector<DynamicalSystem*> vDS = interaction ->getDynamicalSystems();

  vector<DynamicalSystem*>::iterator itDS;
  itDS = vDS.begin();

  while ((*itDS) != ds && itDS != vDS.end())
  {
    k += (*itDS)->getN();
    itDS++;
  }
  // check dimension
  if ((*itDS)->getN() != BBlock.size(0))
    RuntimeException::selfThrow("LinearTIR - getBBlockDSPtr: inconsistent sizes between BBlock and DS");

  // get block
  unsigned int l = k + (*itDS)->getN() - 1;
  vector<unsigned int> index_list(4);
  index_list[0] = k;
  index_list[1] = l;
  index_list[2] = 0;
  index_list[3] = B->size(1) - 1;

  B->getBlock(index_list, BBlock);
}

void LinearTIR::getBBlockDSPtr(const int& DSNumber, SiconosMatrix& BBlock) const
{
  unsigned int k = 0;

  vector<DynamicalSystem*> vDS = interaction ->getDynamicalSystems();

  vector<DynamicalSystem*>::iterator itDS;
  itDS = vDS.begin();

  while ((*itDS)->getNumber() != DSNumber && itDS != vDS.end())
  {
    k += (*itDS)->getN();
    itDS++;
  }
  // check dimension
  if ((*itDS)->getN() != BBlock.size(0))
    RuntimeException::selfThrow("LinearTIR - getBBlockDSPtr: inconsistent sizes between BBlock and DS");

  // get block
  unsigned int l = k + (*itDS)->getN() - 1;
  vector<unsigned int> index_list(4);
  index_list[0] = k;
  index_list[1] = l;
  index_list[2] = 0;
  index_list[3] = B->size(1) - 1;

  B->getBlock(index_list, BBlock);
}

void LinearTIR::computeOutput(const double& time)
{
  IN("LinearTIR::computeOutput\n");
  vector<DynamicalSystem*> vDS = interaction->getDynamicalSystems();
  CompositeVector *xTmp = new CompositeVector();
  CompositeVector *uTmp = new CompositeVector();
  vector<DynamicalSystem*>::iterator it;
  for (it = vDS.begin(); it != vDS.end(); it++)
  {
    // Put x and u of each DS into a composite
    // Warning: use copy constructors, no link between pointers
    if ((*it)->getType() != LDS)
      RuntimeException::selfThrow("LinearTIR - computeOutput: not yet implemented for DS type " + (*it)->getType());

    xTmp->add((*it)->getX());
    if ((*it)->getUPtr() != NULL)
      uTmp->add(*((*it)->getUPtr())) ;
  }

  SimpleVector *y = interaction->getYPtr(0);
  SimpleVector *lambda = interaction->getLambdaPtr(0);

  // compute y
  if (D == NULL && F == NULL && e == NULL)
    *y = *C * *xTmp;

  else if (D != NULL && F == NULL && e == NULL)
    *y = *C * *xTmp + *D * *lambda;

  else if (D != NULL && F != NULL && e == NULL)
    *y = *C * *xTmp + *F * *uTmp + *D * *lambda;

  else if (D != NULL && F != NULL && e != NULL)
    *y = *C * *xTmp + *F * *uTmp + *D * *lambda + *e;

  else if (D != NULL  && F == NULL && e != NULL)
    *y = *C * *xTmp + *D * *lambda + *e;

  else if (D == NULL  && F != NULL && e == NULL)
    *y = *C * *xTmp + *F * *uTmp;

  else if (D == NULL  && F != NULL && e != NULL)
    *y = *C * *xTmp + *F * *uTmp + *D * *lambda + *e;

  else // if (D == NULL  && F == NULL && e!= NULL)
    *y = *C * *xTmp + *e;

  // \todo update y, yDot ... depending on the relative degree.

  // free memory
  delete xTmp;
  delete uTmp;
  OUT("LinearTIR::computeOutput\n");
}
void LinearTIR::computeFreeOutput(const double& time)
{
  IN("LinearTIR::computeOutput\n");
  vector<DynamicalSystem*> vDS = interaction->getDynamicalSystems();
  CompositeVector *xTmp = new CompositeVector();
  //CompositeVector *uTmp = new CompositeVector();
  vector<DynamicalSystem*>::iterator it;
  // \warning : to be reviewed with control term u !!!

  for (it = vDS.begin(); it != vDS.end(); it++)
  {
    // Put xFree and uFree of each DS into a composite
    // Warning: use copy constructors, no link between pointers
    if ((*it)->getType() != LDS)
      RuntimeException::selfThrow("LinearTIR - computeOutput: not yet implemented for DS type " + (*it)->getType());

    xTmp->add((*it)->getXFree());
    if ((*it)->getUPtr() != NULL)
      RuntimeException::selfThrow("LinearTIR - computeOutput: not yet implemented when control term u is present");

    // uTmp->add( *((*it)->getUPtr())) ;
  }

  SimpleVector *yFree = interaction->getYPtr(0);
  // warning : yFree is saved in y !!

  // compute y
  if (F != NULL)
    RuntimeException::selfThrow("LinearTIR - computeOutput: not yet implemented when control term u is present");

  if (e == NULL)
    *yFree = *C * *xTmp;

  else
    *yFree = *C * *xTmp + *e;

  // \todo update y, yDot ... depending on the relative degree.

  // free memory
  delete xTmp;
  //delete uTmp;
  OUT("LinearTIR::computeOutput\n");
}

void LinearTIR::computeInput(const double& time)
{
  vector<DynamicalSystem*> vDS = interaction->getDynamicalSystems();
  CompositeVector *r = new CompositeVector();
  vector<DynamicalSystem*>::iterator it;
  for (it = vDS.begin(); it != vDS.end(); it++)
  {
    // Put r of each DS into a composite
    // Warning: use addPtr -> link between pointers
    r->addPtr((*it)->getRPtr());
  }

  SimpleVector *lambda = interaction->getLambdaPtr(0);

  if (a == NULL)
    *r += *B * *lambda;
  else
    *r += *B * *lambda + *a;
}

void LinearTIR::display() const
{
  cout << " ===== Linear Time Invariant relation display ===== " << endl;
  cout << "| C " << endl;
  if (C != NULL) C->display();
  else cout << "->NULL" << endl;
  cout << "| D " << endl;
  if (D != NULL) D->display();
  else cout << "->NULL" << endl;
  cout << "| F " << endl;
  if (F != NULL) F->display();
  else cout << "->NULL" << endl;
  cout << "| e " << endl;
  if (e != NULL) e->display();
  else cout << "->NULL" << endl;
  cout << "| B " << endl;
  if (B != NULL) B->display();
  else cout << "->NULL" << endl;
  cout << "| a " << endl;
  if (a != NULL) a->display();
  else cout << "->NULL" << endl;
  cout << " ================================================== " << endl;
}

void LinearTIR::saveRelationToXML()
{
  OUT("LinearTIR::saveRelationToXML\n");
  if (relationxml != NULL)
  {
    LinearTIRXML * lTIRxml = (static_cast<LinearTIRXML*>(relationxml));
    lTIRxml->setC(*C);
    lTIRxml->setD(*D);
    lTIRxml->setF(*F);
    lTIRxml->setE(*e);
    lTIRxml->setB(*B);
    lTIRxml->setA(*a);
  }
}

LinearTIR* LinearTIR::convert(Relation *r)
{
  cout << "LinearTIR::convert (Relation *r)" << endl;
  LinearTIR* ltir = dynamic_cast<LinearTIR*>(r);
  return ltir;
}

// Default (private) constructor
LinearTIR::LinearTIR():
  Relation(), C(NULL), D(NULL), F(NULL), e(NULL), B(NULL), a(NULL)
{
  relationType = LINEARTIRELATION;
  isAllocatedIn.resize(6, false);
}

