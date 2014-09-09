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

#ifndef CVADAPTER_HH
#define CVADAPTER_HH

#include "BasicFV.hh"
#include "GroupedTrainingData.hh"
#include "BasicCVAdapter.hh"
#include "StDataASCIIFile.hh"

namespace svt
{
  typedef BasicCVAdapter<svt::BasicFV, 
                         svt::GroupedTrainingData<svt::BasicFV>,
                         svt::StDataASCIIFile> CVAdapter;
}

#endif
