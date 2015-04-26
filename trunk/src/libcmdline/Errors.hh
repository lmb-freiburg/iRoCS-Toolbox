/**************************************************************************
 *
 * Copyright (C) 2005-2015 Olaf Ronneberger, Jörg Mechnich, Florian Pigorsch,
 *                         Mario Emmenlauer, Thorsten Schmidt
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
**       Title: Exceptions...
**    $RCSfile$
**   $Revision: 115 $$Name$
**       $Date: 2003-01-31 03:52:28 +0100 (Fri, 31 Jan 2003) $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2003/01/31 02:52:28  mechnich
**  added XML output of all CmdLine arguments
**
**  Revision 1.1  2002/04/04 12:01:54  pigorsch
**  - initial revision
**
**
**
**************************************************************************/
#ifndef ERRORS_HH
#define ERRORS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "CmdLineError.hh"

/// Usage Error. Is thrown when the --help/-? CmdArg is parsed.
class CmdLineUsageError: public CmdLineError {};

/// Usage HTML Error. Is thrown when the --helphtml CmdArg is parsed.
class CmdLineUsageHTMLError: public CmdLineError {};

/// Usage XML Error. Is thrown when the --usagexml CmdArg is parsed.
class CmdLineUsageXMLError: public CmdLineError {};

/// Syntax Error. Is thrown when an syntax error occurs.
class CmdLineSyntaxError: public CmdLineError {};

/// Internal Error. Is thrown when an internal error occurs.
class CmdLineInternalError: public CmdLineError {};

/// Dublicate Argument Error. Is thrown when an ambigous CmdArg is appended.
class CmdLineDublicateArgError: public CmdLineError {};

#endif
