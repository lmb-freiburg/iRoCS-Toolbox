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
 *  ColorConvert 
 *  File converts different color types to each other,
 *  Color files represented as Blitz arrays
 *
 *  author: Alex Teynor (teynor@informatik.uni-freiburg.de)
 *                                                       
 **********************************************************
 *
 *
 *********************************************************/	

#ifndef _COLORCONVERT
#define _COLORCONVERT

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

// Blitz arrays 
#include <blitz/array.h>
#include "ImageAccessWrapper.hh"

//! Converts different color spaces into each other.
class ColorConvert {

  public:
    
    // rgb image is unsigned char in the range of 0-255,
    // hsv image is float in the range of 0-1

   /*==================================================================*/
   /*! 
    *   Convert RGB to Gray
    *
    *   \param r the red color component, range between 0-255
    *   \param g the green color component, range between 0-255
    *   \param b the blue color component, range between 0-255
    *
    *   \param gray the resulting gray color, range between 0-255
    */
   /*==================================================================*/
    static void rgb2gray(const unsigned char& r, 
                         const unsigned char& g, 
                         const unsigned char& b, 
                         unsigned char& gray);


   /*==================================================================*/
   /*! 
    *   Convert RGB to Gray
    *
    *   \param colRGB  the rgb color in a tinyVector
    *
    *   \param gray    the gray color in a tinyVector, 
    *                  the 3 values are the same 
    */
   /*==================================================================*/
    static void rgb2gray(const blitz::TinyVector<unsigned char,3>& colRGB, 
                 unsigned char& gray);


   /*==================================================================*/
   /*! 
    *   Convert RGB to HSV
    *
    *   \param r the red color component, range between 0-255
    *   \param g the green color component, range between 0-255
    *   \param b the blue color component, range between 0-255
    *
    *   \param h the hue component, range between 0-1
    *   \param s the saturation component, range between 0-1
    *   \param v the value component, range between 0-1
    */
   /*==================================================================*/
    static void rgb2hsv(const unsigned char& r, 
                        const unsigned char& g, 
                        const unsigned char& b, 
                        float& h, 
                        float& s, 
                        float& v);


   /*==================================================================*/
   /*! 
    *   Convert RGB to HSV
    *
    *   \param colRGB  the rgb color in a TinyVector
    *
    *   \param colHSV  the hsv color in a TinyVector
    */
   /*==================================================================*/
    static void rgb2hsv(const blitz::TinyVector<unsigned char,3>& colRGB, 
                 blitz::TinyVector<float,3>& colHSV);


   /*==================================================================*/
   /*! 
    *   Convert HSV to RGB 
    *
    *   \param h the hue component, range between 0-1
    *   \param s the saturation component, range between 0-1
    *   \param v the value component, range between 0-1
    *
    *   \param r the red color component, range between 0-255
    *   \param g the green color component, range between 0-255
    *   \param b the blue color component, range between 0-255
    */
   /*==================================================================*/
    static void hsv2rgb(const float& h, 
                        const float& s, 
                        const float& v, 
                        unsigned char& r, 
                        unsigned char& g, 
                        unsigned char& b);

   /*==================================================================*/
   /*! 
    *   Convert HSV to RGB
    *
    *   \param colHSV  the hsv color in a TinyVector
    *
    *   \param colRGB  the rgb color in a TinyVector
    */
   /*==================================================================*/
    static void hsv2rgb(const blitz::TinyVector<float,3>& colHSV,
                 BlitzRGBColor& colRGB);



   /*==================================================================*/
   /*! 
    *   Convert a RGB (layered) image to a gray image 
    *
    *   \param srcArray source RGB image
    *   \param destArray target gray image
    */
   /*==================================================================*/
    static void rgb2gray(
        blitz::Array<unsigned char,3>& srcArray, 
        blitz::Array<unsigned char, 2>& destArray);



   /*==================================================================*/
   /*! 
    *   Convert a RGB (vectorial) image to a gray image
    *
    *   \param srcArray source RGB image
    *   \param destArray target gray image
    */
   /*==================================================================*/
    static void rgb2gray(
        blitz::Array<blitz::TinyVector<unsigned char,3>,2>& srcArray, 
        blitz::Array<unsigned char,2>& destArray);


