/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 4820 $$Name$
**       $Date: 2011-11-08 10:57:01 +0100 (Tue, 08 Nov 2011) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    Adapter for any Combination of multi-class, two-class and kernel
**    algorithms 
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.15  2006/10/06 13:50:05  fehr
**  linear model optimizer added
**
**  Revision 1.14  2006/01/25 08:53:51  fehr
**  added HistIntersect to Kmatrix Kernels and new BasicSVMAdapter interface to TwoClassModels
**
**  Revision 1.13  2005/11/07 22:18:37  mechnich
**  changes for g++ 4
**
**  Revision 1.12  2005/10/26 06:58:06  ronneber
**  - added set/getStoreClassificationDetailsFlag()
**  - instead of classify() that returns a double detailedResultsVec, now
**    the details can be requested after classify() with the more general
**    saveClassificationDetailsASCII()
**
**  Revision 1.11  2005/07/19 13:03:59  haasdonk
**  removed redundant cout-messages, erroneous filename and added a new function
**  for computing a training-kernel matrix in BasicSVMAdapter*
**
**  Revision 1.10  2005/06/06 21:23:31  haasdonk
**  added updateCache() with two FV-lists, required for classification with precomputed kernel-matrices
**
**  Revision 1.9  2005/03/29 17:50:58  ronneber
**  - added updateKernelCache() and clearKernelCache()
**
**  Revision 1.8  2005/02/24 16:58:17  fehr
**  sometimes... it's better to go home: FINAL bugfix
**
**  Revision 1.7  2005/02/24 16:33:58  fehr
**  bugfix
**
**  Revision 1.6  2005/02/24 15:23:05  fehr
**  bugfix
**
**  Revision 1.5  2005/02/24 13:45:49  fehr
**  enable SV access through SVMAdapter
**
**  Revision 1.4  2005/02/24 12:57:50  fehr
**  some more 64-bit netcdf bugfixing
**
**  Revision 1.3  2005/02/23 16:04:05  fehr
**  added SV access for SVMAdapter
**
**  Revision 1.2  2004/09/08 14:15:36  ronneber
**  - added saveOnlyKernelParameters()
**
**  Revision 1.1  2004/08/26 08:36:58  ronneber
**  initital import
**
**
**
**************************************************************************/


#ifndef BASICSVMADAPTERTEMPL_HH
#define BASICSVMADAPTERTEMPL_HH

#include "BasicSVMAdapter.hh"
#include "CrossValidator.hh"
#include "ModelOptimizer_linearNormal.hh"

namespace svt
{

  template< typename FV, 
            typename STDATA, 
            typename MCSVMTYPE>
  class BasicSVMAdapterTempl : public BasicSVMAdapter<FV,STDATA>
  {
  public:
    BasicSVMAdapterTempl()
            :_pr(0)
          {}
    
     virtual void setProgressReporter( ProgressReporter* pr)
          {
            _pr = pr;
            _svm.setProgressReporter( pr);
          }
    
    

    virtual void loadParameters( STDATA& stData)
          {
            _svm.loadParameters( stData);
          }

    
    virtual void loadParameters( StDataASCII& stData)
          {
            _svm.loadParameters( stData);
          }

    
    virtual void loadParameters( StDataCmdLine& stData)
          {
            _svm.loadParameters( stData);
          }
    
    

    virtual void loadModel(  STDATA& stData)
          {
            _model.loadParameters( stData);
          }
    

   virtual void saveParameters( STDATA& stData) const
          {
            _svm.saveParameters( stData);
          }
    
   virtual void saveParameters( StDataASCII& stData) const
          {
            _svm.saveParameters( stData);
          }
    
    virtual void saveOnlyKernelParameters( StDataASCII& stData) const
          {
            _svm.twoClassSVM().kernel().saveParameters( stData);
          }
    


    virtual void saveModel( STDATA& stData) const
          {
            _model.saveParameters( stData);
          }

    virtual void saveTrainingInfos( STDATA& stData,
                                    int detailLevel = 1)
          {
            saveTrainingInfosTempl( stData, detailLevel);
          }
    
    virtual void saveTrainingInfos( StDataASCII& stData,
                                         int detailLevel = 1)
          {
            saveTrainingInfosTempl( stData, detailLevel);
          }
    
    virtual void updateKernelCache( const GroupedTrainingData<FV>& trainData)
          {
            _svm.updateKernelCache( 
                trainData.FV_begin(),
                trainData.FV_end(),
                DereferencingAccessor());
          }
    
    virtual void updateKernelCache( const SVM_Problem<FV>& problem)
          {
            _svm.updateKernelCache( 
                problem.FV_begin(),
                problem.FV_end(),
                DereferencingAccessor());
          }
    
