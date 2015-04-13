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
 *  ColorConvert.cc
 *  Implements the ColorConvert class
 *  => for a precise description of the classes see 
 *  respective header files
 *                                                       
 **********************************************************
 *
 *
 *********************************************************/	

#include "ColorConvert.hh"
#include "ImageTools.hh"
#include <algorithm>
#include <math.h>


void ColorConvert::rgb2gray(
    const ImageAccessWrapper& srcArray, 
    blitz::Array<unsigned char,2>& destArray)  {
   
    //get individual planes
    int h = srcArray.height();
    int w = srcArray.width();
    destArray.resize(h,w);

    unsigned char grayVal;
    
    for (int i=0; i<h ; ++i) {
        for (int j=0; j<w ; ++j) {
            grayVal =  
                static_cast<unsigned char>(0.2989 * srcArray.getPixelR(i,j) +
                                           0.5870 * srcArray.getPixelG(i,j) +
                                           0.1140 * srcArray.getPixelB(i,j) );
            destArray(i,j) = grayVal;            
        }        
    }    
}


void ColorConvert::rgb2gray(
    blitz::Array<unsigned char, 3>& srcArray, 
    blitz::Array<unsigned char, 2>& destArray ) {
    
    ImageAccessWrapper wrappedData(srcArray);
    rgb2gray(wrappedData,destArray);

}



void ColorConvert::rgb2gray(
    blitz::Array<blitz::TinyVector<unsigned char, 3>, 2>& srcArray, 
    blitz::Array<unsigned char,2>& destArray) {
    
    ImageAccessWrapper wrapSrc(srcArray);    
    rgb2gray(wrapSrc,destArray);    

}


void ColorConvert::rgb2gray(
    blitz::Array<blitz::TinyVector<unsigned char, 3>, 2>& srcArray, 
    blitz::Array<float,2>& destArray) {
    
    ImageAccessWrapper wrapSrc(srcArray);    
    rgb2gray(wrapSrc,destArray);    
}


void ColorConvert::rgb2gray(
    blitz::Array<unsigned char, 3>& srcArray, 
    blitz::Array<float,2>& destArray) {
    
    ImageAccessWrapper wrapSrc(srcArray);    
    rgb2gray(wrapSrc,destArray);    
}



void ColorConvert::rgb2gray(
    const ImageAccessWrapper& srcArray, 
    blitz::Array<float,2>& destArray)  {

    //get individual planes
    int h = srcArray.height();
    int w = srcArray.width();
    destArray.resize(h,w);

    unsigned char grayVal;
    
    for (int i=0; i<h ; ++i) {
        for (int j=0; j<w ; ++j) {
            
            rgb2gray(srcArray.getPixelR(i,j), 
                     srcArray.getPixelG(i,j),
                     srcArray.getPixelB(i,j),
                     grayVal);
            
            destArray(i,j) = static_cast<float>(grayVal)/255.0f;
            
        }
    }
}




void ColorConvert::rgb2gray(const unsigned char& r, 
                            const unsigned char& g, 
                            const unsigned char& b, 
                            unsigned char& gray) {
    gray = static_cast<unsigned char>(round(0.2989 * r + 
                                            0.5870 * g + 
                                            0.1140 * b));
}


void ColorConvert::rgb2gray(const BlitzRGBColor& colRGB, 
                            unsigned char& gray) {
    rgb2gray(colRGB(0),colRGB(1),colRGB(2),gray);
}


/** 
 * Optimized implementation following the instuction form Alvy-Ray-Smith 
 * (http://alvyray.com/Papers/hsv2rgb.htm)
 *
 */
void ColorConvert::rgb2hsv(const unsigned char& r, 
                           const unsigned char& g, 
                           const unsigned char& b, 
                           float& h, float& s, float& v)  {

    //convert rgb to float in range 0-1
    float fr = static_cast<float>(r)/255.0f;
    float fg = static_cast<float>(g)/255.0f;
    float fb = static_cast<float>(b)/255.0f;
        
    //find the min value
    float x = std::min(fr, fg);        
    x = std::min(x,fb);
   
    //find the max value
    v = std::max(fr, fg);
    v = std::max(v,fb);

    // gray pixel, saturation 0, set default color (pure red in our case)
    if (v == x) {
        h = 0; 
        s = 0; 
        return;
    }
    
    float f = (fr == x) ? fg - fb : ((fg == x) ? fb - fr : fr - fg);
    int i = (fr == x) ? 3 : ((fg == x) ? 5 : 1);

    h = ( i - f /(v - x)) / 6.0, 
    s = (v - x) / v;
    
}


void ColorConvert::rgb2hsv(const BlitzRGBColor& colRGB, 
                           blitz::TinyVector<float,3>& colHSV) {
    rgb2hsv(colRGB(0),colRGB(1),colRGB(2),colHSV(0),colHSV(1),colHSV(2));
    
}


/** 
 * Optimized implementation taken form Alvy-Ray-Smith 
 * (http://alvyray.com/Papers/hsv2rgb.htm)
 *
 */
