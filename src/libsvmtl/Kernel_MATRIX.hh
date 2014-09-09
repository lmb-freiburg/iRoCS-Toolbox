/**************************************************************************
**       Title: kernel caching complete kernel matrix
**    $RCSfile$
**   $Revision: 5096 $$Name$
**       $Date: 2013-12-05 16:15:41 +0100 (Thu, 05 Dec 2013) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.7  2007/01/10 09:50:26  fehr
**  cache size bug sixed
**
**  Revision 1.6  2005/07/19 13:03:59  haasdonk
**  removed redundant cout-messages, erroneous filename and added a new function
**  for computing a training-kernel matrix in BasicSVMAdapter*
**
**  Revision 1.5  2005/06/06 21:23:31  haasdonk
**  added updateCache() with two FV-lists, required for classification with precomputed kernel-matrices
**
**  Revision 1.4  2005/03/29 17:55:35  ronneber
**  - renamed freeCache () to clearCache()
**  - clearCache() now also calls clearCache() of underlying Kernel. This
**    should fix problems with training/classification using Kernel_MATRIX
**    together with Kernel_SCALE
**
**  Revision 1.3  2004/09/08 14:29:13  ronneber
**  - adapted to new ParamInfo class
**
**  Revision 1.2  2004/09/03 07:10:32  ronneber
**  - updateCache now takes an Accessor
**  - updateCache now calls updateCache of underlying kernel
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef KERNEL_MATRIX_HH
#define KERNEL_MATRIX_HH

// std includes
#include <map>
#include <set>
#include <string>

// libsvmtl includes
#include "svm_defines.hh"
#include "SVMError.hh"
#include "ProgressReporter.hh"

// requirements of template parameters
#include "svt_check/RequireStData.hh"
#include "svt_check/RequireLoadSaveParameters.hh"
#include "svt_check/RequireNameDescriptionParamInfos.hh"
#include "svt_check/RequireFeatureVectorUniqueID.hh"

namespace svt
{
  template< typename KERNEL>
  class Kernel_MATRIX
  {
    CHECK_CLASS_TEMPLATE1( svt_check::RequireLoadSaveParameters<KERNEL>)
    CHECK_CLASS_TEMPLATE2( svt_check::RequireNameDescriptionParamInfos<KERNEL>)
    
  public:
    Kernel_MATRIX()
            : _data(0),
              _rowStarts(0),
              _width(0),
              _cacheIsUpToDate( false)
          {}
    
    Kernel_MATRIX( const KERNEL& kernel)
            : _kernel(kernel),
              _data(0),
              _rowStarts(0),
              _width(0),
              _cacheIsUpToDate( false)
          {}

/*-------------------------------------------------------------------------
 *  if you really want to copy Kernel_MATRIX's, which might be very
 *  expensive, you can define  ALLOW_KERNEL_MATRIX_COPY
 *-------------------------------------------------------------------------*/
#ifdef ALLOW_KERNEL_MATRIX_COPY
  public:
#else
  private:
