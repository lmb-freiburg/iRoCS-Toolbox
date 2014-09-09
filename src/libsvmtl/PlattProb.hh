/**************************************************************************
**       Title: Platts Probability estimates
**    $RCSfile$
**   $Revision: 604 $$Name$
**       $Date: 2005-02-11 13:26:53 +0100 (Fri, 11 Feb 2005) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.1  2005/02/11 12:26:45  ronneber
**  - initial revision
**
**
**
**************************************************************************/

#ifndef PLATTPROB_HH
#define PLATTPROB_HH

#include <PlattProbModel.hh>
#include <GroupedTrainingData.hh>

namespace svt
{
  template< typename WrappedSVM>
  class PlattProb
  {
    
  public:

    template< typename FV>
    struct Traits
    {
      typedef svt::PlattProbModel< typename WrappedSVM::Traits<FV>::ModelType > ModelType;
    };


    PlattProb( WrappedSVM* svm = 0)
            :_svm( svm),
             _owningSVM( false)
          {
            if( _svm == 0)
            {
              _svm = new WrappedSVM;
              _owningSVM = true;
            }
          }
    
    ~PlattProb()
          {
            if( _owningSVM)
            {
              delete _svm;
            }
          }
    
  private:
    // forbid copying
    PlattProb( const PlattProb<WrappedSVM>& orig) {}
    void operator=( const PlattProb<WrappedSVM>& orig) {}
  public:
    
    
     static  std::string  name()
          {
            return std::string( "plattprob_") + WrappedSVM::name();
          }

    static std::string description()
          {
            return std::string( "Platts Probability estimates for ") 
                + WrappedSVM::description();
          }

    
    /*====================================================================*/
    /*! 
     *   train SVM with given Feature Vectors.  
     *
     *   this is the basic method, all others will call this one
     *
     *   \param trainData  training data
     *   \param model   (output) model containing pointers to feature
     *                  vectors, that became support vectors, alphas
     *                  rho and parameters for sigmoid function
     */
    /*====================================================================*/
    template<typename FV>
    void train( const GroupedTrainingData<FV>& trainData,
                typename Traits<FV>::ModelType& model) const;

    /*====================================================================*/
    /*!
     *   train (STL-like interface)
     *   Your feature vector class must provide a method double
     *   getLabel(), which returns +1 or -1. 
     *   The ModelType is svt::Model<FV>. Positive and
     *   negative labled feature vectors may be in any order in the
     *   given Container.
     *
     *   \param FV_begin  iterator pointing to first element of the
     *                    container with the feature vectors
     *   \param FV_end    iterator pointing one past the last element of the
     *                    container with the feature vectors
     *   \param model   (output) model containing pointers to feature
     *                  vectors, that became support vectors, alphas
     *                  and rho and parameters for sigmoid function
     */
    /*====================================================================*/
    template<typename ForwardIter>
    void train( ForwardIter FV_begin, const ForwardIter& FV_end,
                typename Traits<typename std::iterator_traits<ForwardIter>::value_type>::ModelType& model) const
          {
            train( FV_begin, FV_end, model, DirectAccessor());
          }


    /*====================================================================*/
    /*!
     *   train (STL-like interface with additional Accessor)
     *
     *   same as train(), but you can specify an Accessor
     *   for accessing the elements of your container. This accessor
     *   must return a reference to the corresponding feature
     *   vector. Use DirectAccessor for containers like
     *   std::vector<FV> and DereferencingAccessor for containers like
     *   std::vector<FV*> 
     *
     *   \param FV_begin  iterator pointing to first element of the
     *                    container with the feature vectors
     *   \param FV_end    iterator pointing one past the last element of the
     *                    container with the feature vectors
     *   \param model   (output) model containing pointers to feature
     *                  vectors, that became support vectors, alphas
     *                  and rho and parameters for sigmoid function
     *   \param accessor functor, which converts an iterator
     *                    of your container into a pointer to a
     *                    feature vector 
     */
    /*====================================================================*/
    template<typename ForwardIter, typename Accessor>
    void train( ForwardIter FV_begin, const ForwardIter& FV_end,
                typename Traits<typename Accessor::template Traits<ForwardIter>::value_type>::ModelType& model,
                Accessor accessor) const;

    /*====================================================================*/
    /*! 
     *   standard classification without caching the kernel
     *   evaluations.
     *
     *   \param testObject  feature vector of the test object
     *   \param model       model containing Support Vectors, alphas and
     *                      rho and parameters for sigmoid function
     *
     *   \return probability estimate
     */
    /*====================================================================*/
    template<typename FV>
    double classify( const FV& testObject, 
                     const typename Traits<FV>::ModelType&  model) const;


  private:
    WrappedSVM* _svm;
    bool _owningSVM;
    
  };
  
}

#include "PlattProb.icc"

#endif
