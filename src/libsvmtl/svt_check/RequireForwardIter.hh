/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 476 $$Name$
**       $Date: 2004-08-26 10:36:59 +0200 (Thu, 26 Aug 2004) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/


#ifndef REQUIREFORWARDITER_HH
#define REQUIREFORWARDITER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

namespace svt_check
{
  template<typename TESTCLASS>
  class RequireForwardIter
  {
  public:

    template<typename T> 
    void dummy( T& )
          {}
    

    ~RequireForwardIter()
          {
            if( false)
            {
              TESTCLASS a;
              dummy(*a);
              ++a;
            }
          }
  };
}


#endif
