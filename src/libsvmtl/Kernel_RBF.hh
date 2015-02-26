/**************************************************************************
**       Title: RBF kernel functor
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
**  Revision 1.7  2005/10/26 07:06:38  ronneber
**  - added gradient_of_k_function()
**
**  Revision 1.6  2005/06/06 21:23:31  haasdonk
**  added updateCache() with two FV-lists, required for classification with precomputed kernel-matrices
**
**  Revision 1.5  2005/03/29 17:56:25  ronneber
**  - added clearCache()
**
**  Revision 1.4  2004/09/13 10:04:04  ronneber
**  - documentation update
**
**  Revision 1.3  2004/09/08 14:30:00  ronneber
**  - adapted to new ParamInfo class
**
**  Revision 1.2  2004/09/03 07:10:51  ronneber
**  - updateCache now takes an Accessor
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**  Revision 1.6  2003/05/19 10:53:44  ronneber
**  - converted from MapTools to ParamMapWrapper
**  - added name()
**
**  Revision 1.5  2002/09/05 13:05:39  pigorsch
**  - modfified to use new MapTools
**
**  Revision 1.4  2002/05/21 18:22:40  ronneber
**  - now all parameters can be queried
**
**  Revision 1.3  2002/05/10 11:34:14  ronneber
**  - added methods to modify Parameters later
**
**  Revision 1.2  2002/05/10 11:07:03  ronneber
**  - removed FV template for all public classes, because Feature Vector type
**    can be extracted automatically from passed iterators or other
**    parameters -- this makes the public interface much more intuitive
**
**  Revision 1.1  2002/03/26 12:44:02  ronneber
**  restructured for autoconf
**
**  Revision 1.2  2002/01/14 13:52:58  pigorsch
**  * use FV.dotProduct(...)
**
**  Revision 1.1  2001/12/11 11:03:00  pigorsch
**  Initial Revision
**
**
**
**************************************************************************/

#ifndef KERNEL_RBF_HH
#define KERNEL_RBF_HH

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
   *  \class Kernel_RBF
   *  \ingroup kernel
   *  \brief The Kernel_RBF class specifies a radial basis function
   *         kernel exp(-gamma*|u-v|^2)  
   */
  /*======================================================================*/
  class Kernel_RBF
  {
  public:
    Kernel_RBF(double gamma=1.)
       :p_gamma(gamma)
          {};

    ~Kernel_RBF()
          {};

    void setGamma( double gamma)  { p_gamma = gamma; }
    double gamma() const          { return p_gamma; }
    
        
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
            return exp(-p_gamma*(x.square() - 2*x.dotProduct(y) + y.square()));
          }

    template< typename FV, typename FVGradient>
    void gradient_of_k_function( const FV& x, const FV& y,
                                FVGradient& gradient) const
          {
            gradient = x - y;
            gradient *= -2 * p_gamma * k_function( x, y);
          }

    template<typename STDATA>
    void loadParameters( STDATA& stData)
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);
            if( stData.valueExists( "gamma"))
            {
              stData.getValue( "gamma", p_gamma);
            }
            else if( stData.valueExists( "feature_vector_dim"))
            {
              unsigned int fvDim = 1;
              stData.getValue( "feature_vector_dim", fvDim);
              p_gamma = 1.0/fvDim;
            }
          }
    
    template<typename STDATA>
    void saveParameters( STDATA& stData) const
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);
            stData.setValue( "kernel_type", name());
            stData.setValue( "gamma", p_gamma);
          }

    static const char* name()
          {
            return "rbf";
          }
    
    static const char* description()
          {
            return "radial basis function kernel: exp(-gamma*|u-v|^2)";
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
            p.push_back(
                ParamInfo( "gamma", "g", "value",
                           "gamma for rbf-kernel. (default 1.0, or "
                           "1/k, when feature_vector_dim is given"));
          }
    
    
  protected:
    double p_gamma;
  };
}

#endif






