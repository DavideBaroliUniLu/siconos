//$Id: NewtonImpactLawNSLXML.h,v 1.3 2004/07/29 14:04:00 jbarbier Exp $
/** \class NewtonImpactLawNSLXML
 *  \brief  This class manages NewtonImpactLawNSL data part
 *  \author V. ACARY
 *  \version 0.1
 *  \date (Creation) June 29, 2004
 *
 * $Date: 2004/07/29 14:04:00 $
 * $Revision: 1.3 $
 * $Author: jbarbier $
 * $Source: /CVS/Siconos/SICONOS/src/xml/NewtonImpactLawNSLXML.h,v $
 *
 *
 * NewtonImpactLawNSLXML allows to manage data of a NewtonImpactLawNSL DOM tree.
 * \bug
 *  \warning
 */

#ifndef __NewtonImpactLawNSLXML__
#define __NewtonImpactLawNSLXML__


#include <libxml/tree.h>
#include <string>

#include "NonSmoothLawXML.h"
#include "SiconosDOMTreeTools.h"

using namespace std;

const string NEWTON_E = "e";


class NewtonImpactLawNSLXML : public NonSmoothLawXML
{
public:
  NewtonImpactLawNSLXML();

  /** \fn NewtonImpactLawNSLXML(xmlNode *NewtonImpactLawNSLLawNode)
  *   \brief Build a NewtonImpactLawNSLXML object from a DOM tree describing a Law with Relay type
  *   \param NewtonImpactLawNSLLawNode : theNewtonImpactLawNSLLaw DOM tree
  *   \exception XMLException : if a property of the NewtonImpactLawNSL  lacks in the DOM tree
  */
  NewtonImpactLawNSLXML(xmlNode * NewtonImpactLawNSLNode);

  /** \fn double getE()
  *   \brief Return the E of the NSLaw
  *   \return The E double value of the coefficient of restitution
  */
  inline double getE()
  {
    return  SiconosDOMTreeTools::getDoubleContentValue(this->ENode);
  }

  /** \fn void setE(double e)
  *   \brief Return the E of NSLaw
  *   \return The E double value of the coefficient of restitution
  */
  inline void setE(double e)
  {
    if (this->hasE() == false)
    {
      this->ENode = SiconosDOMTreeTools::createDoubleNode(this->rootNSLawXMLNode, NEWTON_E, e);
    }
    else SiconosDOMTreeTools::setDoubleContentValue(this->ENode, e);
  }

  /** \fn bool hasE()
   *  \brief returns true if ENode is defined
   *  \return true if ENode is defined
   */
  inline bool hasE()
  {
    return (this->ENode != NULL);
  }

private:
  xmlNode * ENode;

};

#endif
//$Log: NewtonImpactLawNSLXML.h,v $
//Revision 1.3  2004/07/29 14:04:00  jbarbier
//- new test on SiconosMemoryXML
//
//- last functions hasAttribute() in the XML part added
//
//Revision 1.2  2004/07/12 10:53:01  jbarbier
//-attribute R added to the save of the Dynamical systems
//-modifications of the xml schema to allow the use of R in the xml input file
//
//Revision 1.1  2004/07/06 14:54:49  acary
//Renaming NSLaw into NonSmoothLaw
//Renaming RelayNSLaw into RelayNSL
//Renaming CCNSLaw into ComplementarityConditionNSL
//Renaming NewtonImpactLaw into NewtonImpactLawNSL
//
//Revision 1.1  2004/06/30 09:44:35  acary
//Added NewtonImpactLawNSL
//