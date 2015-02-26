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
#ifndef SVMAPPLICATIONWITHDEFAULTS_HH
#define SVMAPPLICATIONWITHDEFAULTS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "BasicFV.hh"
#include "DefaultMultiClassList.hh"
#include "DefaultTwoClassList.hh"
#include "DefaultOneClassList.hh"
#include "DefaultKernelList.hh"
#include "AlgorithmLists.hh"
#include "LoadSaveASCII.hh"
#include "SVMApplication.hh"


namespace svt
{
  typedef SVMApplication< BasicFV,
                          AlgorithmLists< DefaultMultiClassList,
                                          DefaultTwoClassList,
                                          DefaultOneClassList,
                                          DefaultKernelList>,
                          LoadSaveASCII > SVMApplicationWithDefaults;
}

#endif
