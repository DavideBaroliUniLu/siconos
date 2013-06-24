/* Siconos-sample , Copyright INRIA 2005-2012.
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

/* !\file SMCExampleImplicit.cpp
  \brief Two independent systems of dimension one controlled to slide
  on \f$x = 0\f$. An implicit scheme is used
  O. Huber
  */

#include "SiconosKernel.hpp"
#include <fenv.h>
using namespace std;

// main program
int main(int argc, char* argv[])
{

  //feenableexcept(FE_INEXACT);

  // User-defined parameters
  unsigned int ndof = 2;          // Number of degrees of freedom of your system
  double t0 = 0.0;                // Starting time
  double T = 1;                   // Total simulation time
  double h = 1.0e-4;              // Time step for simulation
  double hControl = 1.0e-2;       // Time step for control
  double Xinit = 1.0;
  double theta = 0.5;

  if (h > hControl)
  {
    RuntimeException::selfThrow("hControl must be bigger than h");
  }

  // ================= Creation of the model =======================
  // Steps:
  // - create a Dynamical System
  // - add a Simulation to the model

  // -------------------------
  // --- Dynamical systems ---
  // -------------------------

  // First System:
  // dx/dt = Ax + u(t) + r
  // x(0) = x0
  // Note: r = Blambda, B defines in relation below.

  // Matrix declaration
  // For the DynamycalSystem
  SP::SiconosMatrix A(new SimpleMatrix(ndof, ndof, 0));
  SP::SiconosVector x0(new SiconosVector(ndof));
  (*x0)(0) = Xinit;
  (*x0)(1) = -Xinit;
  // For the Sensor
  SP::SimpleMatrix sensorC(new SimpleMatrix(2, 2));
  sensorC->eye();
  // For the Actuator
  SP::SimpleMatrix Csurface(new SimpleMatrix(2, 2));
  Csurface->eye();
  SP::SimpleMatrix Brel(new SimpleMatrix(2, 2));
  Brel->eye();
  *(Brel) *= 2;
  SP::SimpleMatrix Drel(new SimpleMatrix(2, 2, 0));
  // Dynamical Systems
  SP::FirstOrderLinearDS processDS(new FirstOrderLinearDS(x0, A));
  processDS->setComputebFunction("RelayPlugin.so", "computeB");

  // -------------
  // --- Model process ---
  // -------------
  SP::Model process(new Model(t0, T));
  process->nonSmoothDynamicalSystem()->insertDynamicalSystem(processDS);

  // ------------------
  // --- Simulation ---
  // ------------------
  // TimeDiscretisation
  SP::TimeDiscretisation processTD(new TimeDiscretisation(t0, h));
  SP::TimeDiscretisation tSensor(new TimeDiscretisation(t0, hControl));
  SP::TimeDiscretisation tActuator(new TimeDiscretisation(t0, hControl));
  // == Creation of the Simulation ==
  SP::TimeStepping processSimulation(new TimeStepping(processTD, 0));
  processSimulation->setName("plant simulation");
  // -- OneStepIntegrators --
  SP::ZeroOrderHold processIntegrator(new ZeroOrderHold(processDS));
  processSimulation->insertIntegrator(processIntegrator);

  // Control stuff
  SP::ControlManager control(new ControlManager(process));
  // use a controlSensor
  SP::LinearSensor sens(new LinearSensor(tSensor, processDS, sensorC));
  control->addSensorPtr(sens);
  // add the sliding mode controller
  LinearSMC& act = *std11::static_pointer_cast<LinearSMC>(control->addActuator
                   (LINEAR_SMC, tActuator));
  act.setCsurfacePtr(Csurface);
  act.setBPtr(Brel);
  act.setDPtr(Drel);
  act.addSensorPtr(sens);

  // =========================== End of model definition ===========================

  // ================================= Computation =================================

  // --- Simulation initialization ---

  cout << "====> Simulation initialisation ..." << endl << endl;
  // initialise the process and the ControlManager
  process->initialize(processSimulation);
  control->initialize();

  // --- Get the values to be plotted ---
  unsigned int outputSize = 3; // number of required data
  unsigned int N = ceil((T - t0) / h) + 10; // Number of time steps

  SiconosVector& xProc = *processDS->x();
  // Save data in a matrix dataPlot
  SimpleMatrix dataPlot(N, outputSize);
  dataPlot(0, 0) = process->t0(); // Initial time of the model
  dataPlot(0, 1) = xProc(0);
  dataPlot(0, 2) = xProc(1);

  EventsManager& eventsManager = *processSimulation->eventsManager();

  // ==== Simulation loop =====
  cout << "====> Start computation ... " << endl << endl;
  int k = 0; // Current step
  // Simulation loop
  boost::progress_display show_progress(N);
  boost::timer time;
  time.restart();
  while (processSimulation->hasNextEvent())
  {
    eventsManager.display();
    Event& nextEvent = *eventsManager.nextEvent();
    if (nextEvent.getType() == TD_EVENT)
    {
      processSimulation->computeOneStep();
      k++;
      dataPlot(k, 0) = processSimulation->nextTime();
      dataPlot(k, 1) = xProc(0);
      dataPlot(k, 2) = xProc(1);
      ++show_progress;
    }
    processSimulation->nextStep();
  }
  cout << endl << "Computation Time " << time.elapsed()  << endl;
  eventsManager.display();

  // --- Output files ---
  cout << "====> Output file writing ..." << endl;
  dataPlot.resize(k, outputSize);
  ioMatrix::write("ExampleImplicit.dat", "ascii", dataPlot, "noDim");

  // Comparison with a reference file
  SimpleMatrix dataPlotRef(dataPlot);
  dataPlotRef.zero();
  ioMatrix::read("SMCExampleImplicit.ref", "ascii", dataPlotRef);
  std::cout << (dataPlot - dataPlotRef).normInf() << std::endl;
  if ((dataPlot - dataPlotRef).normInf() > 1e-12)
  {
    std::cout << "Warning. The results is rather different from the reference file." << std::endl;
    std::cout << (dataPlot - dataPlotRef).normInf() << std::endl;
    return 1;
  }
}
