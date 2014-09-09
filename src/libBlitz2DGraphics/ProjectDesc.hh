/*!
 *                                                          
 *  \mainpage libBlitz2Dgraphics
 *  
 *  This is a little collection of graphics related functions for images stored
 *  within blitz arrays.
 *  <br><br>
 *  
 *  \section sec1 Overview of the functionality
 *  <ul>
 * <li>Reading images to blitz arrays from the following formats:<br>
 * jpg, png, tiff
 * </li>
 *
 * <li>Writing images from blitz arrays to the following formats:<br>
 * jpg, png, ppm(ascii), pgm(ascii)
 * </li>
 *
 * <li>Basic color space conversion:<br>
 * rgb &lt;=&gt; hsv, hsv &lt;=&gt; rgb, rgb &lt;=&gt; gray, 
 * </li>
 *
 * <li>Drawing of basic graphic primitives<br>
 * lines, circles, rectangles
 * </li>
 *
 * <li>Very simple color management<br>
 * some predefined colors and a jet color map
 * </li>
 *
 * </ul>
 * <br>
 *
 * \section sec2 blitz::Arrays as images
 *
 * 
 * There are basically two storage formats:
 * 
 * <ol>
 * <li> <b>3D blitz arrays</b> (layered representation) <br><br>
 *     The first dimension describes the different color layers, 
 *     typically RGB (red, green, blue) or HSV (hue, saturation, 
 *     value). The second dimension specifies the y, and the 
 *     third dimension the x coordinate. For gray images,
 *     the extent of the first dimension is 1.
 *     In this format, the individual color layers are stored 
 *     consecutively in memory, and processing of individual
 *     color layers becomes more efficient.
 *     <br><br>
 *     For RGB images, the data type is unsigned char, for HSV
 *     images the data type is float:<br>
 *     blitz::Array&lt;unsigned char, 3&gt; rgbImage;<br>
 *     blitz::Array&lt;float, 3&gt; hsvImage;
 *     <br><br>
 * </li>
 *
 * <li> <b>2D blitz arrays</b> (vectorial representation) <br><br>
 *     The first dimension specifies the y, the second 
 *     dimension the x coordinate of a pixel. The color at each pixel
 *     is represented as a blitz::TinyVector with 3 entries. They represent
 *     the RGB or HSV values. For gray images, scalars are used for each
 *     array entry.
 *     <br><br>
 *     Again, for RGB and gray images, the data type is unsigned char, for HSV
 *     images the data type is float:
 *     blitz::Array&lt;blitz::TinyVector&lt;unsigned char, 
 *     3&gt;, 2&gt; rgbImage;<br>
 *     blitz::Array&lt;unsigned char, 2&gt; grayImage;<br>
 *     blitz::Array&lt;blitz::TinyVector&lt;float, 3&gt;, 2&gt; hsvImage;<br>
 * </li>
 * </ol>
 * <br>
 * 
 * \section sec3 Prerequisites
 *
 * This library depends on the following libraries:
 *
 * <ul>
 * <li> Blitz++ <a href=http://www.oonumerics.org/blitz/>
 *  http://www.oonumerics.org/blitz/</a><br> 
 *  In the libBlitz2DGraphics library, we use a wrapped version 
 *  of the blitz library by the chair of pattern recognition and 
 *  image processing Freiburg (lmb). You can also use the standard 
 *  blitz library, then only the part concerning the blitz library in 
 *  the configure.in has to be exchanged.
 * </li>
 *
 * <li>jpeglib 
 * <a href=ftp://ftp.uu.net/graphics/jpeg/>ftp://ftp.uu.net/graphics/jpeg/
 * </a><br>
 * Developed with version 62, but should work with others as well.
 * </li>
 *
 * <li>pnglib 
 * <a href=http://www.libpng.org/pub/png/libpng.html>
 * http://www.libpng.org/pub/png/libpng.html</a><br>
 * Developed with version 1.2.15beta5, but should work with others as well.
 * </li>
 *
 * <li>tifflib 
 * <a href=http://www.libtiff.org/>
 * http://www.libtiff.org</a><br>
 * Developed with version 3.8.2, but should work with others as well.
 * </li>
 * </ul>
 *
 * <br>
 *
 * Questions and comments to:<br>
 * Alex.Teynor (teynor AT informatik.uni-freiburg.de)
 * <br><br>
 * December 2008
 *
 *
 *********************************************************/	

 
