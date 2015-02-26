/**************************************************************************
 **       Title: Interpolation strategies and related functions and classes
 **    $RCSfile:$
 **   $Revision:$ $Name:$
 **       $Date:$
 **   Copyright: GPL $Author:$
 ** Description:
 **
 **   Implementation of several boundary treatments when accessing
 **   out-of-bounds Array positions as for example during filtering.
 **
 **************************************************************************/

/*======================================================================*/
/*!
 *  \file Interpolator.hh
 *  \brief Classes and functions for sub-pixel Array access with different
 *    interpolation strategies.
 */
/*======================================================================*/

#ifndef ATBINTERPOLATOR_HH
#define ATBINTERPOLATOR_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <algorithm>
#include <vector>
#include <complex>

#ifdef HAVE_BLITZ_V9
#include <blitz/tinyvec-et.h>
#endif

#include "BoundaryTreatment.hh"

namespace atb
{
  
/*======================================================================*/
/*! 
 *   \enum InterpolationType Interpolator.hh "libArrayToolbox/Interpolator.hh"
 *   \brief The InterpolatorType enum contains entries for all
 *     implemented interpolation strategies when accessing sub-pixel positions
 *     within an Array.
 */
/*======================================================================*/
  enum InterpolationType {
      /** Return the pixel value of the closest Array pixel
       *  to the query position (nearest neighbor interpolation) */
      NearestIP = 0x0001,
      /** Interpolate linearly between the values of adjacent
       *  pixels (d-linear interpolation) */
      LinearIP = 0x0002,
      /** Fit a cubic function through the local neighborhood
       *  around the query position and return it's value at the query
       *  position. (d-cubic interpolation) */
      CubicIP = 0x0004,
      /** Return the minimum of adjacent pixel values. If integer positions
       *  are queried the value at the given position is returned */
      MinimumIP = 0x0008,
      /** Return the maximum of adjacent pixel values. If integer positions
       *  are queried the value at the given position is returned */
      MaximumIP = 0x0010,
      /** Return the median of adjacent pixel values. If integer positions
       *  are queried the value at the given position is returned */
      MedianIP = 0x0020
  };

/*======================================================================*/
/*!
 *  \class Interpolator Interpolator.hh "libArrayToolbox/Interpolator.hh"
 *  \brief The Interpolator class is the base class of all classes
 *    implementing interpolation strategies for sub-pixel Array access.
 *
 *  \see BoundaryTreatment, ValueBoundaryTreatment, CyclicBoundaryTreatment,
 *    RepeatBoundaryTreatment, MirrorBoundaryTreatment, CropBoundaryTreatment
 */
/*======================================================================*/
  template<typename DataT,int Dim>
  class Interpolator 
  {

