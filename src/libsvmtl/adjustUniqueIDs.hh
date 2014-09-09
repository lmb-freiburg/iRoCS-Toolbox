/**************************************************************************
**       Title: simple functions to adjust the uniqueID's of feature vectors
**    $RCSfile$
**   $Revision: 4820 $$Name$
**       $Date: 2011-11-08 10:57:01 +0100 (Tue, 08 Nov 2011) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.3  2005/01/28 14:03:30  fehr
**  some include bugfixing for external library use
**
**  Revision 1.2  2004/09/03 09:12:41  ronneber
**  - uses Accessor now
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef ADJUSTUNIQUEIDS_HH
#define ADJUSTUNIQUEIDS_HH

#include "DirectAccessor.hh"
#include "DereferencingAccessor.hh"
#include <cstddef>

namespace svt
{
  
  template<typename ForwardIter, typename Accessor>
  void adjustUniqueIDs( const ForwardIter& fvBegin, 
                        const ForwardIter& fvEnd,
                        Accessor accessor)
  {
    ptrdiff_t size = fvEnd - fvBegin;
    ForwardIter p = fvBegin;
    for(ptrdiff_t i = 0; i < size; ++i, ++p)
    {
      accessor(p).setUniqueID(static_cast<unsigned int>(i));
    }
  }
  
  template< typename FV>
  void adjustUniqueIDs( std::vector<FV*>& featureVectorPointers)
  {
    adjustUniqueIDs( featureVectorPointers.begin(), 
                     featureVectorPointers.end(),
                     DereferencingAccessor());
  }
  
  template< typename FV>
  void adjustUniqueIDs( std::vector<FV>& featureVectors)
  {
    adjustUniqueIDs( featureVectors.begin(),
                     featureVectors.end(),
                     DirectAccessor());
  }
  
  
}

#endif
