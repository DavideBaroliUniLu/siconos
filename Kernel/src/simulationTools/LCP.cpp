#include "LCP.h"

// includes to be deleted thanks to factories
#include "Moreau.h"
#include "LagrangianLinearR.h"
#include "NewtonImpactLawNSL.h"
#include "LinearTIR.h"

using namespace std;

// Default constructor
LCP::LCP(): OneStepNSProblem(), nLcp(0), w(NULL), z(NULL), M(NULL), q(NULL),
  isWAllocatedIn(false), isZAllocatedIn(false), isMAllocatedIn(false), isQAllocatedIn(false)
{
  nspbType = LCP_OSNSP;
}

// xml constructor
LCP::LCP(OneStepNSProblemXML* osNsPbXml, Strategy* newStrat):
  OneStepNSProblem(osNsPbXml, newStrat), nLcp(0), w(NULL), z(NULL), M(NULL), q(NULL),
  isWAllocatedIn(false), isZAllocatedIn(false), isMAllocatedIn(false), isQAllocatedIn(false)
{
  nspbType = LCP_OSNSP;
  if (osNsPbXml != NULL)
  {
    LCPXML * xmllcp = (static_cast<LCPXML*>(osNsPbXml));

    // If both q and M are given in xml file, check if sizes are consistent
    if (xmllcp->hasQ() && xmllcp->hasM() && ((xmllcp->getM()).size(0) != (xmllcp->getQ()).size()))
      RuntimeException::selfThrow("LCP: xml constructor, inconsistent sizes between given q and M");

    // first nlcp is given by M matrix size in xml file
    if (xmllcp->hasM())
    {
      nLcp = (xmllcp->getM()).size(0);
      M = new SiconosMatrix(xmllcp->getM());
      isMAllocatedIn = true;
    }

    if (xmllcp->hasQ())
    {
      // get nLcp if necessary
      if (M == NULL)
        nLcp = (xmllcp->getQ()).size();

      q = new SimpleVector(xmllcp->getQ());
      isQAllocatedIn = true;
    }

  }
  else RuntimeException::selfThrow("LCP: xml constructor, xml file=NULL");
}

// Constructor from a set of data
LCP::LCP(Strategy* newStrat, const string& newSolver, const string& newSolvingMethod,
         const int& MaxIter, const double & Tolerance, const string & NormType,
         const double & SearchDirection):
  OneStepNSProblem(newStrat, newSolver, newSolvingMethod, MaxIter, Tolerance, NormType, SearchDirection),
  nLcp(0), w(NULL), z(NULL), M(NULL), q(NULL),
  isWAllocatedIn(false), isZAllocatedIn(false), isMAllocatedIn(false), isQAllocatedIn(false)
{
  nspbType = LCP_OSNSP;
}

// destructor
LCP::~LCP()
{
  if (isWAllocatedIn)
  {
    delete w;
    w = NULL;
  }
  if (isZAllocatedIn)
  {
    delete z;
    z = NULL;
  }
  if (isMAllocatedIn)
  {
    delete M;
    M = NULL;
  }
  if (isQAllocatedIn)
  {
    delete q;
    q = NULL;
  }
}

// Setters

void LCP::setW(const SimpleVector& newValue)
{
  if (nLcp != newValue.size())
    RuntimeException::selfThrow("LCP: setW, inconsistent size between given w size and problem size. You should set nLcp before");

  if (w == NULL)
  {
    w = new SimpleVector(nLcp);
    isWAllocatedIn = true;
  }
  else if (w->size() != nLcp)
    RuntimeException::selfThrow("LCP: setW, w size differs from nLcp");

  *w = newValue;
}

void LCP::setWPtr(SimpleVector* newPtr)
{
  if (nLcp != newPtr->size())
    RuntimeException::selfThrow("LCP: setWPtr, inconsistent size between given w size and problem size. You should set nLcp before");

  if (isWAllocatedIn) delete w;
  w = newPtr;
  isWAllocatedIn = false;
}


void LCP::setZ(const SimpleVector& newValue)
{
  if (nLcp != newValue.size())
    RuntimeException::selfThrow("LCP: setZ, inconsistent size between given z size and problem size. You should set nLcp before");

  if (z == NULL)
  {
    z = new SimpleVector(nLcp);
    isZAllocatedIn = true;
  }

  *z = newValue;
}

void LCP::setZPtr(SimpleVector* newPtr)
{
  if (nLcp != newPtr->size())
    RuntimeException::selfThrow("LCP: setZPtr, inconsistent size between given z size and problem size. You should set nLcp before");

  if (isZAllocatedIn) delete z;
  z = newPtr;
  isZAllocatedIn = false;
}

void LCP::setM(const SiconosMatrix& newValue)
{
  if (nLcp != newValue.size(0) || nLcp != newValue.size(1))
    RuntimeException::selfThrow("LCP: setM, inconsistent size between given M size and problem size. You should set nLcp before");

  if (M == NULL)
  {
    M = new SiconosMatrix(nLcp, nLcp);
    isMAllocatedIn = true;
  }

  *M = newValue;
}

