#include "NonSmoothDynamicalSystem.h"

#include "LagrangianDS.h"
#include "LagrangianLinearTIDS.h"
#include "LinearSystemDS.h"
//#include "LagrangianDSXML.h"
//#include "LagrangianLinearTIDSXML.h"
//#include "LinearSystemDSXML.h"
#include "LinearEC.h"
#include "LinearTIEC.h"
#include "LagrangianEC.h"


#include "check.h"

NonSmoothDynamicalSystem::NonSmoothDynamicalSystem()
{
  this->nsdsxml = NULL;
  this->BVP = false;
}

NonSmoothDynamicalSystem::NonSmoothDynamicalSystem(bool bvp)
{
  this->nsdsxml = NULL;
  this->BVP = bvp;
}

NonSmoothDynamicalSystem::NonSmoothDynamicalSystem(NonSmoothDynamicalSystem* nsds)
{
  this->BVP = nsds->isBVP();
  this->DSVector = nsds->getDynamicalSystems();
  this->interactionVector = nsds->getInteractions();
  this->nsdsxml = nsds->getNSDSXML();
}

NonSmoothDynamicalSystem::NonSmoothDynamicalSystem(NSDSXML* nsdsxml)
{
  this->BVP = false;
  this->nsdsxml = nsdsxml;
}

NonSmoothDynamicalSystem::NonSmoothDynamicalSystem(string type)
{
  if (type == "BVP")
  {
    this->BVP = true;
  }
  else if (type == "IVP")
  {
    this->BVP = false;
  }
  else
  {
    // raise exception
    // or make IVP or BVP as default choice
  }
}

NonSmoothDynamicalSystem::~NonSmoothDynamicalSystem()
{
  IN("NonSmoothDynamicalSystem::~NonSmoothDynamicalSystem\n");
  int i;

  if (this->DSVector.size() > 0)
  {
    for (i = 0; i < this->DSVector.size(); i++)
    {
      delete this->DSVector[i];
    }
    this->DSVector.clear();
  }

  if (this->interactionVector.size() > 0)
  {
    for (i = 0; i < this->interactionVector.size(); i++)
    {
      delete this->interactionVector[i];
    }
    this->interactionVector.clear();
  }
  OUT("NonSmoothDynamicalSystem::~NonSmoothDynamicalSystem\n");
}


DynamicalSystem* NonSmoothDynamicalSystem::getDynamicalSystem(int nb)
{
  IN("DynamicalSystem* NonSmoothDynamicalSystem::getDynamicalSystem(int nb)\n");
  if (nb < this->DSVector.size())
  {
    OUT("DynamicalSystem* NonSmoothDynamicalSystem::getDynamicalSystem(int nb)\n");
    return this->DSVector[nb];
  }
  RuntimeException::selfThrow("NonSmoothDynamicalSystem - getDynamicalSystem : \'nb\' is out of range");
}

DynamicalSystem* NonSmoothDynamicalSystem::getDynamicalSystemOnNumber(int nb)
{
  for (unsigned int i = 0; i < this->DSVector.size(); i++)
  {
    if (DSVector[i]->getNumber() == nb)
    {
      return DSVector[i];
    }
  }
  RuntimeException::selfThrow("NonSmoothDynamicalSystem::getDynamicalSystemOnNumber : DSVector[i] == NULL");
}

bool NonSmoothDynamicalSystem::hasDynamicalSystemNumber(int nb)
{
  for (unsigned int i = 0; i < this->DSVector.size(); i++)
  {
    if (DSVector[i]->getNumber() == nb)
    {
      return true;
    }
  }
  return false;
}


Interaction* NonSmoothDynamicalSystem::getInteraction(int nb)
{
  if (nb < this->interactionVector.size())
  {
    return this->interactionVector[nb];
  }
  RuntimeException::selfThrow("NonSmoothDynamicalSystem - getInteraction : \'nb\' is out of range");
}

Interaction* NonSmoothDynamicalSystem::getInteractionOnNumber(int nb)
{
  for (unsigned int i = 0; i < this->interactionVector.size(); i++)
  {
    if (interactionVector[i] != NULL)
      if (interactionVector[i]->getNumber() == nb)
      {
        return interactionVector[i];
      }
  }
  RuntimeException::selfThrow("NonSmoothDynamicalSystem::getInteractionOnNumber : interactionVector[i] == NULL");
  return NULL;
}

