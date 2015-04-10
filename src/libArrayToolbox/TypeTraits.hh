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
  
/*! The native integer type for indexing blitz++ Arrays. */
  typedef int BlitzIndexT; 

/*======================================================================*/
/*!
 *  \class traits TypeTraits.hh "libArrayToolbox/TypeTraits.hh"
 *
 *  \brief The traits class provides means to query specific information
 *     about different data types.
 *
 *  The traits are helpers to query type traits in a template-context to
 *  avoid exhaustive code duplication. This is the slim variant to
 *  copy&pasting whole functions or even classes to provide specific
 *  behaviour for several data types. When running into code duplication
 *  trouble, think of extending this class first.
 */
/*======================================================================*/
  template<typename DataT>
  struct traits
  {
    
/*======================================================================*/
/*! 
 *   \brief The type with highest precision of this kind.
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
 *   \brief The corresponding complex value type to the given type.
 *
 *   The default implementation is DataT which is pointless
 *   for probably every type that has not been explicitely instantiated.
 */
/*======================================================================*/
    typedef DataT ComplexT;

/*======================================================================*/
/*! 
 *   \brief The corresponding basic type to the given type.
 *
 *   For multi-component types this should return the basic element type,
 *   e.g. traits< std::complex<double> >::BasicT = double.
 */
/*======================================================================*/
    typedef DataT BasicT;

/*======================================================================*/
/*! 
 *   \brief Get a representation of the smallest value for this data type.
 */
/*======================================================================*/
    static DataT const smallest;
    
/*======================================================================*/
/*! 
 *   \brief Get a representation of the greatest value for this data type.
 */
/*======================================================================*/
    static DataT const greatest;
    
/*======================================================================*/
/*! 
 *   \brief Get a representation of the Zero for this data type.
 */
/*======================================================================*/
    static DataT const zero;
    
/*======================================================================*/
/*! 
 *   \brief Get a representation of the One for this data type.
 */
/*======================================================================*/
    static DataT const one;

/*======================================================================*/
/*! 
 *   \brief Get the value that corresponds to the saturation of the sensor.
 *   This is 255 for an 8Bit sensor, 65535 for a 16Bit sensor, and 1.0 for
 *   a floating point type. For unknown types (if the compiler doesn't
 *   complain) a representation of 1 of the data type is returned.
 */
/*======================================================================*/
    static DataT const saturated;

/*======================================================================*/
/*! 
 *   \brief If this type specifies a complex number type this constant is true
 */
/*======================================================================*/
    static bool const isComplex;

  };

}

#include "TypeTraits.icc"

#endif
