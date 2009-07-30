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

/*!\file BouncingBallEDXml.cpp
  \brief \ref EMBouncingBall - C++/XML input file, Event-Driven version - V. Acary, F. Perignon.

  A Ball bouncing on the ground.
  Description of the model with XML input.
  Simulation with an Event-Driven scheme.
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
    SP::Model bouncingBall(new Model("./BallED.xml"));
    cout << "\n *** BallED.xml file loaded ***" << endl << endl;

    cout << "====> Initialisation ..." << endl << endl;
    bouncingBall->initialize();

    // --- Get and initialize the simulation ---
    SP::EventDriven s = boost::static_pointer_cast<EventDriven>(bouncingBall->getSimulationPtr());
    SP::LagrangianDS ball = boost::static_pointer_cast<LagrangianDS> (bouncingBall->getNonSmoothDynamicalSystemPtr()->getDynamicalSystemPtrNumber(0));

    // --- Get the values to be plotted ---
    // -> saved in a matrix dataPlot

    int N = 12368; // Number of saved points: depends on the number of events ...
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
    SP::EventsManager eventsManager = s->getEventsManagerPtr();
    unsigned int numberOfEvent = 0 ;
    int k = 0;
    double T = bouncingBall->getFinalT();
    bool nonSmooth = false;

    while (s->getNextTime() < T)
    {
      k++;

      s->advanceToEvent();
      if (eventsManager->getNextEventPtr()->getType() == 2)
        nonSmooth = true;

      s->processEvents();
      // If the treated event is non smooth, the pre-impact state has been solved in memory vectors during process.
      if (nonSmooth)
      {
        dataPlot(k, 0) = s->getStartingTime();
        dataPlot(k, 1) = (*ball->getQMemoryPtr()->getSiconosVector(1))(0);
        dataPlot(k, 2) = (*ball->getVelocityMemoryPtr()->getSiconosVector(1))(0);
        k++;
        nonSmooth = false;
      }
      dataPlot(k, 0) = s->getStartingTime();
      dataPlot(k, 1) = (*q)(0);
      dataPlot(k, 2) = (*v)(0);
      dataPlot(k, 3) = (*p)(0);
      numberOfEvent++;
    }

    // --- Output files ---
    cout << "===== End of Event Driven simulation. " << numberOfEvent << " events have been processed. ==== " << endl << endl;
    cout << "====> Output file writing ..." << endl;
    ioMatrix io("result.dat", "ascii");
    io.write(dataPlot, "noDim");

  }

  catch (SiconosException e)
  {
    cout << e.report() << endl;
  }
  catch (...)
  {
    cout << "Exception caught in \'sample/MultiBeadsColumn\'" << endl;
  }
  cout << "Computation Time: " << time.elapsed()  << endl << endl;
}
