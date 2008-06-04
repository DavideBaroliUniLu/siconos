/* Siconos-Example version 3.0.0, Copyright INRIA 2005-2008.
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
 * Foundation, Inc., 51 Franklin St, Fifth FLOOR, Boston, MA  02110-1301  USA
 *
 * Contact: Vincent ACARY vincent.acary@inrialpes.fr
 *
 */

#include "BallsModel.h"
#include "Draw_environment.h"
/**
   Balls simulation - 3D frictionl contact problem in presence of a rigid foundation

   The Siconos Model is built and initialized thanks to initSiconos() function.
   All related functions are in src/BallsModel.cpp

   The Simulation is saved in global variable GLOB_SIM

   The visualization is done thanks to drawstuff tools. For more details see the README file.

   16/05/2007- Authors: Houari Khenous, Roger Pissard, Franck Perignon

   Last modification 02/06/2008, H. Khenous


*/

using namespace std;

BallsModel::BallsModel(unsigned int n):
  numberOfFloors(n), numberOfSpheres(n), balls(NULL), nDof(6), dataPlot(NULL), iter_k(1)
{
  allSpheres.resize(numberOfSpheres);
}

BallsModel::~BallsModel()
{
  if (balls != NULL)
    delete balls;
  balls = NULL;
}

void BallsModel::initialize()
{

  // initial computation time
  double t0 = 0.0;
  // final computation time
  double T = 10.;
  // Default time step
  double h = 0.0005;
  // ================= Creation of the model =======================

  // ---------------------------------------
  // --- Build the Dynamical systems set ---
  // ---------------------------------------
  DynamicalSystemsSet allDS; // the list of DS
  buildDynamicalSystems();

  for (DSLIST::iterator it = allSpheres.begin(); it != allSpheres.end(); ++it)
    allDS.insert(*it);

  // -------------------
  // --- Interactions---
  // -------------------
  InteractionsSet allInteractions;
  buildInteractions(&allInteractions);

  // --------------------------------
  // --- NonSmoothDynamicalSystem ---
  // --------------------------------
  NonSmoothDynamicalSystem * nsds = new NonSmoothDynamicalSystem(allDS, allInteractions);

  // -------------
  // --- Model ---
  // -------------

  // initial computation time
  balls = new Model(t0, T);
  balls->setNonSmoothDynamicalSystemPtr(nsds); // set NonSmoothDynamicalSystem of this model

  // ----------------
  // --- Simulation ---
  // ----------------

  // -- Time-discretisation and Simulation --
  TimeDiscretisation * t = new TimeDiscretisation(h, balls);
  TimeStepping *s = new TimeStepping(t);

  // -- OneStepIntegrators --
  OneStepIntegrator * OSI;
  OSI = new Moreau(allDS , 0.5000001 , s);

  // -- OneStepNsProblem --
  string solverName = "NSGS";      // solver algorithm used for non-smooth problem
  IntParameters iparam(5);
  iparam[0] = 100010; // Max number of iteration
  // Solver/formulation
  // 0: projection, 1: Newton/AlartCurnier, 2: Newton/Fischer-Burmeister, 3: Path/Glocker, 4:Projection_modifiee, 5:Fixed Point/Glocker, 6: ProjectionOnConeWithLocalIteration
  iparam[4] = 5;
  DoubleParameters dparam(5);
  dparam[0] = 1e-7; // Tolerance
  NonSmoothSolver * Mysolver = new NonSmoothSolver(solverName, iparam, dparam);
  FrictionContact* osnspb = new FrictionContact(s, 3, Mysolver);
  osnspb->setNumericsVerboseMode(0);
  cout << "=== End of model loading === " << endl;

  // =========================== End of model definition

  // --- Simulation initialization ---

  cout << "..." << endl;
  s->initialize();
  cout << "=== End of simulation initialisation ===" << endl;

  unsigned int N = int((T - t0) / h);
  unsigned int outputSize = 1 + 2 * numberOfSpheres;
  // Output matrix
  dataPlot = new SimpleMatrix(N + 1, outputSize);

  (*dataPlot)(0, 0) = t0; // time
  unsigned int i = 0;
  for (DSLIST::iterator it = allSpheres.begin(); it != allSpheres.end(); ++it)
  {
    (*dataPlot)(0, 1 + 2 * i) = (*it)->getQ(2); // q(2)
    (*dataPlot)(0, 2 + 2 * i) = (*it)->getVelocity(2); // v(2)
    i++;
  }
}

