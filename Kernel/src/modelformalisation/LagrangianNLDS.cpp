//$Id: LagrangianNLDS.cpp,v 1.68 2005/02/28 16:22:33 jbarbier Exp $
#include "LagrangianNLDS.h"
//#include "LagrangianNLDSXML.h"

#include "check.h"


LagrangianNLDS::LagrangianNLDS()/*:DynamicalSystem()*/
{
  IN("LagrangianNLDS::LagrangianNLDS()\n");
  this->DSType = LNLDS;
  this->init();
  this->DSType = LNLDS;

  OUT("LagrangianNLDS::LagrangianNLDS()\n");
}

LagrangianNLDS::LagrangianNLDS(DSXML* dsxml) /*:DynamicalSystem(dsxml)*/
{
  IN("LagrangianNLDS::LagrangianNLDS( DSXML* dsxml)\n");

  this->init();
  this->DSType = LNLDS;
  this->dsxml = dsxml;

  OUT("LagrangianNLDS::LagrangianNLDS( DSXML* dsxml)\n");
}


LagrangianNLDS::~LagrangianNLDS()
{
  IN("LagrangianNLDS::~LagrangianNLDS()\n");

  //  if (this->x != NULL) delete x;
  //  if (this->x0 != NULL) delete x0;
  //  if (this->xDot != NULL) delete xDot;
  //  if (this->xFree != NULL) delete xFree;
  //
  OUT("LagrangianNLDS::~LagrangianNLDS()\n");
}


void LagrangianNLDS::initMemory(int steps)
{
  IN("LagrangianNLDS::initMemory\n");
  DynamicalSystem::initMemory(steps);

  //qMemory = SiconosMemory::SiconosMemory(steps);
  //velocityMemory = SiconosMemory::SiconosMemory(steps);
  qMemory = SiconosMemory::SiconosMemory(steps, this->qMemory.getSiconosMemoryXML());
  velocityMemory = SiconosMemory::SiconosMemory(steps, this->velocityMemory.getSiconosMemoryXML());

  OUT("LagrangianNLDS::initMemory\n");
}


void LagrangianNLDS::swapInMemory(void)
{
  IN("LagrangianNLDS::swapInMemory(void)\n");

  // This operation should be made only if necessary. See todo note.

  DynamicalSystem::swapInMemory();

  this->qMemory.swap(&q);
  this->velocityMemory.swap(&velocity);

  // initialization of the reaction force due to the non smooth law at the beginning of each time step
  this->p.zero();

  OUT("LagrangianNLDS::swapInMemory(void)\n");
}



SiconosMatrix* LagrangianNLDS::getMassPtr(void)
{
  return &(this->mass);
}




/*SiconosVector*/SimpleVector* LagrangianNLDS::getQPtr(void)
{
  return &this->q;
}

/*SiconosVector*/SimpleVector* LagrangianNLDS::getQ0Ptr(void)
{
  return &this->q0;
}


SiconosMemory* LagrangianNLDS::getQMemories(void)
{
  return &this->qMemory;
}



/*SiconosVector*/SimpleVector* LagrangianNLDS::getVelocityPtr(void)
{
  return &this->velocity;
}

/*SiconosVector*/SimpleVector* LagrangianNLDS::getVelocity0Ptr(void)
{
  return &this->velocity0;
}


SiconosMemory* LagrangianNLDS::getVelocityMemories(void)
{
  return &this->velocityMemory;
}


/*SiconosVector*/SimpleVector* LagrangianNLDS::getFIntPtr(void)
{
  return &this->fInt;
}

/*SiconosVector*/SimpleVector* LagrangianNLDS::getFExtPtr(void)
{
  return &this->fExt;
}


/*SiconosVector*/SimpleVector* LagrangianNLDS::getQNLInertiaPtr(void)
{
  return &this->QNLInertia;
}


SiconosMatrix* LagrangianNLDS::getJacobianQFIntPtr(void)
{
  return &this->jacobianQFInt;
}

SiconosMatrix* LagrangianNLDS::getJacobianVelocityFIntPtr(void)
{
  return &this->jacobianVelocityFInt;
}

SiconosMatrix* LagrangianNLDS::getJacobianQQNLInertiaPtr(void)
{
  return &this->jacobianQQNLInertia;
}

SiconosMatrix* LagrangianNLDS::getJacobianVelocityQNLInertiaPtr(void)
{
  return &this->jacobianVelocityQNLInertia;
}




//////////////////////////

void LagrangianNLDS::computeMass(double time)
{
  if (computeMassPtr == NULL)
    RuntimeException::selfThrow("computeMass() is not linked to a plugin function");

  int size = q.size();
  this->computeMassPtr(&size, &time, &q(0), &mass(0, 0));
}

