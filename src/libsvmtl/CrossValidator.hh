/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 2824 $$Name$
**       $Date: 2009-09-14 09:30:46 +0200 (Mon, 14 Sep 2009) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.3  2005/10/26 07:00:50  ronneber
**  - added set/getStoreClassificationDetailsFlag()
**  - now collects classification details and can return them via
**    classificationDetailsByUID()
**
**  Revision 1.2  2005/03/29 17:51:41  ronneber
**  - added updateKernelCache() and clearKernelCache()
**
**  Revision 1.1  2004/08/26 08:36:58  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef CROSSVALIDATOR_HH
#define CROSSVALIDATOR_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <vector>
#include <cmath>
#include "GroupedTrainingData.hh"
#include "ProgressReporter.hh"
#include "StDataASCII.hh"

// requirements of template parameters
#include "svt_check/RequireFeatureVectorUniqueID.hh"
#include "svt_check/RequireSetProgressReporter.hh"
#include "svt_check/RequireSVMForCrossValidation.hh"
#include "svt_check/RequireStData.hh"


namespace svt
{

  void generateSortedSubsets( int nFeatureVectors, int nSubsets,
                              std::vector<int>& subsetIndizesForFVs);
    
  void generateShuffledSubsets( int nFeatureVectors, int nSubsets,
                                std::vector<int>& subsetIndizesForFVs);


/*======================================================================*/
/*!
 *  \class CrossValidator CrossValidator.hh
 *  \brief The CrossValidator class provides a highly optimized cross
 *   validation algorithm
 *
 *  In a first step the SVM's for the full data set are trained and in the
 *  following steps only those SVM's are retrained, for which one or
 *  more of the left out vectors where support vectors. For this the
 *  given SVM class must provide a method retrainWithLeftOutVectors(
 *  trainData, fullModel, leaveOutFlagByUID, resultingModel)
 *  
 *  The template parameters are the following:
 *
 *  \param FV       feature vector class, e.g. BasicFV
 *  \param SVMTYPE  SVM class, could be any one-class, two-class or
 *                  multi-class SVMe.g. MultiClassSVMOneVsOne<
 *                  TwoClassSVMc< Kernel_LINEAR> > 
 *  \param PROBLEM  container for your training data,
 *                  e.g. GroupedTrainingData or SVM_Problem (which is
 *                  considerably slower with multi-class SVM's) 
 *
 *  To use it, do the following steps:
 *  - setTrainingData( ...);
 *  - adjust parameters of svm()  (optional)
 *  - setClassificationDelta()    (optional)
 *  - preprocessTrainingData();
 *  - calculate a segmentation of your data into subsets (e.g. with
 *    generateShuffledSubsets())
 *  - doFullCV() or doPartialCV();
 */
/*======================================================================*/
  template< typename FV, typename SVMTYPE, typename PROBLEM = GroupedTrainingData<FV> >
  class CrossValidator
  {
    CHECK_CLASS_TEMPLATE1( svt_check::RequireSetProgressReporter<SVMTYPE>)
    CHECK_CLASS_TEMPLATE2( svt_check::RequireFeatureVectorUniqueID<FV>)
    // macros with 3 parameters seem not to work...
    svt_check::RequireSVMForCrossValidation<FV,SVMTYPE,PROBLEM>___req3;
    
    
  public:
    typedef typename SVMTYPE::template Traits<FV>::ModelType ModelType;
    typedef FV        FV_TYPE;
    typedef PROBLEM   PROBLEM_TYPE;
    

    CrossValidator( SVMTYPE* svm = 0)
            : _svm(svm),
              _owningSVM(false),
              _problem( 0),
              _classificationDelta( 0.01),
              _pr(0),
              _sum_nSV(0),
              _sum_nFV(0),
              _sum_nBSV(0),
              _storeClassificationDetailsFlag( false)

          {
            if( _svm == 0)
            {
              _svm = new SVMTYPE;
              _owningSVM = true;
            }
          }
    
    ~CrossValidator()
          {
            if( _owningSVM)
            {
              delete _svm;
            }
          }
    
