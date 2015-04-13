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
**   $Revision: 538 $$Name$
**       $Date: 2004-09-13 12:04:04 +0200 (Mon, 13 Sep 2004) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2004/09/13 10:04:04  ronneber
**  - documentation update
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef REQUIRELOADSAVEPARAMETERS_HH
#define REQUIRELOADSAVEPARAMETERS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "RequireHelpers.hh"
#include "MinimalStData.hh"
#include <string>
#include <map>
#include <set>

namespace svt_check
{
  
  /*======================================================================*/
  /*!
   *  \class RequireLoadSaveParameters
   *  \brief Ensure that TESTCLASS provides a loadParameters() and
   *         saveParamters() method. 
   *
   *   See MinimalLoadSaveParameters for an example  
   */
  /*======================================================================*/
  template<typename TESTCLASS>
  class RequireLoadSaveParameters
  {
  public:
    ~RequireLoadSaveParameters()
          {
            if( false)
            {
              /*--------------------------------------------------------------
               *  check templated members
               *--------------------------------------------------------------*/
              void (TESTCLASS::*x1)(MinimalStData&) = &TESTCLASS::loadParameters;
              avoidUnunsedVariableWarning(x1);
              
              void (TESTCLASS::*x2)(MinimalStData&) const = &TESTCLASS::saveParameters;
              avoidUnunsedVariableWarning(x2);
            }
          }
  
    
  };
}


#endif