   /*==================================================================*/
   /*! 
    *   Convert a RGB image to a gray image 
    *   (wrapper version that does all the work)
    *
    *   \param srcArray source RGB image
    *   \param destArray target gray image
    */
   /*==================================================================*/
    static void rgb2gray(
        const ImageAccessWrapper& srcArray, 
         blitz::Array<unsigned char, 2>& destArray );



   /*==================================================================*/
   /*! 
    *   Convert a RGB image to a float gray image 
    *   (wrapper version that does all the work)
    *
    *   \param srcArray source RGB image
    *   \param destArray target gray image, values in range 0-1 (float)
    */
   /*==================================================================*/
    static void rgb2gray(
        const ImageAccessWrapper& srcArray, 
         blitz::Array<float, 2>& destArray );



   /*==================================================================*/
   /*! 
    *   Convert a RGB (vectorial) image to a float gray image
    *
    *   \param srcArray source RGB image
    *   \param destArray target gray image, values in range 0-1 (float)
    */
   /*==================================================================*/
    static void rgb2gray(
        blitz::Array<blitz::TinyVector<unsigned char,3>,2>& srcArray, 
        blitz::Array<float,2>& destArray);


   /*==================================================================*/
   /*! 
    *   Convert a RGB (vectorial) image to a float gray image
    *
    *   \param srcArray source RGB image
    *   \param destArray target gray image, values in range 0-1 (float)
    */
   /*==================================================================*/
    static void rgb2gray(
        blitz::Array<unsigned char,3>& srcArray, 
        blitz::Array<float,2>& destArray);




   /*==================================================================*/
   /*! 
    *   Convert a RGB (layered) image to a HSV image 
    *
    *   \param srcArray source RGB image
    *   \param destArray target gray image
    */
   /*==================================================================*/
    static void rgb2hsv(
        blitz::Array<unsigned char,3>& srcArray, 
        blitz::Array<float,3>& destArray);


   /*==================================================================*/
   /*! 
    *   Convert a RGB (vectorial) image to a HSV image 
    *
    *   \param srcArray source RGB image
    *   \param destArray target gray image
    */
   /*==================================================================*/
    static void rgb2hsv(
        blitz::Array<blitz::TinyVector<unsigned char,3>,2>& srcArray, 
        blitz::Array<blitz::TinyVector<float,3>,2>& destArray);


   /*==================================================================*/
   /*! 
    *   Convert a RGB image to a HSV image 
    *   (wrapper version that does all the work)    *
    *
    *   \param srcArray source RGB image
    *   \param destArray target gray image
    */
   /*==================================================================*/
    static void rgb2hsv(
        const ImageAccessWrapper& srcArray, 
        blitz::Array<float,3>& destArray);



   /*==================================================================*/
   /*! 
    *   Convert a HSV (layered) image to a RGB image 
    *
    *   \param srcArray source RGB image
    *   \param destArray target gray image
    */
   /*==================================================================*/
    static void hsv2rgb(
        blitz::Array<float,3>& srcArray, 
        blitz::Array<unsigned char,3>& destArray);


   /*==================================================================*/
   /*! 
    *   Convert a HSV (vectorial) image to a RGB image 
    *
    *   \param srcArray source RGB image
    *   \param destArray target gray image
    */
   /*==================================================================*/
    static void hsv2rgb(
        blitz::Array<blitz::TinyVector<float,3>,2>& srcArray,
        blitz::Array<blitz::TinyVector<unsigned char,3>,2>& destArray);


   /*==================================================================*/
   /*! 
    *   Convert a HSV image to a RGB image 
    *   (wrapper version that does all the work)    *
    *
    *   \param srcArray source RGB image
    *   \param destArray target gray image
    */
   /*==================================================================*/
    static void hsv2rgb(
        const  blitz::Array<float,3>& srcArray, 
        ImageAccessWrapper& destArray);

};


#endif
