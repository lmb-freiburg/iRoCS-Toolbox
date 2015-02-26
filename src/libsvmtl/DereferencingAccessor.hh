/**************************************************************************
**       Title: Access elements in a container of type like std::vector<FV*>
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

#ifndef DEREFERENCINGACCESSOR_HH
#define DEREFERENCINGACCESSOR_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <iterator>

namespace svt
{
  
  struct DereferencingAccessor
  {
    template< typename Iter>
    struct Traits
    {
      typedef typename std::iterator_traits<typename std::iterator_traits<Iter>::value_type>::value_type value_type;
    };
    
    
    template< typename Iter>
    typename Traits<Iter>::value_type& operator()( const Iter& iter)
          {
            return **iter;
          }
  };
  
}

#endif
