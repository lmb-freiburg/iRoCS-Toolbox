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
 *  BlitzImageReader.cc
 *  Implements the BlitzImageReader class
 *  => for a precise description of the classes see 
 *  respective header files
 *                                                       
 **********************************************************
 *
 *
 *********************************************************/	

#include <vector>
#include <cstdio>
#include <fstream>

#include "BlitzImageReader.hh"

/**
 * BlitzImageLoader::loadJPEG
 * load data from JPEG file names to blitz arrays
 * in case somethings goes wrong, return -1
 */

#ifdef HAVE_JPEG

using namespace JPEGLIB;

int BlitzImageReader::readJPEG(
    blitz::Array<unsigned char,3> &data, 
    const std::string &fileName) {

    ImageAccessWrapper wrappedData(data);
    return readJPEG(wrappedData,fileName);
}


int BlitzImageReader::readJPEG(
    blitz::Array<blitz::TinyVector<unsigned char,3> ,2> &data, 
    const std::string &fileName) {
    
    ImageAccessWrapper wrappedData(data);
    return readJPEG(wrappedData,fileName);
}


int BlitzImageReader::readJPEG(ImageAccessWrapper &data, 
                               const std::string &fileName) {

    struct jpeg_decompress_struct jinfo;
    struct jpeg_error_mgr jerr;
    FILE *jfile;
    int colcount, rowcount;

    unsigned int width, height; //, depth;
   
    jfile = fopen(fileName.c_str(), "rb");
    if (!jfile) {
       std::cerr << "BlitzImageReader: JPG load error: file not found.\n";
       return -1;         
    }
    
    // 0. setup error handling
    jinfo.err = jpeg_std_error(&jerr);

    // 1. allocate and initialize decompression object
    jpeg_create_decompress(&jinfo);

    // 2. specify source of the compressed data (file)
    jpeg_stdio_src(&jinfo, jfile);

    // 3. obtain image information
    jpeg_read_header(&jinfo, true);

    // 4. set parameters for decompression
    jinfo.do_block_smoothing = false;
    jinfo.dct_method = JDCT_FLOAT;
    jinfo.out_color_space = 
        (jinfo.jpeg_color_space == JCS_GRAYSCALE) ? JCS_GRAYSCALE : JCS_RGB;

    jpeg_calc_output_dimensions(&jinfo);
    width = jinfo.output_width;
    height = jinfo.output_height;
    
    // 5. begin decompression
    jpeg_start_decompress(&jinfo);

    // 6. read decompress image data
    JSAMPROW rowptr[1];
    
    if (jinfo.jpeg_color_space == JCS_GRAYSCALE) {

        // depth = 1;
        //create Blitz array of approriate size
        data.resize(height,width);

        std::vector<unsigned char> im(width);
        rowcount = 0;

        while (jinfo.output_scanline < height) {
            
            rowptr[0] = &im[0];
            jpeg_read_scanlines(&jinfo, rowptr, 1);

            //copy data to our format
            //  ordering: z, y, x: depth,height,width
            //  first index makes biggest jumps
            for (unsigned int w = 0; w < width; ++w) {
                data.setPixel(rowcount,w,
                              im[w],
                              im[w],
                              im[w]);
            }

            ++rowcount;            
        }
        
    } else if (jinfo.jpeg_color_space == JCS_RGB || 
               jinfo.jpeg_color_space == JCS_YCbCr) {
       
        // depth = 3;
        //create Blitz array of approriate size
        data.resize(height,width);

        std::vector<unsigned char> im(width*3);
        rowcount = 0;
        while (jinfo.output_scanline < height) {
            rowptr[0] = &im[0];
            jpeg_read_scanlines(&jinfo, rowptr, 1);
            colcount = 0;
            
            //copy data to our format
            //  ordering: z, y, x: depth,height,width 
            //  fist index makes biggest jump
            for (unsigned int w = 0; w < width*3; w += 3) {
                data.setPixel(rowcount,colcount,
                              im[w],
                              im[w+1],
                              im[w+2]);
                ++colcount;
            }           
            ++rowcount;

        }

    } else {

        std::cerr << "BlitzImageReader: "
                  << "JPEG load error: color space not recognized:"
             <<  jinfo.jpeg_color_space <<  "\n";

        jpeg_finish_decompress(&jinfo);
        jpeg_destroy_decompress(&jinfo);
        fclose(jfile); 
        return -1;
        
    }

    // 7. clean up
    jpeg_finish_decompress(&jinfo);
    jpeg_destroy_decompress(&jinfo);

    fclose(jfile); 

    return 0;

}

