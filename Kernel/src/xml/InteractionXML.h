/* Siconos version 1.0, Copyright INRIA 2005.
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
/** \class InteractionXML
 *   \brief This class manages Interaction data part
 *  \author SICONOS Development Team - copyright INRIA
 *   \version 1.0
 *   \date (Creation) 04/12/2004
 *

 *
 * InteractionXML allows to manage data of a Interaction DOM tree.
 */



/** \todo: review completely this class to adapt it to multiple DS loading (rather than only 2)
    -> partially done. Not reviewed: setDSConcerned, loadInteraction.
*/

#ifndef __INTERACTIONXML__
#define __INTERACTIONXML__

#include "SiconosDOMTreeTools.h"
#include "Interaction.h"

#include "NonSmoothLawXML.h"
#include "DynamicalSystem.h"
#include "RelationXML.h"

class DynamicalSystem;
class Interaction;
class NonSmoothLawXML;
class RelationXML;

const std::string INTERACTION_Y = "y";
const std::string INTERACTION_LAMBDA = "lambda";
const std::string INTERACTION_NINTER = "nInter";
const std::string INTERACTION_DS_CONCERNED = "DS_Concerned";
const std::string INTERACTION_INTERACTWITHDS_NUMBER = "interactsWithDS_Number";
const std::string INTERACTION_DS_LIST = "DSList";

class InteractionXML
{
public:
  InteractionXML();

  /** \fn InteractionXML(xmlNode * rootNodeInteraction, int number)
   *   \brief Build a InteractionXML object from a DOM tree describing a Interaction
   *   \param xmlNode * interactionNode : the Interaction DOM tree
   *   \param vector<int> definedDSNumbers : vector of DSXML numbers to verify DS concerned by the interaction (identified by number) exists
   */
  InteractionXML(xmlNode * interactionNode, std::vector<int> definedDSNumbers);

  ~InteractionXML();


  /** \fn xmlNode* getInteractionXMLNode()
   *   \brief Return the root node of the InteractionXML
   *   \return xmlNode* : the root node
   */
  inline xmlNode* getInteractionXMLNode()
  {
    return rootInteractionXMLNode;
  }

  /** \fn int getNumber()
   *   \brief Return the number of the InteractionXML
   *   \return The number of the InteractionXML
   */
  inline int getNumber()
  {
    return SiconosDOMTreeTools::getIntegerAttributeValue(rootInteractionXMLNode, NUMBER_ATTRIBUTE);
  }

  /** \fn void setNumber(int i)
   *   \brief allows to save the number of the InteractionXML
   *   \return The number to save
   */
  inline void setNumber(int i)
  {
    SiconosDOMTreeTools::setIntegerAttributeValue(rootInteractionXMLNode, NUMBER_ATTRIBUTE, i);
  }

  /** \fn bool hasId()
   *  \brief return true if idNode is defined
   *  \return true if idNode is defined
   */
  inline bool hasId()
  {
    return (idNode != NULL);
  }

  /** \fn string getId()
   *   \brief Return the id of the InteractionXML
   *   \return The string id of the InteractionXML
   */
  inline std::string getId()
  {
    return SiconosDOMTreeTools::getStringContentValue(idNode);
  }

  /** \fn void setId(string s)
   *   \brief allows to save the id of the InteractionXML
   *   \return The string id to save
   */
  inline void setId(std::string s)
  {
    if (hasId() == false)
    {
      idNode = SiconosDOMTreeTools::createStringNode(rootInteractionXMLNode, ID_ATTRIBUTE, s);
    }
    else SiconosDOMTreeTools::setStringContentValue(idNode, s);
  }

  /** \fn int getNumber()
   *   \brief Return the number of the InteractionXML
   *   \return The number of the InteractionXML
   */
  inline int getNInter()
  {
    return SiconosDOMTreeTools::getIntegerContentValue(nInterNode);
  }

  /** \fn void setNumber(int i)
   *   \brief allows to save the number of the InteractionXML
   *   \return The number to save
   */
  inline void setNInter(int nInter)
  {
    if (nInterNode == false)
    {
      nInterNode = SiconosDOMTreeTools::createIntegerNode(rootInteractionXMLNode, INTERACTION_NINTER, nInter);
    }
    else SiconosDOMTreeTools::setIntegerContentValue(nInterNode, nInter);
  }

  /** \fn bool hasY()
   *  \brief return true if yNode is defined
   *  \return true if yNode is defined
   */
  inline bool hasY()
  {
    return (yNode != NULL);
  }

  /** \fn SimpleVector getY()
   *   \brief Return y vector of the InteractionXML
   *   \return SimpleVector : the y of the InteractionXML
   */
  inline SimpleVector getY()
  {
    return SiconosDOMTreeTools::getSiconosVectorValue(yNode);
  }