void LagrangianNLDS::computeFInt(double time)
{
  if (computeFIntPtr == NULL)
    RuntimeException::selfThrow("computeFInt() is not linked to a plugin function");

  int size = this->q.size();
  this->computeFIntPtr(&size, &time, &this->q(0), &this->velocity(0), &this->fInt(0));
}

void LagrangianNLDS::computeFExt(double time)
{
  IN("LagrangianNLDS::computeFExt(double time)\n");
  if (computeFExtPtr == NULL)
    RuntimeException::selfThrow("computeFExt() is not linked to a plugin function");

  int size = q.size();

  this->computeFExtPtr(&size, &time, &q(0), &fExt(0));

  OUT("LagrangianNLDS::computeFExt(double time)\n");

}

void LagrangianNLDS::computeQNLInertia()
{
  if (computeQNLInertiaPtr == NULL)
    RuntimeException::selfThrow("computeQ() is not linked to a plugin function");

  int size = q.size();
  this->computeQNLInertiaPtr(&size, &q(0), &velocity(0), &QNLInertia(0));
}

void LagrangianNLDS::computeJacobianQFInt(double time)
{
  if (computeJacobianQFIntPtr == NULL)
    RuntimeException::selfThrow("computeJacobianQFInt() is not linked to a plugin function");

  int size = q.size();
  this->computeJacobianQFIntPtr(&size, &time, &q(0), &velocity(0), &jacobianQFInt(0, 0));
}

void LagrangianNLDS::computeJacobianVelocityFInt(double time)
{
  if (computeJacobianVelocityFIntPtr == NULL)
    RuntimeException::selfThrow("computeJacobianVelocityFInt() is not linked to a plugin function");

  // to do
  //this->computeJacobianVelocityFIntPtr();
}

void LagrangianNLDS::computeJacobianQQNLInertia(double time)
{
  if (computeJacobianQQNLInertiaPtr == NULL)
    RuntimeException::selfThrow("computeJacobianQQNLInertia() is not linked to a plugin function");

  //to do
  //this->computeJacobianQQPtr(time);
}

void LagrangianNLDS::computeJacobianVelocityQNLInertia(double time)
{
  if (computeJacobianVelocityQNLInertiaPtr == NULL)
    RuntimeException::selfThrow("computeJacobianVelocityQNLInertia() is not linked to a plugin function");

  //to do
  //this->computeJacobianVelocityQNLInertiaPtr(time);
}


//////


void LagrangianNLDS::setComputeMassFunction(string pluginPath, string functionName)
{
  IN("LagrangianNLDS::setComputeMassFunction\n");
  this->computeMassPtr = NULL;
  cShared.setFunction(&computeMassPtr, pluginPath, functionName);

  string plugin;
  plugin = pluginPath.substr(0, pluginPath.length() - 3);
  this->massFunctionName = plugin + ":" + functionName;

  OUT("LagrangianNLDS::setComputeMassFunction\n");

}

void LagrangianNLDS::setComputeFIntFunction(string pluginPath, string functionName)
{
  IN("LagrangianNLDS::setComputeFIntFunction\n");
  this->computeFIntPtr = NULL;
  cShared.setFunction(&computeFIntPtr, pluginPath, functionName);

  string plugin;
  plugin = pluginPath.substr(0, pluginPath.length() - 3);
  this->fIntFunctionName = plugin + ":" + functionName;

  OUT("LagrangianNLDS::setComputeFIntFunction\n");
}

void LagrangianNLDS::setComputeFExtFunction(string pluginPath, string functionName)
{
  IN("LagrangianNLDS::setComputeFExtFunction\n");
  this->computeFExtPtr = NULL;
  cShared.setFunction(&computeFExtPtr, pluginPath, functionName);

  string plugin;
  plugin = pluginPath.substr(0, pluginPath.length() - 3);
  this->fExtFunctionName = plugin + ":" + functionName;

  OUT("LagrangianNLDS::setComputeFExtFunction\n");
}

void LagrangianNLDS::setComputeQNLInertiaFunction(string pluginPath, string functionName)
{
  IN("LagrangianNLDS::setComputeQNLInertiaFunction\n");
  this->computeQNLInertiaPtr = NULL;
  cShared.setFunction(&computeQNLInertiaPtr, pluginPath, functionName);

  string plugin;
  plugin = pluginPath.substr(0, pluginPath.length() - 3);
  this->QNLInertiaFunctionName = plugin + ":" + functionName;

  OUT("LagrangianNLDS::setComputeQNLInertiaFunction\n");
}