void BallsModel::compute()
{
  balls->getSimulationPtr()->advanceToEvent();
  unsigned int i = 0;

  for (DSLIST::iterator it = allSpheres.begin(); it != allSpheres.end(); ++it)
  {
    (*dataPlot)(iter_k, 1 + 2 * i) = (*it)->getQ(2); // q(2)
    (*dataPlot)(iter_k, 2 + 2 * i) = (*it)->getVelocity(2); // v(2)
    i++;
  }
  balls->getSimulationPtr()->processEvents();
  iter_k++;
}

bool BallsModel::isSimulationFinished()
{
  return !(balls->getSimulationPtr()->getNextTime() < balls->getFinalT());
}

void BallsModel::draw()
{
  for (DSLIST::iterator it = allSpheres.begin(); it != allSpheres.end(); ++it)
    (*it)->draw();
}

void BallsModel::computeInitialPositions(Vectors q0, Vectors v0, double Radius)
{

  //    // set values
  (*(q0[0]))(0) =  0.0;
  (*(q0[0]))(1) =  0.3;
  (*(q0[0]))(2) =  0.35;
  (*(q0[1]))(0) =  0.0;
  (*(q0[1]))(1) =  0.3;
  (*(q0[1]))(2) =  0.12;

  //     (*(q0[0]))(0) =  0.0;    (*(q0[0]))(1) =  0.;  (*(q0[0]))(2) =  0.12;
  //     (*(q0[1]))(0) =  0.0;    (*(q0[1]))(1) =  0.3;  (*(q0[1]))(2) =  0.12;

  //   (*(q0[0]))(0) =  0.0;    (*(q0[0]))(1) =  0.4;  (*(q0[0]))(2) =  0.1;
  //     (*(v0[0]))(0) =  0.;    (*(v0[0]))(1) =  -1.;  (*(v0[0]))(2) =  0.;

  //     (*(q0[1]))(0) =  0.0;    (*(q0[1]))(1) =  0.;  (*(q0[1]))(2) =  0.1;
  //     (*(v0[1]))(0) =  0.;    (*(v0[1]))(1) =  1.;  (*(v0[1]))(2) =  0.;

  //     (*(q0[2]))(0) =  0.;    (*(q0[2]))(1) =  0.8;  (*(q0[2]))(2) =  0.1;
  //     (*(q0[3]))(0) =  0.;    (*(q0[3]))(1) =  -0.5;  (*(q0[3]))(2) =  0.1;


  //   (*(q0[0]))(0) =  0.0;    (*(q0[0]))(1) =  0.0;  (*(q0[0]))(2) =  0.2;
  //      (*(q0[1]))(0) =  0.0;    (*(q0[1]))(1) =  0.;  (*(q0[1]))(2) =  0.5;
  //      (*(v0[1]))(3) =  10.;
  //      (*(q0[2]))(0) =  0.0;    (*(q0[2]))(1) =  0.;  (*(q0[2]))(2) =  0.6;


  // carreau
  //  (*(q0[0]))(0) =  0.0;    (*(q0[0]))(1) =  0.0;  (*(q0[0]))(2) =  0.1;
  //       (*(q0[1]))(0) =  0.5;    (*(q0[1]))(1) =  0.5;  (*(q0[1]))(2) =  0.3;
  //       (*(v0[1]))(0) = -1.;    (*(v0[1]))(1) = -1.;  (*(v0[1]))(2) =  1.3;
}