void LCP::setMPtr(SiconosMatrix* newPtr)
{
  if (nLcp != newPtr->size(0) || nLcp != newPtr->size(1))
    RuntimeException::selfThrow("LCP: setMPtr, inconsistent size between given M size and problem size. You should set nLcp before");

  if (isMAllocatedIn) delete M;
  M = newPtr;
  isMAllocatedIn = false;
}


void LCP::setQ(const SimpleVector& newValue)
{
  if (nLcp != newValue.size())
    RuntimeException::selfThrow("LCP: setQ, inconsistent size between given q size and problem size. You should set nLcp before");

  if (q == NULL)
  {
    q = new SimpleVector(nLcp);
    isQAllocatedIn = true;
  }

  *q = newValue;
}

void LCP::setQPtr(SimpleVector* newPtr)
{
  if (nLcp != newPtr->size())
    RuntimeException::selfThrow("LCP: setQPtr, inconsistent size between given q size and problem size. You should set nLcp before");

  if (isQAllocatedIn) delete q;
  q = newPtr;
  isQAllocatedIn = false;
}

void LCP::initialize()
{

  // This function performs all steps that are not time dependant
  // General initialize for OneStepNSProblem
  OneStepNSProblem::initialize();

  // compute all block-matrices and save them in OneStepNSProblem
  computeAllBlocks();

  // get topology
  Topology * topology = strategy->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getTopologyPtr();

  // if all relative degrees are equal to 0 or 1
  if (topology->isTimeInvariant())
  {
    nLcp = topology->getEffectiveSizeOutput();
    assembleM();
  }
}

void LCP::computeAllBlocks()
{
  // --- get topology ---
  Topology * topology = strategy->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getTopologyPtr();

  // --- get linked-Interactions map ---
  map< Interaction*, vector<InteractionLink*> > linkedInteractionMap = topology->getLinkedInteractionMap();
  map< Interaction*, vector<InteractionLink*> >::const_iterator itMapInter;

  // --- get interactions list ---
  vector<Interaction*> listInteractions = strategy->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getInteractions();
  vector<Interaction*>::iterator iter;

  // --- get time step ---
  double h = strategy->getTimeDiscretisationPtr()->getH(); // time step

  string relationType, dsType;
  unsigned int globalDSSize, sizeInteraction, linkedInteractionSize;

  unsigned int sizeBlock, sizeLinkedBlock; // For each interaction, this size depends on relative degree(r), indexMin and the number of relations (j).
  // sizeBlock = (r - indexMin)*j  ou = j si r=0.
  vector<unsigned int> r; // relative degrees
  unsigned int rMax;
  vector<unsigned int> indexMin;
  // --- loop over all the interactions ---
  for (iter = listInteractions.begin(); iter != listInteractions.end(); ++iter)
  {
    // the current interaction and its size
    Interaction *currentInteraction = *iter;
    sizeInteraction = currentInteraction->getNInteraction();
    // relative degrees
    r = topology->getRelativeDegrees(currentInteraction);
    rMax = r[0]; // !!! we suppose the interaction is homogeneous !!!
    if (rMax == 0) rMax = 1 ; // warning: review r=0 case
    indexMin = topology->getIndexMin(currentInteraction);
    sizeBlock = (rMax - indexMin[0]) * sizeInteraction;

    // --- DIAGONAL BLOCKS MANAGEMENT ---

    // matrix that corresponds to diagonal block for the current interaction
    diagonalBlocksMap[ currentInteraction ] = new SiconosMatrix(sizeBlock, sizeBlock);
    SiconosMatrix * currentMatrixBlock = diagonalBlocksMap[ currentInteraction ];

    // get DS list of the current interaction
    vector<DynamicalSystem*> vDS = currentInteraction ->getDynamicalSystems();
    unsigned int nbDS = vDS.size(); // number of DS

    // sum of all DS sizes
    globalDSSize = 0;
    vector<DynamicalSystem*>::iterator itDS;
    for (itDS = vDS.begin(); itDS != vDS.end(); itDS++)
    {
      dsType = (*itDS)->getType();
      if (dsType == LNLDS || dsType == LTIDS)
        globalDSSize += (*itDS)->getN() / 2;
      else
        globalDSSize += (*itDS)->getN();
    }

    // Get Wi matrix of each DS concerned by the interaction and assemble global matrix W
    OneStepIntegrator * Osi;
    map<DynamicalSystem*, SiconosMatrix*> W;

    for (itDS = vDS.begin(); itDS != vDS.end(); itDS++)
    {
      Osi = strategy->getIntegratorOfDSPtr(*itDS); // get OneStepIntegrator of current dynamical system
      if (Osi->getType() == MOREAU_INTEGRATOR)
        W[*itDS] = (static_cast<Moreau*>(Osi))->getWPtr();  // get its W matrix
      else
        RuntimeException::selfThrow("LCP::computeAllBlocks not yet implemented for Integrator of type " + Osi->getType());
    }

    // get the relation of the current interaction and its type
    Relation * RCurrent = currentInteraction -> getRelationPtr();
    relationType = RCurrent->getType();

    if (relationType == LINEARTIRELATION)
      computeDiagonalBlocksLinearTIR(RCurrent, sizeInteraction, vDS, W, h, currentMatrixBlock);
    else if (relationType == LAGRANGIANLINEARRELATION)
      computeDiagonalBlocksLagrangianLinearR(RCurrent, sizeInteraction, vDS, W, h, currentMatrixBlock);
    else RuntimeException::selfThrow("LCP::computeAllBlocks not yet implemented for relation of type " + relationType);


    // --- EXTRA-DIAGONAL BLOCKS MANAGEMENT ---

    // check if there are linked interactions with current one, and if so get them.
    itMapInter = linkedInteractionMap.find(currentInteraction);
    if (itMapInter != linkedInteractionMap.end())
    {
      vector<InteractionLink*> ILV = linkedInteractionMap[currentInteraction];
      vector<InteractionLink*>::iterator itLink;

      //map<Interaction*, SiconosMatrix*> tmpMap;
      SiconosMatrix * coupledInteractionsBlock;

      // loop over LinkedInteractions
      for (itLink = ILV.begin(); itLink != ILV.end(); itLink++)
      {
        Interaction * linkedInteraction = (*itLink)->getLinkedInteractionPtr();
        linkedInteractionSize = linkedInteraction->getNInteraction();
        // relative degrees
        r = topology->getRelativeDegrees(linkedInteraction);
        rMax = r[0]; // !!! we suppose the interaction is homogeneous !!!
        if (rMax == 0) rMax = 1 ; // warning: review r=0 case
        indexMin = topology->getIndexMin(linkedInteraction);
        sizeLinkedBlock = (rMax - indexMin[0]) * linkedInteractionSize;

        (extraDiagonalBlocksMap[currentInteraction])[linkedInteraction] = new SiconosMatrix(sizeBlock, sizeLinkedBlock);
        coupledInteractionsBlock = extraDiagonalBlocksMap[currentInteraction][linkedInteraction];

        // get the list of common DS and their number
        vector<DynamicalSystem*> commonDS = (*itLink)->getCommonDS();
        nbDS = commonDS.size();

        // get the relation of the current interaction
        Relation * RLinked = linkedInteraction -> getRelationPtr();
        string RlinkedType = RLinked->getType();
        if (relationType == LINEARTIRELATION && RlinkedType == LINEARTIRELATION)
          computeExtraDiagonalBlocksLinearTIR(RCurrent, RLinked, sizeInteraction, linkedInteractionSize, commonDS, W, h, coupledInteractionsBlock);
        else if (relationType == LAGRANGIANLINEARRELATION && RlinkedType == LAGRANGIANLINEARRELATION)
          computeExtraDiagonalBlocksLagrangianLinearR(RCurrent, RLinked, sizeInteraction, linkedInteractionSize, commonDS, W, h, coupledInteractionsBlock);

        else RuntimeException::selfThrow("LCP::computeAllBlocks not yet implemented for relation of type " + relationType);
      } // end of loop over linked interactions

      // put tmpMap, ie blocks corresponding to interactions linked with current interaction into the map
      //extraDiagonalBlocksMap[currentInteraction] = tmpMap;
    }
  } // end of loop over interactions -> increment current interaction
}

