/**************************************************************************
 **       Title: Boundary Treatment classes and functions
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
 *  \file BoundaryTreatment.hh
 *  \brief Classes and functions for handling out-of-Array access.
 */
/*======================================================================*/

#ifndef ATBBOUNDARYTREATMENT_HH
#define ATBBOUNDARYTREATMENT_HH

#include <blitz/array.h>

#include "TypeTraits.hh"
#include "RuntimeError.hh"

namespace atb
{

/*======================================================================*/
/*! 
 *   \enum BoundaryTreatmentType BoundaryTreatment.hh "libArrayToolbox/BoundaryTreatment.hh"
 *   \brief The BoundaryTreatmentType enum contains entries for all
 *     implemented boundary treatments.
 */
/*======================================================================*/
  enum BoundaryTreatmentType {
      /** Treat positions beyond the Array domain as having a
       *  constant value */
      ValueBT = 0x0001,
      /** Treat values beyond the Array as if the Array continues
       *  cyclically at the boundaries. This means every coordinate passed is
       *  regarded modulo the Array extents. */
      CyclicBT = 0x0002,
      /** Always pick the closest in-Array position to the position
       *  passed. Therefore pixels at the boundaries are virtually repeated. */
      RepeatBT = 0x0004,
      /** Mirror out-of-Array coordinates at the Array boundaries to
       *  get in-Array indices and values. */
      MirrorBT = 0x0008,
      /** Don't use out-of-bounds pixels at all. This is strictly
       *  spoken no boundary treatment. It's a filtering mode in which
       *  out-of-bounds pixels are just not used. If the kernel domain
       *  is no subset of the Array domain, the response will be divided by
       *  the fraction of the kernel that did not contribute.
       *  This makes some sense for smoothing kernels, but should be used with
       *  care. */
      CropBT = 0x0010
  };

/*======================================================================*/
/*!
 *  \class BoundaryTreatment BoundaryTreatment.hh "libArrayToolbox/BoundaryTreatment.hh"
 *  \brief The BoundaryTreatment class provides the interface to all
 *    specific BoundaryTreatment subclasses.
 */
/*======================================================================*/
  template< typename DataT, int Dim >
  class BoundaryTreatment
  {

