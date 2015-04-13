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
**       Title: StDataASCII with additional load/save methods
**    $RCSfile$
**   $Revision: 764 $$Name$
**       $Date: 2005-10-26 09:38:33 +0200 (Wed, 26 Oct 2005) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2005/10/26 07:38:33  ronneber
**  - replaced unsigned int by size_t to make it 64bit compatible and
**    remove some compiler warnings
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/


#include "StDataASCIIFile.hh"

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:  load
 *  ==> see headerfile
 *=======================================================================*/
void svt::StDataASCIIFile::load( std::istream& is, int compatibilityFlag)
{
  /*-----------------------------------------------------------------------
   *  read whole line
   *-----------------------------------------------------------------------*/
  int lineNo = 0;
  std::string line;
  while( std::getline(is, line))
  {
    ++lineNo;
    if( compatibilityFlag == 1)
    {
      if( line == "SV")
      {
        std::string value;
        std::string line2;
        while( std::getline(is, line2) && line2 != "")
        {
          ++lineNo;
          if( value != "") value += separatorChar();
          value += line2;
        }
        _map[line] = value;
        continue;
      }
    }
    
    /*---------------------------------------------------------------------
     *  search for key/value separator (shoul be ' ')
     *---------------------------------------------------------------------*/
    size_t sepPos = line.find( ' ');
    if( sepPos == std::string::npos)
    {
      LoadError err;
      err << "load error: No key/value separator in line " << lineNo 
          << ": '" << line;
      throw( err);
    }

    std::string value = line.substr( sepPos+1);

    if( compatibilityFlag == 1)
    {
      std::replace( value.begin(), value.end(), ' ', separatorChar());
    }
    
    _map[line.substr(0,sepPos)] = value;
  }
  
      
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:  save
 *  ==> see headerfile
 *=======================================================================*/
void svt::StDataASCIIFile::save( std::ostream& os, int compatibilityFlag) const
{
  std::map<std::string,std::string>::const_iterator sv_pointer = _map.end();
  
  for(  std::map<std::string,std::string>::const_iterator
            p = _map.begin(); p != _map.end(); ++p)
  {
    if( compatibilityFlag == 1)
    {
      if( p->first == "SV")
      {
        sv_pointer = p;
      }
      else
      {
        std::string value = p->second;
        std::replace( value.begin(), value.end(), separatorChar(), ' ');
        os << p->first << ' ' << value << std::endl;
      }
    }
    else // normal operation
    {
      os << p->first << ' ' << p->second << std::endl;
    }
  }
  
  if( compatibilityFlag == 1 && sv_pointer != _map.end())
  {
    std::string value = sv_pointer->second;
    std::replace( value.begin(), value.end(), separatorChar(), '\n');
    os << sv_pointer->first << '\n' << value << std::endl;
  }
  
}
