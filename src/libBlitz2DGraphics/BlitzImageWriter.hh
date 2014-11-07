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

