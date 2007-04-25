/* Siconos-sample version 2.0.1, Copyright INRIA 2005-2006.
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


// =============================== Robot arm sample (HuMAnsPa10) ===============================
//
// see modelRobot1.jpg for complete system view.
//
// Keywords: LagrangianDS, LagrangianLinear relation, Moreau TimeStepping, LCP.
//
// =============================================================================================

#include "SiconosKernel.h"
#include <math.h>

#define PI 3.14159265

using namespace std;


int main(int argc, char* argv[])
{
  try
  {

    // ================= Creation of the model =======================

    // User-defined main parameters
    unsigned int nDof = 2;           // degrees of freedom for robot arm
    double t0 = 0;                   // initial computation time
    double T = 25;                   // final computation time
    double h = 0.0005;                // time step
    double criterion = 0.0005;
    unsigned int maxIter = 20000;
    double e = 0.7;                  // nslaw
    double e2 = 0.0;

    // -> mind to set the initial conditions below.

    // -------------------------
    // --- Dynamical systems ---
    // -------------------------

    // unsigned int i;
    DynamicalSystemsSet allDS; // the list of DS

    // --- DS: manipulator arm ---

    // The dof are angles between ground and arm and between differents parts of the arm. (See corresponding .pdf for more details)

    // Initial position (angles in radian)
    SimpleVector q0(nDof), v0(nDof);
    q0.zero();
    v0.zero();
    q0(0) = 1.7;
    q0(1) = -1.5;
    SiconosVector * z = new SimpleVector(nDof * 2);
    (*z)(0) = q0(0);
    (*z)(1) = q0(1);
    (*z)(2) = v0(0);
    (*z)(3) = v0(1);

    LagrangianDS * arm = new LagrangianDS(1, q0, v0);

    // external plug-in
    arm->setComputeMassFunction("Two-linkPlugin.so", "mass");
    arm->setComputeNNLFunction("Two-linkPlugin.so", "NNL");
    arm->setComputeJacobianNNLFunction(1, "Two-linkPlugin.so", "jacobianVNNL");
    arm->setComputeJacobianNNLFunction(0, "Two-linkPlugin.so", "jacobianQNNL");
    arm->setComputeFExtFunction("Two-linkPlugin.so", "U");
    arm->setZPtr(z);

    allDS.insert(arm);

    // -------------------
    // --- Interactions---
    // -------------------

    //  - one with Lagrangian non linear relation to define contact with ground
    //  Both with newton impact nslaw.

    InteractionsSet allInteractions;

    // -- relations --

    NonSmoothLaw * nslaw = new NewtonImpactNSL(e);
    Relation * relation = new LagrangianScleronomousR("Two-linkPlugin:h0", "Two-linkPlugin:G0");
    Interaction * inter = new Interaction("floor-arm", allDS, 0, 2, nslaw, relation);


    SimpleMatrix H1(2, 2);
    SimpleVector b1(2);
    H1.zero();
    H1(0, 0) = -1;
    H1(1, 0) = 1;

    b1(0) = PI;
    b1(1) = 0;

    NonSmoothLaw * nslaw2 = new NewtonImpactNSL(e2);
    Relation * relation1 = new LagrangianLinearR(H1, b1);
    Interaction * inter1 =  new Interaction("floor-arm2", allDS, 1, 2, nslaw2, relation1);

    SimpleMatrix H2(2, 2);
    SimpleVector b2(2);
    H2.zero();
    H2(0, 1) = -1;
    H2(1, 1) = 1;

    b2(0) = -0.01;
    b2(1) = PI - 0.01;


    Relation * relation2 = new LagrangianLinearR(H2, b2);
    Interaction * inter2 =  new Interaction("singular-points", allDS, 2, 2, nslaw2, relation2);

    //  NonSmoothLaw * nslaw = new NewtonImpactNSL(e);
    //      Relation * relation = new LagrangianScleronomousR("Two-linkPlugin:h0","Two-linkPlugin:G0");
    //      Interaction * inter = new Interaction("floor-arm", allDS,0,1, nslaw, relation);

    allInteractions.insert(inter);
    allInteractions.insert(inter1);
    allInteractions.insert(inter2);
    // --------------------------------
    // --- NonSmoothDynamicalSystem ---
    // -------------------------------

    NonSmoothDynamicalSystem * nsds = new NonSmoothDynamicalSystem(allDS, allInteractions);

    // -------------
    // --- Model ---
    // -------------

    Model * Manipulator = new Model(t0, T);
    Manipulator->setNonSmoothDynamicalSystemPtr(nsds); // set NonSmoothDynamicalSystem of this model

    // ----------------
    // --- Simulation ---
    // ----------------

    // -- Time discretisation --
    TimeDiscretisation * t = new TimeDiscretisation(h, Manipulator);

    TimeStepping* s = new TimeStepping(t);

    // -- OneStepIntegrators --
    OneStepIntegrator * OSI =  new Moreau(arm, 0.500001, s);

    // -- OneStepNsProblem --
    OneStepNSProblem * osnspb = new LCP(s, "name", "Lemke", 20001, 0.0005);

    cout << "=== End of model loading === " << endl;

    // =========================== End of model definition ===========================  dataPlot(k,7) = (inter->getY(0))(0);


    // ================================= Computation
    // --- Simulation initialization ---



    s->initialize();
    cout << "End of simulation initialisation" << endl;

    int k = 0;
    int N = t->getNSteps(); // Number of time steps

    // --- Get the values to be plotted ---
    // -> saved in a matrix dataPlot
    unsigned int outputSize = 7;
    SimpleMatrix dataPlot(N + 1, outputSize);
    // For the initial time step:
    // time

    SiconosVector * q = arm->getQPtr();
    SiconosVector * v = arm->getVelocityPtr();

    dataPlot(k, 0) =  Manipulator->getCurrentT();
    dataPlot(k, 1) = (*q)(0);
    dataPlot(k, 2) = (*q)(1);
    dataPlot(k, 3) = (inter->getY(0))(1);
    dataPlot(k, 4) = (*v)(0);
    dataPlot(k, 5) = (*v)(1);
    dataPlot(k, 6) = (inter->getY(0))(0) - 2;
    //     //    EventsManager * eventsManager = s->getEventsManagerPtr();

    while (s->hasNextEvent())
    {
      (*z)(0) = (*q)(0);
      (*z)(1) = (*q)(1);
      (*z)(2) = (*v)(0);
      (*z)(3) = (*v)(1);
      // get current time step
      k++;
      s->newtonSolve(criterion, maxIter);
      dataPlot(k, 0) =  Manipulator->getCurrentT();
      dataPlot(k, 1) = (*q)(0);
      dataPlot(k, 2) = (*q)(1);
      dataPlot(k, 3) = (inter->getY(0))(1);
      dataPlot(k, 4) = (*v)(0);
      dataPlot(k, 5) = (*v)(1);
      dataPlot(k, 6) = (inter->getY(0))(0) - 2;
      //dataPlot(k,7) = (inter->getY(0))(1);
    }

    // --- Output files ---
    ioMatrix out("result.dat", "ascii");
    out.write(dataPlot, "noDim");

    //     // --- Free memory ---
    delete osnspb;
    delete t;
    delete OSI;
    delete s;
    delete Manipulator;
    delete nsds;
    delete inter;
    delete inter1;
    delete inter2;
    delete relation;
    delete relation1;
    delete relation2;
    delete nslaw;
    delete nslaw2;
    delete z;
    delete arm;
  }

  catch (SiconosException e)
  {
    cout << e.report() << endl;
  }
  catch (...)
  {
    cout << "Exception caught in \'sample/MultiBeadsColumn\'" << endl;
  }
}
