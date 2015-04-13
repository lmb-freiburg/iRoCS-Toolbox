/**************************************************************************
 *
 * Copyright (C) 2008 Alexandra Teynor
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

/**********************************************************
 *                                                          
 *  ImageTools
 *  image format related collection of little helper functions
 *                                                       
 *  author: Alex Teynor (teynor@informatik.uni-freiburg.de)  
 *  Dec. 2008
 *
 *********************************************************/	

#ifndef _IMAGETOOLS
#define _IMAGETOOLS

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

// Blitz arrays 
#include <blitz/array.h>

//! image format related collection of little helper functions
class ImageTools {

public:

template<typename DataType>
static void layeredToVectorial(
    const blitz::Array<DataType,3>& layered,
    blitz::Array<blitz::TinyVector<DataType,3>,2>& vectorial);


template<typename DataType>
static void vectorialToLayered(
    const blitz::Array<blitz::TinyVector<DataType,3>,2>& vectorial,
    blitz::Array<DataType,3>& layered);


};


#include "ImageTools.icc"

#endif

