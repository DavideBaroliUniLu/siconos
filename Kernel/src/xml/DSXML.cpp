#include "DSXML.h"
#include "NSDSXML.h"

#include "LinearDSIOXML.h"
#include "LagrangianDSIOXML.h"

#include "check.h"


DSXML::DSXML()
{
  this->boundaryConditionXML = NULL;
  this->parentNode = NULL;
  this->rMemoryXML = NULL;
  this->xDotMemoryXML = NULL;
  this->xMemoryXML = NULL;

  this->idNode = NULL;
  this->nNode = NULL;
  this->x0Node = NULL;
  this->xNode = NULL;
  this->xDotNode = NULL;
  this->xMemoryNode = NULL;
  this->xDotMemoryNode = NULL;
  this->stepsInMemoryNode = NULL;
  this->vectorFieldNode = NULL;
  this->computeJacobianXNode = NULL;
  this->boundaryConditionNode = NULL;
  this->rNode = NULL;
  this->rMemoryNode = NULL;
}


DSXML::DSXML(xmlNode * DSNode, bool isBVP)
{
  this->boundaryConditionXML = NULL;

  this->rMemoryXML = NULL;
  this->xDotMemoryXML = NULL;
  this->xMemoryXML = NULL;

  this->rootDSXMLNode = DSNode;
  this->parentNode = NULL;
  loadDSProperties(isBVP);
}

DSXML::~DSXML()
{
  if (this->boundaryConditionXML != NULL) delete this->boundaryConditionXML;
  if (this->rMemoryXML != NULL) delete this->rMemoryXML;
  if (this->xMemoryXML != NULL) delete this->xMemoryXML;
  if (this->xDotMemoryXML != NULL) delete this->xDotMemoryXML;
}

