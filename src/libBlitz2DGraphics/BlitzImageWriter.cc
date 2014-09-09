/**********************************************************
 *                                                          
 *  BlitzImageWriter.cc
 *  Implements the BlitzImageWriter class
 *  => for a precise description of the classes see 
 *  respective header files
 *                                                       
 *********************************************************/	

#include <fstream>
#include "BlitzImageWriter.hh"

#ifdef HAVE_JPEG

using namespace JPEGLIB;


/**
 * BlitzImageWriter::writeJPEG
 * save data from RGB-blitz arrays to JPEG file names with given quality
 * in case somethings goes wrong, return -1
 */
int BlitzImageWriter::writeJPEG(const ImageAccessWrapper &data, 
                                const std::string &fileName, 
                                int quality)
{
   
  struct jpeg_compress_struct jinfo;
  struct jpeg_error_mgr jerr;
  FILE *jfile;

  int width, height;// depth;
   
  jfile = fopen(fileName.c_str(), "wb");
  if (!jfile) {
    std::cerr << "BlitzImageWriter: JPG save error.\n";
    return -1;         
  }
    
  // 0. setup error handling
  jinfo.err = jpeg_std_error(&jerr);

  // 1. allocate and initialize compression object
  jpeg_create_compress(&jinfo);

  // 2. specify dest of the compressed data (file)
  jpeg_stdio_dest(&jinfo, jfile);

  // 3. get width & height of blitz-image
  width = data.width();
  height = data.height();

  //set image attributes
  jinfo.image_width =  width; 	
  jinfo.image_height = height;

  switch (data.getImageType()) {
  case ImageAccessWrapper::layeredGray: 
    jinfo.input_components = 1;	
    jinfo.in_color_space = JCS_GRAYSCALE; 
    break;

  case ImageAccessWrapper::layeredRGB:
  case ImageAccessWrapper::vectRGB:
    jinfo.input_components = 3;	
    jinfo.in_color_space = JCS_RGB;        
    break;
  default:         
    std::cerr << "BlitzImageWriter: JPG color type error.\n";
    return -1;         
  }
    
  jpeg_set_defaults(&jinfo);
  jpeg_set_quality(&jinfo, quality, true );

  jpeg_start_compress(&jinfo,true);
       
  JSAMPROW rowptr[1];


  if (jinfo.in_color_space == JCS_RGB ) {

    //copy blitz image into a temp-rgb-ordered image
    blitz::Array<JSAMPLE,2> tmp_image;
    tmp_image.resize(height,width*3);

    for (int y=0; y<height; y++) {
            
      for (int x=0; x<width; x++) {
                
        tmp_image(y, x*3 + 0) = 
            static_cast<JSAMPLE>(data.getPixelR(y,x));
        tmp_image(y, x*3 + 1) = 
            static_cast<JSAMPLE>(data.getPixelG(y,x));
        tmp_image(y, x*3 + 2) = 
            static_cast<JSAMPLE>(data.getPixelB(y,x));
      }
    }
		
    int row_stride = width * 3;	
        
    //prepare image for jpeg-compression 
    while (jinfo.next_scanline < jinfo.image_height) {
            
      rowptr[0] = & tmp_image.data()[jinfo.next_scanline * row_stride];
      (void) jpeg_write_scanlines(&jinfo, rowptr, 1);

    }

  } else if (jinfo.in_color_space == JCS_GRAYSCALE) {
        
    int row_stride = width;	
        
    //prepare image for jpeg-compression 
    while (jinfo.next_scanline < jinfo.image_height) {            
      rowptr[0] = & data.data()[jinfo.next_scanline * row_stride];
      (void) jpeg_write_scanlines(&jinfo, rowptr, 1);

    }        
  }
    

  // 7. clean up
  jpeg_finish_compress(&jinfo);
  jpeg_destroy_compress(&jinfo);

  fclose(jfile); 

  return 1;
}

#endif

/**
 * Write Blitz array as PPM (values must be between 0 and 255) 
 */            
void BlitzImageWriter::writePPM(const ImageAccessWrapper &data, 
                                const std::string& fileName, int format)
{
  if (format != 3 && format != 6) 
  {
    std::cerr << "Unrecognized PPM format (" << format << ") while writing '"
              << fileName << "'" << std::endl;
    return;
  }
  
  //open file for writing
  std::ofstream file(fileName.c_str());
  if (!file.good())
  {
    std::cerr << "Could not open '" << fileName << "' for writing" << std::endl;
    exit(-1);
  }
  
  //write header
  file << "P" << format << std::endl 
       << "# Written by BlitzImageWriter" << std::endl;
    
  //size of image
  int xsize = data.width();
  int ysize = data.height();
    
  file << xsize << " " 
       << ysize << " "
       << "255" << std::endl;

  //write data
  switch (format)
  {
  case 6:
  {
    file.write(reinterpret_cast<const char*>(data.data()), 3 * xsize * ysize);
    break;
  }
  default:
  {
    for (int i=0; i<ysize ; ++i) {
      for (int j=0; j<xsize ; ++j) {
        file << static_cast<int>(data.getPixelR(i,j)) << " ";
        file << static_cast<int>(data.getPixelG(i,j)) << " "; 
        file << static_cast<int>(data.getPixelB(i,j)) << " ";
      }
      file << std::endl;
    }
    file << std::endl;
  }
  }

  file.close();
};


/**
 * Write Blitz array as PGM (values must be between 0 and 255) 
 */            
