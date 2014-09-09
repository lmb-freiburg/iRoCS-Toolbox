/**************************************************************************
**       Title: multi class svm using "one versus one" technique
**    $RCSfile$
**   $Revision: 2825 $$Name$
**       $Date: 2009-09-15 17:04:15 +0200 (Tue, 15 Sep 2009) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.7  2005/10/26 07:26:37  ronneber
**  - added saveClassificationDetails()
**  - corrected some comments where "decision value" was called "alpha"
**
**  Revision 1.6  2005/03/29 18:00:31  ronneber
**  - replaced updateCacheFlag, etc with updateKernelCache() and
**    clearKernelCache() methods
**
**  Revision 1.5  2004/09/13 10:04:04  ronneber
**  - documentation update
**
**  Revision 1.4  2004/09/08 14:32:31  ronneber
**  - adapted to new ParamInfo class
**
**  Revision 1.3  2004/09/03 07:13:36  ronneber
**  - adapted to new updateCache() interface of Kernels
**
**  Revision 1.2  2004/09/01 14:43:36  ronneber
**  changed IterToPointerTraits stuff to
**  DirectAccessor and DereferencingAccessor, to make code more
**  intuitive understandable
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**  Revision 1.13  2003/05/19 11:02:52  ronneber
**  - converted from MapTools to ParamMapWrapper
**  - added new train() method where access to the feature vectors
**    container is done via a custom functor. This means, you now can give
**    your training vectors, e.g.,  as std::vector<FV> or std::vector<FV*>
**    or somethin completely different. You just have to pass an
**    appropriate Accessor to make an FV* from an iterator
**  - added verboseLevel
**
**  Revision 1.12  2002/09/05 13:08:27  pigorsch
**  -modified to use new MapTools
**
**  Revision 1.11  2002/06/07 07:40:05  ronneber
**  - added missing #include <vector>
**
**  Revision 1.10  2002/05/23 19:04:32  ronneber
**  - #included  missing SupportVector
**
**  Revision 1.9  2002/05/23 15:24:10  ronneber
**  - added copySVCoefsToModelMatrix()
**
**  Revision 1.8  2002/05/22 16:39:06  ronneber
**  - added progress reporting capabilities
**
**  Revision 1.7  2002/05/14 10:12:25  ronneber
**  - adapted to new TriangularMatrix (now using upper triangular matrix)
**
**  Revision 1.6  2002/05/13 16:24:17  ronneber
**  - added copySVCoefsToFeatureVectors(). This function can be used to
**    store support vectors in an original libsvm compatible format
**
**  Revision 1.5  2002/05/10 11:33:40  ronneber
**  - added default Constructor and twoClassSVM() method
**
**  Revision 1.4  2002/05/10 11:07:03  ronneber
**  - removed FV template for all public classes, because Feature Vector type
**    can be extracted automatically from passed iterators or other
**    parameters -- this makes the public interface much more intuitive
**
**  Revision 1.3  2002/05/08 10:36:11  ronneber
**  - added some debugging stuff
**
**  Revision 1.2  2002/05/06 12:23:37  ronneber
**  - first functional version
**
**  Revision 1.1  2002/05/02 15:43:55  ronneber
**  initial revision
**
**
**
**************************************************************************/

#ifndef MULTICLASSSVMONEVSONE_HH
#define MULTICLASSSVMONEVSONE_HH

// std includes
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

// libsvmtl includes

#include "TriangularMatrix.hh"
#include "ProgressReporter.hh"
#include "Model_MC_OneVsOne.hh"
#include "GroupedTrainingData.hh"
#include "DereferencingAccessor.hh"

// requirements of template parameters
#include "svt_check/RequireStData.hh"
#include "svt_check/RequireLoadSaveParameters.hh"
#include "svt_check/RequireSetProgressReporter.hh"
#include "svt_check/RequireSVMKernelUpdateCache.hh"
#include "svt_check/RequireSVMBasicTraining.hh"
#include "svt_check/RequireSVMBasicClassification.hh"