void LagrangianNLDS::setComputeJacobianQFIntFunction(string pluginPath, string functionName)
{
  IN("LagrangianNLDS::setComputeJacobianQFIntFunction\n");
  this->computeJacobianQFIntPtr = NULL;
  cShared.setFunction(&computeJacobianQFIntPtr, pluginPath, functionName);

  string plugin;
  plugin = pluginPath.substr(0, pluginPath.length() - 3);
  this->jacobianQFIntFunctionName = plugin + ":" + functionName;

  OUT("LagrangianNLDS::setComputeJacobianQFIntFunction\n");
}

void LagrangianNLDS::setComputeJacobianVelocityFIntFunction(string pluginPath, string functionName)
{
  IN("LagrangianNLDS::setComputeMassFunction\n");
  this->computeJacobianVelocityFIntPtr = NULL;
  cShared.setFunction(&computeJacobianVelocityFIntPtr, pluginPath, functionName);

  string plugin;
  plugin = pluginPath.substr(0, pluginPath.length() - 3);
  this->jacobianVelocityFIntFunctionName = plugin + ":" + functionName;

  OUT("LagrangianNLDS::setComputeMassFunction\n");
}

void LagrangianNLDS::setComputeJacobianQQNLInertiaFunction(string pluginPath, string functionName)
{
  IN("LagrangianNLDS::setComputeJacobianQQNLInertiaFunction\n");
  this->computeJacobianQQNLInertiaPtr = NULL;
  cShared.setFunction(&computeJacobianQQNLInertiaPtr, pluginPath, functionName);

  string plugin;
  plugin = pluginPath.substr(0, pluginPath.length() - 3);
  this->jacobianQQNLInertiaFunctionName = plugin + ":" + functionName;

  OUT("LagrangianNLDS::setComputeJacobianQQNLInertiaFunction\n");
}

void LagrangianNLDS::setComputeJacobianVelocityQNLInertiaFunction(string pluginPath, string functionName)
{
  IN("LagrangianNLDS::setComputeJacobianVelocityQNLInertiaFunction\n");
  this->computeJacobianVelocityQNLInertiaPtr = NULL;
  cShared.setFunction(&computeJacobianVelocityQNLInertiaPtr, pluginPath, functionName);

  string plugin;
  plugin = pluginPath.substr(0, pluginPath.length() - 3);
  this->jacobianVelocityQNLInertiaFunctionName = plugin + ":" + functionName;

  OUT("LagrangianNLDS::setComputeJacobianVelocityQNLInertiaFunction\n");
}


//////////////////////////


