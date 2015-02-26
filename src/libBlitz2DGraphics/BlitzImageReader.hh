/**********************************************************
 *                                                          
 *  BlitzImageReader 
 *  Class that loads data from various image formats to 
 *  blitz arrays 
 *
 *  author: Alex Teynor (teynor@informatik.uni-freiburg.de)
 *  Dec. 2008
 *
 *  extended by: Thorsten Schmidt (tschmidt@cs.uni-freiburg.de)
 *  Mar. 2013
 *                                                       
 *********************************************************/	

#ifndef _BLITZIMAGEREADER_HH
#define _BLITZIMAGEREADER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <cstddef>
#include <cstdio>

#ifdef HAVE_JPEG
namespace JPEGLIB {
// jpeg library  
extern "C" {
#include <jpeglib.h>
 }
};
#endif

#ifdef HAVE_TIFF
// TIFF lib
#include <tiffio.h>
#endif

// PNG lib
#include <png.h>

// Blitz arrays 
#include <blitz/array.h>

#include "ImageAccessWrapper.hh"

//! Class that loads data from various image formats to blitz arrays 
class BlitzImageReader {

public:

#ifdef HAVE_JPEG

  /*======================================================================*/
  /*! 
   *   Reads a jpg image into a blitz dataset, in this case
   *   a 3D unsigned char image
   *   storage order is: colorlayer(r,g,b), y, x
   *   Gray images just have extent 1 in the first dimension
   *
   *
   *   \param data        data Array where the data should be read in
   *   \param fileName    name of the jpg file
   *
   *   \return int        returns 0 if ok, -1 if something went wrong
   *
   *   \note the data is just wrapped and the general function version
   *         is called
   */
  /*======================================================================*/
  static int readJPEG(blitz::Array<unsigned char,3> &data, 
                      const std::string& fileName);


  /*======================================================================*/
  /*! 
   *   Reads a jpg image into a blitz dataset, in this case
   *   a 2D array with vectorial data (rgb)
   *  
   *   \overload
   *
   *   \param data        data Array where the data should be read in
   *   \param fileName    name of the jpg file
   *
   *   \return int        returns 0 if ok, -1 if something went wrong
   *
   *   \note the data is just wrapped and the general function version
   *         is called
   */
  /*======================================================================*/
  static int readJPEG(blitz::Array<blitz::TinyVector<unsigned char,3>,2> &data,
                      const std::string& fileName);


  /*======================================================================*/
  /*! 
   *   Reads a jpg image into a blitz dataset, in this case
   *   the wrapped version: in this function, the real processing is done. 
   *
   *   \overload
   *
   *   \param data        data Array where the data should be read in
   *   \param fileName    name of the jpg file
   *
   *   \return int        returns 0 if ok, -1 if something went wrong
   */
  /*======================================================================*/
  static int readJPEG(ImageAccessWrapper &data, 
                      const std::string& fileName);

#endif

  /*======================================================================*/
  /*! 
   *   Reads a png image into a blitz dataset, in this case
   *   a 3D unsigned char image
   *   storage order is: colorlayer(r,g,b), y, x
   *   Gray images just have extent 1 in the first dimension
   *
   *
   *   \param data        data Array where the data should be read in
   *   \param fileName    name of the png file
   *
   *   \return int        returns 0 if ok, -1 if something went wrong
   *
   *   \note the data is just wrapped and the general function version
   *         is called
   */
  /*======================================================================*/
  static int readPNG(blitz::Array<unsigned char,3> &data, 
                     const std::string& fileName);


  /*======================================================================*/
  /*! 
   *   Reads a png image into a blitz dataset, in this case
   *   a 2D array with vectorial data (rgb)
   *  
   *   \overload
   *
   *   \param data        data Array where the data should be read in
   *   \param fileName    name of the png file
   *
   *   \return int        returns 0 if ok, -1 if something went wrong
   *
   *   \note the data is just wrapped and the general function version
   *         is called
   */
  /*======================================================================*/
  static int readPNG(blitz::Array<blitz::TinyVector<unsigned char,3>, 2> &data,
                     const std::string& fileName);


