/**************************************************************************
 **       Title: General blitz++ Array extension for microscopic data
 **    $RCSfile$
 **   $Revision: 3896 $ $Name$
 **       $Date: 2008-07-16 15:17:47 +0200 (Wed, 16 Jul 2008) $
 **   Copyright: GPL $Author: tschmidt $
 ** Description:
 **
 **    This file provides a class to store microscopic datasets with
 **    element size and a transformation to apply. It provides a function
 **    valueAt() to first apply the transformation and then access
 **    the transformed position using the specified ArrayInterpolator.
 **
 **************************************************************************/

/*======================================================================*/
/*!
 *  \file Array.hh
 *  \brief Array class derived from blitz++ Arrays for handling
 *     microscopic datasets with associated element size, transformation
 *     matrix and interpolated array access.
 */
/*======================================================================*/

#ifndef ATBARRAY_HH
#define ATBARRAY_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <cstdlib>
#include <cstring>

#include <libProgressReporter/ProgressReporter.hh>
#include <libBlitzHdf5/FileIOWrapper.hh>

#include "Interpolator.hh"
#include "TinyMatrixOperators.hh"
#include "ATBLinAlg.hh"

namespace atb
{
  
/*======================================================================*/
/*!
 *  \class Array Array.hh "libArrayToolbox/Array.hh"
 *  \brief The Array class is an extension to the blitz++ Array class
 *     providing additional parameters element size in microns and a
 *     transformation.
 *
 *  This class adds relevant metadata to the general purpose blitz++
 *  Arrays to handle microscopic datasets in a unified way. Along with the
 *  raw data with the known manipulation options of blitz++ Arrays, it
 *  additionally specifies the voxel extents in micormeters and you can
 *  specify a transformation to apply before querying the Array.
 *  Application of the transformation may yield sub-pixel positions. You
 *  can specify the interpolation behaviour by specifying an
 *  Interpolator and passing it via the setInterpolator() function.
 *  By default a linear interpolator with zero boundary treatment is used.
 */
/*======================================================================*/
  template<typename DataT, int Dim>
  class Array : public blitz::Array<DataT,Dim>
  {

