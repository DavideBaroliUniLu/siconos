/* Siconos-sample version 1.2.0, Copyright INRIA 2005-2006.
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
//-----------------------------------------------------------------------
//
//  DiodeBridge  : sample of an electrical circuit involving :
//  - a linear dynamical system consisting of an LC oscillator (1 �F , 10 mH)
//  - a non smooth system (a 1000 Ohm resistor supplied through a 4 diodes bridge) in parallel
//    with the oscillator
//
//  Expected behavior :
//  The initial state (Vc = 10 V , IL = 0) of the oscillator provides an initial energy.
//  The period is 2 Pi sqrt(LC) ~ 0,628 ms.
//      The non smooth system is a full wave rectifier :
//  each phase (positive and negative) of the oscillation allows current to flow
//  through the resistor in a constant direction, resulting in an energy loss :
//  the oscillation damps.
//
//  State variables :
//  - the voltage across the capacitor (or inductor)
//  - the current through the inductor
//
//  Since there is only one dynamical system, the interaction is defined by :
//  - complementarity laws between diodes current and voltage. Depending on
//        the diode position in the bridge, y stands for the reverse voltage across the diode
//    or for the diode current (see figure in the template file)
//  - a linear time invariant relation between the state variables and
//    y and lambda (derived from Kirchhoff laws)
//
//-----------------------------------------------------------------------

#include <Model.h>
#include <LinearDS.h>
#include <LinearTIR.h>
#include <TimeStepping.h>
#include <Moreau.h>
#include <LCP.h>
#include <math.h>
#include <sys/time.h>
#include <iostream>

using namespace std;
bool DiodeBridge()
{

  cout << " **************************************" << endl;
  cout << " ******** Start diode bridge *********" << endl << endl << endl;
  double t0 = 0.0;
  double T = 5e-3;        // Total simulation time
  double h_step = 1.0e-6;  // Time step
  double Lvalue = 1e-2;   // inductance
  double Cvalue = 1e-6;   // capacitance
  double Rvalue = 1e3;    // resistance
  double Vinit = 10.0;    // initial voltage
  string Modeltitle = "DiodeBridge";
  string solverName = "NSQP"; // non smooth problem solver algo name.

  bool res = false;

  try
  {

    // --- Dynamical system specification ---
    SimpleVector init_state(2);
    init_state.setValue(0 , Vinit);
    init_state.setValue(1 , 0.0);

    SimpleMatrix LS_A(2, 2);
    LS_A.setValue(0 , 0 , 0.0);

    LS_A.setValue(0 , 1 , -1.0 / Cvalue);
    LS_A.setValue(1 , 0 , 1.0 / Lvalue);
    LS_A.setValue(1 , 1 , 0.0);

    LinearDS* LSDiodeBridge = new LinearDS(1, init_state, LS_A);

    // --- Interaction between linear system and non smooth system ---

    DSSet Inter_DS;
    Inter_DS.insert(LSDiodeBridge);

    SiconosMatrix* Int_C = new SimpleMatrix(4, 2);
    Int_C->zero();
    Int_C->setValue(2 , 0 , -1.0);
    Int_C->setValue(3 , 0 , 1.0);

    SiconosMatrix* Int_D = new SimpleMatrix(4, 4);
    Int_D->zero();
    Int_D->setValue(0 , 0 , 1.0 / Rvalue);
    Int_D->setValue(0 , 1 , 1.0 / Rvalue);
    Int_D->setValue(1 , 0 , 1.0 / Rvalue);
    Int_D->setValue(1 , 1 , 1.0 / Rvalue);
    Int_D->setValue(0 , 2 , -1.0);
    Int_D->setValue(1 , 3 , -1.0);
    Int_D->setValue(2 , 0 , 1.0);
    Int_D->setValue(3 , 1 , 1.0);

    SiconosMatrix* Int_B = new SimpleMatrix(2, 4);
    Int_B->zero();
    Int_B->setValue(0 , 2 , -1.0 / Cvalue);
    Int_B->setValue(0 , 3 , 1.0 / Cvalue);

    Interaction* InterDiodeBridge = new Interaction("InterDiodeBridge", Inter_DS, 1, 4);

    LinearTIR* LTIRDiodeBridge = new LinearTIR(*Int_C, *Int_B, InterDiodeBridge);
    LTIRDiodeBridge->setDPtr(Int_D);

    InterDiodeBridge->setRelationPtr(LTIRDiodeBridge);
    InterDiodeBridge->createComplementarityConditionNSL();

    // --- Dynamical system creation ---
    InteractionsSet allInteractions;
    allInteractions.insert(InterDiodeBridge);
    NonSmoothDynamicalSystem* NSDSDiodeBridge = new NonSmoothDynamicalSystem(Inter_DS, allInteractions, false);

    // --- Model creation ---
    Model DiodeBridge(t0, T, Modeltitle);
    DiodeBridge.setNonSmoothDynamicalSystemPtr(NSDSDiodeBridge);

    // --- Strategy specification---

    TimeStepping* StratDiodeBridge = new TimeStepping(DiodeBridge);

    TimeDiscretisation* TiDiscRLCD = new TimeDiscretisation(h_step, StratDiodeBridge);

    double theta = 1;
    Moreau* OSI_RLCD = new Moreau(LSDiodeBridge, theta, StratDiodeBridge);

    set<OneStepIntegrator*> vOSI_RLCD;
    vOSI_RLCD.insert(OSI_RLCD);
    StratDiodeBridge->setOneStepIntegrators(vOSI_RLCD);
    LCP* LCP_RLCD = new LCP(StratDiodeBridge, solverName, 101, 0.0001, "max", 0.6);

    StratDiodeBridge->setOneStepNSProblemPtr(LCP_RLCD);
    StratDiodeBridge->initialize();
    int k = TiDiscRLCD->getK(); // Current step
    int N = TiDiscRLCD->getNSteps(); // Number of time steps

    // --- Get the values to be plotted ---
    // -> saved in a matrix dataPlot
    SimpleMatrix dataPlot(N + 1, 7);

    // For the initial time step:

    // time
    dataPlot(k, 0) = k * h_step;

    // inductor voltage
    dataPlot(k, 1) = (LSDiodeBridge->getX())(0);

    // inductor current
    dataPlot(k, 2) = (LSDiodeBridge->getX())(1);

    // diode R1 current
    dataPlot(k, 3) = (InterDiodeBridge->getY(0))(0);

    // diode R1 voltage
    dataPlot(k, 4) = -(InterDiodeBridge->getLambda(0))(0);

    // diode F2 voltage
    dataPlot(k, 5) = -(InterDiodeBridge->getLambda(0))(1);

    // diode F1 current
    dataPlot(k, 6) = (InterDiodeBridge->getLambda(0))(2);
    // --- Time loop  ---
    while (k < N)
    {
      StratDiodeBridge->nextStep();

      // get current time step
      k = TiDiscRLCD->getK();

      // solve ...
      StratDiodeBridge->computeFreeState();

      StratDiodeBridge->computeOneStepNSProblem();
      // update
      StratDiodeBridge->update();

      // --- Get values to be plotted ---
      // time
      dataPlot(k, 0) = k * h_step;

      // inductor voltage
      dataPlot(k, 1) = (LSDiodeBridge->getX())(0);

      // inductor current
      dataPlot(k, 2) = (LSDiodeBridge->getX())(1);

      // diode R1 current
      dataPlot(k, 3) = (InterDiodeBridge->getY(0))(0);

      // diode R1 voltage
      dataPlot(k, 4) = -(InterDiodeBridge->getLambda(0))(0);

      // diode F2 voltage
      dataPlot(k, 5) = -(InterDiodeBridge->getLambda(0))(1);

      // diode F1 current
      dataPlot(k, 6) = (InterDiodeBridge->getLambda(0))(2);
    }

    // dataPlot (ascii) output
    SiconosMatrix * dataRef = new SimpleMatrix("refDiodeBridge.dat", true);

    double tol = 1e-9;
    double norm = (dataPlot - (*dataRef)).normInf() ;
    cout << endl << endl;
    if (norm < tol)
    {
      cout << " ******** DiodeBridge test ended with success ********" << endl;
      res = true;
    }
    else
    {
      cout << " ******** DiodeBridge test failed, results differ from those of reference file. ********" << endl;
      res = false;
    }

    cout << endl << endl;

    delete dataRef;
    delete LCP_RLCD;
    delete OSI_RLCD;
    delete TiDiscRLCD;
    delete StratDiodeBridge;
    delete LTIRDiodeBridge;
    delete InterDiodeBridge;
    delete Int_B ;
    delete Int_D ;
    delete Int_C;
    delete LSDiodeBridge;
    delete NSDSDiodeBridge;

  }

  // --- Exceptions handling ---
  catch (SiconosException e)
  {
    cout << e.report() << endl;
  }
  catch (...)
  {
    cout << "Exception caught " << endl;
  }

  return res;

}
