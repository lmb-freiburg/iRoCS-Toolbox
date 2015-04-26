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


#ifndef REQUIRESTDATAFVARRAY_HH
#define REQUIRESTDATAFVARRAY_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <string>
#include <vector>

#include "RequireHelpers.hh"

namespace svt_check
{
  template<typename STDATA, typename FV>
  class RequireStDataFVArray
  {
  public:
    ~RequireStDataFVArray()
          {
            if(false)
            {
              STDATA s;
              int  i;
              unsigned int ui;

              std::vector<FV*> fvv1;
              FV** fvv2;
              s.setFVArray( "", fvv1.begin(), ui);
              s.setFVArray( "", fvv2, ui);
              
              ui = static_cast<unsigned int>(s.getFVArraySize( ""));

              s.getFVArray( "", fvv1.begin(), i);
              s.getFVArray( "", fvv2, i);
            }
          }
  };
}


#endif
