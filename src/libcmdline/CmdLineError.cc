/**************************************************************************
**       Title: basic exception
**    $RCSfile$
**   $Revision: 3319 $$Name$
**       $Date: 2010-01-23 17:10:43 +0100 (Sat, 23 Jan 2010) $
**   Copyright: GPL $Author: emmenlau $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.1  2002/03/26 07:36:28  ronneber
**  restructuring for autoconf
**
**  Revision 1.1.1.1  2002/03/22 13:45:07  pigorsch
**  moved from polsoft repository
**
**
**
**************************************************************************/

#include "CmdLineError.hh"

CmdLineError::
CmdLineError()
{
}

CmdLineError::
CmdLineError(const CmdLineError& copy)
 : pMessage(copy.str())
{
}

CmdLineError::
~CmdLineError()
{
}