  /*======================================================================*/
  /*! 
   *   Reads a png image into a blitz dataset, in this case
   *   the wrapped version: in this function, the real processing is done. 
   *
   *   \overload
   *
   *   \param data        data Array where the data should be read in
   *   \param fileName    name of the png file
   *
   *   \return int        returns 0 if ok, -1 if something went wrong
   */
  /*======================================================================*/
  static int readPNG(ImageAccessWrapper &data, 
                     const std::string& fileName);

  /*======================================================================*/
  /*! 
   *   Reads a pnm image into a blitz dataset, in this case
   *   a 3D unsigned char image. pnm is a meta format applying to pbm, pgm
   *   and ppm. The actual file type is determined from the magic number
   *   and the file accordingly processed.
   *   storage order is: colorlayer(r,g,b), y, x
   *   Gray images just have extent 1 in the first dimension
   *
   *   \overload
   *
   *   \param data        data Array where the data should be read in
   *   \param fileName    name of the pnm file
   *
   *   \return int        returns 0 if ok, -1 if something went wrong
   *
   *   \note the data is just wrapped and the general function version
   *         is called
   */
  /*======================================================================*/
  static int readPNM(blitz::Array<unsigned char,3> &data, 
                     const std::string& fileName);


  /*======================================================================*/
  /*! 
   *   Reads a pnm image into a blitz dataset, in this case
   *   a 2D array with vectorial data (rgb). pnm is a meta format applying
   *   to pbm, pgm and ppm. The actual file type is determined from the
   *   magic number and the file accordingly processed.
   *  
   *   \overload
   *
   *   \param data        data Array where the data should be read in
   *   \param fileName    name of the pnm file
   *
   *   \return int        returns 0 if ok, -1 if something went wrong
   *
   *   \note the data is just wrapped and the general function version
   *         is called
   */
  /*======================================================================*/
  static int readPNM(blitz::Array<blitz::TinyVector<unsigned char,3>, 2> &data,
                     const std::string& fileName);


  /*======================================================================*/
  /*! 
   *   Reads a pnm image into a blitz dataset, in this case
   *   the wrapped version: in this function, the real processing is done. 
   *   pnm is a meta format applying to pbm, pgm and ppm. The actual file
   *   type is determined from the magic number and the file accordingly
   *   processed.
   *
   *   \param data        data Array where the data should be read in
   *   \param fileName    name of the pnm file
   *
   *   \return int        returns 0 if ok, -1 if something went wrong
   */
  /*======================================================================*/
  static int readPNM(ImageAccessWrapper &data, 
                     const std::string& fileName);

#ifdef HAVE_TIFF

  /*======================================================================*/
  /*! 
   *   Reads a tiff image into a blitz dataset, in this case
   *   a 3D unsigned char image
   *   storage order is: colorlayer(r,g,b), y, x
   *   Gray images just have extent 1 in the first dimension
   *
   *
   *   \param data        data Array where the data should be read in
   *   \param fileName    name of the png file
   *
   *   \return int        returns 0 if ok, -1 if something went wrong
   *
   *   \note the data is just wrapped and the general function version
   *         is called
   */
  /*======================================================================*/
  static int readTIFF(blitz::Array<unsigned char,3> &data, 
                      const std::string& fileName);


  /*======================================================================*/
  /*! 
   *   Reads a tiff image into a blitz dataset, in this case
   *   a 2D array with vectorial data (rgb)
   *  
   *   \overload
   *
   *   \param data        data Array where the data should be read in
   *   \param fileName    name of the png file
   *
   *   \return int        returns 0 if ok, -1 if something went wrong
   *
   *   \note the data is just wrapped and the general function version
   *         is called
   */
  /*======================================================================*/
  static int readTIFF(blitz::Array<blitz::TinyVector<unsigned char,3>, 2> &data,
                      const std::string& fileName);


  /*======================================================================*/
  /*! 
   *   Reads a tiff image into a blitz dataset, in this case
   *   the wrapped version: in this function, the real processing is done. 
   *
   *   \overload
   *
   *   \param data        data Array where the data should be read in
   *   \param fileName    name of the png file
   *
   *   \return int        returns 0 if ok, -1 if something went wrong
   */
  /*======================================================================*/
  static int readTIFF(ImageAccessWrapper &data, 
                      const std::string& fileName);

#endif

private:
  
  static std::istream &pnmSkipToNextToken(std::istream &pnmStream);

};

#endif

