/**************************************************************************
** 
** Some tests for the Graphics library 
**
**
**************************************************************************/


#include "Blitz2DGraphics.hh"
#include <vector>

using namespace std;


// -------------------------------------------------------------
// Formated output
// -------------------------------------------------------------
void printCA (blitz::Array<unsigned char, 3> data) {

    for( int i=0; i<data.extent(0); ++i) {
        for( int j=0; j<data.extent(1); ++j) {
            for( int k=0; k<data.extent(2); ++k) {

                cout << (int)data(i,j,k) << " ";                 
            }
            cout << endl;        
        }
        cout <<  endl;        
    }    
}

// -------------------------------------------------------------
// Formated output
// -------------------------------------------------------------
void printCA (blitz::Array<unsigned char, 2> data) {

    for( int j=0; j<data.extent(0); ++j) {
        for( int k=0; k<data.extent(1); ++k) {
            
            cout << (int)data(j,k) << " ";                 
        }
        cout << endl;        
    }

}

// -------------------------------------------------------------
// Formated output
// -------------------------------------------------------------
void printCA (blitz::Array<blitz::TinyVector<unsigned char, 3>, 2> data) {

    for( int i=0; i<data.extent(0); ++i) {
        for( int j=0; j<data.extent(1); ++j) {
            for( int k=0; k<3; ++k) {
                cout << (int)data(i,j)(k) << " ";                 
            }

            cout << ",";        
        }
        cout <<  endl;        
    }    
}



