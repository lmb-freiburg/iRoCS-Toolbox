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
#ifndef REQUIRESVMBASICCLASSIFICATION_HH
#define REQUIRESVMBASICCLASSIFICATION_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <vector>
#include "RequireHelpers.hh"

namespace svt_check
{
  
  /*======================================================================*/
  /*!
   *  Ensure that SVM provides the methods calcClassificationCache()
   *  and classifyWithCache()  
   */
  /*======================================================================*/
  template<typename SVM, typename FV>
  class RequireSVMBasicClassification
  {
  public:
    ~RequireSVMBasicClassification()
          {
            if( false)
            {
              typename SVM::template Traits<FV>::ModelType model;
              SVM s;
              FV fv;
              std::vector<FV*> fvv;
              std::vector<double> kernelResultByUID;
              s.calcClassificationCache( fv, fvv.begin(),fvv.end(), 
                                         kernelResultByUID);
              double d = s.classifyWithCache( model, kernelResultByUID);
              avoidUnunsedVariableWarning( d);
            }
          }
  };
}


#endif
