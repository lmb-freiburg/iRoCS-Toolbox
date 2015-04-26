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

#include <string>
#include <sstream>

#include "lmbunit.hh"
#include "../libsvmtl/Kernel_RBF.hh"
#include "../libsvmtl/BasicFV.hh"


static void testWrongInput()
{
  std::istringstream iss( "+1 1:0.708333 2:1 3:1 4:-0.320755 5:-0.105023 6:-1 7:1 8:-0.419847 9:-1 10:-0.225806 12:1 13:-1");
  
  svt::BasicFV fv;
  
  try
  {
    iss >> fv;
  }
  catch(   

  LMBUNIT_WRITE_FAILURE( "' keine Zahl!' interpreted as "
                         "float must throw exception!");
}
