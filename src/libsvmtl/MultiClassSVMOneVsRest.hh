/**************************************************************************
 *
 * Copyright (C) 2004-2015 Olaf Ronneberger, Florian Pigorsch, Jörg Mechnich,
 *                         Thorsten Falk
 *
 *        Image Analysis Lab, University of Freiburg, Germany
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 **************************************************************************/

/**************************************************************************
**       Title: multi class svm using "one versus rest" technique
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
**  Revision 1.7  2005/10/26 07:27:58  ronneber
**  - added saveClassificationDetails()
**  - corrected some comments where "decision value" was called "alpha"
**
**  Revision 1.6  2005/03/29 18:01:29  ronneber
**  - replaced updateCacheFlag, etc with updateKernelCache() and
**    clearKernelCache() methods
**
**  Revision 1.5  2004/09/13 10:04:04  ronneber
**  - documentation update
**
**  Revision 1.4  2004/09/08 14:32:53  ronneber
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
**  Revision 1.8  2003/10/01 09:26:49  ronneber
**  - added some missing #include's
**
**  Revision 1.7  2003/05/19 11:04:38  ronneber
**  - converted from MapTools to ParamMapWrapper
**  - added new train() method where access to the feature vectors
**    container is done via a custom functor. This means, you now can give
**    your training vectors, e.g.,  as std::vector<FV> or std::vector<FV*>
**    or somethin completely different. You just have to pass an
**    appropriate Accessor to make an FV* from an iterator
**
**  Revision 1.6  2002/09/05 13:08:27  pigorsch
**  -modified to use new MapTools
**
**  Revision 1.5  2002/07/09 06:41:41  ronneber
**  added progress reporter for OneVsRest
**
**  Revision 1.4  2002/07/08 13:46:50  ronneber
**  - added copySVCoef... methods for OneVsRest SVM
**
**  Revision 1.3  2002/05/10 11:33:40  ronneber
**  - added default Constructor and twoClassSVM() method
**
**  Revision 1.2  2002/05/10 11:07:03  ronneber
**  - removed FV template for all public classes, because Feature Vector type
**    can be extracted automatically from passed iterators or other
**    parameters -- this makes the public interface much more intuitive
**
**  Revision 1.1  2002/05/08 10:37:31  ronneber
**  intitial revision
**
**
**
**
**************************************************************************/

#ifndef MULTICLASSSVMONEVSREST_HH
#define MULTICLASSSVMONEVSREST_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

// std includes
#include <map>
#include <string>

// libsvmtl includes

