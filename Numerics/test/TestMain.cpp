#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "TestNumerics.h"


int main()
{
  // on d�clare un runner
  CppUnit::TextUi::TestRunner runner;

  // on r�cup�re les classes de tests d�clar�es dans le registry.
  // chaque classe de test doit bien sur se referencer dans le registry
  CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();

  // on ajoute tous les tests dans le runner
  runner.addTest(registry.makeTest());

  // on lance les tests
  runner.run("", false, true, false);
}