void LCP::computeDiagonalBlocksLinearTIR(Relation * R, const unsigned int& sizeInteraction, vector<DynamicalSystem*> vDS,
    map<DynamicalSystem*, SiconosMatrix*> W, const double& h, SiconosMatrix* currentMatrixBlock)
{
  // convert to linearTIR
  LinearTIR *LTIR = static_cast<LinearTIR*>(R);
  // For 1 relation, we need:
  // - the corresponding row of D
  // - a block-row of C, that corresponds to a specific DS
  // - a block of B, that corresponds to a specific DS
  SiconosMatrix* D = LTIR->getDPtr();
  SiconosMatrix * C, *B;
  vector<DynamicalSystem*>::iterator itDS;
  unsigned int sizeDS;

  bool isHomogeneous = false; // \todo to be defined with relative degrees
  // isHomogeneous = true means that all relative degrees of the interaction are equal.

  if (!isHomogeneous)
  {
    // the resulting row-block
    SimpleVector * currentLine = new SimpleVector(sizeInteraction);
    // a row of D corresponding to the a relation of the current interaction
    SimpleVector * Drow = new SimpleVector(sizeInteraction);
    // a row of C corresponding to a specific DS (its size depends on the DS size)
    SimpleVector * Crow ;

    // compute currentLine = Drow + h sum(j) Crow,j Wj  Bj, with j the list of DS of the
    // current interaction.
    // loop over the relations of the current interaction
    for (unsigned int i = 0; i < sizeInteraction; i++)
    {
      // get row i of D
      *Drow = D->getRow(i);
      *currentLine = *Drow;
      // loop over the DS of the current interaction
      for (itDS = vDS.begin(); itDS != vDS.end(); itDS++)
      {
        sizeDS = (*itDS)->getN();
        // get blocks corresponding to the current DS
        C = new SiconosMatrix(sizeInteraction, sizeDS);
        B = new SiconosMatrix(sizeDS, sizeInteraction);
        LTIR->getCBlockDSPtr(*itDS, *C);
        LTIR->getBBlockDSPtr(*itDS, *B);
        // get row i of C
        Crow = new SimpleVector(sizeDS);
        *Crow = C->getRow(i);

        // compute currentLine
        *currentLine +=  h* *Crow * (*W[*itDS]* *B);
        delete C;
        delete B;
        delete Crow;
      }
      // save the result in currentMatrixBlock (and so in diagonalBlocksMap)
      currentMatrixBlock->setRow(i, *currentLine);
    }
    delete currentLine;
    delete Drow;
  }
  else
  {
    // the resulting block
    *currentMatrixBlock = *D;
    // loop over the DS of the current interaction
    for (itDS = vDS.begin(); itDS != vDS.end(); itDS++)
    {
      sizeDS = (*itDS)->getN();
      // get blocks corresponding to the current DS
      C = new SiconosMatrix(sizeInteraction, sizeDS);
      B = new SiconosMatrix(sizeDS, sizeInteraction);
      LTIR->getCBlockDSPtr(*itDS, *C);
      LTIR->getBBlockDSPtr(*itDS, *B);
      *currentMatrixBlock += h * *C * (*W[*itDS] * *B);
      delete C;
      delete B;
    }
  }
}

