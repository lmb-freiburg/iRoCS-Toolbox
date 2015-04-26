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

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

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