  public:
    
/*======================================================================*/
/*! 
 *   Constructor.
 *
 *   \param bt The boundary treatment to apply when out-of-Array positions
 *     are requested.
 *   \param boundaryValue The default out-of-Array value (for ValueBT)
 */
/*======================================================================*/
    Interpolator(BoundaryTreatmentType bt = ValueBT,
                 DataT const &boundaryValue = traits<DataT>::zero);

/*======================================================================*/
/*! 
 *   Copy Constructor.
 *
 *   \param ip  The Interpolator object to copy
 */
/*======================================================================*/
    Interpolator(Interpolator<DataT,Dim> const &ip);

/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    virtual ~Interpolator();

/*======================================================================*/
/*! 
 *   Copy assignment operator.
 *
 *   \param ip  The Interpolator object to copy
 *
 *   \return A reference to this Interpolator object for operator
 *      chaining
 */
/*======================================================================*/
    Interpolator<DataT,Dim> &operator=(
        Interpolator<DataT,Dim> const &ip);

/*======================================================================*/
/*! 
 *   Get a deep copy of this Interpolator object
 *
 *   return A deep copy of this Interpolator object
 */
/*======================================================================*/
    virtual Interpolator<DataT,Dim> *clone() const = 0;

/*======================================================================*/
/*! 
 *   Get the corresponding enumeration value for the interpolation type
 *
 *   \return The enumeration value corresponding to the interpolation type
 */
/*======================================================================*/
    virtual InterpolationType type() const = 0;
    
/*======================================================================*/
/*! 
 *   Get a read-only reference to the Interpolator's boundary treatment
 *   object
 *
 *   \return A reference to the BoundaryTreatment object associated with
 *     this Interpolator
 */
/*======================================================================*/
    BoundaryTreatment<DataT,Dim> const &boundaryTreatment() const;

/*======================================================================*/
/*! 
 *   Get a random-access reference to the Interpolator's boundary treatment
 *   object
 *
 *   \return A reference to the BoundaryTreatment object associated with
 *     this Interpolator
 */
/*======================================================================*/
    BoundaryTreatment<DataT,Dim> &boundaryTreatment();

/*======================================================================*/
/*! 
 *   Set the Interpolator's boundary treatment type
 *
 *   \param bt             The new boundary treatment type
 *   \param boundaryValue  The default out-of-Array value (for ValueBT)
 */
/*======================================================================*/
    void setBoundaryTreatment(
        BoundaryTreatmentType bt,
        DataT const &boundaryValue = traits<DataT>::zero);

/*======================================================================*/
/*! 
 *   Get the Array value at the given sub-pixel position. For out-of-Array
 *   coordinates the corresponding boundary treatment will be applied
 *
 *   \param data  The Array to read a value from
 *   \param pos   The position to read
 *
 *   \return The Array value at the specified sub-pixel position
 */
/*======================================================================*/
    virtual DataT get(
        blitz::Array<DataT,Dim> const &data,
        blitz::TinyVector<double,Dim> const &pos) const = 0;

  protected:
    
    BoundaryTreatment<DataT,Dim> *p_bt;

  };

/*======================================================================*/
/*!
 *  \class NearestInterpolator Interpolator.hh "libArrayToolbox/Interpolator.hh"
 *  \brief The NearestInterpolator class provides sub-pixel access to blitz++
 *     Arrays using the nearest neighbor startegy.
 *
 *  \see BoundaryTreatment, ValueBoundaryTreatment, CyclicBoundaryTreatment,
 *    RepeatBoundaryTreatment, MirrorBoundaryTreatment, CropBoundaryTreatment
 */
/*======================================================================*/
  template<typename DataT,int Dim>
  class NearestInterpolator : public Interpolator<DataT,Dim>
  {

  public:

/*======================================================================*/
/*! 
 *   Constructor.
 *
 *   \param bt The boundary treatment to apply when out-of-Array positions
 *     are requested.
 *   \param boundaryValue The default out-of-Array value (for ValueBT)
 */
/*======================================================================*/
    NearestInterpolator(BoundaryTreatmentType bt = ValueBT,
                        DataT const &boundaryValue = traits<DataT>::zero);

/*======================================================================*/
/*! 
 *   Copy Constructor.
 *
 *   \param ip  The Interpolator object to copy
 */
/*======================================================================*/
    NearestInterpolator(NearestInterpolator<DataT,Dim> const &ip);

/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    ~NearestInterpolator();

/*======================================================================*/
/*! 
 *   Copy assignment operator.
 *
 *   \param ip  The Interpolator object to copy
 *
 *   \return A reference to this Interpolator object for operator
 *      chaining
 */
/*======================================================================*/
    NearestInterpolator<DataT,Dim> &operator=(
        NearestInterpolator<DataT,Dim> const &ip);

/*======================================================================*/
/*! 
 *   Get a deep copy of this Interpolator object
 *
 *   return A deep copy of this Interpolator object
 */
/*======================================================================*/
    Interpolator<DataT,Dim> *clone() const;

/*======================================================================*/
/*! 
 *   Get the corresponding enumeration value for the interpolation type
 *
 *   \return NearestIP
 */
/*======================================================================*/
    InterpolationType type() const;
    
/*======================================================================*/
/*! 
 *   Get the Array value at the given subpixel position.
 *   With this interpolator the value of the nearest integer position will
 *   be returned after application of the boundary treatment given.
 *
 *   \param data The Array to get a value from
 *   \param pos  The position
 *
 *   \return The value at the position closest to the sub-pixel position
 *     given
 */
/*======================================================================*/
    DataT get(
        blitz::Array<DataT,Dim> const &data,
        blitz::TinyVector<double,Dim> const &pos) const;
  };
  
/*======================================================================*/
/*!
 *  \class LinearInterpolator Interpolator.hh "libArrayToolbox/Interpolator.hh"
 *  \brief The LinearInterpolator class provides sub-pixel access to blitz++
 *     Arrays using linear interpolation.
 *
 *  \see BoundaryTreatment, ValueBoundaryTreatment, CyclicBoundaryTreatment,
 *    RepeatBoundaryTreatment, MirrorBoundaryTreatment, CropBoundaryTreatment
 */
/*======================================================================*/
  template<typename DataT,int Dim>
  class LinearInterpolator : public Interpolator<DataT,Dim>
  {

