#ifndef __LagrangianNLDSXMLTest__
#define __LagrangianNLDSXMLTest__

#include <cppunit/extensions/HelperMacros.h>

#include <string>
#include <cstdlib>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "LagrangianNLDSXML.h"




class LagrangianNLDSXMLTest : public CppUnit::TestFixture
{


private:

  xmlDoc *doc;
  xmlNode *root;
  LagrangianNLDSXML ds;
  SiconosMatrix matrixRef;
  /*SiconosVector*/
  SimpleVector vectorRef;


  // on nomme la suite de tests
  CPPUNIT_TEST_SUITE(LagrangianNLDSXMLTest);

  // on ajoute les tests a effectuer

  // les tests qui doivent passer
  CPPUNIT_TEST(testGetNdof);
  CPPUNIT_TEST(testGetQVelocity);
  CPPUNIT_TEST(testGetMass);
  CPPUNIT_TEST(testIsPlugin);
  CPPUNIT_TEST(testGetPluginName);
  CPPUNIT_TEST(testGetMemory);

  //CPPUNIT_TEST_EXCEPTION(testIfTagIsNotPresent, XMLException);

  // on termine
  CPPUNIT_TEST_SUITE_END();

  // declaration de fonctions de test
  void testGetNdof();
  void testGetQVelocity();
  void testGetMemory();
  void testGetMass();
  void testIsPlugin();
  void testGetPluginName();

public:
  void setUp();
  void tearDown();
};

#endif
