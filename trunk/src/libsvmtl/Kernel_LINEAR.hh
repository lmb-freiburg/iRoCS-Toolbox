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
**       Title: LINEAR kernel functor
**    $RCSfile$
**   $Revision: 2825 $$Name$
**       $Date: 2009-09-15 17:04:15 +0200 (Tue, 15 Sep 2009) $
**   Copyright: LGPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.7  2005/10/26 07:05:16  ronneber
**  - added gradient_of_k_function()
**
**  Revision 1.6  2005/06/06 21:23:31  haasdonk
**  added updateCache() with two FV-lists, required for classification with precomputed kernel-matrices
**
**  Revision 1.5  2005/03/29 17:53:19  ronneber
**  - added clearCache()
**
**  Revision 1.4  2004/09/13 10:03:24  ronneber
**  - documentation update
**
**  Revision 1.3  2004/09/08 14:28:45  ronneber
**  - adapted to new ParamInfo class
**
**  Revision 1.2  2004/09/03 07:09:12  ronneber
**  - updateCache now takes an Accessor
**
**  Revision 1.1  2004/08/26 08:36:58  ronneber
**  initital import
**
**  Revision 1.4  2003/05/19 10:52:34  ronneber
**  - converted from MapTools to ParamMapWrapper
**  - added name()
**
**  Revision 1.3  2002/09/05 13:05:39  pigorsch
**  - modfified to use new MapTools
**
**  Revision 1.2  2002/05/10 11:07:03  ronneber
**  - removed FV template for all public classes, because Feature Vector type
**    can be extracted automatically from passed iterators or other
**    parameters -- this makes the public interface much more intuitive
**
**  Revision 1.1  2002/03/26 12:44:02  ronneber
**  restructured for autoconf
**
**  Revision 1.2  2002/01/14 13:52:25  pigorsch
**  * use FV.dotProduct(...)
**
**  Revision 1.1  2001/12/11 11:03:00  pigorsch
**  Initial Revision
**
**
**
**************************************************************************/

#ifndef KERNEL_LINEAR_HH
#define KERNEL_LINEAR_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

// std includes
#include <map>
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
   *  \class Kernel_LINEAR
   *  \ingroup kernel
   *  \brief The Kernel_LINEAR class specifies a linear kernel function: u'*v
   */
  /*======================================================================*/
  class Kernel_LINEAR
  {
  public:
    Kernel_LINEAR()
          {};
    
    ~Kernel_LINEAR()
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
            return x.dotProduct(y);
          }

    template< typename FV, typename FVGradient>
    void gradient_of_k_function( const FV& x, const FV& y, 
                                FVGradient& gradient) const
          {
            gradient = y;
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
            return "linear";
          }

    static const char* description()
          {
            return "linear kernel: u'*v";
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