namespace svt
{
  /*======================================================================*/
  /*!
   *  \class MultiClassSVMOneVsOne
   *  \ingroup multiclass
   *  \brief The MultiClassSVMOneVsOne class provides a one vs. one
   *  multi class algorithm.
   *
   *   Create a multi class SVM basing on the given "two-class-SVM",
   *   using the One versus One algorithm. 
   * 
   *   - \b SVM a two-class-svm (see \ref twoclass for available
   *             classes in LIBSVMTL) SVM must fulfill the interfaces
   *             (FV is a member template parameter for several
   *             methods)  
   *             - svt_check::RequireLoadSaveParameters< SVM >
   *             - svt_check::RequireSetProgressReporter< SVM >
   *             - svt_check::RequireMinimalTCModel< SVM::Traits<FV>::ModelType >
   *             - svt_check::RequireSVMKernelUpdateCache< SVM, FV >
   *             - svt_check::RequireSVMBasicTraining< SVM, FV >
   *             
   */
  /*======================================================================*/
  template< typename SVM>
  class MultiClassSVMOneVsOne
  {
    CHECK_CLASS_TEMPLATE1( svt_check::RequireLoadSaveParameters<SVM>)
    CHECK_CLASS_TEMPLATE2( svt_check::RequireSetProgressReporter<SVM>)
    
        

  public:

    template< typename FV>
    struct Traits
    {
      typedef Model_MC_OneVsOne<typename SVM::template Traits<FV>::ModelType> ModelType;
    };
    
    typedef TriangularMatrix<double> DetailedResultType;
    

    /*---------------------------------------------------------------------
     *  Dummy result matrix, if no detailed results are requested
     *---------------------------------------------------------------------*/
    class DummyResultMatrix
    {
      double dummy;
    public:
      void resizeWidth( unsigned int)
            {}
      
      double& operator()( int, int)
            {
              return dummy;
            }
    };
    


    /*======================================================================*/
    /*! 
     *   Create a multi class SVM basing on the given svm object
     *
     *   \param svm  Two-Class Support vector maschine, the object is
     *               copied to internal variable
     */
    /*======================================================================*/
    MultiClassSVMOneVsOne( const SVM& svm)
            :_twoClassSVM( svm),
             _pr(0)
          {
          }
  

    /*======================================================================*/
    /*! 
     *   Create a multi class SVM. with Default Constructot of
     *   template Parameter SVM
     *
     */
    /*======================================================================*/
    MultiClassSVMOneVsOne()
            :_pr(0)
          { 
          }
    
    
    /*======================================================================*/
    /*! 
     *   set a progress reporter object. Every time any progress is
     *   made in the training or classification, the reportProgress()
     *   method of this object is callled.  
     *
     *   \param pr  pointer to an object with a child class of
     *              ProgressReporter or 0, if you don't want any more
     *              progress reporting. You are responsible, that your
     *              ProgressReporter object is not deleted during operation of
     *              MultiClassSVMOneVsOne  
     *
     */
    /*======================================================================*/
    void setProgressReporter( ProgressReporter* pr)
          {
            _pr = pr;
            _twoClassSVM.setProgressReporter( pr);
          }

   

    /*======================================================================*/
    /*! 
     *   \return const reference to internal two-class SVM
     */
    /*======================================================================*/
    const SVM& twoClassSVM() const
          {
            return _twoClassSVM;
          }
    
    /*======================================================================*/
    /*! 
     *   \return nonconst reference to internal two-class SVM
     */
    /*======================================================================*/
    SVM& twoClassSVM()
          {
            return _twoClassSVM;
          }


    /*======================================================================*/
    /*! 
     *   call the updateKernelCache() method of selected two-class svm
     *
     *   \param 
     *
     *   \exception 
     *
     *   \return
     */
    /*======================================================================*/
    template< typename ForwardIter, typename Accessor>
    void updateKernelCache( const ForwardIter& fvBegin,  
                            const ForwardIter& fvEnd,
                            Accessor accessor) const
          {
            _twoClassSVM.updateKernelCache( fvBegin, fvEnd, accessor);
          }


    /*======================================================================*/
    /*! 
     *   call the clearKernelCache() method of selected two-class svm
     *
     *   \param 
     *
     *   \exception 
     *
     *   \return
     */
    /*======================================================================*/
    void clearKernelCache() const
          {
            _twoClassSVM.clearKernelCache();
          }