  public:

/*======================================================================*/
/*! 
 *   Constructor.
 */
/*======================================================================*/
    BoundaryTreatment();

/*======================================================================*/
/*! 
 *   Copy Constructor.
 *
 *   \param bt  The BoundaryTreatment object to copy
 */
/*======================================================================*/
    BoundaryTreatment(BoundaryTreatment<DataT,Dim> const &bt);

/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    virtual ~BoundaryTreatment();

/*======================================================================*/
/*! 
 *   Copy assignment operator.
 *
 *   \param bt  The BoundaryTreatment object to copy
 *
 *   \return A reference to this BoundaryTreatment object for operator
 *      chaining
 */
/*======================================================================*/
    BoundaryTreatment<DataT,Dim> &operator=(
        BoundaryTreatment<DataT,Dim> const &bt);

/*======================================================================*/
/*! 
 *   Get a deep copy of this BoundaryTreatment object
 *
 *   \return A deep copy of this BoundaryTreatment object
 */
/*======================================================================*/
    virtual BoundaryTreatment<DataT,Dim> *clone() const = 0;

/*======================================================================*/
/*! 
 *   Get the corresponding enumeration value for the boundary treatment type
 *
 *   \return The enumeration value corresponding to the boundary treatment
 */
/*======================================================================*/
    virtual BoundaryTreatmentType type() const = 0;

/*======================================================================*/
/*! 
 *   The returned boolean indicates, whether the boundary treatment is able to
 *   transform the incoming coordinates into valid Array coordinates and
 *   therefore it indicates, whether the function getIndex() returns
 *   valid in-Array-positions.
 *
 *   \return \c true if the BoundaryTreatment can transform out-of-bounds
 *     indices into in-Array-indices, \c false otherwise
 */
/*======================================================================*/
    virtual bool providesIndices() const = 0;

/*======================================================================*/
/*! 
 *   Get the Array value at the given position. For out-of-Array coordinates
 *   the corresponding treatment will be applied
 *
 *   \param data  The Array to read a value from
 *   \param pos   The position to read
 *
 *   \return The Array value at the specified position after boundary
 *     treatment
 */
/*======================================================================*/
    virtual DataT get(blitz::Array<DataT,Dim> const &data,
                      blitz::TinyVector<ptrdiff_t,Dim> pos) const = 0;

/*======================================================================*/
/*! 
 *   Get the in-Array-Index to the given position. This function throws
 *   a RuntimeError if the BoundaryTreatment cannot return valid
 *   out-of-Array indices. This can be checked using the providesIndices
 *   flag
 *
 *   \param pos   The position to transform to in-Array-coordinates
 *   \param shape The Array shape
 *
 *   \return The in-Array position corresponding to the position that was
 *      passed to the function according to the used boundary treatment
 *
 *   \exception RuntimeError This exception is thrown if the BoundaryTreatment
 *     cannot generate in-Array positions from given out-of-Array positions,
 *     and an out-of-Array position is passed.
 */
/*======================================================================*/
    virtual blitz::TinyVector<ptrdiff_t,Dim> getIndex(
        blitz::TinyVector<ptrdiff_t,Dim> pos,
        blitz::TinyVector<ptrdiff_t,Dim> const &shape) const = 0;

/*======================================================================*/
/*! 
 *   Get the C-array value at the given position. For out-of-array positions
 *   the corresponding treatment will be applied
 *
 *   \param data    The C-array to read a value from
 *   \param pos     The position to read
 *   \param length  The array length
 *
 *   \return The array value at the specified position after boundary
 *     treatment
 */
/*======================================================================*/
    virtual DataT get(
        DataT const *data, ptrdiff_t pos, ptrdiff_t length) const = 0;

/*======================================================================*/
/*! 
 *   Get the in-array-index to the given position. This function throws
 *   a RuntimeError if the BoundaryTreatment cannot return valid
 *   out-of-array indices. This can be checked using the providesIndices
 *   flag
 *
 *   \param pos   The position to transform to in-Array-coordinates
 *   \param length The array length
 *
 *   \return The in-Array position corresponding to the position that was
 *      passed to the function according to the used boundary treatment
 *
 *   \exception RuntimeError This exception is thrown if the BoundaryTreatment
 *     cannot generate in-Array positions from given out-of-Array positions,
 *     and an out-of-Array position is passed.
 */
/*======================================================================*/
    virtual ptrdiff_t getIndex(ptrdiff_t pos, ptrdiff_t length) const = 0;

  };
  
/*======================================================================*/
/*!
 *  \class ValueBoundaryTreatment BoundaryTreatment.hh "libArrayToolbox/BoundaryTreatment.hh"
 *  \brief The ValueBoundaryTreatment class implements constant value border
 *    treatment when accessing Arrays. The default constant is zero.
 */
/*======================================================================*/
  template< typename DataT, int Dim >
  class ValueBoundaryTreatment : public BoundaryTreatment<DataT,Dim>
  {

