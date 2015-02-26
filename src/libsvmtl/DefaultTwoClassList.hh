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
#ifndef DEFAULTTWOCLASSLIST_HH
#define DEFAULTTWOCLASSLIST_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "TList.hh"
#include "TwoClassSVMc.hh"
#include "TwoClassSVMnu.hh"

namespace svt
{
  /*-------------------------------------------------------------------------
   *  specify two-class Algorithms
   *-------------------------------------------------------------------------*/
  typedef TTLIST_2( svt::TwoClassSVMc, 
                    svt::TwoClassSVMnu) DefaultTwoClassList;
}

#endif
