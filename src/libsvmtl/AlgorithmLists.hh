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


#ifndef ALGORITHMLISTS_HH
#define ALGORITHMLISTS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

namespace svt
{
  template< typename MCLIST_, 
            typename TCLIST_, 
            typename OCLIST_, 
            typename KFLIST_>
  struct AlgorithmLists
  {
    typedef MCLIST_ MCLIST;
    typedef TCLIST_ TCLIST;
    typedef OCLIST_ OCLIST;
    typedef KFLIST_ KFLIST;
  };
}


#endif