bool NonSmoothDynamicalSystem::hasInteractionNumber(int nb)
{
  for (unsigned int i = 0; i < this->interactionVector.size(); i++)
  {
    if (interactionVector[i] != NULL)
      if (interactionVector[i]->getNumber() == nb)
      {
        return true;
      }
  }
  return false;
}


void NonSmoothDynamicalSystem::addDynamicalSystem(DynamicalSystem *ds)//, BoundaryCondition* bc)
{
  IN("NonSmoothDynamicalSystem::addDynamicalSystem\n");

  ds->setNSDS(this);
  this->DSVector.push_back(ds);

  //  DynamicalSystem* dsTmp;
  //
  //  if( ds->getType() == LNLDS )
  //  {
  //    dsTmp = new LagrangianDS();
  //    *dsTmp = *ds;
  //    this->DSVector.push_back( dsTmp );
  //  }
  //  else if( ds->getType() == LTIDS )
  //  {
  //    dsTmp = new LagrangianLinearTIDS();
  //    *dsTmp = *ds;
  //    this->DSVector.push_back( dsTmp );
  //    this->DSVector.push_back( ds );
  //  }
  //  else if( ds->getType() == LSDS )
  //  {
  //    dsTmp = new LinearSystemDS();
  //    *dsTmp = *ds;
  //    this->DSVector.push_back( dsTmp );
  //  }
  //  else if( ds->getType() == NLSDS )
  //  {
  //    dsTmp = new DynamicalSystem();
  //    *dsTmp = *ds;
  //    this->DSVector.push_back( dsTmp );
  //  }
  //  else RuntimeException::selfThrow("NonSmoothDynamicalSystem::addDynamicalSystem - bad kind of DynamicalSystem");

  OUT("NonSmoothDynamicalSystem::addDynamicalSystem\n");
}


void NonSmoothDynamicalSystem::addInteraction(Interaction *inter)
{
  Interaction* interTmp;
  interTmp = new Interaction();
  *interTmp = *inter;
  this->interactionVector.push_back(interTmp);
}


EqualityConstraint* NonSmoothDynamicalSystem::getEqualityConstraint(int i)
{
  if (i < this->ecVector.size())
  {
    return this->ecVector[i];
  }
  RuntimeException::selfThrow("NonSmoothDynamicalSystem - getEqualityConstraint : \'i\' is out of range");
}


void NonSmoothDynamicalSystem::addEqualityConstraint(EqualityConstraint* ec)
{
  EqualityConstraint* ecTmp;
  ecTmp = new EqualityConstraint();
  *ecTmp = *ec;
  this->ecVector.push_back(ecTmp);
}

////////////////////////

