/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 752 $$Name$
**       $Date: 2005-10-25 14:09:55 +0200 (Tue, 25 Oct 2005) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.4  2005/10/25 12:09:55  ronneber
**  - added classificationDetailsByUID() and
**    get/setStoreClassificationDetailsFlag()
**
**  Revision 1.3  2005/03/29 17:49:42  ronneber
**  - added updateKernelCache() and clearKernelCache()
**
**  Revision 1.2  2004/09/08 14:12:58  ronneber
**  - added saveOnlyKernelParameters()
**
**  Revision 1.1  2004/08/26 08:36:58  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef BASICCVADAPTERTEMPL_HH
#define BASICCVADAPTERTEMPL_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "BasicCVAdapter.hh"
#include "CrossValidator.hh"

namespace svt
{
  template<typename FV, 
           typename PROBLEM,
           typename STDATA,
           typename SVMTYPE>
  class BasicCVAdapterTempl : public BasicCVAdapter<FV,PROBLEM,STDATA>
  {
  public:
    virtual void setProgressReporter( ProgressReporter* pr)
          {
            _cv.setProgressReporter( pr);
          }
    
    virtual void loadParameters( STDATA& stData)
          {
            _cv.loadParameters( stData);
          }
    
    virtual void loadParameters( StDataASCII& stData)
          {
            _cv.loadParameters( stData);
          }
    
    virtual void loadParameters( StDataCmdLine& stData)
          {
            _cv.loadParameters( stData);
          }
    
    virtual void saveParameters( STDATA& stData)
          {
            _cv.saveParameters( stData);
          }
    
    virtual void saveParameters( StDataASCII& stData)
          {
            _cv.saveParameters( stData);
          }
    
    virtual void saveOnlyKernelParameters( StDataASCII& stData)
          {
            _cv.svm()->twoClassSVM().kernel().saveParameters( stData);
          }
    
    virtual void setTrainingData( const PROBLEM* problem)
          {
            _cv.setTrainingData( problem);
          }
    
    virtual const PROBLEM* trainingData() const
          {
            return _cv.trainingData();
          }
    
    virtual void updateKernelCache()
          {
            _cv.updateKernelCache();
          }
    
    virtual void clearKernelCache()
          {
            _cv.clearKernelCache();
          }
    

    virtual void preprocessTrainingData()
          {
            _cv.preprocessTrainingData();
          }
    
    virtual int doFullCV( const std::vector<int>& subsetIndexByUID,
                          std::vector<double>& predictedClassLabelByUID)
          {
            return _cv.doFullCV( subsetIndexByUID, predictedClassLabelByUID);
          }
    
    virtual void setClassificationDelta( double d)
          {
            _cv.setClassificationDelta(d);
          }
       
    virtual double classificationDelta() const 
          {
            return _cv.classificationDelta();
          }
    
    virtual void setStoreClassificationDetailsFlag( bool f)
          {
            _cv.setStoreClassificationDetailsFlag(f);
          }
    
    virtual bool getStoreClassificationDetailsFlag() const
          {
            return _cv.getStoreClassificationDetailsFlag();
          }
    
    virtual void saveStatistics( STDATA& statistics,
                                 int detailLevel=1)
          {
            _cv.saveStatistics( statistics, detailLevel);
          }
    
    virtual void saveStatistics( StDataASCII& statistics,
                                      int detailLevel=1)
          {
            _cv.saveStatistics( statistics, detailLevel);
          }
    
    virtual const std::vector< StDataASCII>& classificationDetailsByUID() const
          {
            return _cv.classificationDetailsByUID();
          }
    
    
  private:
    svt::CrossValidator<FV, SVMTYPE, PROBLEM> _cv;
  };
}

#endif
    
    

    
    