void LagrangianNLDS::fillDSWithDSXML()
{
  string plugin;

  IN("LagrangianNLDS::fillDSWithDSXML\n");
  //DynamicalSystem::fillDSWithDSXML();
  if (this->dsxml != NULL)
  {
    LagrangianNLDSXML* lgptr = static_cast <LagrangianNLDSXML*>(this->dsxml);

    this->ndof = lgptr->getNdof();
    this->n = 2 * this->ndof;

    this->r = /*SiconosVector*/SimpleVector(this->n);

    this->q = /*SiconosVector*/SimpleVector(this->ndof);
    this->q0 =  /*SiconosVector*/SimpleVector(this->ndof);
    this->qFree =  /*SiconosVector*/SimpleVector(this->ndof);

    this->velocity =  /*SiconosVector*/SimpleVector(this->ndof);
    this->velocity0 =  /*SiconosVector*/SimpleVector(this->ndof);
    this->velocityFree =  /*SiconosVector*/SimpleVector(this->ndof);

    this->p =  /*SiconosVector*/SimpleVector(this->ndof);

    this->fExt =  /*SiconosVector*/SimpleVector(this->ndof);
    this->fInt =  /*SiconosVector*/SimpleVector(this->ndof);
    this->QNLInertia =  /*SiconosVector*/SimpleVector(this->ndof);


    // Mass
    if ((static_cast <LagrangianNLDSXML*>(this->dsxml))->isMPlugin())
    {
      plugin = lgptr->getMPlugin();
      this->setComputeMassFunction(this->cShared.getPluginName(plugin), this->cShared.getPluginFunctionName(plugin));
    }
    // \warning : VA:  It is a very good idea to take the constant Mass Matrix, but for the moment a constant
    //  Mass Matrix is only read by a LagrangianTIDS
    else this->mass = lgptr->getMMatrix();

    this->q0 = lgptr->getQ0();
    if (lgptr->hasQ()) this->q = lgptr->getQ();
    else
    {
      this->q = this->q0;
      cout << "Warning : q is not defined in the XML \n q is initialized with q0" << endl;
    }

    if (lgptr->hasQMemory()) this->qMemory = SiconosMemory::SiconosMemory(lgptr->getQMemoryXML());    //lgptr->getQMemory();
    else cout << "Warning : qMemory is not defined in the XML " << endl;

    this->velocity0 = lgptr->getVelocity0();
    if (lgptr->hasVelocity()) this->velocity = lgptr->getVelocity();
    else
    {
      this->velocity = this->velocity0;
      cout << "Warning : velocity is not defined in the XML \n velocity is initialized with velocity0" << endl;
    }

    if (lgptr->hasVelocityMemory()) this->velocityMemory = SiconosMemory::SiconosMemory(lgptr->getVelocityMemoryXML());    //lgptr->getVelocityMemory();
    else cout << "Warning : velocityMemory is not defined in the XML " << endl;

    static_cast<CompositeVector*>(this->x)->add(this->q);
    static_cast<CompositeVector*>(this->x)->add(this->velocity);

    static_cast<CompositeVector*>(this->x0)->add(this->q0);
    static_cast<CompositeVector*>(this->x0)->add(this->velocity0);

    // QNLInertia
    if (this->DSType == LNLDS)
    {
      // FInt
      if (lgptr->hasFint())
      {
        if (lgptr->isFintPlugin())
        {
          plugin = lgptr->getFintPlugin();
          this->setComputeFIntFunction(this->cShared.getPluginName(plugin), this->cShared.getPluginFunctionName(plugin));
        }
        else this->fInt = lgptr->getFintVector();
      }
      else
      {
        this->fInt = /*SiconosVector*/SimpleVector::SimpleVector();
        cout << "Warning : Fint is not defined in this LagrangianNLDS ( " << this->DSType << " )." << endl;
      }

      // JacobianQFInt
      if (lgptr->isJacobianQFintPlugin())
      {
        plugin = lgptr->getJacobianQFintPlugin();
        this->setComputeJacobianQFIntFunction(this->cShared.getPluginName(plugin), this->cShared.getPluginFunctionName(plugin));
      }
      else this->jacobianQFInt = lgptr->getJacobianQFintMatrix();

      // JacobianVelocityFInt
      if (lgptr->isJacobianVelocityFintPlugin())
      {
        plugin = lgptr->getJacobianVelocityFintPlugin();
        this->setComputeJacobianVelocityFIntFunction(this->cShared.getPluginName(plugin), this->cShared.getPluginFunctionName(plugin));
      }
      else this->jacobianVelocityFInt = lgptr->getJacobianVelocityFintMatrix();

      // JacobianQQNLInertia
      if (lgptr->isJacobianQQNLInertiaPlugin())
      {
        plugin = lgptr->getJacobianQQNLInertiaPlugin();
        this->setComputeJacobianQQNLInertiaFunction(this->cShared.getPluginName(plugin), this->cShared.getPluginFunctionName(plugin));
      }
      else this->jacobianQQNLInertia = lgptr->getJacobianQQNLInertiaMatrix();

      // JacobianVelocityQNLInertiaFunction
      if (lgptr->isJacobianVelocityQNLInertiaPlugin())
      {
        plugin = lgptr->getJacobianVelocityQNLInertiaPlugin();
        this->setComputeJacobianVelocityQNLInertiaFunction(this->cShared.getPluginName(plugin), this->cShared.getPluginFunctionName(plugin));
      }
      else this->jacobianVelocityQNLInertia = lgptr->getJacobianVelocityQNLInertiaMatrix();

      if (lgptr->isQNLInertiaPlugin())
      {
        plugin = lgptr->getQNLInertiaPlugin();
        this->setComputeQNLInertiaFunction(this->cShared.getPluginName(plugin), this->cShared.getPluginFunctionName(plugin));
      }
      else this->QNLInertia = lgptr->getQNLInertiaVector();
    }

    // FExt
    if (lgptr->isFextPlugin())
    {
      plugin = lgptr->getFextPlugin();
      this->setComputeFExtFunction(this->cShared.getPluginName(plugin), this->cShared.getPluginFunctionName(plugin));

    }
    else
    {
      this->fExt = lgptr->getFextVector();
    }

    /****************************/

    this->number = this->dsxml->getNumber();

    if (this->dsxml->hasId() == true) this->id = this->dsxml->getId();
    else cout << "Warning : Id is not defined in the XML " << endl;

    if (this->dsxml->hasX0() == true) *(this->x0) = this->dsxml->getX0();
    else cout << "Warning : x0 is not defined in the XML " << endl;

    if (this->dsxml->hasX() == true)
    {
      *(this->x) = this->dsxml->getX();
    }
    else cout << "Warning : x is not defined in the XML " << endl;

    if (this->dsxml->hasXDot() == true)(this->xDot) = this->dsxml->getXDot();
    else cout << "Warning : xDot is not defined in the XML " << endl;

    if (this->dsxml->hasXMemory() == true) this->xMemory = SiconosMemory::SiconosMemory(this->dsxml->getXMemoryXML()); //this->dsxml->getXMemory();
    else cout << "Warning : xMemory is not defined in the XML " << endl;

    if (this->dsxml->hasXDotMemory() == true) this->xDotMemory = SiconosMemory::SiconosMemory(this->dsxml->getXDotMemoryXML()); //this->dsxml->getXDotMemory();
    else cout << "Warning : xDotMemory is not defined in the XML " << endl;

    if (this->dsxml->hasStepsInMemory() == true) this->stepsInMemory = this->dsxml->getStepsInMemory();
    else cout << "Warning : stepsInMemory is not defined in the XML " << endl;
  }
  else RuntimeException::selfThrow("LagrangianNLDS::fillDSWithDSXML - object DSXML does not exist");
  OUT("LagrangianNLDS::fillDSWithDSXML\n");
}

