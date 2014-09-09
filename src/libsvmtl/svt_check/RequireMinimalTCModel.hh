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
#ifndef REQUIREMINIMALTCMODEL_HH
#define REQUIREMINIMALTCMODEL_HH

#include <vector>

#include "RequireHelpers.hh"
#include "RequireFeatureVectorUniqueID.hh"
#include "MinimalStData.hh"

#include "../StDataASCII.hh"

namespace svt_check
{
  
  /*======================================================================*/
  /*!
   *  Ensure that TCMODEL provides minimal functions to be used With
   *  Multiclass-Algortithms 
   */
  /*======================================================================*/
  template<typename TCMODEL>
  class RequireMinimalTCModel
  {
    CHECK_CLASS_TEMPLATE1( RequireFeatureVectorUniqueID< typename TCMODEL::FV_type>)

  public:
    ~RequireMinimalTCModel()
          {
            if( false)
            {
              typedef typename TCMODEL::FV_type FV;
              
              TCMODEL m;
              // access to Support vectors
              size_t ui = m.size();
              avoidUnunsedVariableWarning( ui);
              
              FV* fv = m.supportVector( static_cast<int>(ui) );
              avoidUnunsedVariableWarning( fv);
              
              
               // training info stuff
              double d = m.getTrainingInfoValue( "");
              avoidUnunsedVariableWarning( d);
              
              std::string s = m.trainingInfoPlainText();
              avoidUnunsedVariableWarning( s);

              MinimalStData stdata;
              m.saveTrainingInfo( stdata, "");
              

              // loading and saving of parameters
              m.saveParametersWithUIDs( stdata, "");

              std::vector<FV*> fvv;
              m.loadParametersWithUIDs( stdata, fvv, "");
              
              
              // Infos for fast retraining
              std::vector<char> v;
              bool f = m.isModelAffectedByLeftOutVectors( v);
              avoidUnunsedVariableWarning( f);
              
              
            }
          }
  };
}


#endif
