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
**       Title: Kernel_DS_RBF.hh
**    $RCSfile$
**   $Revision: 740 $$Name$
**       $Date: 2005-07-19 15:42:59 +0200 (Tue, 19 Jul 2005) $
**   Copyright: LGPL $Author: haasdonk $
** Description: DS_RBF kernel functor
**              Distance-Substitution-RBF Kernel 
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2005/07/19 13:42:59  haasdonk
**  adapted header for automatic CVS-Information extraction
**
**
**************************************************************************/

#ifndef KERNEL_DS_RBF_HH
#define KERNEL_DS_RBF_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

// std includes
#include <cmath>
#include <map>
#include <set>
#include <string>

// libsvmtl includes
#include "ProgressReporter.hh"

// requirements of template parameters
#include "svt_check/RequireStData.hh"
#include "svt_check/RequireFeatureVectorUniqueID.hh"

namespace svt
{

  template< typename KERNEL>
  class Kernel_DS_RBF
  {
  public:
    Kernel_DS_RBF(double gamma=1.)
       :p_gamma(gamma)
          {};

    ~Kernel_DS_RBF()
          {};

    void setGamma( double gamma)  { p_gamma = gamma; }
    double gamma() const          { return p_gamma; }
    
    // new updateCache-Syntax:    
    template< typename ForwardIter1, typename Accessor1,
              typename ForwardIter2, typename Accessor2 >
    void updateCache( const ForwardIter1& fvBegin1,  
                      const ForwardIter1& fvEnd1,
                      Accessor1 accessor1,
                      const ForwardIter2& fvBegin2,  
                      const ForwardIter2& fvEnd2,
                      Accessor2 accessor2,
                      ProgressReporter* pr = 0) const
          {
            _kernel.updateCache(fvBegin1, fvEnd1,accessor1,
                                fvBegin2,fvEnd2,accessor2,pr);
          }

    // old updateCache-Syntax:    

    template< typename ForwardIter, typename Accessor>
    void updateCache( const ForwardIter& fvBegin,  
                      const ForwardIter& fvEnd,
                      Accessor accessor,
                      ProgressReporter* pr = 0) const
          {
            _kernel.updateCache(fvBegin, fvEnd,accessor,pr);
          }

    void clearCache() const
          {
            //  nothing to do here
          }

    template< typename FV>
    double k_function( const FV& x, const FV& y) const
          {
            CHECK_MEMBER_TEMPLATE( 
                svt_check::RequireFeatureVectorUniqueID<FV>);
            return exp(-p_gamma* _kernel.k_function(x,y));
          };

    template<typename STDATA>
    void loadParameters( STDATA& stData)
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);
            if( stData.valueExists( "gamma"))
            {
              stData.getValue( "gamma", p_gamma);
            }
            _kernel.loadParameters(stData);            
          };
    
    template<typename STDATA>
    void saveParameters( STDATA& stData) const
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);
            _kernel.saveParameters(stData);            
            stData.setValue( "kernel_type", name());
            stData.setValue( "gamma", p_gamma);
          };

    static std::string name()
          {
            return std::string("DS_rbf_") + KERNEL::name();
          }
    
    static const char* description()
          {
            return "distance substitution radial basis function" 
                   " kernel: exp(-gamma*d^2)";
          }

    static void getParamInfos( std::vector<ParamInfo>& p)
          {
            p.push_back(
                ParamInfo( "gamma", "g", "value",
                           "gamma for DS-rbf-kernel. (default 1.0)"));
            KERNEL::getParamInfos(p);
          }

  protected:
    KERNEL _kernel;    
    double p_gamma;
  };
}

#endif