#endif

    Kernel_MATRIX( const Kernel_MATRIX<KERNEL>& orig)
            : _data(0),
              _rowStarts(0),
              _width(0)
          {
            operator=( orig);
          }
    
    void operator=( const Kernel_MATRIX<KERNEL>& orig)
          {
            _cacheIsUpToDate = orig._cacheIsUpToDate;
            resizeCache( orig._width);
            if( orig._width != 0)
            {
              unsigned int size = _width*_width;
              std::copy( orig._data, orig._data + size, _data);
            }
            
                        
          }

  public:
    

    ~Kernel_MATRIX()
          {
            clearCache();
          };

    void resizeCache( long width) const
          {
            if( width == _width) return;
            
            clearCache();
            
            _width = width;
            if( _width != 0)
            {
              _data = new double[width*width];
              _rowStarts = new double*[width];
             
              for(long i = 0; i < width; ++i)
              {
                _rowStarts[i] = _data + i*width;
              }
            }
            else
            {
              _data = 0;
              _rowStarts = 0;
            }
            
          }
    
    void clearCache() const
          {
            if( _data != 0)
            {
              delete[] _data;
              _data = 0;
            }
            
            if( _rowStarts != 0)
            {
              delete[] _rowStarts;
              _rowStarts = 0;
            }
            _width = 0;
            _cacheIsUpToDate = false;
            
            /*--------------------------------------------------------------
             *  clear cache of underlying kernel (e.g. if it is a
             *  Kernel_SCALE)
             *--------------------------------------------------------------*/
            _kernel.clearCache();
          }
    

        
    template< typename ForwardIter, typename Accessor>
    void updateCache( const ForwardIter& fvBegin,  
                      const ForwardIter& fvEnd,
                      Accessor accessor,
                      ProgressReporter* pr = 0) const
          {
            /*--------------------------------------------------------------
             *  update cache of underlying kernel (e.g. if it is a
             *  Kernel_SCALE  )
             *--------------------------------------------------------------*/
            _kernel.updateCache( fvBegin, fvEnd, accessor, pr);
            

            /*---------------------------------------------------------------
             *  search max uid
             *---------------------------------------------------------------*/
            unsigned int maxUID = 0;
            
            for( ForwardIter p = fvBegin; p != fvEnd; ++p)
            {
              if( accessor(p).uniqueID() > maxUID)
              {
                maxUID = accessor(p).uniqueID();
              }
            }
            SVM_ASSERT( maxUID <= MAX_BELIEVABLE_UNIQUE_ID);
            
            /*--------------------------------------------------------------
             *  fill matrix with kernel results
             *--------------------------------------------------------------*/
            resizeCache( maxUID+1);
            size_t nFeatureVectors = fvEnd - fvBegin;
            ForwardIter p = fvBegin;
            int nKernelEvaluations =
                static_cast<int>(nFeatureVectors * (nFeatureVectors + 1) / 2);
            int reportProgressEveryNthEval = nKernelEvaluations / 100;  
            if( reportProgressEveryNthEval == 0) reportProgressEveryNthEval = 1;
            int nKernelEvaluationsFinished = 0;
            int silentProgressCounter = reportProgressEveryNthEval;
            
            for( unsigned int i = 0; i < nFeatureVectors; ++i, ++p)
            {
              unsigned int row = accessor(p).uniqueID();
              ForwardIter q = fvBegin + i; 
              for( unsigned int j = i; j < nFeatureVectors; ++j, ++q)
              {
                unsigned int col = accessor(q).uniqueID();
		double val = _kernel.k_function( accessor(p), accessor(q));
                _rowStarts[row][col] = val;
                _rowStarts[col][row] = val;
                ++nKernelEvaluationsFinished;
                --silentProgressCounter;
                if( silentProgressCounter <= 0)
                {
                  silentProgressCounter = reportProgressEveryNthEval;
                  if( pr != 0)
                  {
                    pr->reportProgress(
                        TASK_LEVEL_CROSS_VAL, 
                        "calculating full kernel matrix",
                        static_cast<float>(nKernelEvaluationsFinished) /
                        static_cast<float>(nKernelEvaluations), "");
                  }
                }
                                
              }
            }
            if( pr != 0)
            {
              pr->reportProgress( TASK_LEVEL_CROSS_VAL,
                                  "full kernel matrix calculated", 1.0,
                                  "");
            }
            _cacheIsUpToDate = true;
          }
    

    // new updateCache-Syntax:    
    template< typename ForwardIter1, typename Accessor1,
              typename ForwardIter2, typename Accessor2 >
    void updateCache( const ForwardIter1&,  
                      const ForwardIter1&,
                      Accessor1,
                      const ForwardIter2&,  
                      const ForwardIter2&,
                      Accessor2,
                      ProgressReporter* = 0) const
          {
            // nothing to be done or can be implemented later
          }

    template< typename FV>
    double k_function( const FV& x, const FV& y) const
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireFeatureVectorUniqueID<FV>);
            if( _cacheIsUpToDate == false)
            {
              return _kernel.k_function( x, y);
            }

            long row = static_cast<long>(x.uniqueID());
            long col = static_cast<long>(y.uniqueID());
            
            SVM_ASSERT( row < _width);
            SVM_ASSERT( col < _width);
            
            return _rowStarts[row][col];
          }

    template<typename STDATA>
    void loadParameters( STDATA& stData)
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);
            _kernel.loadParameters( stData);
          }
    
    template<typename STDATA>
    void saveParameters( STDATA& stData) const
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);
            _kernel.saveParameters( stData);
            stData.setValue( "kernel_type", name());
          }

    static std::string name()
          {
            return std::string( "kmatrix_") + KERNEL::name();
          }

    static std::string description()
          {
            return std::string( "cached kernel matrix for ") + KERNEL::description();
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
            KERNEL::getParamInfos( p);
          }
    
  private:
    KERNEL       _kernel;
    mutable double*      _data;
    mutable double**     _rowStarts;
    mutable long	 _width;
    mutable bool         _cacheIsUpToDate;

    
    
  };

}

#endif
