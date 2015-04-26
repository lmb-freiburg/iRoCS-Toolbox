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

#ifndef MINIMALLOADSAVEPARAMETERS_HH
#define MINIMALLOADSAVEPARAMETERS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "RequireHelpers.hh"
#include "RequireStData.hh"

namespace svt_check
{
  /*======================================================================*/
  /*!
   *  \class MinimalLoadSaveParameters
   *  \brief The MinimalLoadSaveParameters provides a minimal
   *         implementation that fulfills the
   *         svt_check::RequireLoadSaveParameters interface 
   */
  /*======================================================================*/
  class MinimalLoadSaveParameters
  {
  public:
    /*======================================================================*/
    /*! 
     *   Load all internal variables of this class from the given
     *   structured data object    
     *
     *   \param stdata a structured data object containing the data
     *
     */
    /*======================================================================*/
    template<class STDATA>
    void loadParameters( STDATA& stdata)
          {
            if( false)
            {
              RequireStData<STDATA> x;
              avoidUnunsedVariableWarning( x);
            }
          }
    
    
    
    /*======================================================================*/
    /*! 
     *   Save all internal variables of this class to the given
     *   structured data object    
     *
     *   \param stdata a structured data object containing the data
     *
     */
    /*======================================================================*/
    template<class STDATA>
    void saveParameters( STDATA& stdata)
          {
            if( false)
            {
              RequireStData<STDATA> x;
              avoidUnunsedVariableWarning( x);
            }
          }
    
  };
}

#endif