void DSXML::loadDSProperties(bool isBVP)
{
  xmlNode *node;

  if ((node = SiconosDOMTreeTools::findNodeChild(this->rootDSXMLNode, BOUNDARYCONDITION_TAG)) != NULL)
  {
    this->loadBoundaryConditionXML(node);
    this->boundaryConditionNode = node;
  }
  else
  {
    //XMLException::selfThrow("DSXML - loadDSPropertiesXML error : tag " +BOUNDARYCONDITION_TAG+ " not found whereas NSDS is BVP.");
    cout << "DSXML - loadDSProperties warning : tag " << BOUNDARYCONDITION_TAG << " not found, it is an optional attribute." << endl;
    this->boundaryConditionNode = NULL;
  }

  //    if ((node=SiconosDOMTreeTools::findNodeChild(this->rootDSXMLNode, DSINPUTOUTPUT_DEFINITION_TAG)) !=NULL)
  //    {
  //    this->loadDSInputOutputXML(node);
  //    this->dsInputOutputNode = node;
  //    }
  //    else
  //    {
  //    cout<<"DSXML - loadNSDS WARNING : tag "<<DSINPUTOUTPUT_DEFINITION_TAG<<" not found,\nDefining DS InputOutput is optional."<<endl;
  //    this->dsInputOutputNode = NULL;
  //    }

  if ((node = SiconosDOMTreeTools::findNodeChild(this->rootDSXMLNode, ID_ATTRIBUTE)) != NULL)
  {
    this->idNode = node;
  }
  else
  {
    //XMLException::selfThrow("DSXML - loadDSProperties error : tag " +DS_ID+ " not found.");
    cout << "DSXML - loadDSProperties warning : tag " << ID_ATTRIBUTE << " not found, it is an optional attribute." << endl;
    this->idNode = NULL;
  }

  if ((node = SiconosDOMTreeTools::findNodeChild(this->rootDSXMLNode, DS_N)) != NULL)
  {
    this->nNode = node;
  }
  else
  {
    if ((this->getType() == LAGRANGIAN_NON_LINEARDS_TAG) || (this->getType() == LAGRANGIAN_TIME_INVARIANTDS_TAG))
    {
      cout << "DSXML - loadDSProperties warning : tag " << DS_N << " not found, it is an optional attribute." << endl;
      this->nNode = NULL;
    }
    else XMLException::selfThrow("DSXML - loadDSProperties error : tag " + DS_N + " not found.");
  }

  if ((node = SiconosDOMTreeTools::findNodeChild(this->rootDSXMLNode, DS_X0)) != NULL)
  {
    this->x0Node = node;
  }
  else
  {
    //XMLException::selfThrow("DSXML - loadDSProperties error : tag " +DS_X0+ " not found.");
    cout << "DSXML - loadDSProperties warning : tag " << DS_X0 << " not found, it is an optional attribute." << endl;
    this->x0Node = NULL;
  }

  if ((node = SiconosDOMTreeTools::findNodeChild(this->rootDSXMLNode, DS_X)) != NULL)
  {
    this->xNode = node;
  }
  else
  {
    //XMLException::selfThrow("DSXML - loadDSProperties error : tag " + DS_X + " not found.");
    cout << "DSXML - loadDSProperties warning : tag " << DS_X << " not found, it is an optional attribute." << endl;
    this->xNode = NULL;
  }

  if ((node = SiconosDOMTreeTools::findNodeChild(this->rootDSXMLNode, DS_XDOT)) != NULL)
  {
    this->xDotNode = node;
  }
  else
  {
    //XMLException::selfThrow("DSXML - loadDSProperties error : tag " + DS_XDOT + " not found.");
    cout << "DSXML - loadDSProperties warning : tag " << DS_XDOT << " not found, it is an optional attribute." << endl;
    this->xDotNode = NULL;
  }

  if ((node = SiconosDOMTreeTools::findNodeChild(this->rootDSXMLNode, DS_XMEMORY)) != NULL)
  {
    this->xMemoryNode = node;
    this->xMemoryXML = new SiconosMemoryXML(this->xMemoryNode, this->parentNode, DS_XMEMORY);
  }
  else
  {
    //XMLException::selfThrow("DSXML - loadDSProperties error : tag " + DS_XMEMORY + " not found.");
    cout << "DSXML - loadDSProperties warning : tag " << DS_XMEMORY << " not found, it is an optional attribute." << endl;
    this->xMemoryNode = NULL;
  }

  if ((node = SiconosDOMTreeTools::findNodeChild(this->rootDSXMLNode, DS_XDOTMEMORY)) != NULL)
  {
    this->xDotMemoryNode = node;
    this->xDotMemoryXML = new SiconosMemoryXML(this->xDotMemoryNode, this->parentNode, DS_XDOTMEMORY);
  }
  else
  {
    //XMLException::selfThrow("DSXML - loadDSProperties error : tag " + DS_XDOTMEMORY + " not found.");
    cout << "DSXML - loadDSProperties warning : tag " << DS_XDOTMEMORY << " not found, it is an optional attribute." << endl;
    this->xDotMemoryNode = NULL;
  }


  /*    if ((node=SiconosDOMTreeTools::findNodeChild(this->rootDSXMLNode, DS_OMEGA0)) !=NULL)
      {
      this->omega0Node=node;
      }
      else
      {
      XMLException::selfThrow("DSXML - loadDSProperties error : tag " + DS_OMEGA0 + " not found.");
      }

      if ((node=SiconosDOMTreeTools::findNodeChild(this->rootDSXMLNode, DS_OMEGAT)) !=NULL)
      {
      this->omegaTNode=node;
      }
      else
      {
      XMLException::selfThrow("DSXML - loadDSProperties error : tag " + DS_OMEGAT + " not found.");
      }
  */
  if ((node = SiconosDOMTreeTools::findNodeChild(this->rootDSXMLNode, DS_STEPSINMEMORY)) != NULL)
  {
    this->stepsInMemoryNode = node;
  }
  else
  {
    //XMLException::selfThrow("DSXML - loadDSProperties error : tag " + DS_STEPSINMEMORY + " not found.");
    cout << "DSXML - loadDSProperties warning : tag " << DS_STEPSINMEMORY << " not found, it is an optional attribute." << endl;
    this->stepsInMemoryNode = NULL;
  }

  if ((node = SiconosDOMTreeTools::findNodeChild(this->rootDSXMLNode, DS_R)) != NULL)
  {
    this->rNode = node;
  }
  else
  {
    cout << "DSXML - loadDSProperties warning : tag " << DS_R << " not found, it is an optional attribute." << endl;
    this->rNode = NULL;
  }

  if ((node = SiconosDOMTreeTools::findNodeChild(this->rootDSXMLNode, DS_RMEMORY)) != NULL)
  {
    this->rMemoryNode = node;
    this->rMemoryXML = new SiconosMemoryXML(this->rMemoryNode, this->parentNode, DS_RMEMORY);
  }
  else
  {
    //XMLException::selfThrow("DSXML - loadDSProperties error : tag " + DS_RMEMORY + " not found.");
    cout << "DSXML - loadDSProperties warning : tag " << DS_RMEMORY << " not found, it is an optional attribute." << endl;
    this->rMemoryNode = NULL;
  }

  if ((node = SiconosDOMTreeTools::findNodeChild(this->rootDSXMLNode, DS_VECTORFIELD)) != NULL)
  {
    this->vectorFieldNode = node;
  }
  else
  {
    //XMLException::selfThrow("DSXML - loadDSProperties error : tag " + DS_VECTORFIELD + " not found.");
    cout << "DSXML - loadDSProperties warning : tag " << DS_VECTORFIELD << " not found, it is an optional attribute." << endl;
    this->vectorFieldNode = NULL;
  }

  if ((node = SiconosDOMTreeTools::findNodeChild(this->rootDSXMLNode, DS_COMPUTEJACOBIANX)) != NULL)
  {
    this->computeJacobianXNode = node;
  }
  else
  {
    //XMLException::selfThrow("DSXML - loadDSProperties error : tag " + DS_COMPUTEJACOBIANX + " not found.");
    cout << "DSXML - loadDSProperties warning : tag " << DS_COMPUTEJACOBIANX << " not found, it is an optional attribute." << endl;
    this->computeJacobianXNode = NULL;
  }
}

