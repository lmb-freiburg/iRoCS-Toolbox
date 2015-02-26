/**************************************************************************
**       Title: POLY kernel functor
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
**  Revision 1.6  2005/06/06 21:23:31  haasdonk
**  added updateCache() with two FV-lists, required for classification with precomputed kernel-matrices
**
**  Revision 1.5  2005/03/29 17:56:07  ronneber
**  - added clearCache()
**
**  Revision 1.4  2004/09/13 10:03:35  ronneber
**  - documentation update
**
**  Revision 1.3  2004/09/08 14:29:37  ronneber
**  - adapted to new ParamInfo class
**
**  Revision 1.2  2004/09/03 07:10:43  ronneber
**  - updateCache now takes an Accessor
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**  Revision 1.6  2003/05/19 10:53:12  ronneber
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
**  Revision 1.2  2002/01/14 13:52:47  pigorsch
**  * use FV.dotProduct(...)
**
**  Revision 1.1  2001/12/11 11:03:00  pigorsch
**  Initial Revision
**
**
**
**************************************************************************/

#ifndef KERNEL_POLY_HH
#define KERNEL_POLY_HH

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
   *  \class Kernel_POLY
   *  \ingroup kernel
   *  \brief The Kernel_POLY class specifies a polynomial kernel
   *         function: (gamma*u'*v + coef0)^degree 
   */
  /*======================================================================*/
  class Kernel_POLY
  {
  public:
    Kernel_POLY(
        double gamma=1., 
        double coef0=0., 
        double degree=3.)
            :p_gamma(gamma), 
        p_coef0(coef0), 
        p_degree(degree)
          {};
    
    ~Kernel_POLY()
          {};

    void setGamma( double gamma)   { p_gamma = gamma; }
    void setCoef0( double coef0)   { p_coef0 = coef0; }
    void setDegree( double degree) { p_degree = degree; }
    double gamma() const           { return p_gamma; }
    double coef0() const           { return p_coef0; }
    double degree() const          { return p_degree; }
    
    
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
            // nothing  to do here
          }

    void clearCache() const
          {
            //  nothing to do here
          }
            
    template< typename FV>
    double k_function( const FV& x, const FV& y) const
          {
            return pow(p_gamma*x.dotProduct(y)+p_coef0, p_degree);
          }

    template<typename STDATA>
    void loadParameters( STDATA& stData)
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);
            stData.getValue( "gamma", p_gamma);
            stData.getValue( "coef0", p_coef0);
            stData.getValue( "degree", p_degree);
            
          }
    
    template<typename STDATA>
    void saveParameters( STDATA& stData) const
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);
            stData.setValue( "kernel_type", name());
            stData.setValue( "gamma", p_gamma);
            stData.setValue( "coef0", p_coef0);
            stData.setValue( "degree", p_degree);
          }

    static const char* name()
          {
            return "poly";
          }

    static const char* description()
          {
            return "polynomial kernel: (gamma*u'*v + coef0)^degree";
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
                           "gamma for polynomial kernel. (default 1)"));
            p.push_back(
                ParamInfo( "coef0", "r", "value",
                           "coef0 for polynomial kernel. (default 0)"));
            p.push_back(
                ParamInfo( "degree", "d", "value",
                           "degree for polynomial kernel. (default 3)"));
          }
    
  protected:
    double p_gamma;
    double p_coef0;
    double p_degree;
  };
}


#endif
