#include "LCP.h"

// includes to be deleted thanks to factories
#include "Moreau.h"
#include "LagrangianLinearR.h"
#include "NewtonImpactLawNSL.h"

using namespace std;


// Default (private) constructor
LCP::LCP(): OneStepNSProblem(), nLcp(0), w(NULL), z(NULL), M(NULL), q(NULL),
  isWAllocatedIn(false), isZAllocatedIn(false), isMAllocatedIn(false), isQAllocatedIn(false)

{
  nspbType = LCP_OSNSP;
}

// xml constructor
LCP::LCP(OneStepNSProblemXML* osNsPbXml, Strategy* newStrat):
  OneStepNSProblem(osNsPbXml, newStrat), nLcp(0), w(NULL), z(NULL), M(NULL), q(NULL),
  isWAllocatedIn(true), isZAllocatedIn(true), isMAllocatedIn(true), isQAllocatedIn(true)
{
  nspbType = LCP_OSNSP;
  if (osNsPbXml != NULL)
  {
    // no getter-xml for nlcp ...
    nLcp = ((static_cast<LCPXML*>(onestepnspbxml))->getQ()).size();
    w = new SimpleVector(nLcp);
    z = new SimpleVector(nLcp);
    M = new SiconosMatrix(nLcp, nLcp);
    q = new SimpleVector(nLcp);
    *M = (static_cast<LCPXML*>(onestepnspbxml))->getM();
    *q = (static_cast<LCPXML*>(onestepnspbxml))->getQ();
  }
  else RuntimeException::selfThrow("LCP: xml constructor, xml file=NULL");
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

void LCP::formalize(const double& time)
{
  IN("LCP::formalize(void)\n");
  // compute M and q operators for LCP problem
  computeM();
  computeQ(time);

  // check if size w and z are allright
  if ((isWAllocatedIn) && (w->size() != q->size()))
  {
    delete w;
    if (isZAllocatedIn) delete z;
    w = new SimpleVector(nLcp);
    isWAllocatedIn = true;
    z = new SimpleVector(nLcp);
    isZAllocatedIn = true;
  }
  OUT("LCP::formalize(void)\n");
}


void LCP::compute()
{
  IN("LCP::compute(void)\n");
  int res, i, j;

  // Call Numerics solver
  if (nLcp == 0)
  {}
  else //if( nLcp == 1 )
    res = solve_lcp(M->getArray(), q->getArray(), &nLcp, &solvingMethod, z->getArray(), w->getArray());

  // Update the relation
  SiconosVector *yDot, *lambda;
  int activeInteraction = 0;

  yDot = interactionVector[0]->getYDotPtr();
  lambda = interactionVector[0]->getLambdaPtr();

  if (nLcp == 0) lambda->zero();
  else //if (nLcp == 1)
  {
    for (int i = 0; i < interactionVector.size(); i++)
    {
      lambda = interactionVector[i]->getLambdaPtr();
      lambda->zero();

      if (connectedInteractionMap.find(interactionVector[i]) != connectedInteractionMap.end())
      {
        yDot = interactionVector[i]->getYDotPtr();
        lambda = interactionVector[i]->getLambdaPtr();

        (*yDot)(0) = (*w)(activeInteraction);
        (*lambda)(0) = (*z)(activeInteraction);
        activeInteraction++;
      }
    }
  }
  OUT("LCP::compute(void)\n");
}

void LCP::computeM()
{
  IN("LCP::computeM(void)\n");
  int i, j, number;
  int orgDSRank, connectedDSRank;
  int currentActiveInteraction = 0;
  int interConnectedNumber = 0;
  vector<int> status;
  vector<SiconosMatrix*> v(2);
  vector<DynamicalSystem*> vDS;
  vector<Connection*> vCo;
  vector<OneStepIntegrator*> OsiV(2);
  vector<Moreau *> MoreauV(2);
  SiconosMatrix /*W1, *W2,*/ *H, *WW ;
  bool isWWAllocatedIn = false;
  SiconosMatrix orgH, connectedH, wTmp, Mtmp;
  Relation *R, *RConnected;
  LagrangianLinearR *LLR;

  // --- Count the number of active interactions ---
  nLcp = connectedInteractionMap.size();

  if (isMAllocatedIn) delete M;
  M = new SiconosMatrix(nLcp, nLcp);
  isMAllocatedIn = true;
  M->zero();

  // --- For each interaction in the Map (ie active interaction) ... ---
  map<Interaction* , vector <Connection*> >::iterator iter;
  for (iter = connectedInteractionMap.begin(); iter != connectedInteractionMap.end(); ++iter)
  {
    Interaction *CurrentInteraction = iter->first;
    vDS.clear();

    // --- Check if W matrix of Moreau's integrator is already inversed ---
    vDS = CurrentInteraction ->getDynamicalSystems();
    if (vDS.size() == 2)
    {
      for (i = 0; i < 2; i++)
      {
        number = vDS[i]->getNumber();
        OsiV[i] = strategy->getIntegratorOfDSPtr(number);
        if (OsiV[i]->getType() == MOREAU_INTEGRATOR)
        {
          MoreauV[i] = static_cast<Moreau*>(OsiV[i]);
          v[i] = MoreauV[i]->getWPtr();
          if (!v[i]->isInversed()) v[i]->PLUInverseInPlace();
        }
        else
          RuntimeException::selfThrow("LCP::computeM not yet implemented for Integrator of type " + OsiV[i]->getType());
      }
      int size = v[0]->size(0) + v[1]->size(0);
      WW = new SiconosMatrix(size, size);
      isWWAllocatedIn = true;
      *WW = BlockMatrixAssemble(v);
    }
    else
      RuntimeException::selfThrow("LCP::computeM not yet implemented for one DS in an Interaction ");

    // --- Get the relation parameters and compute M ---
    R = CurrentInteraction->getRelationPtr();
    if (R->getType() == LAGRANGIANLINEARRELATION)
    {
      //  compute H W Ht
      LLR = static_cast<LagrangianLinearR*>(R);
      H = LLR->getHPtr();
      Mtmp = *H * WW->multTranspose(*H);
      M->blockMatrixCopy(Mtmp, currentActiveInteraction, currentActiveInteraction);
    }
    else
      RuntimeException::selfThrow("LCP::computeM [level1] not yet implemented for relation of type " + R->getType());

    // --- Compute M for connected interactions ---

    interConnectedNumber = 0;
    if (iter ->second[0] != NULL)
    {
      // get from the map the connexion vector of the current interaction
      vCo = iter -> second ;
      for (int k = 0; k < vCo.size(); k++)
      {
        orgDSRank = vCo[k]->originInteractionDSRank;
        connectedDSRank = vCo[k]->connectedInteractionDSRank;

        // get W(Moreau) of common DS
        wTmp = *v[orgDSRank];

        // get H matrix of the common DS
        // /!\ we supose that all the DS have the same size !!!!
        if (R->getType() == LAGRANGIANLINEARRELATION)
        {
          LLR = static_cast<LagrangianLinearR*>(R);
          // /!\ copy of matrices !!!
          orgH = LLR->getHRelatingToDS(orgDSRank);
        }
        else
          RuntimeException::selfThrow("LCP::computeM [level2] not yet implemented for relation of type " + R->getType());

        // get H matrix of the connected DS
        RConnected = vCo[k]->connected->getRelationPtr();
        if (RConnected->getType() == LAGRANGIANLINEARRELATION)
        {
          LLR = static_cast<LagrangianLinearR*>(RConnected);
          // /!\ copy of matrices !!!
          connectedH = LLR->getHRelatingToDS(connectedDSRank);
        }
        else
          RuntimeException::selfThrow("LCP::computeM [level3] not yet implemented for relation of type " + R->getType());

        Mtmp = orgH * wTmp.multTranspose(connectedH);
        //int interConnectedNumber = vCo[k]->connected->getNumber();

        // \todo : to be verified !!
        if (interConnectedNumber == currentActiveInteraction)
          interConnectedNumber++;
        M->blockMatrixCopy(Mtmp, currentActiveInteraction, interConnectedNumber);
        interConnectedNumber++;
      }
    }
    // incrementation of the number of active interaction
    currentActiveInteraction++;
  }
  if (isWWAllocatedIn) delete WW;
  OUT("LCP::computeM(void)\n");
}

void LCP::computeQ(const double& time)
{
  IN("LCP::computeQ(void)\n");
  Relation *R;
  LagrangianLinearR *LLR;
  NonSmoothLaw *nslaw;
  NewtonImpactLawNSL * newton;

  SimpleVector qLCPtmp;
  int qPos = 0;
  if (isQAllocatedIn) delete q;
  q = new SimpleVector(nLcp);
  isQAllocatedIn = true;
  q->zero();

  // --- For each interaction in the Map (ie active interaction) ... ---
  map<Interaction* , vector <Connection*> >::iterator iter;
  for (iter = connectedInteractionMap.begin(); iter != connectedInteractionMap.end(); ++iter)
  {
    // get current interaction ...
    Interaction *CurrentInteraction = iter->first;
    // get the corresponding relation and nslaw ...
    R = CurrentInteraction->getRelationPtr();
    nslaw = CurrentInteraction->getNonSmoothLawPtr();
    if (R->getType() == LAGRANGIANLINEARRELATION)
    {
      LLR = static_cast<LagrangianLinearR*>(R);
      if (nslaw->getType() == NEWTONIMPACTLAWNSLAW)
      {
        newton = static_cast<NewtonImpactLawNSL*>(nslaw);
        double e = newton->getE();
        LLR->computeFreeOutput(time);
        qLCPtmp = CurrentInteraction -> getYDot();
        qLCPtmp += e * CurrentInteraction -> getYDotOld();
        // Assemble q
        //q = (-1)*qLCPtmp;

        /*
         * in a LCP problem the contribution of each interaction
         * to the vector 'q' is only a vector of dimension 1
         * so for the moment the assemblage of the q vector will be the copy
         * of 1 double value into 'q' for each active interaction
         */
        (*q)(qPos++) = -qLCPtmp(0);
      }
      else
        RuntimeException::selfThrow("LCP::computeQ not yet implemented for NSlaw of type " + nslaw->getType() + "and for relation of type " + R->getType());
    }
    else
      RuntimeException::selfThrow("LCP::computeQ not yet implemented for relation of type " + R->getType());
  }
  OUT("LCP::computeQ(void)\n");
}

void LCP::display() const
{
  cout << "------------------------------------------------------" << endl;
  cout << "____ data of the LCP " << endl;
  cout << "| nLcp : " << nLcp << endl;
  cout << "| LCP Matrix M  : " << endl;
  if (M != NULL) M->display();
  else cout << "-> NULL" << endl;
  cout << "| LCP vector q : " << endl;
  if (q != NULL) q->display();
  else cout << "-> NULL" << endl;
  cout << "____________________________" << endl;
  cout << "------------------------------------------------------" << endl;
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