void DSXML::loadBoundaryConditionXML(xmlNode * rootBoundaryConditionNode)
{
  if (rootBoundaryConditionNode == NULL)  //BoundaryCondition is not defined
  {
    this->boundaryConditionXML = NULL;
  }
  else
  {
    //string type = SiconosDOMTreeTools::getStringAttributeValue(rootBoundaryConditionNode, TYPE_ATTRIBUTE);
    xmlNode *node = SiconosDOMTreeTools::findNodeChild(rootBoundaryConditionNode);
    string type((char*)node->name);
    if (type == NON_LINEARBC_TAG)
    {
      this->boundaryConditionXML = new NLinearBCXML(node);
    }
    else if (type == LINEARBC_TAG)
    {
      this->boundaryConditionXML = new LinearBCXML(node);
    }
    else if (type == PERIODICBC_TAG)
    {
      this->boundaryConditionXML = new PeriodicBCXML(node);
    }
    else
    {
      XMLException::selfThrow("DSXML : undefined boundary condition type : " + type);
    }
  }
}

void DSXML::updateDynamicalSystemXML(xmlNode* rootDSXMLNode, DynamicalSystem* ds, BoundaryCondition* bc)
{
  IN("DSXML::updateDynamicalSystemXML\n");
  this->rootDSXMLNode = rootDSXMLNode;
  this->loadDS(ds);
  OUT("DSXML::updateDynamicalSystemXML\n");
}

