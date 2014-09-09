/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 674 $$Name$
**       $Date: 2005-03-29 20:04:36 +0200 (Tue, 29 Mar 2005) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2005/03/29 18:04:36  ronneber
**  - added requirement for updateKernelCache() and clearKernelCache()
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/
#ifndef REQUIRESVMFORCROSSVALIDATION_HH
#define REQUIRESVMFORCROSSVALIDATION_HH

#include <vector>
#include "RequireHelpers.hh"

namespace svt_check
{
  
  /*======================================================================*/
  /*!
   *  Ensure that SVM provides a typedef for a ModelType, a train(
   *  SVM_Problem, ModelType) 
   */
  /*======================================================================*/
  template<typename FV, typename SVM, typename PROBLEM>
  class RequireSVMForCrossValidation
  {
  public:
    ~RequireSVMForCrossValidation()
          {
            if( false)
            {
              SVM s;
              PROBLEM p;
              s.updateKernelCache(p.FV_begin(), p.FV_end(), svt::DereferencingAccessor());
              s.clearKernelCache();
              
              std::vector<char> leaveOutFlagsByUID;
              
              typename SVM::template Traits<FV>::ModelType model;
              s.train( p, model);
              s.retrainWithLeftOutVectors( p, model, leaveOutFlagsByUID, model);
              
              FV fv;
              s.classify( fv, model);
              
              
            }
          }
  };
}


#endif