  public:
    
/*======================================================================*/
/*! 
 *   Constructor.
 *
 *   \param value  The value to assume for out-of-array pixels
 */
/*======================================================================*/
    ValueBoundaryTreatment(DataT const &value = traits<DataT>::zero);

/*======================================================================*/
/*! 
 *   Copy Constructor.
 *
 *   \param bt  The BoundaryTreatment object to copy
 */
/*======================================================================*/
    ValueBoundaryTreatment(ValueBoundaryTreatment<DataT,Dim> const &bt);

/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    ~ValueBoundaryTreatment();

/*======================================================================*/
/*! 
 *   Copy assignment operator.
 *
 *   \param bt  The BoundaryTreatment object to copy
 *
 *   \return A reference to this BoundaryTreatment object for operator
 *      chaining
 */
/*======================================================================*/
    ValueBoundaryTreatment<DataT,Dim> &operator=(
        ValueBoundaryTreatment<DataT,Dim> const &bt);

/*======================================================================*/
/*! 
 *   Get a deep copy of this BoundaryTreatment object
 *
 *   \return A deep copy of this BoundaryTreatment object
 */
/*======================================================================*/
    BoundaryTreatment<DataT,Dim> *clone() const;

/*======================================================================*/
/*! 
 *   Get the corresponding enumeration value for the boundary treatment type
 *
 *   \return \c ValueBT
 */
/*======================================================================*/
    BoundaryTreatmentType type() const;

/*======================================================================*/
/*! 
 *   The returned boolean indicates, whether the boundary treatment is able to
 *   transform the incoming coordinates into valid Array coordinates and
 *   therefore it indicates, whether the function getIndex() returns
 *   valid in-Array-positions.
 *
 *   \return \c false
 */
/*======================================================================*/
    bool providesIndices() const;

/*======================================================================*/
/*! 
 *   Get the Array value at the given position. For out-of-bounds positions
 *   the provided constant value will be returned
 *
 *   \param data  The Array to read a value from
 *   \param pos   The position to read
 *
 *   \return The Array value at the specified position or the constant if the
 *      position is out-of-bounds
 */
/*======================================================================*/
    DataT get(blitz::Array<DataT,Dim> const &data,
              blitz::TinyVector<ptrdiff_t,Dim> pos) const;

/*======================================================================*/
/*! 
 *   Get the in-Array-Index to the given position. For the
 *   ValueBoundaryTreatment class this function just returns the given
 *   in-Array-position or throws an RuntimeError if an out-of-Array
 *   position is passed.
 *
 *   \param pos   The position to transform to in-Array-coordinates
 *   \param shape The Array shape
 *
 *   \return The position that was passed to the function
 *
 *   \exception RuntimeError This exception is thrown if an out-of-Array
 *      position is passed.
 */
/*======================================================================*/
    blitz::TinyVector<ptrdiff_t,Dim> getIndex(
        blitz::TinyVector<ptrdiff_t,Dim> pos,
        blitz::TinyVector<ptrdiff_t,Dim> const &shape) const;

/*======================================================================*/
/*! 
 *   Get the C-array value at the given position. For out-of-array positions
 *   the corresponding treatment will be applied
 *
 *   \param data    The C-array to read a value from
 *   \param pos     The position to read
 *   \param length  The array length
 *
 *   \return The array value at the specified position after boundary
 *     treatment
 */
/*======================================================================*/
    DataT get(DataT const *data, ptrdiff_t pos, ptrdiff_t length) const;

/*======================================================================*/
/*! 
 *   Get the in-array-index to the given position. This function throws
 *   a RuntimeError if the BoundaryTreatment cannot return valid
 *   out-of-array indices. This can be checked using the providesIndices
 *   flag
 *
 *   \param pos    The position to transform to in-Array-coordinates
 *   \param length The array length
 *
 *   \return The in-Array position corresponding to the position that was
 *      passed to the function according to the used boundary treatment
 *
 *   \exception RuntimeError This exception is thrown if the BoundaryTreatment
 *     cannot generate in-Array positions from given out-of-Array positions,
 *     and an out-of-Array position is passed.
 */
/*======================================================================*/
    ptrdiff_t getIndex(ptrdiff_t pos, ptrdiff_t length) const;

/*======================================================================*/
/*! 
 *   Get the value, that is returned when out-of-Array positions
 *   are requested.
 *
 *   \return The out-of-Array value
 */
/*======================================================================*/
    DataT const &boundaryValue() const;
    
/*======================================================================*/
/*! 
 *   Set the value, that is returned when out-of-Array positions
 *   are requested.
 *
 *   \param boundaryValue  The new out-of-Array value
 */
/*======================================================================*/
    void setBoundaryValue(DataT const &boundaryValue);

