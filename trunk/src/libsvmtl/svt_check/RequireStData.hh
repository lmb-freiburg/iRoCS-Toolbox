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
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/


#ifndef REQUIRESTDATA_HH
#define REQUIRESTDATA_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <string>
#include <vector>

#include "RequireHelpers.hh"

namespace svt_check
{
  template<typename STDATA>
  class RequireStData
  {
  public:
    ~RequireStData()
          {
            if(false)
            {
              STDATA s;

              s.setExceptionFlag(true);
              bool b = s.exceptionFlag();
              
              b = s.valueExists( "");
              
              int  i;
              unsigned int ui;
              float f;
              double d;
              std::string st;

              s.setValue( "", b );
              s.setValue( "", i );
              s.setValue( "", ui );
              s.setValue( "", f );
              s.setValue( "", d );
              s.setValue( "", st );

              std::vector<double> v1;
              std::vector<unsigned int> v2;
              double* v3;
              s.setArray( "", v1.begin(), ui );
              s.setArray( "", v2.begin(), ui );
              s.setArray( "", v3, ui );

              
              s.getValue( "", b );
              s.getValue( "", i );
              s.getValue( "", ui );
              s.getValue( "", f );
              s.getValue( "", d );
              s.getValue( "", st );
              
              ui = static_cast<unsigned int>(s.getArraySize( ""));

              s.getArray(  "", v1.begin(), i );
              s.getArray(  "", v2.begin(), i );
              s.getArray(  "", v3, i );
        
            }
          }
  };
}


#endif
