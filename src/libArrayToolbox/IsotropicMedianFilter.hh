/**************************************************************************
 **       Title: Spherical Median Filter
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
 *  \file IsotropicMedianFilter.hh
 *  \brief Median filter implementation
 */
/*======================================================================*/

#ifndef ATBISOTROPICMEDIANFILTER_HH
#define ATBISOTROPICMEDIANFILTER_HH

#include "Filter.hh"

namespace atb
{

/*======================================================================*/
/*!
 *  \class IsotropicMedianFilter IsotropicMedianFilter.hh "ArrayToolbox/IsotropicMedianFilter.hh"
 *  \brief The IsotropicMedianFilter class implements the n-dimensional
 *    isotropic median filter.
 */
/*======================================================================*/
  template<typename DataT, int Dim>
  class IsotropicMedianFilter : public Filter<DataT,Dim,DataT>
  {

  public:
    
    typedef DataT ResultT;

/*======================================================================*/
/*! 
 *   Constructor. In contrast to other filters the median filter always
 *   uses crop boundary treatment. Therefore any changes in the
 *   setting of the boundary treatment are bogus and do not change the
 *   behaviour of the filter.
 *
 *   \param radiusUm The filter radius in micrometers
 */
/*======================================================================*/ 
    IsotropicMedianFilter(double radiusUm = 1.0);
    
/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    virtual ~IsotropicMedianFilter();

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
 *   \param extentsPx      The filter extents in pixels
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
   
  };

}

#include "IsotropicMedianFilter.icc"

#endif