void LCP::computeExtraDiagonalBlocksLinearTIR(Relation * RCurrent, Relation* RLinked, const unsigned int& sizeInteraction,
    const unsigned int& linkedInteractionSize, vector<DynamicalSystem*> commonDS,
    map<DynamicalSystem*, SiconosMatrix*> W, const double& h, SiconosMatrix* coupledInteractionsBlock)
{
  // convert to linearTIR
  LinearTIR *LTIR1 = static_cast<LinearTIR*>(RCurrent);
  LinearTIR *LTIR2 = static_cast<LinearTIR*>(RLinked);
  SiconosMatrix *C, *B;
  coupledInteractionsBlock->zero();
  vector<DynamicalSystem*>::iterator itDS;
  unsigned int sizeDS;
  bool isHomogeneous = false; // \todo to be defined with relative degrees
  if (!isHomogeneous)
  {
    // the resulting row-block
    SimpleVector * currentLine = new SimpleVector(sizeInteraction);
    // a row of C corresponding to a specific DS (its size depends on the DS size)
    SimpleVector * Crow ;

    // compute currentLine =  h sum(j) Crow,j Wj  Bj, with j the list of common DS
    // C belongs to current Interaction and B to linked interaction
    // loop over the relations of the current interaction
    for (unsigned int i = 0; i < sizeInteraction; i++)
    {
      // loop over common DS
      for (itDS = commonDS.begin(); itDS != commonDS.end(); itDS++)
      {
        sizeDS = (*itDS)->getN();
        // get blocks corresponding to the current DS
        C = new SiconosMatrix(sizeInteraction, sizeDS);
        B = new SiconosMatrix(sizeDS, linkedInteractionSize);
        LTIR1->getCBlockDSPtr(*itDS, *C);
        LTIR2->getBBlockDSPtr(*itDS, *B);
        // get row i of C
        Crow = new SimpleVector(sizeDS);
        *Crow = C->getRow(i);
        // compute currentLine
        *currentLine +=   *Crow * (*W[*itDS]* *B);
        delete Crow;
        delete C;
        delete B;
      }
      // save the result in currentMatrixBlock (and so in diagonalBlocksMap)
      coupledInteractionsBlock->setRow(i, *currentLine);
    }
    delete currentLine;
  }
  else
  {
    // the resulting block
    // loop over the DS of the current interaction
    for (itDS = commonDS.begin(); itDS != commonDS.end(); itDS++)
    {
      sizeDS = (*itDS)->getN();
      // get blocks corresponding to the current DS
      C = new SiconosMatrix(sizeInteraction, sizeDS);
      B = new SiconosMatrix(sizeDS, linkedInteractionSize);
      LTIR1->getCBlockDSPtr(*itDS, *C);
      LTIR2->getBBlockDSPtr(*itDS, *B);
      *coupledInteractionsBlock +=  *C * (*W[*itDS] * *B);
      delete C;
      delete B;
    }
  }
}
void LCP::computeDiagonalBlocksLagrangianLinearR(Relation * R, const unsigned int& sizeInteraction, vector<DynamicalSystem*> vDS,
    map<DynamicalSystem*, SiconosMatrix*> W, const double& h, SiconosMatrix* currentMatrixBlock)
{
  // convert to lagrangianLinearR
  LagrangianLinearR *LLR = static_cast<LagrangianLinearR*>(R);

  // For 1 relation, we need:
  // - a block-row of H, that corresponds to a specific DS
  // - a block of tH, that corresponds to a specific DS
  SiconosMatrix *H;
  vector<DynamicalSystem*>::iterator itDS;
  unsigned int sizeDS;

  bool isHomogeneous = false; // \todo to be defined with relative degrees
  // isHomogeneous = true means that all relative degrees of the interaction are equal.
  if (!isHomogeneous)
  {
    // the resulting row-block
    SimpleVector * currentLine = new SimpleVector(sizeInteraction);
    // a row of H corresponding to a specific DS (its size depends on the DS size)
    SimpleVector * Hrow ;

    // compute currentLine = sum(j) Hrow,j Wj  tHj, with j the list of DS of the
    // current interaction.
    // loop over the relations of the current interaction
    for (unsigned int i = 0; i < sizeInteraction; i++)
    {
      currentLine->zero();
      // loop over the DS of the current interaction
      for (itDS = vDS.begin(); itDS != vDS.end(); itDS++)
      {
        sizeDS = (*itDS)->getN() / 2; // divided by 2 to get nDof
        // get blocks corresponding to the current DS
        H = new SiconosMatrix(sizeInteraction, sizeDS);
        LLR->getHBlockDS(*itDS, *H);
        // get row i of H
        Hrow = new SimpleVector(sizeDS);
        *Hrow = H->getRow(i);

        // compute currentLine
        *currentLine +=  *Hrow * (W[*itDS])->multTranspose(*H);
        delete H;
        delete Hrow;
      }
      // save the result in currentMatrixBlock (and so in diagonalBlocksMap)
      currentMatrixBlock->setRow(i, *currentLine);
    }
    delete currentLine;
  }
  else
  {
    // the resulting block
    currentMatrixBlock->zero();
    // loop over the DS of the current interaction
    for (itDS = vDS.begin(); itDS != vDS.end(); itDS++)
    {

      sizeDS = (*itDS)->getN() / 2; // divided by 2 to get nDof
      // get blocks corresponding to the current DS
      H = new SiconosMatrix(sizeInteraction, sizeDS);
      LLR->getHBlockDS(*itDS, *H);
      *currentMatrixBlock +=  *H * (W[*itDS])->multTranspose(*H);
      delete H;
    }
  }
}

