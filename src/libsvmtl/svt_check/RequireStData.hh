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


#ifndef REQUIRESTDATA_HH
#define REQUIRESTDATA_HH

#include <string>
#include <vector>

#include "RequireHelpers.hh"

namespace svt_check
{
  template<typename STDATA>
  class RequireStData
  {
  public:
    ~RequireStData()
          {
            if(false)
            {
              STDATA s;

              s.setExceptionFlag(true);
              bool b = s.exceptionFlag();
              
              b = s.valueExists( "");
              
              int  i;
              unsigned int ui;
              float f;
              double d;
              std::string st;

              s.setValue( "", b );
              s.setValue( "", i );
              s.setValue( "", ui );
              s.setValue( "", f );
              s.setValue( "", d );
              s.setValue( "", st );

              std::vector<double> v1;
              std::vector<unsigned int> v2;
              double* v3;
              s.setArray( "", v1.begin(), ui );
              s.setArray( "", v2.begin(), ui );
              s.setArray( "", v3, ui );

              
              s.getValue( "", b );
              s.getValue( "", i );
              s.getValue( "", ui );
              s.getValue( "", f );
              s.getValue( "", d );
              s.getValue( "", st );
              
              ui = static_cast<unsigned int>(s.getArraySize( ""));

              s.getArray(  "", v1.begin(), i );
              s.getArray(  "", v2.begin(), i );
              s.getArray(  "", v3, i );
        
            }
          }
  };
}


#endif
