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
**       Title: test if specified class fulfills the ClassReflectionInterface
**    $RCSfile$
**   $Revision: 522 $$Name$
**       $Date: 2004-09-08 16:42:15 +0200 (Wed, 08 Sep 2004) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.1  2004/09/08 14:42:15  ronneber
**  adapted to new ParamInfo class
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef REQUIRENAMEDESCRIPTIONPARAMHELP_HH
#define REQUIRENAMEDESCRIPTIONPARAMHELP_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "RequireHelpers.hh"
#include <string>
#include <vector>

namespace svt_check
{
  
  template<typename TESTCLASS>
  class RequireNameDescriptionParamInfos
  {
  public:
    ~RequireNameDescriptionParamInfos()
          {
            if( false)
            {
              /*---------------------------------------------------------------
               *  check static members
               *--------------------------------------------------------------*/
              std::string a = TESTCLASS::name();
              avoidUnunsedVariableWarning( a);
              std::string b = TESTCLASS::description();
              avoidUnunsedVariableWarning( b);
              std::vector< svt::ParamInfo> c;
              TESTCLASS::getParamInfos( c);
            }
            
          }
  };
}


#endif
