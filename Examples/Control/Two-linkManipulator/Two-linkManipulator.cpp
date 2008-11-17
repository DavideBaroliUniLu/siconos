/* Siconos-sample version 2.1.1, Copyright INRIA 2005-2007.
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

#include "SiconosKernel.hpp"
#include <math.h>

#define PI 3.14159265

using namespace std;


int main(int argc, char* argv[])
{
  boost::timer time;
  time.restart();
  try
  {

    // ================= Creation of the model =======================

    // User-defined main parameters
    unsigned int nDof = 2;           // degrees of freedom for robot arm
    double t0 = 0;                   // initial computation time
    double T = 30;                   // final computation time
    double h = 1e-4;                // time step
    double criterion = 1e-8;
    unsigned int maxIter = 20000;
    double e = 0.7;                  // nslaw
    double e2 = 0.0;
    double L = 0.0;
    int test = 0;
    int nimpact = 0;

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
    q0(0) = 0.9;
    q0(1) = -1.6;
    SP::SiconosVector z(new SimpleVector(nDof * 12));
    (*z)(0) = q0(0);
    (*z)(1) = q0(1);
    (*z)(2) = v0(0);
    (*z)(3) = v0(1);
    (*z)(4) = 0;
    (*z)(5) = 0;
    (*z)(6) = 0;
    (*z)(7) = 0;
    (*z)(8) = 0;
    (*z)(9) = 0;
    (*z)(10) = 0;
    (*z)(11) = PI;
    (*z)(12) = 0;
    (*z)(13) = 0;
    (*z)(14) = 0;
    (*z)(15) = 0;
    (*z)(16) = 0;
    (*z)(17) = 0;
    (*z)(22) = 0;
    (*z)(23) = 0;

    SP::LagrangianDS  arm(new LagrangianDS(q0, v0));

    // external plug-in
    arm->setComputeMassFunction("Two-linkPlugin.so", "mass");
    arm->setComputeNNLFunction("Two-linkPlugin.so", "NNL");
    arm->setComputeJacobianNNLFunction(1, "Two-linkPlugin.so", "jacobianVNNL");
    arm->setComputeJacobianNNLFunction(0, "Two-linkPlugin.so", "jacobianQNNL");
    arm->setComputeFIntFunction("Two-linkPlugin.so", "U");
    arm->setComputeJacobianFIntFunction(1, "Two-linkPlugin.so", "jacobFintV");
    arm->setComputeJacobianFIntFunction(0, "Two-linkPlugin.so", "jacobFintQ");
    arm->setZPtr(z);

    allDS.insert(arm);

    // -------------------
    // --- Interactions---
    // -------------------

    //  - one with Lagrangian non linear relation to define contact with ground
    //  Both with newton impact nslaw.

    InteractionsSet allInteractions;

    // -- relations --

    SP::NonSmoothLaw nslaw(new NewtonImpactNSL(e));
    SP::Relation relation(new LagrangianScleronomousR("Two-linkPlugin:h0", "Two-linkPlugin:G0"));
    SP::Interaction inter(new Interaction("floor-arm", allDS, 0, 2, nslaw, relation));


    SimpleMatrix H1(2, 2);
    SimpleVector b1(2);
    H1.zero();
    H1(0, 0) = -1;
    H1(1, 0) = 1;

    b1(0) = PI;
    b1(1) = 0;

    SP::NonSmoothLaw nslaw2(new NewtonImpactNSL(e2));
    SP::Relation relation1(new LagrangianLinearTIR(H1, b1));
    SP::Interaction inter1(new Interaction("floor-arm2", allDS, 1, 2, nslaw2, relation1));

    SimpleMatrix H2(2, 2);
    SimpleVector b2(2);
    H2.zero();
    H2(0, 1) = -1;
    H2(1, 1) = 1;

    b2(0) = 0.0001;
    b2(1) = PI - 0.0001;


    SP::Relation relation2(new LagrangianLinearTIR(H2, b2));
    SP::Interaction inter2(new Interaction("singular-points", allDS, 2, 2, nslaw2, relation2));

    allInteractions.insert(inter);
    allInteractions.insert(inter1);
    allInteractions.insert(inter2);
    // --------------------------------
    // --- NonSmoothDynamicalSystem ---
    // -------------------------------

    SP::NonSmoothDynamicalSystem nsds(new NonSmoothDynamicalSystem(allDS, allInteractions));

    // -------------
    // --- Model ---
    // -------------

    SP::Model Manipulator(new Model(t0, T));
    Manipulator->setNonSmoothDynamicalSystemPtr(nsds); // set NonSmoothDynamicalSystem of this model

    // ----------------
    // --- Simulation ---
    // ----------------

    // -- Time discretisation --
    SP::TimeDiscretisation t(new TimeDiscretisation(t0, h));

    SP::TimeStepping s(new TimeStepping(t));

    // -- OneStepIntegrators --
    SP::OneStepIntegrator OSI(new Moreau(arm, 0.500001));
    s->recordIntegrator(OSI);

    // -- OneStepNsProblem --
    IntParameters iparam(5);
    iparam[0] = 20000; // Max number of iteration
    DoubleParameters dparam(5);
    dparam[0] = 1e-5; // Tolerance
    string solverName = "Lemke" ;
    SP::NonSmoothSolver mySolver(new NonSmoothSolver(solverName, iparam, dparam));
    // -- OneStepNsProblem --
    SP::OneStepNSProblem osnspb(new LCP(mySolver));
    s->recordNonSmoothProblem(osnspb);
    // OneStepNSProblem  osnspb(new LCP(s,"name","Lemke",200001, 0.00001);

    cout << "=== End of model loading === " << endl;

    // =========================== End of model definition ===========================


    // ================================= Computation
    // --- Model initialization ---



    Manipulator->initialize(s);
    cout << "End of model initialisation" << endl;

    int k = 0;
    int N = (int)((T - t0) / h); // Number of time steps

    // --- Get the values to be plotted ---
    // -> saved in a matrix dataPlot
    unsigned int outputSize = 14;
    SimpleMatrix dataPlot(N + 1, outputSize);
    // For the initial time step:
    // time

    SP::SiconosVector q = arm->getQPtr();
    SP::SiconosVector v = arm->getVelocityPtr();
    SP::SiconosVector p = arm->getPPtr(2);
    // EventsManager * eventsManager = s->getEventsManagerPtr();

    dataPlot(k, 0) =  Manipulator->getT0();
    dataPlot(k, 1) = (*q)(0);
    dataPlot(k, 2) = (*q)(1);
    dataPlot(k, 3) = (inter->getY(0))(1);
    dataPlot(k, 4) = (*v)(0);
    dataPlot(k, 5) = (*v)(1);
    dataPlot(k, 6) = (inter->getY(0))(0) - 2;
    dataPlot(k, 7) = nimpact; //(inter->getY(1))(1);
    dataPlot(k, 8) = (*z)(6);
    dataPlot(k, 9) =  L; //(*z)(4);
    dataPlot(k, 10) = test;
    dataPlot(k, 11) = (*p)(1);
    dataPlot(k, 12) = (*z)(22);
    dataPlot(k, 13) = (*z)(23);

    cout << "====> Start computation ... " << endl << endl;

    while (k < N)
    {
      (*z)(0) = (*q)(0);
      (*z)(1) = (*q)(1);
      (*z)(2) = (*v)(0);
      (*z)(3) = (*v)(1);
      (*z)(16) = (*z)(14);
      (*z)(17) = (*z)(15);
      (*z)(20) = (*z)(18);
      (*z)(21) = (*z)(19);

      // get current time step
      k++;

      dataPlot(k, 0) =  s->getNextTime();
      dataPlot(k, 1) = (*q)(0);
      dataPlot(k, 2) = (*q)(1);
      dataPlot(k, 3) = (inter->getY(0))(1);
      dataPlot(k, 4) = (*v)(0);
      dataPlot(k, 5) = (*v)(1);
      dataPlot(k, 6) = (inter->getY(0))(0) - 2;
      dataPlot(k, 7) = nimpact; //(inter->getY(1))(1);
      dataPlot(k, 8) = (*z)(6);
      if (test == 3) dataPlot(k, 9) = (*z)(4) / h;
      else dataPlot(k, 9) = (*z)(4);
      dataPlot(k, 10) = test;
      dataPlot(k, 12) = (*z)(22);
      dataPlot(k, 13) = (*z)(23);

      s->newtonSolve(criterion, maxIter);
      dataPlot(k, 11) = (*p)(1);
      (*z)(4) = (inter->getLambda(1))(1);
      s->nextStep();

      //    controller during impacts accumulation phase before the first impact
      if ((dataPlot(k, 3) <= 0.01) && (test == 0) && (dataPlot(k, 6) < 0.6))
      {
        (*z)(8) = dataPlot(k, 0);
        (*z)(5) =  0.65 + 0.1 * cos(2 * PI * ((*z)(8)) / (*z)(11));
        (*z)(7) = (*z)(9);
        arm->setComputeFIntFunction("Two-linkPlugin.so", "U10");
        test = 1;
      }

      //  controller during impacts accumulation phase after the first impact
      if ((dataPlot(k, 11) > 0) && (test == 1))
      {
        (*z)(8) = dataPlot(k, 0);
        arm->setComputeFIntFunction("Two-linkPlugin.so", "U11");
        test = 2;
      }
      if ((dataPlot(k, 11) > 0) && (test == 2))
        nimpact = nimpact + 1;

      // controller during constraint-motion phase.
      if ((dataPlot(k, 11) > 0) && (test == 2) && (dataPlot(k, 7) - dataPlot(k - 1, 7) == 1)) // && (fabs((inter->getY(1))(1))<1e-8))
      {
        L = dataPlot(k, 0) - (*z)(8);
        (*z)(8) = dataPlot(k, 0);
        arm->setComputeFIntFunction("Two-linkPlugin.so", "U2");
        test = 3;
        nimpact = 0;
      }

      // change of control law with a particular design of the desired trajectory that guarantee the take-off
      if ((trunc((dataPlot(k, 0) + h) / (*z)(11)) > trunc((dataPlot(k, 0)) / (*z)(11))) && (test == 3))
      {
        (*z)(10) = dataPlot(k, 0) + h;
        (*z)(8) = (*z)(12);
        arm->setComputeFIntFunction("Two-linkPlugin.so", "U3");
        test = 4;
        L = 0;
      }

      //  controller during free-motion phase
      if (((*z)(13) - 0.1 >= 0) && (test == 4))
      {
        arm->setComputeFIntFunction("Two-linkPlugin.so", "U");
        test = 0;
        (*z)(13) = 0;
      }
    }
    cout << endl << "End of computation - Number of iterations done: " << k << endl;
    cout << "Computation Time " << time.elapsed()  << endl;
    // --- Output files ---
    ioMatrix out("result.dat", "ascii");
    out.write(dataPlot, "noDim");

  }

  catch (SiconosException e)
  {
    cout << e.report() << endl;
  }
  catch (...)
  {
    cout << "Exception caught in TwolinkManipulator" << endl;
  }

}
