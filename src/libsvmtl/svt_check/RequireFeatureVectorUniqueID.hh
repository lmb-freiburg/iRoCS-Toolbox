/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 476 $$Name$
**       $Date: 2004-08-26 10:36:59 +0200 (Thu, 26 Aug 2004) $
**   Copyright: GPL $Author: ronneber $
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

#ifndef REQUIREFEATUREVECTORUNIQUEID_HH
#define REQUIREFEATUREVECTORUNIQUEID_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "RequireHelpers.hh"

namespace svt_check
{
  template<  typename FV>
  class RequireFeatureVectorUniqueID
  {
  public:
    
    ~RequireFeatureVectorUniqueID()
          {
            if( false)
            {
              unsigned int (FV::*x1)() const = &FV::uniqueID;
              avoidUnunsedVariableWarning(x1);
            }
          }
  };
}

#endif