    /*====================================================================*/
    /*! 
     *   train SVM with given Feature Vectors.  (raw interface)
     *
     *   this is the basic method, all others will call this one
     *
     *   \param trainData Structure with pointers to feature-vectors
     *   \param model   (output) model containing a triangular
     *                  matrix of trained of TwoClassModels. It stores
     *                  only pointers to the feature vectors, but
     *                  you still own them!
     *
     *   <b>Template Parameters:</b>
     *   - \b FV feature vector class. Must fulfill the interfaces  
     *             - svt_check::RequireFeatureVectorUniqueID< FV >
     *             - svt_check::RequireMinimalTCModel<
     *               SVM::Traits<FV>::ModelType >
     *             - svt_check::RequireSVMKernelUpdateCache< SVM, FV>
     *             - svt_check::RequireSVMBasicTraining< SVM, FV>
     */
    /*====================================================================*/
    template<typename FV>
    void train( const GroupedTrainingData<FV>& trainData,
                typename Traits<FV>::ModelType& model) const;
  


     /*====================================================================*/
    /*! 
     *   train SVM with given Feature Vectors. (general SVM interface)
     *
     *   \param problem  Structure with pointers to feature-vectors and y's
     *   \param model   (output) model containing a triangular
     *                  matrix of trained of TwoClassModels. It stores
     *                  only pointers to the feature vectors, but
     *                  you still own them!
     */
    /*====================================================================*/
    template<typename FV>
    void train( const SVM_Problem<FV>& problem,
                typename Traits<FV>::ModelType& model) const
          {
            GroupedTrainingData<FV> trainData( problem);
            train( trainData, model);
          }
    
            
  


   /*======================================================================*/
    /*! 
     *   train the Multi Class SVM with the given feature vectors. 
     *   The FeatureVector-Container must contain the Feature vectors
     *   directly, e.g., std::vector<MyFeatureVec>.
     *   
     *   \param FV_begin  iterator to the first element of your
     *                    FeatureVector-Container 
     *   \param FV_end    iterator to one past the last element of your
     *                    FeatureVector-Container 
     *   \param model     (output) model containing a triangular
     *                    matrix of trained of TwoClassModels. It stores
     *                    only pointers to the feature vectors, but
     *                    you still own them!
     *   
     *   \exception 
     *
     */
    /*======================================================================*/
    template< typename ForwardIter>
    void train( ForwardIter FV_begin,  
                const ForwardIter& FV_end,
                typename Traits<typename std::iterator_traits< ForwardIter>::value_type>::ModelType& model) const
          {
            train( FV_begin, FV_end, model, DirectAccessor());
          }
    

    /*======================================================================*/
    /*! 
     *   same as train(), but you can specify an Accessor
     *   for accessing the elements of your container. This accessor
     *   must return a reference to the corresponding feature
     *   vector. Use DirectAccessor for containers like
     *   std::vector<FV> and DereferencingAccessor for containers like
     *   std::vector<FV*> 
     *
     *   \param FVP_begin iterator to the first element of your
     *                    PointerToFeatureVector-Container 
     *   \param FVP_end   iterator to one past the last element of your
     *                    PointerToFeatureVector-Container 
     *   \param model     (output) model containing a triangular
     *                    matrix of trained of TwoClassModels. It stores
     *                    only pointers to the feature vectors, but
     *                    you still own them!
     *   \param accessor  functor, which accesses the feature vector
     *                    in the container
     *
     *   \exception 
     */
    /*======================================================================*/
    template< typename ForwardIter, typename Accessor>
    void train( ForwardIter FV_begin,  
                const ForwardIter& FV_end,
                typename Traits<typename Accessor::template Traits<ForwardIter>::value_type>::ModelType& model, 
                Accessor accessor) const
          {
            typedef typename Accessor::template Traits<ForwardIter>::value_type FV;
            GroupedTrainingData<FV> trainData( FV_begin, FV_end, 
                                               accessor);
            train( trainData, model);
          }



    
    /*======================================================================*/
    /*! 
     *   calls the retrainWithLeftOutVectors() for each two-class
     *   model, only if the model is affected by the left out vectors
     *
     *   \param trainData Structure with pointers to feature-vectors
     *                    must be identical to training data set that
     *                    was used to train the fullModel
     *   \param fullModel model that was trained using complete
     *                    training data set without leaving out
     *                    anything 
     *   \param leaveOutFlagsByUID leave out flag for each feature
     *                    vector, accessed via its uniqueID (true
     *                    means leave this vector out from training)
     *   \param resultingModel (output) model containing a triangular
     *                    matrix of trained of TwoClassModels. It stores
     *                    only pointers to the feature vectors, but
     *                    you still own them!
     *   
     */
    /*======================================================================*/
    template<typename FV>
    void retrainWithLeftOutVectors( 
        const GroupedTrainingData<FV>& trainData,
        const typename Traits<FV>::ModelType& fullModel,
        const std::vector<char>& leaveOutFlagsByUID,
        typename Traits<FV>::ModelType& resultingModel) const;
    