#endif

int BlitzImageReader::readPNG (blitz::Array<unsigned char,3> &data, 
                               const std::string &fileName) {

    ImageAccessWrapper wrappedData(data);
    return readPNG(wrappedData,fileName);
}


int BlitzImageReader::readPNG (
    blitz::Array<blitz::TinyVector<unsigned char, 3>,2> &data, 
    const std::string &fileName) {
    
    ImageAccessWrapper wrappedData(data);
    return readPNG(wrappedData,fileName);
}



int BlitzImageReader::readPNG(ImageAccessWrapper &data, 
                              const std::string &fileName) {

    // code from adapted from: 
    // http://www.libpng.org/pub/png/libpng-1.2.5-manual.html

   png_structp png_ptr;
   png_infop info_ptr;

   png_uint_32 width, height;
   int color_type;
   const unsigned short PNG_BYTES_TO_CHECK = 2;

   unsigned int i, j;

   FILE *fp;
   png_byte buf[PNG_BYTES_TO_CHECK];
   
   int rowcount;


   // Open file 
   if ((fp = fopen(fileName.c_str(), "rb")) == NULL) {
       std::cerr << "BlitzImageReader: PNG load error: file not found.\n";
       return -1;
   }

   // Read in some of the signature bytes => to check if it is a png
   if (fread(buf, 1, PNG_BYTES_TO_CHECK, fp) != PNG_BYTES_TO_CHECK)
      return -1;

   // Compare the first PNG_BYTES_TO_CHECK bytes of the signature.
   // value of png_sig_cmp is 1 if read bytes match signature*/
   if (png_sig_cmp(buf, (png_size_t)0, PNG_BYTES_TO_CHECK)) {
       std::cerr << "BlitzImageReader: PNG load error: " << fileName << " not a png file.\n";
       return -1;
   }
   
   // Create and initialize the png_struct with the desired error handler
   // functions.  If you want to use the default stderr and longjump method,
   // you can supply NULL for the last three parameters.  We also supply the
   // the compiler header file version, so that we know if the application
   // was compiled with a compatible version of the library.  REQUIRED 
   png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                    NULL, 
                                    NULL, 
                                    NULL);

   if (png_ptr == NULL) {
      fclose(fp);
      std::cerr << "BlitzImageReader: PNG load error: " 
                << "error handling could not be set up.\n";
      return -1;
   }

   // Allocate/initialize the memory for image information.  REQUIRED. 
   info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL) {
      fclose(fp);
      png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
      
      std::cerr << "BlitzImageReader: PNG load error: "
                << "memory initialization\n";
      return -1;
   }

   // Set error handling if you are using the setjmp/longjmp method (this is
   // the normal method of doing things with libpng).  REQUIRED unless you
   // set up your own error handlers in the png_create_read_struct() earlier.
   //

   if (setjmp(png_jmpbuf(png_ptr))) {
      /* Free all of the memory associated with the png_ptr and info_ptr */
      png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
      fclose(fp);

      /* If we get here, we had a problem reading the file */
      std::cerr << "BlitzImageReader: PNG load error: "
                << "setjmp error handling\n";
      return -1;
   }

   // Set up the input control if you are using standard C streams */
   png_init_io(png_ptr, fp);
   
   // If we have already read some of the signature */
   png_set_sig_bytes(png_ptr, PNG_BYTES_TO_CHECK);

   // read the image in info structure (must fit in memory all at a time)
   // read without alpha channel
   png_read_png(png_ptr, info_ptr,PNG_TRANSFORM_STRIP_ALPHA, png_voidp_NULL);

   //get row pointers & other info
   png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
   width                   = png_get_image_width(png_ptr, info_ptr);
   height                  = png_get_image_height(png_ptr, info_ptr);
   color_type              = png_get_color_type(png_ptr, info_ptr);

   //case: colored images
   //we can treat with and without alpha the same, 
   //since we strip alpha channel 
   if (color_type == PNG_COLOR_TYPE_RGB && PNG_COLOR_TYPE_RGB_ALPHA) {
       
       //read in blitz
       data.resize(height,width);

       for (j=0; j<height; ++j) {
           
           rowcount=0;
           
           for (i=0; i<width*3; i+=3) {

               //replace with Accessors
               data.setPixel(static_cast<int>(j),rowcount,  
                             static_cast<unsigned char>(row_pointers[j][i]),
                             static_cast<unsigned char>(row_pointers[j][i+1]),
                             static_cast<unsigned char>(row_pointers[j][i+2]));
               
               rowcount++;
           }
       }
       
   } else if  (color_type == PNG_COLOR_TYPE_GRAY && PNG_COLOR_TYPE_GRAY_ALPHA) {
       
       //read in blitz
       data.resize(height,width);

       std::cout << "Gray image" << std::endl;
       
       for (j=0; j<height; ++j) {
        
           for (i=0; i<width; ++i) {

               //replace with Accessors
               data.setPixel(static_cast<int>(j),static_cast<int>(i),  
                             static_cast<unsigned char>(row_pointers[j][i]),
                             static_cast<unsigned char>(row_pointers[j][i]),
                             static_cast<unsigned char>(row_pointers[j][i]));
           }
       }

   } else {
       std::cerr << "BlitzImageReader: PNG read error: "
                 <<"this PNG color type is not supported yet\n";  
   }
   
   // clean up after the read, and free any memory allocated - REQUIRED
   png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);

   // close the file
   fclose(fp);

   // that's it
   return 0;
}