  private:
    
    DataT _value;

  };
  
/*======================================================================*/
/*!
 *  \class CyclicBoundaryTreatment BoundaryTreatment.hh "libArrayToolbox/BoundaryTreatment.hh"
 *  \brief The CyclicBoundaryTreatment class implements cyclic border treatment
 *    when accessing Arrays.
 */
/*======================================================================*/
  template< typename DataT, int Dim >
  class CyclicBoundaryTreatment : public BoundaryTreatment<DataT,Dim>
  {

  public:
    
/*======================================================================*/
/*! 
 *   Constructor.
 */
/*======================================================================*/
    CyclicBoundaryTreatment();

/*======================================================================*/
/*! 
 *   Copy Constructor.
 *
 *   \param bt  The BoundaryTreatment object to copy
 */
/*======================================================================*/
    CyclicBoundaryTreatment(CyclicBoundaryTreatment<DataT,Dim> const &bt);

/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    ~CyclicBoundaryTreatment();

/*======================================================================*/
/*! 
 *   Copy assignment operator.
 *
 *   \param bt  The BoundaryTreatment object to copy
 *
 *   \return A reference to this BoundaryTreatment object for operator
 *      chaining
 */
/*======================================================================*/
    CyclicBoundaryTreatment<DataT,Dim> &operator=(
        CyclicBoundaryTreatment<DataT,Dim> const &bt);

/*======================================================================*/
/*! 
 *   Get a deep copy of this BoundaryTreatment object
 *
 *   \return A deep copy of this BoundaryTreatment object
 */
/*======================================================================*/
    BoundaryTreatment<DataT,Dim> *clone() const;

/*======================================================================*/
/*! 
 *   Get the corresponding enumeration value for the boundary treatment type
 *
 *   \return \c CyclicBT
 */
/*======================================================================*/
    BoundaryTreatmentType type() const;

/*======================================================================*/
/*! 
 *   The returned boolean indicates, whether the boundary treatment is able to
 *   transform the incoming coordinates into valid Array coordinates and
 *   therefore it indicates, whether the function getIndex() returns
 *   valid in-Array-positions.
 *
 *   \return \c true
 */
/*======================================================================*/
    bool providesIndices() const;

/*======================================================================*/
/*! 
 *   Get the Array value at the given position. The position will be
 *   transformed modulo the Array size, leading to cyclic behaviour.
 *
 *   \param data  The Array to read a value from
 *   \param pos   The position to read
 *
 *   \return The Array value at the in-Array-position corresponding to the
 *      passed position 
 */
/*======================================================================*/
    DataT get(blitz::Array<DataT,Dim> const &data,
              blitz::TinyVector<ptrdiff_t,Dim> pos) const;
    
/*======================================================================*/
/*! 
 *   Get the in-Array-Index to the given position. The position will be
 *   transformed modulo the Array size, leading to cyclic behaviour.
 *
 *   \param pos   The position to transform to in-Array-coordinates
 *   \param shape The Array shape
 *
 *   \return The passed position modulo the Array shape
 */
/*======================================================================*/
    blitz::TinyVector<ptrdiff_t,Dim> getIndex(
        blitz::TinyVector<ptrdiff_t,Dim> pos,
        blitz::TinyVector<ptrdiff_t,Dim> const &shape) const;

/*======================================================================*/
/*! 
 *   Get the C-array value at the given position. For out-of-array positions
 *   the corresponding treatment will be applied
 *
 *   \param data    The C-array to read a value from
 *   \param pos     The position to read
 *   \param length  The array length
 *
 *   \return The array value at the specified position after boundary
 *     treatment
 */
/*======================================================================*/
    DataT get(
        DataT const *data, ptrdiff_t pos, ptrdiff_t length) const;

/*======================================================================*/
/*! 
 *   Get the in-array-index to the given position. This function throws
 *   a RuntimeError if the BoundaryTreatment cannot return valid
 *   out-of-array indices. This can be checked using the providesIndices
 *   flag
 *
 *   \param pos    The position to transform to in-Array-coordinates
 *   \param length The array length
 *
 *   \return The in-Array position corresponding to the position that was
 *      passed to the function according to the used boundary treatment
 *
 *   \exception RuntimeError This exception is thrown if the BoundaryTreatment
 *     cannot generate in-Array positions from given out-of-Array positions,
 *     and an out-of-Array position is passed.
 */
/*======================================================================*/
    ptrdiff_t getIndex(ptrdiff_t pos, ptrdiff_t length) const;

  };
  
/*======================================================================*/
/*!
 *  \class RepeatBoundaryTreatment BoundaryTreatment.hh "libArrayToolbox/BoundaryTreatment.hh"
 *  \brief The RepeatBoundaryTreatment class implements repeating border
 *    treatment when accessing Arrays.
 */
/*======================================================================*/
  template< typename DataT, int Dim >
  class RepeatBoundaryTreatment : public BoundaryTreatment<DataT,Dim>
  {