void NonSmoothDynamicalSystem::linkNSDSXML()
{
  IN("NonSmoothDynamicalSystem::linkNSDSXML\n");

  int i = 0;

  // get all the DSXML objects then create the DS for this DSXML and add this DS to the vector of DS of the NonSmoothDynamicalSystem
  vector<int> nbDStab = this->nsdsxml->getDSNumbers();
  for (i = 0; i < nbDStab.size(); i++)
  {
    DynamicalSystem *ds;
    // with the data of the XML object, we know the type of DynamicalSystem, so we can instanciate the right type of DynamicalSystem

    // DynamicalSystem - LagrangianDS
    if ((this->nsdsxml->getDSXML(nbDStab[i]))->getType() == LAGRANGIAN_NON_LINEARDS_TAG)
    {
      // creation of the LagrangianDS with this constructor and call of a method to fill
      ds = new LagrangianDS();
      this->DSVector.push_back(ds);
      (static_cast<LagrangianDS*>(ds))->createDynamicalSystem(this->nsdsxml->getDSXML(nbDStab[i]));
      ds->setNSDS(this);
    }
    else if ((this->nsdsxml->getDSXML(nbDStab[i]))->getType() == LAGRANGIAN_TIME_INVARIANTDS_TAG)
    {
      ds = new LagrangianLinearTIDS();
      this->DSVector.push_back(ds);
      (static_cast<LagrangianLinearTIDS*>(ds))->createDynamicalSystem(this->nsdsxml->getDSXML(nbDStab[i]));
      ds->setNSDS(this);
    }
    else if ((this->nsdsxml->getDSXML(nbDStab[i]))->getType() == LINEAR_SYSTEMDS_TAG)
    {
      ds = new LinearSystemDS();
      this->DSVector.push_back(ds);
      (static_cast<LinearSystemDS*>(ds))->createDynamicalSystem(this->nsdsxml->getDSXML(nbDStab[i]));
      ds->setNSDS(this);
    }
    else if ((this->nsdsxml->getDSXML(nbDStab[i]))->getType() == NON_LINEAR_SYSTEMDS_TAG)
    {
      ds = new DynamicalSystem();
      this->DSVector.push_back(ds);
      ds->createDynamicalSystem(this->nsdsxml->getDSXML(nbDStab[i]));
      ds->setNSDS(this);
    }
    else RuntimeException::selfThrow("NonSmoothDynamicalSystem::LinkNSDSXML - bad kind of DynamicalSystem");

    // for the other DS, we must have the xxxDSXML.h .cpp objects needed, and the objects in the DynamicalSystems inherited of the platform
    /*
     *  other "if" to create LagrangianLinearTIDS and LinearSystemDS
     *
     */
  }

  // get all the InteractionXML objects then create the Interaction for this InteractionXML and add this Interaction to the vector of Interaction of the NonSmoothDynamicalSystem
  vector<int> nbInteractionTab = this->nsdsxml->getInteractionNumbers();

  int ds1, ds2;
  for (i = 0; i < nbInteractionTab.size(); i++)
  {
    // the creation of the Interaction with this constructor call a method to fill
    Interaction * inter;
    inter = new Interaction();
    inter->createInteraction(this->nsdsxml->getInteractionXML(nbInteractionTab[i]));
    cout << "#@# createInteraction of the NonSmoothDynamicalSystem::linkNSDSXML ....... in progress" << endl;
    cout << " size of the vector of DS of the NonSmoothDynamicalSystem == " << this->DSVector.size() << endl;
    ds1 = (this->nsdsxml->getInteractionXML(nbInteractionTab[i])->getDSConcerned()[0])[0];
    ds2 = (this->nsdsxml->getInteractionXML(nbInteractionTab[i])->getDSConcerned()[0])[1];
    cout << " id of the ds1 == " << ds1 << endl;
    cout << " id of the ds2 == " << ds2 << endl;
    inter->setDynamicalSystems(this->getDynamicalSystemOnNumber(ds1), this->getDynamicalSystemOnNumber(ds2));
    this->interactionVector.push_back(inter);
  }
  if (nbInteractionTab.size() == 0) cout << "Warning : There are no Interaction defined." << endl;


  // get all the EqualityConstraintXML objects then create the EqualityConstraint for this EqualityConstraintXML
  vector<int> nbECtab = this->nsdsxml->getEqualityConstraintNumbers();
  for (i = 0; i < nbECtab.size(); i++)
  {
    EqualityConstraint *ec;
    if ((this->nsdsxml->getEqualityConstraintXML(nbECtab[i]))->getType() == LINEAR_EC_TAG)
    {
      ec = new LinearEC();
      this->ecVector.push_back(ec);
      (static_cast<LinearEC*>(ec))->createEqualityConstraint(this->nsdsxml->getEqualityConstraintXML(nbECtab[i]));
    }
    else if ((this->nsdsxml->getEqualityConstraintXML(nbECtab[i]))->getType() == NON_LINEAR_EC_TAG)
    {
      ec = new EqualityConstraint();
      this->ecVector.push_back(ec);
      (static_cast<EqualityConstraint*>(ec))->createEqualityConstraint(this->nsdsxml->getEqualityConstraintXML(nbECtab[i]));
    }
    else if ((this->nsdsxml->getEqualityConstraintXML(nbECtab[i]))->getType() == LINEAR_TIME_INVARIANT_EC_TAG)
    {
      ec = new LinearTIEC();
      this->ecVector.push_back(ec);
      (static_cast<LinearTIEC*>(ec))->createEqualityConstraint(this->nsdsxml->getEqualityConstraintXML(nbECtab[i]));
    }
    else if ((this->nsdsxml->getEqualityConstraintXML(nbECtab[i]))->getType() == LAGRANGIAN_EC_TAG)
    {
      ec = new LagrangianEC();
      this->ecVector.push_back(ec);
      (static_cast<LagrangianEC*>(ec))->createEqualityConstraint(this->nsdsxml->getEqualityConstraintXML(nbECtab[i]));
    }
    else RuntimeException::selfThrow("NonSmoothDynamicalSystem::LinkNSDSXML - bad kind of EqualityConstraint");
  }

  OUT("NonSmoothDynamicalSystem::linkNSDSXML\n");

}

