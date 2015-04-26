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
**       Title: 
**    $RCSfile$
**   $Revision: 590 $$Name$
**       $Date: 2005-01-28 15:29:34 +0100 (Fri, 28 Jan 2005) $
**   Copyright: GPL $Author: fehr $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.4  2005/01/28 14:29:34  fehr
**  some more include bugfixes
**
**  Revision 1.3  2005/01/28 14:03:30  fehr
**  some include bugfixing for external library use
**
**  Revision 1.2  2004/09/03 09:13:04  ronneber
**  - adapted to new updateCache() interface of Kernels
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/
#ifndef REQUIRESVMKERNELUPDATECACHE_HH
#define REQUIRESVMKERNELUPDATECACHE_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "RequireHelpers.hh"
#include "../ProgressReporter.hh"
#include "../DirectAccessor.hh"
#include "../DereferencingAccessor.hh"

namespace svt_check
{
  
  /*======================================================================*/
  /*!
   *  Ensure that SVM provides a 
   *  kernel().updateCache( fv_begin, fv_end, progress) method
   */
  /*======================================================================*/
  template<typename SVM, typename FV>
  class RequireSVMKernelUpdateCache
  {
  public:
    ~RequireSVMKernelUpdateCache()
          {
            if( false)
            {
              SVM s;
              std::vector<FV> v;
              svt::ProgressReporter pr;
              s.kernel().updateCache( v.begin(), v.end(), 
                                      svt::DirectAccessor(), &pr);
              std::vector<FV*> v2;
              s.kernel().updateCache( v2.begin(), v2.end(), 
                                      svt::DereferencingAccessor(), &pr);
            }
          }
  };
}


#endif