  /** \fn void setY(const SiconosVector *v)
   *   \brief allows to save the y of the InteractionXML
   *   \param SiconosVector* : the y to save
   */
  inline void setY(const SiconosVector& v)
  {
    if (hasY() == false)
    {
      yNode = SiconosDOMTreeTools::createVectorNode(rootInteractionXMLNode, INTERACTION_Y, v);
    }
    else SiconosDOMTreeTools::setSiconosVectorNodeValue(yNode, v);
  }

  /** \fn bool hasLambda()
   *  \brief return true if lambdaNode is defined
   *  \return true if lambdaNode is defined
   */
  inline bool hasLambda()
  {
    return (lambdaNode != NULL);
  }

  /** \fn SimpleVector getLambda()
   *   \brief Return the lambda vector of the InteractionXML
   *   \return SimpleVector : the lambda of the InteractionXML
   */
  inline SimpleVector getLambda()
  {
    return SiconosDOMTreeTools::getSiconosVectorValue(lambdaNode);
  }

  /** \fn void setLambda(const SiconosVector& v)
   *   \brief allows to save the lambda of the InteractionXML
   *   \return SiconosVector* : the lambda to save
   */
  inline void setLambda(const SiconosVector& v)
  {
    if (hasLambda() == false)
      lambdaNode = SiconosDOMTreeTools::createVectorNode(rootInteractionXMLNode, INTERACTION_LAMBDA, v);
    else SiconosDOMTreeTools::setSiconosVectorNodeValue(lambdaNode, v);
  }


  /** \fn vector<int> getDSConcerned()
   *   \brief Return the list of ds numbers concerned by the InteractionXML
   *   \return a vector of int
   */
  inline std::vector<int> getDSConcerned()
  {
    return SiconosDOMTreeTools::getVectorIntContentValue(dsListNode);
  }

  /** \fn SimpleVector getDSConcernedVector()
   *   \brief Return the DSs concerned by the InteractionXML
   *   \return a simple vector which contains a list of DS number
   */
  inline SimpleVector getDSConcernedVector()
  {
    return SiconosDOMTreeTools::getSiconosVectorValue(dsListNode);
  }

  /** \fn SimpleVector setDSConcernedVector()
   *   \brief set the DSs concerned by the InteractionXML
   *   \param a SimpleVector which contains a list of DS number
   */
  //\todo inline SimpleVector setDSConcernedVector(){};

  /** \fn RelationXML* getRelationXML()
   *   \brief Return the relationXML of the InteractionXML
   *   \return The relationXML of the InteractionXML
   */
  inline RelationXML* getRelationXML()
  {
    return relationXML;
  }

  /** \fn NonSmoothLawXML* getNonSmoothLawXML()
   *   \brief Return the NonSmoothLawXML of the InteractionXML
   *   \return The NonSmoothLawXML of the InteractionXML
   */
  inline NonSmoothLawXML* getNonSmoothLawXML()
  {
    return nSLawXML;
  }

  /** \fn void updateInteractionXML( xmlNode* node, Interaction* inter );
   *   \brief makes the operations to add an Interaction to the NSDS
   *   \param xmlNode* : the root node of the InteractionXML
   *   \param Interaction* : the Interaction of this InteractionXML
   */
  void updateInteractionXML(xmlNode* node, Interaction* inter);

  /** \fn void loadInteraction( Interaction* )
   *   \brief loads the data of the Interaction into the InteractionXML (in the DOM tree)
   *   \param NSDS* : the Interaction of this InteractionXML
   */
  void loadInteraction(Interaction*);

  /** \fn bool hasAll()
   *  \brief All is an attribute of the DS_Concerned tag
   *  \return bool : true if attribute all is defined
   */
  bool hasAll() const;

  /** \fn void setAll(bool all)
   *   \brief modify the attribute "all" of the DS_concerned tag
   *   \param bool : the value to assign to the attribute
   */
  void setAll(const bool&) ;

private:

  //Nodes
  xmlNode * rootInteractionXMLNode;

  xmlNode * idNode;
  xmlNode * nInterNode;
  xmlNode * yNode;
  xmlNode * lambdaNode;
  xmlNode * dsConcernedNode;
  // Node that contains a list (vector) of the ds concerned by this interaction
  xmlNode * dsListNode;

  //Relation
  RelationXML *relationXML;

  //NSLAW
  NonSmoothLawXML *nSLawXML;

  /** Flags to know if pointers have been allocated inside constructors or not */

  bool isRelationXMLAllocatedIn;
  bool isNsLawXMLAllocatedIn;

};

#endif
