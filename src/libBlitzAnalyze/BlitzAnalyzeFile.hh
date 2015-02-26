/**************************************************************************
 **	Title: BlitzAnalyzeFile class
 **	$RCSfile$
 **	$Revision: $$Name$
 **	$Date: $
 **	Copyright: GPL $Author: $
 **	Description:
 **
 **************************************************************************/

/*======================================================================*/
/*!
 * -----------------------------
 * - Copyright Acknowledgement -
 * -----------------------------
 * The Analyze header type definitions in "BlitzAnalyzeType.hh" and
 * the methode "saveArrayWithElemSize" in "BlitzAnalyzeFile.icc"
 * are mainly based on the Analyze 7.5 File Format document
 * http://www.mayo.edu/bir/PDF/ANALYZE75.pdf
 * (.../libBlitzAnalyze/docs/ANALYZE75.pdf)
 *
 * It contains the following copyright notice regarding the
 * header file format:
 *
 * ANALYZE TM Header File Format
 * (c) Copyright, 1986-1995
 * Biomedical Imaging Resource
 * Mayo Foundation
 */
/*======================================================================*/

#ifndef BLITZANALYZEFILE_HH
#define BLITZANALYZEFILE_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <string>
#include <fstream>
#include <blitz/array.h>

#include "BlitzAnalyzeType.hh"	// lib's type definitions for Analyze header
#include "BlitzAnalyzeError.hh"  // lib's own error class
#include "BlitzAnalyzeTraits.hh" // lib's type traits

/*======================================================================*/
/*!
 *  \class BlitzAnalyzeFile BlitzAnalyzeFile.hh
 *  \brief The BlitzAnalyzeFile class provides a function
 *  		for saving of voxel blocks to the Analyze file format.
 *
 *	Currently supported are Blitz-Arrays of data element types:
 *		unsigned char
 *		short int
 *		blitz::TinyVector<unsigned char,3>	//for RGB arrays
 */
/*======================================================================*/

class BlitzAnalyzeFile {

public:

  /*======================================================================*/
  /*!
   *   Creates an AnalyzeFile, opening the given file.
   *
   *   \param fName  file to be opened, without file extention!
   */
  /*======================================================================*/
  BlitzAnalyzeFile( const std::string& fName);

  /*-----------------------------------------------------------------------
   *  Destructor
   *-----------------------------------------------------------------------*/
  ~BlitzAnalyzeFile();

  /*-----------------------------------------------------------------------
   *  data methods
   *-----------------------------------------------------------------------*/

  /*======================================================================*/
  /*!
   *   Writes a three dimensional data set.
   *
   *   \param inArray			array to be written into file
   *   \param element_size_um	element size in um (lev,row,col)
   *
   *   Supported types T:
   *   		unsigned char
   *   		short int
   *   		blitz::TinyVector<unsigned char,3>
   */
  /*======================================================================*/
  template<typename T>
  void saveArrayWithElemSize( const blitz::Array<T,3>& inArray, const blitz::TinyVector<float,3>& element_size_um)
      throw(BlitzAnalyzeError);

private:

  /*-------------------------------------------------------------------------
   *  private vars
   *-------------------------------------------------------------------------*/

  const std::string _fName;
  std::string _datatype;
  bool _saveArrayWithElemSize_called;

};

#include "BlitzAnalyzeFile.icc"

#endif

