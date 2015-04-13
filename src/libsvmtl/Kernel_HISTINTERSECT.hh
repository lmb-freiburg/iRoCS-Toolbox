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
**       Title: histogram intersection kernel
**    $RCSfile$
**   $Revision: 4820 $$Name$
**       $Date: 2011-11-08 10:57:01 +0100 (Tue, 08 Nov 2011) $
**   Copyright: LGPL $Author: tschmidt $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
** 
**  $Log:  
**
**
**
**************************************************************************/

#ifndef KERNEL_HISTINTERSECT_HH
#define KERNEL_HISTINTERSECT_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

// std includes
#include <map>
#include <algorithm>
#include <set>
#include <string>

// libsvmtl includes
#include "ProgressReporter.hh"

// requirements of template parameters
#include "svt_check/RequireStData.hh"

namespace svt
{
  /*======================================================================*/
  /*!
   *  \class Kernel_HISTINTERSECT
   *  \ingroup kernel
   *  \brief The Kernel_HISTINTERSECT class specifies a HISTINTERSECT kernel function: min(u'(i),v(i))
   */
  /*======================================================================*/
  class Kernel_HISTINTERSECT
  {
  public:
    Kernel_HISTINTERSECT()
          {};
    
    ~Kernel_HISTINTERSECT()
          {};
        
    template< typename ForwardIter, typename Accessor>
    void updateCache( const ForwardIter&,  
                      const ForwardIter&,
                      Accessor,
                      ProgressReporter*) const
          {
            //  nothing to do here
          }
    
    // new updateCache-Syntax:    
    template< typename ForwardIter1, typename Accessor1,
              typename ForwardIter2, typename Accessor2 >
    void updateCache( const ForwardIter1&,  
                      const ForwardIter1&,
                      Accessor1,
                      const ForwardIter2&,  
                      const ForwardIter2&,
                      Accessor2,
                      ProgressReporter* = 0) const
          {
            // nothing to do here
          }
    
    void clearCache() const
          {
            //  nothing to do here
          }
            
    template< typename FV>
    double k_function( const FV& x, const FV& y) const
          {
            double temp = 0.0;
            for (int i = 0; i < static_cast<int>(x.size()); i++)
                temp += std::min(x[i],y[i]);
            return temp;
          }

    template<typename STDATA>
    void loadParameters( STDATA&)
          {}
    
    template<typename STDATA>
    void saveParameters( STDATA& stData) const
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);
            stData.setValue( "kernel_type", name());
          }

    static const char* name()
          {
            return "histintersect";
          }

    static const char* description()
          {
            return "histintersect kernel: min(u(i),v(i))";
          }

    /*======================================================================*/
    /*! 
     *   get information about the parameters, that are used in
     *   loadParameters() and saveParameters(). The Infos are appended
     *   to the passed array
     *
     *   \param p  array of ParamInfos. The Infos will be appended to
     *             this array
     */
    /*======================================================================*/
    static void getParamInfos( std::vector<ParamInfo>&)
          {}
    
    
    
  };
}

#endif