  public:

/*======================================================================*/
/*! 
 *   Constructor.
 */
/*======================================================================*/
    RepeatBoundaryTreatment();

/*======================================================================*/
/*! 
 *   Copy Constructor.
 *
 *   \param bt  The BoundaryTreatment object to copy
 */
/*======================================================================*/
    RepeatBoundaryTreatment(RepeatBoundaryTreatment<DataT,Dim> const &bt);

/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    ~RepeatBoundaryTreatment();

/*======================================================================*/
/*! 
 *   Copy assignment operator.
 *
 *   \param bt  The BoundaryTreatment object to copy
 *
 *   \return A reference to this BoundaryTreatment object for operator
 *      chaining
 */
/*======================================================================*/
    RepeatBoundaryTreatment<DataT,Dim> &operator=(
        RepeatBoundaryTreatment<DataT,Dim> const &bt);

/*======================================================================*/
/*! 
 *   Get a deep copy of this BoundaryTreatment object
 *
 *   \return A deep copy of this BoundaryTreatment object
 */
/*======================================================================*/
    BoundaryTreatment<DataT,Dim> *clone() const;

/*======================================================================*/
/*! 
 *   Get the corresponding enumeration value for the boundary treatment type
 *
 *   \return \c RepeatBT
 */
/*======================================================================*/
    BoundaryTreatmentType type() const;

/*======================================================================*/
/*! 
 *   The returned boolean indicates, whether the boundary treatment is able to
 *   transform the incoming coordinates into valid Array coordinates and
 *   therefore it indicates, whether the function getIndex() returns
 *   valid in-Array-positions.
 *
 *   \return \c true
 */
/*======================================================================*/
    bool providesIndices() const;

/*======================================================================*/
/*! 
 *   Get the Array value at the given position. For out-of-bounds positions
 *   the value at the nearest in Array position will be returned.
 *
 *   \param data  The Array to read a value from
 *   \param pos   The position to read
 *
 *   \return The Array value at the in-Array-position corresponding to the
 *      passed position 
 */
/*======================================================================*/
    DataT get(blitz::Array<DataT,Dim> const &data,
              blitz::TinyVector<ptrdiff_t,Dim> pos) const;

/*======================================================================*/
/*! 
 *   Get the in-Array-Index to the given position. For out-of-bounds positions
 *   the nearest in Array position will be returned.
 *
 *   \param pos   The position to transform to in-Array-coordinates
 *   \param shape The Array shape
 *
 *   \return The nearest in-Array position to the position passed
 */
/*======================================================================*/
    blitz::TinyVector<ptrdiff_t,Dim> getIndex(
        blitz::TinyVector<ptrdiff_t,Dim> pos,
        blitz::TinyVector<ptrdiff_t,Dim> const &shape) const;

/*======================================================================*/
/*! 
 *   Get the C-array value at the given position. For out-of-array positions
 *   the corresponding treatment will be applied
 *
 *   \param data    The C-array to read a value from
 *   \param pos     The position to read
 *   \param length  The array length
 *
 *   \return The array value at the specified position after boundary
 *     treatment
 */
/*======================================================================*/
    DataT get(
        DataT const *data, ptrdiff_t pos, ptrdiff_t length) const;

/*======================================================================*/
/*! 
 *   Get the in-array-index to the given position. This function throws
 *   a RuntimeError if the BoundaryTreatment cannot return valid
 *   out-of-array indices. This can be checked using the providesIndices
 *   flag
 *
 *   \param pos    The position to transform to in-Array-coordinates
 *   \param length The array length
 *
 *   \return The in-Array position corresponding to the position that was
 *      passed to the function according to the used boundary treatment
 *
 *   \exception RuntimeError This exception is thrown if the BoundaryTreatment
 *     cannot generate in-Array positions from given out-of-Array positions,
 *     and an out-of-Array position is passed.
 */
/*======================================================================*/
    ptrdiff_t getIndex(ptrdiff_t pos, ptrdiff_t length) const;

  };
  
/*======================================================================*/
/*!
 *  \class MirrorBoundaryTreatment BoundaryTreatment.hh "libArrayToolbox/BoundaryTreatment.hh"
 *  \brief The MirrorBoundaryTreatment class implements mirroring border
 *    treatment when accessing Arrays.
 */
/*======================================================================*/
  template< typename DataT, int Dim >
  class MirrorBoundaryTreatment : public BoundaryTreatment<DataT,Dim>
  {

