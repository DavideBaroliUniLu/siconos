/* Siconos-Kernel version 2.0.0, Copyright INRIA 2005-2006.
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

/*! \file RelationXML.h
*/

#ifndef __RelationXML__
#define __RelationXML__

#include "SiconosDOMTreeTools.h"

class Relation;

/** XML management for Relation
 *
 *
 *  \author SICONOS Development Team - copyright INRIA
 *   \version 2.0.0.
 *   \date 04/13/2004
 *
 * RelationXML allows to manage data of a Relation DOM tree.
 */
class RelationXML
{

public:

  RelationXML();
  RelationXML(xmlNode*);
  virtual ~RelationXML();

  /** Return the computeInput Plugin name of the RelationXML
  *   \return The computeInput Plugin name of the RelationXML
  *  \exception XMLException
  */
  virtual std::string getComputeInputPlugin() const ;

  /** Return the computeOutput Plugin name of the RelationXML
  *   \return The computeOutput Plugin name of the RelationXML
  *  \exception XMLException
  */
  virtual std::string getComputeOutputPlugin() const ;

  /** sets the computeInput Plugin name of the RelationXML
  *   \param string :  The computeInput Plugin name of the RelationXML
  *  \exception XMLException
  */
  void setComputeInputPlugin(const std::string&  plugin);

  /** sets the computeOutput Plugin name of the RelationXML
  *   \param string :  The computeOutput Plugin name of the RelationXML
  *  \exception XMLException
  */
  void setComputeOutputPlugin(const std::string&  plugin);

  /** Return true if computeInput is calculated from a plugin
  *   \return True if computeInput is calculated from plugin
  */
  inline bool isComputeInputPlugin() const
  {
    return xmlHasProp((xmlNodePtr)computeInputNode, (xmlChar *) PLUGIN_ATTRIBUTE.c_str());
  }

  /** Return true if computeOutput is calculated from a plugin
  *   \return True if computOutput is calculated from plugin
  */
  inline bool isComputeOutputPlugin() const
  {
    return xmlHasProp((xmlNodePtr)computeOutputNode, (xmlChar *) PLUGIN_ATTRIBUTE.c_str());
  }


  /** return true if computeInputNode is defined
  *  \return true if computeInputNode is defined
  */
  inline bool hasComputeInput() const
  {
    return (computeInputNode != NULL);
  }

  /** return true if computeOutputNode is defined
  *  \return true if computeOutputNode is defined
  */
  inline bool hasComputeOutput() const
  {
    return (computeOutputNode != NULL);
  }


  /** Return the type of the RelationXML
  *   \return The string type of the RelationXML
  */
  inline const std::string  getType() const
  {
    //return SiconosDOMTreeTools::getStringAttributeValue(rootRelationXMLNode, RELATION_TYPE);
    std::string type((char*)rootRelationXMLNode->name);
    return type;
  }

  /** Return the node of the RelationXML
  *   \return xmlNode* : the node of the RelationXML in the DOM tree
  */
  inline xmlNode* getNode()const
  {
    return rootRelationXMLNode;
  }

  /** makes the operations to create the Relation of the Interaction
  *   \param xmlNode* : the root node of the RelationXML
  *   \param Relation* : the Relation of this RelationXML
  */
  void updateRelationXML(xmlNode* node, Relation* rel);


protected:
  xmlNode * rootRelationXMLNode;
  xmlNode * computeInputNode;
  xmlNode * computeOutputNode;
};


#endif
