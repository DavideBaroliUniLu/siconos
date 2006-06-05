/* Siconos-Kernel version 1.2.0, Copyright INRIA 2005-2006.
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
#ifndef __XMLTAGSNAME__
#define __XMLTAGSNAME__

#include<string>

/*
 * the different kind of attribute we can encounter
 */
const std::string TYPE_ATTRIBUTE = "type";
const std::string ID_ATTRIBUTE = "Id";
const std::string NUMBER_ATTRIBUTE = "number";
const std::string PLUGIN_ATTRIBUTE = "plugin";
const std::string ALL_ATTRIBUTE = "all";
const std::string SIZE_ATTRIBUTE = "size";
// usefull when a list of objects, identified thanks to numbers, is required;
// for example a list of DS in an interaction:
const std::string INDEX_LIST = "indexList";


// common tags
const std::string COMPUTE_INPUT_TAG = "computeInput";
const std::string COMPUTE_OUTPUT_TAG = "computeOutput";

/*
 * the different node names we can encounter
 */
// BoundaryCondition tags
const std::string LINEARBC_TAG = "Linear";
const std::string NON_LINEARBC_TAG = "NLinear";
const std::string PERIODICBC_TAG = "Periodic";

// DynamicalSystem tags
const std::string LAGRANGIAN_TIDS_TAG = "LagrangianLinearTIDS";
const std::string LAGRANGIAN_NON_LINEARDS_TAG = "LagrangianDS";
const std::string LINEAR_DS_TAG = "LinearDS";
const std::string LINEAR_TIDS_TAG = "LinearTIDS";
const std::string NON_LINEAR_DS_TAG = "NonLinearDS";

const std::string BOUNDARYCONDITION_TAG = "BoundaryCondition";
const std::string INTERACTION_TAG = "Interaction";
const std::string INTERACTION_CONTENT_TAG = "Interaction_Content";
const std::string RELATION_CONTENT_TAG = "Relation_Content";

// NSDS
const std::string DYNAMICAL_SYSTEM_TAG = "DS";
const std::string NON_SMOOTH_DYNAMICAL_SYSTEM_TAG = "NSDS";
const std::string DSINPUTOUTPUT_TAG = "DSInputOutput";
const std::string EQUALITYCONSTRAINT_TAG = "EqualityConstraint";

const std::string EQUALITYCONSTRAINT_DEFINITION_TAG = "EqualityConstraint_Definition";
const std::string DSINPUTOUTPUT_DEFINITION_TAG = "DSInputOutput_Definition";
const std::string DYNAMICAL_SYSTEM_DEFINITION_TAG = "DS_Definition";
const std::string INTERACTION_DEFINITION_TAG = "Interaction_Definition";
const std::string LMGC90_NSDS_TAG = "DS_LMGC90";
const std::string LMGC90_STRATEGY_TAG = "OneStepIntegrator_LMGC90";


// Interaction
// - Relations -
const std::string RELATION_TAG = "Relation";
const std::string LINEAR_TIME_INVARIANT_RELATION_TAG = "LinearTimeInvariantRelation";
const std::string LAGRANGIAN_RELATION_TAG = "LagrangianRelation";
const std::string LAGRANGIAN_LINEAR_RELATION_TAG = "LagrangianLinearRelation";
// - Non-smooth laws -
const std::string COMPLEMENTARITY_CONDITION_NSLAW_TAG = "ComplementarityCondition";
const std::string RELAY_NSLAW_TAG = "Relay";
const std::string NEWTON_IMPACT_NSLAW_TAG = "NewtonImpactLaw";
const std::string NEWTON_IMPACT_FRICTION_NSLAW_TAG = "NewtonImpactFrictionLaw";

// DSIO
const std::string LINEAR_DSIO_TAG = "LinearDSIO";
const std::string NON_LINEAR_DSIO_TAG = "NonLinearDSIO";
const std::string LAGRANGIAN_DSIO_TAG = "LagrangianDSIO";
const std::string LAGRANGIAN_LINEAR_DSIO_TAG = "LagrangianLinearDSIO";

// EqualityConstraint
const std::string LINEAR_EC_TAG = "LinearEC";
const std::string NON_LINEAR_EC_TAG = "NonLinearEC";
const std::string LINEAR_TIME_INVARIANT_EC_TAG = "LinearTIEC";
const std::string LAGRANGIAN_EC_TAG = "LagrangianEC";
const std::string LAGRANGIAN_LINEAR_EC_TAG = "LagrangianLinearEC";


const std::string DSIO_CONCERNED = "DSInputOutput_Concerned";
const std::string DS_CONCERNED = "DS_Concerned";

//===========================================

// SiconosModel
const std::string MODEL_TAG = "SiconosModel";
const std::string NSDS_TAG = "NSDS";
const std::string STRATEGY_TAG = "Strategy";

// Strategy
const std::string ONESTEPINTEGRATOR_DEFINITION_TAG = "OneStepIntegrator_Definition";
const std::string ONESTEPINTEGRATOR_TAG = "OneStepIntegrator";
const std::string ONESTEPNSPROBLEM_TAG = "OneStepNSProblem";

const std::string TIMEDISCRETISATION_TAG = "TimeDiscretisation";
const std::string TIMESTEPPING_TAG = "TimeStepping";
const std::string EVENTDRIVEN_TAG = "EventDriven";

/*Types of OneStepIntegrator defined*/
const std::string MOREAU_TAG = "Moreau";
const std::string LSODAR_TAG = "LSODAR";
const std::string ADAMS_TAG = "Adams";

/*Types of OneStepNSProblem defined*/
const std::string LCP_TAG = "LCP";
const std::string FrictionContact2D_TAG = "FrictionContact2D";
const std::string FrictionContact3D_TAG = "FrictionContact3D";
const std::string QP_TAG = "QP";
const std::string RELAY_TAG = "Relay";


#endif

