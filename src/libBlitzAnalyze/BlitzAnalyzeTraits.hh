/**************************************************************************
 *
 * Copyright (C) 2010 Robert Bensch
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

/**************************************************************************
 **	Title: BlitzAnalyzeTraits class
 **	$RCSfile$
 **	$Revision: $$Name$
 **	$Date: $
 **	Copyright: GPL $Author: $
 **	Description:
 **
 **************************************************************************/

#ifndef BLITZANALYZETRAITS_HH
#define BLITZANALYZETRAITS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <string>
#include <fstream>
#include <blitz/array.h>

//#include "BlitzAnalyzeType.hh"	// lib's type traits
#include "BlitzAnalyzeError.hh"  // lib's own error class

/*======================================================================*/
/*!
 *  \class BlitzAnalyzeTraits BlitzAnalyzeTraits.hh
 *  \brief The BlitzAnalyzeTraits class provides type traits
 *  	   for BlitzAnalyzeFile.
 *
 */
/*======================================================================*/

template<typename T>
class BlitzAnalyzeTraits {

public:

  /*======================================================================*/
  /*!
   *   Public singleton access methode (Instance)
   *   to one single instance of the class.
   */
  /*======================================================================*/
  static BlitzAnalyzeTraits* Instance();

  /*-----------------------------------------------------------------------
   *  Destructor
   *-----------------------------------------------------------------------*/
  ~BlitzAnalyzeTraits();

  /*-----------------------------------------------------------------------
   *  data methods
   *-----------------------------------------------------------------------*/

  /*======================================================================*/
  /*!
   *   Selects datatype.
   */
  /*======================================================================*/
  std::string Type();
  /*======================================================================*/
  /*!
   *   Returns maximum value in inArray or seperately specified value.
   */
  /*======================================================================*/
  int max( const blitz::Array< T,3>& inArray);
  /*======================================================================*/
  /*!
   *   Returns minimum value in inArray or seperately specified value.
   */
  /*======================================================================*/
  int min( const blitz::Array< T,3>& inArray);
  /*======================================================================*/
  /*!
   *   Returns size of datatype T in Byte.
   */
  /*======================================================================*/
  size_t sizeof_type();

protected:
  /*-----------------------------------------------------------------------
   *  Constructors
   *-----------------------------------------------------------------------*/

  /*======================================================================*/
  /*!
   *   Creates a stub BlitzAnalyzeTraits object.
   */
  /*======================================================================*/
  BlitzAnalyzeTraits()
  throw(BlitzAnalyzeError);

private:

  /*-------------------------------------------------------------------------
   *  private vars
   *-------------------------------------------------------------------------*/
  static BlitzAnalyzeTraits* _instance;

};

#include "BlitzAnalyzeTraits.icc"

#endif