void LagrangianNLDS::display() const
{
  IN("LagrangianNLDS::display\n");

  cout << "-----------------------------------------------------" << endl;
  cout << "____ data of the LagrangianNLDS " << endl;
  DynamicalSystem::display();
  cout << "| ndof : " << this->ndof << endl;
  cout << "| q " << endl;
  this->q.display();
  cout << "| q0 " << endl;
  this->q0.display();
  cout << "| qFree " << endl;
  this->qFree.display();
  cout << "| velocity " << endl;
  this->velocity.display();
  cout << "| velocity0 " << endl;
  this->velocity0.display();
  cout << "| velocityFree " << endl;
  this->velocityFree.display();
  cout << "| p " << endl;
  this->p.display();
  cout << "-----------------------------------------------------" << endl << endl;

  OUT("LagrangianNLDS::display\n");
}


void LagrangianNLDS::init()
{
  IN("LagrangianNLDS::init\n");

  this->r = SimpleVector::SimpleVector();
  this->BC = NULL;
  this->jacobianX = SiconosMatrix::SiconosMatrix();
  this->setVectorFieldFunction("BasicPlugin.so", "vectorField");
  this->setComputeJacobianXFunction("BasicPlugin.so", "computeJacobianX");

  velocityFree = /*SiconosVector()*/SimpleVector::SimpleVector();
  qFree = /*SiconosVector()*/SimpleVector::SimpleVector();

  this->x = new CompositeVector();
  this->x0 = new CompositeVector();
  this->xFree = new CompositeVector();

  this->setComputeMassFunction("BasicPlugin.so", "computeMass");
  this->setComputeFIntFunction("BasicPlugin.so", "computeFInt");
  this->setComputeFExtFunction("BasicPlugin.so", "computeFExt");
  /*
   * \WARNING bizarre que quelque chose propre au LTIDS soit ici
   */
  if (this->DSType == LNLDS)
  {
    this->setComputeQNLInertiaFunction("BasicPlugin.so", "computeQNLInertia");
    this->setComputeJacobianQFIntFunction("BasicPlugin.so", "computeJacobianQFInt");
    this->setComputeJacobianVelocityFIntFunction("BasicPlugin.so", "computeJacobianVelocityFInt");
    this->setComputeJacobianQQNLInertiaFunction("BasicPlugin.so", "computeJacobianQQNLInertia");
    this->setComputeJacobianVelocityQNLInertiaFunction("BasicPlugin.so", "computeJacobianVelocityQNLInertia");
  }

  OUT("LagrangianNLDS::init\n");
}

