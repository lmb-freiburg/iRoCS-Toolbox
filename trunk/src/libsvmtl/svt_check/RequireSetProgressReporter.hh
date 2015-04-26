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
**   $Revision: 590 $$Name$
**       $Date: 2005-01-28 15:29:34 +0100 (Fri, 28 Jan 2005) $
**   Copyright: GPL $Author: fehr $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.3  2005/01/28 14:29:34  fehr
**  some more include bugfixes
**
**  Revision 1.2  2005/01/28 14:03:30  fehr
**  some include bugfixing for external library use
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef REQUIRESETPROGRESSREPORTER_HH
#define REQUIRESETPROGRESSREPORTER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "../ProgressReporter.hh"

namespace svt_check
{
  /*======================================================================*/
  /*!
   *  Ensure that TESTCLASS provides a setProgressReporter() method
   */
  /*======================================================================*/
  template<typename TESTCLASS>
  class RequireSetProgressReporter
  {
  public:
    ~RequireSetProgressReporter()
          {
            if( false)
            {
              void (TESTCLASS::*x1)(svt::ProgressReporter*) = &TESTCLASS::setProgressReporter;
              avoidUnunsedVariableWarning(x1);
              
            }
          }
  
    
  };
}


#endif
