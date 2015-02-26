/**************************************************************************
**       Title: Squared Euclidean Distance 
**    $RCSfile$
**   $Revision: 739 $$Name$
**       $Date: 2005-07-19 15:06:38 +0200 (Tue, 19 Jul 2005) $
**   Copyright: LGPL $Author: haasdonk $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.1  2005/07/19 13:06:38  haasdonk
**  initial revision
**
**
**************************************************************************/

#ifndef KERNEL_EDSQR_HH
#define KERNEL_EDSQR_HH

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

namespace svt
{
  /*======================================================================*/
  /*!
   *  \class Kernel_EDsqr
   *  \ingroup kernel
   *  \brief The Kernel_EDsqr class specifies a Euclidean distance kernel 
   *         |u-v|^2 to be used in the distance substitution kernels 
   *         Kernel_DS_*  
   */
  /*======================================================================*/
  class Kernel_EDsqr
  {
  public:
    Kernel_EDsqr()
          {};

    ~Kernel_EDsqr()
          {};

    template< typename ForwardIter, typename Accessor>
    void updateCache( const ForwardIter& fvBegin,  
                      const ForwardIter& fvEnd,
                      Accessor accessor,
                      ProgressReporter* pr = 0) const
          {
            //  nothing to do here
          }

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
            // nothing to do here
          }

    void clearCache() const
          {
            //  nothing to do here
          }

    template< typename FV>
    double k_function( const FV& x, const FV& y) const
          {
            return x.square() - 2*x.dotProduct(y) + y.square();
          };

    template<typename STDATA>
    void loadParameters( STDATA& stData)
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);
          };
    
    template<typename STDATA>
    void saveParameters( STDATA& stData) const
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);
            stData.setValue( "kernel_type", name());
          };

    static const char* name()
          {
            return "edsqr";
          }
    
    static const char* description()
          {
            return "squared Euclidean distance function: |u-v|^2";
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
    static void getParamInfos( std::vector<ParamInfo>& p)
          {
          }    
  };
}

#endif






