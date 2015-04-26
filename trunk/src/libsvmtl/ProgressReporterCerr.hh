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
**       Title: simple progress reporter (just writes to stderr)
**    $RCSfile$
**   $Revision: 1292 $$Name$
**       $Date: 2007-11-10 14:51:29 +0100 (Sat, 10 Nov 2007) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.1  2007/11/10 13:51:29  tschmidt
**  - initial revision (taken from polsoft-extsrc)
**
**  Revision 1.1  2002/05/22 16:39:06  ronneber
**  - added progress reporting capabilities
**
**
**
**************************************************************************/


#ifndef PROGRESSREPORTERCERR_HH
#define PROGRESSREPORTERCERR_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <iostream>
#include "ProgressReporter.hh"

namespace svt
{
  class ProgressReporterCerr : public ProgressReporter
  {
  public:
    virtual void reportProgress( const char* message,
                                 float percentOfCompletion)
          {
            std::cerr << percentOfCompletion << "% done: (" << message << ")\n";
          }

    virtual int reportingLevel()
          {
            return 2;
          }
    
  };
}

#endif