  public:
    
/*======================================================================*/
/*! 
 *   Constructor.
 *
 *   \param bt The boundary treatment to apply when out-of-Array positions
 *     are requested.
 *   \param boundaryValue The default out-of-Array value (for ValueBT)
 */
/*======================================================================*/
    LinearInterpolator(BoundaryTreatmentType bt = ValueBT,
                       DataT const &boundaryValue = traits<DataT>::zero);

/*======================================================================*/
/*! 
 *   Copy Constructor.
 *
 *   \param ip  The Interpolator object to copy
 */
/*======================================================================*/
    LinearInterpolator(LinearInterpolator<DataT,Dim> const &ip);

/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    ~LinearInterpolator();

/*======================================================================*/
/*! 
 *   Copy assignment operator.
 *
 *   \param ip  The Interpolator object to copy
 *
 *   \return A reference to this Interpolator object for operator
 *      chaining
 */
/*======================================================================*/
    LinearInterpolator<DataT,Dim> &operator=(
        LinearInterpolator<DataT,Dim> const &ip);

/*======================================================================*/
/*! 
 *   Get a deep copy of this Interpolator object
 *
 *   return A deep copy of this Interpolator object
 */
/*======================================================================*/
    Interpolator<DataT,Dim> *clone() const;

/*======================================================================*/
/*! 
 *   Get the corresponding enumeration value for the interpolation type
 *
 *   \return LinearIP
 */
/*======================================================================*/
    InterpolationType type() const;
    
/*======================================================================*/
/*! 
 *   Get the Array value at the given subpixel position.
 *   With this interpolator the value will be a linear combination of
 *   the values at the surrounding integer positions after application
 *   of the boundary treatment given.
 *
 *   \param data The Array to get a value from
 *   \param pos  The position
 *
 *   \return The linearly interpolated Array value at the given position
 */
/*======================================================================*/
    DataT get(
        blitz::Array<DataT,Dim> const &data,
        blitz::TinyVector<double,Dim> const &pos) const;
  };

/*======================================================================*/
/*!
 *  \class CubicInterpolator Interpolator.hh "libArrayToolbox/Interpolator.hh"
 *  \brief The CubicInterpolator class provides sub-pixel access to blitz++
 *     Arrays using cubic interpolation.
 *
 *  \see BoundaryTreatment, ValueBoundaryTreatment, CyclicBoundaryTreatment,
 *    RepeatBoundaryTreatment, MirrorBoundaryTreatment, CropBoundaryTreatment
 */
/*======================================================================*/
  template<typename DataT,int Dim>
  class CubicInterpolator : public Interpolator<DataT,Dim>
  {

