/**************************************************************************
**       Title: General type traits
**    $RCSfile$
**   $Revision: 3896 $ $Name$
**       $Date: 2008-07-16 15:17:47 +0200 (Wed, 16 Jul 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    Templates to obtain specific information about used types
**
**************************************************************************/

/*======================================================================*/
/*!
 *  \file TypeTraits.hh
 *  \brief Query specific information about different data types.
 */
/*======================================================================*/

#ifndef ATBTYPETRAITS_HH
#define ATBTYPETRAITS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <complex>
#include <blitz/array.h>

namespace atb
{
  
  typedef int BlitzIndexT;

/*======================================================================*/
/*!
 *  \class traits <>
 *  \brief The traits class provides means to query specific information
 *     about different data types.
 *
 *  The traits are helpers to query type traits in a template-context to
 *  avoid exhaustive code duplication. This is the slim variant to
 *  copy&pasting whole functions or even classes to provide specific
 *  behaviour for several data types. When running into cod duplication
 *  trouble, think of extending this class first.
 */
/*======================================================================*/
  template<typename DataT>
  struct traits
  {
    
/*======================================================================*/
/*! 
 *   The type with highest precision of this kind.
 *
 *   If you want to do floating point arithmetics on variables of
 *   this data type you can get the most appropriate floating point
 *   type using this typedef. For scalars it returns double, for compound
 *   types the corresponding equivalent.
 */
/*======================================================================*/
    typedef DataT HighPrecisionT;

/*======================================================================*/
/*! 
 *   The corresponding complex value type to the given type.
 *
 *   The default implementation is DataT which is pointless
 *   for probably every type that has not been explicitely instantiated.
 */
/*======================================================================*/
    typedef DataT ComplexT;

/*======================================================================*/
/*! 
 *   The corresponding basic type to the given type.
 *
 *   For multi-component types this should return the basic element type,
 *   e.g. traits< std::complex<double> >::BasicT = double.
 */
/*======================================================================*/
    typedef DataT BasicT;

/*======================================================================*/
/*! 
 *   Get a representation of the smallest value for this data type.
 */
/*======================================================================*/
    static DataT const smallest;
    
/*======================================================================*/
/*! 
 *   Get a representation of the greatest value for this data type.
 */
/*======================================================================*/
    static DataT const greatest;
    
/*======================================================================*/
/*! 
 *   Get a representation of the Zero for this data type.
 */
/*======================================================================*/
    static DataT const zero;
    
/*======================================================================*/
/*! 
 *   Get a representation of the One for this data type.
 */
/*======================================================================*/
    static DataT const one;

/*======================================================================*/
/*! 
 *   Get the value that corresponds to the saturation of the sensor.
 *   This is 255 for an 8Bit sensor, 65535 for a 16Bit sensor, and 1.0 for
 *   a floating point type. or unknown types (if the compiler doesn't
 *   complain) a representation of 1 of the data type is returned.
 */
/*======================================================================*/
    static DataT const saturated;

/*======================================================================*/
/*! 
 *   If this type specifies a complex number type this constant is true
 */
/*======================================================================*/
    static bool const isComplex;

  };

}

#include "TypeTraits.icc"

#endif
