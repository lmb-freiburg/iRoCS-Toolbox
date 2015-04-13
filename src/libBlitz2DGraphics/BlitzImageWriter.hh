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
 *  BlitzImageWriter 
 *  Class that writes blitz arrays into a desired image 
 *  format
 *
 *  author: Alex Teynor (teynor@informatik.uni-freiburg.de)  
 *  Dec. 2008
 * 
 *  extended by: Thorsten Schmidt (tschmidt@cs.uni-freiburg.de)
 *  Mar. 2013
 *
 *********************************************************/	

#ifndef _BLITZIMAGEWRITER
#define _BLITZIMAGEWRITER

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#ifdef HAVE_JPEG
namespace JPEGLIB {
// jpeg library  
extern "C" {
#include <jpeglib.h>
}
};
#endif

// Blitz arrays 
#include <blitz/array.h>

//PNG lib
#include <png.h>

#include "ImageAccessWrapper.hh"



 //! Write blitz arrays into a desired image format
class BlitzImageWriter {

public:

  /*======================================================================*/
  /*! 
   *   Write a .pgm file (portable gray map) from a blitz array
   *   Image is written as ASCII data or raw binary depending on the
   *   format parameter
   *
   *   \param data     the blitz array with gray values
   *   \param fileName the image file Name
   *   \param format   The format may be 2 (plain ASCII) or 5 (raw binary)
   */
  /*======================================================================*/
  static void writePGM(const blitz::Array<unsigned char,2> &data, 
                       const std::string& fileName, int format = 2);

  /*======================================================================*/
  /*! 
   *   Write a .ppm file (portable pix map) from a blitz array
   *   Image is written as ASCII data or raw binary depending on the format
   *   parameter
   *
   *   \param data     wrapped blitz array (might be different formats, 
   *                   default constructor is called) 
   *   \param fileName the image file Name
   *   \param format   The format may be 3 (plain ASCII) or 6 (raw binary)
   */
  /*======================================================================*/
  static void writePPM(const ImageAccessWrapper &data,
                       const std::string& fileName, int format = 3);

#ifdef HAVE_JPEG

  /*======================================================================*/
  /*! 
   *   Write a .jpg file from a blitz array
   *
   *   \param data     the blitz array with gray values
   *   \param fileName the image file Name
   *   \param quality  the image quality after compression 
   *                   in percent (100:best, 1:worst)
   */
  /*======================================================================*/
  static int writeJPEG(const ImageAccessWrapper &data, 
                       const std::string& fileName,
                       int quality = 100);

#endif

  /*======================================================================*/
  /*! 
   *   Write a .png file from a blitz array
   *
   *   \param data     the blitz array with gray values
   *   \param fileName the image file Name
   *   \param compfac  compression factor [1-9], 
   *                   1: best compression but slow, 
   *                   9: worst compression but fast
   *
   *   \note  Code adapted from 
   *          http://www.libpng.org/pub/png/libpng-1.2.5-manual.html
   */
  /*======================================================================*/
  static int writePNG(const ImageAccessWrapper &data, 
                      const std::string& fileName,
                      int compfac = 9);

};


#endif


