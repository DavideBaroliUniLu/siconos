
#include "SiconosKernel.hpp"
#include "adjointInput.h"
#include "myDS.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std;

/************************************************************/
/************************************************************/
/************************************************************/
/*call back for the source*/
/*call back for the formulation with inversion*/
//void (bLDS) (double t, unsigned int N, double* b, unsigned int z, double*zz){
//}


/************************************************************/
/************************************************************/
/************************************************************/
/************************************************************/
/*main program*/

static int sNSLawSize = 2;

int main()
{
  string solverDirEnum = "DIRECT_ENUM" ;
  string solverDirPath = "DIRECT_PATH" ;
  string solverDirSimplex = "DIRECT_SIMPLEX" ;
  string solverEnum = "ENUM" ;
  string solverSimplex = "SIMPLEX" ;
  string solverPath = "PATH" ;
  string * solverName = 0;
  bool diodeIsOn;
  bool switchIsOn;
  bool stateChanged;
  // One Step non smooth problem
  IntParameters iparam(10);
  DoubleParameters dparam(10);

  double* floatWorkingMem = 0;
  int * intWorkingMem = 0;

  int freq = 1000;
  int Nfreq = 0;
  int cmp = 0;

  int NbDataMax = 10000;
  int NData = 0;

  /************************************************************/
  /************************************************************/
  /*Solver options*/
  iparam[5] = 5; // nb config
  iparam[0] = 0; // verbose
  dparam[0] = 1e-11; // Tolerance
  solverName = &solverEnum;


  int dimX = 4;
  SimpleMatrix * M = 0;
  SimpleMatrix * A = 0;
  SimpleVector* x0 = 0;
  SimpleVector* As = 0;
  SimpleVector* mti = 0;


  x0 = new SimpleVector(dimX);

  x0->setValue(0, 3.5481);
  x0->setValue(1, -1.5621);
  x0->setValue(2, 2.2862);
  x0->setValue(3, -0.6226);



  double sT = 0.5;
  double sStep = 0.1e-2;
  int NBStep = (int) floor(sT / sStep);

  //  NBStep = 3;
  //*****BUILD THE DYNAMIC SYSTEM
  SP::MyDS aDS ;
  aDS.reset(new MyDS(*x0));

  DynamicalSystemsSet  Inter_DS ;
  Inter_DS.insert(aDS);

  //******BUILD THE RELATION
  SimpleMatrix* C = 0;
  SimpleMatrix* D = 0;
  SimpleMatrix* B = 0;
  SP::adjointInput aR;
  aR.reset(new adjointInput());

  int sN = 2;
  int sM = 0;


  //*****BUILD THE NSLAW
  SP::NonSmoothLaw aNSL;
  aNSL.reset(new ComplementarityConditionNSL(sN));





  //****BUILD THE INTERACTION
  SP::Interaction aI(new Interaction("LCP", Inter_DS, 1, sNSLawSize, aNSL, aR));
  //****BUILD THE SYSTEM
  SP::NonSmoothDynamicalSystem  aNSDS(new NonSmoothDynamicalSystem(aDS, aI));

  SP::Model  aM(new Model(0, sT));
  aM->setNonSmoothDynamicalSystemPtr(aNSDS);
  SP::TimeDiscretisation  aTD(new TimeDiscretisation(0, sStep));
  SP::TimeStepping aS(new TimeStepping(aTD));
  aS->setComputeResiduY(true);
  aS->setUseRelativeConvergenceCriteron(false);
  //*****BUILD THE STEP INTEGRATOR
  SP::OneStepIntegrator  aMoreau ;
  aMoreau.reset(new Moreau(aDS, 0.5));
  aS->insertIntegrator(aMoreau);
  SP::NonSmoothSolver  mySolver(new NonSmoothSolver((*solverName), iparam, dparam, floatWorkingMem, intWorkingMem));

  //**** BUILD THE STEP NS PROBLEM
  SP::LCP  aLCP ;
  aLCP.reset(new LCP(mySolver, "LCP"));
  aS->insertNonSmoothProblem(aLCP);
  aM->initialize(aS);

  //      setNumericsVerbose(1);


  SP::SiconosVector  x = aDS->x();
  SP::SiconosVector  y = aI->y(0);
  SP::SiconosVector  lambda = aI->lambda(0);
  ofstream * fout = new ofstream("simu.log");


  unsigned int outputSize = 10; // number of required data
  SimpleMatrix dataPlot(NBStep, outputSize);

  SP::SiconosVector z = aDS->x();
  SP::SiconosVector lambdaOut = aI->lambda(0);
  SP::SiconosVector yOut = aI->y(0);

  dataPlot(0, 0) = aM->t0(); // Initial time of the model
  dataPlot(0, 1) = (*z)(0);
  dataPlot(0, 2) = (*z)(1);
  dataPlot(0, 3) = (*z)(2);
  dataPlot(0, 4) = (*z)(3);
  dataPlot(0, 5) = (*lambdaOut)(0);
  dataPlot(0, 6) = (*lambdaOut)(1);
  dataPlot(0, 7) = (*y)(0);
  dataPlot(0, 8) = (*y)(1);




  unsigned int count = 0; // events counter.
  // do simulation while events remains in the "future events" list of events manager.
  cout << " ==== Start of  simulation : " << NBStep << " steps====" << endl;

  for (int k = 0 ; k < NBStep ; k++)
  {

    cout << " ==== Step: " << endl;
    //      if (cmp==150)
    setNumericsVerbose(1);
    //      else if (cmp==151)
    //        setNumericsVerbose(0);
    cout << "..." << cmp << endl;
    cmp++;
    // solve ...
    /*aS->computeOneStep();*/

    aS-> newtonSolve(1e-4, 20);
    aS->nextStep();
    x = aDS->x();
    lambda = aI->lambda(0);
    dataPlot(k, 0) = aS->nextTime(); // Initial time of the model
    dataPlot(k, 1) = (*z)(0);
    dataPlot(k, 2) = (*z)(1);
    dataPlot(k, 3) = (*z)(2);
    dataPlot(k, 4) = (*z)(3);
    dataPlot(k, 5) = (*lambdaOut)(0);
    dataPlot(k, 6) = (*lambdaOut)(1);
    dataPlot(k, 7) = (*yOut)(0);
    dataPlot(k, 8) = (*yOut)(1);


  }


  // --- Output files ---
  cout << "====> Output file writing ..." << endl;
  ioMatrix io("OptimalControl.dat", "ascii");
  io.write(dataPlot, "noDim");

  delete fout;
  cout << "===== End of simulation. ==== " << endl;
  return 0;

}
