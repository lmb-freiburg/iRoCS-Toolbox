/**************************************************************************
**       Title: abstract two class SVM
**    $RCSfile$
**   $Revision: 766 $$Name$
**       $Date: 2005-10-26 09:42:23 +0200 (Wed, 26 Oct 2005) $
**   Copyright: LGPL $Author: ronneber $
** Description: 
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.4  2005/10/26 07:42:23  ronneber
**  - corrected some comments (confusion between alpha and decision value)
**  - added first version calcGradientCache() and
**    gradientOfDecisionFunction() for planned feature selection (not
**    functional yet)
**
**  Revision 1.3  2004/09/13 10:13:13  ronneber
**  - adapted to rename SVM -> SVMBase
**
**  Revision 1.2  2004/09/13 10:04:04  ronneber
**  - documentation update
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**  Revision 1.9  2003/05/19 11:25:28  ronneber
**  - converted from MapTools to ParamMapWrapper
**  - added verboseLevel
**
**  Revision 1.8  2002/09/05 13:10:54  pigorsch
**  - modified to use new MapTools
**
**  Revision 1.7  2002/06/07 11:37:04  ronneber
**  - made ModelType in classify() aa own template parameter to make
**    it more flexible (e.g. when using a slightly different but
**    compatible Feature Vector class)
**
**  Revision 1.6  2002/05/21 18:55:33  ronneber
**  - added const version of kernel()
**
**  Revision 1.5  2002/05/21 18:31:31  ronneber
**  - now all parameters can be queried
**
**  Revision 1.4  2002/05/13 12:49:07  ronneber
**  sync
**
**  Revision 1.3  2002/05/10 11:07:03  ronneber
**  - removed FV template for all public classes, because Feature Vector type
**    can be extracted automatically from passed iterators or other
**    parameters -- this makes the public interface much more intuitive
**
**  Revision 1.2  2002/05/06 13:30:51  ronneber
**  - removed Parameters struct
**  - added setTerminationEpsilon(), setCacheSizeMB(), setShrinkingFlag() instead
**  - added typedefs for kernel function and feature vector type
**    (for use with MultiClassSVM)
**
**  Revision 1.1  2002/03/26 12:44:02  ronneber
**  restructured for autoconf
**
**  Revision 1.2  2002/03/13 13:48:13  pigorsch
**  * this is now an abstract base class
**  * made parameter param of train const
**
**  Revision 1.1  2002/01/28 13:44:22  pigorsch
**  * initial revision
**
**
**
**
**************************************************************************/

#ifndef TWOCLASSSVM_HH
#define TWOCLASSSVM_HH

// std includes
#include <iostream>
#include <string>

// libsvmtl includes

#include "Model.hh"
#include "SVMError.hh"
#include "SVMBase.hh"

// requirements of template parameters
#include "svt_check/RequireKernel_k_function.hh"
#include "svt_check/RequireFeatureVectorUniqueID.hh"
#include "svt_check/RequireForwardIter.hh"

namespace svt
{
/*======================================================================*/
/*!
 *  \class TwoClassSVM TwoClassSVM.hh
 *  \brief The TwoClassSVM class is the basic class for TwoClassSVMc
 *  and TwoClassSVMnu and maybe some more later
 *
 *  (description)
 */
/*======================================================================*/
  template< typename KF>
  class TwoClassSVM : public SVMBase<KF>
  {
  public:

    typedef KF KF_type;
    template< typename FV>
    struct Traits
    {
      typedef svt::Model<FV> ModelType;
    };
    
    

    /*====================================================================*/
    /*! 
     *
     */
    /*====================================================================*/
    TwoClassSVM()
          {}
    


    /*====================================================================*/
    /*! 
     *   (description)
     *
     *   \param 
     */
    /*====================================================================*/
    TwoClassSVM(const KF& kernel)
            : SVMBase<KF>( kernel)
          {}
    