  public:
    
/*======================================================================*/
/*! 
 *   Constructor.
 */
/*======================================================================*/
    MirrorBoundaryTreatment();

/*======================================================================*/
/*! 
 *   Copy Constructor.
 *
 *   \param bt  The BoundaryTreatment object to copy
 */
/*======================================================================*/
    MirrorBoundaryTreatment(MirrorBoundaryTreatment<DataT,Dim> const &bt);

/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    ~MirrorBoundaryTreatment();

/*======================================================================*/
/*! 
 *   Copy assignment operator.
 *
 *   \param bt  The BoundaryTreatment object to copy
 *
 *   \return A reference to this BoundaryTreatment object for operator
 *      chaining
 */
/*======================================================================*/
    MirrorBoundaryTreatment<DataT,Dim> &operator=(
        MirrorBoundaryTreatment<DataT,Dim> const &bt);

/*======================================================================*/
/*! 
 *   Get a deep copy of this BoundaryTreatment object
 *
 *   \return A deep copy of this BoundaryTreatment object
 */
/*======================================================================*/
    BoundaryTreatment<DataT,Dim> *clone() const;

/*======================================================================*/
/*! 
 *   Get the corresponding enumeration value for the boundary treatment type
 *
 *   \return \c MirrorBT
 */
/*======================================================================*/
    BoundaryTreatmentType type() const;

/*======================================================================*/
/*! 
 *   The returned boolean indicates, whether the boundary treatment is able to
 *   transform the incoming coordinates into valid Array coordinates and
 *   therefore it indicates, whether the function getIndex() returns
 *   valid in-Array-positions.
 *
 *   \return \c true
 */
/*======================================================================*/
    bool providesIndices() const;

/*======================================================================*/
/*! 
 *   Get the Array value at the given position. Out-of-bounds positions
 *   will be mirrored at the Array boundaries to obtain in-Array coordinates.
 *
 *   \param data  The Array to read a value from
 *   \param pos   The position to read
 *
 *   \return The Array value at the in-Array-position corresponding to the
 *      passed position 
 */
/*======================================================================*/
    DataT get(blitz::Array<DataT,Dim> const &data,
              blitz::TinyVector<ptrdiff_t,Dim> pos) const;

/*======================================================================*/
/*! 
 *   Get the in-Array-Index to the given position. Out-of-bounds positions
 *   will be mirrored at the Array boundaries to obtain in-Array coordinates.
 *
 *   \param pos   The position to transform to in-Array-coordinates
 *   \param shape The Array shape
 *
 *   \return The corresponding in-Array position to the position passed
 */
/*======================================================================*/
    blitz::TinyVector<ptrdiff_t,Dim> getIndex(
        blitz::TinyVector<ptrdiff_t,Dim> pos,
        blitz::TinyVector<ptrdiff_t,Dim> const &shape) const;

/*======================================================================*/
/*! 
 *   Get the C-array value at the given position. For out-of-array positions
 *   the corresponding treatment will be applied
 *
 *   \param data    The C-array to read a value from
 *   \param pos     The position to read
 *   \param length  The array length
 *
 *   \return The array value at the specified position after boundary
 *     treatment
 */
/*======================================================================*/
    DataT get(
        DataT const *data, ptrdiff_t pos, ptrdiff_t length) const;

/*======================================================================*/
/*! 
 *   Get the in-array-index to the given position. This function throws
 *   a RuntimeError if the BoundaryTreatment cannot return valid
 *   out-of-array indices. This can be checked using the providesIndices
 *   flag
 *
 *   \param pos    The position to transform to in-Array-coordinates
 *   \param length The array length
 *
 *   \return The in-Array position corresponding to the position that was
 *      passed to the function according to the used boundary treatment
 *
 *   \exception RuntimeError This exception is thrown if the BoundaryTreatment
 *     cannot generate in-Array positions from given out-of-Array positions,
 *     and an out-of-Array position is passed.
 */
/*======================================================================*/
    ptrdiff_t getIndex(ptrdiff_t pos, ptrdiff_t length) const;

  };

/*======================================================================*/
/*!
 *  \class CropBoundaryTreatment BoundaryTreatment.hh "libArrayToolbox/BoundaryTreatment.hh"
 *  \brief The CropBoundaryTreatment class has actually no function.
 *    It only exists for convenience and throws a RuntimeError when
 *    used with out-of-Array positions, because, there
 *    is no defined behaviour in those cases.
 */
/*======================================================================*/
  template< typename DataT, int Dim >
  class CropBoundaryTreatment : public BoundaryTreatment<DataT,Dim>
  {