void LagrangianNLDS::saveDSToXML()
{
  IN("LagrangianNLDS::saveDSToXML\n");
  DynamicalSystem::saveDSToXML();

  if (this->dsxml != NULL)
  {
    LagrangianNLDSXML* lgptr = static_cast <LagrangianNLDSXML*>(this->dsxml);
    lgptr->setNdof(this->ndof);

    if (this->DSType == LTIDS)
    {
      lgptr->setMMatrix(&(this->mass));
    }
    else if (this->DSType == LNLDS)
    {
      lgptr->setMPlugin(this->massFunctionName);
    }

    lgptr->setQ(&(this->q));
    lgptr->setQ0(&(this->q0));
    lgptr->setQMemory(&(this->qMemory));

    lgptr->setVelocity(&(this->velocity));
    lgptr->setVelocity0(&(this->velocity0));
    lgptr->setVelocityMemory(&(this->velocityMemory));

    // FExt
    if (lgptr->hasFext())
    {
      if (!lgptr->isFextPlugin())
      {
        lgptr->setFextVector(&(this->fExt));
      }
    }
    else
    {
      lgptr->setFextPlugin(this->fExtFunctionName);
    }

    if (this->DSType != LTIDS)  // for a LagrangianTIDS, these plugin must not be saved
    {
      // FInt
      if (lgptr->hasFint())
      {
        if (!lgptr->isFintPlugin())
        {
          if (this->fInt.size() > 0)
            lgptr->setFintVector(&(this->fInt));
          else cout << "Warning : Fint can't be saved, the Fint vector is not defined." << endl;
        }
      }
      else
      {
        lgptr->setFintPlugin(this->fIntFunctionName);
      }

      // JacobianQFInt
      if (lgptr->hasJacobianQFint())
      {
        if (!lgptr->isJacobianQFintPlugin())
        {
          lgptr->setJacobianQFintMatrix(&(this->jacobianQFInt));
        }
      }
      else
      {
        lgptr->setJacobianQFintPlugin(this->jacobianQFIntFunctionName);
      }

      // JacobianVelocityFInt
      if (lgptr->hasJacobianVelocityFint())
      {
        if (!lgptr->isJacobianVelocityFintPlugin())
        {
          lgptr->setJacobianVelocityFintMatrix(&(this->jacobianVelocityFInt));
        }
      }
      else
      {
        lgptr->setJacobianVelocityFintPlugin(this->jacobianVelocityFIntFunctionName);
      }

      // JacobianQQNLInertia
      if (lgptr->hasJacobianQQNLInertia())
      {
        if (!lgptr->isJacobianQQNLInertiaPlugin())
        {
          lgptr->setJacobianQQNLInertiaMatrix(&(this->jacobianQQNLInertia));
        }
      }
      else
      {
        lgptr->setJacobianQQNLInertiaPlugin(this->jacobianQQNLInertiaFunctionName);
      }

      // JacobianVelocityQNLInertiaFunction
      if (lgptr->hasJacobianVelocityQNLInertia())
      {
        if (!lgptr->isJacobianVelocityQNLInertiaPlugin())
        {
          lgptr->setJacobianVelocityQNLInertiaMatrix(&(this->jacobianVelocityQNLInertia));
        }
      }
      else
      {
        lgptr->setJacobianVelocityQNLInertiaPlugin(this->jacobianVelocityQNLInertiaFunctionName);
      }

      // QNLInertia
      if (lgptr->hasQNLInertia())
      {
        if (!lgptr->isQNLInertiaPlugin())
        {
          lgptr->setQNLInertiaVector(&(this->QNLInertia));
        }
      }
      else
      {
        lgptr->setQNLInertiaPlugin(this->QNLInertiaFunctionName);
      }
    }
  }
  else RuntimeException::selfThrow("LagrangianNLDS::saveDSToXML - object DSXML does not exist");
  OUT("LagrangianNLDS::saveDSToXML\n");
}

void LagrangianNLDS::createDynamicalSystem(DSXML * dsXML, int number, int ndof,
    SiconosVector* q0, SiconosVector* velocity0, string mass,
    string fInt, string fExt,
    string jacobianQFInt, string jacobianVelocityFInt,
    string jacobianQQNLInertia, string jacobianVelocityQNLInertia,
    string QNLInertia)
//,NSDS * nsds)
{
  if (dsXML != NULL)
  {
    this->DSType = LNLDS;
    //DynamicalSystem::init();
    //this->init();
    this->dsxml = dsXML;
    this->fillDSWithDSXML();
    this->linkDSXML();
  }
  else
  {
    this->DSType = LNLDS;
    this->number = number;
    this->ndof = ndof;
    this->n = 2 * this->ndof;

    SiconosVectorSizeInit();

    this->q0 = *q0;
    this->q = *q0;
    this->velocity0 = *velocity0;
    this->velocity = *velocity0;


    CompositeVectorInit();

    this->setComputeMassFunction(this->cShared.getPluginName(mass), this->cShared.getPluginFunctionName(mass));

    this->setComputeFIntFunction(this->cShared.getPluginName(fInt), this->cShared.getPluginFunctionName(fInt));
    this->setComputeFExtFunction(this->cShared.getPluginName(fExt), this->cShared.getPluginFunctionName(fExt));

    this->setComputeJacobianQFIntFunction(this->cShared.getPluginName(jacobianQFInt), this->cShared.getPluginFunctionName(jacobianQFInt));
    this->setComputeJacobianVelocityFIntFunction(this->cShared.getPluginName(jacobianVelocityFInt), this->cShared.getPluginFunctionName(jacobianQFInt));
    this->setComputeJacobianQQNLInertiaFunction(this->cShared.getPluginName(jacobianQQNLInertia), this->cShared.getPluginFunctionName(jacobianQQNLInertia));
    this->setComputeJacobianVelocityQNLInertiaFunction(this->cShared.getPluginName(jacobianVelocityQNLInertia), this->cShared.getPluginFunctionName(jacobianVelocityQNLInertia));

    this->setComputeQNLInertiaFunction(this->cShared.getPluginName(QNLInertia), this->cShared.getPluginFunctionName(QNLInertia));
  }
}