void BallsModel::buildDynamicalSystems()
{
  // Set the same radius and mass for all balls
  double Radius = 0.1;
  double m = 1.0;

  // -- Initial positions and velocities --
  // q0[i] and v0[i] correspond to position and velocity of ball i.

  Vectors q0, v0;
  q0.resize(numberOfSpheres, NULL);
  v0.resize(numberOfSpheres, NULL);

  // Memory allocation for q0[i] and v0[i]
  for (unsigned int i = 0; i < numberOfSpheres; i++)
  {
    q0[i] = new SimpleVector(nDof);
    v0[i] = new SimpleVector(nDof);
  }
  /*******************************************************************************/
  // Computation of the position of all beads of the pyramid
  computeInitialPositions(q0, v0, Radius);

  //  // External forces = gravity
  //   SimpleVector * weight = new SimpleVector(nDof);
  //   (*weight)(2) = -m*g;

  // Build and insert the DS into allDS

  for (unsigned int i = 0; i < numberOfSpheres; i++)
    allSpheres[i] = new Sphere(Radius, m, *(q0[i]), *(v0[i]), i);

}


void BallsModel::buildInteractions(InteractionsSet* allInteractions)
{
  // Definition of some obstacles

  // A ceiling (z=TOP)
  bool hasCeil = false;
  // A ground (z=Ground)
  bool hasGround = true;
  // Some walls
  bool obst_y_p = true;                    //  for y --> +
  bool obst_y_m = true;                    //  for y --> -
  bool obst_x_p = true;                    //  for x --> +
  bool obst_x_m = true;                    //  for x --> -

  int Fact = (numberOfSpheres) * (numberOfSpheres - 1) / 2;
  vector<Relation*> LLR(Fact);
  vector<Relation*> LLR1(numberOfSpheres);
  vector<Relation*> LLR1_(numberOfSpheres);
  vector<Relation*> LLR2(numberOfSpheres);
  vector<Relation*> LLR2_(numberOfSpheres);
  vector<Relation*> LLR3(numberOfSpheres);
  vector<Relation*> LLR3_(numberOfSpheres);

  double Radius = 0.1;
  double e  = 0.9;
  double mu = 0.1;
  NonSmoothLaw * nslaw1 = new NewtonImpactFrictionNSL(e, e, mu, 3);

  SimpleVector bground(3), bceil(3), bwallYp(3), bwallYm(3), bwallXp(3), bwallXm(3);
  SimpleMatrix Hground(3, nDof), Hceil(3, nDof), HwallYp(3, nDof), HwallYm(3, nDof), HwallXp(3, nDof), HwallXm(3, nDof) ;
  if (hasGround)
  {
    bground(0) = GROUND - Radius;
    Hground(0, 2) = 1.0;
    Hground(1, 0) = 1.0;
    Hground(1, 4) = -Radius;
    Hground(2, 1) = 1.0;
    Hground(2, 3) =  Radius;
  }
  // Interaction beads and ceiling
  if (hasCeil)
  {
    bceil(0) = TOP - Radius;
    Hceil(0, 2) = -1.0;
    Hceil(1, 0) = 1.0;
    Hceil(1, 4) = -Radius;
    Hceil(2, 1) = 1.0;
    Hceil(2, 3) =  Radius;
  }
  if (obst_y_p)
  {
    bwallYp(0) = WALL - Radius;
    HwallYp(0, 1) = 1.0;
    HwallYp(1, 0) = 1.0;
    HwallYp(1, 5) = -Radius;
    HwallYp(2, 2) = 1.0;
    HwallYp(2, 3) =  Radius;
  }
  if (obst_y_m)
  {
    bwallYm(0) = WALL - Radius;
    HwallYm(0, 1) = -1.0;
    HwallYm(1, 0) = 1.0;
    HwallYm(1, 5) = -Radius;
    HwallYm(2, 2) = 1.0;
    HwallYm(2, 3) =  Radius;
  }
  if (obst_x_p)
  {
    bwallXp(0) = WALL - Radius;
    HwallXp(0, 0) = 1.0;
    HwallXp(1, 1) = 1.0;
    HwallXp(1, 5) = -Radius;
    HwallXp(2, 2) = 1.0;
    HwallXp(2, 4) =  Radius;
  }
  if (obst_x_m)
  {
    bwallXm(0) = WALL - Radius;
    HwallXm(0, 0) = -1.0;
    HwallXm(1, 1) = 1.0;
    HwallXm(1, 5) = -Radius;
    HwallXm(2, 2) = 1.0;
    HwallXm(2, 4) =  Radius;
  }


  // All Dynamical Systems can interact with ground/ceil/walls
  //  for (unsigned int i=0;i<numberOfSpheres;i++)
  //    {
  // Interaction beads and ground (z=0)
  for (unsigned int i = 0; i < numberOfSpheres; i++)
  {
    if (hasGround)
    {
      LLR1[i] = new LagrangianLinearR(Hground, bground);
      allInteractions->insert(new Interaction(allSpheres[i], i, 3, nslaw1, LLR1[i]));
    }
    // Interaction beads and ceiling
    if (hasCeil)
    {
      LLR1_[i] = new LagrangianLinearR(Hceil, bceil);
      allInteractions->insert(new Interaction(allSpheres[i], i, 3, nslaw1, LLR1_[i]));
    }
    // Interaction beads and plan2 (OXZ)
    if (obst_y_p)
    {
      LLR2[i] = new LagrangianLinearR(HwallYp, bwallYp);
      allInteractions->insert(new Interaction(allSpheres[i], i, 3, nslaw1, LLR2[i]));
    }
    // Interaction beads and plan2 (-ZOX)
    if (obst_y_m)
    {
      LLR2_[i] = new LagrangianLinearR(HwallYm, bwallYm);
      allInteractions->insert(new Interaction(allSpheres[i], i, 3, nslaw1, LLR2_[i]));
    }
    // Interaction beads and plan3 (OYZ)
    if (obst_x_p)
    {
      LLR3[i] = new LagrangianLinearR(HwallXp, bwallXp);
      allInteractions->insert(new Interaction(allSpheres[i], i, 3, nslaw1, LLR3[i]));
    }
    // Interaction beads and plan3 (-ZOY)
    if (obst_x_m)
    {
      LLR3_[i] = new LagrangianLinearR(HwallXm, bwallXm);
      allInteractions->insert(new Interaction(allSpheres[i], i, 3, nslaw1, LLR3_[i]));
    }
  }

  // Interaction between beads

  // frictional contact condition between beads
  double e2 = 0.9;
  NonSmoothLaw * nslaw2 = new NewtonImpactFrictionNSL(e2, e2, mu, 3);
  unsigned int l = 0;
  DynamicalSystemsSet dsConcerned;
  for (unsigned int i = 0; i < numberOfSpheres; i++)
  {
    dsConcerned.insert(allSpheres[i]);
    for (unsigned int j = 0; j < numberOfSpheres; j++)
    {
      if (j > i)
      {
        dsConcerned.insert(allSpheres[j]);
        LLR[l] = new LagrangianScleronomousR("BallsPlugin:h0", "BallsPlugin:G0");
        allInteractions->insert(new Interaction(dsConcerned, l, 3, nslaw2, LLR[l]));
        dsConcerned.erase(allSpheres[j]);
        l++;
      }
    }
    dsConcerned.clear();
  }

}

void BallsModel::end()
{
  cout << "End of computation - Number of iterations done: " << iter_k << endl;
  // --- Output files ---
  ioMatrix io("result.dat", "ascii");
  io.write(*dataPlot, "noDim");
  cout << "Hit Enter to close graph window and stop the program." << endl;
  getchar();
}
