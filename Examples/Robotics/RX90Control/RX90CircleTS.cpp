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


// =============================== Robot arm sample  ===============================
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
    unsigned int nDof = 6;           // degrees of freedom for robot arm
    double t0 = 0;                   // initial computation time
    double T = 6.0;                   // final computation time
    double h = 5e-3;                // time step
    double criterion = 1e-8;
    unsigned int maxIter = 200000;
    double e = 0.0;                  // nslaw

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
    q0(1) = PI / 3;
    q0(2) = -PI / 6;
    q0(4) = PI / 6;
    v0(0) = -0.34;
    v0(3) = 0.59;
    v0(5) = -0.34;


    SP::LagrangianDS arm(new LagrangianDS(q0, v0, "RX90Plugin:mass"));

    // external plug-in
    //    arm->setComputeMassFunction("RX90Plugin.so","mass");
    arm->setComputeNNLFunction("RX90Plugin.so", "NNL");
    arm->setComputeJacobianNNLFunction(1, "RX90Plugin.so", "jacobianVNNL");
    arm->setComputeJacobianNNLFunction(0, "RX90Plugin.so", "jacobianQNNL");
    arm->setComputeFIntFunction("RX90Plugin.so", "U");
    arm->setComputeJacobianFIntFunction(1, "RX90Plugin.so", "jacobFintV");
    arm->setComputeJacobianFIntFunction(0, "RX90Plugin.so", "jacobFintQ");

    // creating Z parameter computed in Actuators and used in FInt
    SP::SimpleVector torques(new SimpleVector(nDof));
    torques->zero();
    arm->setZPtr(torques);

    allDS.insert(arm);

    // -------------------
    // --- Interactions---
    // -------------------

    //  - one with linear relation to define joints limits
    //  Both with newton impact nslaw.

    InteractionsSet allInteractions;

    // -- relations --

    SP::NonSmoothLaw nslaw(new NewtonImpactNSL(e));

    SimpleMatrix H(12, 6);
    SimpleVector b(12);
    H.zero();
    for (unsigned int i = 0; i < nDof; i++)
    {
      H(2 * i, i) = 1;
      H(2 * i + 1, i) = -1;
    }
    b(0) = PI * 167.0 / 180.0;
    b(1) = b(0);
    b(2) = PI * 137.5 / 180.0;
    b(3) = b(2);
    b(4) = PI * 142.5 / 180.0;
    b(5) = b(4);
    b(6) = PI * 270.0 / 180.0;
    b(7) = b(6);
    b(8) = PI * 112.5 / 180.0;
    b(9) = b(8);
    b(10) = PI * 270.0 / 180.0;
    b(11) = b(10);
    SP::Relation relation(new LagrangianLinearTIR(H, b));
    SP::Interaction inter(new Interaction("butée", allDS, 0, 12, nslaw, relation));

    allInteractions.insert(inter);

    // -------------
    // --- Model ---
    // -------------

    SP::Model RX90(new Model(t0, T, allDS, allInteractions));

    // ----------------
    // --- Simulation ---
    // ----------------

    // -- Time discretisation --
    SP::TimeDiscretisation t(new TimeDiscretisation(t0, h));

    // -- Actuation (ici?) --
    //le premier evenement doit etre un evenement doit etre un evenement
    //de calcul, et pas de Actuators, ni sensors, c'est pourquoi le pas
    //de temps de la simu (h = 5e-3) est plus petit que le retard (6e-3)
    SP::TimeDiscretisation Sampling(new TimeDiscretisation(t0, 2 * h));

    unsigned int N = (unsigned int)((T - t0) / h + 1);
    SP::vector<double>tmp(new vector<double>(N));
    (*tmp)[0] = t0;
    for (unsigned int i = 1; i < tmp->size() - 1; i++)
      (*tmp)[i] = t0 + (i - 1) * 2 * h + 6e-3;
    (*tmp)[tmp->size() - 1] = T;

    SP::TimeDiscretisation delay(new TimeDiscretisation(*tmp, RX90));

    //Creation du control et ajout du Sensor et Actuator
    SP::ControlManager control(new ControlManager(RX90));
    control->addSensor(2, sampling);
    control->addActuator(2, delay);
    (*(control->getActuators().begin()))->addSensorPtr(*((control->getSensors()).begin()));

    SP::TimeStepping s(new TimeStepping(t));

    // -- OneStepIntegrators --
    SP::OneStepIntegrator OSI(new Moreau(arm, 0.5));
    s->recordIntegrator(OSI);

    IntParameters iparam(5);
    iparam[0] = 1000; // Max number of iteration
    DoubleParameters dparam(5);
    dparam[0] = 1e-15; // Tolerance
    string solverName = "Lemke" ;
    SP::NonSmoothSolver mySolver(new NonSmoothSolver(solverName, iparam, dparam));
    // -- OneStepNsProblem --
    SP::OneStepNSProblem osnsp(new LCP(mySolver));
    s->recordNonSmoothProblem(osnsp);

    cout << "=== End of model loading === " << endl;

    // =========================== End of model definition ===========================


    // ================================= Computation
    // --- Simulation initialization ---


    RX90->initialize(s);
    cout << "End of simulation initialisation" << endl;

    int k = 0;
    //    int N = t->getNSteps(); // Number of time steps

    // --- Get the values to be plotted ---
    // -> saved in a matrix dataPlot
    unsigned int outputSize = 13;
    SimpleMatrix dataPlot(N + 1, outputSize);
    // For the initial time step:
    SP::SiconosVector q = arm->q();
    SP::SiconosVector v = arm->velocity();
    SP::EventsManager eventsManager = s->eventsManager();

    dataPlot(k, 0) =  RX90->t0();
    dataPlot(k, 1) = (*q)(0);
    dataPlot(k, 2) = (*q)(1);
    dataPlot(k, 3) = (*q)(2);
    dataPlot(k, 4) = (*q)(3);
    dataPlot(k, 5) = (*q)(4);
    dataPlot(k, 6) = (*q)(5);
    dataPlot(k, 7) = (*v)(0);
    dataPlot(k, 8) = (*v)(1);
    dataPlot(k, 9) = (*v)(2);
    dataPlot(k, 10) = (*v)(3);
    dataPlot(k, 11) = (*v)(4);
    dataPlot(k, 12) = (*v)(5);


    while (s->nextTime() <= T)
    {
      s->advanceToEvent();
      s->processEvents();
      // get current time step
      if (abs(s->startingTime() - (k + 1)*h) < 1e-12)
      {
        k++;
        dataPlot(k, 0) =  s->startingTime();
        dataPlot(k, 1) = (*q)(0);
        dataPlot(k, 2) = (*q)(1);
        dataPlot(k, 3) = (*q)(2);
        dataPlot(k, 4) = (*q)(3);
        dataPlot(k, 5) = (*q)(4);
        dataPlot(k, 6) = (*q)(5);
        dataPlot(k, 7) = (*v)(0);
        dataPlot(k, 8) = (*v)(1);
        dataPlot(k, 9) = (*v)(2);
        dataPlot(k, 10) = (*v)(3);
        dataPlot(k, 11) = (*v)(4);
        dataPlot(k, 12) = (*v)(5);
      }
    }

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
    cout << "Exception caught in RX90Control" << endl;
  }
  cout << "Computation Time " << time.elapsed()  << endl;
}