// -------------------------------------------------------------
// Main program
// -------------------------------------------------------------
int main(int argc, char** argv) {

    cout << "--------------------------------------------------" << endl;    
    cout << " Testing image and color formats: " << endl;    
    cout << "--------------------------------------------------" << endl;    
    cout << endl;
    
    // Supported image formats  --------------------------------

    blitz::Array<unsigned char, 3> 
        layeredImageRGB(3,10,10);
    blitz::Array<unsigned char, 2> 
        layeredImageGray(10,10);
    blitz::Array<blitz::TinyVector<unsigned char, 3>, 2> 
        vectorialImageRGB(10,10);

    layeredImageRGB = (unsigned char) 0;
    layeredImageGray = (unsigned char) 0;
    vectorialImageRGB = (unsigned char) 0;

    // Test colors  --------------------------------------------
    BlitzRGBColor myCol(1,2,3);
    BlitzRGBColor col1 = BlitzRGBColors::aquamarine;
    BlitzRGBColor col2 = BlitzRGBColors::jet(0.5f);
    

    cout << "--------------------------------------------------" << endl;    
    cout << " Testing accessing image data: "  << endl;    
    cout << "--------------------------------------------------" << endl;    
    cout << endl;

    // Index vector for coding a position in an image ----------
    blitz::TinyVector<int,2> pos(5,5);

    // Image access wrapper  -----------------------------------
    ImageAccessWrapper wrapLayeredRGB(layeredImageRGB);
    ImageAccessWrapper wrapLayeredGray(layeredImageGray);
    ImageAccessWrapper wrapVectorialRGB(vectorialImageRGB);
    
    std::vector<ImageAccessWrapper> allImages;
    allImages.push_back(wrapLayeredRGB);
    allImages.push_back(wrapLayeredGray);
    allImages.push_back(wrapVectorialRGB);
    
    std::vector<ImageAccessWrapper>::iterator imageIt;

    //setting pixles via the access wrapper
    for (imageIt  = allImages.begin(); 
         imageIt != allImages.end();
         ++imageIt) {
        
        imageIt->setPixel(pos,col1);       
        imageIt->setPixel(50,50,col2);       

    }

    // Image as text output: -----------------------------------
    
    cout << "layeredImageRGB: " << endl;    
    printCA(layeredImageRGB);
    cout << endl << endl;    

    cout << "layeredImageGray: " << endl;    
    printCA(layeredImageGray);
    cout << endl << endl;        

    cout << "vectorialImageRGB: " << endl;    
    printCA(vectorialImageRGB);
    cout << endl << endl;    


    cout << "--------------------------------------------------" << endl;    
    cout << " Testing Reading images " << endl;    
    cout << "--------------------------------------------------" << endl;    

    // a) color PNG:
    cout << "Reading color PNG into layeredRGB image: ";    
    BlitzImageReader::readPNG(layeredImageRGB,"test.png");
    cout << endl;    
    printCA(layeredImageRGB);
    cout << endl << endl;    
    
    cout << "Reading color PNG into vectorialRGB image: ";    
    BlitzImageReader::readPNG(vectorialImageRGB,"test.png");
    cout << endl;    
    printCA(vectorialImageRGB);
    cout << endl << endl;    


    // b) gray PNG 
    cout << "Reading gray PNG into layeredRGB image: ";    
    BlitzImageReader::readPNG(layeredImageRGB,"testGray.png");
    std::cout << endl;    
    printCA(layeredImageRGB);
    cout << endl << endl;    

    cout << "Reading gray PNG into vectorialRGB image: ";    
    BlitzImageReader::readPNG(vectorialImageRGB,"testGray.png");
    cout << endl;    
    printCA(vectorialImageRGB);
    cout << endl << endl;    

#ifdef HAVE_JPEG

    // c) color JPEG:
    cout << "Reading color JPEG into layeredRGB image: ";    
    BlitzImageReader::readJPEG(layeredImageRGB,"test.jpg");
    cout << endl;    
    printCA(layeredImageRGB);


    cout << "Reading color JPEG into vectorialRGB image: ";
    BlitzImageReader::readJPEG(vectorialImageRGB,"test.jpg");
    cout << endl;    
    printCA(vectorialImageRGB);
    cout << endl << endl;    

#endif

    cout << "--------------------------------------------------" << endl;    
    cout << " Testing Color Conversion: "  << endl;    
    cout << "--------------------------------------------------" << endl;    


    unsigned char grayVal;    
    myCol = 244,30,50;

    cout << endl;
    cout << "RGB to Gray: ....................................." << endl;
    cout << endl;    
    cout << "Singe pixel conversion: " << endl;
    cout << endl;    

    ColorConvert::rgb2gray(244,30,50,grayVal);
    cout << "RGB->Gray 244,30,50 is " << (int)grayVal << endl;

    ColorConvert::rgb2gray(myCol,grayVal);
    cout << "RGB(vector)->Gray(vector)" 
         << (int)myCol(0) << "," << (int)myCol(1) << "," << (int)myCol(2)
         <<  " is " << (int)grayVal << endl;

    cout << endl;    
    cout << "Entire image conversion: " << endl;
    cout << endl;    


    ColorConvert::rgb2gray(layeredImageRGB,layeredImageGray);
    cout << "Gray image from layeredRGB image " << endl;    
    printCA(layeredImageGray);
    cout << endl;    

    ColorConvert::rgb2gray(vectorialImageRGB,layeredImageGray);
    cout << "Gray image from vectorialRGB image " << endl;    
    printCA(layeredImageGray);
    cout << endl;    


    cout << endl;
    cout << "RGB to HSV: ....................................." << endl;
    cout << endl;    

    cout << "Singe pixel conversion: " << endl;
    cout << endl;    

    float ch;    
    float cs;    
    float cv;    

    ColorConvert::rgb2hsv(244,30,50,ch,cs,cv);
    cout << "RGB->HSV : 244,30,50 is " 
         << ch << "," << cs << "," << cv << endl;

    blitz::TinyVector<float,3> hsvCol;
    hsvCol = 0.0f;
    ColorConvert::rgb2hsv(myCol,hsvCol);
    cout << "RGB(vector)->HSV(vector):  "  
         << (int)myCol(0) << "," << (int)myCol(1) << "," << (int)myCol(2)
         << " is " << hsvCol << endl;

    cout << endl;    
    cout << "Entire image conversion: " << endl;
    cout << endl;    

    blitz::Array<float,3> layeredHSV;
    blitz::Array<blitz::TinyVector<float,3>,2> vectorialHSV;


    ColorConvert::rgb2hsv(layeredImageRGB,layeredHSV);
    cout << "layeredHSV from layeredRGB: " 
         << layeredHSV << endl;
    cout << endl;        

    ColorConvert::rgb2hsv(vectorialImageRGB,vectorialHSV);
    cout << "vectorialHSV from vectorialRGB: " 
         << vectorialHSV  << endl;
    cout << endl;    


    cout << endl;
    cout << "HSV to RGB: ....................................." << endl;
    cout << endl;    

    cout << "Singe pixel conversion: " << endl;
    cout << endl;    

    unsigned char cr;
    unsigned char cg;
    unsigned char cb;
    
    ColorConvert::hsv2rgb(ch,cs,cv,cr,cg,cb);
    cout << "HSV->RGB : " << ch << "," << cs << "," << cv << " is "
         << (int)cr << "," << (int)cg << "," << (int)cb << endl;

    
    ColorConvert::hsv2rgb(hsvCol,myCol);
    cout << "HSV(vector)->RGB(vector): " << hsvCol << " is " 
         << (int)myCol(0) << "," << (int)myCol(1) << "," 
         << (int)myCol(2)  << endl;


    cout << endl;    
    cout << "Entire image conversion: " << endl;
    cout << endl;    

    ColorConvert::hsv2rgb(layeredHSV,layeredImageRGB);
    cout << "layeredRGB from layeredHSV " << endl;
    printCA(layeredImageRGB);
    cout << endl;    

    ColorConvert::hsv2rgb(vectorialHSV,vectorialImageRGB);
    cout << "vectorialRGB from vectorialHSV "  << endl;
    printCA(vectorialImageRGB);
    cout << endl;    



    cout << "--------------------------------------------------" << endl;    
    cout << " Testing Graphic Primitives: "  << endl;    
    cout << "--------------------------------------------------" << endl;    

    //make the images a little bit bigger:
    layeredImageRGB.resize(3,100,150);      
    vectorialImageRGB.resize(100,150);    
    layeredImageGray.resize(100,150);

    //make them black
    layeredImageRGB = 0;      
    vectorialImageRGB = BlitzRGBColors::black;    
    layeredImageGray = 0;

    //setPixel
    cout << "Set Pixel ........................................" << endl;

    Blitz2Ddraw::setPixel(layeredImageRGB,10,20,
                        BlitzRGBColors::aquamarine);
    cout << " in a layeredRGB image. " << endl;    

    Blitz2Ddraw::setPixel(vectorialImageRGB,10,20,
                        BlitzRGBColors::aquamarine);
    cout << " in a vectorialRGB image. " << endl;    

    Blitz2Ddraw::setPixel(layeredImageRGB,10,20,
                        BlitzRGBColors::aquamarine);
    cout << " in a layeredGray image. " << endl;    
    cout << endl << endl;



    //drawLine
    cout << "Draw Line ........................................" << endl;

    Blitz2Ddraw::drawLine(layeredImageRGB,10,20,80,120,
                          BlitzRGBColors::copper);
    cout << " in a layeredRGB image. " << endl;    

    Blitz2Ddraw::drawLine(vectorialImageRGB,10,20,80,120,
                          BlitzRGBColors::copper);
    cout << " in a vectorialRGB image. " << endl;    

    Blitz2Ddraw::drawLine(layeredImageGray,10,20,80,120,
                          (unsigned char)100);
    cout << " in a layeredGray image. " << endl;    
    cout << endl << endl;
    

    //drawCircle
    cout << "Draw Circle 1 ...................................." << endl;

    Blitz2Ddraw::drawCircle(layeredImageRGB,10,20,80,
                            BlitzRGBColors::aquamarine);
    cout << " in a layeredRGB image. " << endl;    

    Blitz2Ddraw::drawCircle(vectorialImageRGB,10,20,80,
                            BlitzRGBColors::aquamarine);
    cout << " in a vectorialRGB image. " << endl;    

    Blitz2Ddraw::drawCircle(layeredImageGray,10,20,80,
                            (unsigned char)180);
    cout << " in a layeredGray image. " << endl;    


    cout << "Draw Circle 2 ...................................." << endl;

    Blitz2Ddraw::drawCircle(layeredImageRGB,50,50,30,
                            BlitzRGBColors::yellow);
    cout << " in a layeredRGB image. " << endl;    

    Blitz2Ddraw::drawCircle(vectorialImageRGB,50,50,30,
                            BlitzRGBColors::yellow);
    cout << " in a vectorialRGB image. " << endl;    

    Blitz2Ddraw::drawCircle(layeredImageGray,50,50,30,
                            (unsigned char)230);
    cout << " in a layeredGray image. " << endl;    
    cout << endl << endl;


    //drawRect
    cout << "Draw Rect 1 ......................................" << endl;

    Blitz2Ddraw::drawRect(layeredImageRGB,10,110,60,144,
                            BlitzRGBColors::white);
    cout << " in a layeredRGB image. " << endl;    

    Blitz2Ddraw::drawRect(vectorialImageRGB,10,110,60,144,
                            BlitzRGBColors::white);
    cout << " in a vectorialRGB image. " << endl;    

    Blitz2Ddraw::drawRect(layeredImageGray,10,110,60,144,
                            (unsigned char)230);
    cout << " in a layeredGray image. " << endl;    


    cout << "Draw Rect 2 ......................................" << endl;

    Blitz2Ddraw::drawRect(layeredImageRGB,40,60,90,10,
                            BlitzRGBColors::cyan);
    cout << " in a layeredRGB image. " << endl;    

    Blitz2Ddraw::drawRect(vectorialImageRGB,40,60,90,10,
                            BlitzRGBColors::cyan);
    cout << " in a vectorialRGB image. " << endl;    

    Blitz2Ddraw::drawRect(layeredImageGray,40,60,90,10,
                            (unsigned char)230);
    cout << " in a layeredGray image. " << endl;    
    cout << endl << endl;



    cout << "--------------------------------------------------" << endl;    
    cout << " Testing Writing images "  << endl;    
    cout << "--------------------------------------------------" << endl;    

#ifdef HAVE_JPEG

    cout << endl << "layeredRGB to JPEG" << endl << flush;
    BlitzImageWriter::writeJPEG(layeredImageRGB,"outLayeredRGB.jpg");

    cout << "vectorialRGB to JPEG"  << endl << flush;
    BlitzImageWriter::writeJPEG(vectorialImageRGB,"outVectorialRGB.jpg");
    
    cout << "layeredGray to JPEG(gray)" << endl << flush;
    BlitzImageWriter::writeJPEG(layeredImageGray,"outLayeredGray.jpg");
    cout << endl << endl;

#endif

    cout << "layeredRGB to PPM" << endl << flush;
    BlitzImageWriter::writePPM(layeredImageRGB,"outLayeredPPM.ppm");

    cout << "vectorialRGB to PPM" << endl << flush;
    BlitzImageWriter::writePPM(vectorialImageRGB,"outVectorialPPM.ppm");

    cout << "layeredGray to PPM" << endl << flush;
    BlitzImageWriter::writePPM(layeredImageGray,"outLayeredGray.ppm");
    cout << endl << endl;


    cout << endl << "layeredRGB to PNG" << endl << flush;
    BlitzImageWriter::writePNG(layeredImageRGB,"outLayeredRGB.png");

    cout << "vectorialRGB to PNG"  << endl << flush;
    BlitzImageWriter::writePNG(vectorialImageRGB,"outVectorialRGB.png");
    
    cout << "layeredGray to PNG(gray)" << endl << flush;
    BlitzImageWriter::writePNG(layeredImageGray,"outLayeredGray.png");
    cout << endl << endl;



    cout << "layeredGray to PGB" << endl << flush;
    BlitzImageWriter::writePGM(layeredImageGray,"outLayeredGray.pgm");
    cout << endl << endl;

    
    cout << "Tests done." << endl << flush;

}
