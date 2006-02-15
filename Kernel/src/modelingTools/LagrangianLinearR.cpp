/* Siconos-Kernel version 1.1.1, Copyright INRIA 2005-2006.
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
#include "LagrangianLinearR.h"
//
#include "LagrangianDS.h"

using namespace std;

// Xml constructor
LagrangianLinearR::LagrangianLinearR(RelationXML* relxml, Interaction* inter):
  LagrangianR(relxml, inter), H(NULL), b(NULL), D(NULL),
  isHAllocatedIn(false), isBAllocatedIn(false), isDAllocatedIn(false)
{
  // if one between h and G is plugged, both must be.
  if (isHPlugged != isGPlugged[0])
    RuntimeException::selfThrow("LagrangianLinearR:: xml constructor, can not have plug-in for h and matrix for G=H in linear case.");

  relationType = LAGRANGIANLINEARRELATION;
  if (relxml != NULL)
  {
    if (!isHPlugged)
    {
      LagrangianLinearRXML* LLRxml = (static_cast<LagrangianLinearRXML*>(relationxml));
      unsigned int row = LLRxml->getH().size(0);

      if (LLRxml->hasD())
        setLagrangianRelationType("scleronomic+lambda");
      else
        setLagrangianRelationType("scleronomic");

      if (inter != NULL)
      {
        // get size of vector y
        unsigned int size = interaction->getNInteraction();
        if (row != size)
          RuntimeException::selfThrow("LagrangianLinearR:: xml constructor, inconsistent size with y vector for input vector or matrix");

        H = G[0];
        *H = LLRxml->getH();
        if (LagrangianRelationType == "scleronomic+lambda")
        {
          D = G[1];
          *D = LLRxml->getD();
        }
      }
      else
      {
        H = new SiconosMatrix(LLRxml->getH());
        isHAllocatedIn = true;
        G[0] = H;
        if (LagrangianRelationType == "scleronomic+lambda")
        {
          unsigned int rowD = LLRxml ->getD().size(0);
          unsigned int colD = LLRxml ->getD().size(1);
          if (rowD != colD || (interaction != NULL && rowD !=  interaction->getNInteraction()))
            RuntimeException::selfThrow("LagrangianLinearR:: xml constructor, inconsistent size for input matrix D");

          D = new SiconosMatrix(LLRxml->getD());
          isDAllocatedIn = true;
          G[1] = D;
        }
      }

      if (LLRxml->hasB())
      {
        unsigned int rowB = LLRxml->getB().size();
        if (row != rowB)
          RuntimeException::selfThrow("LagrangianLinearR:: xml constructor, inconsistent size between b and H");
        b = new SimpleVector(rowB);
        *b = LLRxml->getB();
        isBAllocatedIn = true;
      }
    }
    else
    {
      cout << "Warning: LagrangianLinearR xml constructor, original relations uses plug-in functions for h and G=H definition." << endl;
      H = G[0];
      if (LagrangianRelationType == "scleronomic+lambda")
        D = G[1];
    }
  }
  else RuntimeException::selfThrow("LagrangianLinearR::xml constructor xml file=NULL");
}

// Constructor from data: H, b and interaction (optional)
LagrangianLinearR::LagrangianLinearR(const SiconosMatrix& newH, const SimpleVector& newB, Interaction* inter):
  LagrangianR(inter), H(NULL), b(NULL), D(NULL), isHAllocatedIn(false), isBAllocatedIn(true), isDAllocatedIn(false)
{
  relationType = LAGRANGIANLINEARRELATION;
  unsigned int row = newH.size(0);
  unsigned int row2 = newB.size(0) ;
  if (row2 != row)
    RuntimeException::selfThrow("LagrangianLinearR:: constructor from data, inconsistent size between H and b");

  setLagrangianRelationType("scleronomic");

  if (inter != NULL)
  {
    // get size of vector y
    unsigned int size = interaction->getNInteraction();
    if (row != size)
      RuntimeException::selfThrow("LagrangianLinearR:: constructor from data, inconsistent size with y vector for input vector or matrix");

    H = G[0];
    *H = newH;
  }
  else
  {
    H = new SiconosMatrix(newH);
    isHAllocatedIn = true;
    G[0] = H;
  }
  isGPlugged.push_back(false);

  b = new SimpleVector(newB);
}

// Constructor from data: H and interaction (optional)
LagrangianLinearR::LagrangianLinearR(const SiconosMatrix& newH, Interaction* inter):
  LagrangianR(inter), H(NULL), b(NULL), D(NULL), isHAllocatedIn(false), isBAllocatedIn(true), isDAllocatedIn(false)
{
  relationType = LAGRANGIANLINEARRELATION;
  setLagrangianRelationType("scleronomic");
  unsigned int row = newH.size(0);
  if (inter != NULL)
  {
    // get size of vector y
    unsigned int size = interaction->getNInteraction();
    if (row != size)
      RuntimeException::selfThrow("LagrangianLinearR:: constructor from data, inconsistent size with y vector for input vector or matrix");

    H = G[0];
    *H = newH;
  }
  else
  {
    H = new SiconosMatrix(newH);
    isHAllocatedIn = true;
    G[0] = H;
  }
  isGPlugged.push_back(false);
}

// Constructor from data: H, b, D and interaction (optional)
LagrangianLinearR::LagrangianLinearR(const SiconosMatrix& newH, const SimpleVector& newB, const SiconosMatrix& newD, Interaction* inter):
  LagrangianR(inter), H(NULL), b(NULL), D(NULL), isHAllocatedIn(false), isBAllocatedIn(true), isDAllocatedIn(false)
{
  relationType = LAGRANGIANLINEARRELATION;
  unsigned int row = newH.size(0);
  unsigned int row2 = newB.size(0);
  unsigned int rowD = newD.size(0);
  unsigned int colD = newD.size(1);

  if (row2 != row)
    RuntimeException::selfThrow("LagrangianLinearR:: constructor from data, inconsistent size between H and b");

  if (rowD != colD || rowD != row)
    RuntimeException::selfThrow("LagrangianLinearR:: constructor from data, inconsistent size for D input matrix");

  setLagrangianRelationType("scleronomic+lambda");

  if (inter != NULL)
  {
    // get size of vector y
    unsigned int size = interaction->getNInteraction();
    if (row != size)
      RuntimeException::selfThrow("LagrangianLinearR:: constructor from data, inconsistent size with y vector for input vector or matrix");
    H = G[0];
    *H = newH;
    D = G[1];
    *D = newD;
  }
  else
  {
    H = new SiconosMatrix(newH);
    isHAllocatedIn = true;
    G[0] = H;
    D = new SiconosMatrix(newD);
    isDAllocatedIn = true;
    G[1] = D;
  }

  isGPlugged.push_back(false); // G[0]
  isGPlugged.push_back(false); // G[1]
  b = new SimpleVector(newB);
}

// copy constructor (inter is optional)
LagrangianLinearR::LagrangianLinearR(const Relation & newLLR, Interaction* inter):
  LagrangianR(newLLR, inter), H(NULL), b(NULL), D(NULL), isHAllocatedIn(true), isBAllocatedIn(true), isDAllocatedIn(false)
{
  if (relationType != LAGRANGIANLINEARRELATION)
    RuntimeException::selfThrow("LagrangianLinearR:: copy constructor, inconsistent relation types for copy");

  // if one between h and G is plugged, both must be.
  if (isHPlugged != isGPlugged[0])
    RuntimeException::selfThrow("LagrangianLinearR:: data constructor, can not have plug-in for h and matrix for G=H in linear case.");

  const LagrangianLinearR *  llr = static_cast<const LagrangianLinearR*>(&newLLR);

  // warning: G may have already been allocated in LagrangianR copy constructor!

  if (!isHPlugged)
  {
    if (G[0] == NULL)
    {
      H = new SiconosMatrix(llr->getH());
      isHAllocatedIn = true;
      G[0] == H;
    }
    else
    {
      H = G[0];
      isHAllocatedIn = false;
    }

    if (llr->getBPtr() != NULL)
    {
      b = new SimpleVector(llr->getB());
      isBAllocatedIn = true;
    }

    if (llr->getDPtr() != NULL)
    {
      if (G[1] == NULL)
      {
        D = new SiconosMatrix(llr->getD());
        isDAllocatedIn = true;
        G[1] == D;
      }
      else
      {
        D = G[1];
        isDAllocatedIn = false;
      }
    }
  }
  else
  {
    cout << "Warning: LagrangianLinearR copy constructor, original relations uses plug-in functions for h and G=H definition." << endl;
    H = G[0];
    if (LagrangianRelationType == "scleronomic+lambda")
      G[1] = D;

    isHAllocatedIn = false;
    isBAllocatedIn = false;
    isDAllocatedIn = false;
  }
}

LagrangianLinearR::~LagrangianLinearR()
{
  if (isHAllocatedIn) delete H;
  H = NULL;
  if (isBAllocatedIn) delete b;
  b = NULL;
  if (isDAllocatedIn) delete D;
  D = NULL;
}

// Setters

void LagrangianLinearR::setH(const SiconosMatrix& newValue)
{
  isHPlugged = false;
  isGPlugged[0] = false;
  unsigned int sizeY = newValue.size(0);
  unsigned int sizeQ = newValue.size(1);

  if (interaction != NULL)
  {
    unsigned int size = interaction->getNInteraction();
    if (size != sizeY)
      RuntimeException::selfThrow("LagrangianLinearR - setH: inconsistent dimensions with problem size for input matrix H");
  }

  if (H == NULL)
  {
    H = new SiconosMatrix(newValue);
    isHAllocatedIn = true;
  }
  else
  {
    if (sizeQ == H->size(1))
      *H = newValue;
    else
      RuntimeException::selfThrow("lagrangianLinearR - setH: inconsistent dimensions with problem size for input matrix H");
  }
}

void LagrangianLinearR::setHPtr(SiconosMatrix *newPtr)
{
  isHPlugged = false;
  isGPlugged[0] = false;
  if (isHAllocatedIn) delete H;
  if (interaction != NULL)
  {
    unsigned int sizeY = interaction->getNInteraction();
    if (newPtr->size(0) != sizeY)
      RuntimeException::selfThrow("LagrangianLinearR - setHPtr: inconsistent dimensions with problem size for input matrix H");
  }
  H = newPtr;
  isHAllocatedIn = false;
}

void LagrangianLinearR::setB(const SimpleVector& newValue)
{
  isHPlugged = false;
  isGPlugged[0] = false;
  unsigned int sizeY = newValue.size();
  if (interaction != NULL)
  {
    unsigned int size = interaction->getNInteraction();
    if (size != sizeY)
      RuntimeException::selfThrow("LagrangianLinearR - setB: inconsistent dimensions with problem size for input vector b");
  }

  if (b == NULL)
  {
    b = new SimpleVector(newValue);
    isBAllocatedIn = true;
  }
  else
  {
    if (sizeY == b->size())
      *b = newValue;
    else
      RuntimeException::selfThrow("LagrangianLinearR - setB: inconsistent dimensions with problem size for input vector b");
  }
}

void LagrangianLinearR::setBPtr(SimpleVector *newPtr)
{
  isHPlugged = false;
  isGPlugged[0] = false;
  if (isBAllocatedIn) delete b;
  b = newPtr;
  isBAllocatedIn = false;
}

void LagrangianLinearR::setD(const SiconosMatrix& newValue)
{
  unsigned int sizeY = newValue.size(0);
  unsigned int colD = newValue.size(1);
  if (sizeY != colD)
    RuntimeException::selfThrow("lagrangianLinearR - setD: inconsistent dimensions for input matrix D");

  if (interaction != NULL)
  {
    unsigned int size = interaction->getNInteraction();
    if (size != sizeY)
      RuntimeException::selfThrow("LagrangianLinearR - setD: inconsistent dimensions with problem size for input matrix D");
  }

  if (D == NULL)
  {
    D = new SiconosMatrix(newValue);
    isDAllocatedIn = true;
  }
  else
    *D = newValue;
}

void LagrangianLinearR::setDPtr(SiconosMatrix *newPtr)
{
  if (newPtr->size(0) != newPtr->size(1))
    RuntimeException::selfThrow("lagrangianLinearR - setDPtr: inconsistent dimensions for input matrix D");

  if (isDAllocatedIn) delete D;
  if (interaction != NULL)
  {
    unsigned int sizeY = interaction->getNInteraction();
    if (newPtr->size(0) != sizeY)
      RuntimeException::selfThrow("LagrangianLinearR - setDPtr: inconsistent dimensions with problem size for input matrix D");
  }
  D = newPtr;
  isDAllocatedIn = false;
}

void LagrangianLinearR::getHBlockDS(DynamicalSystem * ds, SiconosMatrix& Block) const
{
  unsigned int k = 0;
  vector<DynamicalSystem*> vDS = interaction ->getDynamicalSystems();
  vector<DynamicalSystem*>::iterator itDS;
  itDS = vDS.begin();

  // look for ds
  while (*itDS != ds && itDS != vDS.end())
  {
    k += (*itDS)->getN() / 2;
    itDS++;
  }
  // check dimension
  if ((*itDS)->getN() / 2 != Block.size(1))
    RuntimeException::selfThrow("LagrangianLinearR - getHBlockDS: inconsistent sizes between HBlock and DS");

  // get block
  unsigned int l = k + (*itDS)->getN() / 2 - 1;
  vector<unsigned int> index_list(4);
  index_list[0] = 0;
  index_list[1] = H->size(0) - 1;
  index_list[2] = k;
  index_list[3] = l;
  H->getBlock(index_list, Block);
}

void LagrangianLinearR::getHBlockDS(const int& DSNumber, SiconosMatrix& Block) const
{
  unsigned int k = 0;

  vector<DynamicalSystem*> vDS = interaction ->getDynamicalSystems();

  vector<DynamicalSystem*>::iterator itDS;
  itDS = vDS.begin();

  // look for DS number DSNumber ...
  while ((*itDS)->getNumber() != DSNumber && itDS != vDS.end())
  {
    k += (*itDS)->getN() / 2;
    itDS++;
  }

  // check dimension
  if ((*itDS)->getN() / 2 != Block.size(1))
    RuntimeException::selfThrow("LagrangianLinearR - getHBlockDS: inconsistent sizes between HBlock and DS");

  // get block
  unsigned int l = k + (*itDS)->getN() / 2 - 1;
  vector<unsigned int> index_list(4);
  index_list[0] = 0;
  index_list[1] = H->size(0) - 1;
  index_list[2] = k;
  index_list[3] = l;
  H->getBlock(index_list, Block);
}

void LagrangianLinearR::computeOutput(const double& time)
{
  IN("LagrangianLinearR::computeOutput\n");

  if (interaction == NULL)
    RuntimeException::selfThrow("LagrangianLinearR::computeOutput, no interaction linked with this relation");

  if (!isHPlugged)
  {
    // Get the DS concerned by the interaction of this relation
    vector<DynamicalSystem*> vDS = interaction->getDynamicalSystems();
    vector<LagrangianDS*> vLDS;

    unsigned int size = vDS.size(), i;
    CompositeVector *qTmp = new CompositeVector();
    CompositeVector *velocityTmp = new CompositeVector();
    for (i = 0; i < size; i++)
    {
      // check dynamical system type
      if (vDS[i]->getType() != LTIDS && vDS[i]->getType() != LNLDS)
        RuntimeException::selfThrow("LagrangianLinearR::computeOutput not yet implemented for dynamical system of type: " + vDS[i]->getType());

      // convert vDS systems into LagrangianDS and put them in vLDS
      vLDS.push_back(static_cast<LagrangianDS*>(vDS[i]));

      // Put q and velocity of each DS into a composite
      // Warning: use copy constructors (add function), no link between pointers
      qTmp->add(vLDS[i]->getQ());
      velocityTmp->add(vLDS[i]->getVelocity());
    }

    // get y and yDot of the interaction
    SimpleVector *y = interaction->getYPtr(0);
    SimpleVector *yDot = interaction->getYPtr(1);
    SimpleVector *lambda = interaction->getLambdaPtr(0);
    SimpleVector *lambdaDot = interaction->getLambdaPtr(1);
    // compute y and yDot

    *y = (*H * *qTmp) ;

    if (b != NULL)
      *y += *b;

    *yDot = (*H * *velocityTmp);

    if (D != NULL)
    {
      *y    += *D * *lambda ;
      *yDot += *D * *lambdaDot ;
    }

    // free memory
    delete qTmp;
    delete velocityTmp;
  }
  else
    LagrangianR::computeOutput(time);

  OUT("LagrangianLinearR::computeOutput\n");
}

void LagrangianLinearR::computeFreeOutput(const double& time)
{
  IN("LagrangianLinearR::computeFreeOutput\n");

  if (interaction == NULL)
    RuntimeException::selfThrow("LagrangianLinearR::computeFreeOutput, no interaction linked with this relation");

  if (!isHPlugged)
  {
    // Get the DS concerned by the interaction of this relation
    vector<DynamicalSystem*> vDS = interaction->getDynamicalSystems();
    vector<LagrangianDS*> vLDS;

    unsigned int size = vDS.size(), i;
    CompositeVector *qFreeTmp = new CompositeVector();
    CompositeVector *velocityFreeTmp = new CompositeVector();

    for (i = 0; i < size; i++)
    {
      // check dynamical system type
      if (vDS[i]->getType() != LTIDS && vDS[i]->getType() != LNLDS)
        RuntimeException::selfThrow("LagrangianLinearR::computeFreeOutput not yet implemented for dynamical system of type " + vDS[i]->getType());

      // convert vDS systems into LagrangianDS and put them in vLDS
      vLDS.push_back(static_cast<LagrangianDS*>(vDS[i]));

      // Put qFree and velocityFree of each DS into a composite
      // Warning: use copy constructors, no link between pointers
      qFreeTmp->add(vLDS[i]->getQFree());
      velocityFreeTmp->add(vLDS[i]->getVelocityFree());
    }

    // get y and yDot of the interaction
    SimpleVector *y = interaction->getYPtr(0);
    SimpleVector *yDot = interaction->getYPtr(1);
    // compute y and yDot (!! values for free state)

    *y = (*H * *qFreeTmp) ;

    if (b != NULL)
      *y += *b;

    *yDot = (*H * *velocityFreeTmp);

    // free memory
    delete qFreeTmp;
    delete velocityFreeTmp;
  }
  else
    LagrangianR::computeFreeOutput(time);

  OUT("LagrangianLinearR::computeFreeOutput\n");
}

void LagrangianLinearR::computeInput(const double& time)
{
  IN("LagrangianLinearR::computeInput\n");
  if (interaction == NULL)
    RuntimeException::selfThrow("LagrangianLinearR::computeInput, no interaction linked with this relation");

  if (!isHPlugged)
  {
    // Get the DS concerned by the interaction of this relation
    vector<DynamicalSystem*> vDS = interaction->getDynamicalSystems();
    vector<LagrangianDS*> vLDS;
    unsigned int numberDS = vDS.size(), i;
    vLDS.resize(numberDS);

    CompositeVector *p = new CompositeVector();
    string typeDS;

    for (i = 0; i < numberDS; i++)
    {
      // check dynamical system type
      typeDS = vDS[i] ->getType();
      if (typeDS != LTIDS && typeDS != LNLDS)
        RuntimeException::selfThrow("LagrangianLinearR::computeInput not yet implemented for this type of dynamical system " + typeDS);

      // convert vDS systems into LagrangianDS and put them in vLDS
      vLDS[i] = static_cast<LagrangianDS*>(vDS[i]);

      // Put p of each DS into a composite
      // Warning: use addPtr -> link between pointers
      p->addPtr(vLDS[i]->getPPtr());
    }

    // get lambda of the concerned interaction
    SimpleVector *lambda = interaction->getLambdaPtr(1);

    // compute p = Ht lambda
    *p += matTransVecMult(*H, *lambda);
    delete p;
  }
  else
    LagrangianR::computeInput(time);

  OUT("LagrangianLinearR::computeInput\n");
}

void LagrangianLinearR::saveRelationToXML() const
{
  IN("LagrangianLinearR::saveRelationToXML\n");
  if (relationxml != NULL)
  {
    (static_cast<LagrangianLinearRXML*>(relationxml))->setH(*H) ;
    (static_cast<LagrangianLinearRXML*>(relationxml))->setB(*b) ;
    (static_cast<LagrangianLinearRXML*>(relationxml))->setD(*D) ;
  }
  else RuntimeException::selfThrow("LagrangianLinearR::saveRelationToXML - object RelationXML does not exist");
  OUT("LagrangianLinearR::saveRelationToXML\n");
}

LagrangianLinearR* LagrangianLinearR::convert(Relation *r)
{
  cout << "LagrangianLinearR::convert (Relation *r)" << endl;
  LagrangianLinearR* llr = dynamic_cast<LagrangianLinearR*>(r);
  return llr;
}

// Default (private) constructor
LagrangianLinearR::LagrangianLinearR():
  LagrangianR(), H(NULL), b(NULL), D(NULL), isHAllocatedIn(false), isBAllocatedIn(false), isDAllocatedIn(false)
{
  relationType = LAGRANGIANLINEARRELATION;
}

void LagrangianLinearR::display() const
{
  IN("LagrangianLinearR::display\n");
  LagrangianR::display();
  cout << "===== Lagrangian Linear Relation display ===== " << endl;
  cout << " H: " << endl;
  if (H != NULL)
    H->display();
  else
    cout << " -> NULL " << endl;
  cout << " b: " << endl;
  if (b != NULL)
    b->display();
  else
    cout << " -> NULL " << endl;
  cout << " D: " << endl;
  if (D != NULL)
    D->display();
  else
    cout << " -> NULL " << endl;


  cout << "===================================== " << endl;
  OUT("LagrangianLinearR::display\n");
}