void LagrangianNLDS::SiconosVectorSizeInit()
{
  /*
   * initilaisation of the SiconosVectors size
   */
  this->r = /*SiconosVector*/SimpleVector(this->n);

  this->q = /*SiconosVector*/SimpleVector(this->ndof);
  this->q0 =  /*SiconosVector*/SimpleVector(this->ndof);
  this->qFree =  /*SiconosVector*/SimpleVector(this->ndof);

  this->velocity =  /*SiconosVector*/SimpleVector(this->ndof);
  this->velocity0 =  /*SiconosVector*/SimpleVector(this->ndof);
  this->velocityFree =  /*SiconosVector*/SimpleVector(this->ndof);

  this->p =  /*SiconosVector*/SimpleVector(this->ndof);

  this->fExt =  /*SiconosVector*/SimpleVector(this->ndof);
  this->fInt =  /*SiconosVector*/SimpleVector(this->ndof);
  this->QNLInertia =  /*SiconosVector*/SimpleVector(this->ndof);
}

void LagrangianNLDS::CompositeVectorInit()
{
  /*
   * initialisation of the CompositeVectors
   */
  static_cast<CompositeVector*>(this->x)->add(this->q);
  static_cast<CompositeVector*>(this->x)->add(this->velocity);

  static_cast<CompositeVector*>(this->x0)->add(this->q0);
  static_cast<CompositeVector*>(this->x0)->add(this->velocity0);
}


LagrangianNLDS* LagrangianNLDS::convert(DynamicalSystem* ds)
{
  cout << "LagrangianNLDS::convert (DynamicalSystem* ds)" << endl;
  LagrangianNLDS* lnlds = dynamic_cast<LagrangianNLDS*>(ds);
  return lnlds;
}

