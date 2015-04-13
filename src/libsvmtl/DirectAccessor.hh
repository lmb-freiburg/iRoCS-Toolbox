/**************************************************************************
 *
 * Copyright (C) 2004-2015 Olaf Ronneberger, Florian Pigorsch, Jörg Mechnich,
 *                         Thorsten Falk
 *
 *        Image Analysis Lab, University of Freiburg, Germany
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 **************************************************************************/

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

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

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