void ColorConvert::hsv2rgb(const float& h, 
                           const float& s, 
                           const float& v, 
                           unsigned char& r, 
                           unsigned char& g, 
                           unsigned char& b)  {

    int i;
    float m,n,f;
    
    //convert h to the range of [0-6] 
    float h6 = h*6.0f;
    
    i = static_cast<int>(floor(h6));
    f = h6-i;
    
    // if i is even
    if (!(i&1)) f = 1-f;
    
    m = v * (1 - s);
    n = v * (1 - s * f);

    switch (i) {
    case 6:
    case 0: 
        r = static_cast<unsigned char>(v * 255.0f);
        g = static_cast<unsigned char>(n * 255.0f);
        b = static_cast<unsigned char>(m * 255.0f);
        break;        
    case 1: 
        r = static_cast<unsigned char>(n * 255.0f);
        g = static_cast<unsigned char>(v * 255.0f);
        b = static_cast<unsigned char>(m * 255.0f);
        break;        
    case 2: 
        r = static_cast<unsigned char>(m * 255.0f);
        g = static_cast<unsigned char>(v * 255.0f);
        b = static_cast<unsigned char>(n * 255.0f);
        break;        
    case 3: 
        r = static_cast<unsigned char>(m * 255.0f);
        g = static_cast<unsigned char>(n * 255.0f);
        b = static_cast<unsigned char>(v * 255.0f);
        break;        
    case 4: 
        r = static_cast<unsigned char>(n * 255.0f);
        g = static_cast<unsigned char>(m * 255.0f);
        b = static_cast<unsigned char>(v * 255.0f);
        break;        
    case 5: 
        r = static_cast<unsigned char>(v * 255.0f);
        g = static_cast<unsigned char>(m * 255.0f);
        b = static_cast<unsigned char>(n * 255.0f);
        break;        
    }
}


void ColorConvert::hsv2rgb(const blitz::TinyVector<float,3>& colHSV,
             BlitzRGBColor& colRGB) {
    
    hsv2rgb(colHSV(0),colHSV(1),colHSV(2),
            colRGB(0),colRGB(1),colRGB(2));
}



void ColorConvert::rgb2hsv(blitz::Array<unsigned char,3>& srcArray, 
                           blitz::Array<float,3>& destArray)  {

    ImageAccessWrapper wrappedSrc(srcArray);
    rgb2hsv(wrappedSrc,destArray);
    
}


void ColorConvert::rgb2hsv(
    blitz::Array<blitz::TinyVector<unsigned char,3>,2>& srcArray, 
    blitz::Array<blitz::TinyVector<float,3>,2>& destArray) {
    
    ImageAccessWrapper wrappedSrc(srcArray);
    blitz::Array<float,3> layeredFloatImage;
    
    //used layered prototpe function
    rgb2hsv(wrappedSrc,layeredFloatImage);

    //copy the layered content to vectorial storage
    ImageTools::layeredToVectorial(layeredFloatImage, destArray);

}



void ColorConvert::rgb2hsv(const ImageAccessWrapper& srcArray, 
                           blitz::Array<float,3>& destArray)  {

    int xdim = srcArray.width();
    int ydim = srcArray.height();

    // make the dest array the right size
    destArray.resize(3,ydim,xdim);
    
    float h = 0.0f;
    float s = 0.0f;
    float v = 0.0f;
    
    //convert every pixel:
    for (int y=0; y<ydim; ++y){
        for (int x=0; x<xdim; ++x){            

            rgb2hsv(srcArray.getPixelR(y,x),
                    srcArray.getPixelG(y,x),
                    srcArray.getPixelB(y,x),
                    h,s,v);
            
            destArray(0,y,x) = h;
            destArray(1,y,x) = s;
            destArray(2,y,x) = v;
        }
    }    
}



void ColorConvert::hsv2rgb(blitz::Array<float,3>& srcArray, 
                           blitz::Array<unsigned char,3>& destArray) {
 
    ImageAccessWrapper wrappedDest(destArray);
    hsv2rgb(srcArray,wrappedDest);
}



void ColorConvert::hsv2rgb(
    blitz::Array<blitz::TinyVector<float,3>,2>& srcArray,
    blitz::Array<blitz::TinyVector<unsigned char,3>,2>& destArray) {

    blitz::Array<float,3> layeredFloatImage;

    //copy the layered content to vectorial storage
    ImageTools::vectorialToLayered(srcArray,layeredFloatImage);

    ImageAccessWrapper wrappedDest(destArray);
    hsv2rgb(layeredFloatImage,wrappedDest);

    
}
                           

void ColorConvert::hsv2rgb(const blitz::Array<float,3>& srcArray, 
                           ImageAccessWrapper& destArray)  {

    if (srcArray.extent(0) != 3) {
        std::cerr << "ColorConvert::hsv2rgb: "
                  << "Source array is not HSV: dimension mismatch!" 
                  << std::endl;
        return;        
    }

    int xdim = srcArray.extent(2);
    int ydim = srcArray.extent(1);

    // make the dest array the right size
    destArray.resize(ydim,xdim);
    
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
    
    //convert every pixel:
    for (int y=0; y<ydim; ++y){
        for (int x=0; x<xdim; ++x){            

            hsv2rgb(srcArray(0,y,x),srcArray(1,y,x),srcArray(2,y,x),r,g,b);
            destArray.setPixel(y,x,r,g,b);

        }
    }    
}



