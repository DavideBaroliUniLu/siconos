#include "LCP.h"
#include "LCPXML.h"
#include "check.h"


#include "Interaction.h"
// hazardous dependency
#include "Moreau.h"
#include "LagrangianLinearR.h"
#include "NewtonImpactLawNSL.h"
#include <stdio.h>


LCP::LCP(): OneStepNSProblem()
{
  this->nspbType = LCP_OSNSP;
}

LCP::LCP(OneStepNSProblemXML* osnspbxml): OneStepNSProblem(osnspbxml)
{
  this->nspbType = LCP_OSNSP;
}

LCP::~LCP()
{}



SiconosMatrix* LCP::getMPtr(void)
{
  return &this->M;
}

SimpleVector* LCP::getQPtr(void)
{
  return &this->q;
}


void LCP::formalize(double time)
{
  IN("LCP::formalize(void)\n");
  // --- check and update status of the interactions ---
  // get time step
  double pasH = strategy->getTimeDiscretisationPtr()->getH();
  for (int i = 0; i < interactionVector.size(); i++)
  {
    interactionVector[i]->check(time, pasH);
  }
  updateConnectedInteractionMap();

  // compute M and q operators for LCP problem
  computeM();
  computeQ(time);

  w = SimpleVector::SimpleVector(nLcp);
  z = SimpleVector::SimpleVector(nLcp);

  OUT("LCP::formalize(void)\n");
}


void LCP::compute(void)
{
  IN("LCP::compute(void)\n");

  //cout<<" #%#% use of Numerics"<<endl;
  /***********************************
   * integration of Siconos/Numerics *
   ***********************************/
  // now we'll use Numerics !!
  int res, i, j;
  int nn = this->nLcp;//this->n;

  if (this->nLcp == 0)
  {}
  else //if( this->nLcp == 1 )
  {
    res = solve_lcp(/*vec*/this->M.getArray(), /*q*/(this->q.getArray()), &nn, &(this->solvingMethod), this->z.getArray(), this->w.getArray());
    /**
     * Pretty hazardous copy !!!!!
     */

    //    res = solve_lcp(/*vec*/this->M.getArray(), /*q*/(this->q.getArray()), &n, &(this->solvingMethod), this->z.getArray(), this->w.getArray());
    //
    //
    //    for (i=0; i<n; i++)
    //    {
    //      this->w(i) = z[i];
    //      this->z(i) = w[i];
    //    }
  }
  //  else
  //  RuntimeException::selfThrow("LCP::compute not yet implemented for size > 1 ");

  // Update the relation
  SiconosVector *yDot, *lambda;
  int activeInteraction = 0;

  yDot = this->interactionVector[0]->getYDotPtr();
  lambda = this->interactionVector[0]->getLambdaPtr();

  if (this->nLcp == 0)
  {
    lambda->zero();
  }
  else //if (this->nLcp == 1)
  {
    //    cout<<"### M then q :"<<endl;
    //    this->M.display();
    //    this->q.display();
    //    cout<<"### yDot then lambda :"<<endl;
    //    this->w.display();
    //    this->z.display();
    //    cout<<"### "<<endl;
    //    this->displayConnectedInteractionMap();
    //    cout<<"### ---"<<endl;

    //    cout<<"                    <<Press Enter>>"<<endl;
    //    getchar();

    for (int i = 0; i < this->interactionVector.size(); i++)
    {
      lambda = this->interactionVector[i]->getLambdaPtr();
      lambda->zero();

      if (this->connectedInteractionMap.find(this->interactionVector[i]) != this->connectedInteractionMap.end())
      {
        yDot = this->interactionVector[i]->getYDotPtr();
        lambda = this->interactionVector[i]->getLambdaPtr();

        (*yDot)(0) = this->w(activeInteraction);
        (*lambda)(0) = this->z(activeInteraction);

        activeInteraction++;
      }
    }
  }
  //cout<<"### quit LCP::compute"<<endl;
  OUT("LCP::compute(void)\n");
}


