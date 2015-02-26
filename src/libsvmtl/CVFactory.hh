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
**  Revision 1.1  2004/08/26 08:36:58  ronneber
**  initital import
**
**
**
**************************************************************************/
#ifndef CVFACTORY_HH
#define CVFACTORY_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "TList.hh"
#include "DefaultMultiClassList.hh"
#include "DefaultTwoClassList.hh"
#include "DefaultKernelList.hh"
#include "GroupedTrainingData.hh"
#include "StDataASCIIFile.hh"
#include "BasicFV.hh"
#include "BasicCVFactory.hh"

namespace svt
{
  /*-----------------------------------------------------------------------
   *  Specify SVM Factory, with all these algorithms and BasicFV
   *-----------------------------------------------------------------------*/
  typedef svt::BasicCVFactory<svt::BasicFV, 
                              svt::GroupedTrainingData<svt::BasicFV>,
                              svt::StDataASCIIFile,
                              DefaultMultiClassList,
                              DefaultTwoClassList,
                              DefaultKernelList> CVFactory;
  
  
}

#endif
