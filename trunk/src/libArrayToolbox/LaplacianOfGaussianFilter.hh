/**************************************************************************
 *
 * Copyright (C) 2015 Thorsten Falk
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

/*======================================================================*/
/*!
 *  \file LaplacianOfGaussianFilter.hh
 *  \brief The Laplacian of Gaussian filter
 */
/*======================================================================*/

#ifndef ATBLAPLACIANFILTER_HH
#define ATBLAPLACIANFILTER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "SeparableConvolutionFilter.hh"

#include "ATBDataSynthesis.hh"

namespace atb
{

/*======================================================================*/
/*!
 *  \class LaplacianOfGaussianFilter LaplacianOfGaussianFilter.hh "libArrayToolbox/LaplacianOfGaussianFilter.hh"
 *  \brief The LaplacianOfGaussianFilter class implements the Filter
 *    interface and provides Laplacian of Gaussian computation using
 *    Gaussian derivative filters
 */
/*======================================================================*/
  template<typename DataT, int Dim>
  class LaplacianOfGaussianFilter :
        public Filter< DataT,Dim,DataT >
  {

  public:
    
    typedef DataT ResultT;

/*======================================================================*/
/*! 
 *   Default Constructor. Creates a Laplacian of Gaussian filter.
 *
 *   \param bt             The boundary treatment this filter uses
 *   \param boundaryValue  If bt is ValueBT, this value will be used for
 *     out-of-Array access
 */
/*======================================================================*/ 
    LaplacianOfGaussianFilter(
        BoundaryTreatmentType bt = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero);
    
/*======================================================================*/
/*! 
 *   Constructor.
 *
 *   \param standardDeviationUm  The isotropic standard deviation of the
 *     Gaussian in micrometers
 *   \param minimumKernelShapePx The minimum kernel shape in pixels.
 *     Usually the kernel has a shape of 12 * standardDeviation (in pixels) + 1
 *     in the dimension of the derivative and half the size for the
 *     perpendicular directions. If you want to explicitely force larger
 *     kernels you can give the minimum kernel extents here.
 *   \param bt             The boundary treatment this filter uses
 *   \param boundaryValue  If bt is ValueBT, this value will be used for
 *     out-of-Array access
 */
/*======================================================================*/ 
    LaplacianOfGaussianFilter(
        double standardDeviationUm,
        blitz::TinyVector<BlitzIndexT,Dim> const &minimumKernelShapePx =
        BlitzIndexT(0), BoundaryTreatmentType bt = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero);
    
/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    virtual ~LaplacianOfGaussianFilter();

/*======================================================================*/
/*! 
 *   Get the standard deviation of the Gaussian
 *
 *   \return The standard deviation of the Gaussian in micrometers
 */
/*======================================================================*/
    double standardDeviationUm() const
          {
            return _standardDeviationUm;
          }

/*======================================================================*/
/*! 
 *   Set the standard deviation of the Gaussian
 *
 *   \param standardDeviationUm The standard deviation of the Gaussian in
 *     micrometers
 */
/*======================================================================*/
    void setStandardDeviationUm(double standardDeviationUm)
          {
            _standardDeviationUm = standardDeviationUm;
          }

/*======================================================================*/
/*! 
 *   Get the minimum kernel shape of the Gaussian in pixels for all
 *   dimensions.
 *
 *   \return The minimum kernel shape in pixels
 */
/*======================================================================*/
    blitz::TinyVector<BlitzIndexT,Dim> minimumKernelShapePx() const
          {
            return _minimumKernelShapePx;
          }

/*======================================================================*/
/*! 
 *   Set the minimum kernel shape of the Gaussian in pixels for all
 *   dimensions.
 *
 *   \param minimumKernelShapePx The minimum kernel shape in pixels
 */
/*======================================================================*/
    void setMinimumKernelShapePx(
        blitz::TinyVector<BlitzIndexT,Dim> const &minimumKernelShapePx) const
          {
            _minimumKernelShapePx = minimumKernelShapePx;
          }

/*======================================================================*/
/*! 
 *   Render the used kernel into the given Array. The Array will be resized
 *   to resemble the kernel that would be used to filter a dataset with the
 *   element size given in the kernel Array.
 *
 *   \param kernel The Array the kernel will be rendered to. The elementSizeUm
 *     attribute of the kernel must be set to the desired element size.
 */
/*======================================================================*/
    void renderKernel(atb::Array<DataT,Dim> &kernel) const;

/*======================================================================*/
/*! 
 *   Apply the filter to the given Array.
 *
 *   \param data          The blitz++ Array to apply the filter to
 *   \param elementSizeUm The element size of the Array
 *   \param filtered      The filtering result
 *
 *   \exception RuntimeError If an error occurs during the filter operation
 *     an exception of this kind is thrown
 */
/*======================================================================*/
    virtual void apply(
        blitz::Array<DataT,Dim> const &data,
        blitz::TinyVector<double,Dim> const &elementSizeUm,
        blitz::Array<ResultT,Dim> &filtered,
        iRoCS::ProgressReporter *pr = NULL) const;
    
    // Explicitly force the name mangler to also consider the base class
    // implementation
    using atb::Filter<DataT,Dim,ResultT>::apply;

/*======================================================================*/
/*! 
 *   Apply the filter to the given Array.
 *
 *   \param data           The blitz++ Array to apply the filter to
 *   \param elementSizeUm  The element size of the Array
 *   \param filtered       The filtering result
 *   \param standardDeviationUm        The standard deviation of the Gaussian
 *   \param btType         The boundary treatment to apply
 *   \param boundaryValue  The out-of-Array value that should be used if
 *     btType == \c ValueBT
 *
 *   \exception RuntimeError If an error occurs during the filter operation
 *     an exception of this kind is thrown
 */
/*======================================================================*/
    static void apply(
        blitz::Array<DataT,Dim> const &data,
        blitz::TinyVector<double,Dim> const &elementSizeUm,
        blitz::Array<ResultT,Dim> &filtered,
        double standardDeviationUm,
        blitz::TinyVector<BlitzIndexT,Dim>
        const &minimumKernelShapePx = BlitzIndexT(0),
        BoundaryTreatmentType btType = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero,
        iRoCS::ProgressReporter *pr = NULL);

/*======================================================================*/
/*! 
 *   Apply the filter to the given Array.
 *
 *   \param data          The blitz++ Array to apply the filter to
 *   \param filtered      The filtering result
 *   \param standardDeviationUm        The standard deviation of the Gaussian
 *   \param btType        The boundary treatment to apply
 *   \param boundaryValue If btType is Value, this value is assumed outside
 *     the Arrays
 *   \param btType         The boundary treatment to apply
 *   \param boundaryValue  The out-of-Array value that should be used if
 *     btType == \c ValueBT
 *
 *   \exception RuntimeError If an error occurs during the filter operation
 *     an exception of this kind is thrown
 */
/*======================================================================*/
    static void apply(
        Array<DataT,Dim> const &data, Array<ResultT,Dim> &filtered,
        double standardDeviationUm,
        blitz::TinyVector<BlitzIndexT,Dim>
        const &minimumKernelShapePx = BlitzIndexT(0),
        BoundaryTreatmentType btType = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero,
        iRoCS::ProgressReporter *pr = NULL);

  private:
    
    static void _gaussian(
        atb::Array<DataT,1> &gauss, double sigmaUm, BlitzIndexT minExtent);
    static void _laplacianOfGaussian(
        atb::Array<DataT,1> &laplacian, double sigmaUm, BlitzIndexT minExtent);

    double _standardDeviationUm;
    blitz::TinyVector<BlitzIndexT,Dim> _minimumKernelShapePx;

  };

}

#include "LaplacianOfGaussianFilter.icc"

#endif