void LCP::computeM(void)
{
  IN("LCP::computeM(void)\n");
  int i, j, n;
  int orgDSRank, connectedDSRank;
  int currentActiveInteraction = 0;
  int interConnectedNumber = 0;
  vector<int> status;
  vector<SiconosMatrix*> v(2);
  vector<DynamicalSystem*> vDS;
  vector<Connection*> vCo;

  SiconosMatrix *W1, *W2, *H ;
  SiconosMatrix WW, orgH, connectedH, wTmp, Mtmp;
  Relation *R, *RConnected;
  OneStepIntegrator *I, *I2;
  Moreau *M1, *M2;
  LagrangianLinearR *LLR;

  this->nLcp = 0;

  // --- Count the number of active interactions ---
  int  activeInteraction = this->connectedInteractionMap.size();

  this->M = SiconosMatrix::SiconosMatrix(activeInteraction, activeInteraction);
  this->M.zero();

  // --- For each interaction in the Map (ie active interaction) ... ---
  map<Interaction* , vector <Connection*> >::iterator iter;
  for (iter = this->connectedInteractionMap.begin(); iter != this->connectedInteractionMap.end(); ++iter)
  {
    Interaction *CurrentInteraction ;
    CurrentInteraction = iter->first;
    WW = SiconosMatrix::SiconosMatrix(0, 0); // strange !!??
    vDS.clear();

    // --- Check if W matrix of Moreau's integrator is already inversed ---

    vDS = CurrentInteraction ->getDynamicalSystems();
    if (vDS.size() == 2)
    {
      n = vDS[0]->getNumber();
      I = this->strategy->getIntegratorOfDS(n);
      if (I->getType() == MOREAU_INTEGRATOR)
      {
        M1 = static_cast<Moreau*>(I);
        W1 = M1->getWPtr();
        if (!W1->isInversed()) W1->PLUInverseInPlace();
      }
      else
        RuntimeException::selfThrow("LCP::computeA not yet implemented for Integrator of type " + I->getType());

      n = vDS[1]->getNumber();
      I2 = this->strategy->getIntegratorOfDS(n);
      if (I2->getType() == MOREAU_INTEGRATOR)
      {
        M2 = static_cast<Moreau*>(I2);
        W2 = M2->getWPtr();
        if (!W2->isInversed()) W2->PLUInverseInPlace();
      }
      else
        RuntimeException::selfThrow("LCP::computeA not yet implemented for Integrator of type " + I->getType());

      // Assembly of W
      v[0] = W1;
      v[1] = W2;
      WW = BlockMatrixAssemble(v);
      // Why a new PLUinversion ? Already done in the previous loops.
      W1->PLUInverseInPlace();
      W2->PLUInverseInPlace();
    }
    else
      RuntimeException::selfThrow("LCP::computeA not yet implemented for one DS in an Interaction ");

    // --- Get the relation parameters and compute M ---

    R = CurrentInteraction->getRelation();
    if (R->getType() == LAGRANGIANLINEARRELATION)
    {
      //  compute H W Ht
      LLR = static_cast<LagrangianLinearR*>(R);
      H = LLR->getHPtr();
      Mtmp = *H * WW.multTranspose(*H);
      this->M.blockMatrixCopy(Mtmp, currentActiveInteraction, currentActiveInteraction);
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
        RConnected = vCo[k]->connected->getRelation();
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

        //cout<<"#_# "<<currentActiveInteraction<<" - "<<interConnectedNumber<<endl;
        this->M.blockMatrixCopy(Mtmp, currentActiveInteraction, interConnectedNumber);
        interConnectedNumber++;
      }
    }
    // incrementation of the number of active interaction for the M creation
    currentActiveInteraction++;
  }

  this->nLcp = activeInteraction;
  OUT("LCP::computeM(void)\n");
}