void LCP::computeExtraDiagonalBlocksLagrangianLinearR(Relation * RCurrent, Relation* RLinked, const unsigned int& sizeInteraction,
    const unsigned int& linkedInteractionSize, vector<DynamicalSystem*> commonDS,
    map<DynamicalSystem*, SiconosMatrix*> W, const double& h, SiconosMatrix* coupledInteractionsBlock)
{
  // convert to LagrangianLinear relation
  LagrangianLinearR *LLR1 = static_cast<LagrangianLinearR*>(RCurrent);
  LagrangianLinearR *LLR2 = static_cast<LagrangianLinearR*>(RLinked);
  SiconosMatrix *Hcurrent, *Hlinked;
  coupledInteractionsBlock->zero();
  vector<DynamicalSystem*>::iterator itDS;
  unsigned int sizeDS;
  bool isHomogeneous = false; // \todo to be defined with relative degrees

  if (!isHomogeneous)
  {
    // the resulting row-block
    SimpleVector * currentLine = new SimpleVector(linkedInteractionSize);
    // a row of H corresponding to a specific DS (its size depends on the DS size)
    SimpleVector * Hrow ;

    // compute currentLine =  sum(j) Hrow,j Wj  tHj, with j the list of common DS
    // Hrow belongs to current Interaction and tH to linked interaction
    // loop over the relations of the current interaction
    for (unsigned int i = 0; i < sizeInteraction; i++)
    {
      // loop over common DS
      for (itDS = commonDS.begin(); itDS != commonDS.end(); itDS++)
      {
        sizeDS = (*itDS)->getN() / 2; // divided by 2 to get nDof
        // get blocks corresponding to the current DS
        Hcurrent = new SiconosMatrix(sizeInteraction, sizeDS);
        Hlinked = new SiconosMatrix(linkedInteractionSize, sizeDS);
        LLR1->getHBlockDS(*itDS, *Hcurrent);
        LLR2->getHBlockDS(*itDS, *Hlinked);
        // get row i of H
        Hrow = new SimpleVector(sizeDS);
        *Hrow = Hcurrent->getRow(i);
        // compute currentLine
        *currentLine +=  h* *Hrow * (W[*itDS])->multTranspose(*Hlinked);
        delete Hrow;
        delete Hcurrent;
        delete Hlinked;
      }
      // save the result in currentMatrixBlock (and so in diagonalBlocksMap)
      coupledInteractionsBlock->setRow(i, *currentLine);
    }
    delete currentLine;
  }
  else
  {
    // the resulting block
    // loop over the DS of the current interaction
    for (itDS = commonDS.begin(); itDS != commonDS.end(); itDS++)
    {
      sizeDS = (*itDS)->getN() / 2; // divided by 2 to get nDof
      // get blocks corresponding to the current DS
      Hcurrent = new SiconosMatrix(sizeInteraction, sizeDS);
      Hlinked = new SiconosMatrix(linkedInteractionSize, sizeDS);
      LLR1->getHBlockDS(*itDS, *Hcurrent);
      LLR2->getHBlockDS(*itDS, *Hlinked);
      *coupledInteractionsBlock += h* *Hcurrent * (W[*itDS])->multTranspose(*Hlinked);
      delete Hcurrent;
      delete Hlinked;
    }
  }
}

