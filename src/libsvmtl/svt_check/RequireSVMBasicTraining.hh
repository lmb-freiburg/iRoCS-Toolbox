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
#ifndef REQUIRESVMBASICTRAINING_HH
#define REQUIRESVMBASICTRAINING_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "RequireHelpers.hh"

namespace svt_check
{
  
  /*======================================================================*/
  /*!
   *  Ensure that SVM provides a typedef for a ModelType, a train(
   *  SVM_Problem, ModelType) 
   */
  /*======================================================================*/
  template<typename SVM, typename FV>
  class RequireSVMBasicTraining
  {
  public:
    ~RequireSVMBasicTraining()
          {
            if( false)
            {
              typename SVM::template Traits<FV>::ModelType model;
              SVM s;
              svt::SVM_Problem<FV> p;
              s.train( p, model);
            }
          }
  };
}


#endif
