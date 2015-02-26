/**********************************************************
 *                                                          
 *  Blitz2Ddraw  
 *  Basic drawing routines in blitz arrays 
 *
 *  author: Alex Teynor (teynor@informatik.uni-freiburg.de)  
 *  Dec. 2008
 *
 *********************************************************/	

#ifndef _BLITZ2DDRAW
#define _BLITZ2DDRAW

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

// Blitz arrays 
#include <blitz/array.h>


//! Basic drawing routines for blitz arrays 
class Blitz2Ddraw {

public:


/*======================================================================*/
/*! 
 *    Sets a single pixel in a image.
 *
 *   \param image Blitz array that represents the image
 *   \param y     y-coordinate in the image (first dim in image)
 *   \param x     x-coordinate in the image (second dim in image)
 *   \param color TinyVector that contais the color of the pixel to be set
 */
/*======================================================================*/

template <typename T>
static void setPixel(blitz::Array<T,3>& image,  
                   int y, int x, 
                   const blitz::TinyVector<T,3>& color);


/*======================================================================*/
/*! 
 *    Sets a single pixel in a gray image.
 *
 *   \overload 
 *
 *   \param image Blitz array that represents the image
 *   \param y     y-coordinate in the image (first dim in image)
 *   \param x     x-coordinate in the image (second dim in image)
 *   \param color contains the intensity of the pixel to be set
 */
/*======================================================================*/

template <typename T>
static void setPixel(blitz::Array<T,2>& image, 
                   int y, int x, 
                   const T& color);
 
/*======================================================================*/
/*! 
 *    Draw a simple, non-aliased line.
 *
 *   \param image     Blitz array that represents the image
 *   \param y0        y-coordinate of the start point
 *   \param x0        x-coordinate of the start point
 *   \param y1        y-coordinate of the end point
 *   \param x1        x-coordinate of the end point
 *   \param color     TinyVector that contains the color of the line
 *
 *   \note (Code adapted from 
 *         http://www.cs.unc.edu/~mcmillan/comp136/Lecture6/Lines.html)
 */
/*======================================================================*/

template<typename Type> 
static void drawLine(blitz::Array<Type,3> & image, 
                     int y0, int x0, int y1, int x1, 
                     const blitz::TinyVector<Type,3>& color);


/*======================================================================*/
/*! 
 *    Draw a simple, non-aliased line.
 *
 *   \overload 
 *
 *   \param image     Blitz array that represents the image
 *   \param y0        y-coordinate of the start point
 *   \param x0        x-coordinate of the start point
 *   \param y1        y-coordinate of the end point
 *   \param x1        x-coordinate of the end point
 *   \param color     contains the intensity of the line
 *
 *   \note (Code adapted from 
 *         http://www.cs.unc.edu/~mcmillan/comp136/Lecture6/Lines.html)
 */
/*======================================================================*/

template<typename Type> 
static void drawLine(blitz::Array<Type,2> & image, 
                     int y0, int x0, int y1, int x1, 
                     const Type& color);

/*======================================================================*/
/*! 
 *    Draw a simple, non-aliased circle.
 *
 *   \param image     Blitz array that represents the image
 *   \param yCenter   y-coordinate of the center point
 *   \param xCenter   x-coordinate of the center point
 *   \param radius    radius of the circle
 *   \param color     TinyVector that contains the color of the line
 */
/*======================================================================*/


template<typename Type>
static void drawCircle(blitz::Array<Type,3>& image, 
                       int yCenter, int xCenter, int radius, 
                       const blitz::TinyVector<Type,3>& color);

/*======================================================================*/
/*! 
 *    Draw a simple, non-aliased circle.
 *
 *   \overload
 *
 *   \param image     Blitz array that represents the image
 *   \param yCenter   y-coordinate of the center point
 *   \param xCenter   x-coordinate of the center point
 *   \param radius    radius of the circle
 *   \param color     TinyVector that contains the color of the line
 */
/*======================================================================*/

template<typename Type>
static void drawCircle(blitz::Array<Type,2>& image, 
                       int yCenter, int xCenter, int radius, 
                       const Type& color);



/*======================================================================*/
/*! 
 *   Draw a rectangle into an blitz array
 *   the upper right corner and the lower left corner must be given
 *
 *   \param image     Blitz array that represents the image
 *   \param y0        y-coordinate of the upper left point
 *   \param x0        x-coordinate of the upper left point
 *   \param y1        y-coordinate of the lower right point
 *   \param x1        x-coordinate of the lower right point
 *   \param color     contains the intensity of the line
 */
/*======================================================================*/

template<typename Type> 
static void drawRect(blitz::Array<Type,3> & image, 
                     int y0, int x0, int y1, int x1, 
                     const blitz::TinyVector<Type,3>& color);



/*======================================================================*/
/*! 
 *   Draw a rectangle into an blitz array
 *   the upper right corner and the lower left corner must be given
 *
 *   \overload
 *
 *   \param image     Blitz array that represents the image
 *   \param y0        y-coordinate of the upper left point
 *   \param x0        x-coordinate of the upper left point
 *   \param y1        y-coordinate of the lower right point
 *   \param x1        x-coordinate of the lower right point
 *   \param color     contains the intensity of the line
 */
/*======================================================================*/

template<typename Type> 
static void drawRect(blitz::Array<Type,2> & image, 
                     int y0, int x0, int y1, int x1, 
                     const Type& color);



private:

/*======================================================================*/
/*! 
 *   Private internal method for drawing circles
 */
/*======================================================================*/

template<typename Type>
static void circlePoints(blitz::Array<Type,2>& image, 
                         int cy, int cx, int y, int x, 
                         const Type& color);

template<typename Type>
static void circlePoints(blitz::Array<Type,3>& image, 
                         int cy, int cx, int y, int x, 
                         const blitz::TinyVector<Type,3>& color);






};

#include "Blitz2Ddraw.icc"


#endif

