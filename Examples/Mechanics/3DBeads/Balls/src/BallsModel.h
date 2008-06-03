/* Siconos-Kernel version 3.0.0, Copyright INRIA 2005-2008.
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

/*! \file BallsModel.h
  \brief Model for build/sim and drawing of Spheres - Inherits from Model
*/

#ifndef BallsModel_H
#define BallsModel_H

#include "SiconosKernel.h"
#include "Sphere.h"
typedef std::vector<SimpleVector*> Vectors;
typedef std::vector<Sphere*> DSLIST;

/** CanonBallsModel

   \author H. Khenous, F. Perignan
   \version 3.0.0.
   \date (Creation) May 2008



 */
class BallsModel
{
private:

  /** Number of floors of beads */
  unsigned int numberOfFloors;

  /** Total Number of Spheres */
  unsigned int numberOfSpheres;

  /** The Siconos Model */
  Model* balls;

  unsigned int nDof;

  SimpleMatrix * dataPlot;

  unsigned int iter_k;

  /** */
  DSLIST allSpheres;

public:

  /** Default Constructor
   */
  BallsModel(unsigned int);

  /** destructor
   */
  ~BallsModel();

  inline Model* getModelPtr()
  {
    return balls;
  };

  inline DSLIST getDSLIst()
  {
    return allSpheres;
  };

  /** Build and initialize the Model(NonSmoothDynamicalSystem and Simulation)
   */
  void initialize();

  /** Prepare output
   */
  void initializeOutput();

  /** Run the simulation
   */
  void compute();

  bool isSimulationFinished();

  /** Draw
   */
  void draw();


  /**
   */
  void computeInitialPositions(Vectors, Vectors, double);

  void buildDynamicalSystems();

  void buildInteractions(InteractionsSet* allInteractions);

  void end();

  inline unsigned int getIter()
  {
    return iter_k;
  };

};

#endif
