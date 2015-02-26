/**************************************************************************
**       Title: test if specified class fulfills the ClassReflectionInterface
**    $RCSfile$
**   $Revision: 522 $$Name$
**       $Date: 2004-09-08 16:42:15 +0200 (Wed, 08 Sep 2004) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.1  2004/09/08 14:42:15  ronneber
**  adapted to new ParamInfo class
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef REQUIRENAMEDESCRIPTIONPARAMHELP_HH
#define REQUIRENAMEDESCRIPTIONPARAMHELP_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "RequireHelpers.hh"
#include <string>
#include <vector>

namespace svt_check
{
  
  template<typename TESTCLASS>
  class RequireNameDescriptionParamInfos
  {
  public:
    ~RequireNameDescriptionParamInfos()
          {
            if( false)
            {
              /*---------------------------------------------------------------
               *  check static members
               *--------------------------------------------------------------*/
              std::string a = TESTCLASS::name();
              avoidUnunsedVariableWarning( a);
              std::string b = TESTCLASS::description();
              avoidUnunsedVariableWarning( b);
              std::vector< svt::ParamInfo> c;
              TESTCLASS::getParamInfos( c);
            }
            
          }
  };
}


#endif
