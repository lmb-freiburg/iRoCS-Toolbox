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

