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