void DSXML::loadDS(DynamicalSystem* ds)
{
  IN("DSXML::loadDS( DynamicalSystem* ds)\n");
  string type;
  xmlNode* node;

  if (ds->getBoundaryCondition() != NULL)
  {
    type = ds->getBoundaryCondition()->getType();
    node = xmlNewChild(rootDSXMLNode, NULL, (xmlChar*)BOUNDARYCONDITION_TAG.c_str(), NULL);
    if (type == NLINEARBC)
    {
      //xmlNewProp( node, (xmlChar*)TYPE_ATTRIBUTE.c_str(), (xmlChar*)NON_LINEARBC_TAG.c_str() );
      node = xmlNewChild(node, NULL, (xmlChar*)NON_LINEARBC_TAG.c_str(), NULL);
      this->boundaryConditionXML = new NLinearBCXML();

      // linkage between the DynamicalSystem and his DSXML
      ds->getBoundaryCondition()->setBoundaryConditionXML(this->boundaryConditionXML);

      // creation of the DynamicalSystemXML
      static_cast<NLinearBCXML*>(this->boundaryConditionXML)->updateBoundaryConditionXML(node);  //, ds->getBoundaryCondition() );
    }
    else if (type == LINEARBC)
    {
      //xmlNewProp( node, (xmlChar*)TYPE_ATTRIBUTE.c_str(), (xmlChar*)LINEARBC_TAG.c_str() );
      node = xmlNewChild(node, NULL, (xmlChar*)LINEARBC_TAG.c_str(), NULL);
      this->boundaryConditionXML = new LinearBCXML();

      // linkage between the DynamicalSystem and his DSXML
      ds->getBoundaryCondition()->setBoundaryConditionXML(this->boundaryConditionXML);

      // creation of the DynamicalSystemXML
      static_cast<LinearBCXML*>(this->boundaryConditionXML)->updateBoundaryConditionXML(node); //, ds->getBoundaryCondition() );
    }
    else if (type == PERIODICBC)
    {
      //xmlNewProp( node, (xmlChar*)TYPE_ATTRIBUTE.c_str(), (xmlChar*)PERIODICBC_TAG.c_str() );
      node = xmlNewChild(node, NULL, (xmlChar*)PERIODICBC_TAG.c_str(), NULL);
      this->boundaryConditionXML = new PeriodicBCXML();

      // linkage between the DynamicalSystem and his DSXML
      ds->getBoundaryCondition()->setBoundaryConditionXML(this->boundaryConditionXML);

      // creation of the DynamicalSystemXML
      static_cast<PeriodicBCXML*>(this->boundaryConditionXML)->updateBoundaryConditionXML(node); //, ds->getBoundaryCondition() );
    }
    else
    {
      XMLException::selfThrow("DSXML - loadDS error : undefined DS type : " + type + " (have you forgotten to verify the xml files with the Siconos Schema file or update it!?).");
    }
  }

  if (ds->getDSInputOutputs().size() > 0)
  {
    int number;
    char num[32];
    map<int, DSInputOutputXML*>::iterator itdsio;
    xmlNode *dsioDefinitionNode, *nsdsNode;
    DSInputOutputXML *dsioXML;

    nsdsNode = ds->getNSDS()->getNSDSXML()->getNSDSXMLNode();
    dsioDefinitionNode = SiconosDOMTreeTools::findNodeChild((const xmlNode*)nsdsNode, DSINPUTOUTPUT_DEFINITION_TAG);
    if (dsioDefinitionNode == NULL)
      dsioDefinitionNode = xmlNewChild(nsdsNode, NULL, (xmlChar*)DSINPUTOUTPUT_DEFINITION_TAG.c_str(), NULL);

    for (int i = 0; i < ds->getDSInputOutputs().size(); i++)
    {
      if (ds->getDSInputOutput(i)->getDSInputOutputXML() == NULL)
      {
        number = ds->getDSInputOutput(i)->getNumber();
        sprintf(num, "%d", number);
        this->definedDSInputOutputNumbers.push_back(number);

        // verifies if this DSInputOutput has a number which not used
        itdsio = this->dsInputOutputXMLMap.find(number);
        if (itdsio == this->dsInputOutputXMLMap.end())
        {
          if (ds->getDSInputOutput(i)->getType() == LINEARDSIO)
          {
            node = xmlNewChild(dsioDefinitionNode, NULL, (xmlChar*)LINEAR_DSIO_TAG.c_str(), NULL);
            xmlNewProp(node, (xmlChar*)NUMBER_ATTRIBUTE.c_str(), (xmlChar*)num);
            //xmlNewChild( node, NULL, (xmlChar*)DSINPUTOUTPUT_H.c_str(), NULL );
            dsioXML = new LinearDSIOXML();

            // linkage between the DSInputOutput and his DSInputOutputXML
            ds->getDSInputOutput(i)->setDSInputOutputXML(dsioXML);
            dsioXML->updateDSInputOutputXML(node, ds->getDSInputOutput(i));
            this->dsInputOutputXMLMap[number] = dsioXML;
          }
          else if (ds->getDSInputOutput(i)->getType() == NLINEARDSIO)
          {
            node = xmlNewChild(dsioDefinitionNode, NULL, (xmlChar*)NON_LINEAR_DSIO_TAG.c_str(), NULL);
            xmlNewProp(node, (xmlChar*)NUMBER_ATTRIBUTE.c_str(), (xmlChar*)num);
            //xmlNewChild( node, NULL, (xmlChar*)DSINPUTOUTPUT_H.c_str(), NULL );
            dsioXML = new DSInputOutputXML();

            // linkage between the DSInputOutput and his DSInputOutputXML
            ds->getDSInputOutput(i)->setDSInputOutputXML(dsioXML);
            dsioXML->updateDSInputOutputXML(node, ds->getDSInputOutput(i));
            this->dsInputOutputXMLMap[number] = dsioXML;
          }
          else if (ds->getDSInputOutput(i)->getType() == LAGRANGIANDSIO)
          {
            node = xmlNewChild(dsioDefinitionNode, NULL, (xmlChar*)LAGRANGIAN_DSIO_TAG.c_str(), NULL);
            xmlNewProp(node, (xmlChar*)NUMBER_ATTRIBUTE.c_str(), (xmlChar*)num);
            //xmlNewChild( node, NULL, (xmlChar*)DSINPUTOUTPUT_H.c_str(), NULL );
            dsioXML = new LagrangianDSIOXML();

            // linkage between the DSInputOutput and his DSInputOutputXML
            ds->getDSInputOutput(i)->setDSInputOutputXML(dsioXML);
            dsioXML->updateDSInputOutputXML(node, ds->getDSInputOutput(i));
            this->dsInputOutputXMLMap[number] = dsioXML;
          }
          else XMLException::selfThrow("DSXML - loadDS | Error : the DSInputOutput type : " + ds->getDSInputOutput(i)->getType() + " doesn't exist!");

          /*  end of the save : saving the DynamicalSystem linked to this DSInputOutput */
          node = xmlNewChild(node, NULL, (xmlChar*)DS_CONCERNED.c_str(), NULL);
          node = xmlNewChild(node, NULL, (xmlChar*) DYNAMICAL_SYSTEM_TAG.c_str(), NULL);
          number = ds->getNumber();
          sprintf(num, "%d", number);
          xmlNewProp(node, (xmlChar*)NUMBER_ATTRIBUTE.c_str(), (xmlChar*)num);
        }
        else cout << "DSXML - loadDS : the DSInputOutput type : " << ds->getDSInputOutput(i)->getType() << " already exists!" << endl;
      }
      else cout << "### strange, DSIOXML != NULL :gratgrat:" << endl;
    }
  }
  //    else
  //    cout<<"DSXML - loadDS WARNING : tag "<<DSINPUTOUTPUT_DEFINITION_TAG<<" not found,\nDefining DS InputOutput is optional."<<endl;
  OUT("DSXML::loadDS( DynamicalSystem* ds)\n");
}

