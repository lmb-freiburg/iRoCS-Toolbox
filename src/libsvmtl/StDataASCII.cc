/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 4820 $$Name$
**       $Date: 2011-11-08 10:57:01 +0100 (Tue, 08 Nov 2011) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2005/10/26 07:36:33  ronneber
**  - made all get/read-functions const. They were non-const, because the
**    derived StDataCmdLine stores, wether parameters were accessed
**    not. Making the functions const and store these informations in a
**    mutable member seems to be the cleaner way
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/

#include "StDataASCII.hh"

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:  getArraySize
 *  ==> see headerfile
 *=======================================================================*/
size_t svt::StDataASCII::getArraySize( std::string key) const
{
  size_t size;
  std::map<std::string, std::string>::const_iterator p = findKey(key);
  if (p == _map.end()) return 0;
  std::istringstream ist( p->second);
  readArraySizeFromStream( ist, size);
  return size;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:  getFVListSize
 *  ==> see headerfile
 *=======================================================================*/
size_t svt::StDataASCII::getFVArraySize( std::string key) const
{
  return getArraySize( key);
}




/*=========================================================================
 *  DESCRIPTION OF FUNCTION:  readArraySizeFromStream
 *  ==> see headerfile
 *=======================================================================*/
void svt::StDataASCII::readArraySizeFromStream( 
    std::istream& is, size_t& size) const
{
  char openBracket;
  is >> openBracket;
  if( openBracket != '[')
  {
    InvalidDataTypeError err;
    err << "Cannot interpret string as array. Does not start with '['";
    throw err;
  }
  is >> size;
  char closeBracket;
  is >> closeBracket;
  if( closeBracket != ']')
  {
    InvalidDataTypeError err;
    err << "Cannot interpret string as array. Does not contain ']' at correct position";
    throw err;
  }
}
 
