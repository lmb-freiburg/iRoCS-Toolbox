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

#ifndef REQUIREKERNEL_K_FUNCTION_HH
#define REQUIREKERNEL_K_FUNCTION_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

namespace svt_check
{
  template< typename KERNEL, typename FV>
  class RequireKernel_k_function
  {
  public:
    
    ~RequireKernel_k_function()
          {
            if(false)
            {
              double (KERNEL::*x1)(const FV&, const FV&) const = 
                  &KERNEL::k_function;
              avoidUnunsedVariableWarning( x1);
            }
            
          }
  };
}

#endif