void BlitzImageWriter::writePGM(const blitz::Array<unsigned char, 2> &data, 
                                const std::string& fileName, int format)
{
  if (format != 2 && format != 5) 
  {
    std::cerr << "Unrecognized PGM format (" << format << ") while writing '"
              << fileName << "'" << std::endl;
    return;
  }
  
  //open file for writing
  std::ofstream file(fileName.c_str());
  
  //write header
  file << "P" << format << std::endl
       << "# Written by BlitzImageWriter" << std::endl;
  
  //size of image
  int xsize = data.extent(blitz::secondDim);
  int ysize = data.extent(blitz::firstDim);
  
  file << xsize << " " 
       << ysize << " " 
       << "255" << std::endl;
  
  //write data
  switch (format)
  {
  case 5:
    file.write(reinterpret_cast<const char*>(data.data()), xsize * ysize);
    break;
  default:
    for (int i=0; i<ysize ; ++i) {
      for (int j=0; j<xsize ; ++j) {
        file << static_cast<int>(data(i,j)) << " ";               
      }
      file << std::endl;        
    }  
    file << std::endl;
    break;
  }
  
  //close file
  file.close();

  return;    
}


int BlitzImageWriter::writePNG(const ImageAccessWrapper &data, 
                               const std::string& fileName,
                               int quality) {
    

  FILE* fp = fopen(fileName.c_str(), "wb");

  if (quality < 1 || quality > 9) {
    std::cerr << "BlitzImageWriter::writePNG: " 
              << "Image Quality factor not in the right range. " 
              << "Is: " << quality << ". Must be [1-9]." << std::endl;
    return(-1);      
  }

  if (!fp) {
    std::cerr << "BlitzImageWriter::writePNG: Can't create output file" 
              << std::endl;
    return(-1);
  }
  
  png_structp png_ptr = png_create_write_struct
      (PNG_LIBPNG_VER_STRING, png_voidp_NULL,
       NULL, NULL);

  if (!png_ptr) {
    std::cerr << "BlitzImageWriter::writePNG: " 
              << "png_create_write_struct() failed" << std::endl;
    return(-1);
  }
      
  png_set_compression_level(png_ptr, quality);
  
  png_infop info_ptr = png_create_info_struct(png_ptr);

  if (!info_ptr) {
    png_destroy_write_struct(&png_ptr,
                             (png_infopp)NULL);
    std::cerr << "BlitzImageWriter::writePNG: " 
              << "png_create_info_struct() failed" << std::endl;
    return(-1);
  }
    
  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);

    std::cerr << "BlitzImageWriter::writePNG: " 
              << "setjmp(png_jmpbuf(png_ptr) failed" << std::endl;

    return (-1);
  }

  png_init_io(png_ptr, fp);

  //size of image
  int xsize = data.width();
  int ysize = data.height();

  int pngColorType;
  
  // setup the rows array
  png_byte* row_pointers[ysize];

  blitz::Array<png_byte,2> tmp_image;
  
  // prepare data, depending on the image type
  // We copy the data to a tmp storage due to unify our different
  // image storage formats. This is to make the code more
  // similar, in the case of the vectorial or gray data, this
  // might be a little inefficient.  

  switch (data.getImageType()) {
  case ImageAccessWrapper::layeredGray: 
    pngColorType = PNG_COLOR_TYPE_GRAY;  
    tmp_image.resize(ysize,xsize);
    
    for (int y=0; y<ysize; ++y) {          
      for (int x=0; x<xsize; ++x) {              
        tmp_image(y,x) = static_cast<png_byte>(data.getPixelR(y,x));
      }          
      row_pointers[y] = &tmp_image(y,0);
    }
    break;

  case ImageAccessWrapper::layeredRGB:
    pngColorType = PNG_COLOR_TYPE_RGB;
    tmp_image.resize(ysize,xsize*3);

    for (int y=0; y<ysize; ++y) {          
      for (int x=0; x<xsize; ++x) {              
        tmp_image(y, x*3 + 0) = 
            static_cast<png_byte>(data.getPixelR(y,x));
        tmp_image(y, x*3 + 1) = 
            static_cast<png_byte>(data.getPixelG(y,x));
        tmp_image(y, x*3 + 2) = 
            static_cast<png_byte>(data.getPixelB(y,x));
      }
          
      row_pointers[y] = &tmp_image(y,0);
    }
    break;
      
  case ImageAccessWrapper::vectRGB:
    pngColorType = PNG_COLOR_TYPE_RGB;
    tmp_image.resize(ysize,xsize*3);

    for (int y=0; y<ysize; ++y) {          
      for (int x=0; x<xsize; ++x) {              
        tmp_image(y, x*3 + 0) = 
            static_cast<png_byte>(data.getPixelR(y,x));
        tmp_image(y, x*3 + 1) = 
            static_cast<png_byte>(data.getPixelG(y,x));
        tmp_image(y, x*3 + 2) = 
            static_cast<png_byte>(data.getPixelB(y,x));
      }
          
      row_pointers[y] = &tmp_image(y,0);
    }
    break;

  default:         
    std::cerr << "BlitzImageWriter: data format error.\n";
    return -1;         
  }

  //set up header
  png_set_IHDR(png_ptr, info_ptr, 
               xsize,    // width
               ysize,    // height
               8,                  // bit_depth, 
               pngColorType, // color_type, 
               PNG_INTERLACE_NONE,           // interlace_type,
               PNG_COMPRESSION_TYPE_DEFAULT, // compression_type, 
               PNG_FILTER_TYPE_DEFAULT);     // filter_method


  // Now write the stuff: 

  // write header
  png_write_info(png_ptr, info_ptr);

  // write image
  png_write_image(png_ptr, row_pointers);

  // finalize write
  png_write_end(png_ptr, info_ptr);

  // free memory
  png_destroy_write_struct(&png_ptr, &info_ptr);

  // close file
  fclose( fp);

  return 1;
   
}



