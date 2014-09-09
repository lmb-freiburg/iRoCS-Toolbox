/**************************************************************************
**       Title: Kernel_DS_TRIA.hh
**    $RCSfile$
**   $Revision: 740 $$Name$
**       $Date: 2005-07-19 15:42:59 +0200 (Tue, 19 Jul 2005) $
**   Copyright: LGPL $Author: haasdonk $
** Description: DS_TRIA kernel functor
**              Distance-Substitution-TRIA Kernel 
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2005/07/19 13:42:59  haasdonk
**  adapted header for automatic CVS-Information extraction
**
**
**************************************************************************/


#ifndef KERNEL_DS_TRIA_HH
#define KERNEL_DS_TRIA_HH

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
  template< typename KERNEL >
  class Kernel_DS_TRIA
  {

  public:
    Kernel_DS_TRIA(double beta=2.)
       :p_beta(beta)
          {};

    ~Kernel_DS_TRIA()
          {};

    void setBeta( double beta)  { p_beta = beta; }
    double beta() const          { return p_beta; }
    
// old updateCache syntax 
    
    template< typename ForwardIter, typename Accessor>
    void updateCache( const ForwardIter& fvBegin,  
                      const ForwardIter& fvEnd,
                      Accessor accessor,
                      ProgressReporter* pr = 0) const
          {
            _kernel.updateCache(fvBegin,fvEnd,accessor,pr);
            
           }

// new updateCache syntax 
   template< typename ForwardIter1, typename Accessor1,
             typename ForwardIter2, typename Accessor2>
   void updateCache( const ForwardIter1& fvBegin1,  
                     const ForwardIter1& fvEnd1,
                      Accessor1 accessor1,
                     const ForwardIter2& fvBegin2,  
                     const ForwardIter2& fvEnd2,
                     Accessor2 accessor2,
                     ProgressReporter* pr = 0) const
          {
            _kernel.updateCache(fvBegin1,fvEnd1,accessor1,
                                fvBegin2,fvEnd2,accessor2,
                                pr);
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
            return -pow(_kernel.k_function(x,y),p_beta/2);
          };

    template<typename STDATA>
    void loadParameters( STDATA& stData)
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);
            if( stData.valueExists( "beta"))
            {
              stData.getValue( "beta", p_beta);
            }
            _kernel.loadParameters(stData);
          };
    
    template<typename STDATA>
    void saveParameters( STDATA& stData) const
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);
            _kernel.saveParameters(stData);
            stData.setValue( "kernel_type", name());
            stData.setValue( "beta", p_beta);
            
          };

    static std::string name()
          {
            return std::string("DS_tria_")+KERNEL::name();
          }
    
    static const char* description()
          {
            return "triangular distance substitution" 
                   " kernel: -d^beta for 0<beta<=2";
          }

    static void getParamInfos( std::vector<ParamInfo>& p)
          {
            p.push_back(
                ParamInfo( "beta", "b", "value",
                           "beta for DS-tria-kernel. (default 2.0)"));
            KERNEL::getParamInfos(p);
            
          }

  protected:
    KERNEL _kernel;
    double p_beta;
  };
}

#endif






