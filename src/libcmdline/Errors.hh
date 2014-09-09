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