//$Log: LagrangianNLDS.cpp,v $
//Revision 1.68  2005/02/28 16:22:33  jbarbier
//- rolling balls sample almost finished
//
//- in LCP, compute function now use all the interactions to make computations
//
//Revision 1.67  2005/02/11 17:35:55  charlety
//
//_ little "inspection of code"
//_ basic getters and setters passed inline
//_ getters functions passed const
//
//Revision 1.66  2005/02/01 11:08:41  charlety
//
//_ some displays of values during computations suppressed.
//
//Revision 1.65  2005/01/31 16:26:19  charlety
//
//_ Added a method named "convert" to classes which inherits from another. This is necessary for Python interface, in order to be able to use down-casting mechanism.
//
//Revision 1.64  2004/09/28 08:21:28  jbarbier
//
//- manual creation of the BouncingBall example successful
//
//Revision 1.63  2004/09/27 13:27:13  jbarbier
//
//- Siconos schema renamed : SiconosModelSchema-V1.0.xsd
//
//- new required tags of the model : title, author, description, date, xmlSchema.
//They replace previous attributes author, description and date of the Model.
//
//Revision 1.62  2004/09/22 10:54:43  jbarbier
//- light modification according to the attribute mass of the lagrangian dynamical
//systems. The lagrangianNLDS take always an function from a plugin to compute the
//mass, whereas the lagrangianTIDS needs only a matrix.
//
//- xml input files have been modified in consequence
//
//Revision 1.61  2004/09/21 11:49:09  jbarbier
//- correction in the XML save for a manual construction of the platform :
//    DS_Concerned of the Interaction
//    DS_Concerned of the Integrator
//
//- test updated for these changes
//
//Revision 1.60  2004/09/14 13:24:53  charlety
//
//_ changes in the interface of SiconosVector
//
//Revision 1.59  2004/09/10 11:26:13  charlety
//
//_ Integration of the new version of the SiconosVector in the platform. the class simpleVector is used mostly to replace old SiconosVector. When a vector can be composite or simple, like the state of a dynamical system, a pointer on SiconosVector is used, and the vector is initialized simple or composite when the system is initialized.
//
//_ All the tests which worked with the previous version of the vector are OK with the new version.
//
//_ Example SICONOS and bouncingBall are OK
//
//_ some comments have still to be adapted to NewSiconosVector .
//
//_ This version of NewSiconosVector could be called 0.9. some details have to be fixed, it will be done before the end of September.
//
//Revision 1.58  2004/08/23 14:30:01  jbarbier
//- All the dynamical systems can be created in a comand program and added to a
//NSDS. The save is OK, but the creation of the boundary conditions is not yet
//finished.
//
//Revision 1.57  2004/08/20 15:26:44  jbarbier
//- creation of a Model and save in the XML is ok
//- creation of a NSDS and save in the XML is ok
//- creation of a NonLinearSystemDS and save in the XML is OK
//
//Revision 1.56  2004/08/17 15:12:37  jbarbier
//- methods createDynamicalSystem, createBoundaryCondition, createInteraction,
//createRelation and createNSLaw completed with the required attributes
//
//Revision 1.55  2004/08/13 11:26:58  jbarbier
//- function createNSDS complete
//
//- function createDynamicalSystem and createLinearSystemDS complete
//
//- function  createLagrangianNLDS in progress
//
//Revision 1.54  2004/08/12 11:55:14  jbarbier
//- new methods createModel, createNSDS, createStrategy, ...
//they now allow to make the link with upper objects of the platform
//it will be used for the creation of the platform without XML input file
//
//- the createModel method is finished but the attributes of the other objects
//of the platform are missing for the conctruction
//
//Revision 1.53  2004/08/10 12:04:27  jbarbier
//- save of the plugin's name for fInt
//
//Revision 1.52  2004/08/09 15:00:52  jbarbier
//- changes in the cardinality of some attributes of the DynamicalSystem,
//OneStepIntegrator
//
//- modifications in classes Moreau, Lsodar, Adams for these new cardinalities
//
//- corrections in the test xml files
//
//Revision 1.51  2004/08/06 11:27:53  jbarbier
//- new tests with the XML and the optional attributes
//
//- tests on the save of the XML data
//
//Revision 1.50  2004/08/04 11:03:22  jbarbier
//- about the SiconosMemory : when a SiconosMemory has a maxSize greater than the
//number of steps in memory required by an integrator, the oldest SiconosVector
//are deleted
//
//- the way to initialize the SiconosMemory by the integrator has been updated to
//match with these changes
//
//Revision 1.49  2004/08/03 12:07:11  jbarbier
//- all test on th eModel are successfull
//
//- new tests on the Model with the opening of XML file
//
//- link TimeDiscretisation -> Strategy
//
//- attribute T of the Model is now optional
//
//Revision 1.48  2004/08/02 09:26:25  jbarbier
//- xml save for SiconosMemory corrected
//- temporary operation in Moreau::integrate because of the current version of
//SiconosVector
//
//Revision 1.47  2004/07/30 14:37:14  jbarbier
//- saving methods for DynamicalSystemXML and LagrangianNLDSXML
//
//Revision 1.46  2004/07/28 08:23:19  charlety
//
//_ Found a bug in a constructor of SiconosMemory
//_ try to get a SiconosMemory from XML successfull
//
//Revision 1.45  2004/07/27 09:32:43  jbarbier
//- functions createNSLaw for complementarityConditionNSL, newtonImpactLawNSL and RelayNSL
//
//Revision 1.44  2004/07/23 14:39:25  jbarbier
//- createModel, createNSDS, createDynamicalSystem, createBoundaryCondition OK
//it's the first step, it do the same thing that before, but the process is
//unified and it must simply add the traitment for the creation of the nodes in
//the DOM tree
//
//Revision 1.43  2004/07/22 15:16:22  jbarbier
//- xml_test.xml corrected with because of n which is equals to 2*ndof
//- manage the save of n (optional attribute) for Dynamical Systems
//
//Revision 1.42  2004/07/16 06:26:25  charlety
//
//_ Try of "intelligent" initialization of Timediscretisation
//
//Revision 1.41  2004/07/12 13:04:33  jbarbier
//- $id$, $log$, $date$, ... added in the XML management files
//- n id calculated with ndof for Lagrangian dynamical systems
//
//Revision 1.40  2004/07/12 11:22:53  charlety
//
//_ the state vector x of the dynamical system is now plugged to q and the velocity when this system is a Lagrangian one.
//
//_ A function to force a SiconosVector to be composite has been written. This is a temporary solution. We should use the operator = and the constructor by copy instead. This problem will be fixed later in the summer.
//
//Revision 1.39  2004/07/09 11:14:53  charlety
//
//_ Added a constructor by copy and an operator = in class SiconosMemory
//_ the getters on memory in DynamicalSystems return now some pointers
//
//Revision 1.38  2004/07/02 14:48:28  acary
//Added MACRO IN and OUT
//
//Revision 1.37  2004/07/02 14:40:20  jbarbier
//some IN and OUT added
//BoundaryConditon saveToXML ok
//problem after, during the call of the destructors
//
//Revision 1.36  2004/06/29 15:05:54  acary
//Change  in the Display method of the Dynamical system
//
//Revision 1.35  2004/06/29 08:49:57  acary
//Ajout des commentaires Doxygen et des tages CVS
//