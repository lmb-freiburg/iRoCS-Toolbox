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
 *  Blitz2Dgeometry  
 *  Basic geometry routines like scaling, rotation (and 
 *  possibly later other distortions as well)
 *
 *  author: Alex Teynor (teynor@informatik.uni-freiburg.de)  
 *  March. 2009
 *
 *********************************************************/	

#ifndef _BLITZ2DGEOMETRY
#define _BLITZ2DGEOMETRY

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

// Blitz arrays 
#include <blitz/array.h>

enum scaleType {
    BILINEAR=0,
    BICUBIC=1   
};

//! Basic geometry changing routines for blitz images 
class Blitz2Dgeometry {

public:


/*======================================================================*/
/*! 
 *    Scales an image
 *
 *   \param origImage    Blitz array that represents the input
 *   \param scaledImage  Blitz array that holds the output
 *   \param facy         scaling factor in y direction     
 *   \param facx         scaling factor in x direction     
 *   \param type         scaling type (default: BILINEAR)
 */
/*======================================================================*/

template <typename T>
static void scale(const blitz::Array<T,2>& origImage, 
                  blitz::Array<T,2>& scaledImage, 
                  float facy, float facx, 
                  scaleType type);


};

#include "Blitz2Dgeometry.icc"


#endif

