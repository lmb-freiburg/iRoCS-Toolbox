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