    virtual void updateTestKernelCache(typename 
                                       std::vector<FV>::iterator FV_begin, 
                                       const typename 
                                       std::vector<FV>::iterator& FV_end)
          {
            typename std::vector<FV*>::const_iterator svbegin=
                _model.collectedSupportVectors().begin();
            typename std::vector<FV*>::const_iterator svend=
                _model.collectedSupportVectors().end();
            _svm.twoClassSVM().kernel().updateCache(FV_begin,FV_end, 
                                                    DirectAccessor(),
                                                    svbegin,svend, 
                                                    DereferencingAccessor());
          }


    virtual void train( const GroupedTrainingData<FV>& trainData)
          {

            _svm.train( trainData, _model);
          }
    


    virtual void train( const SVM_Problem<FV>& problem)
          {
            _svm.train( problem, _model);
          }
  

    virtual void clearKernelCache()
          {
            _svm.twoClassSVM().kernel().clearCache();
          }
    

    virtual double classify( const FV& testObject)
          {
            if( BasicSVMAdapter<FV,STDATA>::_storeClassificationDetailsFlag)
            {
              return _svm.classify( testObject, _model, _detailedResults);
            }
            else
            {
              return _svm.classify( testObject, _model);
            }
          }

//    virtual double classify( const FV& testObject, 
//                             std::vector<double>& detailedResultsVec)
//          {
//            typename MCSVMTYPE::DetailedResultType detailedResults;
//            double result = _svm.classify( testObject, _model, 
//                                           detailedResults);
//            detailedResultsVec.resize( detailedResults.size());
//            std::copy( detailedResults.begin(), detailedResults.end(),
//                       detailedResultsVec.begin());
//            return result;
//          }

    
    virtual void saveClassificationDetailsASCII( StDataASCII& stData) const
          {
            _svm.saveClassificationDetails( _model, _detailedResults, stData);
          }
    

    virtual void computeTrainKernelMatrix(float** kmatrix,  
                                          const std::vector<FV>& featureVectors)
          {            
//            return _svm.twoClassSVM().kernel().k_function(fv1,fv2);  
            size_t nfv = featureVectors.size();
            
            typename std::vector<FV>::const_iterator it =
                featureVectors.begin();
            for (size_t i = 0; i < nfv; i++, it++)
            {
              typename std::vector<FV>::const_iterator jt =
                  featureVectors.begin();
              for (size_t j = 0; j < nfv; j++, jt++)
                  kmatrix[i][j] = static_cast<float>(
                      _svm.twoClassSVM().kernel().k_function(*it,*jt));
            }
          }

    virtual int nClasses() const
          {
            return _model.nClasses();
          }
    
    double classIndexToLabel( unsigned int classIndex) const
          {
            return _model.classIndexToLabel( classIndex);
          }
    
    size_t nSupportVectors() const
	 {
	     return _model.getSupportVectors().size();
	 }
	
    
    FV* getSupportVector(int i) const
	 {
	     return _model.getSupportVectors().operator[](i);
	 }

    std::vector<const typename svt::Model<FV>* > getTwoClassModels() const
	{
	    std::vector<const typename svt::Model<FV>* > models;
	    for (unsigned int i = 0; i<_model.nTwoClassModels();i++)
	    {
		models.push_back(&(_model.twoClassModel(i)));
	    }
	    return models;
	}

    std::vector<typename svt::Model<FV>* > getTwoClassModels() 
      {
            std::vector<typename svt::Model<FV>* > models;
            for (unsigned int i = 0; i<_model.nTwoClassModels();i++)
            {
                models.push_back(&(_model.twoClassModel(i)));
            }
            return models;
      }

    void optimizeLinearModel()
    {
    /*---------------------------------------------------------------------
     * Optimize Linear Model: precalc normals for fast classification 
     *---------------------------------------------------------------------*/
	svt::ModelOptimizer_linearNormal<FV> optimizer;
	optimizer.optimizeModel(_model);
	    
    }
      
    protected:
    template<typename STDATA2>
    void saveTrainingInfosTempl(STDATA2& stData, int detailLevel)
          {
            if( detailLevel == 2)
            {
              _model.saveTCTrainingInfos( stData);
            }
            _model.saveTrainingInfoStatistics( stData);
          }

  private:
    MCSVMTYPE             _svm;
    typename MCSVMTYPE::template Traits<FV>::ModelType _model;
    typename MCSVMTYPE::DetailedResultType _detailedResults; 
    ProgressReporter* _pr;
    
  };

}


#endif
