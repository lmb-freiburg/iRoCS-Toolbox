/**************************************************************************
 **       Title: Spherical Percentile Filter
 **    $RCSfile:$
 **   $Revision:$ $Name:$
 **       $Date:$
 **   Copyright: GPL $Author:$
 ** Description:
 **
 **   The median filter implementation for isotropic filtering
 **
 **************************************************************************/

/*======================================================================*/
/*!
 *  \file IsotropicPercentileFilter.hh
 *  \brief Percentile filter implementation
 */
/*======================================================================*/

#ifndef ATBISOTROPICMEDIANFILTER_HH
#define ATBISOTROPICMEDIANFILTER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "Filter.hh"

namespace atb
{

/*======================================================================*/
/*!
 *  \class IsotropicPercentileFilter IsotropicPercentileFilter.hh "ArrayToolbox/IsotropicPercentileFilter.hh"
 *  \brief The IsotropicPercentileFilter class implements the n-dimensional
 *    percentile filter.
 *
 *  The percentile filter is a generalization of the median filter, that
 *  takes an arbitrary percentile in the sorted intensity array instead of
 *  the median element. A percentile filter with percentile 50 is equivalent
 *  to the median filter, a percentile value of 0 results in a minimum filter
 *  and a percentile value of 100 in a maximum filter.
 */
/*======================================================================*/
  template<typename DataT, int Dim>
  class IsotropicPercentileFilter : public Filter<DataT,Dim,DataT>
  {

  public:
    
    typedef DataT ResultT;

/*======================================================================*/
/*! 
 *   Constructor. In contrast to other filters the percentile filter always
 *   uses crop boundary treatment. Therefore any changes in the
 *   setting of the boundary treatment are bogus and do not change the
 *   behaviour of the filter.
 *
 *   \param radiusUm   The filter radius in micrometers
 *   \param percentile The filter percentile in per cent
 */
/*======================================================================*/ 
    IsotropicPercentileFilter(
        double radiusUm = 1.0, double percentile = 50.0);
    
/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    virtual ~IsotropicPercentileFilter();

/*======================================================================*/
/*! 
 *   Get the filter radius in micrometers. 
 *
 *   \return The filter radius in micrometers. 
 */
/*======================================================================*/
    double filterRadiusUm() const;

/*======================================================================*/
/*! 
 *   Set the filter radius in micrometers. 
 *
 *   \param radiusUm The filter radius in micrometers. 
 */
/*======================================================================*/
    void setFilterRadiusUm(double radiusUm);

/*======================================================================*/
/*! 
 *   Get the percentile. 
 *
 *   \return The percentile in per cent. 
 */
/*======================================================================*/
    double percentile() const;

/*======================================================================*/
/*! 
 *   Set the percentile. 
 *
 *   \param percentile The percentile in per cent. 
 */
/*======================================================================*/
    void setPercentile(double percentile);

/*======================================================================*/
/*! 
 *   Apply the filter to the given Array.
 *
 *   \param data          The blitz++ Array to apply the filter to
 *   \param elementSizeUm The element size of the Array
 *   \param result        The filtering result
 *
 *   \exception RuntimeError If an error occurs during the filter operation
 *     an exception of this kind is thrown
 */
/*======================================================================*/
    virtual void apply(
        blitz::Array<DataT,Dim> const &data,
        blitz::TinyVector<double,Dim> const &elementSizeUm,
        blitz::Array<ResultT,Dim> &result,
        iRoCS::ProgressReporter *pr = NULL) const;
    
    // Explicitly force the name mangler to also consider the base class
    // implementation
    using atb::Filter<DataT,Dim,ResultT>::apply;
    
/*======================================================================*/
/*! 
 *   Apply the filter to the given Array.
 *
 *   \param data           The blitz++ Array to apply the filter to
 *   \param filtered       The filtering result
 *   \param radiusUm       The filter radius in micrometers
 *   \param percentile     The filter percentile in per cent
 *
 *   \exception RuntimeError If an error occurs during the filter operation
 *     an exception of this kind is thrown
 */
/*======================================================================*/
    static void apply(
        blitz::Array<DataT,Dim> const &data,
        blitz::Array<ResultT,Dim> &filtered,
        double radiusUm, iRoCS::ProgressReporter *pr = NULL);

  private:
    
    double _filterRadiusUm;
    double _percentile;
   
  };

}

#include "IsotropicPercentileFilter.icc"

#endif