void NonSmoothDynamicalSystem::fillNSDSWithNSDSXML()
{
  IN("NonSmoothDynamicalSystem::fillNSDSWithNSDSXML\n");

  if (this->nsdsxml != NULL)
  {
    //this->BVP = this->nsdsxml->isBVP();
  }
  else RuntimeException::selfThrow("NonSmoothDynamicalSystem::fillNSDSWithNSDSXML - The NSDSXML object doesn't exists");

  OUT("NonSmoothDynamicalSystem::fillNSDSWithNSDSXML\n");

}

void NonSmoothDynamicalSystem::saveNSDSToXML()
{
  IN("NonSmoothDynamicalSystem::saveNSDSToXML\n");
  int size, i;

  if (this->nsdsxml != NULL)
  {
    this->nsdsxml->setBVP(this->BVP);// no need to change the value of BVP, it mustn't change anyway

    size = this->DSVector.size();
    for (i = 0; i < size; i++)
    {
      if (this->DSVector[i]->getType() == LNLDS)
        (static_cast<LagrangianDS*>(this->DSVector[i]))->saveDSToXML();
      else if (this->DSVector[i]->getType() == LTIDS)
        (static_cast<LagrangianLinearTIDS*>(this->DSVector[i]))->saveDSToXML();
      else if (this->DSVector[i]->getType() == LSDS)
        (static_cast<LinearSystemDS*>(this->DSVector[i]))->saveDSToXML();
      else if (this->DSVector[i]->getType() == NLSDS)
        this->DSVector[i]->saveDSToXML();
      else RuntimeException::selfThrow("NonSmoothDynamicalSystem::saveToXML - bad kind of DynamicalSystem");
    }

    size = this->ecVector.size();
    for (i = 0; i < size; i++)
    {
      if (this->ecVector[i]->getType() == LINEAREC)
        (static_cast<LinearEC*>(this->ecVector[i]))->saveEqualityConstraintToXML();
      else if (this->ecVector[i]->getType() == LINEARTIEC)
        (static_cast<LinearTIEC*>(this->ecVector[i]))->saveEqualityConstraintToXML();
      else if (this->ecVector[i]->getType() == NLINEAREC)
        this->ecVector[i]->saveEqualityConstraintToXML();
      else if (this->ecVector[i]->getType() == LAGRANGIANEC)
        (static_cast<LagrangianEC*>(this->ecVector[i]))->saveEqualityConstraintToXML();
      else RuntimeException::selfThrow("NonSmoothDynamicalSystem::saveToXML - bad kind of EqualityConstraint");
    }

    size = this->interactionVector.size();
    for (i = 0; i < size; i++)
      this->interactionVector[i]->saveInteractionToXML();

  }
  else RuntimeException::selfThrow("NonSmoothDynamicalSystem::saveNSDSToXML - The NSDSXML object doesn't exists");

  OUT("NonSmoothDynamicalSystem::saveNSDSToXML\n");
}

void NonSmoothDynamicalSystem::createNonSmoothDynamicalSystem(NSDSXML * nsdsXML, bool bvp)//, Model * model)//, xmlNode * rootDOMTreeNode)
{
  if (nsdsXML != NULL)
  {
    /*
     * if bvp is true, it is not used, data of the DOM tree are more important
     */
    this->nsdsxml = nsdsXML;
    this->fillNSDSWithNSDSXML();
    cout << "NonSmoothDynamicalSystem filled" << endl;
    this->linkNSDSXML();
    cout << "NonSmoothDynamicalSystem linked" << endl;
  }
  else
  {
    this->BVP = bvp;
  }
}

void NonSmoothDynamicalSystem::display() const
{
  IN("NonSmoothDynamicalSystem::display\n");

  cout << "| this = " << this << endl;
  cout << "| BVP = " << this->BVP << endl;
  cout << "| &nsdsxml = " << this->nsdsxml << endl;
  cout << "|===========================" << endl;

  OUT("NonSmoothDynamicalSystem::display\n");
}

