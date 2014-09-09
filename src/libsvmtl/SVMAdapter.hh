/**************************************************************************
 **       Title: 
 **    $RCSfile$
 **   $Revision: 476 $$Name$
 **       $Date: 2004-08-26 10:36:59 +0200 (Thu, 26 Aug 2004) $
 **   Copyright: LGPL $Author: ronneber $
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
 **  Revision 1.3  2003/05/19 11:09:03  ronneber
 **  - moved all code into templated BasicSVMAdapter class, to be able to
 **    use it with other and own FV classes
 **
 **  Revision 1.2  2002/09/05 13:04:15  pigorsch
 **  - added typedef MCFV
 **
 **  Revision 1.1  2002/09/04 10:29:57  pigorsch
 **  - initial revision, copied from libpollenpp and modified
 **
 **
 **
 **************************************************************************/

#ifndef SVMADAPTER_HH
#define SVMADAPTER_HH

#include "BasicSVMAdapter.hh"
#include "BasicFV.hh"
#include "StDataASCIIFile.hh"

namespace svt
{
  typedef BasicSVMAdapter<svt::BasicFV,  svt::StDataASCIIFile> SVMAdapter;
}


#endif
