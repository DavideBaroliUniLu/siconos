//$Id: LinearDSIOXML.cpp,v 1.4 2005/03/10 12:55:21 jbarbier Exp $

#include "LinearDSIOXML.h"

LinearDSIOXML::LinearDSIOXML(): DSInputOutputXML()
{
  //  this->CNode = NULL;
  //  this->DNode = NULL;
  //  this->ENode = NULL;
  //  this->aNode = NULL;
}

LinearDSIOXML::LinearDSIOXML(xmlNode * dsioNode/*, vector<int> definedDSNumbers */)
  : DSInputOutputXML(dsioNode/*, definedDSNumbers */)
{
  xmlNode *node;

  //    if ((node=SiconosDOMTreeTools::findNodeChild(dsioNode, LINEARDSIO_C)) !=NULL)
  //    {
  //    this->CNode=node;
  //    }
  //    else
  //    {
  //    XMLException::selfThrow("LinearDSIOXML - constructor error : tag " + LINEARDSIO_C + " not found.");
  //    }
  //
  //    if ((node=SiconosDOMTreeTools::findNodeChild(dsioNode, LINEARDSIO_D)) !=NULL)
  //    {
  //    this->DNode=node;
  //    }
  //    else
  //    {
  //    XMLException::selfThrow("LinearDSIOXML - constructor error : tag " + LINEARDSIO_D + " not found.");
  //    }
  //
  //    if ((node=SiconosDOMTreeTools::findNodeChild(dsioNode, LINEARDSIO_E)) !=NULL)
  //    {
  //    this->ENode=node;
  //    }
  //    else
  //    {
  //    XMLException::selfThrow("LinearDSIOXML - constructor error : tag " + LINEARDSIO_E + " not found.");
  //    }
  //
  //    if ((node=SiconosDOMTreeTools::findNodeChild(dsioNode, LINEARDSIO_A)) !=NULL)
  //    {
  //    this->aNode=node;
  //    }
  //    else
  //    {
  //    XMLException::selfThrow("LinearDSIOXML - constructor error : tag " + LINEARDSIO_A + " not found.");
  //    }
}

LinearDSIOXML::~LinearDSIOXML()
{}

//void LinearDSIOXML::setC(SiconosMatrix *matrix)
//{
//  if( this->CNode == NULL )
//  {
//    this->CNode = SiconosDOMTreeTools::createMatrixNode(this->rootDSIOXMLNode, LINEARDSIO_C, matrix);
//  }
//  else SiconosDOMTreeTools::setSiconosMatrixValue(CNode, matrix);
//}
//
//void LinearDSIOXML::setD(SiconosMatrix *matrix)
//{
//  if( this->DNode == NULL )
//  {
//    this->DNode = SiconosDOMTreeTools::createMatrixNode(this->rootDSIOXMLNode, LINEARDSIO_D, matrix);
//  }
//  else SiconosDOMTreeTools::setSiconosMatrixValue(DNode, matrix);
//}
//
//void LinearDSIOXML::setE(SiconosMatrix *matrix)
//{
//  if( this->ENode == NULL )
//  {
//    this->ENode = SiconosDOMTreeTools::createMatrixNode(this->rootDSIOXMLNode, LINEARDSIO_E, matrix);
//  }
//  else SiconosDOMTreeTools::setSiconosMatrixValue(ENode, matrix);
//}
//
//void LinearDSIOXML::setA(SiconosVector *vector)
//{
//  if( this->aNode == NULL )
//  {
//    this->aNode = SiconosDOMTreeTools::createVectorNode(this->rootDSIOXMLNode, LINEARDSIO_A, vector);
//  }
//  else SiconosDOMTreeTools::setSiconosVectorValue(aNode, vector);
//}

//$Log: LinearDSIOXML.cpp,v $
//Revision 1.4  2005/03/10 12:55:21  jbarbier
//- implmentation of the EqualityConstraint and DSInputOutput classes in progress
//    attributes H (DSIO) et G (EC) added in XML and managed in XML objects
//
//Revision 1.3  2005/03/09 15:30:37  jbarbier
//- add of LagrangianEC class
//
//- in progress : implementation of the EqualityConstraint and DSInputOutput - create methods
//
//Revision 1.2  2005/01/26 13:50:40  jbarbier
//
//- loading of an XML input file now loads EqualityConstraints and DSInputOutputs
//
//Revision 1.1  2005/01/17 10:56:27  jbarbier
//- classes EqualityConstraint and DSInputOutput added with inherited classes
//
//- classes EqualityConstraintXML and DSInputOutputXML added with inherited classes
//
