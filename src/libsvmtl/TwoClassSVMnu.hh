/**************************************************************************
**       Title: two class NU-SVM
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
**  Revision 1.3  2004/09/08 14:40:39  ronneber
**  - changed key for two class svm selection from "svm_type" to "two_class_type"
**  - adapted to new ParamInfo class
**
**  Revision 1.2  2004/09/01 14:43:36  ronneber
**  changed IterToPointerTraits stuff to
**  DirectAccessor and DereferencingAccessor, to make code more
**  intuitive understandable
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**  Revision 1.8  2003/05/19 11:29:55  ronneber
**  - converted from MapTools to ParamMapWrapper
**
**  Revision 1.7  2002/09/05 13:10:54  pigorsch
**  - modified to use new MapTools
**
**  Revision 1.6  2002/06/07 12:23:15  ronneber
**  - clean up of source code
**
**  - made ModelType in train() an own template parameter to make
**    it more flexible (e.g. when using a slightly different but
**    compatible Feature Vector class)
**
**  - added two additional interfaces for train() method
**
**  - moved functionality of train_one() and solve_nu_svc() method
**    into low-level train() method
**
**  Revision 1.5  2002/06/05 17:18:31  mechnich
**  Made minor corrections for compilation under Win32
**
**  Revision 1.4  2002/05/21 18:31:31  ronneber
**  - now all parameters can be queried
**
**  Revision 1.3  2002/05/10 11:07:03  ronneber
**  - removed FV template for all public classes, because Feature Vector type
**    can be extracted automatically from passed iterators or other
**    parameters -- this makes the public interface much more intuitive
**
**  Revision 1.2  2002/05/06 13:42:25  ronneber
**  - removed Parameters struct
**  - added setNu()
**
**  Revision 1.1  2002/03/26 12:44:02  ronneber
**  restructured for autoconf
**
**  Revision 1.1  2002/03/13 14:08:58  pigorsch
**  * initial revision
**
**
**
**************************************************************************/

#ifndef TWOCLASSSVMNU_HH
#define TWOCLASSSVMNU_HH

// std includes
#include <map>
#include <string>

// libsvmtl includes

#include "TwoClassSVM.hh"
#include "Solver_NU.hh"
#include "SVC_Q.hh"
#include "DirectAccessor.hh"

// requirements of template parameters
#include "svt_check/RequireKernel_k_function.hh"
#include "svt_check/RequireStData.hh"


namespace svt
{
/*======================================================================*/
/*!
 *  \class TwoClassSVMnu TwoClassSVMnu.hh
 *  \brief The TwoClassSVMnu class (is/provides/specifies/contains)
 *
 *  (description)
 */
/*======================================================================*/
  template< typename KF>
  class TwoClassSVMnu: public TwoClassSVM<KF>
  {
  public:

    /*====================================================================*/
    /*!
     *   (description)
     */
    /*====================================================================*/
    TwoClassSVMnu();


    /*====================================================================*/
    /*!
     *   (description)
     *
     *   \param
     */
    /*====================================================================*/
    TwoClassSVMnu(const KF& kernel);


    /*====================================================================*/
    /*!
     *   (description)
     */
    /*====================================================================*/
    ~TwoClassSVMnu();


    /*====================================================================*/
    /*! 
     *   train SVM with given Feature Vectors.  (raw interface)
     *
     *   this is the basic method, all others will call this one
     *
     *   \param prob    Structure with pointers to feature-vectors
     *   \param model   (output) model containing pointers to feature
     *                  vectors, that became support vectors, alphas
     *                  and rho   
     */
    /*====================================================================*/
    template<typename FV>
    void train( const SVM_Problem<FV>& problem,
                Model<FV>& model) const;


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
     *                  and rho   
     */
    /*====================================================================*/
    template<typename ForwardIter>
    void train( ForwardIter FV_begin, const ForwardIter& FV_end,
                svt::Model<typename std::iterator_traits<ForwardIter>::value_type>& model) const
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
     *                  and rho   
     *   \param accessor  functor, which accesses the feature vector
     *                    in the container
     */
    /*====================================================================*/
    template<typename ForwardIter, typename Accessor>
    void train( ForwardIter FV_begin, const ForwardIter& FV_end,
                svt::Model<typename Accessor::template Traits<ForwardIter>::value_type>& model,
                Accessor accessor) const;


    /*======================================================================*/
    /*!
     *   set the parameter nu of nu-SVC. Default: 0.5
     *
     *   \param nu new value for nu
     *
     */
    /*======================================================================*/
    void setNu( double nu)
          {
            _nu = nu;
          }

    double nu() const
          {
            return _nu;
          }


    /*======================================================================*/
    /*!
     *   Load and save parameter nu from map
     */
    /*======================================================================*/
    template<typename STDATA>
    void loadParameters( STDATA& stData)
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);

            TwoClassSVM<KF>::loadParameters( stData);
            stData.getValue( "nu", _nu);
          }
    
    template<typename STDATA>
    void saveParameters( STDATA& stData) const
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);

            TwoClassSVM<KF>::saveParameters( stData);
            stData.setValue( "two_class_type", name());
            stData.setValue( "nu", _nu);
          }

    static const char* name()
          {
            return "nu_svc";
          }

    static const char* description()
          {
            return "Two class SVM using nu-SVC algorithm for training";
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
    static void getParamInfos( std::vector<ParamInfo>& p)
          {
            p.push_back(
                ParamInfo( "nu", "n", "value",
                           "parameter nu in nu-SVC. (default 0.5)"));
            TwoClassSVM<KF>::getParamInfos( p);
          }

  protected:
    template<typename FV>
    void solve_nu_svc( const SVM_Problem<FV>* prob,
                       double *alpha, 
                       SolutionInfo* si,
                       Model<FV>& model) const;
    


  private:
    double _nu;   // Parameter 'nu' for nu-SVM
  };

}

#include "TwoClassSVMnu.icc"


#endif
