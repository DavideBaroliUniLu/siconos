/* Siconos-sample version 3.0.0, Copyright INRIA 2005-2008.
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
 * Contact: Vincent ACARY vincent.acary@inrialpes.fr
*/


/*!\file BouncingBallTSXml.cpp
\brief \ref EMBouncingBall - C++/XML input file, Time-Stepping version - V. Acary, F. Perignon.

A Ball bouncing on the ground.
Description of the model with XML input.
Simulation with a Time-Stepping scheme.
*/

#include "SiconosKernel.hpp"

using namespace std;

int main(int argc, char* argv[])
{
  boost::timer time;
  time.restart();
  try
  {

    // --- Model loading from xml file ---
    cout << "====> Model loading (XML) ..." << endl << endl;

    SP::Model bouncingBall(new Model("./BallTS.xml"));
    cout << "\n *** BallTS.xml file loaded ***" << endl << endl;


    cout << "====> Initialisation ..." << endl << endl;
    bouncingBall->initialize();

    // --- Get the simulation ---
    SP::TimeStepping s = boost::static_pointer_cast<TimeStepping>(bouncingBall->getSimulationPtr());
    SP::LagrangianDS ball = boost::static_pointer_cast<LagrangianDS> (bouncingBall->getNonSmoothDynamicalSystemPtr()->getDynamicalSystemPtrNumber(1));
    // --- Get the time discretisation scheme ---
    SP::TimeDiscretisation t = s->getTimeDiscretisationPtr();

    int N = 2000; // Number of time steps
    // --- Get the values to be plotted ---
    // -> saved in a matrix dataPlot

    unsigned int outputSize = 4;
    SimpleMatrix dataPlot(N + 1, outputSize);

    SP::SiconosVector q = ball->getQPtr();
    SP::SiconosVector v = ball->getVelocityPtr();
    SP::SiconosVector p = ball->getPPtr(2);

    dataPlot(0, 0) = bouncingBall->getT0();
    dataPlot(0, 1) = (*q)(0);
    dataPlot(0, 2) = (*v)(0);
    dataPlot(0, 3) = (*p)(0);

    cout << "====> Start computation ... " << endl << endl;
    // --- Time loop  ---
    int k = 1;
    while (s->getNextTime() <= bouncingBall->getFinalT())
    {
      s->computeOneStep();
      // --- Get values to be plotted ---
      dataPlot(k, 0) =  s->getNextTime();
      dataPlot(k, 1) = (*q)(0);
      dataPlot(k, 2) = (*v)(0);
      dataPlot(k, 3) = (*p)(0);
      s->nextStep();
      k++;
    }
    cout << "End of computation - Number of iterations done: " << k - 1 << endl << endl;

    cout << "====> Output file writing ..." << endl << endl;
    ioMatrix io("result.dat", "ascii");
    io.write(dataPlot, "noDim");

    // Xml output
    //  bouncingBall->saveToXMLFile("./BouncingBall_TIDS.xml.output");
  }

  // --- Exceptions handling ---
  catch (SiconosException e)
  {
    cout << e.report() << endl;
  }
  catch (...)
  {
    cout << "Exception caught in \'sample/BouncingBallXml\'" << endl;
  }
  cout << "Computation Time: " << time.elapsed()  << endl;
}