  public:
    
/*======================================================================*/
/*! 
 *   Constructor.
 */
/*======================================================================*/
    CropBoundaryTreatment();

/*======================================================================*/
/*! 
 *   Copy Constructor.
 *
 *   \param bt  The BoundaryTreatment object to copy
 */
/*======================================================================*/
    CropBoundaryTreatment(CropBoundaryTreatment<DataT,Dim> const &bt);

/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    ~CropBoundaryTreatment();

/*======================================================================*/
/*! 
 *   Copy assignment operator.
 *
 *   \param bt  The BoundaryTreatment object to copy
 *
 *   \return A reference to this BoundaryTreatment object for operator
 *      chaining
 */
/*======================================================================*/
    CropBoundaryTreatment<DataT,Dim> &operator=(
        CropBoundaryTreatment<DataT,Dim> const &bt);

/*======================================================================*/
/*! 
 *   Get a deep copy of this BoundaryTreatment object
 *
 *   \return A deep copy of this BoundaryTreatment object
 */
/*======================================================================*/
    BoundaryTreatment<DataT,Dim> *clone() const;

/*======================================================================*/
/*! 
 *   Get the corresponding enumeration value for the boundary treatment type
 *
 *   \return \c CropBT
 */
/*======================================================================*/
    BoundaryTreatmentType type() const;

/*======================================================================*/
/*! 
 *   The returned boolean indicates, whether the boundary treatment is able to
 *   transform the incoming coordinates into valid Array coordinates and
 *   therefore it indicates, whether the function getIndex() returns
 *   valid in-Array-positions.
 *
 *   \return \c false
 */
/*======================================================================*/
    bool providesIndices() const;

/*======================================================================*/
/*! 
 *   Get the Array value at the given position. When trying to access an
 *   out-of-Array position a RuntimeError is thrown.
 *
 *   \param data  The Array to read a value from
 *   \param pos   The position to read
 *
 *   \return The Array value at the passed position
 *
 *   \exception RuntimeError If the position passed is not within the Array
 *     bounds this exception is thrown
 */
/*======================================================================*/
    DataT get(blitz::Array<DataT,Dim> const &data,
              blitz::TinyVector<ptrdiff_t,Dim> pos) const;

/*======================================================================*/
/*! 
 *   Get the in-Array-Index to the given position. When passing an
 *   out-of-Array position a RuntimeError is thrown.
 *
 *   \param pos   The position to transform to in-Array-coordinates
 *   \param shape The Array shape
 *
 *   \return The position passed
 *
 *   \exception RuntimeError If the position passed is not within the Array
 *     bounds this exception is thrown
 */
/*======================================================================*/
    blitz::TinyVector<ptrdiff_t,Dim> getIndex(
        blitz::TinyVector<ptrdiff_t,Dim> pos,
        blitz::TinyVector<ptrdiff_t,Dim> const &shape) const;

/*======================================================================*/
/*! 
 *   Get the C-array value at the given position. For out-of-array positions
 *   the corresponding treatment will be applied
 *
 *   \param data    The C-array to read a value from
 *   \param pos     The position to read
 *   \param length  The array length
 *
 *   \return The array value at the specified position after boundary
 *     treatment
 */
/*======================================================================*/
    DataT get(
        DataT const *data, ptrdiff_t pos, ptrdiff_t length) const;

/*======================================================================*/
/*! 
 *   Get the in-array-index to the given position. This function throws
 *   a RuntimeError if the BoundaryTreatment cannot return valid
 *   out-of-array indices. This can be checked using the providesIndices
 *   flag
 *
 *   \param pos    The position to transform to in-Array-coordinates
 *   \param length The array length
 *
 *   \return The in-Array position corresponding to the position that was
 *      passed to the function according to the used boundary treatment
 *
 *   \exception RuntimeError This exception is thrown if the BoundaryTreatment
 *     cannot generate in-Array positions from given out-of-Array positions,
 *     and an out-of-Array position is passed.
 */
/*======================================================================*/
    ptrdiff_t getIndex(ptrdiff_t pos, ptrdiff_t length) const;

  };

/*======================================================================*/
/*!
 *  \class BoundaryTreatmentFactory BoundaryTreatment.hh "libArrayToolbox/BoundaryTreatment.hh"
 *  \brief The BoundaryTreatmentFactory class contains a factory method
 *    for creating a BoundaryTreatment object from the given
 *    BoundaryTreatment enumeration value.
 */
/*======================================================================*/
  template<typename DataT, int Dim>
  class BoundaryTreatmentFactory
  {

  public:
    
/*======================================================================*/
/*! 
 *   Get a new BoundaryTreatment object corresponding to the given enumeration
 *   value. Ownership of the returned pointer is passed to the caller, who
 *   then is responsible for cleaning up the object using \c delete when
 *   the BoundaryTreatment object is not needed any more.
 *
 *   \param type  The boundary treatment type
 *   \param value The default value of out-of-Array pixels for the
 *     ValueBoundaryTreatment class. For all other boundary treatment
 *     strategies this parameter is ignored.
 *
 *   \return A pointer to a newly created BoundaryTreatment object
 *
 *   \exception RuntimeError If no boundary treatment object could be created
 *     this exception is thrown
 */
/*======================================================================*/
    static BoundaryTreatment<DataT,Dim> *get(
        BoundaryTreatmentType type, DataT const &value = traits<DataT>::zero);
    
  };

}

#include "BoundaryTreatment.icc"

#endif
