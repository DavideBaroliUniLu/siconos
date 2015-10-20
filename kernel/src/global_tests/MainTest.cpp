/* Siconos-sample version 3.0.0, Copyright INRIA 2005-2012.
 * Siconos is a program dedicated to modeling, simulation and control
 * of non smooth dynamical systems.
 * Siconos is a free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * Siconos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Siconos; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Contact: Vincent ACARY, siconos-team@lists.gforge.inria.fr
*/

// =============================== Global Siconos Kernel tests =========================================
//
//  Tests for complete samples (ie not cppunit tests), model construction, simulation and so on.
//  Based on examples of sample directory.
//
//  How to implement a new one?
//   - add a file myExample.cpp, that contains a function of type:
//       bool myExample(), which run the simulation you want and return true if succeed, else false.
//   - declare this function in TestMain.h
//   - add it in the GlobalTest object (see add Test comment) in the present file.
//   - change Makefile.am to take new source into account
//   if your sample needs some external plug-in, put them in gTestPlugin.cpp
// Then run make check.
// \todo: change makefile to have to different commands for present tests and cppunit tests.
//
// ======================================================================================================

#include "MainTest.hpp"



int main(int argc, char* argv[])
{
  try
  {
    std::cout <<std::endl;
    std::cout <<std::endl;
    std::cout << " ===================================================" <<std::endl;
    std::cout << " ========= Global tests for Siconos-Kernel =========" <<std::endl;
    std::cout << " ===================================================" <<std::endl;
    std::cout <<std::endl;
    std::cout <<std::endl;

    string logFile = "GlobalTests.log";

    // Declare the tests list, with its output log file as argument
    GlobalTest* GTest = new GlobalTest(logFile);

    // Add tests
    GTest->addTest("BouncingBall", BouncingBall);

    GTest->addTest("DiodeBridge", DiodeBridge);

    GTest->addTest("BallBowl", BallBowl);
    // Run tests
    GTest->run();

    // Print results
    GTest->print();

    std::cout << " ===================================================" <<std::endl;
    std::cout << " ============== End of global testing ==============" <<std::endl;
    std::cout << " The number of tests run is: " << GTest->getNumberOfTests() <<std::endl;
    std::cout << " Among them, " << GTest->getNumberOfFailedTests() << " has failed." <<std::endl;
    std::cout << " See "  << logFile << " for details." <<std::endl;
    std::cout << " ===================================================" <<std::endl;

    if (GTest->getNumberOfFailedTests() != 0)
      return 1;
    return 0;

  }

  // --- Exceptions handling ---
  catch (SiconosException e)
  {
    std::cout << e.report() <<std::endl;
  }
  catch (...)
  {
    std::cout << "Exception caught in \'sample/BouncingBall\'" <<std::endl;
  }
}