DynamicalSystem* NonSmoothDynamicalSystem::addNonLinearSystemDS(int number, int n,
    SiconosVector* x0, string vectorFieldPlugin)
{
  if (!this->hasDynamicalSystemNumber(number))
  {
    DynamicalSystem* dsTmp;

    dsTmp = new DynamicalSystem();
    dsTmp->createDynamicalSystem(NULL, number, n, x0, vectorFieldPlugin);
    this->DSVector.push_back(dsTmp);
    dsTmp->setNSDS(this);

    return dsTmp;
  }
  else
  {
    char n[50];
    sprintf(n, "%i", number);
    string num = n;
    string msg = "NonSmoothDynamicalSystem::addNonLinearSystemDS : ERROR - The DynamicalSystem number " + num + " is already declared!";
    RuntimeException::selfThrow(msg);
  }
}

DynamicalSystem* NonSmoothDynamicalSystem::addLinearSystemDS(int number, int n,
    SiconosVector* x0)
{
  if (!this->hasDynamicalSystemNumber(number))
  {
    DynamicalSystem* dsTmp;

    dsTmp = new LinearSystemDS();
    static_cast<LinearSystemDS*>(dsTmp)->createDynamicalSystem(NULL, number, n, x0);
    this->DSVector.push_back(dsTmp);
    dsTmp->setNSDS(this);

    return dsTmp;
  }
  else
  {
    char n[50];
    sprintf(n, "%i", number);
    string num = n;
    string msg = "NonSmoothDynamicalSystem::addLinearSystemDS : ERROR - The DynamicalSystem number " + num + " is already declared!";
    RuntimeException::selfThrow(msg);
  }
}

DynamicalSystem* NonSmoothDynamicalSystem::addLagrangianDS(int number, int ndof,
    SiconosVector* q0, SiconosVector* velocity0,
    string mass, string fInt, string fExt,
    string jacobianQFInt, string jacobianVelocityFInt,
    string jacobianQQNLInertia, string jacobianVelocityQNLInertia,
    string QNLInertia)
{
  if (!this->hasDynamicalSystemNumber(number))
  {
    DynamicalSystem* dsTmp;

    dsTmp = new LagrangianDS();
    static_cast<LagrangianDS*>(dsTmp)->createDynamicalSystem(NULL, number, ndof,
        q0, velocity0, mass, fInt, fExt,
        jacobianQFInt, jacobianVelocityFInt,
        jacobianQQNLInertia, jacobianVelocityQNLInertia,
        QNLInertia);
    this->DSVector.push_back(dsTmp);
    dsTmp->setNSDS(this);

    return dsTmp;
  }
  else
  {
    char n[50];
    sprintf(n, "%i", number);
    string num = n;
    string msg = "NonSmoothDynamicalSystem::addLagrangianDS : ERROR - The DynamicalSystem number " + num + " is already declared!";
    RuntimeException::selfThrow(msg);
  }
}

DynamicalSystem* NonSmoothDynamicalSystem::addLagrangianLinearTIDS(int number, int ndof,
    SiconosVector* q0, SiconosVector* velocity0,
    SiconosMatrix* mass, string fExt,
    SiconosMatrix* K, SiconosMatrix* C)
{
  if (!this->hasDynamicalSystemNumber(number))
  {
    DynamicalSystem* dsTmp;

    dsTmp = new LagrangianLinearTIDS();
    static_cast<LagrangianLinearTIDS*>(dsTmp)->createDynamicalSystem(NULL, number, ndof,
        q0, velocity0, mass, fExt, K, C);
    this->DSVector.push_back(dsTmp);
    dsTmp->setNSDS(this);

    return dsTmp;
  }
  else
  {
    char n[50];
    sprintf(n, "%i", number);
    string num = n;
    string msg = "NonSmoothDynamicalSystem::addLagrangianLinearTIDS : ERROR - The DynamicalSystem number " + num + " is already declared!";
    RuntimeException::selfThrow(msg);
  }
}

Interaction* NonSmoothDynamicalSystem::addInteraction(int number, int nInter, vector<int>* status, vector<DynamicalSystem*>* dsConcerned)
{
  if (!this->hasInteractionNumber(number))
  {
    Interaction* interTmp;
    interTmp = new Interaction();
    interTmp->createInteraction(NULL, number, nInter, status, dsConcerned);

    this->interactionVector.push_back(interTmp);
    return interTmp;
  }
  else
  {
    char n[50];
    sprintf(n, "%i", number);
    string num = n;
    string msg = "NonSmoothDynamicalSystem::addInteraction : ERROR - The Interaction number " + num + " is already declared!";
    RuntimeException::selfThrow(msg);
  }
}

