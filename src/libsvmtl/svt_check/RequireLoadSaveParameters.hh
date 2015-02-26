/**************************************************************************
**       Title: test if specified class fulfills the ClassReflectionInterface
**    $RCSfile$
**   $Revision: 538 $$Name$
**       $Date: 2004-09-13 12:04:04 +0200 (Mon, 13 Sep 2004) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2004/09/13 10:04:04  ronneber
**  - documentation update
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef REQUIRELOADSAVEPARAMETERS_HH
#define REQUIRELOADSAVEPARAMETERS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "RequireHelpers.hh"
#include "MinimalStData.hh"
#include <string>
#include <map>
#include <set>

namespace svt_check
{
  
  /*======================================================================*/
  /*!
   *  \class RequireLoadSaveParameters
   *  \brief Ensure that TESTCLASS provides a loadParameters() and
   *         saveParamters() method. 
   *
   *   See MinimalLoadSaveParameters for an example  
   */
  /*======================================================================*/
  template<typename TESTCLASS>
  class RequireLoadSaveParameters
  {
  public:
    ~RequireLoadSaveParameters()
          {
            if( false)
            {
              /*--------------------------------------------------------------
               *  check templated members
               *--------------------------------------------------------------*/
              void (TESTCLASS::*x1)(MinimalStData&) = &TESTCLASS::loadParameters;
              avoidUnunsedVariableWarning(x1);
              
              void (TESTCLASS::*x2)(MinimalStData&) const = &TESTCLASS::saveParameters;
              avoidUnunsedVariableWarning(x2);
            }
          }
  
    
  };
}


#endif