  public:

/*======================================================================*/
/*! 
 *   Constructor from BlitzH5File.
 *
 *   \param fileName  The hdf5 file name
 *   \param dataset   The hdf5 dataset name
 */
/*======================================================================*/
  explicit Array(
      std::string const &fileName, std::string const &dataset,
      iRoCS::ProgressReporter *progress = NULL);

/*======================================================================*/
/*! 
 *   Constructor from Array expression.
 *
 *   \param expr The Array expression to create an Array from
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param transformation The transformation to apply when accessing
 *      Array entries with valueAt() and setValueAt()
 */
/*======================================================================*/
    template<typename T_expr>
    explicit Array(
        blitz::_bz_ArrayExpr<T_expr> expr,
        blitz::TinyVector<double,Dim> const &elementSizeUm = 1.0,
        blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation =
        (traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one))
            : blitz::Array<DataT,Dim>(expr),
              _elementSizeUm(elementSizeUm), _transformation(transformation),
              p_interpolator(InterpolatorFactory<DataT,Dim>::get(LinearIP))
          {}

/*======================================================================*/
/*! 
 *   Default Constructor.
 *
 *   This constructor calls the blitz::Array default constructor, sets the
 *   given element size and transformation matrix. The interpolator will be
 *   set to linear with ZeroBoundaryTreatment.
 *
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param transformation The transformation to apply when accessing
 *      Array entries with valueAt() and setValueAt()
 *   \param storage The GeneralArrayStorage to use for the underlying
 *      blitz++ Array
 */
/*======================================================================*/
    Array(
        blitz::TinyVector<double,Dim> const &elementSizeUm = 1.0,
        blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation =
        (traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one),
        blitz::GeneralArrayStorage<Dim> storage =
        blitz::GeneralArrayStorage<Dim>())
            : blitz::Array<DataT,Dim>(storage),
              _elementSizeUm(elementSizeUm), _transformation(transformation),
              p_interpolator(InterpolatorFactory<DataT,Dim>::get(LinearIP))
          {}
    
/*======================================================================*/
/*! 
 *   Constructor taking one extent parameter.
 *
 *   This constructor calls the corresponding blitz::Array constructor,
 *   sets the given element size and transformation matrix. The
 *   interpolator will be set to linear with ZeroBoundaryTreatment.
 *
 *   \param extent0 The Array extent for the zero's dimension (If the
 *      Array has more than one dimension this will be used for all
 *      remaining dimensions as well)
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param transformation The transformation to apply when accessing
 *      Array entries with valueAt() and setValueAt()
 *   \param storage The GeneralArrayStorage to use for the underlying
 *      blitz++ Array
 */
/*======================================================================*/
    explicit Array(
        BlitzIndexT extent0,
        blitz::TinyVector<double,Dim> const &elementSizeUm = 1.0,
        blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation =
        (traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one),
        blitz::GeneralArrayStorage<Dim> storage =
        blitz::GeneralArrayStorage<Dim>())
            : blitz::Array<DataT,Dim>(extent0, storage),
              _elementSizeUm(elementSizeUm), _transformation(transformation),
              p_interpolator(InterpolatorFactory<DataT,Dim>::get(LinearIP))
          {}

/*======================================================================*/
/*! 
 *   Constructor taking two extent parameters.
 *
 *   This constructor calls the corresponding blitz::Array constructor,
 *   sets the given element size and transformation matrix. The
 *   interpolator will be set to linear with ZeroBoundaryTreatment.
 *
 *   \param extent0 The Array extent for the zeroth dimension
 *   \param extent1 The Array extent for the first dimension (If the
 *      Array has more than two dimensions this will be used for all
 *      remaining dimensions as well)
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param transformation The transformation to apply when accessing
 *      Array entries with valueAt() and setValueAt()
 *   \param storage The GeneralArrayStorage to use for the underlying
 *      blitz++ Array
 */
/*======================================================================*/
    Array(
        BlitzIndexT extent0, BlitzIndexT extent1,
        blitz::TinyVector<double,Dim> const &elementSizeUm = 1.0,
        blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation =
        (traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one),
        blitz::GeneralArrayStorage<Dim> storage =
        blitz::GeneralArrayStorage<Dim>())
            : blitz::Array<DataT,Dim>(extent0, extent1, storage),
              _elementSizeUm(elementSizeUm), _transformation(transformation),
              p_interpolator(InterpolatorFactory<DataT,Dim>::get(LinearIP))
          {}

/*======================================================================*/
/*! 
 *   Constructor taking three extent parameters.
 *
 *   This constructor calls the corresponding blitz::Array constructor,
 *   sets the given element size and transformation matrix. The
 *   interpolator will be set to linear with ZeroBoundaryTreatment.
 *
 *   \param extent0 The Array extent for the zeroth dimension
 *   \param extent1 The Array extent for the first dimension
 *   \param extent2 The Array extent for the second dimension (If the
 *      Array has more than three dimensions this will be used for all
 *      remaining dimensions as well)
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param transformation The transformation to apply when accessing
 *      Array entries with valueAt() and setValueAt()
 *   \param storage The GeneralArrayStorage to use for the underlying
 *      blitz++ Array
 */
/*======================================================================*/
    Array(
        BlitzIndexT extent0, BlitzIndexT extent1, BlitzIndexT extent2,
        blitz::TinyVector<double,Dim> const &elementSizeUm = 1.0,
        blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation =
        (traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one),
        blitz::GeneralArrayStorage<Dim> storage =
        blitz::GeneralArrayStorage<Dim>())
            : blitz::Array<DataT,Dim>(extent0, extent1, extent2, storage),
              _elementSizeUm(elementSizeUm), _transformation(transformation),
              p_interpolator(InterpolatorFactory<DataT,Dim>::get(LinearIP))
          {}

/*======================================================================*/
/*! 
 *   Constructor taking four extent parameters.
 *
 *   This constructor calls the corresponding blitz::Array constructor,
 *   sets the given element size and transformation matrix. The
 *   interpolator will be set to linear with ZeroBoundaryTreatment.
 *
 *   \param extent0 The Array extent for the zeroth dimension
 *   \param extent1 The Array extent for the first dimension
 *   \param extent2 The Array extent for the second dimension
 *   \param extent3 The Array extent for the third dimension (If the
 *      Array has more than four dimensions this will be used for all
 *      remaining dimensions as well)
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param transformation The transformation to apply when accessing
 *      Array entries with valueAt() and setValueAt()
 *   \param storage The GeneralArrayStorage to use for the underlying
 *      blitz++ Array
 */
/*======================================================================*/
    Array(
        BlitzIndexT extent0, BlitzIndexT extent1, BlitzIndexT extent2,
        BlitzIndexT extent3, 
        blitz::TinyVector<double,Dim> const &elementSizeUm = 1.0,
        blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation =
        (traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one),
        blitz::GeneralArrayStorage<Dim> storage =
        blitz::GeneralArrayStorage<Dim>())
            : blitz::Array<DataT,Dim>(
                extent0, extent1, extent2, extent3, storage),
              _elementSizeUm(elementSizeUm), _transformation(transformation),
              p_interpolator(InterpolatorFactory<DataT,Dim>::get(LinearIP))
          {}

/*======================================================================*/
/*! 
 *   Constructor taking five extent parameters.
 *
 *   This constructor calls the corresponding blitz::Array constructor,
 *   sets the given element size and transformation matrix. The
 *   interpolator will be set to linear with ZeroBoundaryTreatment.
 *
 *   \param extent0 The Array extent for the zeroth dimension
 *   \param extent1 The Array extent for the first dimension
 *   \param extent2 The Array extent for the second dimension
 *   \param extent3 The Array extent for the third dimension
 *   \param extent4 The Array extent for the fourth dimension (If the
 *      Array has more than five dimensions this will be used for all
 *      remaining dimensions as well)
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param transformation The transformation to apply when accessing
 *      Array entries with valueAt() and setValueAt()
 *   \param storage The GeneralArrayStorage to use for the underlying
 *      blitz++ Array
 */
/*======================================================================*/
    Array(
        BlitzIndexT extent0, BlitzIndexT extent1, BlitzIndexT extent2,
        BlitzIndexT extent3, BlitzIndexT extent4,
        blitz::TinyVector<double,Dim> const &elementSizeUm = 1.0,
        blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation =
        (traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one),
        blitz::GeneralArrayStorage<Dim> storage =
        blitz::GeneralArrayStorage<Dim>())
            : blitz::Array<DataT,Dim>(
                extent0, extent1, extent2, extent3, extent4, storage),
              _elementSizeUm(elementSizeUm), _transformation(transformation),
              p_interpolator(InterpolatorFactory<DataT,Dim>::get(LinearIP))
          {}

/*======================================================================*/
/*! 
 *   Constructor taking six extent parameters.
 *
 *   This constructor calls the corresponding blitz::Array constructor,
 *   sets the given element size and transformation matrix. The
 *   interpolator will be set to linear with ZeroBoundaryTreatment.
 *
 *   \param extent0 The Array extent for the zeroth dimension
 *   \param extent1 The Array extent for the first dimension
 *   \param extent2 The Array extent for the second dimension
 *   \param extent3 The Array extent for the third dimension
 *   \param extent4 The Array extent for the fourth dimension
 *   \param extent5 The Array extent for the fifth dimension (If the
 *      Array has more than six dimensions this will be used for all
 *      remaining dimensions as well)
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param transformation The transformation to apply when accessing
 *      Array entries with valueAt() and setValueAt()
 *   \param storage The GeneralArrayStorage to use for the underlying
 *      blitz++ Array
 */
/*======================================================================*/
    Array(
        BlitzIndexT extent0, BlitzIndexT extent1, BlitzIndexT extent2,
        BlitzIndexT extent3, BlitzIndexT extent4, BlitzIndexT extent5,
        blitz::TinyVector<double,Dim> const &elementSizeUm = 1.0,
        blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation =
        (traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one),
        blitz::GeneralArrayStorage<Dim> storage =
        blitz::GeneralArrayStorage<Dim>())
            : blitz::Array<DataT,Dim>(
                extent0, extent1, extent2, extent3, extent4, extent5, storage),
              _elementSizeUm(elementSizeUm), _transformation(transformation),
              p_interpolator(InterpolatorFactory<DataT,Dim>::get(LinearIP))
          {}

/*======================================================================*/
/*! 
 *   Constructor taking seven extent parameters.
 *
 *   This constructor calls the corresponding blitz::Array constructor,
 *   sets the given element size and transformation matrix. The
 *   interpolator will be set to linear with ZeroBoundaryTreatment.
 *
 *   \param extent0 The Array extent for the zeroth dimension
 *   \param extent1 The Array extent for the first dimension
 *   \param extent2 The Array extent for the second dimension
 *   \param extent3 The Array extent for the third dimension
 *   \param extent4 The Array extent for the fourth dimension
 *   \param extent5 The Array extent for the fifth dimension
 *   \param extent6 The Array extent for the sixth dimension (If the
 *      Array has more than seven dimensions this will be used for all
 *      remaining dimensions as well)
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param transformation The transformation to apply when accessing
 *      Array entries with valueAt() and setValueAt()
 *   \param storage The GeneralArrayStorage to use for the underlying
 *      blitz++ Array
 */
/*======================================================================*/
    Array(
        BlitzIndexT extent0, BlitzIndexT extent1, BlitzIndexT extent2,
        BlitzIndexT extent3, BlitzIndexT extent4, BlitzIndexT extent5,
        BlitzIndexT extent6,
        blitz::TinyVector<double,Dim> const &elementSizeUm = 1.0,
        blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation =
        (traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one),
        blitz::GeneralArrayStorage<Dim> storage =
        blitz::GeneralArrayStorage<Dim>())
            : blitz::Array<DataT,Dim>(
                extent0, extent1, extent2, extent3, extent4, extent5, extent6,
                storage),
              _elementSizeUm(elementSizeUm), _transformation(transformation),
              p_interpolator(InterpolatorFactory<DataT,Dim>::get(LinearIP))
          {}

/*======================================================================*/
/*! 
 *   Constructor taking eight extent parameters.
 *
 *   This constructor calls the corresponding blitz::Array constructor,
 *   sets the given element size and transformation matrix. The
 *   interpolator will be set to linear with ZeroBoundaryTreatment.
 *
 *   \param extent0 The Array extent for the zeroth dimension
 *   \param extent1 The Array extent for the first dimension
 *   \param extent2 The Array extent for the second dimension
 *   \param extent3 The Array extent for the third dimension
 *   \param extent4 The Array extent for the fourth dimension
 *   \param extent5 The Array extent for the fifth dimension
 *   \param extent6 The Array extent for the sixth dimension
 *   \param extent7 The Array extent for the seventh dimension (If the
 *      Array has more than eight dimensions this will be used for all
 *      remaining dimensions as well)
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param transformation The transformation to apply when accessing
 *      Array entries with valueAt() and setValueAt()
 *   \param storage The GeneralArrayStorage to use for the underlying
 *      blitz++ Array
 */
/*======================================================================*/
    Array(
        BlitzIndexT extent0, BlitzIndexT extent1, BlitzIndexT extent2,
        BlitzIndexT extent3, BlitzIndexT extent4, BlitzIndexT extent5,
        BlitzIndexT extent6, BlitzIndexT extent7,
        blitz::TinyVector<double,Dim> const &elementSizeUm = 1.0,
        blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation =
        (traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one),
        blitz::GeneralArrayStorage<Dim> storage =
        blitz::GeneralArrayStorage<Dim>())
            : blitz::Array<DataT,Dim>(
                extent0, extent1, extent2, extent3, extent4, extent5, extent6,
                extent7, storage),
              _elementSizeUm(elementSizeUm), _transformation(transformation),
              p_interpolator(InterpolatorFactory<DataT,Dim>::get(LinearIP))
          {}

/*======================================================================*/
/*! 
 *   Constructor taking nine extent parameters.
 *
 *   This constructor calls the corresponding blitz::Array constructor,
 *   sets the given element size and transformation matrix. The
 *   interpolator will be set to linear with ZeroBoundaryTreatment.
 *
 *   \param extent0 The Array extent for the zeroth dimension
 *   \param extent1 The Array extent for the first dimension
 *   \param extent2 The Array extent for the second dimension
 *   \param extent3 The Array extent for the third dimension
 *   \param extent4 The Array extent for the fourth dimension
 *   \param extent5 The Array extent for the fifth dimension
 *   \param extent6 The Array extent for the sixth dimension
 *   \param extent7 The Array extent for the seventh dimension
 *   \param extent8 The Array extent for the eighth dimension (If the
 *      Array has more than nine dimensions this will be used for all
 *      remaining dimensions as well)
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param transformation The transformation to apply when accessing
 *      Array entries with valueAt() and setValueAt()
 *   \param storage The GeneralArrayStorage to use for the underlying
 *      blitz++ Array
 */
/*======================================================================*/
    Array(
        BlitzIndexT extent0, BlitzIndexT extent1, BlitzIndexT extent2,
        BlitzIndexT extent3, BlitzIndexT extent4, BlitzIndexT extent5,
        BlitzIndexT extent6, BlitzIndexT extent7, BlitzIndexT extent8,
        blitz::TinyVector<double,Dim> const &elementSizeUm = 1.0,
        blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation =
        (traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one),
        blitz::GeneralArrayStorage<Dim> storage =
        blitz::GeneralArrayStorage<Dim>())
            : blitz::Array<DataT,Dim>(
                extent0, extent1, extent2, extent3, extent4, extent5, extent6,
                extent7, extent8, storage),
              _elementSizeUm(elementSizeUm), _transformation(transformation),
              p_interpolator(InterpolatorFactory<DataT,Dim>::get(LinearIP))
          {}

/*======================================================================*/
/*! 
 *   Constructor taking ten extent parameters.
 *
 *   This constructor calls the corresponding blitz::Array constructor,
 *   sets the given element size and transformation matrix. The
 *   interpolator will be set to linear with ZeroBoundaryTreatment.
 *
 *   \param extent0 The Array extent for the zeroth dimension
 *   \param extent1 The Array extent for the first dimension
 *   \param extent2 The Array extent for the second dimension
 *   \param extent3 The Array extent for the third dimension
 *   \param extent4 The Array extent for the fourth dimension
 *   \param extent5 The Array extent for the fifth dimension
 *   \param extent6 The Array extent for the sixth dimension
 *   \param extent7 The Array extent for the seventh dimension
 *   \param extent8 The Array extent for the eighth dimension
 *   \param extent9 The Array extent for the nineth dimension (If the
 *      Array has more than ten dimensions this will be used for all
 *      remaining dimensions as well)
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param transformation The transformation to apply when accessing
 *      Array entries with valueAt() and setValueAt()
 *   \param storage The GeneralArrayStorage to use for the underlying
 *      blitz++ Array
 */
/*======================================================================*/
    Array(
        BlitzIndexT extent0, BlitzIndexT extent1, BlitzIndexT extent2,
        BlitzIndexT extent3, BlitzIndexT extent4, BlitzIndexT extent5,
        BlitzIndexT extent6, BlitzIndexT extent7, BlitzIndexT extent8,
        BlitzIndexT extent9,
        blitz::TinyVector<double,Dim> const &elementSizeUm = 1.0,
        blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation =
        (traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one),
        blitz::GeneralArrayStorage<Dim> storage =
        blitz::GeneralArrayStorage<Dim>())
            : blitz::Array<DataT,Dim>(
                extent0, extent1, extent2, extent3, extent4, extent5, extent6,
                extent7, extent8, extent9, storage),
              _elementSizeUm(elementSizeUm), _transformation(transformation),
              p_interpolator(InterpolatorFactory<DataT,Dim>::get(LinearIP))
          {}

/*======================================================================*/
/*! 
 *   Constructor taking eleven extent parameters.
 *
 *   This constructor calls the corresponding blitz::Array constructor,
 *   sets the given element size and transformation matrix. The
 *   interpolator will be set to linear with ZeroBoundaryTreatment.
 *
 *   \param extent0 The Array extent for the zeroth dimension
 *   \param extent1 The Array extent for the first dimension
 *   \param extent2 The Array extent for the second dimension
 *   \param extent3 The Array extent for the third dimension
 *   \param extent4 The Array extent for the fourth dimension
 *   \param extent5 The Array extent for the fifth dimension
 *   \param extent6 The Array extent for the sixth dimension
 *   \param extent7 The Array extent for the seventh dimension
 *   \param extent8 The Array extent for the eighth dimension
 *   \param extent9 The Array extent for the nineth dimension
 *   \param extent10 The Array extent for the tenth dimension (If the
 *      Array has more than eleven dimensions this will be used for all
 *      remaining dimensions as well)
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param transformation The transformation to apply when accessing
 *      Array entries with valueAt() and setValueAt()
 *   \param storage The GeneralArrayStorage to use for the underlying
 *      blitz++ Array
 */
/*======================================================================*/
    Array(
        BlitzIndexT extent0, BlitzIndexT extent1, BlitzIndexT extent2,
        BlitzIndexT extent3, BlitzIndexT extent4, BlitzIndexT extent5,
        BlitzIndexT extent6, BlitzIndexT extent7, BlitzIndexT extent8,
        BlitzIndexT extent9, BlitzIndexT extent10,
        blitz::TinyVector<double,Dim> const &elementSizeUm = 1.0,
        blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation =
        (traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one),
        blitz::GeneralArrayStorage<Dim> storage =
        blitz::GeneralArrayStorage<Dim>())
            : blitz::Array<DataT,Dim>(
                extent0, extent1, extent2, extent3, extent4, extent5, extent6,
                extent7, extent8, extent9, extent10, storage),
              _elementSizeUm(elementSizeUm), _transformation(transformation),
              p_interpolator(InterpolatorFactory<DataT,Dim>::get(LinearIP))
          {}

/*======================================================================*/
/*! 
 *   Pre-existing data constructor with strides without ownership passing.
 *
 *   This constructor creates an Array from an existing data block with
 *   the given set of strides. Data ownership is not passed to the Array.
 *
 *   The constructor sets the given element size and transformation matrix.
 *   The interpolator will be set to linear with ZeroBoundaryTreatment.
 *
 *   \param data The pointer to the first data element of the pre-allocated
 *      memory block
 *   \param shape The shape of the resulting Array
 *   \param stride The strides for the different dimensions
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param transformation The transformation to apply when accessing
 *      Array entries with valueAt() and setValueAt()
 *   \param storage The GeneralArrayStorage to use for the underlying
 *      blitz++ Array
 */
/*======================================================================*/
    Array(
        DataT * restrict data, blitz::TinyVector<BlitzIndexT,Dim> const &shape,
        blitz::TinyVector<BlitzIndexT,Dim> const &stride,
        blitz::TinyVector<double,Dim> const &elementSizeUm = 1.0,
        blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation =
        (traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one),
        blitz::GeneralArrayStorage<Dim> storage =
        blitz::GeneralArrayStorage<Dim>())
            : blitz::Array<DataT,Dim>(data, shape, stride, storage),
              _elementSizeUm(elementSizeUm), _transformation(transformation),
              p_interpolator(InterpolatorFactory<DataT,Dim>::get(LinearIP))
          {}

/*======================================================================*/
/*! 
 *   Pre-existing data constructor with ownership passing (if requested).
 *
 *   This constructor creates an Array from an existing data block.
 *
 *   The constructor sets the given element size and transformation matrix.
 *   The interpolator will be set to linear with ZeroBoundaryTreatment.
 *
 *   \param data The pointer to the first data element of the pre-allocated
 *      memory block
 *   \param shape The shape of the resulting Array
 *   \param deletionPolicy The deletion policy to apply.
 *      \li \c blitz::neverDeleteData Ownership is not passed
 *      \li \c blitz::duplicateData Create the Array from a copy of the data
 *         provided
 *      \li \c blitz::deleteDataWhenDone Pass ownership to this blitz++ Array.
 *         The memory block has to be allocated with \c new to make this work.
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param transformation The transformation to apply when accessing
 *      Array entries with valueAt() and setValueAt()
 *   \param storage The GeneralArrayStorage to use for the underlying
 *      blitz++ Array
 */
/*======================================================================*/
    Array(
        DataT * restrict data, blitz::TinyVector<BlitzIndexT,Dim> const &shape,
        blitz::preexistingMemoryPolicy deletionPolicy,
        blitz::TinyVector<double,Dim> const &elementSizeUm = 1.0,
        blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation =
        (traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one),
        blitz::GeneralArrayStorage<Dim> storage =
        blitz::GeneralArrayStorage<Dim>())
            : blitz::Array<DataT,Dim>(data, shape, deletionPolicy, storage),
              _elementSizeUm(elementSizeUm), _transformation(transformation),
              p_interpolator(InterpolatorFactory<DataT,Dim>::get(LinearIP))
          {}

/*======================================================================*/
/*! 
 *   Pre-existing data constructor with strides and ownership passing
 *   (if requested).
 *
 *   This constructor creates an Array from an existing data block.
 *
 *   The constructor sets the given element size and transformation matrix.
 *   The interpolator will be set to linear with ZeroBoundaryTreatment.
 *
 *   \param data The pointer to the first data element of the pre-allocated
 *      memory block
 *   \param shape The shape of the resulting Array
 *   \param stride The strides for the different dimensions
 *   \param deletionPolicy The deletion policy to apply.
 *      \li \c blitz::neverDeleteData Ownership is not passed
 *      \li \c blitz::duplicateData Create the Array from a copy of the data
 *         provided
 *      \li \c blitz::deleteDataWhenDone Pass ownership to this blitz++ Array.
 *         The memory block has to be allocated with \c new to make this work.
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param transformation The transformation to apply when accessing
 *      Array entries with valueAt() and setValueAt()
 *   \param storage The GeneralArrayStorage to use for the underlying
 *      blitz++ Array
 */
/*======================================================================*/
    Array(
        DataT * restrict data, blitz::TinyVector<BlitzIndexT,Dim> const &shape,
        blitz::TinyVector<BlitzIndexT,Dim> const &stride,
        blitz::preexistingMemoryPolicy deletionPolicy,
        blitz::TinyVector<double,Dim> const &elementSizeUm = 1.0,
        blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation =
        (traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one),
        blitz::GeneralArrayStorage<Dim> storage =
        blitz::GeneralArrayStorage<Dim>())
            : blitz::Array<DataT,Dim>(data, shape, stride, storage),
              _elementSizeUm(elementSizeUm), _transformation(transformation),
              p_interpolator(InterpolatorFactory<DataT,Dim>::get(LinearIP))
          {}

/*======================================================================*/
/*! 
 *   Empty Array Constructor of specified size and storage format.
 *
 *   This constructor creates an Array of specified dimensions and sets the
 *   element size and transformation to the given values.
 *   The interpolator will be set to linear with ZeroBoundaryTreatment.
 *
 *   \param extents The Array extents
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param transformation The transformation to apply to the query position
 *      before accessing the Array using valueAt() and setValueAt()
 *   \param storage The GeneralArrayStorage to use for the underlying
 *      blitz++ Array
 */
/*======================================================================*/
    Array(
        blitz::TinyVector<BlitzIndexT,Dim> const &extents,
        blitz::TinyVector<double,Dim> const &elementSizeUm = 1.0,
        blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation =
        (traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one),
        blitz::GeneralArrayStorage<Dim> storage =
        blitz::GeneralArrayStorage<Dim>())
            : blitz::Array<DataT,Dim>(extents, storage),
              _elementSizeUm(elementSizeUm), _transformation(transformation),
              p_interpolator(InterpolatorFactory<DataT,Dim>::get(LinearIP))
          {}

/*======================================================================*/
/*! 
 *   Empty Array Constructor of specified size and storage format with
 *   index offset.
 *
 *   This constructor creates an Array of specified dimensions and sets the
 *   element size and transformation to the given values.
 *   The interpolator will be set to linear with ZeroBoundaryTreatment.
 *
 *   \param lbounds The Array lower index bounds
 *   \param extents The Array extents
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param transformation The transformation to apply to the query position
 *      before accessing the Array using valueAt() and setValueAt()
 *   \param storage The GeneralArrayStorage to use for the underlying
 *      blitz++ Array
 */
/*======================================================================*/
    Array(
        blitz::TinyVector<BlitzIndexT,Dim> const &lbounds,
        blitz::TinyVector<BlitzIndexT,Dim> const &extents,
        blitz::TinyVector<double,Dim> const &elementSizeUm = 1.0,
        blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation =
        (traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one),
        blitz::GeneralArrayStorage<Dim> storage =
        blitz::GeneralArrayStorage<Dim>())
            : blitz::Array<DataT,Dim>(lbounds, extents, storage),
              _elementSizeUm(elementSizeUm), _transformation(transformation),
              p_interpolator(InterpolatorFactory<DataT,Dim>::get(LinearIP))
          {}

/*======================================================================*/
/*! 
 *   Empty Array constructor with specified addressing space for one
 *   dimension.
 *
 *   The Range argument indicates that the Array indices should start at
 *   the lower bound of the Range and end at the upper bound. e.g.
 *   \c Array<float,1>(blitz::Range(5, 15)) will create a new
 *   1-D Array of length 11, starting at index 5.
 *
 *   \param r0 The range of the zeroth dimension. If the Array has more than
 *      one dimension this range is used for the remaining dimensions as well.
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param transformation The transformation to apply to the query position
 *      before accessing the Array using valueAt() and setValueAt()
 *   \param storage The GeneralArrayStorage to use for the underlying
 *      blitz++ Array
 */
/*======================================================================*/
    Array(
        blitz::Range const &r0,
        blitz::TinyVector<double,Dim> const &elementSizeUm = 1.0,
        blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation =
        (traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one),
        blitz::GeneralArrayStorage<Dim> storage =
        blitz::GeneralArrayStorage<Dim>())
            : blitz::Array<DataT,Dim>(r0, storage),
              _elementSizeUm(elementSizeUm), _transformation(transformation),
              p_interpolator(InterpolatorFactory<DataT,Dim>::get(LinearIP))
          {}

/*======================================================================*/
/*! 
 *   Empty Array constructor with specified addressing space for two
 *   dimensions.
 *
 *   The Range argument indicates that the Array indices should start at
 *   the lower bound of the Range and end at the upper bound. e.g.
 *   \c Array<float,1>(blitz::Range(5, 15)) will create a new
 *   1-D Array of length 11, starting at index 5.
 *
 *   \param r0 The range of the zeroth dimension.
 *   \param r1 The range of the first dimension. If the Array has more than
 *      two dimensions this range is used for the remaining dimensions as well.
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param transformation The transformation to apply to the query position
 *      before accessing the Array using valueAt() and setValueAt()
 *   \param storage The GeneralArrayStorage to use for the underlying
 *      blitz++ Array
 */
/*======================================================================*/
    Array(
        blitz::Range const &r0, blitz::Range const &r1,
        blitz::TinyVector<double,Dim> const &elementSizeUm = 1.0,
        blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation =
        (traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one),
        blitz::GeneralArrayStorage<Dim> storage =
        blitz::GeneralArrayStorage<Dim>())
            : blitz::Array<DataT,Dim>(r0, r1, storage),
              _elementSizeUm(elementSizeUm), _transformation(transformation),
              p_interpolator(InterpolatorFactory<DataT,Dim>::get(LinearIP))
          {}

/*======================================================================*/
/*! 
 *   Empty Array constructor with specified addressing space for three
 *   dimensions.
 *
 *   The Range argument indicates that the Array indices should start at
 *   the lower bound of the Range and end at the upper bound. e.g.
 *   \c Array<float,1>(blitz::Range(5, 15)) will create a new
 *   1-D Array of length 11, starting at index 5.
 *
 *   \param r0 The range of the zeroth dimension.
 *   \param r1 The range of the first dimension.
 *   \param r2 The range of the second dimension. If the Array has more than
 *      three dimensions this range is used for the remaining dimensions as
 *      well.
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param transformation The transformation to apply to the query position
 *      before accessing the Array using valueAt() and setValueAt()
 *   \param storage The GeneralArrayStorage to use for the underlying
 *      blitz++ Array
 */
/*======================================================================*/
    Array(
        blitz::Range const &r0, blitz::Range const &r1, blitz::Range const &r2,
        blitz::TinyVector<double,Dim> const &elementSizeUm = 1.0,
        blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation =
        (traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one),
        blitz::GeneralArrayStorage<Dim> storage =
        blitz::GeneralArrayStorage<Dim>())
            : blitz::Array<DataT,Dim>(r0, r1, r2, storage),
              _elementSizeUm(elementSizeUm), _transformation(transformation),
              p_interpolator(InterpolatorFactory<DataT,Dim>::get(LinearIP))
          {}

/*======================================================================*/
/*! 
 *   Empty Array constructor with specified addressing space for four
 *   dimensions.
 *
 *   The Range argument indicates that the Array indices should start at
 *   the lower bound of the Range and end at the upper bound. e.g.
 *   \c Array<float,1>(blitz::Range(5, 15)) will create a new
 *   1-D Array of length 11, starting at index 5.
 *
 *   \param r0 The range of the zeroth dimension.
 *   \param r1 The range of the first dimension.
 *   \param r2 The range of the second dimension.
 *   \param r3 The range of the third dimension. If the Array has more than
 *      four dimensions this range is used for the remaining dimensions as
 *      well.
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param transformation The transformation to apply to the query position
 *      before accessing the Array using valueAt() and setValueAt()
 *   \param storage The GeneralArrayStorage to use for the underlying
 *      blitz++ Array
 */
/*======================================================================*/
    Array(
        blitz::Range const &r0, blitz::Range const &r1, blitz::Range const &r2,
        blitz::Range const &r3,
        blitz::TinyVector<double,Dim> const &elementSizeUm = 1.0,
        blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation =
        (traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one),
        blitz::GeneralArrayStorage<Dim> storage =
        blitz::GeneralArrayStorage<Dim>())
            : blitz::Array<DataT,Dim>(r0, r1, r2, r3, storage),
              _elementSizeUm(elementSizeUm), _transformation(transformation),
              p_interpolator(InterpolatorFactory<DataT,Dim>::get(LinearIP))
          {}

/*======================================================================*/
/*! 
 *   Empty Array constructor with specified addressing space for five
 *   dimensions.
 *
 *   The Range argument indicates that the Array indices should start at
 *   the lower bound of the Range and end at the upper bound. e.g.
 *   \c Array<float,1>(blitz::Range(5, 15)) will create a new
 *   1-D Array of length 11, starting at index 5.
 *
 *   \param r0 The range of the zeroth dimension.
 *   \param r1 The range of the first dimension.
 *   \param r2 The range of the second dimension.
 *   \param r3 The range of the third dimension.
 *   \param r4 The range of the fourth dimension. If the Array has more than
 *      five dimensions this range is used for the remaining dimensions as
 *      well.
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param transformation The transformation to apply to the query position
 *      before accessing the Array using valueAt() and setValueAt()
 *   \param storage The GeneralArrayStorage to use for the underlying
 *      blitz++ Array
 */
/*======================================================================*/
    Array(
        blitz::Range const &r0, blitz::Range const &r1, blitz::Range const &r2,
        blitz::Range const &r3, blitz::Range const &r4,
        blitz::TinyVector<double,Dim> const &elementSizeUm = 1.0,
        blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation =
        (traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one),
        blitz::GeneralArrayStorage<Dim> storage =
        blitz::GeneralArrayStorage<Dim>())
            : blitz::Array<DataT,Dim>(r0, r1, r2, r3, r4, storage),
              _elementSizeUm(elementSizeUm), _transformation(transformation),
              p_interpolator(InterpolatorFactory<DataT,Dim>::get(LinearIP))
          {}

/*======================================================================*/
/*! 
 *   Empty Array constructor with specified addressing space for six
 *   dimensions.
 *
 *   The Range argument indicates that the Array indices should start at
 *   the lower bound of the Range and end at the upper bound. e.g.
 *   \c Array<float,1>(blitz::Range(5, 15)) will create a new
 *   1-D Array of length 11, starting at index 5.
 *
 *   \param r0 The range of the zeroth dimension.
 *   \param r1 The range of the first dimension.
 *   \param r2 The range of the second dimension.
 *   \param r3 The range of the third dimension.
 *   \param r4 The range of the fourth dimension.
 *   \param r5 The range of the fifth dimension. If the Array has more than
 *      six dimensions this range is used for the remaining dimensions as
 *      well.
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param transformation The transformation to apply to the query position
 *      before accessing the Array using valueAt() and setValueAt()
 *   \param storage The GeneralArrayStorage to use for the underlying
 *      blitz++ Array
 */
/*======================================================================*/
    Array(
        blitz::Range const &r0, blitz::Range const &r1, blitz::Range const &r2,
        blitz::Range const &r3, blitz::Range const &r4, blitz::Range const &r5,
        blitz::TinyVector<double,Dim> const &elementSizeUm = 1.0,
        blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation =
        (traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one),
        blitz::GeneralArrayStorage<Dim> storage =
        blitz::GeneralArrayStorage<Dim>())
            : blitz::Array<DataT,Dim>(r0, r1, r2, r3, r4, r5, storage),
              _elementSizeUm(elementSizeUm), _transformation(transformation),
              p_interpolator(InterpolatorFactory<DataT,Dim>::get(LinearIP))
          {}

/*======================================================================*/
/*! 
 *   Empty Array constructor with specified addressing space for seven
 *   dimensions.
 *
 *   The Range argument indicates that the Array indices should start at
 *   the lower bound of the Range and end at the upper bound. e.g.
 *   \c Array<float,1>(blitz::Range(5, 15)) will create a new
 *   1-D Array of length 11, starting at index 5.
 *
 *   \param r0 The range of the zeroth dimension.
 *   \param r1 The range of the first dimension.
 *   \param r2 The range of the second dimension.
 *   \param r3 The range of the third dimension.
 *   \param r4 The range of the fourth dimension.
 *   \param r5 The range of the fifth dimension.
 *   \param r6 The range of the sixth dimension. If the Array has more than
 *      seven dimensions this range is used for the remaining dimensions as
 *      well.
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param transformation The transformation to apply to the query position
 *      before accessing the Array using valueAt() and setValueAt()
 *   \param storage The GeneralArrayStorage to use for the underlying
 *      blitz++ Array
 */
/*======================================================================*/
    Array(
        blitz::Range const &r0, blitz::Range const &r1, blitz::Range const &r2,
        blitz::Range const &r3, blitz::Range const &r4, blitz::Range const &r5,
        blitz::Range const &r6,
        blitz::TinyVector<double,Dim> const &elementSizeUm = 1.0,
        blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation =
        (traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one),
        blitz::GeneralArrayStorage<Dim> storage =
        blitz::GeneralArrayStorage<Dim>())
            : blitz::Array<DataT,Dim>(r0, r1, r2, r3, r4, r5, r6, storage),
              _elementSizeUm(elementSizeUm), _transformation(transformation),
              p_interpolator(InterpolatorFactory<DataT,Dim>::get(LinearIP))
          {}

/*======================================================================*/
/*! 
 *   Empty Array constructor with specified addressing space for eight
 *   dimensions.
 *
 *   The Range argument indicates that the Array indices should start at
 *   the lower bound of the Range and end at the upper bound. e.g.
 *   \c Array<float,1>(blitz::Range(5, 15)) will create a new
 *   1-D Array of length 11, starting at index 5.
 *
 *   \param r0 The range of the zeroth dimension.
 *   \param r1 The range of the first dimension.
 *   \param r2 The range of the second dimension.
 *   \param r3 The range of the third dimension.
 *   \param r4 The range of the fourth dimension.
 *   \param r5 The range of the fifth dimension.
 *   \param r6 The range of the sixth dimension.
 *   \param r7 The range of the seventh dimension. If the Array has more than
 *      eight dimensions this range is used for the remaining dimensions as
 *      well.
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param transformation The transformation to apply to the query position
 *      before accessing the Array using valueAt() and setValueAt()
 *   \param storage The GeneralArrayStorage to use for the underlying
 *      blitz++ Array
 */
/*======================================================================*/
    Array(
        blitz::Range const &r0, blitz::Range const &r1, blitz::Range const &r2,
        blitz::Range const &r3, blitz::Range const &r4, blitz::Range const &r5,
        blitz::Range const &r6, blitz::Range const &r7,
        blitz::TinyVector<double,Dim> const &elementSizeUm = 1.0,
        blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation =
        (traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one),
        blitz::GeneralArrayStorage<Dim> storage =
        blitz::GeneralArrayStorage<Dim>())
            : blitz::Array<DataT,Dim>(r0, r1, r2, r3, r4, r5, r6, r7, storage),
              _elementSizeUm(elementSizeUm), _transformation(transformation),
              p_interpolator(InterpolatorFactory<DataT,Dim>::get(LinearIP))
          {}

/*======================================================================*/
/*! 
 *   Empty Array constructor with specified addressing space for nine
 *   dimensions.
 *
 *   The Range argument indicates that the Array indices should start at
 *   the lower bound of the Range and end at the upper bound. e.g.
 *   \c Array<float,1>(blitz::Range(5, 15)) will create a new
 *   1-D Array of length 11, starting at index 5.
 *
 *   \param r0 The range of the zeroth dimension.
 *   \param r1 The range of the first dimension.
 *   \param r2 The range of the second dimension.
 *   \param r3 The range of the third dimension.
 *   \param r4 The range of the fourth dimension.
 *   \param r5 The range of the fifth dimension.
 *   \param r6 The range of the sixth dimension.
 *   \param r7 The range of the seventh dimension.
 *   \param r8 The range of the eighth dimension. If the Array has more than
 *      nine dimensions this range is used for the remaining dimensions as
 *      well.
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param transformation The transformation to apply to the query position
 *      before accessing the Array using valueAt() and setValueAt()
 *   \param storage The GeneralArrayStorage to use for the underlying
 *      blitz++ Array
 */
/*======================================================================*/
    Array(
        blitz::Range const &r0, blitz::Range const &r1, blitz::Range const &r2,
        blitz::Range const &r3, blitz::Range const &r4, blitz::Range const &r5,
        blitz::Range const &r6, blitz::Range const &r7, blitz::Range const &r8,
        blitz::TinyVector<double,Dim> const &elementSizeUm = 1.0,
        blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation =
        (traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one),
        blitz::GeneralArrayStorage<Dim> storage =
        blitz::GeneralArrayStorage<Dim>())
            : blitz::Array<DataT,Dim>(
                r0, r1, r2, r3, r4, r5, r6, r7, r8, storage),
              _elementSizeUm(elementSizeUm), _transformation(transformation),
              p_interpolator(InterpolatorFactory<DataT,Dim>::get(LinearIP))
          {}

/*======================================================================*/
/*! 
 *   Empty Array constructor with specified addressing space for ten
 *   dimensions.
 *
 *   The Range argument indicates that the Array indices should start at
 *   the lower bound of the Range and end at the upper bound. e.g.
 *   \c Array<float,1>(blitz::Range(5, 15)) will create a new
 *   1-D Array of length 11, starting at index 5.
 *
 *   \param r0 The range of the zeroth dimension.
 *   \param r1 The range of the first dimension.
 *   \param r2 The range of the second dimension.
 *   \param r3 The range of the third dimension.
 *   \param r4 The range of the fourth dimension.
 *   \param r5 The range of the fifth dimension.
 *   \param r6 The range of the sixth dimension.
 *   \param r7 The range of the seventh dimension.
 *   \param r8 The range of the eighth dimension.
 *   \param r9 The range of the nineth dimension. If the Array has more than
 *      ten dimensions this range is used for the remaining dimensions as
 *      well.
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param transformation The transformation to apply to the query position
 *      before accessing the Array using valueAt() and setValueAt()
 *   \param storage The GeneralArrayStorage to use for the underlying
 *      blitz++ Array
 */
/*======================================================================*/
    Array(
        blitz::Range const &r0, blitz::Range const &r1, blitz::Range const &r2,
        blitz::Range const &r3, blitz::Range const &r4, blitz::Range const &r5,
        blitz::Range const &r6, blitz::Range const &r7, blitz::Range const &r8,
        blitz::Range const &r9,
        blitz::TinyVector<double,Dim> const &elementSizeUm = 1.0,
        blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation =
        (traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one),
        blitz::GeneralArrayStorage<Dim> storage =
        blitz::GeneralArrayStorage<Dim>())
            : blitz::Array<DataT,Dim>(
                r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, storage),
              _elementSizeUm(elementSizeUm), _transformation(transformation),
              p_interpolator(InterpolatorFactory<DataT,Dim>::get(LinearIP))
          {}

/*======================================================================*/
/*! 
 *   Empty Array constructor with specified addressing space for eleven
 *   dimensions.
 *
 *   The Range argument indicates that the Array indices should start at
 *   the lower bound of the Range and end at the upper bound. e.g.
 *   \c Array<float,1>(blitz::Range(5, 15)) will create a new
 *   1-D Array of length 11, starting at index 5.
 *
 *   \param r0 The range of the zeroth dimension.
 *   \param r1 The range of the first dimension.
 *   \param r2 The range of the second dimension.
 *   \param r3 The range of the third dimension.
 *   \param r4 The range of the fourth dimension.
 *   \param r5 The range of the fifth dimension.
 *   \param r6 The range of the sixth dimension.
 *   \param r7 The range of the seventh dimension.
 *   \param r8 The range of the eighth dimension.
 *   \param r9 The range of the nineth dimension.
 *   \param r10 The range of the tenth dimension. If the Array has more than
 *      eleven dimensions this range is used for the remaining dimensions as
 *      well.
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param transformation The transformation to apply to the query position
 *      before accessing the Array using valueAt() and setValueAt()
 *   \param storage The GeneralArrayStorage to use for the underlying
 *      blitz++ Array
 */
/*======================================================================*/
    Array(
        blitz::Range const &r0, blitz::Range const &r1, blitz::Range const &r2,
        blitz::Range const &r3, blitz::Range const &r4, blitz::Range const &r5,
        blitz::Range const &r6, blitz::Range const &r7, blitz::Range const &r8,
        blitz::Range const &r9, blitz::Range const &r10,
        blitz::TinyVector<double,Dim> const &elementSizeUm = 1.0,
        blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation =
        (traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one),
        blitz::GeneralArrayStorage<Dim> storage =
        blitz::GeneralArrayStorage<Dim>())
            : blitz::Array<DataT,Dim>(
                r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, storage),
              _elementSizeUm(elementSizeUm), _transformation(transformation),
              p_interpolator(InterpolatorFactory<DataT,Dim>::get(LinearIP))
          {}

/*======================================================================*/
/*! 
 *   Reference Constructor.
 *
 *   This constructor creates a reference to another Array. This
 *   resembles the copy-Constructor of the blitz++ Arrays. It will not
 *   copy the array data. This is necessary to allow automatic type promotion
 *   and several Array assignment tricks known from blitz++. The meta-data
 *   will be copied to the new Array.
 *
 *   \param array The Array to create a reference to
 */
/*======================================================================*/
    Array(Array<DataT,Dim> const &array)
            : blitz::Array<DataT,Dim>(array),
              _elementSizeUm(array._elementSizeUm),
              _transformation(array._transformation),
              p_interpolator(array.p_interpolator->clone())
          {}

/*======================================================================*/
/*! 
 *   Mixed type Constructor.
 *
 *   This constructor is internally used by the () operators, don't use it
 *   directly to actually create an Array.
 *
 *   \param array The Array to pick a sub-Array from
 *   \param r0 The zeroth Range or index
 *   \param r1 The first Range or index
 *   \param r2 The second Range or index
 *   \param r3 The third Range or index
 *   \param r4 The fourth Range or index
 *   \param r5 The fifth Range or index
 *   \param r6 The sixth Range or index
 *   \param r7 The seventh Range or index
 *   \param r8 The eighth Range or index
 *   \param r9 The nineth Range or index
 *   \param r10 The tenth Range or index
 *
 *   \internal
 */
/*======================================================================*/
    template<int Dim2, typename R0, typename R1, typename R2, typename R3,
             typename R4, typename R5, typename R6, typename R7, typename R8,
             typename R9, typename R10>
    Array(Array<DataT,Dim2> &array, R0 r0, R1 r1, R2 r2,
          R3 r3, R4 r4, R5 r5, R6 r6, R7 r7, R8 r8, R9 r9, R10 r10)
            : blitz::Array<DataT,Dim>(
                array, r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10),
              _elementSizeUm(array._elementSizeUm),
              _transformation(array._transformation),
              p_interpolator(array.p_interpolator->clone())
          {}

/*======================================================================*/
/*! 
 *   blitz++ Array Copy Constructor.
 *
 *   This constructor creates an Array from the given blitz::Array.
 *   The data will be copied to the new Array and metadata set as given.
 *
 *   \param array The Array to copy
 *   \param elementSizeUm The voxel extents in micrometers
 *   \param transformation The transformation to apply to the query position
 *      before accessing the Array using valueAt() and setValueAt()
 */
/*======================================================================*/
    Array(
        blitz::Array<DataT,Dim> const &array,
        blitz::TinyVector<double,Dim> const &elementSizeUm = 1.0,
        blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation =
        (traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one))
            : blitz::Array<DataT,Dim>(array.shape()),
              _elementSizeUm(elementSizeUm), _transformation(transformation),
              p_interpolator(InterpolatorFactory<DataT,Dim>::get(LinearIP))
          {
            std::memcpy(
                this->data(), array.data(), array.size() * sizeof(DataT));
          }

/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    ~Array();
    
/*======================================================================*/
/*! 
 *   (Copy) Assignment operator.
 *
 *   This operator copies the values from the given Array into this Array,
 *   but leaves the meta-data of both Array untouched.
 *
 *   \param array The Array to copy
 */
/*======================================================================*/
    Array<DataT,Dim> &operator=(Array<DataT,Dim> const &array);

/*======================================================================*/
/*! 
 *   Assignment operator from blitz++ Array.
 *
 *   This operator copies the values from the given blitz++ Array into
 *   this Array.
 *
 *   \param array The Array to copy
 */
/*======================================================================*/
    Array<DataT,Dim> &operator=(blitz::Array<DataT,Dim> const &array);

/*======================================================================*/
/*! 
 *   Assignment operator from blitz++ Array expression.
 *
 *   This operator copies the values from the evaluation of the given
 *   Array expression into this Array.
 *
 *   \param expr The Array expression to evaluate
 */
/*======================================================================*/
    template<typename T_expr>
    Array<DataT,Dim> &operator=(blitz::ETBase<T_expr> const &expr);

/*======================================================================*/
/*! 
 *   List assignment operator.
 *
 *   This operator allows to initialize an Array like \c "A = 1, 2, 3".
 *
 *   \param x The first value in the list
 *
 *   \return The list initialization switch that parsed the rest of
 *      the input
 */
/*======================================================================*/
    blitz::ListInitializationSwitch<typename blitz::Array<DataT,Dim>, DataT*>
    operator=(DataT x)
          {
            return blitz::ListInitializationSwitch<
                typename blitz::Array<DataT,Dim>, DataT*>(*this, x);
          }

/*======================================================================*/
/*! 
 *   Get the element size of this Array.
 *
 *   \return The voxel extents of this Array in micrometers
 */
/*======================================================================*/
    blitz::TinyVector<double,Dim> const &elementSizeUm() const
          {
            return _elementSizeUm;
          }
    
/*======================================================================*/
/*! 
 *   Set the element size of this Array.
 *
 *   \param elementSizeUm The new voxel extents of this Array in micrometers
 */
/*======================================================================*/
    void setElementSizeUm(blitz::TinyVector<double,Dim> const &elementSizeUm)
          {
            this->_elementSizeUm = elementSizeUm;
          }

/*======================================================================*/
/*! 
 *   Get the homogeneous transformation matrix of this Array.
 *
 *   \return The transformation matrix of this Array
 */
/*======================================================================*/
    blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation() const
          {
            return this->_transformation;
          }
    
/*======================================================================*/
/*! 
 *   Set the homogeneous transformation matrix of this Array.
 *
 *   \param transformation The transformation matrix of this Array
 */
/*======================================================================*/
    void setTransformation(
        blitz::TinyMatrix<double,Dim+1,Dim+1> const &transformation)
          {
            this->_transformation = transformation;
          }

/*======================================================================*/
/*! 
 *   Get a read-only reference to the ArrayInterpolator applied to this
 *   Array.
 *
 *   \return The Interpolator of this Array
 */
/*======================================================================*/
    Interpolator<DataT,Dim> const &interpolator() const
          {
            return *(this->p_interpolator);
          }
    
/*======================================================================*/
/*! 
 *   Set the Interpolator applied to this Array.
 *
 *   \param interpolator The new Interpolator for this Array
 */
/*======================================================================*/
    void setInterpolator(Interpolator<DataT,Dim> const &interpolator)
          {
            delete this->p_interpolator;
            this->p_interpolator = interpolator.clone();
          }

/*======================================================================*/
/*! 
 *   Filter this Array with a recursive filter approximating a Gaussian
 *   (Deriche filter) along the selected dimension.
 *
 *   Let \f$\alpha := \frac{5}{2\sqrt{\pi}\sigma}\f$ and \f$I\f$ be a
 *   line of this Array to be filtered, then the recursive filter is
 *   defined by \f$\tilde I_i = f_i + g_i\f$ where
 *
 *   \f[ f_i := \frac{\left( 1 - e^{\alpha} \right)^2}{1 + 2\alpha
 e^{-\alpha} - e^{-2\alpha}} \left( I_i + e^{-\alpha} \left( \alpha - 1
 \right) I_{i-1} \right) + 2e^{-\alpha} f_{i-1} - e^{-2\alpha} f_{i-2} \f]
 *   
 *   and
 *
 *   \f[ g_i := \frac{\left( 1 - e^{\alpha} \right)^2}{1 + 2\alpha
 e^{-\alpha} - e^{-2\alpha}} \left( e^{-\alpha} \left( \alpha + 1 \right)
 I_{i+1} - e^{-2\alpha} I_{i+2} \right) + 2e^{-\alpha} g_{i+1} - e^{-2\alpha}
 g_{i+2} \f]
 *
 *   \param sigmaUm The standard deviation of the Gaussian that is approximated
 *      by the recursive filter in micrometers
 *   \param dim The dimension along the filtering is performed
 *
 *   \return A reference to this Array after filtering
 */
/*======================================================================*/ 
    Array<DataT,Dim>& filterDeriche(double sigmaUm, int dim);

/*======================================================================*/
/*! 
 *   Rescale the Array so that the new Array element size is the given
 *   target element size.
 *
 *   If down-scaling in any dimension is requested (increase in element size),
 *   the according dimension will first be convolved with a  Gaussian to
 *   avoid aliasing. The Interpolator associated with this Array will be
 *   applied.
 *
 *   \param targetElementSizeUm The target voxel extents in micrometers
 *
 *   \return A reference to this Array after rescaling
 */
/*======================================================================*/
    Array<DataT,Dim>& rescale(
        blitz::TinyVector<double,Dim> const &targetElementSizeUm,
        iRoCS::ProgressReporter *pr = NULL);

/*======================================================================*/
/*! 
 *   Set the Array data and meta-data according to a specification stored
 *   in an HDF5 file.
 *
 *   This function loads the specified HDF5 dataset and associated
 *   meta-data. If meta-data are missing, they will be set to default
 *   values, i. e. unit element size and identity transformation.
 *   The meta-data are expected to be found in dataset attributes 
 *   'element_size_um' (double vector of length Dim) and 'transformation'
 *   (double vector of length (Dim + 1) * (Dim + 1)).
 *
 *   \param fileName The hdf5 file to read the Array data from
 *   \param dataset The hdf5 dataset to read the Array data from
 *
 *   \exception BlitzH5Error If the dataset cannot be read this exception
 *      is thrown.
 */
/*======================================================================*/
    void load(std::string const &fileName, std::string const &dataset,
      iRoCS::ProgressReporter *progress = NULL);

/*======================================================================*/
/*! 
 *   Set the Array data and meta-data according to a specification stored
 *   in an HDF5 file.
 *
 *   This function loads the specified HDF5 dataset and associated
 *   meta-data. If meta-data are missing, they will be set to default
 *   values, i. e. unit element size and identity transformation.
 *   The meta-data are expected to be found in dataset attributes 
 *   'element_size_um' (double vector of length Dim) and 'transformation'
 *   (double vector of length (Dim + 1) * (Dim + 1)).
 *
 *   \param inFile  The hdf5 file to read the Array data from
 *   \param dataset The hdf5 dataset to read the Array data from
 *
 *   \exception BlitzH5Error If the dataset cannot be read this exception
 *      is thrown.
 */
/*======================================================================*/
    void load(BlitzH5File const &inFile, std::string const &dataset,
      iRoCS::ProgressReporter *progress = NULL);

/*======================================================================*/
/*! 
 *   Save the Array data and meta-data to an HDF5 file.
 *
 *   This function saves this Array to the specified HDF5 dataset.
 *   The meta-data are stored in dataset attributes 
 *   'element_size_um' (double vector of length Dim) and 'transformation'
 *   (double vector of length (Dim + 1) * (Dim + 1)).
 *
 *   \param fileName The hdf5 file to write the Array data to
 *   \param dataset The hdf5 dataset to write the Array data to
 *   \param compression The compression level to apply (0-9). 0 is no
 *      compression. Usually a good trade-off is a compression level of 3
 *
 *   \exception BlitzH5Error If the dataset cannot be written this exception
 *      is thrown.
 */
/*======================================================================*/
    void save(
        std::string const &fileName, std::string const &dataset,
        int compression = 0, iRoCS::ProgressReporter *progress = NULL) const;

/*======================================================================*/
/*! 
 *   Save the Array data and meta-data to an HDF5 file.
 *
 *   This function saves this Array to the specified HDF5 dataset.
 *   The meta-data are stored in dataset attributes 
 *   'element_size_um' (double vector of length Dim) and 'transformation'
 *   (double vector of length (Dim + 1) * (Dim + 1)).
 *
 *   \param outFile The hdf5 file to write the Array data to
 *   \param dataset The hdf5 dataset to write the Array data to
 *   \param compression The compression level to apply (0-9). 0 is no
 *      compression. Usually a good trade-off is a compression level of 3
 *
 *   \exception BlitzH5Error If the dataset cannot be written this exception
 *      is thrown.
 */
/*======================================================================*/
    void save(
        BlitzH5File &outFile, std::string const &dataset,
        int compression = 0, iRoCS::ProgressReporter *progress = NULL) const;

/*======================================================================*/
/*! 
 *   Get the lower bound of the Array bounding box after application of the
 *   transformation in micrometers.
 *
 *   \return The lower bound of the Array bounding box
 */
/*======================================================================*/
    blitz::TinyVector<double,Dim> lowerBoundUm() const;

/*======================================================================*/
/*! 
 *   Get the upper bound of the Array bounding box after application of the
 *   transformation in micrometers.
 *
 *   \return The upper bound of the Array bounding box
 */
/*======================================================================*/
    blitz::TinyVector<double,Dim> upperBoundUm() const;

/*======================================================================*/
/*! 
 *   Get the Array value at the model (sub-pixel) position given.
 *
 *   To obtain the Array value first the transform is applied to the given
 *   micrometer position, the resulting position is divided by the element
 *   size to obtain the (sub-pixel) Array index to access. Then the
 *   ArrayInterpolator (with associated BoundaryTreatment) is applied to
 *   obtain the interpolated Array value.
 *
 *   \f[ \textrm{valueAt}(\vec{x}) = A\left(\left( \mathbf{1}_{\mathrm{Dim}},
 \vec{0} \right) \mathbf{T} \left(\vec{x}, 1\right)^T / \mathrm{elementSizeUm}
 \right) \f]
 *
 *   \param positionUm The micrometer position to query
 *
 *   \return The interpolated value at the given position
 */
/*======================================================================*/
    DataT valueAt(blitz::TinyVector<double,Dim> const &positionUm)
        const;

  protected:
    
/*======================================================================*/
/*! 
 *   The voxel extents in micrometers per voxel
 */
/*======================================================================*/
    blitz::TinyVector<double,Dim> _elementSizeUm;
    
/*======================================================================*/
/*! 
 *   The transformation matrix to apply when accessing the Array using
 *   valueAt() and setValueAt()
 */
/*======================================================================*/
    blitz::TinyMatrix<double,Dim+1,Dim+1> _transformation;
    
/*======================================================================*/
/*! 
 *   The ArrayInterpolator to use when accessing sub-pixel Array positions
 *   using valueAt() and setValueAt()
 */
/*======================================================================*/
    Interpolator<DataT,Dim> *p_interpolator;

  };

}

#include "Array.icc"

#endif
