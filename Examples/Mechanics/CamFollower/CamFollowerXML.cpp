/* Siconos-Examples , Copyright INRIA 2005-2011.
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

// =============================== Cam Follower (1DOF Impact System) ===============================
//
// The Cam Follower system is modelled as a Generalised Langrangian System impacting against a fixed wall
// the moving constraint (i.e. a rotational cam) is modelled as an input force
//
// Direct description of the model with XML input.
//
// Keywords: LagrangianLinearDS, LagrangianLinear relation, Moreau TimeStepping, LCP, XML.
//
// ======================================================================================================

#include "SiconosKernel.hpp"
#include "CamState.h"

using namespace std;

int main(int argc, char* argv[])
{
  try
  {
    // --- Model loading from xml file ---
    //Model bouncingBall("./BouncingBall_TIDS.xml");
    SP::Model CamFollower(new Model("./CamFollower_TIDS.xml"));

    cout << "\n *** CamFollower_TIDS.xml file loaded ***" << endl;
    CamFollower->initialize();

    // --- Get and initialize the simulation ---
    SP::TimeStepping S = std11::static_pointer_cast<TimeStepping>(CamFollower->simulation());
    int k = 0;
    double T = CamFollower->finalT();
    double t0 = CamFollower->t0();
    double h = S->timeStep();
    int N = ceil((T - t0) / h);

    // --- Get the values to be plotted ---
    // -> saved in a matrix dataPlot
    SimpleMatrix DataPlot(N + 1, 8);

    cout << "Prepare data for plotting ... " << endl;
    // For the initial time step:
    // time
    DataPlot(k, 0) = t0;

    // state q for the Follower
    SP::LagrangianDS Follower = std11::static_pointer_cast<LagrangianDS> (CamFollower->nonSmoothDynamicalSystem()->dynamicalSystem(1));
    // Position of the Follower
    DataPlot(k, 1) = ((*Follower->q()))(0);
    // Velocity for the Follower
    DataPlot(k, 2) = ((*Follower->velocity()))(0);
    // Reaction
    DataPlot(k, 3) = (*CamFollower->nonSmoothDynamicalSystem()->topology()->interactions()->getPtr(1)->lambda(1))(0);
    // External Forcing
    DataPlot(k, 4) = ((*Follower->fExt()))(0);

    // State of the Cam
    double rpm = 358;
    double CamEqForce, CamPosition, CamVelocity, CamAcceleration;

    CamEqForce = CamState(t0, rpm, CamPosition, CamVelocity, CamAcceleration);
    // Position of the Cam
    DataPlot(k, 5) = CamPosition;
    // Velocity of the Cam
    DataPlot(k, 6) = CamVelocity;
    // Acceleration of the Cam
    DataPlot(k, 7) = CamPosition + ((*Follower->q()))(0);


    // --- Compute elapsed time ---
    boost::timer tt;
    tt.restart();
    cout << "Computation ... " << endl;
    // --- Time loop  ---

    //    s->run();
    while (k < N)
    {
      // get current time step
      k++;

      S->computeOneStep();
      // --- Get values to be plotted ---
      DataPlot(k, 0) = S->nextTime();
      //  DataPlot(k, 1) = ((*Follower->q()))(0);
      //  DataPlot(k, 2) = ((*ball->velocity()))(0);
      //  DataPlot(k, 3) = (bouncingBall.nonSmoothDynamicalSystem()->topology()->interactions()->getPtr(0)->lambda(1))(0);
      DataPlot(k, 1) = ((*Follower->q()))(0);
      DataPlot(k, 2) = ((*Follower->velocity()))(0);
      DataPlot(k, 3) = (*CamFollower->nonSmoothDynamicalSystem()->topology()->interactions()->getPtr(1)->lambda(1))(0);
      DataPlot(k, 4) = ((*Follower->fExt()))(0);

      CamEqForce = CamState(S->nextTime(), rpm, CamPosition, CamVelocity, CamAcceleration);

      DataPlot(k, 5) = CamPosition;
      DataPlot(k, 6) = CamVelocity;
      DataPlot(k, 7) = CamPosition + ((*Follower->q()))(0);
      // transfer of state i+1 into state i and time incrementation

      S->nextStep();
    }

    // --- elapsed time computing ---
    cout << "time = " << tt.elapsed() << endl;

    // Number of time iterations
    cout << "Number of iterations done: " << k << endl;

    // dataPlot (ascii) output
    ioMatrix::write("result.dat", "ascii", DataPlot, "noDim");

    // Xml output
    //  bouncingBall.saveToXMLFile("./CamFollower_TIDS.xml.output");
  }

  // --- Exceptions handling ---
  catch (SiconosException e)
  {
    cout << e.report() << endl;
  }
  catch (...)
  {
    cout << "Exception caught in \'sample/CamFollowser\'" << endl;
  }
}
