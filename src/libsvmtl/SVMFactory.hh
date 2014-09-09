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
 **  Revision 1.3  2003/05/19 11:12:04  ronneber
 **  - moved all code into templated BasicSVMFactory class, to be able to
 **    use it with other and own FV classes
 **
 **  Revision 1.2  2002/09/05 13:16:44  pigorsch
 **  *** empty log message ***
 **
 **  Revision 1.1  2002/09/04 10:29:57  pigorsch
 **  - initial revision, copied from libpollenpp and modified
 **
 **
 **
 **************************************************************************/

#ifndef SVMFACTORY_HH
#define SVMFACTORY_HH

#include "TList.hh"
#include "DefaultMultiClassList.hh"
#include "DefaultTwoClassList.hh"
#include "DefaultKernelList.hh"
#include "StDataASCIIFile.hh"
#include "BasicFV.hh"
#include "BasicSVMFactory.hh"

namespace svt
{
  /*-----------------------------------------------------------------------
   *  Specify SVM Factory, with all these algorithms and BasicFV
   *-----------------------------------------------------------------------*/
  typedef svt::BasicSVMFactory<BasicFV, 
                               StDataASCIIFile,
                               DefaultMultiClassList,
                               DefaultTwoClassList,
                               DefaultKernelList> SVMFactory;
  
  
}

#endif
