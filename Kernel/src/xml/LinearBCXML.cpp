#include "LinearBCXML.h"
using namespace std;

LinearBCXML::LinearBCXML() : BoundaryConditionXML()
{
  this->omegaNode = NULL;
  this->omega0Node = NULL;
  this->omegaTNode = NULL;
}


LinearBCXML::LinearBCXML(xmlNode * linearBCNode) : BoundaryConditionXML(linearBCNode)
{
  this->loadLinearBCProperties();
}

LinearBCXML::~LinearBCXML()
{}

void LinearBCXML::loadLinearBCProperties()
{
  xmlNode *node;

  if ((node = SiconosDOMTreeTools::findNodeChild(this->rootBCNode, LINEARBC_OMEGA)) != NULL)
  {
    this->omegaNode = node;
  }
  else
  {
    XMLException::selfThrow("LinearBCXML - loadLinearBCProperties error : tag " + LINEARBC_OMEGA + " not found.");
  }

  if ((node = SiconosDOMTreeTools::findNodeChild(this->rootBCNode, LINEARBC_OMEGA0)) != NULL)
  {
    this->omega0Node = node;
  }
  else
  {
    XMLException::selfThrow("LinearBCXML - loadLinearBCProperties error : tag " + LINEARBC_OMEGA0 + " not found.");
  }

  if ((node = SiconosDOMTreeTools::findNodeChild(this->rootBCNode, LINEARBC_OMEGAT)) != NULL)
  {
    this->omegaTNode = node;
  }
  else
  {
    XMLException::selfThrow("LinearBCXML - loadLinearBCProperties error : tag " + LINEARBC_OMEGAT + " not found.");
  }
}

void LinearBCXML::updateBoundaryConditionXML(xmlNode* node) //, BoundaryCondition* bc)
{
  IN("LinearBCXML::updateBoundaryConditionXML\n");
  this->rootBCNode = node;
  OUT("LinearBCXML::updateBoundaryConditionXML\n");
}