  public:
    
/*======================================================================*/
/*! 
 *   Constructor.
 *
 *   \param bt The boundary treatment to apply when out-of-Array positions
 *     are requested.
 *   \param boundaryValue The default out-of-Array value (for ValueBT)
 */
/*======================================================================*/
    CubicInterpolator(BoundaryTreatmentType bt = ValueBT,
                      DataT const &boundaryValue = traits<DataT>::zero);

/*======================================================================*/
/*! 
 *   Copy Constructor.
 *
 *   \param ip  The Interpolator object to copy
 */
/*======================================================================*/
    CubicInterpolator(CubicInterpolator<DataT,Dim> const &ip);

/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    ~CubicInterpolator();

/*======================================================================*/
/*! 
 *   Copy assignment operator.
 *
 *   \param ip  The Interpolator object to copy
 *
 *   \return A reference to this Interpolator object for operator
 *      chaining
 */
/*======================================================================*/
    CubicInterpolator<DataT,Dim> &operator=(
        CubicInterpolator<DataT,Dim> const &ip);

/*======================================================================*/
/*! 
 *   Get a deep copy of this Interpolator object
 *
 *   return A deep copy of this Interpolator object
 */
/*======================================================================*/
    Interpolator<DataT,Dim> *clone() const;

/*======================================================================*/
/*! 
 *   Get the corresponding enumeration value for the interpolation type
 *
 *   \return CubicIP
 */
/*======================================================================*/
    InterpolationType type() const;
    
/*======================================================================*/
/*! 
 *   Get the Array value at the given subpixel position.
 *   With this interpolator the value will be the evaluation of a locally
 *   fit cubic function to the data at the query position.
 *
 *   \param data The Array to get a value from
 *   \param pos  The position
 *
 *   \return The interpolated Array value at the given position using
 *     cubic interpolation
 */
/*======================================================================*/
    DataT get(
        blitz::Array<DataT,Dim> const &data,
        blitz::TinyVector<double,Dim> const &pos) const;
  };


/*======================================================================*/
/*!
 *  \class MinimumInterpolator Interpolator.hh "libArrayToolbox/Interpolator.hh"
 *  \brief The MinimumInterpolator class provides sub-pixel access to blitz++
 *     Arrays returning the minimum of all adjacent pixel values.
 *
 *  \see BoundaryTreatment, ValueBoundaryTreatment, CyclicBoundaryTreatment,
 *    RepeatBoundaryTreatment, MirrorBoundaryTreatment, CropBoundaryTreatment
 */
/*======================================================================*/
  template<typename DataT,int Dim>
  class MinimumInterpolator : public Interpolator<DataT,Dim>
  {

  public:
    
/*======================================================================*/
/*! 
 *   Constructor.
 *
 *   \param bt The boundary treatment to apply when out-of-Array positions
 *     are requested.
 *   \param boundaryValue The default out-of-Array value (for ValueBT)
 */
/*======================================================================*/
    MinimumInterpolator(BoundaryTreatmentType bt = ValueBT,
                        DataT const &boundaryValue = traits<DataT>::zero);

/*======================================================================*/
/*! 
 *   Copy Constructor.
 *
 *   \param ip  The Interpolator object to copy
 */
/*======================================================================*/
    MinimumInterpolator(MinimumInterpolator<DataT,Dim> const &ip);

/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    ~MinimumInterpolator();

/*======================================================================*/
/*! 
 *   Copy assignment operator.
 *
 *   \param ip  The Interpolator object to copy
 *
 *   \return A reference to this Interpolator object for operator
 *      chaining
 */
/*======================================================================*/
    MinimumInterpolator<DataT,Dim> &operator=(
        MinimumInterpolator<DataT,Dim> const &ip);

/*======================================================================*/
/*! 
 *   Get a deep copy of this Interpolator object
 *
 *   return A deep copy of this Interpolator object
 */
/*======================================================================*/
    Interpolator<DataT,Dim> *clone() const;

/*======================================================================*/
/*! 
 *   Get the corresponding enumeration value for the interpolation type
 *
 *   \return CubicIP
 */
/*======================================================================*/
    InterpolationType type() const;
    
/*======================================================================*/
/*! 
 *   Get the Array value at the given subpixel position.
 *   With this interpolator the value will be the minimum of all adjacent
 *   pixel values.
 *
 *   \param data The Array to get a value from
 *   \param pos  The position
 *
 *   \return The interpolated Array value at the given position
 */
/*======================================================================*/
    DataT get(
        blitz::Array<DataT,Dim> const &data,
        blitz::TinyVector<double,Dim> const &pos) const;
  };


/*======================================================================*/
/*!
 *  \class MaximumInterpolator Interpolator.hh "libArrayToolbox/Interpolator.hh"
 *  \brief The MaximumInterpolator class provides sub-pixel access to blitz++
 *     Arrays returning the minimum of all adjacent pixel values.
 *
 *  \see BoundaryTreatment, ValueBoundaryTreatment, CyclicBoundaryTreatment,
 *    RepeatBoundaryTreatment, MirrorBoundaryTreatment, CropBoundaryTreatment
 */
/*======================================================================*/
  template<typename DataT,int Dim>
  class MaximumInterpolator : public Interpolator<DataT,Dim>
  {