void LCP::preLCP(const double& time)
{
  IN("LCP::preLCP()\n");
  // compute M and q operators for LCP problem

  // get topology
  Topology * topology = strategy->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getTopologyPtr();
  // if relative degree is not equal to 0 or 1, check effective output
  if (! topology->isTimeInvariant())
  {
    computeEffectiveOutput();
    nLcp = topology->getEffectiveSizeOutput();
    if (nLcp != 0)
      assembleM();
  }

  if (nLcp != 0)
  {
    computeQ(time);
    // check z and w sizes and reset if necessary
    if (z == NULL)
    {
      z = new SimpleVector(nLcp);
      isZAllocatedIn = true;
    }
    else if (z->size() != nLcp)
    {
      // reset z if it has a wrong size
      if (isZAllocatedIn) delete z;
      z = new SimpleVector(nLcp);
      isZAllocatedIn = true;
    }

    if (w == NULL)
    {
      w = new SimpleVector(nLcp);
      isWAllocatedIn = true;
    }
    else if (w->size() != nLcp)
    {
      // reset w if it has a wrong size
      if (isWAllocatedIn) delete w;
      w = new SimpleVector(nLcp);
      isWAllocatedIn = true;
    }
  }
  OUT("LCP::preLCP()\n");
}

void LCP::assembleM() //
{

  if (M == NULL)
  {
    M = new SiconosMatrix(nLcp, nLcp);
    isMAllocatedIn = true;
  }
  else if (M->size(0) != nLcp || M->size(1) != nLcp)
  {
    // reset M matrix if it has a wrong size
    if (isMAllocatedIn) delete M;
    M = new SiconosMatrix(nLcp, nLcp);
    isMAllocatedIn = true;
  }
  M->zero();

  // get topology
  Topology * topology = strategy->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getTopologyPtr();

  map< Interaction* , SiconosMatrix*>::iterator itDiago;
  //map< Interaction*, unsigned int>  interactionPositionMap =  topology->getInteractionPositionMap();

  unsigned int pos, col;
  unsigned int size, sizeLinked;

  map< Interaction*, unsigned int>  interactionEffectivePositionMap =  topology->getInteractionEffectivePositionMap();
  vector<unsigned int>  effectiveIndexes, effectiveIndexesLinked;
  map< Interaction* , map<Interaction *, SiconosMatrix*> >::iterator itCoupled  ;
  map<Interaction *, SiconosMatrix*>::iterator it;
  SiconosMatrix *blockDiago , *coupledBlock;
  // --- loop over all the interactions ---
  for (itDiago = diagonalBlocksMap.begin(); itDiago != diagonalBlocksMap.end(); itDiago++)
  {
    // the current interaction
    Interaction * currentInteraction = itDiago->first;
    // and its corresponding diagonal block
    blockDiago = itDiago->second;
    pos = interactionEffectivePositionMap[ currentInteraction ];

    // get, if they exist, the linked interactions
    itCoupled = extraDiagonalBlocksMap.find(currentInteraction);

    if (topology->isTimeInvariant())
    {
      // diagonal blocks
      M->blockMatrixCopy(*blockDiago, pos, pos); // \todo avoid copy
      // extra-diagonal blocks
      if (itCoupled != extraDiagonalBlocksMap.end())
      {
        for (it = extraDiagonalBlocksMap[currentInteraction].begin(); it != extraDiagonalBlocksMap[currentInteraction].end(); it++)
        {
          coupledBlock = (*it).second;
          col = interactionEffectivePositionMap[(*it).first ];
          M->blockMatrixCopy(*coupledBlock, pos, col); // \todo avoid copy
        }
      }
    }
    else
    {
      // get list of effective relations for current interaction
      effectiveIndexes = topology->getEffectiveIndexes(currentInteraction);

      // get the "reduced" diagonal block for the current interaction
      size = effectiveIndexes.size();
      SiconosMatrix * reducedBlock = new SiconosMatrix(size, size);
      blockDiago->getBlock(effectiveIndexes, effectiveIndexes, *reducedBlock);

      // diagonal blocks
      M->blockMatrixCopy(*reducedBlock, pos, pos); // \todo avoid copy

      // extra-diagonal blocks
      SiconosMatrix * reducedCoupledBlock;

      // if currentInteraction is coupled with another interaction ...
      if (itCoupled != extraDiagonalBlocksMap.end())
      {
        // loop over linked interactions
        for (it = extraDiagonalBlocksMap[currentInteraction].begin(); it != extraDiagonalBlocksMap[currentInteraction].end(); it++)
        {
          // get list of effective relations for linked interaction
          Interaction * linkedInteraction = (*it).first;
          effectiveIndexesLinked = topology->getEffectiveIndexes(linkedInteraction);
          sizeLinked = effectiveIndexesLinked.size();
          // get the corresponding "reduced" block
          coupledBlock = (*it).second;
          reducedCoupledBlock = new SiconosMatrix(size, sizeLinked);
          coupledBlock->getBlock(effectiveIndexes, effectiveIndexesLinked, *reducedCoupledBlock);
          col = interactionEffectivePositionMap[ linkedInteraction ];
          M->blockMatrixCopy(*reducedCoupledBlock, pos, col); // \todo avoid copy
        }
      }
    }
  }// --- end of interactions loop ---
}

