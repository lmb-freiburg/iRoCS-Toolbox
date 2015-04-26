/**************************************************************************
 *
 * Copyright (C) 2004-2015 Olaf Ronneberger, Florian Pigorsch, Jörg Mechnich,
 *                         Thorsten Falk
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
 
