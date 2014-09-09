/**************************************************************************
 **	Title: BlitzAnalyzeTraits implementation
 **	$RCSfile$
 **	$Revision: $$Name$
 **	$Date: $
 **	Copyright: GPL $Author: $
 **	Description:
 **
 **************************************************************************/

#include "BlitzAnalyzeTraits.hh"

/*-----------------------------------------------------------------------
 *  Constructors
 *-----------------------------------------------------------------------*/

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
template<>
BlitzAnalyzeTraits<unsigned char>::BlitzAnalyzeTraits()
throw(BlitzAnalyzeError)
{

}
/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
template<>
BlitzAnalyzeTraits<short int>::BlitzAnalyzeTraits()
throw(BlitzAnalyzeError)
{

}
/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
template<>
BlitzAnalyzeTraits< blitz::TinyVector<unsigned char,3> >::BlitzAnalyzeTraits()
throw(BlitzAnalyzeError)
{

}

/*-----------------------------------------------------------------------
 *  Implementations of Type()
 *-----------------------------------------------------------------------*/

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
template<>
std::string
BlitzAnalyzeTraits<unsigned char>::
Type()
{
  std::string datatype("CHAR");
  return datatype;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
template<>
std::string
BlitzAnalyzeTraits<short int>::
Type()
{
  std::string datatype("SHORT");
  return datatype;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
template<>
std::string
BlitzAnalyzeTraits< blitz::TinyVector<unsigned char,3> >::
Type()
{
  std::string datatype("RGB");
  return datatype;
}

/*-----------------------------------------------------------------------
 *  Implementations of max()
 *-----------------------------------------------------------------------*/

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
template<>
int
BlitzAnalyzeTraits<unsigned char>::
max( const blitz::Array< unsigned char,3>& inArray)
{
  return static_cast<int>(blitz::max(inArray));
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
template<>
int
BlitzAnalyzeTraits<short int>::
max( const blitz::Array< short int,3>& inArray)
{
  return static_cast<int>(blitz::max(inArray));
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
template<>
int
BlitzAnalyzeTraits< blitz::TinyVector<unsigned char,3> >::
max( const blitz::Array< blitz::TinyVector<unsigned char,3>, 3>&)
{
  return 255;
}

/*-----------------------------------------------------------------------
 *  Implementations of min()
 *-----------------------------------------------------------------------*/

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
template<>
int
BlitzAnalyzeTraits<unsigned char>::
min( const blitz::Array< unsigned char,3>& inArray)
{
  return static_cast<int>(blitz::min(inArray));
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
template<>
int
BlitzAnalyzeTraits<short int>::
min( const blitz::Array< short int,3>& inArray)
{
  return static_cast<int>(blitz::min(inArray));
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
template<>
int
BlitzAnalyzeTraits< blitz::TinyVector<unsigned char,3> >::
min( const blitz::Array< blitz::TinyVector<unsigned char,3>, 3>& inArray)
{
  return 0;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
template<>
size_t
BlitzAnalyzeTraits<unsigned char>::
sizeof_type()
{
  return sizeof(unsigned char);
}

/*-----------------------------------------------------------------------
 *  Implementations of sizeof_type()
 *-----------------------------------------------------------------------*/

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
template<>
size_t
BlitzAnalyzeTraits<short int>::
sizeof_type()
{
  return sizeof(short int);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
template<>
size_t
BlitzAnalyzeTraits< blitz::TinyVector<unsigned char,3> >::
sizeof_type()
{
  return 3*sizeof(unsigned char);
}