    /*======================================================================*/
    /*! 
     *   classify the given testObject using the model
     *
     *   \param testObject the test object
     *   \param model      the model
     *   \param[out] resultMatrix results (decision values) from the two-class
     *                     classifications. resultMatrix (should be
     *                     TriangularMatrix<double>) will be resized
     *                     properly  
     *
     *   \return  classIndex of winning class
     */
    /*======================================================================*/
    template<  typename FV, typename ResultMatrix>
    unsigned int predictClassIndex( 
        const FV& testObject, 
        const typename Traits<FV>::ModelType& model, 
        ResultMatrix& resultMatrix) const;
    

    /*======================================================================*/
    /*! 
     *   classify the given testObject using the model
     *
     *   \param testObject the test object
     *   \param model      the model
     *
     *   \return  classIndex of winning class
     */
    /*======================================================================*/
    template<  typename FV>
    unsigned int predictClassIndex( 
        const FV& testObject, 
        const typename Traits<FV>::ModelType& model) const
          {
            DummyResultMatrix dummy;
            return predictClassIndex( testObject, model, dummy);
          }
    
    
    /*======================================================================*/
    /*! 
     *   classify the given testObject using the models in Model matrix. 
     *
     *   \param testObject the test object
     *   \param model      the model
     *   \param[out] resultMatrix results (decision values) from the two-class
     *                     classifications. resultMatrix will be resized
     *                     properly. The type could be the
     *                     typedef'ed DetailedResultType or your own
     *                     matrix class.
     *
     *   \return  label of winning class
     */
    /*======================================================================*/
    template< typename FV, typename ResultMatrix>
    double classify( const FV& testObject, 
                     const typename Traits<FV>::ModelType& model, 
                     ResultMatrix& resultMatrix) const
          {
            return model.classIndexToLabel( 
                predictClassIndex( testObject, model, resultMatrix));
          }
    
    
    /*======================================================================*/
    /*! 
     *   classify the given testObject using the model
     *
     *   \param testObject the test object
     *   \param model      the model
     *
     *   \return  label of winning class
     */
    /*======================================================================*/
    template< typename FV>
    double classify( const FV& testObject, 
                  const typename Traits<FV>::ModelType& model) const
          {
            DummyResultMatrix dummy;
            return classify( testObject, model, dummy);
          }
    

    template<typename STDATA>
    void loadParameters( STDATA& stData)
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);
            
            _twoClassSVM.loadParameters(stData);
            
          }
    
    template<typename STDATA>
    void saveParameters( STDATA& stData) const
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);
            
            stData.setValue( "multi_class_type", name());
            _twoClassSVM.saveParameters( stData);
          }

    /*======================================================================*/
    /*! 
     *   save classification details. Here the ordered lables as
     *   "labels" and the according decision values as "dec_values"  
     *
     *   \param model        the used model
     *   \param resulMatrix  results from the classification
     *   \param stData       structured data, where to store the details
     */
    /*======================================================================*/
    template< typename ModelType, typename STDATA>
    void saveClassificationDetails( const ModelType&,
                                    const DetailedResultType&, 
                                    STDATA&) const
          {
            std::cerr << "warning: saving details from "
                "MultiClassSVMOneVsOne not implemented yet!\n";
          }

    /*======================================================================*/
    /*! 
     *   get information about the parameters, that are used in
     *   loadParameters() and saveParameters(). The Infos are appended
     *   to the passed array
     *
     *   \param p  array of ParamInfos. The Infos will be appended to
     *             this array
     */
    /*======================================================================*/
    static void getParamInfos( std::vector<ParamInfo>&)
          {
          }
   

    static const char* name()
          {
            return "one_vs_one";
          }
    
    static const char* description()
          {
            return "multi-class SVM by using the 'one versus one' approach";
          }
    
  private:
    SVM _twoClassSVM;
    ProgressReporter* _pr;
    
    
  };
  
#include "MultiClassSVMOneVsOne.icc"
}

#endif