void LCP::computeQ(double time)
{
  IN("LCP::computeQ(void)\n");
  Relation *R;
  LagrangianLinearR *LLR;
  NonSmoothLaw *nslaw;
  NewtonImpactLawNSL * newton;

  double e;
  SimpleVector qLCPtmp;
  /*
   * \warning : initialisation of "q" removed! It seems that that BouncingBall sample
   * is still running good ...
   */
  int qSize = this->nLcp;
  int qPos = 0;
  this->q = SimpleVector::SimpleVector(qSize);
  this->q.zero();

  // --- For each interaction in the Map (ie active interaction) ... ---
  map<Interaction* , vector <Connection*> >::iterator iter;
  for (iter = this->connectedInteractionMap.begin(); iter != this->connectedInteractionMap.end(); ++iter)
  {
    Interaction *CurrentInteraction ;
    CurrentInteraction = iter->first;
    R = CurrentInteraction->getRelation();
    nslaw = CurrentInteraction->getNonSmoothLaw();
    if (R->getType() == LAGRANGIANLINEARRELATION)
    {
      LLR = static_cast<LagrangianLinearR*>(R);
      if (nslaw->getType() == NEWTONIMPACTLAWNSLAW)
      {
        newton = static_cast<NewtonImpactLawNSL*>(nslaw);
        e = newton->getE();
        LLR->computeFreeOutput(time);
        qLCPtmp = CurrentInteraction -> getYDot();
        qLCPtmp += e * CurrentInteraction -> getYDotOld();
        // Assemble q
        //this->q = (-1)*qLCPtmp;

        /*
         * in a LCP problem the contribution of each interaction
         * to the vector 'q' is only a vector of dimension 1
         * so for the moment the assemblage of the q vector will be the copy
         * of 1 double value into 'q' for each active interaction
         */
        this->q(qPos++) = -qLCPtmp(0);
      }
      else
        RuntimeException::selfThrow("LCP::computeQ not yet implemented for NSlaw of type " + nslaw->getType() + "and for relation of type " + R->getType());
    }
    else
      RuntimeException::selfThrow("LCP::computeQ not yet implemented for relation of type " + R->getType());
  }
  OUT("LCP::computeQ(void)\n");
}

void LCP::fillNSProblemWithNSProblemXML()
{
  OUT("LCP::fillNSProblemWithNSProblemXML\n");
  OneStepNSProblem::fillNSProblemWithNSProblemXML();
  if (this->onestepnspbxml != NULL)
  {
    this->M = (static_cast<LCPXML*>(this->onestepnspbxml))->getM();
    this->q = (static_cast<LCPXML*>(this->onestepnspbxml))->getQ();
  }
  else RuntimeException::selfThrow("LCP::fillNSProblemWithNSProblemXML - OneStepNSProblemXML object not exists");
}

void LCP::display() const
{
  cout << "------------------------------------------------------" << endl;
  cout << "____ data of the LCP " << endl;
  cout << "| nLcp : " << this->nLcp << endl;
  cout << "| LCP Matrix M  : " << endl;
  M.display();
  cout << "| LCP vector q : " << endl;
  q.display();
  cout << "____________________________" << endl;
  cout << "------------------------------------------------------" << endl;
}

void LCP::saveNSProblemToXML()
{
  IN("LCP::saveNSProblemToXML\n");
  OneStepNSProblem::saveNSProblemToXML();
  if (this->onestepnspbxml != NULL)
  {
    (static_cast<LCPXML*>(this->onestepnspbxml))->setM(&(this->M));
    (static_cast<LCPXML*>(this->onestepnspbxml))->setQ(&(this->q));
  }
  else RuntimeException::selfThrow("LCP::saveNSProblemToXML - OneStepNSProblemXML object not exists");
  OUT("LCP::saveNSProblemToXML\n");
}

void LCP::saveMToXML()
{
  IN("LCP::saveMToXML\n");
  if (this->onestepnspbxml != NULL)
  {
    (static_cast<LCPXML*>(this->onestepnspbxml))->setM(&(this->M));
  }
  else RuntimeException::selfThrow("LCP::saveMToXML - OneStepNSProblemXML object not exists");
  OUT("LCP::saveMToXML\n");
}

void LCP::saveQToXML()
{
  IN("LCP::saveQToXML\n");
  if (this->onestepnspbxml != NULL)
  {
    (static_cast<LCPXML*>(this->onestepnspbxml))->setQ(&(this->q));
  }
  else RuntimeException::selfThrow("LCP::saveQToXML - OneStepNSProblemXML object not exists");
  OUT("LCP::saveQToXML\n");
}

void LCP::createOneStepNSProblem(OneStepNSProblemXML * osnspbXML, Strategy * strategy)
{
  if (osnspbXML != NULL)
  {
    this->onestepnspbxml = osnspbXML;
    this->fillNSProblemWithNSProblemXML();
  }
  else
  {
    this->strategy = strategy;
    this->nspbType = LCP_OSNSP;
    this->fillInteractionVector();
  }
  this->init();
}


LCP* LCP::convert(OneStepNSProblem* osnsp)
{
  cout << "LCP::convert (OneStepNSProblem* osnsp)" << endl;
  LCP* lcp = dynamic_cast<LCP*>(osnsp);
  return lcp;
}