  private:
    // forbid copying
    CrossValidator( const CrossValidator<FV, SVMTYPE, PROBLEM>& orig) {}
    void operator=( const CrossValidator<FV, SVMTYPE, PROBLEM>& orig) {}
  public:
    


    SVMTYPE* svm()
          {
            return _svm;
          }
    
    void setProgressReporter( ProgressReporter* pr)
          {
            _pr = pr;
            _svm->setProgressReporter( pr);
          }
    

    /*======================================================================*/
    /*! 
     *   set the training data. the CrossValidator only stores a
     *   pointer to it, so you are responsible that the training data
     *   keeps avaiblibe during further calls, and is deleted at the
     *   end of the program
     *
     *   \param problem  pointer to the training data
     *
     */
    /*======================================================================*/
    void setTrainingData( const PROBLEM* problem)
          {
            _problem = problem;
          }

    /*======================================================================*/
    /*! 
     *   get pointer to training data, that was set with setTrainingData().
     *
     *   \return pointer to training data or 0, if no training data
     *           was specified yet  
     */
    /*======================================================================*/
    const PROBLEM* trainingData() const
          {
            return _problem;
          }
    

    /*======================================================================*/
    /*! 
     *   call updateKernelCache() of selected svm with given problem.
     *   You are responsible to call this before
     *   preprocessTrainingData(), every time when kernel parameters
     *   or dataset was chenged. (usually every time before you call
     *   preprocessTrainingData())  
     *
     *   \param 
     *
     *   \exception 
     *
     *   \return
     */
    /*======================================================================*/
    void updateKernelCache()
          {
            SVM_ASSERT( _problem != 0);
            _svm->updateKernelCache( _problem->FV_begin(),
                                     _problem->FV_end(),
                                     DereferencingAccessor());
          }
    
    /*======================================================================*/
    /*! 
     *   call clearKernelCache() of selected svm.
     *
     *   \param 
     *
     *   \exception 
     *
     *   \return
     */
    /*======================================================================*/
    void clearKernelCache()
          {
            _svm->clearKernelCache();
          }
    


    /*======================================================================*/
    /*! 
     *   trains all two-class SVM's with the whole data set. This must
     *   be done _before_ calling doFullCV() of doPartialCV(). If
     *   Kernel_MATRIX is used, you must call updateKernelCache() before
     *
     *
     */
    /*======================================================================*/
    void preprocessTrainingData()
          {
            SVM_ASSERT( _problem != 0);
            
            if( _pr != 0)
            {
              _pr->reportProgress( TASK_LEVEL_CROSS_VAL, 
                                   "preprocess training data", 0, "");
            }

            _svm->train( *_problem, _fullModel);

            if( _pr != 0)
            {
              _pr->reportProgress( TASK_LEVEL_CROSS_VAL, 
                                   "preprocess training data", 1.0, "");
            }
          }
    