int BlitzImageReader::readPNM(
    blitz::Array<unsigned char,3> &data, const std::string &fileName)
{
  ImageAccessWrapper wrappedData(data);
  return readPNM(wrappedData, fileName);
}


int BlitzImageReader::readPNM(
    blitz::Array<blitz::TinyVector<unsigned char, 3>,2> &data, 
    const std::string &fileName)
{    
  ImageAccessWrapper wrappedData(data);
  return readPNM(wrappedData, fileName);
}

std::istream &BlitzImageReader::pnmSkipToNextToken(std::istream &pnmStream)
{
  while (pnmStream.good())
  {
    pnmStream >> std::ws;
    if (static_cast<char>(pnmStream.peek()) != '#') break;
    pnmStream.ignore(std::numeric_limits<std::streamsize>::max(), 10);
  }
  return pnmStream;
}

int BlitzImageReader::readPNM(
    ImageAccessWrapper &data, const std::string &fileName)
{
  try
  {
    std::ifstream pnmStream(fileName.c_str());

    // Get actual file type and read header
    char magic[2];
    pnmStream.get(magic[0]);
    pnmStream.get(magic[1]);
    if (magic[0] != 'P')
    {
      std::cerr << fileName << " is no valid pnm file." << std::endl;
      exit(-1);
    }
    pnmSkipToNextToken(pnmStream);

    int width, height, maxVal;

    switch (magic[1])
    {
    case '1': // pbm (ASCII)
    {
      pnmStream >> width >> pnmSkipToNextToken >> height >> pnmSkipToNextToken;
      data.resize(height, width);
      for (ptrdiff_t row = 0; pnmStream.good() && row < height; ++row)
      {
        for (ptrdiff_t col = 0; pnmStream.good() && col < width; ++col)
        {
          if (!pnmStream.good())
          {
            std::cerr << "Error while reading '" << fileName
                      << ": pbm file is incomplete" << std::endl;
            exit(-1);
          }
          int tmp;
          pnmStream >> pnmSkipToNextToken >> tmp;
          unsigned char value = static_cast<unsigned char>((1 - tmp) * 255);
          data.setPixel(row, col, value, value, value);
        }
      }
      break;
    }
    case '4': // pbm (binary)
    {
      pnmStream >> width >> pnmSkipToNextToken >> height;
      pnmStream.ignore();
      data.resize(height, width);
      for (ptrdiff_t row = 0; pnmStream.good() && row < height; ++row)
      {
        for (ptrdiff_t col = 0; pnmStream.good() && col < width; )
        {
          if (!pnmStream.good())
          {
            std::cerr << "Error while reading '" << fileName
                      << ": pbm file is incomplete" << std::endl;
            exit(-1);
          }
          char buf;
          pnmStream.get(buf);
          for (int j = 7; col < width && j >= 0; --j)
          {
            unsigned char value = 255 * (1 - ((buf & (1 << j)) >> j));
            data.setPixel(row, col, value, value, value);
            ++col;
          }
        }
      }
      break;
    }
    case '2': // pgm (ASCII)
    {
      pnmStream >> width >> pnmSkipToNextToken >> height
                >> pnmSkipToNextToken >> maxVal >> pnmSkipToNextToken;
      if (maxVal > 255)
      {
        std::cerr << "Sorry, only grayvalues up to 255 are supported. The "
                  << "maximum value specified in the pgm file is " << maxVal
                  << std::endl;
        exit(-1);
      }
      data.resize(height, width);
      for (ptrdiff_t row = 0; pnmStream.good() && row < height; ++row)
      {
        for (ptrdiff_t col = 0; pnmStream.good() && col < width; ++col)
        {
          if (!pnmStream.good())
          {
            std::cerr << "Error while reading '" << fileName
                      << ": pgm file is incomplete" << std::endl;
            exit(-1);
          }
          int tmp;
          pnmStream >> pnmSkipToNextToken >> tmp;
          unsigned char value = static_cast<unsigned char>(tmp);
          data.setPixel(row, col, value, value, value);
        }
      }
      break;
    }
    case '5': // pgm (binary)
    {
      pnmStream >> width >> pnmSkipToNextToken >> height
                >> pnmSkipToNextToken >> maxVal;
      pnmStream.ignore();
      data.resize(height, width);
      for (ptrdiff_t row = 0; pnmStream.good() && row < height; ++row)
      {
        for (ptrdiff_t col = 0; pnmStream.good() && col < width; ++col)
        {
          if (!pnmStream.good())
          {
            std::cerr << "Error while reading '" << fileName
                      << ": pgm file is incomplete" << std::endl;
            exit(-1);
          }
          char buf;
          pnmStream.get(buf);
          unsigned char value = *reinterpret_cast<unsigned char*>(&buf);
          data.setPixel(row, col, value, value, value);
        }
      }
      break;
    }
    case '3': // ppm (ASCII)
    {
      pnmStream >> width >> pnmSkipToNextToken >> height
                >> pnmSkipToNextToken >> maxVal >> pnmSkipToNextToken;
      if (maxVal > 255)
      {
        std::cerr << "Sorry, only grayvalues up to 255 are supported. The "
                  << "maximum value specified in the ppm file is " << maxVal
                  << std::endl;
        exit(-1);
      }
      data.resize(height, width);
      for (ptrdiff_t row = 0; pnmStream.good() && row < height; ++row)
      {
        for (ptrdiff_t col = 0; pnmStream.good() && col < width; ++col)
        {
          if (!pnmStream.good())
          {
            std::cerr << "Error while reading '" << fileName
                      << ": ppm file is incomplete" << std::endl;
            exit(-1);
          }
          int buf[3];
          pnmStream >> pnmSkipToNextToken >> buf[0]
                    >> pnmSkipToNextToken >> buf[1]
                    >> pnmSkipToNextToken >> buf[2];
          unsigned char r = static_cast<unsigned char>(buf[0]);
          unsigned char g = static_cast<unsigned char>(buf[1]);
          unsigned char b = static_cast<unsigned char>(buf[2]);
          data.setPixel(row, col, r, g, b);
        }
      }
      break;
    }
    case '6': // ppm (binary)
    {
      pnmStream >> width >> pnmSkipToNextToken >> height
                >> pnmSkipToNextToken >> maxVal;
      pnmStream.ignore();
      data.resize(height, width);
      for (ptrdiff_t row = 0; pnmStream.good() && row < height; ++row)
      {
        for (ptrdiff_t col = 0; pnmStream.good() && col < width; ++col)
        {
          if (!pnmStream.good())
          {
            std::cerr << "Error while reading '" << fileName
                      << ": ppm file is incomplete" << std::endl;
            exit(-1);
          }
          unsigned char r = static_cast<unsigned char>(pnmStream.get());
          unsigned char g = static_cast<unsigned char>(pnmStream.get());
          unsigned char b = static_cast<unsigned char>(pnmStream.get());
          data.setPixel(row, col, r, g, b);
        }
      }
      break;
    }
    default:
      std::cerr << fileName << " is no valid pnm file or its type is not "
                << "supported: Read magic is " << magic << std::endl;
      exit(-1);
    }
    
    if (!pnmStream.good())
    {
      std::cerr << fileName << " is truncated." << std::endl;
      exit(-1);
    }
  }
  catch (std::exception &e)
  {
    std::cerr << "Error while processing '" << fileName << "': " << e.what()
              << std::endl;
    exit(-1);
  }
  return 0;
}

