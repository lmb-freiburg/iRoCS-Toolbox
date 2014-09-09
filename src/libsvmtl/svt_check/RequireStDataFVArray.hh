/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 4820 $$Name$
**       $Date: 2011-11-08 10:57:01 +0100 (Tue, 08 Nov 2011) $
**   Copyright: GPL $Author: tschmidt $
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


#ifndef REQUIRESTDATAFVARRAY_HH
#define REQUIRESTDATAFVARRAY_HH

#include <string>
#include <vector>

#include "RequireHelpers.hh"

namespace svt_check
{
  template<typename STDATA, typename FV>
  class RequireStDataFVArray
  {
  public:
    ~RequireStDataFVArray()
          {
            if(false)
            {
              STDATA s;
              int  i;
              unsigned int ui;

              std::vector<FV*> fvv1;
              FV** fvv2;
              s.setFVArray( "", fvv1.begin(), ui);
              s.setFVArray( "", fvv2, ui);
              
              ui = static_cast<unsigned int>(s.getFVArraySize( ""));

              s.getFVArray( "", fvv1.begin(), i);
              s.getFVArray( "", fvv2, i);
            }
          }
  };
}


#endif
