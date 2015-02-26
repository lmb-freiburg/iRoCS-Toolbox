/**********************************************************
 *                                                          
 *  ImageAccessWrapper
 *  Ensures acces to the image for different image types
 *                                                       
 *  author: Alex Teynor (teynor@informatik.uni-freiburg.de)  
 *  Dec. 2008
 *  
 *********************************************************/	

#ifndef _BLITZIMAGEACCESSWRAPPER_HH
#define _BLITZIMAGEACCESSWRAPPER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

// Blitz arrays 
#include <blitz/array.h>
#include "BlitzRGBColor.hh"

//! To allow for different blitz storage formats to be handeled equally
class ImageAccessWrapper {

public:

    enum ImageType {
      layeredRGB,
      layeredGray,
      vectRGB
    };

    ImageAccessWrapper (blitz::Array<unsigned char, 3>& data);
    ImageAccessWrapper (blitz::Array<unsigned char, 2>& data);
    ImageAccessWrapper (blitz::Array<blitz::TinyVector<unsigned char,3>, 2>& 
                        data);

    //PIXEL setting functions --------------------------------
    // Coordinates are given in y,x (analogus to array indexing)
    // Coordinates might also be given as a TinyVector: (y,x)
    // Pixel (0/0) in the upper left corner
    void setPixel(int y, int x, 
                  unsigned char R, 
                  unsigned char G, 
                  unsigned char B);

    void setPixel(int y, int x, 
                  const BlitzRGBColor& col);


    void setPixel(blitz::TinyVector<int,2>& pos, 
                  unsigned char R, 
                  unsigned char G, 
                  unsigned char B);

    void setPixel(blitz::TinyVector<int,2>& pos, 
                  const BlitzRGBColor& col);


    void resize(int y, int x);

    void resize(blitz::TinyVector<int,2>& pos);

    int height() const;
    int width() const;

    unsigned char getPixelR(int y, int x) const;
    unsigned char getPixelG(int y, int x) const;
    unsigned char getPixelB(int y, int x) const;

    int getImageType() const;


    unsigned char* data() const;

private:

    //image type
    ImageType type;

    //pointer to different image types (only one present, according to type)
    blitz::Array<unsigned char, 3>*                     layeredDataRGB;
    blitz::Array<unsigned char, 2>*                     layeredDataGray;
    blitz::Array<blitz::TinyVector<unsigned char,3>,2>* vectDataRGB;   


};



#endif