  public:
    
/*======================================================================*/
/*! 
 *   Constructor.
 *
 *   \param bt The boundary treatment to apply when out-of-Array positions
 *     are requested.
 *   \param boundaryValue The default out-of-Array value (for ValueBT)
 */
/*======================================================================*/
    MaximumInterpolator(BoundaryTreatmentType bt = ValueBT,
                        DataT const &boundaryValue = traits<DataT>::zero);

/*======================================================================*/
/*! 
 *   Copy Constructor.
 *
 *   \param ip  The Interpolator object to copy
 */
/*======================================================================*/
    MaximumInterpolator(MaximumInterpolator<DataT,Dim> const &ip);

/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    ~MaximumInterpolator();

/*======================================================================*/
/*! 
 *   Copy assignment operator.
 *
 *   \param ip  The Interpolator object to copy
 *
 *   \return A reference to this Interpolator object for operator
 *      chaining
 */
/*======================================================================*/
    MaximumInterpolator<DataT,Dim> &operator=(
        MaximumInterpolator<DataT,Dim> const &ip);

/*======================================================================*/
/*! 
 *   Get a deep copy of this Interpolator object
 *
 *   return A deep copy of this Interpolator object
 */
/*======================================================================*/
    Interpolator<DataT,Dim> *clone() const;

/*======================================================================*/
/*! 
 *   Get the corresponding enumeration value for the interpolation type
 *
 *   \return CubicIP
 */
/*======================================================================*/
    InterpolationType type() const;
    
/*======================================================================*/
/*! 
 *   Get the Array value at the given subpixel position.
 *   With this interpolator the value will be the minimum of all adjacent
 *   pixel values.
 *
 *   \param data The Array to get a value from
 *   \param pos  The position
 *
 *   \return The interpolated Array value at the given position
 */
/*======================================================================*/
    DataT get(
        blitz::Array<DataT,Dim> const &data,
        blitz::TinyVector<double,Dim> const &pos) const;
  };


/*======================================================================*/
/*!
 *  \class MedianInterpolator Interpolator.hh "libArrayToolbox/Interpolator.hh"
 *  \brief The MedianInterpolator class provides sub-pixel access to blitz++
 *     Arrays returning the median of all adjacent pixel values.
 *
 *  \see BoundaryTreatment, ValueBoundaryTreatment, CyclicBoundaryTreatment,
 *    RepeatBoundaryTreatment, MirrorBoundaryTreatment, CropBoundaryTreatment
 */
/*======================================================================*/
  template<typename DataT,int Dim>
  class MedianInterpolator : public Interpolator<DataT,Dim>
  {