#include "ProgressReporter.hh"
#include "Model_MC_OneVsRest.hh"
#include "GroupedTrainingData.hh"
#include "DirectAccessor.hh"
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
   *  \class MultiClassSVMOneVsRest
   *  \ingroup multiclass
   *  \brief The MultiClassSVMOneVsRest class provides a one vs. rest
   *         multi class algorithm 
   *
   *  \param SVM a two-class-svm
   */
  /*======================================================================*/
  template< typename SVM>
  class MultiClassSVMOneVsRest
  {
    CHECK_CLASS_TEMPLATE1( svt_check::RequireLoadSaveParameters<SVM>)
    CHECK_CLASS_TEMPLATE2( svt_check::RequireSetProgressReporter<SVM>)
  public:

    template< typename FV>
    struct Traits
    {
      typedef Model_MC_OneVsRest<typename SVM::template Traits<FV>::ModelType> ModelType;
    };
    
    typedef std::vector<double> DetailedResultType;
    

    /*---------------------------------------------------------------------
     *  Dummy result vector, if no detailed results are requested
     *---------------------------------------------------------------------*/
    class DummyResultVector
    {
      double dummy;
    public:
      void resize( unsigned int)
            {}
      
      double& operator[]( int)
            {
              return dummy;
            }
    };

    /*---------------------------------------------------------------------
     *  struct to create sorted results list
     *---------------------------------------------------------------------*/
    struct DecisionValueAndLabel
    {
      double decisionValue;
      double label;
      bool operator<( const DecisionValueAndLabel& rhs) const
            {
              return (decisionValue > rhs.decisionValue);
            }
    };
    
      




    /*======================================================================*/
    /*! 
     *   Create a multi class SVM basing on the given "two-class-SVM",
     *   using the One versus Rest algorithm. The two class SVM may be
     *   either one of svt::TwoClassSVMc, or svt::TwoClassSVMnu or
     *   your own SVM implementation, which fulfills the following
     *   interface:
     *           SVM::train( ITER fvs_begin, ITER fvs_end, MODEL& model);
     *           ... \todo complete this
     *
     *   \param svm   Two-Class Support vector maschine 
     *
     *   \exception 
     *
     *   \return
     */
    /*======================================================================*/
    MultiClassSVMOneVsRest( const SVM& svm)
            :_twoClassSVM( svm),
             _pr(0)
          {     
          }

  
    MultiClassSVMOneVsRest()
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
     *   (description)
     *
     *   \param 
     *
     *   \exception 
     *
     *   \return
     */
    /*======================================================================*/
    const SVM& twoClassSVM() const
          {
            return _twoClassSVM;
          }
    
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
     *   \param model   (output) model containing a vector
     *                  of trained of TwoClassModels. It stores
     *                  only pointers to the feature vectors, but
     *                  you still own them!
     */
    /*====================================================================*/
    template<typename FV>
    void train( const GroupedTrainingData<FV>& trainData,
                typename Traits<FV>::ModelType& model) const;
  
    
    /*====================================================================*/
    /*! 
     *   train SVM with given Feature Vectors.  (general SVM interface)
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
     *   \param[out] resultVector results (decision values) from the two-class
     *                     classifications. resultVector will
     *                     be resized properly. The type could be the
     *                     typedef'ed DetailedResultType or your own
     *                     resultVector class.
     *
     *   \return  classIndex of winning class
     */
    /*======================================================================*/
    template<  typename FV, typename ResultVector>
    unsigned int predictClassIndex( 
        const FV& testObject, 
        const typename Traits<FV>::ModelType& model, 
        ResultVector& resultVector) const;
    
    
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
            DummyResultVector dummy;
            return predictClassIndex( testObject, model, dummy);
          }

    /*======================================================================*/
    /*! 
     *   classify the given testObject using the model
     *
     *   \param testObject the test object
     *   \param model      the model
     *   \param[out] resultVector results (decision values) from the two-class
     *                     classifications. resultVector will
     *                     be resized properly.  The type could be the
     *                     typedef'ed DetailedResultType or your own
     *                     resultVector class.
     *
     *   \return label of winning class
     */
    /*======================================================================*/
    template<  typename FV, typename ResultVector>
    double classify(  const FV& testObject, 
                      const typename Traits<FV>::ModelType& model, 
                      ResultVector& resultVector) const
          {
            return model.classIndexToLabel( 
                predictClassIndex( testObject, model, resultVector));
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
    template<  typename FV>
    double classify( const FV& testObject, 
                  const typename Traits<FV>::ModelType& model) const
          {
            DummyResultVector dummy;
            return classify( testObject, model, dummy);
          }

    
    template<typename STDATA>
    void loadParameters( STDATA& stData)
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);
            _twoClassSVM.loadParameters( stData);
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
     *   save classification details. Here the ordered labels as
     *   "labels" and the according decision values as "dec_values"  
     *
     *   \param model        the used model
     *   \param resultVector results from the classification
     *   \param stData       structured data, where to store the details
     */
    /*======================================================================*/
    template< typename ModelType, typename STDATA>
    void saveClassificationDetails( const ModelType&  model,
                                    const DetailedResultType& resultVector, 
                                    STDATA& stData) const
          {
            // create list of decision values with labels
            std::vector< DecisionValueAndLabel > 
                decValueLabelList( resultVector.size());
            
            // copy decision values and labels to that list
            for( size_t i = 0; i < resultVector.size(); ++i)
            {
              decValueLabelList[i].decisionValue = resultVector[i];
              decValueLabelList[i].label = model.classIndexToLabel(
                  static_cast<int>(i));
            }
            
            // sort the list
            std::sort( decValueLabelList.begin(), decValueLabelList.end());
            
            // store it to structured data
            std::vector<double> decValueList( decValueLabelList.size());
            std::vector<double> labelList( decValueLabelList.size());
            for( size_t i = 0; i < decValueLabelList.size(); ++i)
            {
              decValueList[i] = decValueLabelList[i].decisionValue;
              labelList[i] = decValueLabelList[i].label;
            }

            stData.setArray( "labels", labelList.begin(), labelList.size());
            stData.setArray( "dec_values", 
                             decValueList.begin(), decValueList.size());
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
            return "one_vs_rest";
          }
    
    static const char* description()
          {
            return "multi-class SVM by using the One versus Rest approach";
          }
      
  private:
    SVM _twoClassSVM;
    ProgressReporter* _pr;

  };
  
#include "MultiClassSVMOneVsRest.icc"
}

#endif