void LCP::computeQ(const double& time)
{
  IN("LCP::computeQ(void)\n");
  if (q == NULL)
  {
    q = new SimpleVector(nLcp);
    isQAllocatedIn = true;
  }
  else if (q->size() != nLcp)
  {
    // reset q if it has a wrong size
    if (isQAllocatedIn) delete q;
    q = new SimpleVector(nLcp);
    isQAllocatedIn = true;
  }

  q->zero();

  // --- get topology ---
  Topology * topology = strategy->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getTopologyPtr();
  // --- get interactions list ---
  vector<Interaction*> listInteractions = strategy->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getInteractions();
  vector<Interaction*>::iterator iter;
  // get Interaction position map
  map< Interaction* , SiconosMatrix*>::iterator itDiago;
  map< Interaction*, unsigned int>  interactionEffectivePositionMap =  topology->getInteractionEffectivePositionMap();
  unsigned int pos;
  Relation *R;
  NonSmoothLaw *nslaw;
  vector<unsigned int> index ;
  unsigned int effectiveSize ;
  Interaction *currentInteraction ;
  SimpleVector * yFree;
  // --- loop over all the interactions ---
  for (iter = listInteractions.begin(); iter != listInteractions.end(); ++iter)
  {
    // get current interaction, its relation and its nslaw
    currentInteraction = *iter;
    unsigned int sizeInteraction = currentInteraction->getNInteraction();
    R = currentInteraction->getRelationPtr();
    string relationType = R->getType();
    nslaw = currentInteraction->getNonSmoothLawPtr();
    // position of current yFree in global (including all interactions) y vector
    pos = interactionEffectivePositionMap[currentInteraction];
    index = topology->getEffectiveIndexes(currentInteraction);
    if (relationType == LINEARTIRELATION)
    {
      LinearTIR *LTIR = static_cast<LinearTIR*>(R);
      string nslawType = nslaw->getType() ;
      if (nslawType == COMPLEMENTARITYCONDITIONNSLAW)
      {
        LTIR->computeFreeOutput(); // free output is saved in y
        if (topology->isTimeInvariant())
        {
          yFree = new SimpleVector(currentInteraction->getY(0));   // copy, no pointer equality
          for (unsigned int i = 0; i < sizeInteraction; i++)
            (*q)(i + pos) = (*yFree)(i);
        }
        else
        {
          effectiveSize = index.size();
          yFree = new SimpleVector(effectiveSize); // we get only the "effective" relations
          (currentInteraction->getY(0)).getBlock(index, *yFree); // copy, no pointer equality
          for (unsigned int i = 0; i < effectiveSize; i++)
            (*q)(i + pos) = (*yFree)(i);
        }
      }
      else
        RuntimeException::selfThrow("LCP::computeQ not yet implemented for NSlaw of type " + nslaw->getType() + "and for relation of type " + R->getType());
    }
    else if (relationType == LAGRANGIANLINEARRELATION)
    {
      LagrangianLinearR *LLR = static_cast<LagrangianLinearR*>(R);
      if (nslaw->getType() == NEWTONIMPACTLAWNSLAW)
      {
        NewtonImpactLawNSL * newton = static_cast<NewtonImpactLawNSL*>(nslaw);
        double e = newton->getE();
        LLR->computeFreeOutput(time);
        if (topology->isTimeInvariant())
        {
          yFree = new SimpleVector(currentInteraction -> getY(1)); // copy, no pointer equality
          *yFree += e * currentInteraction -> getYOld(1);
          for (unsigned int i = 0; i < sizeInteraction; i++)
            (*q)(i + pos) = (*yFree)(i);
        }
        else
        {
          effectiveSize = index.size();
          yFree = new SimpleVector(effectiveSize); // we get only the "effective" relations
          (currentInteraction->getY(1)).getBlock(index, *yFree); // copy, no pointer equality
          SimpleVector * tmp =  new SimpleVector(effectiveSize);
          (currentInteraction -> getYOld(1)).getBlock(index, *tmp);
          *yFree += e * *tmp;
          delete tmp;
          for (unsigned int i = 0; i < effectiveSize; i++)
            (*q)(i + pos) = (*yFree)(i);
        }
      }
      else
        RuntimeException::selfThrow("LCP::computeQ not yet implemented for NSlaw of type " + nslaw->getType() + "and for relation of type " + R->getType());
    }
    else
      RuntimeException::selfThrow("LCP::computeQ not yet implemented for relation of type " + R->getType());

    delete yFree;
  }
  OUT("LCP::computeQ(void)\n");
}

void LCP::compute(const double& time)
{
  IN("LCP::compute(void)\n");

  // --- Prepare data for LCP computing ---
  preLCP(time);

  // --- Call Numerics solver ---
  if (nLcp != 0)
  {
    int info;
    int Nlcp = (int)nLcp;
    info = lcp_solver(M->getArray(), q->getArray(), &Nlcp, &solvingMethod, z->getArray(), w->getArray());
    if (info != 0)
      RuntimeException::selfThrow("LCP::compute, solve_lcp: convergence failed");
    // --- Recover the desired variables from LCP output ---
    postLCP(*w, *z);
  }

  OUT("LCP::compute(void)\n");
}

