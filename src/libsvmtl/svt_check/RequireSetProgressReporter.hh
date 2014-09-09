/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 590 $$Name$
**       $Date: 2005-01-28 15:29:34 +0100 (Fri, 28 Jan 2005) $
**   Copyright: GPL $Author: fehr $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.3  2005/01/28 14:29:34  fehr
**  some more include bugfixes
**
**  Revision 1.2  2005/01/28 14:03:30  fehr
**  some include bugfixing for external library use
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef REQUIRESETPROGRESSREPORTER_HH
#define REQUIRESETPROGRESSREPORTER_HH

#include "../ProgressReporter.hh"

namespace svt_check
{
  /*======================================================================*/
  /*!
   *  Ensure that TESTCLASS provides a setProgressReporter() method
   */
  /*======================================================================*/
  template<typename TESTCLASS>
  class RequireSetProgressReporter
  {
  public:
    ~RequireSetProgressReporter()
          {
            if( false)
            {
              void (TESTCLASS::*x1)(svt::ProgressReporter*) = &TESTCLASS::setProgressReporter;
              avoidUnunsedVariableWarning(x1);
              
            }
          }
  
    
  };
}


#endif
