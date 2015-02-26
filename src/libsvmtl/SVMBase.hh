/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 5096 $$Name$
**       $Date: 2013-12-05 16:15:41 +0100 (Thu, 05 Dec 2013) $
**   Copyright: LGPL $Author: tschmidt $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.3  2005/10/26 07:31:32  ronneber
**  - corrected some comments where "decision value" was called "alpha"
**
**  Revision 1.2  2005/03/29 18:03:41  ronneber
**  - added updateKernelCache() and clearKernelCache()
**
**  Revision 1.1  2004/09/10 11:51:50  ronneber
**  renamed class SVM to SVMBase to avoid confusion with SVM template parameters
**
**  Revision 1.2  2004/09/08 14:35:16  ronneber
**  - adapted to new ParamInfo class
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/
#ifndef SVM_HH
#define SVM_HH


#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

// std includes
#include <iostream>
#include <set>
#include <string>

// libsvmtl includes


#include "ProgressReporter.hh"
#include "Model.hh"
#include "SVMError.hh"

// requirements of template parameters
#include "svt_check/RequireLoadSaveParameters.hh"
#include "svt_check/RequireKernel_k_function.hh"
#include "svt_check/RequireStData.hh"


namespace svt
{
  /*-------------------------------------------------------------------------
   *  default values for SVM's (same as original libsvm)
   *-------------------------------------------------------------------------*/
  static const double TERMINATION_EPSILON_DEFAULT = 0.001;
  static const float  CACHE_SIZE_MB_DEFAULT       = 40;
  static const bool   SHRINKING_FLAG_DEFAULT      = true;
  

  /*======================================================================*/
  /*!
   *  \class SVMBase SVMBase.hh
   *  \brief The SVMBase class is the the base class for all SVM's 
   *
   *  it contains the parameters, that are needed for all training
   *  procedures (similar to svm_param in original libsvm)
   */
  /*======================================================================*/
  template< typename KF>
  class SVMBase
  {
    CHECK_CLASS_TEMPLATE1( svt_check::RequireLoadSaveParameters<KF>)
    

  public:

    typedef KF KF_type;
    template< typename FV>
    struct Traits
    {
      typedef svt::Model<FV> ModelType;
    };
    /*====================================================================*/
    /*! 
     *   Default Constructor
     */
    /*====================================================================*/
    SVMBase()
            : _terminationEpsilon( TERMINATION_EPSILON_DEFAULT),
              _cacheSizeMB( CACHE_SIZE_MB_DEFAULT),
              _shrinkingFlag( SHRINKING_FLAG_DEFAULT),
              _pr( 0)
          {}
    


    /*====================================================================*/
    /*! 
     *   Constructor with Kernel function. The given kernel will be
     *   copied to a private variable. You may access it later with
     *   kernel() 
     *
     *   \param kernel the kernel function object
     */
    /*====================================================================*/
    SVMBase(const KF& kernel)
            : p_kernel( kernel),
              _terminationEpsilon( TERMINATION_EPSILON_DEFAULT),
              _cacheSizeMB( CACHE_SIZE_MB_DEFAULT),
              _shrinkingFlag( SHRINKING_FLAG_DEFAULT),
              _pr( 0)
          {}


    /*====================================================================*/
    /*! 
     *   Destructor
     */
    /*====================================================================*/
    ~SVMBase()
          {   
            
          }
    

    /*====================================================================*/
    /*! 
     *   access the kernel function
     *
     *   \return reference to kernel function
     */
    /*====================================================================*/
    KF& kernel()
          {
            return p_kernel;
          }


    const KF& kernel() const
          {
            return p_kernel;
          }
    
    /*======================================================================*/
    /*! 
     *   call the updateCache() method of selected Kernel
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
            p_kernel.updateCache( fvBegin, fvEnd, accessor, _pr);
          }


    /*======================================================================*/
    /*! 
     *   call the clearCache() method of selected Kernel
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
            p_kernel.clearCache();
          }
    

    /*====================================================================*/
    /*! 
     *   standard classification without caching the kernel
     *   evaluations.
     *
     *   \param testObject  feature vector of the test object
     *   \param model       model containing Support Vectors, alphas and
     *                      rho. 
     *
     *   \return decision value
     */
    /*====================================================================*/
    template<typename FV>
    double classify( const FV& testObject, const Model<FV>& model) const;

                
    /*======================================================================*/
    /*! 
     *   set tolerance of termination criterion (default: 0.001)
     *
     *   \param e  new value for epsilon
     *
     */
    /*======================================================================*/
    void setTerminationEpsilon( double e)
          {
            _terminationEpsilon = e;
          }

    double terminationEpsilon() const
          {
            return _terminationEpsilon;
          }
    
    
    /*======================================================================*/
    /*! 
     *   set cache memory size in MB (default: 40)
     *
     *   \param s cache size in MBytes
     *
     */
    /*======================================================================*/
    void setCacheSizeMB( float s)
          {
            _cacheSizeMB = s;
          }

    float cacheSizeMB() const
          {
            return _cacheSizeMB;
          }
    
    
    /*======================================================================*/
    /*! 
     *   whether to use the shrinking heuristics (default: true)
     *
     *   \param f true = use shrinking heuristics
     *
     */
    /*======================================================================*/
    void setShrinkingFlag( bool f)
          {
            _shrinkingFlag = f;
          }

    bool shrinkingFlag() const
          {
            return _shrinkingFlag;
          }
  
    /*======================================================================*/
    /*!
     *   Load parameters from structured data object
     */
    /*======================================================================*/
    template< typename STDATA>
    void loadParameters( STDATA& stData)
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);
            
            p_kernel.loadParameters( stData);
            stData.getValue( "epsilon", _terminationEpsilon);
            stData.getValue( "cache_size", _cacheSizeMB);
            stData.getValue( "shrinking", _shrinkingFlag);
          }
    
    /*======================================================================*/
    /*!
     *   save parameters to structured data object
     */
    /*======================================================================*/
    template< typename STDATA>
    void saveParameters( STDATA& stData) const
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);

            p_kernel.saveParameters(stData);
            stData.setValue( "epsilon", terminationEpsilon());
            stData.setValue( "cache_size", cacheSizeMB());
            stData.setValue( "shrinking", shrinkingFlag());
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
                ParamInfo( "epsilon", "e", "value", 
                           "tolerance of termination criterion "
                           "(default 0.001)"));
            p.push_back(
                ParamInfo( "cache_size", "cs", "size",
                           "cache memory size in MB (default 40)"));
            p.push_back(ParamInfo( "shrinking", "sh"));
            p.back().addAlternative("0", "don't use the shrinking heuristics");
            p.back().addAlternative("1", "use the shrinking heuristics "
                                    "(default)");
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
          }


  protected:
    
    KF p_kernel;
    double _terminationEpsilon;   // epsilon as termination criterium
    float  _cacheSizeMB; // cache size in Mega-Bytes
    bool   _shrinkingFlag; // flag, wether to do shrinking during training
    ProgressReporter* _pr;
  };
    
}


#include "SVMBase.icc"

#endif
