//$Id: RuntimeException.h,v 1.1 2005/01/05 15:59:04 charlety Exp $
/** \class RuntimeException
*   \brief This class represent a runtime exeption causing by the plateforme
*   \author A. Ravoux
*   \version 1.0
*   \date (Creation) 05/25/2004
*
* $Date: 2005/01/05 15:59:04 $
* $Revision: 1.1 $
* $Author: charlety $
* $Source: /CVS/Siconos/SICONOS/src/utils/SiconosException/RuntimeException.h,v $
*
* RuntimeException can be throws for example when a pointer is used but not allocated
* This exception can be catched by "catch(RuntimeException)" or "catch(SiconosException)"
*
*/

#ifndef __RuntimeException__
#define __RuntimeException__

#include "SiconosException.h"

// --------------------------------------------------------------------------
class RuntimeException: public SiconosException
{
public:

  /**
   * \fn RuntimeException()
   * \brief constructor
   */
  RuntimeException();

  /**
   * \fn RuntimeException(string report)
   * \brief constructor with a report
   * \param string report : exception description
   */
  RuntimeException(string report);

  /**
   * \fn ~RuntimeException()
   * \brief destructor
   */
  ~RuntimeException();

  /**
   * \fn static void selfThrow()
   * \brief static function which throw a RuntimeException
   * \exception RuntimeException
   */
  static void selfThrow();

  /**
   * \fn static void selfThrow(string report)
   * \brief static function which throw a RuntimeException with a report
   * \param string report : exception description
   * \exception RuntimeException
   */
  static void selfThrow(string report);

};

#endif //__RuntimeException__