    /*====================================================================*/
    /*! 
     *   (description)
     */
    /*====================================================================*/
    ~TwoClassSVM()
          {}
    


    
    /*======================================================================*/
    /*! 
     *   caculcate the Classification Cache kernelResultByUID, which
     *   contains kernel evaluations for classification. This is 
     *   separated from the classify method, because
     *   multiclass-algorithms often need the evaluated kernels
     *   multiple times.
     *   To identify the requested support vectors in
     *   classifyWithCache() each given support vector must provide a
     *   uniqueID() method, that returns a unique index (unsigend int)
     *   between 0 and kernelResultByUID.size()
     *
     *   \param testObject   feature vector of the test object
     *   \param SV_begin     begin of container with pointers to
     *                       support vectors
     *   \param SV_end       end of container with pointers to support vectors
     *   \param kernelResultByUID (output) vector with kernel
     *                       evaluation results by uniqueID() of
     *                       feature vector. Must have already the
     *                       correct size!
     */
    /*======================================================================*/
    template< typename FV, typename ForwardIter>
    void calcClassificationCache(const FV& testObject, 
                                 const ForwardIter& SVP_begin,  
                                 const ForwardIter& SVP_end,
                                 std::vector<double>& kernelResultByUID) const;
    
    /*======================================================================*/
    /*! 
     *   classify testObject (that was passed to
     *   calcClassificationCache() before) using the given model.The
     *   kernel evaluations (kernelResulsByUID) must be computed
     *   before by calcClassificationCache.
     *
     *   \param model  model containing Support Vectors, alphas and
     *                 rho. Each Support Vector must provide a
     *                 uniqueID() method, to find the cached kernel
     *                 evalutaion, that was calculated in
     *                 calcClassificationCache() 
     *   \param kernelResultByUID vector with kernel
     *                 evaluation results by uniqueID() of
     *                 feature vector. 
     *
     *   \return decision value
     */
    /*======================================================================*/
    template< typename FV>
    double classifyWithCache( const Model<FV>& model, 
                              const std::vector<double>& kernelResultByUID) const;


    /*======================================================================*/
    /*! 
     *   compute the gradient of the kernel Result for given
     *   testObject and support vectors.
     *
     *   \param testObject   feature vector of the test object
     *   \param SVP_begin    begin of container with pointers to
     *                       support vectors
     *   \param SVP_end      end of container with pointers to support
     *                       vectors
     *   \param[out] gradientOfKernelResultByUID (output) vector with
     *                       gradient of kernel results for testObject
     *                       and each given support vector. the
     *                       std::vector container must have already
     *                       the correct size and each FVGradient must
     *                       already have the correctnumber of
     *                       components. 
     */
    /*======================================================================*/
    template< typename FV, typename ForwardIter, typename FVGradient>
    void calcGradientCache(
        const FV& testObject,
        const ForwardIter& SVP_begin,
        const ForwardIter& SVP_end,
        std::vector<FVGradient>& gradientOfKernelResultByUID) const;
    



    /*======================================================================*/
    /*! 
     *   compute the gradient of the decision function at testObject's
     *   position in the feature space for the given model. The
     *   gradientOfKernelResultByUID must have been 
     *   computed before by passing your testObject to
     *   calcGradientCache()
     *
     *   \param model  model containing Support Vectors, alphas and
     *                 rho. Each Support Vector must provide a
     *                 uniqueID() method
     *   \param gradientOfKernelResultByUID kernel gradient evaluations for
     *                 the testObject with each support vector,
     *                 computed with calcGradientCache()
     *   \param[out] gradient resulting gradient of the decision
     *                 function. the passed vector must already have
     *                 the correct number of components 
     */
    /*======================================================================*/
    template< typename FV, typename FVGradient>
    void gradientOfDecisionFunction( 
        const Model<FV>& model,
        const std::vector<FVGradient>& gradientOfKernelResultByUID,
        FVGradient& gradient) const;
    
  };
  
#include "TwoClassSVM.icc"
  
}

#endif