    /*======================================================================*/
    /*! 
     *   do a full cross validation. Just calls doPartialCV() for each
     *   subset index and collects the results. See there for
     *   the preconditions
     *
     *   \param subsetIndexByUID tells for each feature vector
     *                           (identified by its uniqueID) to which
     *                           subset it belongs. If the uniqueIDs
     *                           of your feature vectors are
     *                           consecutive numbers from
     *                           0...nFeatureVectors-1, then you can
     *                           create this array with
     *                           generateSortedSubsets() or
     *                           generateShuffledSubsets() 
     *
     *   \param predictedClassLabelByUID (output) stores the
     *                           predicted class label for each
     *                           feature vector. Will be resized to
     *                           same size as subsetIndexByUID
     *
     *   \return total number of correct classifications
     */
    /*======================================================================*/
    int doFullCV( const std::vector<int>& subsetIndexByUID,
                  std::vector<double>& predictedClassLabelByUID);

    
    /*======================================================================*/
    /*! 
     *   Do one part of a cross validation. The training data must
     *   already be given with setTrainingData() and processed with
     *   preprocessTrainingData(). 
     *   The segmentation into the subsets 0...n must be given in
     *   subsetIndexByUID, where the membership for each feature
     *   vector is defined.
     *   To compare, wether the predicted Label is equal to the
     *   original label, the classificationDelta is used (you can
     *   change it with setClassificationDelta()) 
     *
     *   this method may be called from multiple threads to speed up
     *   the calculation 
     *
     *   \param subsetIndex      index of the subset, that shall be
     *                           left out for training and be used for
     *                           classification 
     *
     *   \param subsetIndexByUID tells for each feature vector
     *                           (identified by its uniqueID) to wich
     *                           subset it belongs. If the uniqueIDs
     *                           of your feature vectors are
     *                           consecutive numbers from
     *                           0...nFeatureVectors-1, then you can
     *                           create this array with
     *                           generateSortedSubsets() or
     *                           generateShuffledSubsets() 
     *
     *   \param predictedClassLabelByUID (output) stores the
     *                           predicted class label for each
     *                           feature vector in test-subset. Must
     *                           have same size as subsetIndexByUID.
     *
     *   \param partialModel     pointer to existing model object: Will
     *                           be passed to
     *                           _svm->retrainWithLeftOutVectors() or
     *                           0: allocate tempoarary model for this
     *                           purpose  
     *
     *   \return total number of correct classifications
     */
    /*======================================================================*/
    int doPartialCV( int subsetIndex, 
                     const std::vector<int>& subsetIndexByUID,
                     std::vector<double>& predictedClassLabelByUID,
                     ModelType* partialModel = 0);


    const ModelType& fullModel() const
          {
            return _fullModel;
          }

    void setClassificationDelta( double d)
          {
            _classificationDelta = d;
          }
    
    double classificationDelta() const
          {
            return _classificationDelta;
          }
    

    void setStoreClassificationDetailsFlag( bool f)
          {
            _storeClassificationDetailsFlag = f;
          }
    
    bool getStoreClassificationDetailsFlag() const
          {
            return _storeClassificationDetailsFlag;
          }
    

    /*======================================================================*/
    /*! 
     *   save cross validation statistics. 
     *
     *   \param statistics       structured data, where to store the
     *                           statistics
     *   \param detailLevel      0: nothing, 
     *                           1: training statistics
     *                              from last call to doFullCV()
     */
    /*======================================================================*/
    template< typename STDATA>
    void saveStatistics( STDATA& statistics,
                         int detailLevel = 1)
          {
            if( detailLevel >= 1)
            {
              statistics.setValue( "sum_nFV", _sum_nFV);
              statistics.setValue( "sum_nSV", _sum_nSV);
              statistics.setValue( "sum_nBSV", _sum_nBSV);
              statistics.setValue( "nSV_per_nFV", double(_sum_nSV) / _sum_nFV);
              statistics.setValue( "nBSV_per_nSV", double( _sum_nBSV) / _sum_nSV);
            }
          }
    
    /*======================================================================*/
    /*! 
     *   get classification details for each uid from last
     *   full CV or parital CV. Use
     *   setStoreClassificationDetailsFlag() before
     *   preprocessTrainingData() to enable storage of these infos
     *   during cross validation 
     *
     *   \return vector with classification details for each UID
     */
    /*======================================================================*/
    const std::vector< StDataASCII>& classificationDetailsByUID() const
          {
            return _classificationDetailsByUID;
          }
    


    template<typename STDATA>
    void loadParameters( STDATA& stData)
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);
            
            _svm->loadParameters(stData);
            
          }
                                   
    
    template<typename STDATA>
    void saveParameters( STDATA& stData)
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);
            
            _svm->saveParameters(stData);
            
          }
                                   
    


    
  private:
    SVMTYPE*          _svm;
    bool              _owningSVM;
    const PROBLEM*    _problem;
    ModelType         _fullModel;
    double            _classificationDelta;
    ProgressReporter* _pr;
    unsigned int      _sum_nSV;
    unsigned int      _sum_nFV;
    unsigned int      _sum_nBSV;
    bool              _storeClassificationDetailsFlag;
    std::vector< StDataASCII > _classificationDetailsByUID;
    
  };
}


#include "CrossValidator.icc"

#endif
