/**************************************************************************
**       Title: Access elements in a container
**    $RCSfile$
**   $Revision: 492 $$Name$
**       $Date: 2004-09-01 16:45:13 +0200 (Wed, 01 Sep 2004) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.1  2004/09/01 14:43:36  ronneber
**  changed IterToPointerTraits stuff to
**  DirectAccessor and DereferencingAccessor, to make code more
**  intuitive understandable
**
**
**
**************************************************************************/

#ifndef DIRECTACCESSOR_HH
#define DIRECTACCESSOR_HH

#include <iterator>

namespace svt
{
  struct DirectAccessor
  {
    template< typename Iter>
    struct Traits
    {
      typedef typename std::iterator_traits<Iter>::value_type value_type;
    };
    
    template< typename Iter>
    typename Traits<Iter>::value_type& operator()( const Iter& iter)
          {
            return *iter;
          }
  };
  
}

#endif