  public:
    
/*======================================================================*/
/*! 
 *   Constructor.
 *
 *   \param bt The boundary treatment to apply when out-of-Array positions
 *     are requested.
 *   \param boundaryValue The default out-of-Array value (for ValueBT)
 */
/*======================================================================*/
    MedianInterpolator(BoundaryTreatmentType bt = ValueBT,
                        DataT const &boundaryValue = traits<DataT>::zero);

/*======================================================================*/
/*! 
 *   Copy Constructor.
 *
 *   \param ip  The Interpolator object to copy
 */
/*======================================================================*/
    MedianInterpolator(MedianInterpolator<DataT,Dim> const &ip);

/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    ~MedianInterpolator();

/*======================================================================*/
/*! 
 *   Copy assignment operator.
 *
 *   \param ip  The Interpolator object to copy
 *
 *   \return A reference to this Interpolator object for operator
 *      chaining
 */
/*======================================================================*/
    MedianInterpolator<DataT,Dim> &operator=(
        MedianInterpolator<DataT,Dim> const &ip);

/*======================================================================*/
/*! 
 *   Get a deep copy of this Interpolator object
 *
 *   return A deep copy of this Interpolator object
 */
/*======================================================================*/
    Interpolator<DataT,Dim> *clone() const;

/*======================================================================*/
/*! 
 *   Get the corresponding enumeration value for the interpolation type
 *
 *   \return CubicIP
 */
/*======================================================================*/
    InterpolationType type() const;
    
/*======================================================================*/
/*! 
 *   Get the Array value at the given subpixel position.
 *   With this interpolator the value will be the minimum of all adjacent
 *   pixel values.
 *
 *   \param data The Array to get a value from
 *   \param pos  The position
 *
 *   \return The interpolated Array value at the given position
 */
/*======================================================================*/
    DataT get(
        blitz::Array<DataT,Dim> const &data,
        blitz::TinyVector<double,Dim> const &pos) const;
  };

/*======================================================================*/
/*!
 *  \class InterpolatorFactory Interpolator.hh "libArrayToolbox/Interpolator.hh"
 *  \brief The InterpolatorFactory class contains a factory method for
 *    creating a new Interpolator object using the given InterpolationType
 *    and BoundaryTreatmentType with corresponding default boundary value.
 */
/*======================================================================*/
  template<typename DataT,int Dim>
  class InterpolatorFactory
  {

  public:
    
/*======================================================================*/
/*! 
 *   Get e new Interpolator object corresponding to the given InterpolationType
 *   enumeration value with specified boundary treatment. Ownership of the
 *   returned pointer is passed to the caller, who then is responsible for
 *   cleaning up the object using \c delete when the Interpolator object is
 *   not needed any more.
 *
 *   \param ipType The type of the interpolator
 *   \param btType The type of the boundary treatment
 *   \param boundaryValue  If btType == Value this value is returned when
 *     trying to access out-of-Array positions
 *
 *   \return A pointer to e newly created Interpolator object
 *
 *   \exception RuntimeError If no Interpolator object could be created
 *     this exception is thrown
 */
/*======================================================================*/
    static Interpolator<DataT,Dim> *get(
        InterpolationType ipType, BoundaryTreatmentType btType = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero);

  };
      

/*======================================================================*/
/*! 
 *   Interpolation within an n-dimensional blitz::Array. This function
 *   creates a new Interpolator object for accessing the Array position.
 *   If you want to query many positions you should explicitely create an
 *   Interpolator object instead and use its get() method. To create
 *   an Interpolator object for your data you can use the InterpolatorFactory.
 *
 *   \param data    The array a data element will be interpolated from
 *   \param pos     A TinyVector containing the subpixel position the
 *                  value will be interpolated for
 *   \param ipType  The interpolation type
 *   \param btType  The border treatment type
 *   \param boundaryValue   The out-of-Array value when btType == Value
 *
 *   \return The interpolated value for position pos in data block data
 *
 *   \see InterpolatorFactory, Interpolator
 */
/*======================================================================*/
  template<typename DataT,typename IndexT,int Dim>
  DataT interpolate(blitz::Array<DataT,Dim> const &data,
                    blitz::TinyVector<IndexT,Dim> const &pos,
                    InterpolationType ipType = LinearIP,
                    BoundaryTreatmentType btType = ValueBT,
                    DataT const &boundaryValue = traits<DataT>::zero);

}

#include "Interpolator.icc"

#endif