DSInputOutputXML* DSXML::getDSInputOutputXML(int number)
{
  map<int, DSInputOutputXML*>::iterator it;

  it = this->dsInputOutputXMLMap.find(number);
  if (it == this->dsInputOutputXMLMap.end())
  {
    return NULL;
  }
  return this->dsInputOutputXMLMap[number];
}

void DSXML::setDSInputOutputXML(map<int, DSInputOutputXML*> m)
{
  this->definedDSInputOutputNumbers.clear();

  map<int, DSInputOutputXML*>::iterator iter;
  for (iter = m.begin(); iter != m.end(); iter++)
  {
    this->definedDSInputOutputNumbers.push_back((*iter).first);
    //cout<<"** DSXML::setDSInputOutputXML ==> "<<(*iter).first<<" - "<<(*iter).second->getType()<<endl;
  }

  this->dsInputOutputXMLMap = m;
}

//void DSXML::loadDSInputOutputXML(xmlNode * rootdsioNode)
//{
//  xmlNode *node;
//    int number; //Number of an EqualityCopnstraint
//  map<int, DSInputOutputXML*>::iterator i;
//
//    node = SiconosDOMTreeTools::findNodeChild((const xmlNode*)rootdsioNode);
//
//  while(node!=NULL)
//    {
//      DSInputOutputXML *ecxml;
//
//    number = SiconosDOMTreeTools::getIntegerAttributeValue(node, NUMBER_ATTRIBUTE);
//
//    cout<<"CHARGEMENT DSInputOutput number"<<number<<endl;
//
//    i = dsInputOutputXMLMap.find(number);
//    if (i == dsInputOutputXMLMap.end())
//    {
//      this->definedDSInputOutputNumbers.push_back(number);
//      ecxml = new DSInputOutputXML((xmlNode *)node/*, this->definedDSNumbers*/);
//      this->dsInputOutputXMLMap[number] = ecxml;
//      }
//      else
//      {
//      XMLException::selfThrow("DSXML - loadDSInputOutputXML error : wrong DSINPUTOUTPUT number : already exists.");
//    }
//
//      node = SiconosDOMTreeTools::findFollowNode(node);
//   }
//}
