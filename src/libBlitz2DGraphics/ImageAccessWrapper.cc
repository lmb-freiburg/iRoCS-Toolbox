/**********************************************************
 *                                                          
 *  ImageAccessWrapper.cc
 *  Implements the ImageAccessWrapper class
 *  => for a precise description of the classes see 
 *  respective header files
 *                                                       
 *********************************************************/	

#include "ImageAccessWrapper.hh"

ImageAccessWrapper::ImageAccessWrapper (
    blitz::Array<unsigned char, 3>& data):type(layeredRGB) {
    layeredDataRGB = &data;
}


ImageAccessWrapper::ImageAccessWrapper (
    blitz::Array<unsigned char, 2>& data):type(layeredGray) {
    layeredDataGray = &data;
}

ImageAccessWrapper::ImageAccessWrapper (
    blitz::Array<blitz::TinyVector<unsigned char, 3 >, 2>& data):
        type(vectRGB) {
    vectDataRGB = &data;       
}


void ImageAccessWrapper::setPixel(blitz::TinyVector<int,2>& pos,
                                  unsigned char R, 
                                  unsigned char G, 
                                  unsigned char B) {
    switch(type) {

    case layeredRGB:
        (*layeredDataRGB)(0,pos(0),pos(1)) = R;
        (*layeredDataRGB)(1,pos(0),pos(1)) = G;
        (*layeredDataRGB)(2,pos(0),pos(1)) = B;        
        break;        

    case layeredGray:
        (*layeredDataGray)(pos) = R;
        break;        

    case vectRGB:
        (*vectDataRGB)(pos)[0] = R;        
        (*vectDataRGB)(pos)[1] = G;        
        (*vectDataRGB)(pos)[2] = B;        
        break;        
    }
}


void ImageAccessWrapper::setPixel(blitz::TinyVector<int,2>& pos, 
                                  const BlitzRGBColor& col) {
    
    switch(type) {
    case layeredRGB:
        (*layeredDataRGB)(0,pos(0),pos(1)) = col[0];
        (*layeredDataRGB)(1,pos(0),pos(1)) = col[1];
        (*layeredDataRGB)(2,pos(0),pos(1)) = col[2];        
        break;        

    case layeredGray:
        (*layeredDataGray)(pos) = col[0];
        break;        

    case vectRGB:
        (*vectDataRGB)(pos)= col;        
        break;        
    }
    
}

void ImageAccessWrapper::setPixel(int y, int x, 
              unsigned char R, 
              unsigned char G, 
              unsigned char B) {

    switch(type) {

    case layeredRGB:
        (*layeredDataRGB)(0,y,x) = R;
        (*layeredDataRGB)(1,y,x) = G;
        (*layeredDataRGB)(2,y,x) = B;        
        break;        

    case layeredGray:
        (*layeredDataGray)(y,x) = R;
        break;        

    case vectRGB:
        (*vectDataRGB)(y,x)[0] = R;        
        (*vectDataRGB)(y,x)[1] = G;        
        (*vectDataRGB)(y,x)[2] = B;        
        break;        
    }
}



void ImageAccessWrapper::setPixel(int y, int x, const BlitzRGBColor& col) {
    
    switch(type) {
    case layeredRGB:
        (*layeredDataRGB)(0,y,x) = col[0];
        (*layeredDataRGB)(1,y,x) = col[1];
        (*layeredDataRGB)(2,y,x) = col[2];        
        break;        
        
    case layeredGray:
        (*layeredDataGray)(y,x) = col[0];
        break;        

    case vectRGB:
        (*vectDataRGB)(y,x)= col;        
        break;        
    }
}


void ImageAccessWrapper::resize(int y, int x) {

    switch(type) {

    case layeredRGB:
        layeredDataRGB->resize(3,y,x);
        break;        

    case layeredGray:
        layeredDataGray->resize(y,x);
        break;        

    case vectRGB:
        vectDataRGB->resize(y,x);        
        break;        
    }
}


void ImageAccessWrapper::resize(blitz::TinyVector<int,2>& pos) {

    resize(pos(0), pos(1));    

}



int ImageAccessWrapper::height() const {

    switch(type) {

    case layeredRGB:
        return layeredDataRGB->extent(1);
    case layeredGray:
        return layeredDataGray->extent(0);
    case vectRGB:
        return vectDataRGB->extent(0);        
    }
    
    //should not happen!
    std::cout << "ImageAccessWrapper::height(): "  
              << "Error in image type" 
              << std::endl;    
    return 0;

}



int ImageAccessWrapper::width() const {

    switch(type) {

    case layeredRGB:
        return layeredDataRGB->extent(2);
    case layeredGray:
        return layeredDataGray->extent(1);
    case vectRGB:
        return vectDataRGB->extent(1);        
    }

    //should not happen!
    std::cout << "ImageAccessWrapper::width(): " 
              << "Error in image type" 
              << std::endl;    
    return 0;

}


unsigned char ImageAccessWrapper::getPixelR(int y, int x) const {

    switch(type) {
        
    case layeredRGB:
        return (*layeredDataRGB)(0,y,x);
    case layeredGray:
        return (*layeredDataGray)(y,x);
    case vectRGB:
        return (*vectDataRGB)(y,x)[0];        
    }   

    //should not happen!
    std::cout << "ImageAccessWrapper::getPixelR(): " 
              << "Error in image type" 
              << std::endl;    
    return 0;

}


unsigned char ImageAccessWrapper::getPixelG(int y, int x) const {
    
 
    switch(type) {
        
    case layeredRGB:
        return (*layeredDataRGB)(1,y,x);
    case layeredGray:
        return (*layeredDataGray)(y,x);
    case vectRGB:
        return (*vectDataRGB)(y,x)[1];        
    }

    //should not happen!
    std::cout << "ImageAccessWrapper::getPixelG(): "
              << " Error in image type" << std::endl;    
    return 0;

}

unsigned char ImageAccessWrapper::getPixelB(int y, int x) const {

    switch(type) {
        
    case layeredRGB:
        return (*layeredDataRGB)(2,y,x);
    case layeredGray:
        return (*layeredDataGray)(y,x);
    case vectRGB:
        return (*vectDataRGB)(y,x)[2];        
    }    

    //should not happen!
    std::cout << "ImageAccessWrapper::getPixelB(): "
              << " Error in image type" << std::endl;    
    return 0;
    
}



int ImageAccessWrapper::getImageType() const {    
    return type;    
}


// data should not be modified via the AccessWrapper
unsigned char* ImageAccessWrapper::data() const {    

    switch(type) {
        
    case layeredRGB:
        return layeredDataRGB->data();
    case layeredGray:
        return layeredDataGray->data();
    case vectRGB:
        return reinterpret_cast<unsigned char*>(vectDataRGB->data());
    }   

    //should not happen!
    std::cout << "ImageAccessWrapper::data():" 
              << "Error in image type" << std::endl;    
    return NULL;
    
}
