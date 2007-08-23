/* Siconos-Examples version 2.1.1, Copyright INRIA 2005-2007.
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

#include "SiconosKernel.h"
#include "CamState.h"

using namespace std;

int main(int argc, char* argv[])
{
  try
  {
    // --- Model loading from xml file ---
    //Model bouncingBall("./BouncingBall_TIDS.xml");
    Model CamFollower("./CamFollower_TIDS.xml");

    cout << "\n *** CamFollower_TIDS.xml file loaded ***" << endl;

    // --- Get and initialize the simulation ---
    //Simulation* s = bouncingBall.getSimulationPtr();
    TimeStepping* S = static_cast<TimeStepping*>(CamFollower.getSimulationPtr());
    cout << "Simulation initialization ..." << endl;
    S->initialize();
    cout << "\n **** the simulation is ready ****" << endl;

    // --- Get the time discretisation scheme ---
    TimeDiscretisation* t = S->getTimeDiscretisationPtr();
    int k = 0;
    int N = t->getNSteps(); // Number of time steps

    //t->display();

    // --- Get the values to be plotted ---
    // -> saved in a matrix dataPlot
    SimpleMatrix DataPlot(N + 1, 8);

    cout << "Prepare data for plotting ... " << endl;
    // For the initial time step:
    // time
    DataPlot(k, 0) = k * t->getH();

    // state q for the ball
    //  LagrangianDS* ball = static_cast<LagrangianDS*> (bouncingBall.getNonSmoothDynamicalSystemPtr()->getDynamicalSystemPtr(0));
    //     dataPlot(k, 1) = (ball->getQ())(0);
    //     // velocity for the ball
    //     dataPlot(k, 2) = (ball->getVelocity())(0);
    //     // Reaction
    //     dataPlot(k, 3) = (bouncingBall.getNonSmoothDynamicalSystemPtr()->getInteractionPtr(0)->getLambda(1))(0);

    // state q for the Follower
    LagrangianDS* Follower = static_cast<LagrangianDS*>(CamFollower.getNonSmoothDynamicalSystemPtr()->getDynamicalSystemPtr(0));
    // Position of the Follower
    DataPlot(k, 1) = (Follower->getQ())(0);
    // Velocity for the Follower
    DataPlot(k, 2) = (Follower->getVelocity())(0);
    // Reaction
    DataPlot(k, 3) = (CamFollower.getNonSmoothDynamicalSystemPtr()->getInteractionPtr(0)->getLambda(1))(0);
    // External Forcing
    DataPlot(k, 4) = (Follower->getFExt())(0);

    // State of the Cam
    double rpm = 358;
    double CamEqForce, CamPosition, CamVelocity, CamAcceleration;

    CamEqForce = CamState(k * t->getH(), rpm, CamPosition, CamVelocity, CamAcceleration);
    // Position of the Cam
    DataPlot(k, 5) = CamPosition;
    // Velocity of the Cam
    DataPlot(k, 6) = CamVelocity;
    // Acceleration of the Cam
    DataPlot(k, 7) = CamPosition + (Follower->getQ())(0);


    // --- Compute elapsed time ---
    double t1, t2, elapsed;
    struct timeval tp;
    int rtn;
    clock_t start, end;
    double elapsed2;
    start = clock();
    rtn = gettimeofday(&tp, NULL);
    t1 = (double)tp.tv_sec + (1.e-6) * tp.tv_usec;

    cout << "Computation ... " << endl;
    // --- Time loop  ---

    //    s->run();
    while (k < N)
    {
      // get current time step
      k++;

      S->computeOneStep();

      // --- Get values to be plotted ---
      DataPlot(k, 0) = k * t->getH();
      //  DataPlot(k, 1) = (Follower->getQ())(0);
      //  DataPlot(k, 2) = (ball->getVelocity())(0);
      //  DataPlot(k, 3) = (bouncingBall.getNonSmoothDynamicalSystemPtr()->getInteractionPtr(0)->getLambda(1))(0);
      DataPlot(k, 1) = (Follower->getQ())(0);
      DataPlot(k, 2) = (Follower->getVelocity())(0);
      DataPlot(k, 3) = (CamFollower.getNonSmoothDynamicalSystemPtr()->getInteractionPtr(0)->getLambda(1))(0);
      DataPlot(k, 4) = (Follower->getFExt())(0);

      CamEqForce = CamState(k * t->getH(), rpm, CamPosition, CamVelocity, CamAcceleration);

      DataPlot(k, 5) = CamPosition;
      DataPlot(k, 6) = CamVelocity;
      DataPlot(k, 7) = CamPosition + (Follower->getQ())(0);
      // transfer of state i+1 into state i and time incrementation

      S->nextStep();
    }

    // --- elapsed time computing ---
    end = clock();
    rtn = gettimeofday(&tp, NULL);
    t2 = (double)tp.tv_sec + (1.e-6) * tp.tv_usec;
    elapsed = t2 - t1;
    elapsed2 = (end - start) / (double)CLOCKS_PER_SEC;
    cout << "time = " << elapsed << " --- cpu time " << elapsed2 << endl;

    // Number of time iterations
    cout << "Number of iterations done: " << k << endl;

    // dataPlot (ascii) output
    ioMatrix io("result.dat", "ascii");
    io.write(DataPlot, "noDim");

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