void LCP::postLCP(const SimpleVector& w, const SimpleVector &z)
{
  // --- get topology ---
  Topology * topology = strategy->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getTopologyPtr();

  // --- get interactions list ---
  vector<Interaction*> listInteractions = strategy->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getInteractions();
  vector<Interaction*>::iterator iter;
  // get Interaction position map
  map< Interaction* , SiconosMatrix*>::iterator itDiago;
  map< Interaction*, unsigned int>  interactionEffectivePositionMap =  topology->getInteractionEffectivePositionMap();

  vector<unsigned int> effectiveIndexes, indexMin;
  unsigned int effectivePosition;
  unsigned int effectiveSize, sizeInteraction ;
  unsigned int i; // index of derivation for Y
  unsigned int j; // number of relation in a specific interaction
  unsigned int k;
  unsigned int rMax; // maximum value for relative degrees
  SimpleVector * tmpY, *tmpLambda;
  vector< SimpleVector* >  Y, Lambda;
  vector<unsigned int> r; // relative degrees

  // --- loop over all the interactions ---
  for (iter = listInteractions.begin(); iter != listInteractions.end(); ++iter)
  {
    // the current interaction and its size
    Interaction *currentInteraction = *iter;
    sizeInteraction = currentInteraction->getNInteraction();

    // Y vector of the interactions
    Y = currentInteraction -> getY();
    // lambda vector
    Lambda = currentInteraction ->getLambda();

    // relative degrees
    r = topology->getRelativeDegrees(currentInteraction);
    rMax = r[0]; // we suppose the interaction is homogeneous
    if (rMax == 0) rMax = 1 ; // warning: review r=0 case

    // get the list of relations that are constrained and the position vector
    effectiveSize = topology->computeEffectiveSizeOutput(currentInteraction) ; // Improvement: save this value in Topology?
    effectiveIndexes = topology->getEffectiveIndexes(currentInteraction);
    indexMin = topology->getIndexMin(currentInteraction);
    // 'offset' due to indexMin
    for (j = 0; j < effectiveSize; j++)
      effectiveIndexes[j] += indexMin[effectiveIndexes[j]];
    effectivePosition = topology->getInteractionEffectivePosition(currentInteraction);

    // we consider that the interaction is homogeneous, ie all degrees are equals
    tmpY = new SimpleVector(effectiveSize); // warning: review r=0 case
    tmpLambda = new SimpleVector(effectiveSize);

    unsigned int pos;
    vector<unsigned int>::iterator itPos;
    // First we get in w results corresponding to the current interaction
    for (j = 0; j < effectiveSize ; j++)
    {
      (*tmpY)(j) =  w(j + effectivePosition);
      (*tmpLambda)(j) = z(j + effectivePosition);
    }

    // then we save these results in Y and Lambda, only for effective relations
    for (i = 0; i < rMax ; i++)
    {
      for (j = 0; j < sizeInteraction; j++)
      {
        pos = j * rMax + i;
        for (k = 0; k < effectiveSize; k++)
        {
          // itPos = find(effectiveIndexes.begin(),effectiveIndexes.end(),pos);
          // -> how can we get k/ effectiveIndex(k)=itPos ????
          //if (itPos!=effectiveIndexes.end())
          if (effectiveIndexes[k] == pos)
          {
            (*(Y[i]))(j) = (*tmpY)(k);
            (*(Lambda[i]))(j) = (*tmpLambda)(k);
          }
        }
      }
    }
  }

  delete tmpY;
  delete tmpLambda;
}


void LCP::display() const
{
  cout << "======= LCP display ======" << endl;
  cout << "| nLcp : " << nLcp << endl;
  cout << "| LCP Matrix M  : " << endl;
  if (M != NULL) M->display();
  else cout << "-> NULL" << endl;
  cout << "| LCP vector q : " << endl;
  if (q != NULL) q->display();
  else cout << "-> NULL" << endl;
  cout << "==========================" << endl;

}

void LCP::saveNSProblemToXML()
{
  IN("LCP::saveNSProblemToXML\n");
  OneStepNSProblem::saveNSProblemToXML();
  if (onestepnspbxml != NULL)
  {
    (static_cast<LCPXML*>(onestepnspbxml))->setM(*M);
    (static_cast<LCPXML*>(onestepnspbxml))->setQ(*q);
  }
  else RuntimeException::selfThrow("LCP::saveNSProblemToXML - OneStepNSProblemXML object not exists");
  OUT("LCP::saveNSProblemToXML\n");
}

void LCP::saveMToXML()
{
  IN("LCP::saveMToXML\n");
  if (onestepnspbxml != NULL)
  {
    (static_cast<LCPXML*>(onestepnspbxml))->setM(*M);
  }
  else RuntimeException::selfThrow("LCP::saveMToXML - OneStepNSProblemXML object not exists");
  OUT("LCP::saveMToXML\n");
}

void LCP::saveQToXML()
{
  IN("LCP::saveQToXML\n");
  if (onestepnspbxml != NULL)
  {
    (static_cast<LCPXML*>(onestepnspbxml))->setQ(*q);
  }
  else RuntimeException::selfThrow("LCP::saveQToXML - OneStepNSProblemXML object not exists");
  OUT("LCP::saveQToXML\n");
}

LCP* LCP::convert(OneStepNSProblem* osnsp)
{
  cout << "LCP::convert (OneStepNSProblem* osnsp)" << endl;
  LCP* lcp = dynamic_cast<LCP*>(osnsp);
  return lcp;
}