#ifdef HAVE_TIFF

int BlitzImageReader::readTIFF(blitz::Array<unsigned char,3> &data, 
                               const std::string &fileName)
{
  ImageAccessWrapper wrappedData(data);
  return readTIFF(wrappedData,fileName);
}

int BlitzImageReader::readTIFF(
    blitz::Array<blitz::TinyVector<unsigned char, 3>,2> &data, 
    const std::string &fileName)
{
  ImageAccessWrapper wrappedData(data);
  return readTIFF(wrappedData,fileName);
}

int BlitzImageReader::readTIFF(ImageAccessWrapper &data, 
                               const std::string &fileName)
{
  TIFF* tif = TIFFOpen(fileName.c_str(), "r");
  if (!tif) 
  {
    std::cerr << "Error: Can't open `" << fileName << "'\n";
    return -1;
  }

  // Read needed TIFF header information
  uint32 fcols;
  TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &fcols);
  
  uint32 frows;
  TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &frows);
  
  blitz::Array<blitz::TinyVector<unsigned char,4>,2> imageData(fcols, frows);
  int result = TIFFReadRGBAImageOriented(
      tif, fcols, frows, reinterpret_cast<uint32*>(imageData.data()),
      ORIENTATION_TOPLEFT, 0);
  if (result != 1) 
  {
    std::cerr << "Could not read image data." << std::endl;
    return -1;
  }
  
  data.resize(fcols, frows);

  for (ssize_t r = 0; r < static_cast<ssize_t>(frows); ++r)
  {
    for (ssize_t c = 0; c < static_cast<ssize_t>(fcols); ++c) 
    {
      data.setPixel(static_cast<int>(c), static_cast<int>(r),
                    imageData(c, r)(0), imageData(c, r)(1), imageData(c, r)(2));
    }
  }

  TIFFClose(tif);
  return 0;
}

#endif
