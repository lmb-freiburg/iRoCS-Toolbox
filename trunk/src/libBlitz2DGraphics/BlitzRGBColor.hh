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
 *  BlitzRGBClolor
 *  convenience color related functions
 *
 *                                                       
 **********************************************************
 *                              
 * $Author: teynor $                                       
 *
 *********************************************************/	

#ifndef _BLITZ_RGB_COLOR_HH
#define _BLITZ_RGB_COLOR_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

// Blitz arrays 
#include <blitz/array.h>

typedef blitz::TinyVector<unsigned char,3> BlitzRGBColor;


/*======================================================================*/
/*! 
 *   Descr.
 *
 *   \param paramName description
 *
 *   \return
 *
 *   \note
 */
/*======================================================================*/

namespace BlitzRGBColors {

const blitz::TinyVector<unsigned char,3> red(255,0,0);
const blitz::TinyVector<unsigned char,3> green(0,255,0);
const blitz::TinyVector<unsigned char,3> blue(0,0,255);
const blitz::TinyVector<unsigned char,3> yellow(255,255,0);
const blitz::TinyVector<unsigned char,3> magenta(255,0,255);
const blitz::TinyVector<unsigned char,3> cyan(0,255,255);
const blitz::TinyVector<unsigned char,3> black(0,0,0);
const blitz::TinyVector<unsigned char,3> white(255,255,255);
const blitz::TinyVector<unsigned char,3> gray(127,127,127);
const blitz::TinyVector<unsigned char,3> darkRed(127,0,0);
const blitz::TinyVector<unsigned char,3> copper(255,158,102);
const blitz::TinyVector<unsigned char,3> aquamarine(125,155,212);

blitz::TinyVector<unsigned char,3> jet(float val);

}

#endif
