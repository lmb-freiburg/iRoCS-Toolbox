/**************************************************************************
 **	Title: BlitzAnalyzeTraits class
 **	$RCSfile$
 **	$Revision: $$Name$
 **	$Date: $
 **	Copyright: GPL $Author: $
 **	Description:
 **
 **************************************************************************/

#ifndef BLITZANALYZETRAITS_HH
#define BLITZANALYZETRAITS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <string>
#include <fstream>
#include <blitz/array.h>

//#include "BlitzAnalyzeType.hh"	// lib's type traits
#include "BlitzAnalyzeError.hh"  // lib's own error class

/*======================================================================*/
/*!
 *  \class BlitzAnalyzeTraits BlitzAnalyzeTraits.hh
 *  \brief The BlitzAnalyzeTraits class provides type traits
 *  	   for BlitzAnalyzeFile.
 *
 */
/*======================================================================*/

template<typename T>
class BlitzAnalyzeTraits {

public:

  /*======================================================================*/
  /*!
   *   Public singleton access methode (Instance)
   *   to one single instance of the class.
   */
  /*======================================================================*/
  static BlitzAnalyzeTraits* Instance();

  /*-----------------------------------------------------------------------
   *  Destructor
   *-----------------------------------------------------------------------*/
  ~BlitzAnalyzeTraits();

  /*-----------------------------------------------------------------------
   *  data methods
   *-----------------------------------------------------------------------*/

  /*======================================================================*/
  /*!
   *   Selects datatype.
   */
  /*======================================================================*/
  std::string Type();
  /*======================================================================*/
  /*!
   *   Returns maximum value in inArray or seperately specified value.
   */
  /*======================================================================*/
  int max( const blitz::Array< T,3>& inArray);
  /*======================================================================*/
  /*!
   *   Returns minimum value in inArray or seperately specified value.
   */
  /*======================================================================*/
  int min( const blitz::Array< T,3>& inArray);
  /*======================================================================*/
  /*!
   *   Returns size of datatype T in Byte.
   */
  /*======================================================================*/
  size_t sizeof_type();

protected:
  /*-----------------------------------------------------------------------
   *  Constructors
   *-----------------------------------------------------------------------*/

  /*======================================================================*/
  /*!
   *   Creates a stub BlitzAnalyzeTraits object.
   */
  /*======================================================================*/
  BlitzAnalyzeTraits()
  throw(BlitzAnalyzeError);

private:

  /*-------------------------------------------------------------------------
   *  private vars
   *-------------------------------------------------------------------------*/
  static BlitzAnalyzeTraits* _instance;

};

#include "BlitzAnalyzeTraits.icc"

#endif

