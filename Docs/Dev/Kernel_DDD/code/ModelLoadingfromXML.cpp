// Default Constructor of a Model
Model model1;
// Constructor of a nsds with minimal data
NonSmoothDynamicalSystem nsds1(false);
// Set the NonSmoothDynamicalSystem of the Model
model1.setNonSmoothDynamicalSystem(nsds1) ;

SimpleVector q0(3);
q0.zero();
q0(0) = 1.0;
SimpleVector v0(3);
v0.zero();
SiconosMatrix mass(3, 3);
mass.eye();
SiconosMatrix K(3, 3);
K.zero();
SiconosMatrix C(3, 3);
C.zero();

// Constructor of a LagrangianLinearTIDS with minimal data
LagrangianLinearTIDS lltids1(1, 3, &q0, &v0, &mass,
                             "BasicPlugin:FExt", &K, &C);

nsds1.addDS(&lltids1);
